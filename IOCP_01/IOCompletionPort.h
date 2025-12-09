#pragma once
#pragma comment(lib,"ws2_32")
// #pragma comment 의미 : 
// winsock2.h
#include <winsock2.h>

#include <thread>
#include <vector>

#define MAX_SOCKBUF 1024 // 패킷크기 
#define MAX_WORKERTHREAD 4 // 쓰레드 풀에 넣을 쓰레드 수

enum class IOOperation
{
	RECV,
	SEND
};

// WSAOVERLAPPED구조체 : winsock2.h 
// WSAOVERLAPPED구조체를 확장 시켜서 필요한 정보를 더 넣었다.
struct stOverlappedEx
{
	WSAOVERLAPPED	m_wsaOverlapped;		// overlapped I/O 구조체
	SOCKET			m_socketClient;			// 클라이언트 소켓
	WSABUF			m_wsaBuf;				// overlapped I/O 작업 버퍼
	char			m_szBuf[MAX_SOCKBUF];	// 데이터 버퍼
	IOOperation		m_eOperation;			// 작업동작 종류
};

//클라이언트 정보를 담기위한 구조체
struct stClientInfo
{
	SOCKET					m_socketClient;			// 클라이언트와 연결되는 소켓
	stOverlappedEx			m_stRecvOverlappedEx;	// Recv overlapped I/O 작업을 위한 변수
	stOverlappedEx			m_stSendOverlappedEx;	// Send overlapped I/O 작업을 위한 변수

	stClientInfo()
	{
		ZeroMemory(&m_stRecvOverlappedEx, sizeof(stOverlappedEx));
		ZeroMemory(&m_stSendOverlappedEx, sizeof(stOverlappedEx));
		m_socketClient = INVALID_SOCKET;
	}

};

class IOCompletionPort
{
public:

	IOCompletionPort(void) {}

	~IOCompletionPort(void)
	{
		// 윈속의 사용을 끝내기 
		WSACleanup();
	}

	// 소켓을 초기화 하는 함수
	bool InitSocket()
	{
		WSADATA wsaData;
		int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);

		if (0 != nRet)
		{
			printf("[에러] WSAStartup 함수 실패 : %d\n", WSAGetLastError());
			return false;
		}

		// 연결지향형 TCP , Overlapped I/O 소켓을 생성
		mListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

		if (mListenSocket == INVALID_SOCKET)
		{
			printf("에러 socket()함수 실패 : %d\n", WSAGetLastError());
			return false;
		}

		printf("소켓초기화\n");
		return true;
	}

	//======================== 서버용 함수 ============================= 
		// 서버의 주소정보를 소켓과 연결시키고 접속요청을 받아야함
		// 소켓을 등록하는 함수
	bool BindandListen(int nBindPort)
	{
		// (1) 주소설정
		SOCKADDR_IN		stServerAddr;
		stServerAddr.sin_family = AF_INET;
		// AF_INET : 주소체계 설정
		// sin_family : 전송주소의 주소 패밀리.

		stServerAddr.sin_port = htons(nBindPort);
		stServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);


		// (2) bind:지정한 서버 주소 정보와 cIOCompletionPort 소켓을 연결
		int nRet = bind(mListenSocket, (sockaddr*)& stServerAddr, sizeof(SOCKADDR_IN));

		// mListenSocket	: 클라이언트의 접속을 받기위해 WSASocket() 함수로 만든 리슨 소켓
		// (sockaddr*)&stServerAddr :  구조체 변수 주소(SOCKADDR_IN*) 을 sockaddr* 으로 형변환

		if (0 != nRet)
		{
			printf("에러 bind 험수 실패 : %d\n", WSAGetLastError());
			return false;
		}


		// (3) listen 함수 :접속 요청을 받아들이기위해 cIOCompletion 소켓을 등록하고 
		// 접속대기큐를 5개 설정 (백로그)=>Accept 큐만 설정. Syn큐 설정은 알아서되있음

		nRet = listen(mListenSocket, 5);
		if (0 != nRet)
		{
			printf("에러 listen() 함수 실패 : %d \n", WSAGetLastError());
			return false;
		}

		printf("서버 등록 성공 \n");
		return true;
	}

	// 접속요청을 수락하고 메세지를 받아서 처리하는 함수
	bool StartServer(const UINT32 maxClientCount)
	{
		CreateClient(maxClientCount);

		// ComletionPort 객체 핸들 생성
		mIOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, MAX_WORKERTHREAD);

		if (NULL == mIOCPHandle)
		{
			printf("[에러] CreateIoCompeltionPort() 함수 실패 : %d \n", GetLastError());
			return false;
		}

		//
		bool bRet = CreateWorkerThread();
		if (false == bRet)
		{
			return false;
		}

		bRet = CreateAccepterThread();
		if (false == bRet)
		{
			return false;
		}

		printf("서버시작\n");
		return true;
	}

	// 생성되어있는 쓰레드를 파괴한다.
	void DestroyThread()
	{
		mIsWorkerRun = false;

		CloseHandle(mIOCPHandle);



		// Accepter 쓰레드를 종료한다
	}



