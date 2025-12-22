#pragma once

#include "Packet.h"

class User
{
	const UINT32 PACKET_DATA_BUFFER_SIZE = 8096;

public:

	enum class DOMAIN_STATE
	{
		NONE =0,
		LOGIN=1,
		ROOM =2
	};

	void Init(const INT32 index)
	{
		mIndex = index;

		mPacketDataBuffer = new char[PACKET_DATA_BUFFER_SIZE];
	}

	void Clear()
	{

	}

	void SetPacketData()
	{

	}

private:

	INT32 mIndex = -1;

	INT32 mRoomIndex = -1;

	std::string mUserID;

	UINT32 mPacketDataBufferWPos = 0;

	UINT32 mPacketDataBufferRPos = 0;

	char* mPacketDataBuffer = nullptr;
};