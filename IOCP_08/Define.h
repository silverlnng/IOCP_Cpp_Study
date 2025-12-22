#pragma once

#include <winsock2.h>

const UINT32 MAX_SOCK_RECVBUF = 256;
const UINT32 MAX_SOCK_SENDBUF = 4096;

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