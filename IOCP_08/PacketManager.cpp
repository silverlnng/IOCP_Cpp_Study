#include <utility>
#include <chrono>
#include <fstream>

#include "UserManager.h"
#include "PacketManager.h"
#include "RedisManager.h"
#include "MySQLManager.h"

void PacketManager::Init(const UINT32 maxClient_)
{
	mRecvFuntionalDictionary = std::unordered_map<int, PROCESS_RECV_PACKET_FUNCTION>();

	// 멤버함수의 메모리 주소 (value) 에 저장

	mRecvFuntionalDictionary[(int)PACKET_ID::SYS_USER_CONNECT] = &PacketManager::ProcessUserConnect;
	mRecvFuntionalDictionary[(int)PACKET_ID::SYS_USER_DISCONNECT] = &PacketManager::ProcessUserDisConnect;
	
	mRecvFuntionalDictionary[(int)PACKET_ID::DEV_ECHO_REQUEST] = &PacketManager::ProcessDevEcho;


	mRecvFuntionalDictionary[(int)PACKET_ID::LOGIN_REQUEST] = &PacketManager::ProcessLogin;


	mRecvFuntionalDictionary[(int)RedisTaskID::RESPONSE_LOGIN] = &PacketManager::ProcessLoginDBResult;
	mRecvFuntionalDictionary[(int)MySQLTaskID::RESPONSE_LOGIN] = &PacketManager::ProcessLoginDBResult;

	mRecvFuntionalDictionary[(int)PACKET_ID::ROOM_ENTER_REQUEST] = &PacketManager::ProcessEnterRoom;

	mRecvFuntionalDictionary[(int)PACKET_ID::UPDATE_CLIENT_SCORE_REQUEST] = &PacketManager::ProcessUpdateScore;


	// 패킷매니저에서 시작하면서 유저매니저를 생성하고 Init 실행시킴
	CreateComponent(maxClient_);

	mRedisManager = new RedisManager;
	mMySQLManager = new MySQLManager;
}

void PacketManager::CreateComponent(const UINT32 maxClient_)
{
	mUserManager = new UserManager;
	mUserManager->Init(maxClient_);


}



bool PacketManager::Run()
{
	// MySQL 서버와의 연결하기
	if(mMySQLManager->Run("tcp://127.0.0.1:3306", "root", "1234",1)==false)
	{
		//return false;
	}
	// Redis 서버와의 연결하기
	if (mRedisManager->Run("127.0.0.1",6379,1)==false)
	{
		//return false;	
	}



	mIsRunProcessThread = true;
	mProcessThread = std::thread([this]() {ProcessPacket(); });

	return true;
}

