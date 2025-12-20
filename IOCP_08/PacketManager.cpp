

#include "UserManager.h"
#include "PacketManager.h"

void PacketManager::Init(const UINT32 maxClient_)
{
	mRecvFuntionalDictionary = std::unordered_map<int, PROCESS_RECV_PACKET_FUNCTION>();

	// 멤버함수의 메모리 주소 (value) 에 저장

	mRecvFuntionalDictionary[(int)PACKET_ID::SYS_USER_CONNECT] = &PacketManager::ProcessUserConnect;

}

bool PacketManager::Run()
{
	mIsRunProcessThread = true;
	mProcessThread = std::thread([this]() {ProcessPacket(); });

	return true;
}

void PacketManager::ReceivePacketData(const UINT32 clientIndex_, const UINT size_, char* pData_)
{


	EnqueuePacketData(clientIndex_);
}

void PacketManager::PushSystemPacket(PacketInfo packet_)
{
	std::lock_guard<std::mutex> guard(mLock);
	mSystemPacketQueue.push_back(packet_);
}

void PacketManager::CreateComponent(const UINT32 maxClient)
{
	mUserManager = new UserManager;

}

void PacketManager::EnqueuePacketData(const UINT32 clientIndex_)
{
	std::lock_guard<std::mutex> guard(mLock);
	mInComingPacketUserIndex.push_back(clientIndex_);
}

PacketInfo PacketManager::DequePacketData()
{
	UINT32 userIndex = 0;

	// 중괄호로 std::lock_guard 의 scope를 필요한 부분만 으로 제한 해주는 것 
	// 임계영역 범위를 최소화 ! => 성능 높이는 방법
	{
		std::lock_guard<std::mutex> guard(mLock);

		if (mInComingPacketUserIndex.empty())
		{
			// 비어 있으면 
			return PacketInfo();
		}

		userIndex = mInComingPacketUserIndex.front();
		mInComingPacketUserIndex.pop_front();
	}

	mUser

	return PacketInfo();
}

void PacketManager::ProcessPacket()
{
	while (mIsRunProcessThread)
	{

		if(dequePacket)

	}
}

void PacketManager::ProcessRecvPacket(const UINT32 clientIndex_, const UINT16 packetId_, const UINT16 packetSize_, char* pPacket_)
{
	auto iter = mRecvFuntionalDictionary.find(packetId_);
	if (iter != mRecvFuntionalDictionary.end())
	{
		// *(함수포인터) : 함수포인터 value 앞에 포인터로 함수가있는 부분까지 가서  funtional call operator (괄호) 로 실행까지.
		(this->*(iter->second))(clientIndex_, packetSize_, pPacket_);
	}
}

void PacketManager::ProcessUserConnect(UINT32 clientIndex_, UINT16 packetSize_, char* pPacket_)
{
	printf("[ProcessUserConnect] clientIndex : %d\n", clientIndex_);
	
}
