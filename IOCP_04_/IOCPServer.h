#pragma once
#pragma comment(lib,"ws2_32")

#include "ClientInfo.h"
#include "Define.h"
#include <thread>
#include <vector>

class IOCPServer
{
public:

	IOCPServer(void){}

	virtual ~IOCPServer(void)
	{

	}

	// 소켓을 초기화 하는 함수
	bool InitSocket()
	{

	}

	virtual void OnReceive(const UINT32 clientIndex_,const UINT32 size_,char* pData ){}

private:
	void CreateClient(const UINT32 maxClientCount )
	{
		for (UINT32	i =0 ; i < maxClientCount ; ++i )
		{

		}
	}


	void WorkerThread()
	{

		LPOVERLAPPED lpOverlapped = NULL;

		while (mIsWorkerRun)
		{


			// Overlapped I/O Recv 작업결과 뒤처리
			if ()
			{
				// 파생클래스 EchoServer 의 OnReceive 가 실행된다
				OnReceive();

			}

		}

	}


	// 클라이언트 정보 저장 구조체
	std::vector<stClientInfo> mClientInfos;


	// IO Worker 스레드
	std::vector<std::thread> mIOWorkerThreads;

	// Accept 스레드
	std::thread mAccepterThread;

	// CompletionPort 객체핸들
	HANDLE mIOCPHandle = INVALID_HANDLE_VALUE;

	// 작업 쓰레드 동작 플래그
	bool mIsWorkerRun = true;

	// 접속 쓰레드 동작 플래그
	bool mIsAccepterRun = true;

};

