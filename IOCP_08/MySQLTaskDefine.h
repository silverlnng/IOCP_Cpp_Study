#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "ErrorCode.h"
#include "Packet.h"

enum class MySQLTaskID : UINT16
{
	INVALID = 0,

	REQUEST_LOGIN = 1001,
	RESPONSE_LOGIN = 1002,
};



struct MySQLTask
{
	UINT32 UserIndex = 0;
	MySQLTaskID TaskID = MySQLTaskID::INVALID;
	UINT16 DataSize = 0;
	char* pData = nullptr;

	void Release()
	{
		if (pData != nullptr)
		{
			delete[] pData;
		}
	}
};




#pragma pack(push,1)

struct MySQLLoginReq
{
	char UserID[MAX_USER_ID_LEN + 1];
	char UserPW[MAX_USER_PW_LEN + 1];
};

struct MySQLLoginRes
{
	UINT16 Result = (UINT16)ERROR_CODE::NONE;
};

#pragma pack(pop) //위에 설정된 패킹설정이 사라짐