private:

	void CreateClient(const UINT32 maxClientCount)
	{
		for (UINT32 i = 0;i<maxClientCount;++i)
		{
			mClientInfos.emplace_back();
			// emplace_back() :push_back() 함수와 달리 임시객체 생성안함
		}
	}

	// WaitingThread Queue 에서 대기할 쓰레드들 생성
	bool CreateWorkerThread()
	{
		unsigned int uiThreadId = 0;
		// waitingThread Queue 에 대기상태로 넣을 쓰레드들 생성 권장되는 갯수 :
		// (cpu*2) +1

		for (int i=0;i<MAX_WORKERTHREAD;i++)
		{
			mIOWorkerThreads.emplace_back(
				[this]() {WorkerThread(); }
			);

			// 쓰레드를 생성하고 쓰레드가 바로 WorkerThread() 함수를 실행하게 한다
		}
		printf("workthread 시작\n");
		return true;
	}

	// accept 요청을 처리하는 쓰레드 생성
	bool CreateAccepterThread()
	{
		mAccepterThread = std::thread([this]() {AccepterThread(); });

		printf("AccepterThread 시작! \n");
		return true;
	}


	// 사용하지 않는 클라이언트 정보 구조체를 반환한다.
	stClientInfo* GetEmptyClientInfo()
	{
		for (auto& client : mClientInfos)
		{
			if (INVALID_SOCKET == client.m_socketClient)
			{
				return &client;
			}
		}
		return nullptr;
	}

	// CompletionPort 객체와 소켓과 CompetionKey를 연결시키는 역할을 한다
	bool BindIOCompletionPort(stClientInfo* pClientInfo)
	{

		// socket 과 pClientInfo를 CompletionPort 객체와 연결시킨다.
		auto hIOCP=CreateIoCompletionPort((HANDLE)pClientInfo->m_socketClient,
											mIOCPHandle,
											(ULONG_PTR)pClientInfo, 0);

		if (hIOCP == NULL || mIOCPHandle != hIOCP)
		{
			printf("[에러] CreateIoCompletionPort() 함수 실패 :%d \n", GetLastError());

			return false;
		}
		return true;
	}

	bool BindRecv(stClientInfo* pClientInfo)
	{
		DWORD dwFlag = 0;
		DWORD dwRecvNumBytes = 0;

		// Overlapped I/O 을 위해 각 정보를 세팅
		pClientInfo->m_stRecvOverlappedEx.m_wsaBuf.len = MAX_SOCKBUF;

		// WSABUF 포인터가 실제 저장되는 m_szBuf 를 가리키도록 세팅
		pClientInfo->m_stRecvOverlappedEx.m_wsaBuf.buf = pClientInfo->m_stRecvOverlappedEx.m_szBuf;

		// 작업종류를 Recv 으로 세팅 
		pClientInfo->m_stRecvOverlappedEx.m_eOperation = IOOperation::RECV;

		int nRet = WSARecv(pClientInfo->m_socketClient,
			&(pClientInfo->m_stRecvOverlappedEx.m_wsaBuf),
			1,
			&dwRecvNumBytes,
			&dwFlag,
			(LPWSAOVERLAPPED) & (pClientInfo->m_stRecvOverlappedEx),
			NULL);

		// socket error 이면 client socket 이 끊어진걸로 처리
		if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
		{
			printf("[에러] WSARecv() 함수 실패 : %d \n", WSAGetLastError());
			return false;
		}

		return true;
	}

	// WSASend Overlapped I/O 작업을 시킨다
	bool SendMsg(stClientInfo* pClientInfo,char* pMsg ,int nLen)
	{
		DWORD dwRecvNumBytes = 0;

		// 전송될 메시지를 복사
		// 실제로 메시지가 저장되어있는곳은 m_stRecvOverlappedEx.m_szBuf
		CopyMemory(pClientInfo->m_stRecvOverlappedEx.m_szBuf, pMsg, nLen);

		// Overlapped I/O 를 위해 각 정보를 세팅
		pClientInfo->m_stSendOverlappedEx.m_wsaBuf.len = nLen;
		
		pClientInfo->m_stSendOverlappedEx.m_wsaBuf.buf = pClientInfo->m_stSendOverlappedEx.m_szBuf;

		pClientInfo->m_stSendOverlappedEx.m_eOperation = IOOperation::SEND;


		int nRet = WSASend(pClientInfo->m_socketClient,
			&(pClientInfo->m_stSendOverlappedEx.m_wsaBuf),
			1,
			&dwRecvNumBytes,
			0,
			(LPWSAOVERLAPPED) & (pClientInfo->m_stSendOverlappedEx),
			NULL);

		// socket_error 이면 client socket 이 끊어진것을 처리
		if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
		{
			printf("[에러] WSASend() 함수 실패 : %d \n ", WSAGetLastError());
			return false;
		}
		return true;
	}

	// Overlapped I/O 작업에 대한 완료통보를 받아 그에 대한 해당처리를 하는 함수
	void WorkerThread()
	{
		// Completion Key 를 받을 포인터 변수
		stClientInfo* pClientInfo = NULL;

		// 함수 호출 성공 여부
		bool bSuccess = TRUE;

		// Overlapped I/O 작업에서 전송된 데이터 크기
		DWORD dwIOSize = 0;

		// I/O 작업을 위해 요청한 Overlapped 구조체를 받을 포인터
		LPOVERLAPPED lpOverlapped = NULL;

		while (mIsWorkerRun)
		{
			// GetQueuedCompletionStatus : 이걸 보고 운영체제가 이 쓰레드가 IOCP 를 기다리는 걸 알게되어서 쓰레드를 Running->Blocked 대기상태로 변경. 그리고 IOCP 커널객체의 WaitingThread Queue 에 대기상태로 들어가게 된다, 
			// 
			// 그래서(blocked 상태가 되었으니까) while 문도 계속해서 도는게 아님.
			bSuccess = GetQueuedCompletionStatus(mIOCPHandle,
				&dwIOSize,						// 실제로 전송된 바이트
				(PULONG_PTR) & pClientInfo,		// CompletionKey
				&lpOverlapped,					// Ovelapped IO 객체
				INFINITE);						//대기할시간


			// 쓰레드가 깨워진 후 실행하는 함수
		
			// 사용자 쓰레드 종료 메세지 처리
			if (TRUE == bSuccess && 0 == dwIOSize && NULL == lpOverlapped)
			{
				mIsWorkerRun = false;
				continue;
			}

			if (NULL == lpOverlapped)
			{
				continue;
			}


			// client 가 접속을 끊었을때 
			// 
			if (FALSE == bSuccess ||(0==dwIOSize && TRUE ==bSuccess))
			{
				printf("socket(%d) 접속 끊김\n", (int)pClientInfo->m_socketClient);
				CloseSocket(pClientInfo);
				continue;
			}


			stOverlappedEx* pOverlappedEx = (stOverlappedEx*)lpOverlapped;

			// Overlapped I/O Recv 작업 결과 뒤 처리 
			if (pOverlappedEx->m_eOperation == IOOperation::RECV )
			{
				pOverlappedEx->m_szBuf[dwIOSize] = NULL;

				// 클라이언트 메시지를 에코하기 
				SendMsg(pClientInfo, pOverlappedEx->m_szBuf, dwIOSize);

				// 다시 BindRecv 를 걸어서 계속해서 비동기 작업을 걸어두기
				BindRecv(pClientInfo);
			}

			// Overlapped I/O Send 작업 결과 뒤 처리
			else if (pOverlappedEx->m_eOperation == IOOperation::SEND)
			{

				printf("[송신] bytes : %d , msg : %s \n ", dwIOSize, pOverlappedEx->m_szBuf);
			}
			// 예외 상황
			else
			{
				printf("socket(%d) 에서 예외상황 \n", (int)pClientInfo->m_socketClient);
			}
		}
	}


	// 사용자의 접속을 받는 쓰레드
	void AccepterThread()
	{
		SOCKADDR_IN		stClientAddr;
		int nAddrLen = sizeof(SOCKADDR_IN);

		while (mIsAccepterRun)
		{
			// 접속을 받을 구조체의 인덱스를 얻어온다
			stClientInfo* pClientInfo = GetEmptyClientInfo();

			if (NULL == pClientInfo)
			{
				printf("[에러] Client Full \n");
				return;
			}

			// 클라이언트 접속 요청이 들어올때까지 기다린다
			// accept : 블로킹 함수로 , 클라이언트가 접속을 시도하기 전 까지 이 함수안에서 쓰레드는 커널에 의해 Blocked .
			// While 문이 계속 작동하는게 아님

			pClientInfo->m_socketClient = accept(mListenSocket, (SOCKADDR*)&stClientAddr, &nAddrLen);
			if (INVALID_SOCKET == pClientInfo->m_socketClient)
			{
				continue;
			}

			// I/O Completion Port 객체와 소켓을 연결한다

			// Recv Overlapped I/O 작업을 요청해 놓는다



		}

	}

	// 소켓의 연결을 종료시킨다
	void CloseSocket(stClientInfo* pClientInfo ,bool bIsForce=false)
	{

		// 소켓의 데이터 송수신을 모두 중단시킨다

		// 소켓 옵션을 설정한다

		// 소켓 연결을 종료시킨다
		closesocket(pClientInfo->m_socketClient);

	}


	//클라이언트 정보 저장 구조체
	std::vector<stClientInfo> mClientInfos;

	// 클라이언트의 접속을 받기위한 리슨소켓
	SOCKET mListenSocket = INVALID_SOCKET;

	// 접속되어있는 클라이언트 수
	int mClientCnt = 0;

	// IO Worker 스레드
	std::vector<std::thread> mIOWorkerThreads;

	// Accept 스레드
	std::thread mAccepterThread;

	// CompletionPort 객체 핸들
	HANDLE mIOCPHandle = INVALID_HANDLE_VALUE;

	// 작업쓰레드 동작 플래그
	bool mIsWorkerRun = true;

	// 접속 쓰레드 동작 플래그
	bool mIsAccepterRun = true;

	// 소켓 버퍼
	char mSocketBuf[1024] = { 0, };

};