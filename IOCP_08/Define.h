#pragma once

#include <winsock2.h>
#include <mswsock.h>

const UINT32 MAX_SOCK_RECVBUF = 256;
const UINT32 MAX_SOCK_SENDBUF = 4096;
const UINT64 RE_USE_SESSION_WAIT_TIMESEC = 3;

enum class IOOperation
{
	ACCEPT,
	RECV,
	SEND
};

// WSAOVERLAPPED 구조체를 확장 시켜서 필요한 정보를 더 넣기
struct stOverlappedEx
{
	WSAOVERLAPPED m_wsaOverlapped;

	WSABUF m_wsaBuf;

	IOOperation m_eOperation;

	UINT32 SessionIndex = 0;
};