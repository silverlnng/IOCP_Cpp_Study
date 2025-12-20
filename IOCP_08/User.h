#pragma once

#include "Packet.h"

class User
{
	const UINT32 PACKET_DATA_BUFFER_SIZE = 8096;

public:

	void Init(const INT32 index)
	{

	}

	void Clear()
	{

	}

	PacketInfo GetPacket()
	{
		// PacketLength 을 기반으로 완전한 형태의 패킷하나를 분리하는 역할
	}

private:

	INT32 mIndex = -1;

	INT32 mRoomIndex = -1;

	std::string mUserID;


};