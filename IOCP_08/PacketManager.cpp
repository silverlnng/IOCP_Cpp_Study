#include <utility>

#include "UserManager.h"
#include "PacketManager.h"
#include "RedisManager.h"

void PacketManager::Init(const UINT32 maxClient_)
{
	mRecvFuntionalDictionary = std::unordered_map<int, PROCESS_RECV_PACKET_FUNCTION>();

	// 멤버함수의 메모리 주소 (value) 에 저장

	mRecvFuntionalDictionary[(int)PACKET_ID::SYS_USER_CONNECT] = &PacketManager::ProcessUserConnect;
	mRecvFuntionalDictionary[(int)PACKET_ID::SYS_USER_DISCONNECT] = &PacketManager::ProcessUserDisConnect;

	mRecvFuntionalDictionary[(int)PACKET_ID::LOGIN_REQUEST] = &PacketManager::ProcessLogin;
	mRecvFuntionalDictionary[(int)RedisTaskID::RESPONSE_LOGIN] = &PacketManager::ProcessLoginDBResult;

	mRecvFuntionalDictionary[(int)PACKET_ID::ROOM_ENTER_REQUEST] = &PacketManager::ProcessEnterRoom;



	// 패킷매니저에서 시작하면서 유저매니저를 생성하고 Init 실행시킴
	CreateComponent(maxClient_);

	mRedisManager = new RedisManager;
}

void PacketManager::CreateComponent(const UINT32 maxClient_)
{
	mUserManager = new UserManager;
	mUserManager->Init(maxClient_);


}



bool PacketManager::Run()
{
	// Redis 서버와의 연결하기
	if (mRedisManager->Run("127.0.0.1",6379,1)==false)
	{
		return false;	
	}


	mIsRunProcessThread = true;
	mProcessThread = std::thread([this]() {ProcessPacket(); });

	return true;
}

void PacketManager::End()
{
	mIsRunProcessThread = false;

	if (mProcessThread.joinable())
	{
		mProcessThread.join();
	}
}

void PacketManager::ClearConnectionInfo(UINT32 clientIndex_)
{
	auto pReqUser = mUserManager->GetUserByConnIdx(clientIndex_);

	//if (pReqUser->)

}

void PacketManager::ReceivePacketData(const UINT32 clientIndex_, const UINT size_, char* pData_)
{
	auto pUser = mUserManager->GetUserByConnIdx(clientIndex_);

	pUser->SetPacketData(size_, pData_);

	EnqueuePacketData(clientIndex_);
}


// 패킷이 들어오는걸 검사하고 , 처리하는 스레드

void PacketManager::ProcessPacket()
{
	
	while (mIsRunProcessThread)
	{
		bool  isIdle = true;
		// 패킷의 기본생성자들은 PacketId 을 0 으로 채움

		if (auto packetData = DequePacketData(); packetData.PacketId > (UINT16)PACKET_ID::SYS_END)
		{
			isIdle = false;

			ProcessRecvPacket(packetData.ClientIndex, packetData.PacketId, packetData.DataSize, packetData.pDataPtr);
		}

		if (auto packetData = DequeSystemPacketData(); packetData.PacketId!=0)
		{
			isIdle = false;

			ProcessRecvPacket(packetData.ClientIndex, packetData.PacketId, packetData.DataSize, packetData.pDataPtr);
		}

		// Redis 의 응답 Task 확인하고 가져오기 추가
		if (auto task = mRedisManager->TakeResponseTask(); task.TaskID != RedisTaskID::INVALID)
		{
			isIdle = false;
			
			ProcessRecvPacket(task.UserIndex, (UINT16)task.TaskID, task.DataSize, task.pData);

			task.Release();
		}


		if (isIdle)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

	}
}


void PacketManager::EnqueuePacketData(const UINT32 clientIndex_)
{
	std::lock_guard<std::mutex> guard(mLock);
	mInComingPacketUserIndex.push_back(clientIndex_);
}

PacketInfo PacketManager::DequePacketData()
{
	// 

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

	auto pUser = mUserManager->GetUserByConnIdx(userIndex);

	auto packetData = pUser->GetPacket();

	packetData.ClientIndex = userIndex;


	return packetData;
}

//  워커스레드가 작동
void PacketManager::PushSystemPacket(PacketInfo packet_)
{
	std::lock_guard<std::mutex> guard(mLock);
	mSystemPacketQueue.push_back(packet_);
}

// ProcessThread 에서 작동
PacketInfo PacketManager::DequeSystemPacketData()
{
	std::lock_guard<std::mutex> guard(mLock);

	if (mSystemPacketQueue.empty())
	{
		return PacketInfo();
	}

	auto packetData =mSystemPacketQueue.front();
	mSystemPacketQueue.pop_front();

	return packetData;
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
	
	auto pUser =mUserManager->GetUserByConnIdx(clientIndex_);

	pUser->Clear();
}

void PacketManager::ProcessUserDisConnect(UINT32 clientIndex_, UINT16 packetSize_, char* pPacket_)
{
	printf("[ProcessUserDisConnect] clientIndex : %d\n", clientIndex_)

	
}


