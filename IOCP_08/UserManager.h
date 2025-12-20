#pragma once

#include <vector>
#include "User.h"

class UserManager
{
public:

	void Init(const INT32 maxUserCount_)
	{
		mMaxUserCnt = maxUserCount_;

		// 벡터의 크기를 MaxUser 만큼 미리 확보
		mUserObjPool = std::vector<User*>(mMaxUserCnt);

		for (auto i = 0; i < mMaxUserCnt; i++)
		{
			// 실제 User 객체 미리 생성 (Pooling)
			// 유저가 접속할때 마다 new User() 을 하는게 아님 !!
			mUserObjPool[i] = new User();

			//mUserObjPool[i]->Init
		}
	}

	void DeleteUserInfo(User* user_)
	{

		// 유저가 연결을 끊어도 파괴(delete)가 아니라 내부데이터만 초기화 하여서 재사용
		//user_->Clear();
	}

	User* GetUserByConnIdx(INT32 clientIndex_)
	{
		// 유저 접속시 // 검색 없이 O(1) 접근
		return mUserObjPool[clientIndex_];
	}

private: 

	INT32 mMaxUserCnt = 0;
	INT32 mCurrentUserCnt = 0;

	// Object Pool 패턴 사용으로 서버 접속자 관리.
	std::vector<User*> mUserObjPool;
};