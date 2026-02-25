#pragma once

#include "RedisTaskDefine.h"

#include "../ThirdParty/CRedisConn.h"
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>

class RedisManager
{
public:
	bool Run(const	UINT32 threadCount_)
	{
		if (ConnectWithSentinel() == false)
		{
			printf("[Redis] 초기 마스터 접속 실패 (Sentinel 응답 없음)\n");
			return false;
		}

		mIsTaskRun = true;

		for (UINT32 i = 0 ; i<threadCount_;i++)
		{
			mTaskThreads.emplace_back([this]()
				{TaskProcessThread(); });
		}

		printf("Redis 동작 중 \n");

		return true;
	}

	void End()
	{
		mIsTaskRun = false;
		// [추가] 모든 대기 중인 스레드를 깨움
		mTaskCond.notify_all();
		for (auto& thread : mTaskThreads)
		{
			if (thread.joinable())
			{
				thread.join();
			}
		}
		printf("Redis thread 종료 \n");
	}

	void PushTask(RedisTask task_)
	{
		// [수정] PushTask: 데이터를 넣고 스레드를 깨움 (Producer)
		{
			std::lock_guard<std::mutex> guard(mReqLock);
			mRequestTask.push_back(task_);
		}
		mTaskCond.notify_one(); // [추가] 대기 중인 스레드 하나를 깨움
	}

	// Packet Manager 의 ProcessThread 에서 계속 확인하고 작동 중
	RedisTask TakeResponseTask()
	{
		std::lock_guard<std::mutex> guard(mResLock);

		if(mResponseTask.empty())
		{
			return RedisTask();
		}

		auto task = mResponseTask.front();
		mResponseTask.pop_front();

		return task;
	}

private:
	bool Connect(std::string ip_,UINT16 port_)
	{
		if (mConn.connect(ip_,port_) ==false)
		{
			std::cout << "Redis Connect Error" << mConn.getErrorStr() << std::endl;

			return false;
		}
		else
		{
			std::cout << "Redis Connect success" << std::endl;
			return true;
		}
	}

	void TaskProcessThread()
	{
		printf(" Redis 의 스레드 시작 \n");

		// TODO : 대기 방식을 폴링방식에서 이벤트 기반으로 수정하기 
		// Sleep for 대신 wait 사용

		while (mIsTaskRun)
		{
			RedisTask task;

			// [블록 시작] 락을 걸고 조건 검사
			{
				std::unique_lock<std::mutex> lock(mReqLock); // wait을 쓰려면 unique_lock 필요

				// 큐가 비어있으면 깨울 때까지 잠듦 (Spurious Wakeup 방지)
				mTaskCond.wait(lock, [this]() {
					return !mIsTaskRun || !mRequestTask.empty();
					});

				if (!mIsTaskRun) break;

				// 데이터 꺼내기
				task = mRequestTask.front();
				mRequestTask.pop_front();
			} // [블록 끝] 여기서 락 해제. 데이터 처리는 락 없이 진행

			// 작업 종류에 따라서 처리하기
			if (task.TaskID == RedisTaskID::REQUEST_UPDATE_SCORE)
			{
				auto pRequest = (RedisUpdateScoreReq*)task.pData;

				// Redis ZADD 커맨드: ZADD key score member
				// 예: ZADD "GameRanking" 1500 "User_1"


				// CRedisConn 래퍼에 Sorted Set(정렬된 집합) 을 에 추가하는 함수 zadd를 구현
				bool success = mConn.zadd("GameRanking", pRequest->Score, pRequest->UserID);

				// [전략적 수정] 명령 실행이 실패하면 재접속 로직을 가동
				if (!success)
				{
					std::cout << "[Redis] 명령 실패! 마스터 장애 의심. 재접속 시도..." << std::endl;

					// 센티널을 통해 마스터 재탐색 (성공할 때까지 혹은 일정 횟수 반복)
					int retry = 0;
					while (retry < 5) {
						if (ConnectWithSentinel()) break;
						std::this_thread::sleep_for(std::chrono::seconds(1)); // 센티널 투표 시간 대기
						retry++;
					}

					// 재접속 후 다시 시도
					mConn.zadd("GameRanking", pRequest->Score, pRequest->UserID);
				}


				// 예시 (가상 코드):
				std::cout << "[Redis] 랭킹 업데이트 요청 (ZADD GameRanking "
					<< pRequest->Score << " " << pRequest->UserID << ")" << std::endl;

				// 실제 구현된 라이브러리에 맞게 호출 (예: cpp_redis 등)
				// mConn.sendCommand("ZADD", "GameRanking", std::to_string(pRequest->Score), pRequest->UserID);

				task.Release(); // 메모리 해제 필수 [10]
			}

			
		}

		printf(" Redis 의 스레드 종료 \n");
	}

