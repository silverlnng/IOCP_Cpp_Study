#pragma once
#pragma comment(lib,"ws2_32")
#pragma comment(lib,"mswsock.lib")

#include "ClientInfo.h"
#include <vector>
#include <thread>
#include <winsock2.h>

class IOCPServer
{
public:

	IOCPServer(void){}

	virtual ~IOCPServer(void)
	{
		// 윈속의 사용을 끝내기
		WSACleanup();
	}

	//  소켓 초기화
	bool Init(const UINT32 maxIOWorkerThreadCount_)
	{
		WSADATA wsaData;

		// 
		int nResult = WSAStartup(MAKEWORD(2,2),&wsaData);
		if (nResult != 0)
		{
			printf("[에러] WSAStartup() 함수 실패 : %d\n", WSAGetLastError());

			return false;
		}


		// 연결지향형 TCP ,Overlapped IO 소켓을생성
		mListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

		if (mListenSocket == INVALID_SOCKET)
		{
			printf("[에러] WSASocket () 함수 실패 : %d\n", WSAGetLastError());
			return false;
		}

		MaxIOWorkerThreadCount = maxIOWorkerThreadCount_;

		printf("소켓 초기화 성공\n");

		return true;
	}

	// 서버의 주소정버와 소켓과 연결시키고 접속 요청을 받기위해 소켓을 등록 
	bool BindAndListen(int bindPort_)
	{
		SOCKADDR_IN stServerAddr;

		// 주소패밀리 : AF_INET => IPv4 사용
		stServerAddr.sin_family = AF_INET;

		stServerAddr.sin_port = htons(bindPort_);

		stServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

		// 위에서 지정한 서버 주소 정보와 리슨소켓을 연결
		int nResult = bind(mListenSocket,(SOCKADDR*)& stServerAddr, sizeof(SOCKADDR_IN));
		if (0 != nResult)
		{
			printf("[에러] bind() 함수 실패 : %d \n",WSAGetLastError());
			return false;
		}

		// 접속요청을 받기위해서 리슨소켓을 Listen State 으로 전환. 
		// 리슨소켓의 백로그(접속대기큐) 를 5개로 설정
		nResult = listen(mListenSocket, 5);
		if (nResult != 0)
		{
			printf("[에러] listen() 함수 실패 :%d\n", WSAGetLastError());
			return false;
		}

		// CompletionPort 객체를 생성 요청
		mIOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, MaxIOWorkerThreadCount);

		if (mIOCPHandle == NULL)
		{
			printf("[에러] CreateIoCompletionPort() 함수 실패 :%d\n ", GetLastError());
			return false;
		}

		auto hIOCPHandle = CreateIoCompletionPort((HANDLE)mListenSocket, mIOCPHandle, (UINT32)0, 0);

		if (hIOCPHandle == NULL)
		{
			printf("[에러] 리슨소켓과  IOCP 바인드 실패 :%d\n ", GetLastError());
			return false;
		}

		printf("서버등록 성공 !! \n");

		return true;
	}

	// 접속 요청을 수락하고 메세지를 받아서 처리하는 함수
	bool StartServer(const UINT32 maxClientCount_)
	{
		// 접속된 클라이언트 주소 정보를 저장할 구조체 최대인원수만큼 미리 만들기
		CreateClient(maxClientCount_);

		// 워커스레드 만들기
		bool bResult = CreateWorkerThread();
		if (bResult == false)
		{
			return false;
		}

		// Accepter 스레드 만들기
		bResult = CreateAccepterThread();
		if (bResult == false)
		{
			return false;
		}

		printf("서버시작\n");

		return true;
	}

	// 생성되어있는 쓰레드를 파괴
	void DestroyThread()
	{
		mIsWorkerRun = false;
		CloseHandle(mIOCPHandle);

		for (auto& workerThread : mIOWorkerThreads)
		{
			if (workerThread.joinable())
			{
				workerThread.join();
			}
		}

		mIsAccepterRun = false;
		closesocket(mListenSocket);

		if (mAccepterThread.joinable())
		{
			mAccepterThread.join();
		}

	}

	bool SendMsg(const UINT32 clientIndex_ , const UINT32 dataSize_ ,char* pData)
	{
		auto pClient = GetClientInfo(clientIndex_);
		return pClient->SendMsg(dataSize_, pData);
	}

	virtual void OnConnect(const UINT32 clientIndex_){}

	virtual void OnClose(const UINT32 clientIndex_){}

	virtual void OnReceive(const UINT32 clientIndex_,const UINT32 size_ , char* pData_ ){}


