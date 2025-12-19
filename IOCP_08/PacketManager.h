#pragma once

#include "Packet.h"

#include <unordered_map>
#include <deque>
#include <thread>
#include <mutex>

class PacketManager
{
public:
	void Init(const UINT32 maxClient_);

	void ReceivePacketData(const UINT32 clientIndex_, const UINT size_, char* pData_);

	void PushSystemPacket(PacketInfo packet_);

private:

	void EnqueuePacketData(const UINT32 clientIndex_);

	void ProcessPacket();

	void ProcessRecvPacket(const UINT32 clientIndex , const UINT16 packetId_ , const UINT16 packetSize_, char* pPacket_);

	void ProcessUserConnect(UINT32 clientIndex_,UINT16 packetSize_,char* pPacket_);



	// 멤버 함수 포인터를 "PROCESS_RECV_PACKET_FUNCTION" 이라는 별칭으로  정의
	typedef void(PacketManager::* PROCESS_RECV_PACKET_FUNCTION)(UINT32, UINT16,char*);

	// int (key) - 함수포인터(value) 의 맵을 정의
	// 패킷 핸들러 디스패티 패턴으로 if-else ,switch-case 문을 길게 나열하는 대신 맵으로 알맞는 함수를 찾아가서 실행하는 효율적인 방식 
	std::unordered_map<int, PROCESS_RECV_PACKET_FUNCTION> mRecvFuntionalDictionary;

	std::thread mProcessThread;

	std::mutex mLock;

	std::deque<UINT32> mInComingPacketUserIndex;

	std::deque<PacketInfo> mSystemPacketQueue;
};

