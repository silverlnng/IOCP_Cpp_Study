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
	bool Init()
	{
		WSASocket
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

	// 클라이언트 정보 저장 구조체
	//std::vector<stClientIn>

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
	bool	mIsAcceptRun = true;
};

