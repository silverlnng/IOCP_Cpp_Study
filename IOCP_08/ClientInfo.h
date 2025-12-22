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