void PacketManager::End()
{
	mRedisManager->End();

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

// 스레드에서 작동하는 함수
// 패킷이 들어오는걸 검사하고 , 처리
void PacketManager::ProcessPacket()
{
	// 시간 측정을 위한 변수 선언
	auto startTime = std::chrono::high_resolution_clock::now();
	int packetCount = 0;
	int totalPacketCount = 0;
	int totalElapsedSec = 0;

	// [추가] CSV 파일 열기 (덮어쓰기 모드)
	// Lock-Free 테스트 시에는 파일명을 "tps_log_lockfree.csv"로 변경하세요.
	std::ofstream tpsFile("tps_log_lock_based.csv");

	// [추가] CSV 헤더 작성 (엑셀에서 X축, Y축 인식용)
	if (tpsFile.is_open())
	{
		tpsFile << "Time(Sec),TPS,TotalPacketCount" << std::endl;
	}

	while (mIsRunProcessThread)
	{
		bool  isIdle = true;
		// 패킷의 기본생성자들은 PacketId 을 0 으로 채움

		if (auto packetData = DequePacketData(); packetData.PacketId > (UINT16)PACKET_ID::SYS_END)
		{
			isIdle = false;

			ProcessRecvPacket(packetData.ClientIndex, packetData.PacketId, packetData.DataSize, packetData.pDataPtr);
			packetCount++; // 처리 횟수 증가
		}

		if (auto packetData = DequeSystemPacketData(); packetData.PacketId!=0)
		{
			isIdle = false;

			ProcessRecvPacket(packetData.ClientIndex, packetData.PacketId, packetData.DataSize, packetData.pDataPtr);
			packetCount++; // 처리 횟수 증가
		}

		// Redis 의 응답 Task 확인하고 가져오기 추가
		if (auto task = mRedisManager->TakeResponseTask(); task.TaskID != RedisTaskID::INVALID)
		{
			isIdle = false;
			
			// ProcessRecvPacket 는 동기방식으로 작동하므로, pData 를 다 사용한 다음 Release() 호출해서 메모리 해제	
			ProcessRecvPacket(task.UserIndex, (UINT16)task.TaskID, task.DataSize, task.pData);

			// 다 사용했으니까 pData 메모리 해제
			task.Release();
		}

		// MySQL 의 응답 Task 확인하고 가져오기 추가
		if (auto task = mMySQLManager->TakeResponseTask(); task.TaskID != MySQLTaskID::INVALID)
		{
			isIdle = false;

			ProcessRecvPacket(task.UserIndex, (UINT16)task.TaskID, task.DataSize, task.pData);

			// 다 사용했으니까 pData 메모리 해제
			task.Release();
		}

		// TPS 출력 로직
		auto endTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = endTime - startTime;
		if (elapsed.count() >= 1.0) // 1초가 지났다면
		{
			totalElapsedSec++; // 경과 시간 증가
			totalPacketCount += packetCount;
			// 콘솔 출력 (실시간 확인용)
			printf("[TPS Measure] Time: %d sec, TPS: %d ,total TPS: %d \n", totalElapsedSec, packetCount, totalPacketCount);

			// 파일 출력 (포트폴리오 그래프용)
			if (tpsFile.is_open())
			{
				// 형식: 시간,처리량
				tpsFile << totalElapsedSec << "," << packetCount<<","<< totalPacketCount<< std::endl;
			}

			packetCount = 0; // 카운터 초기화
			startTime = std::chrono::high_resolution_clock::now(); // 시간 초기화
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
	printf("[ProcessUserDisConnect] clientIndex : %d\n", clientIndex_);

}

void PacketManager::ProcessDevEcho(UINT32 clientIndex_, UINT16 packetSize_, char* pPacket_)
{
	// TPS 측정용이므로 단순히 아무 작업도 안 하거나,
	// 클라이언트에 다시 응답을 보내주려면 아래와 같이 작성 (부하 테스트시에는 생략 가능)

	
	// 예: 받은 그대로 다시 전송 (Echo)
	SendPacketFunc(clientIndex_, packetSize_, pPacket_);
	
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
	loginResPacket.PacketLength = sizeof(LOGIN_RESPONSE_PACKET);



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
		
		/*RedisLoginReq dbReq;
		CopyMemory(dbReq.UserID, pLoginReqPacket->UserID, (MAX_USER_ID_LEN + 1));
		CopyMemory(dbReq.UserPW, pLoginReqPacket->UserPW, (MAX_USER_PW_LEN + 1));

		RedisTask task;
		task.UserIndex = clientIndex_;
		task.TaskID = RedisTaskID::REQUEST_LOGIN;
		task.DataSize = sizeof(RedisLoginReq);
		task.pData = new char[task.DataSize];
		CopyMemory(task.pData, (char*)&dbReq, task.DataSize);
		mRedisMgr->PushTask(task);*/


	
		// TODO : MySQL 으로 변경해주기

		MySQLLoginReq dbReq;
		CopyMemory(dbReq.UserID, pLoginReqPacket->UserID, (MAX_USER_ID_LEN + 1));
		CopyMemory(dbReq.UserPW, pLoginReqPacket->UserPW, (MAX_USER_PW_LEN + 1));

		MySQLTask task;
		task.UserIndex = clientIndex_;
		task.TaskID = MySQLTaskID::REQUEST_LOGIN;
		task.DataSize = sizeof(MySQLLoginReq);
		task.pData = new char[task.DataSize];

		CopyMemory(task.pData, (char*)&dbReq, task.DataSize);

		// 주의 :  MySQLTask의 char* pData 는 얕은복사가 됨
		mMySQLManager->PushTask(task);

		printf("[PacketManager::ProcessLogin] MySQL Login Request PushTask UserID : %s \n", pUserID);

		//task.Release();=> 여기서는 Release 하면 안됨 . MySQLManager 쪽에서 처리 후 Release 해야함
		//왜? => MySQLManager 쪽에서 TakeResponseTask 할때 pData 가 날아가버림
		// pData 가 new char[] 로 할당된 메모리이기 때문에 MySQLManager 쪽에서 처리가 끝난 후에 Release 해야함
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

	auto pBody = (MySQLLoginRes*)pPacket_;

	if(pBody->Result == (UINT16)ERROR_CODE::NONE)
	{
		// [여기가 핵심] 로그인 성공했으므로 매핑 테이블에 등록!
		// 이제 1. UserManager의 Dictionary에 ID가 등록되고
		// 2. User 객체 상태가 LOGIN으로 변경됨
		mUserManager->AddUser(pBody->UserID, clientIndex_);

		printf("[Login Success] User: %s (ClientIdx: %d)\n", pBody->UserID, clientIndex_);
	}
	else
	{
		printf("[Login Failed] ClientIdx: %d, ErrorCode: %d\n", clientIndex_, pBody->Result);
	}


	LOGIN_RESPONSE_PACKET loginResPacket;
	loginResPacket.PacketId = (UINT16)PACKET_ID::LOGIN_RESPONSE;
	loginResPacket.PacketLength = sizeof(LOGIN_RESPONSE_PACKET);
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

	roomEnterResPacket.PacketLength = sizeof(ROOM_ENTER_RESPONSE_PACKET);

	//roomEnterResPacket.Result = mRoomManager

}

void PacketManager::ProcessLeaveRoom(UINT32 clientIndex_, UINT16 packetSize_, char* pPacket_)
{

}

void PacketManager::ProcessRoomChatMessage(UINT32 clientIndex_, UINT16 packetSize_, char* pPacket_)
{
}

void PacketManager::ProcessUpdateScore(UINT32 clientIndex_, UINT16 packetSize_, char* pPacket_)
{
	// 1. 요청한 유저 확인 (로그인 상태인지 검증)
	auto pUser = mUserManager->GetUserByConnIdx(clientIndex_);
	if (pUser == nullptr || pUser->GetDomainState() != User::DOMAIN_STATE::LOGIN)
	{
		// 로그인하지 않은 유저의 요청은 무시하거나 에러 로그 출력
		return;
	}

	// 2. 패킷 파싱 (Packet.h에 정의된 구조체로 캐스팅)
	// struct UPDATE_CLIENT_SCORE_REQUEST_PACKET : public PACKET_HEADER { INT32 Score; };
	auto pScorePacket = reinterpret_cast<UPDATE_CLIENT_SCORE_REQUEST_PACKET*>(pPacket_);

	INT32 newScore = pScorePacket->NewScore;
	std::string userID = pUser->GetUserId();

	// 로그 출력 (디버깅용)
	printf("[Score Request] UserID: %s, Score: %d\n", userID.c_str(), newScore);

	// =================================================================
	// 3. MySQL Task 생성 및 요청 (DB에 점수 기록)
	// =================================================================
	{
		MySQLUpdateScoreReq sqlReq;
		// UserID 복사 (메모리 안전을 위해 초기화 후 복사)
		ZeroMemory(sqlReq.UserID, sizeof(sqlReq.UserID));
		CopyMemory(sqlReq.UserID, userID.c_str(), userID.length());
		sqlReq.Score = newScore;

		MySQLTask sqlTask;
		sqlTask.UserIndex = clientIndex_;
		sqlTask.TaskID = MySQLTaskID::REQUEST_UPDATE_SCORE; // 2001
		sqlTask.DataSize = sizeof(MySQLUpdateScoreReq);

		// [중요] Task 처리를 위한 별도 메모리 할당 (MySQLManager가 처리 후 해제함)
		sqlTask.pData = new char[sqlTask.DataSize];
		CopyMemory(sqlTask.pData, (char*)&sqlReq, sqlTask.DataSize);

		// 큐에 넣기
		mMySQLManager->PushTask(sqlTask);
	}

	// =================================================================
	// 4. Redis Task 생성 및 요청 (실시간 랭킹 업데이트)
	// =================================================================
	{
		RedisUpdateScoreReq redisReq;
		ZeroMemory(redisReq.UserID, sizeof(redisReq.UserID));
		CopyMemory(redisReq.UserID, userID.c_str(), userID.length());
		redisReq.Score = newScore;

		RedisTask redisTask;
		redisTask.UserIndex = clientIndex_;
		redisTask.TaskID = RedisTaskID::REQUEST_UPDATE_SCORE; // 2001
		redisTask.DataSize = sizeof(RedisUpdateScoreReq);

		// [중요] Task 처리를 위한 별도 메모리 할당 (RedisManager가 처리 후 해제함)
		redisTask.pData = new char[redisTask.DataSize];
		CopyMemory(redisTask.pData, (char*)&redisReq, redisTask.DataSize);

		// 큐에 넣기
		mRedisManager->PushTask(redisTask);
	}
}