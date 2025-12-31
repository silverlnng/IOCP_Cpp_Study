#pragma once

#include "Define.h"
#include <mutex>
#include <queue>

// 클라이언트 정보를 담기 위한 구조체
class stClientInfo
{
public:

	stClientInfo()
	{
		ZeroMemory(&mRecvOverlappedEx, sizeof(stOverlappedEx));
		mSocket = INVALID_SOCKET;
	}

	void Init(const UINT32 index , HANDLE iocpHandle_)
	{
		mIndex = index;
		mIOCpHandle = iocpHandle_;
	}

	UINT32 GetIndex() { return mIndex; }

	bool IsConnected() { return mIsConnect == 1; }

	SOCKET GetSock() { return mSocket; }

	UINT64 GetLatestClosedTimeSec() { return mLatestClosedTimeSec; }

	char* RecvBuffer() { return mRecvBuf; }

	bool OnConnect(HANDLE iocpHandle_,SOCKET socket_)
	{
		mSocket = socket_;
		mIsConnect = 1;

		// IOCP 객체와 소켓을 연결
		if (BindIOCompletionPort(iocpHandle_) == false)
		{
			return false;
		}
		return BindRecv();
	}

	void Close(bool bIsForce = false)
	{
		struct linger stLinger = { 0,0 };
		

		// bIsForce 가 true 이면 강제종료시키기 (주의 : 데이터 손실일어날수있음)
		if (bIsForce == true)
		{
			// {1,0} 으로 설정.
			// 옵션을 키고 , 기다리는 시간을 0 으로
			stLinger.l_onoff = 1;
		}

		// 소켓의 데이터 송수신을 모두 중단시킨다
		shutdown(mSocket, SD_BOTH);

		// 소켓 옵션을 설정
		setsockopt(mSocket, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));

		mIsConnect = 0;
		mLatestClosedTimeSec = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

