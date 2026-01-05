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

			if (auto task = TakeRequestTask(); task.
			{

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

