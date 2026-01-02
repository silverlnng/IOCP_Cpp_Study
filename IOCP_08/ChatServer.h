#pragma once

#include "IOCPServer.h"
#include "Packet.h"
#include "PacketManager.h"

class ChatServer : public IOCPServer 
{
public:
	ChatServer() = default;
	virtual ~ChatServer() = default;

	// 워커스레드가 작동
	virtual void OnConnect(const UINT32 clientIndex_) override
	{
		printf("[OnConnect] 클라이언트 : Index(%d)\n", clientIndex_);

		// 패킷 헤더+data 미리 설정한 구조체
		PacketInfo packet{clientIndex_,(UINT16)PACKET_ID::SYS_USER_CONNECT,0};

		m_pPacketManager->PushSystemPacket(packet);
	}

	virtual void OnClose(const UINT32 clientIndex_) override
	{
		printf("[OnClose] 클라이언트 : Index(%d)\n", clientIndex_);

		PacketInfo packet{ clientIndex_,(UINT16)PACKET_ID::SYS_USER_DISCONNECT,0 };

		m_pPacketManager->PushSystemPacket(packet);
	}

	// 워커스레드가 작동
	virtual void OnReceive(const UINT32 clientIndex_, const UINT32 size_, char* pData_) override
	{
		printf("[OnReceive] 클라이언트 : Index(%d) , dataSize(%d) \n", clientIndex_, size_);

		m_pPacketManager->ReceivePacketData(clientIndex_, size_, pData_);
	}

	void Run(const UINT32 maxClient)
	{
		// 람다 표현식
		auto sendPacketFunc = [&](UINT32 clientIndex_, UINT16 packetSize_, char* pSendPacket)
			{
				SendMsg(clientIndex_, packetSize_, pSendPacket);
			};

		m_pPacketManager = std::make_unique<PacketManager>();
		// 결합도를 낮추기 위해서 , 패킷매니저가 send 해야하는 순간 자기멤버함수 호출로 ChatServer 의 등록되어있는 함수 SendMsg 를 실행할수있다
		m_pPacketManager->SendPacketFunc = sendPacketFunc;
		m_pPacketManager->Init(maxClient);
		m_pPacketManager->Run();
		
		StartServer(maxClient);
	}

	void End()
	{
		m_pPacketManager->End();

		DestroyThread();
	}
private:

	std::unique_ptr<PacketManager> m_pPacketManager;
};