		// 소켓연결을 종료시킨다
		closesocket(mSocket);
		mSocket = INVALID_SOCKET;
	}

	bool PostAccept(SOCKET listenSock_,const UINT64 curTimeSec_)
	{
		printf_s("PostAccept. client Index : %d \n", GetIndex());
		// 미리 접속할 빈소켓을 만들고 운영체제에 예약해두는 역할

		//  닫힌 시간을 최대값으로 초기화
		mLatestClosedTimeSec = UINT32_MAX;

		// mSocket (클라이언트 소켓 ) 을 미리 만들기 
		mSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);

		if (mSocket == INVALID_SOCKET)
		{
			printf_s("client socket WSASokcet Error : %d\n", GetLastError());
			return false;
		}

		// 비동기 접속 예약 AcceptEx : 이 함수는 이 함수는 운영체제에게 **"이 listenSock_으로 누가 접속하면, 내가 미리 만든 mSocket하고 연결해 주세요."**라고 부탁하는 함수입니다.

		ZeroMemory(&mAcceptContext, sizeof(stOverlappedEx));
		DWORD bytes = 0;
		DWORD flags = 0;
		mAcceptContext.m_wsaBuf.len = 0;
		mAcceptContext.m_wsaBuf.buf = nullptr;
		mAcceptContext.m_eOperation = IOOperation::ACCEPT;
		mAcceptContext.SessionIndex = mIndex;

		// AcceptEx() : 4번째 인자가 0인 것은 **"접속만 시키고, 첫 데이터는 기다리지 말라"**는 뜻입니다.
		if (AcceptEx(listenSock_,mSocket,mAcceptBuf,0,
			sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN) + 16,&bytes,(LPOVERLAPPED) & mAcceptContext) == false)
		{
			if (WSAGetLastError()!=WSA_IO_PENDING)
			{
				printf_s("Accept Error : %d\n", GetLastError());
				return false;
			}
		}

		return true;
	}

	bool AcceptCompletion()
	{


	}

	bool BindIOCompletionPort(HANDLE iocpHandle_)
	{
		// socket 과 pClientInfo 를  CompletionPort 객체를 연결
		auto hIOCP = CreateIoCompletionPort(
			(HANDLE)GetSock(),
			iocpHandle_,
			(ULONG_PTR)(this), 0);

		if (hIOCP == INVALID_HANDLE_VALUE)
		{
			printf("[에러] socket 과 pClientInfo 를  CompletionPort 객체와 바인드 실패 : %d \n", GetLastError());

			return false;
		}
		return true;
	}

	bool BindRecv()
	{
		DWORD dwFlag = 0;
		DWORD dwRecvNumBytes = 0;

		// Overlapped IO 를 위해 각 정보를 세팅
		mRecvOverlappedEx.m_wsaBuf.len = MAX_SOCK_RECVBUF;
		mRecvOverlappedEx.m_wsaBuf.buf = mRecvBuf;
		mRecvOverlappedEx.m_eOperation = IOOperation::RECV;

		// 비동기 수신 작업 요청 걸어두기
		int nRet = WSARecv(mSocket,
			&(mRecvOverlappedEx.m_wsaBuf),
			1,
			&dwRecvNumBytes,
			&dwFlag,
			(LPWSAOVERLAPPED)&(mRecvOverlappedEx),
			NULL);

		//  SOCKET_ERROR 일때 클라이언트 소켓이 끊어진것으로 처리하기
		// ERROR_IO_PENDING은 비동기 작업이 즉시 완료되지 않았으며 백그라운드에서 계속 처리 중임을 나타내는 정상적인 상태 코드

		if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
		{
			printf("[에러] WSARecv() 함수 실패 : %d \n", WSAGetLastError());
			return false;
		}
		return true;
	}

	//*********** 1개의 스레드에서만 호출해야한다 ???
	bool SendMsg(const UINT32 dataSize_,char* pMsg_)
	{
		auto sendOverlappedEx = new stOverlappedEx;

		ZeroMemory(sendOverlappedEx, sizeof(stOverlappedEx));

		sendOverlappedEx->m_wsaBuf.len = dataSize_;
		sendOverlappedEx->m_wsaBuf.buf = new char[dataSize_];

		CopyMemory(sendOverlappedEx->m_wsaBuf.buf, pMsg_, dataSize_);

		sendOverlappedEx->m_eOperation = IOOperation::SEND;

		// 

		std::lock_guard<std::mutex> guard(mSendLock);

		mSendDataQueue.push(sendOverlappedEx);

		if (mSendDataQueue.size() == 1)
		{
			SendIO();
		}

		return true;
	}

	void SendCompleted(const UINT32 dataSize_)
	{
		printf("[송신완료] bytes : %d\n", dataSize_);

		std::lock_guard<std::mutex> guard(mSendLock);

		// 동적 메모리 버퍼 해제
		delete[] mSendDataQueue.front()->m_wsaBuf.buf;

		// 객체 자체를 해제
		delete mSendDataQueue.front();


		// delete 먼저하고 송신완료 되면 큐에서 제거(pop)
		mSendDataQueue.pop();

		if (mSendDataQueue.empty() == false)
		{
			SendIO();
		}
	}

private:

	bool SendIO()
	{
		auto sendOverlappedEx = mSendDataQueue.front();

		DWORD dwRecvNumBytes = 0;

		int nResult = WSASend(mSocket,
			&(sendOverlappedEx->m_wsaBuf),
			1,
			&dwRecvNumBytes,
			0,
			(LPWSAOVERLAPPED)sendOverlappedEx,
			NULL);

		if (nResult == SOCKET_ERROR && (WSAGetLastError()!=ERROR_IO_PENDING))
		{
			printf("[에러] WSASend 함수 실패 : %d\n", WSAGetLastError());
			return false;
		}
		return true;
	}


	INT32 mIndex = 0;
	HANDLE mIOCpHandle = INVALID_HANDLE_VALUE;

	INT64 mIsConnect = 0;
	UINT64 mLatestClosedTimeSec = 0;

	SOCKET mSocket; // 클라이언트와 연결되는 소켓

	stOverlappedEx mAcceptContext;

	char mAcceptBuf[64];

	stOverlappedEx mRecvOverlappedEx;

	char mRecvBuf[MAX_SOCK_RECVBUF];

	std::mutex mSendLock;

	std::queue<stOverlappedEx*> mSendDataQueue;
};
