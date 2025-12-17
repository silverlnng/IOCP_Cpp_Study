#pragma once

#include "IOCPServer.h"
#include "Packet.h"

#include <deque>
#include <thread>
#include <mutex>

class EchoServer : public IOCPServer
{
public:

	virtual void OnReceive(const UINT32 clientIndex_, const UINT32 size_, char* pData) override
	{
		printf("[OnReceive] 클라이언트 : Index(%d) , dataSize(%d)\n", clientIndex_, size_);

		PacketData packet;
		packet.Set(clientIndex_, size_, pData);

		// lock_guard : 자동으로 lock, unlock 을 해주는 도어락 시스템. 함수가 어떻게 끝나든 무조건 잠금을 해제해 주므로 훨씬 안전
		// mlock.lock() 을 자동으로 수행
		std::lock_guard<std::mutex> guard(mLock);

		// 이제 부터 이 블록이 lock
		
		// deque 에 안저하게 데이터를 넣음
		mPacketDataQueue.push_back(packet);

	} // 자동으로 guard 가 mlock.unlock() 수행

private:

	std::thread mProcessThread;

	std::mutex mLock;	// 자물쇠 그 자체 
	std::deque<PacketData> mPacketDataQueue;
};