	RedisTask TakeRequestTask()
	{
		std::lock_guard<std::mutex> quard(mReqLock);

		if (mRequestTask.empty())
		{
			return RedisTask();
		}

		auto task = mRequestTask.front();
		mRequestTask.pop_front();

		return task;
	}

	void PushResponse(RedisTask task_)
	{
		std::lock_guard<std::mutex> guard(mResLock);
		mResponseTask.push_back(task_);	
	}

	// [핵심] 센티널들에게 돌아가며 물어봐서 현재 마스터 주소를 가져옵니다.
	bool ConnectWithSentinel()
	{
		for (const auto& s : mSentinels) {
			// CRedisConn의 도구를 사용하여 주소를 알아옴
			auto [masterIp, masterPort] = mConn.GetMasterAddrFromSentinel(s.ip, s.port, "mymaster");

			if (!masterIp.empty()) {
				// 2. 센티널이 대답해준 주소를 정확히 출력
				std::cout << "[Manager] ★ 센티널 응답 수신 ★ -> Master IP: [" << masterIp << "], Port: [" << masterPort << "]" << std::endl;

				// 3. 실제 마스터와 연결 시도 로그
				std::cout << "[Manager] 해당 마스터로 접속 시도 중..." << std::endl;
				if (mConn.connect(masterIp, masterPort)) {
					std::cout << "[Manager] 마스터 접속 성공!! (" << masterIp << ":" << masterPort << ")" << std::endl;
					return true;
				}
				else {
					// 연결 실패 시 원인 파악을 위해 에러 로그 출력 (CRedisConn에 getErrorStr이 있다고 가정)
					std::cout << "[Manager] ※ 마스터 접속 실패 (" << masterIp << ":" << masterPort << ")" << std::endl;
				}
			}
			else {
				std::cout << "[Manager] 센티널(" << s.ip << ")이 마스터 정보를 알고 있지 않거나 응답이 없습니다." << std::endl;
			}
		}
		std::cout << "[Manager] 모든 센티널 시도 종료. 연결 가능한 마스터가 없습니다." << std::endl;
		return false;
	}

	// RedisCpp::CRedisConn 래퍼 객체
	RedisCpp::CRedisConn mConn;

	// 센티널 정보를 담을 간단한 구조체
	struct SentinelAddr {
		std::string ip;
		uint16_t port;
	};

	std::vector<SentinelAddr> mSentinels= {
	{"127.0.0.1", 26379}, // sentinel-1
	{"127.0.0.1", 26380}, // sentinel-2
	{"127.0.0.1", 26381}  // sentinel-3
	}; // 센티널 목록
	

	bool mIsTaskRun = false;

	std::vector<std::thread> mTaskThreads;

	std::mutex mReqLock;
	std::deque<RedisTask> mRequestTask;

	std::mutex mResLock;
	std::deque<RedisTask> mResponseTask;

	// [추가] 조건 변수 선언
	std::condition_variable mTaskCond;
};

