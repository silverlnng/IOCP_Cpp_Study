#pragma once
#include <string>
#include <atomic>
#include "Packet.h"

class User
{
	const UINT32 PACKET_DATA_BUFFER_SIZE = 8192;

public:

	enum class DOMAIN_STATE
	{
		NONE =0,
		LOGIN=1,
		ROOM =2
	};

	User() = default;
	~User() = default;

	void Init(const INT32 index)
	{
		mIndex = index;

		// 유저의 개인 버퍼
		mPacketDataBuffer = new char[PACKET_DATA_BUFFER_SIZE];
		// 잘린 패킷을 임시로 펴주기 위한 선형 버퍼
		mSplitPacketBuffer.resize(PACKET_DATA_BUFFER_SIZE);

		// Atomic 변수 초기화
		mWritePos = 0;
		mReadPos = 0;
	}

	void Clear()
	{
		mRoomIndex = -1;
		mUserID = "";
		
		mIsConfirm = false;
		mCurDomainState = DOMAIN_STATE::NONE;

		// Atomic 변수 초기화 (순서 중요하지 않음, 단일 스레드 호출 보장 시)
		mWritePos.store(0, std::memory_order_relaxed);
		mReadPos.store(0, std::memory_order_relaxed);
	}

	int SetLogin(char* userID_)
	{
		mCurDomainState = DOMAIN_STATE::LOGIN;
		mUserID = userID_;

		return 0;
	}

	void EnterRoom(INT32 roomIndex_)
	{
		mRoomIndex = roomIndex_;
		mCurDomainState = DOMAIN_STATE::ROOM;
	}

	INT32 GetCurrentRoom()
	{
		return mRoomIndex;
	}


	std::string GetUserId() const
	{
		return mUserID;
	}

	DOMAIN_STATE GetDomainState() const
	{
		return mCurDomainState;
	}

  // =================================================================
  // [핵심] 링 버퍼 쓰기 (Write / Producer)
  // 데이터 이동(Shift) 없이 순환하여 기록합니다.
  // 워커스레드에서 작동
  // =================================================================

	void SetPacketData(const UINT32 dataSize_,char* pData_)
	{
		// TCP 의 스트림 특성을 처리
		// 조각난 데이터가 올 때마다 버퍼에 계속 이어 붙여서 하나의 덩어리로 만드는 역할
		// 유저의 개인 버퍼(mPakcetDataBuffer)에 이어 붙이는 역할

		 // 1. 소비자(Logic)가 어디까지 읽었는지 확인 
		UINT32 currentReadPos = mReadPos.load(std::memory_order_acquire);
		UINT32 currentWritePos = mWritePos.load(std::memory_order_relaxed); 
		// WritePos 는 여기서만 업데이트하니까 가벼운 std::memory_order_relaxed

		// 1. 여유 공간 계산
		// 링 버퍼의 여유 공간 = (전체 크기 - 사용 중인 크기 - 1)
		// (-1을 하는 이유는 꽉 찬 상태와 빈 상태를 구분하기 위함)
		UINT32 useSize = (currentWritePos >= currentReadPos) ?
			(currentWritePos - currentReadPos) :
			(PACKET_DATA_BUFFER_SIZE - currentReadPos + currentWritePos);

		UINT32 freeSize = PACKET_DATA_BUFFER_SIZE - useSize - 1;

		// 여유공간보다 쓸 데이터가 더 크면 오류
		if (freeSize < dataSize_)
		{
			// 에러 처리: 버퍼 오버플로우 (연결 끊기 등)
			// printf("User Buffer Full!\n");
			return;
		}

		// 2. 데이터 복사 (Wrap Around 처리)
		// 버퍼 끝까지 남은 선형 공간
		UINT32 linearFreeSize = PACKET_DATA_BUFFER_SIZE - currentWritePos;

		if (dataSize_ <= linearFreeSize)
		{
			// (A) 한번에 쓸 수 있는 경우
			CopyMemory(&mPacketDataBuffer[currentWritePos], pData_, dataSize_);
		}
		else
		{
			// (B) 버퍼 끝을 넘어가서 앞쪽으로 이어 써야 하는 경우
			CopyMemory(&mPacketDataBuffer[currentWritePos], pData_, linearFreeSize);
			CopyMemory(&mPacketDataBuffer, pData_ + linearFreeSize, dataSize_ - linearFreeSize);
		}

		
		// 3. [중요] 쓰기 포인터 업데이트 (Release)
	    // 이 명령은 위의 CopyMemory가 완전히 끝난 후에 실행됨을 보장합니다.
		UINT32 nextWritePos = (currentWritePos + dataSize_) % PACKET_DATA_BUFFER_SIZE;
		mWritePos.store(nextWritePos, std::memory_order_release);
	}



	// 쌓여있는 버퍼에서 약속된 헤더 (프로토콜) 을 기준으로 의미있는 패킷 하나를 잘라내는 역할
	// =================================================================
	// [핵심] 링 버퍼 읽기 (Read / Consumer)
	// PacketManager 의 ProcessThread 에서 작동
	// =================================================================