private:

	void CreateClient(const UINT32 maxClientCount_)
	{
		for (UINT32 i=0; i<maxClientCount_; ++i)
		{
			auto client = new stClientInfo;
			client->Init(i, mIOCPHandle);

			mClientInfos.push_back(client);
		}
	}

	// 생성되면 Waiting Thread Queue 에 대기할 쓰레드들 생성
	bool CreateWorkerThread()
	{
		for (UINT32 i = 0; i < MaxIOWorkerThreadCount; i++)
		{
			mIOWorkerThreads.emplace_back([this]() {
				WorkerThread();});
		}
		printf("WorkerThread 시작 \n");
		return true;
	}

	// 사용하지 않는 클라이언트 정보 구조체를 반환
	stClientInfo* GetEmptyClient()
	{
		for (auto& client : mClientInfos)
		{
			if (client->IsConnected() == false)
			{
				return client;
			}
		}

		return nullptr;
	}

	stClientInfo* GetClientInfo(const UINT32 clientIndex_)
	{
		return mClientInfos[clientIndex_];
	}

	// accept 요청을 처리하는 쓰레드 생성
	bool CreateAccepterThread()
	{
		mAccepterThread = std::thread([this]() {AccepterThread(); });

		printf("AccepterThread 시작! \n");
		return true;
	}

	// Overlapped IO 작업에 대한 완료통보를 받았을때 그에해당하는 처리를 하는 함수
	void WorkerThread()
	{

		// Completion Key 를 받을 포인터 변수
		stClientInfo* pClientInfo = nullptr;

		// 함수 호출 성공 여부
		BOOL bSuccess = TRUE;
	
		// Overlapped IO 작업에서 전송된 데이터 크기
		DWORD dwIoSize = 0;

		// IO 작업을 위해 요청한 Overlapped 구조체를 받을 포인터
		LPOVERLAPPED lpOverlapped = NULL;

		while (mIsWorkerRun)
		{
			bSuccess = GetQueuedCompletionStatus(
				mIOCPHandle,
				&dwIoSize,
				(PULONG_PTR)&pClientInfo, 
				&lpOverlapped,
				INFINITE);

			// 워커쓰레드 종료 ******************
			if (bSuccess==TRUE && dwIoSize==0 && lpOverlapped == NULL)
			{
				mIsWorkerRun = false;
				continue;
			}

			if (lpOverlapped == NULL)
			{
				continue;
			}

			auto pOverlappedEx =(stOverlappedEx*)lpOverlapped;
			
			// 클라이언트가 접속을 끊었을때
			if (bSuccess == FALSE || (dwIoSize==0 && pOverlappedEx->m_eOperation !=IOOperation::ACCEPT))
			{
				CloseSocket(pClientInfo);

				continue;
			}

			//Overlapped I/O ACCEPT작업 결과 뒤 처리
			if (pOverlappedEx->m_eOperation == IOOperation::ACCEPT )
			{
				pClientInfo = GetClientInfo(pOverlappedEx->SessionIndex);

				if (pClientInfo->AcceptCompletion())
				{
					// 클라이언트 수 증가 
					++mClientCnt;

					OnConnect(pClientInfo->GetIndex());
				}
				else
				{
					CloseSocket(pClientInfo, true);
				}
			}

			//Overlapped I/O Recv작업 결과 뒤 처리
			else if (pOverlappedEx->m_eOperation == IOOperation::RECV)
			{
				OnReceive(pClientInfo->GetIndex(), dwIoSize, pClientInfo->RecvBuffer());

				// 다시 받을수있도록 예약 걸어두기
				pClientInfo->BindRecv();
			}
			//Overlapped I/O SEND작업 결과 뒤 처리
			else if (pOverlappedEx->m_eOperation == IOOperation::SEND)
			{
				pClientInfo->SendCompleted(dwIoSize);

			}
			else
			{
				// 예외 상황
				printf("Client Index(%d) 에서 예외 상황\n", pClientInfo->GetIndex());
			}

		}

	}

	void AccepterThread()
	{
		while (mIsAccepterRun)
		{
			// 현재시간을 초(second) 단위의 정수로 변환 
			auto curTimeSec = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

			for (auto client : mClientInfos)
			{
				// 클라이언트가 이미 연결되어있으면 바로 다음 반복문으로 
				if (client->IsConnected())
				{
					continue;
				}

				
				if ((UINT64)curTimeSec < client->GetLatestClosedTimeSec())
				{
					continue;
				}

				// 재사용 대기시간 체크 ( 중요 !! )
					// 어떤 클라이언트가 접속을 끊자마자 해당 메모리 공간(객체)을 즉시 다른 사람에게 할당하면, 네트워크 상에서 지연 도착한 이전 사람의 패킷이 새 사람의 패킷으로 오인될 수 있는 위험이 있습니다 (TCP의 Time-Wait 문제와 유사).

				auto diff = curTimeSec - client->GetLatestClosedTimeSec();

				if (diff <= RE_USE_SESSION_WAIT_TIMESEC)
				{
					continue;
				}

				// 리슨 소켓과 현재 시간을 전달
				client->PostAccept(mListenSocket, curTimeSec);
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(32));

			// 1초/32 만큼 쉬다가 다시 확인
			// => 1초에 32 번 확인하게 됨
		}
	}

	void CloseSocket(stClientInfo* clientInfo_,bool isForce_ = false)
	{
		if (clientInfo_->IsConnected() == false)
		{
			return;
		}

		auto clientIndex = clientInfo_->GetIndex();

		clientInfo_->Close(isForce_);

		OnClose(clientIndex);
	}


	UINT32 MaxIOWorkerThreadCount = 0;

	// 클라이언트 정보 저장 구조체
	std::vector<stClientInfo*> mClientInfos;

	// 클라이언트 접속을 받기위한 리슨 소켓
	SOCKET mListenSocket = INVALID_SOCKET;

	// 접속되어 있는 클라이언트 수
	int		mClientCnt = 0;

	// IO Worker 스레드
	std::vector<std::thread> mIOWorkerThreads;

	// Accept 스레드
	std::thread				mAccepterThread;

	// CompletionPort 객체 핸들
	HANDLE mIOCPHandle = INVALID_HANDLE_VALUE;

	// 작업 쓰레드 동작 플래드
	bool	mIsWorkerRun = true;

	// 접속 쓰레드 동작 플래드
	bool	mIsAccepterRun = true;
};

