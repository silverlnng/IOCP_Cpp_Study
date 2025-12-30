#pragma once

#include "IOCPServer.h"
#include "Packet.h"
#include "PacketManager.h"

class ChatServer : public IOCPServer 
{
public:

	virtual void OnConnect(const UINT32 clientIndex_) override
	{

	}
	virtual void OnClose(const UINT32 clientIndex_) override
	{

	}

	virtual void OnReceive(const UINT32 clientIndex_, const UINT32 size_, char* pData_) override
	{

	}

	void Run(const UINT32 maxClient)
	{

	}

	void End()
	{

	}
private:

	std::unique_ptr<PacketManager> m_pPacketManager;
};

