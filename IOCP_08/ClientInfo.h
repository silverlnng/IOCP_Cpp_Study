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

	bool OnConnect()
	{
	}

	void Close()
	{


	}

	bool PostAccept(SOCKET listenSock_,const UINT64 curTimeSec_)
	{
		// 미리 접속할 빈소켓을 만들고 운영체제에 예약해두는 역할

		//  닫힌 시간을 최대값으로 초기화
		mLatestClosedTimeSec = UINT32_MAX;

		// mSocket 을 소켓을 미리 만들기 
		mSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);

		// 비동기 접속 예약 AcceptEx : 이 함수는 이 함수는 운영체제에게 **"이 listenSock_으로 누가 접속하면, 내가 미리 만든 mSocket하고 연결해 주세요."**라고 부탁하는 함수입니다.

		// AcceptEx() : 4번째 인자가 0인 것은 **"접속만 시키고, 첫 데이터는 기다리지 말라"**는 뜻입니다.
	}

	bool AcceptCompletion()
	{

	}

	bool BindIOCompletionPort(HANDLE iocpHandle_)
	{
		// socket 과 CompletionPort 객체를 연결
	}

private:

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
