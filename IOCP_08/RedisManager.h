#pragma once

#include "RedisTaskDefine.h"

#include "../ThirdParty/CRedisConn.h"
#include <deque>
#include <thread>
#include <mutex>

class RedisManager
{
public:
	bool Run(std::string ip_,UINT16 port_,const	UINT32 threadCount_)
	{
		if (Connect(ip_, port_) == false)
		{
			printf("Redis 접속실패\n");
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

	RedisTask TakeResponseTask()
	{
		// Packet Manager 의 ProcessThread 에서 계속 확인하고 작동 중


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
		}
	}

	void TaskProcessThread()
	{
		printf(" Redis 의 스레드 시작 \n");

		while (mIsTaskRun)
		{
			bool isIdle = true;

			
			if (auto task = TakeRequestTask(); task.TaskID !=RedisTaskID::INVALID)
			{
				isIdle = false;

			
		
				if (task.TaskID == RedisTaskID::REQUEST_LOGIN)
				{
					auto pRequest = (RedisLoginReq*)task.pData;

					RedisLoginRes bodyData;

					bodyData.Result = (UINT16)ERROR_CODE::LOGIN_USER_INVALID_PW;

					std::string value;
					if (mConn.get(pRequest->UserID,value))
					{
						// Redis 서버 조회를 통해서 , 서버에 해당 유저의 ID 가 있으면, 저장되어있던 비밀번호를 value 변수 안에 채워넣는 것 

						if (value.compare(pRequest->UserPW) ==0)
						{
							// DB 값 과 입력값 비교 . 
							// 동일할때만 ERROR_CODE::NONE으로 변경
							bodyData.Result = (UINT16)ERROR_CODE::NONE;
						}

					}

					RedisTask resTask;

					// 이번에는 작업종류를 RESPONSE_LOGIN 으로 변경하고


				}

			}

		}
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


	// RedisCpp::CRedisConn 래퍼 객체
	RedisCpp::CRedisConn mConn;

	bool mIsTaskRun = false;

	std::vector<std::thread> mTaskThreads;

	std::mutex mReqLock;
	std::deque<RedisTask> mRequestTask;

	std::mutex mResLock;
	std::deque<RedisTask> mResponseTask;
};

