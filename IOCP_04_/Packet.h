#pragma once

#include <Windows.h>

// 클라이언트가 보낸 패킷을 저장하는 구조체
struct PacketData
{
	UINT32 SessionIndex = 0;
	UINT32 DataSize = 0;
	char* pPacketData = nullptr;

	void Set(UINT32 sessionIndex_,UINT32 dataSize_,char* pData )
	{
		SessionIndex = sessionIndex_;
		DataSize = dataSize_;

		pPacketData = new char[dataSize_];

		CopyMemory(pPacketData, pData, dataSize_);
	}

};