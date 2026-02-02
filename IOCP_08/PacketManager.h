#pragma once

#include "Packet.h"

#include <unordered_map>
#include <deque>
#include <thread>
#include <mutex>
#include <functional>
#include <atomic>

class UserManager;
class RedisManager;
class RoomManager;
class MySQLManager;

class PacketManager
{
public:

	void Init(const UINT32 maxClient_);

	bool Run();

	void End();

	void ReceivePacketData(const UINT32 clientIndex_, const UINT size_, char* pData_);

	void PushSystemPacket(PacketInfo packet_);

	//  어떤 형태(반환형은 void이고, 인자로 (UINT32, UINT32, char*)를 받는)의 함수를 담을 수 있는 그릇
	//  이 그릇을 통해서 PacketManager 외부(chatserver.cpp)에서 패킷 전송 함수(SendMsg)를 주입받아 사용할 수 있다
	std::function<void(UINT32,UINT32,char*)> SendPacketFunc;


private:
	void CreateComponent(const UINT32 maxClient_);

	void ClearConnectionInfo(UINT32 clientIndex_);

	void EnqueuePacketData(const UINT32 clientIndex_);

	PacketInfo DequePacketData();

	PacketInfo DequeSystemPacketData();

	void ProcessPacket();

	void ProcessRecvPacket(const UINT32 clientIndex , const UINT16 packetId_ , const UINT16 packetSize_, char* pPacket_);

	void ProcessUserConnect(UINT32 clientIndex_,UINT16 packetSize_,char* pPacket_);

	void ProcessUserDisConnect(UINT32 clientIndex_,UINT16 packetSize_,char* pPacket_);

	void ProcessDevEcho(UINT32 clientIndex_, UINT16 packetSize_, char* pPacket_);

	void ProcessLogin(UINT32 clientIndex_,UINT16 packetSize_,char* pPacket_);

	void ProcessLoginDBResult(UINT32 clientIndex_, UINT16 packetSize_, char* pPacket_);

	void ProcessEnterRoom(UINT32 clientIndex_, UINT16 packetSize_, char* pPacket_);

	void ProcessLeaveRoom(UINT32 clientIndex_, UINT16 packetSize_, char* pPacket_);

	void ProcessRoomChatMessage(UINT32 clientIndex_, UINT16 packetSize_, char* pPacket_);

	void ProcessUpdateScore(UINT32 clientIndex_, UINT16 packetSize_, char* pPacket_);

	// 멤버 함수 포인터를 "PROCESS_RECV_PACKET_FUNCTION" 이라는 별칭으로  정의
	typedef void(PacketManager::* PROCESS_RECV_PACKET_FUNCTION)(UINT32, UINT16,char*);

	// int (key) - 함수포인터(value) 의 맵을 정의
	// 패킷 핸들러 디스패티 패턴으로 if-else ,switch-case 문을 길게 나열하는 대신 맵으로 알맞는 함수를 찾아가서 실행하는 효율적인 방식 
	std::unordered_map<int, PROCESS_RECV_PACKET_FUNCTION> mRecvFuntionalDictionary;

	UserManager* mUserManager;
	RedisManager* mRedisManager;
	RoomManager* mRoomManager;
	MySQLManager* mMySQLManager;

	std::function<void(int, char*)> mSendMQDataFunc;

	bool mIsRunProcessThread = false;

	std::thread mProcessThread;

	std::mutex mLock;


	// 연결 성립, 연결 종료때 만 사용.
	// 데이터 버퍼에 쌓을 필요없음
	// 패킷을 받은 즉시 처리하면 된다
	std::deque<PacketInfo> mSystemPacketQueue;


	//std::deque<UINT32> mInComingPacketUserIndex;

	// 1. 버퍼 크기 (2의 승수 권장: 65536 = 64K)
	static const UINT32 INCOMING_BUFFER_SIZE = 65536;
	static const UINT32 INCOMING_BUFFER_MASK = INCOMING_BUFFER_SIZE - 1;

	// 2. 고정 배열 (힙 할당 제거, 캐시 적중률 향상)
	UINT32 mInComingPacketUserIndex[INCOMING_BUFFER_SIZE];

	// 3. 인덱스 관리 (False Sharing 방지 패딩 적용)
	// Head: Consumer(LogicThread)가 읽는 위치
	alignas(64) std::atomic<size_t> mInComingHead = { 0 };

	// Tail: Producer(IO Threads)가 쓰는 위치
	alignas(64) std::atomic<size_t> mInComingTail = { 0 };

	// 4. MPSC 동기화용 SpinLock (Mutex보다 훨씬 가벼움)
	std::atomic_flag mInComingSpinLock = ATOMIC_FLAG_INIT;
};