void PacketManager::ProcessLogin(UINT32 clientIndex_, UINT16 packetSize_, char* pPacket_)
{
	if (LOGIN_REQUEST_PACKET_SIZE != packetSize_)
	{
		return;
	}

	// 바이트 단위를 패킷 구조체로 캐스팅
	// pPacket_  은 char* 타입이므로 LOGIN_REQUEST_PACKET* 타입으로 변환
	

	auto pLoginReqPacket = reinterpret_cast<LOGIN_REQUEST_PACKET*>(pPacket_);

	auto pUserID = pLoginReqPacket->UserID;

	printf("Request User ID = %s\n", pUserID);

	// 로그인 응답 패킷 준비
	LOGIN_RESPONSE_PACKET loginResPacket;
	loginResPacket.PacketId = (UINT16)PACKET_ID::LOGIN_RESPONSE;
	loginResPacket.PacketLengeh = sizeof(LOGIN_RESPONSE_PACKET);



	if (mUserManager->GetCurrentUserCnt() >= mUserManager ->GetMaxUserCnt())
	{
		// UserManager 를 통해  최대유저 , 현재 들어온 유저 수를 비교해서 남은 인원수가 있는지 확인

		// 지금 접속자 수가 최대 인원수 보다 같거나 커서 접속불가

		// 에러 코드 적어서 응답하기  
		loginResPacket.Result = (UINT16)ERROR_CODE::LOGIN_USER_USED_ALL_OBJ;

		SendPacketFunc(clientIndex_,sizeof(LOGIN_RESPONSE_PACKET),(char*)&loginResPacket);

		return;
	}

	// 여기서는 이미 접속된 유저인지 확인. 
	if (mUserManager->FindUserIndexByID(pUserID) == -1)
	{
		// 이미 접속중인 유저가 아닌 경우
		// RedisLoginReq 구조체에 로그인 요청 정보 채우기
		RedisLoginReq dbReq;

		
		CopyMemory(dbReq.UserID, pLoginReqPacket->UserID, MAX_USER_ID_LEN + 1);
		CopyMemory(dbReq.UserPW, pLoginReqPacket->UserPW, MAX_USER_PW_LEN + 1);

		// RedisTask 생성
		// RedisTask 에 로그인 요청 정보 채우기

		RedisTask task;
		task.UserIndex = clientIndex_;
		task.TaskID = RedisTaskID::REQUEST_LOGIN;
		task.DataSize = sizeof(RedisLoginReq);
		task.pData = new char[task.DataSize];

		CopyMemory(task.pData, (char*) & dbReq, task.DataSize);

		mRedisManager->PushTask(task);

		printf("[PacketManager::ProcessLogin] Redis Login Request PushTask UserID : %s \n", pUserID);

	}
	else
	{
		// 이미 mUserIDDictionary 에 등록된 접속중인 유저인 경우
		// 접속 중인 유저여서 실패 의미를 패킷에 보내줌
		loginResPacket.Result = (UINT16)ERROR_CODE::LOGIN_USER_ALREADY;

		SendPacketFunc(clientIndex_, sizeof(LOGIN_RESPONSE_PACKET), (char*)&loginResPacket);

		return;
	}

}

void PacketManager::ProcessLoginDBResult(UINT32 clientIndex_, UINT16 packetSize_, char* pPacket_)
{
	printf("[PacketManager::ProcessLoginDBResult] UserIndex : %d \n", clientIndex_);

	auto pBody = (RedisLoginRes*)pPacket_;

	if(pBody->Result == (UINT16)ERROR_CODE::NONE)
	{
		// 로그인 완료로 변경하기 	
	}

	LOGIN_RESPONSE_PACKET loginResPacket;
	loginResPacket.PacketId = (UINT16)PACKET_ID::LOGIN_RESPONSE;
	loginResPacket.PacketLengeh = sizeof(LOGIN_RESPONSE_PACKET);
	loginResPacket.Result = pBody->Result;

	SendPacketFunc(clientIndex_, sizeof(LOGIN_RESPONSE_PACKET), (char*)&loginResPacket);
}

void PacketManager::ProcessEnterRoom(UINT32 clientIndex_, UINT16 packetSize_, char* pPacket_)
{
	// UNREFERENCED_PARAMETER : 매개변수가 사용되지 않음을 컴파일러에 알리는 매크로 (매개변수 안쓸때 경고가 안나오도록 한다 )
	UNREFERENCED_PARAMETER(packetSize_);

	// 룸넘버가 추가된 패킷구조체로 캐스팅
	auto pEnterRoomReqPacket =reinterpret_cast<ROOM_ENTER_REQUEST_PACKET*>(pPacket_);

	auto pReqUser = mUserManager->GetUserByConnIdx(clientIndex_);

	if(!pReqUser || pReqUser == nullptr)
	{
		return;
	}

	ROOM_ENTER_RESPONSE_PACKET roomEnterResPacket;

	roomEnterResPacket.PacketId = (UINT16)PACKET_ID::ROOM_ENTER_RESPONSE;

	roomEnterResPacket.PacketLengeh = sizeof(ROOM_ENTER_RESPONSE_PACKET);

	roomEnterResPacket.Result = mRoomManager

}

void PacketManager::ProcessLeaveRoom(UINT32 clientIndex_, UINT16 packetSize_, char* pPacket_)
{

}

void PacketManager::ProcessRoomChatMessage(UINT32 clientIndex_, UINT16 packetSize_, char* pPacket_)
{
}
