#pragma once
#include <string>

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

		// 유저의 개인 버퍼
		mPacketDataBuffer = new char[PACKET_DATA_BUFFER_SIZE];
	}

	void Clear()
	{

	}

	std::string GetUserId() const
	{
		return mUserID;
	}

	void SetPacketData(const UINT32 dataSize_,char* pData_)
	{
		// TCP 의 스트림 특성을 처리
		// 조각난 데이터가 올 때마다 버퍼에 계속 이어 붙여서 하나의 덩어리로 만드는 역할
		// 유저의 개인 버퍼(mPakcetDataBuffer)에 이어 붙이는 역할

			// 만약 버퍼의 끝에 도달해서 더쓸 공간이 부족하다
		if ((mPacketDataBufferWPos+dataSize_)>=PACKET_DATA_BUFFER_SIZE)
		{
			// 아직 안 읽은 데이터 양
			auto remainDataSize = mPacketDataBufferWPos - mPacketDataBufferRPos;

			// 아직 안 읽은 데이터 양 이 있음
			if (remainDataSize>0)
			{

				// 아직 안 읽은 데이터를 버퍼의 맨 앞(0 번지) 러 복사 이동 시킴
				CopyMemory(&mPacketDataBuffer[0], &mPacketDataBuffer[mPacketDataBufferRPos], remainDataSize);
				
				// 
				mPacketDataBufferWPos = remainDataSize;
			}
			else  // 아직 안 읽은 데이터 양 없으면
			{
				//앞으로 읽을 위치 mPacketDataBufferWPos 만 맨앞으로 이동 시키기
				mPacketDataBufferWPos = 0;
			}

			mPacketDataBufferRPos = 0;
		}

		// 데이터를 버퍼의 현재 쓰기 위치에 복사하기 
		CopyMemory(&mPacketDataBuffer[mPacketDataBufferWPos], pData_, dataSize_);

		mPacketDataBufferWPos += dataSize_;

	}

	PacketInfo GetPacket()
	{
		// 쌓여있는 버퍼에서 약속된 헤더 (프로토콜) 을 기준으로 의미있는 패킷 하나를 잘라내는 역할

		UINT32 remainByte = mPacketDataBufferWPos - mPacketDataBufferRPos;

		// 받은 데이터가 헤더크기보다 작은지 확인
		if (remainByte<PACKET_HEADER_LENGTH)
		{
			// 작으면 아직 헤더도 안 온 경우 => 빈 패킷을 반환
			return PacketInfo();
		}

		// 헤더 보다는 크다 => 앞부분을 헤더라고 가정하고 덮어쓰기 (Casting) 
		auto pHeader =(PACKET_HEADER*) & mPacketDataBuffer[mPacketDataBufferRPos];

		// 헤더에 적힌 전체길이 만큼 데이터가 다왔는지 확인 
		if (pHeader->PacketLengeh > remainByte)
		{
			// 다 안왔으면 빈 패킷을 반환
			return PacketInfo();
		}

		// 여기까지 왔으면 온전한 패킷하나가 완성된것
		// 정보채우기 
		PacketInfo packetInfo;
		packetInfo.PacketId = pHeader->PacketId;
		packetInfo.DataSize = pHeader->PacketLengeh;
		packetInfo.pDataPtr = &mPacketDataBuffer[mPacketDataBufferRPos];
			

		// 읽은 만큼 읽기 커서 이동
		mPacketDataBufferRPos += pHeader->PacketLengeh;

		return packetInfo;
	}

private:

	INT32 mIndex = -1;

	INT32 mRoomIndex = -1;

	std::string mUserID;

	UINT32 mPacketDataBufferWPos = 0;

	UINT32 mPacketDataBufferRPos = 0;

	// 유저의 개인 버퍼
	char* mPacketDataBuffer = nullptr;
};