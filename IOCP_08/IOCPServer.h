#pragma once
#pragma comment(lib,"mswsock.lib")

#include "ClientInfo.h"
#include <vector>
#include <thread>
#include <winsock2.h>

class IOCPServer
{
public:

	//  소켓 초기화
	bool Init(const UINT32 maxIOWorkerThreadCount_)
	{
		WSADATA wsaData;

		// 
		int nResult = WSAStartup();

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
		int nRet = bind(mListenSocket,(SOCKADDR*)& stServerAddr, sizeof(SOCKADDR_IN));

		// 접속요청을 받기위해서 리슨소켓을 Listen State 으로 전환. 
		 

		// CompletionPort 객체를 생성 요청

	}

private:

	void CreateClient()
	{

	}

	bool CreateWorkerThread()
	{

	}

	bool CreateAccepterThread()
	{

	}

	void WorkerThread()
	{

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

	void CloseSocket()
	{

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