	PacketInfo GetPacket()
	{
		// 1. 생산자(IO)가 어디까지 썼는지 확인 (Acquire)
	   // Release로 기록된 WritePos를 읽으면, 그 앞의 데이터 복사가 완료되었음을 보장받습니다.
		UINT32 currentWritePos = mWritePos.load(std::memory_order_acquire);
		UINT32 currentReadPos = mReadPos.load(std::memory_order_relaxed);
		// ReadPos는 는 여기서만 업데이트하니까 가벼운 std::memory_order_relaxed
		
		// 1. 현재 버퍼에 들어있는 데이터 크기 계산
		UINT32 useSize = (currentWritePos >= currentReadPos) ?
			(currentWritePos - currentReadPos) :
			(PACKET_DATA_BUFFER_SIZE - currentReadPos + currentWritePos);

		// 헤더조차 다 못 읽었으면 리턴
		if (useSize < PACKET_HEADER_LENGTH)
		{
			return PacketInfo();
		}

		// 2. 헤더 읽기 (Peek)
		PACKET_HEADER header;

		// 헤더가 버퍼 끝에 걸쳐 있는지 확인
		if (currentReadPos + PACKET_HEADER_LENGTH <= PACKET_DATA_BUFFER_SIZE)
		{
			// (A) 헤더가 연속된 공간에 있음
			header = *(PACKET_HEADER*)&mPacketDataBuffer[currentReadPos];
		}
		else
		{
			// (B) 헤더가 잘려 있음 (Wrap Around) -> 임시 복사해서 읽음
			UINT32 firstPartSize = PACKET_DATA_BUFFER_SIZE - currentReadPos;
			char tempHeaderBuf[sizeof(PACKET_HEADER)];

			CopyMemory(tempHeaderBuf, &mPacketDataBuffer[currentReadPos], firstPartSize);
			CopyMemory(tempHeaderBuf + firstPartSize, &mPacketDataBuffer, PACKET_HEADER_LENGTH - firstPartSize);

			header = *(PACKET_HEADER*)tempHeaderBuf;
		}

		// 3. 패킷 전체 크기 확인
		if (useSize < header.PacketLength)
		{
			return PacketInfo(); // 아직 바디가 다 안 옴
		}

		// 4. 패킷 데이터 가져오기
		PacketInfo packetInfo;
		packetInfo.PacketId = header.PacketId;
		packetInfo.DataSize = header.PacketLength;

		// 패킷이 버퍼 끝에 걸쳐 있는지 확인
		if (currentReadPos + header.PacketLength <= PACKET_DATA_BUFFER_SIZE)
		{
			// [최적화 Case] 패킷이 잘리지 않음 -> Zero Copy (원본 버퍼 포인터 반환)
			packetInfo.pDataPtr = &mPacketDataBuffer[currentReadPos];
		}
		else
		{
			// [일반 Case] 패킷이 잘림 -> 임시 버퍼에 선형화(Linearize)하여 반환
			UINT32 firstPartSize = PACKET_DATA_BUFFER_SIZE - currentReadPos;
			UINT32 secondPartSize = header.PacketLength - firstPartSize;

			CopyMemory(&mSplitPacketBuffer, &mPacketDataBuffer[currentReadPos], firstPartSize);
			CopyMemory(&mSplitPacketBuffer[firstPartSize], &mPacketDataBuffer, secondPartSize);

			packetInfo.pDataPtr = &mSplitPacketBuffer[0];
		}

		// 4. 읽기 포인터 업데이트 (Release)
	   // 생산자가 공간이 생겼음을 알 수 있도록 업데이트
		UINT32 nextReadPos = (currentReadPos + header.PacketLength) % PACKET_DATA_BUFFER_SIZE;
		mReadPos.store(nextReadPos, std::memory_order_release);



		return packetInfo;

	}

private:

	INT32 mIndex = -1;

	INT32 mRoomIndex = -1;

	std::string mUserID;


	DOMAIN_STATE mCurDomainState = DOMAIN_STATE::NONE;

	/// <summary>	 기존의 유저의 개인 패킷 버퍼에서 읽기, 쓰기 위치
	
	//UINT32 mPacketDataBufferWPos = 0;

	//UINT32 mPacketDataBufferRPos = 0;

	// 유저의 개인 버퍼
	//char* mPacketDataBuffer = nullptr;

	/// </summary>
	
	bool mIsConfirm = false;
	std::string mAuthToken;

	// [Atomic 적용 및 캐시 라인 패딩]
   // alignas(64)는 False Sharing 방지를 위해 서로 다른 캐시 라인에 배치
	alignas(64) std::atomic<UINT32> mWritePos = { 0 };
	alignas(64) std::atomic<UINT32> mReadPos = { 0 };

	char* mPacketDataBuffer = nullptr;

	// [패킷 선형화용 임시 버퍼]
	// 링 버퍼 끝에 걸친 패킷을 처리하기 위해 사용
	std::vector<char> mSplitPacketBuffer;
};