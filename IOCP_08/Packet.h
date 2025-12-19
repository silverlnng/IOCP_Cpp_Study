#pragma once

#include <windows.h>

struct PacketInfo
{
	UINT32 ClientIndex = 0;
	UINT16 PacketId = 0;
	UINT16 DataSize = 0;
	char* pDataPtr = nullptr;
};

enum class PACKET_ID : UINT16
{
	// 시스템
	SYS_USER_CONNECT =11,
	SYS_USER_DISCONNECT =12,
	SYS_END=30,

	// 클라이어트
	LOGIN_REQUEST = 201,
	LOGIN_RESPONSE = 202,

	ROOM_ENTER_REQUEST = 215,
	ROOM_ENTER_RESPONSE =216,

};
