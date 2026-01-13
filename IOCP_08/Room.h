#pragma once

#include "UserManager.h"
#include "Packet.h"

#include <functional>

class Room
{
public:
	Room() = default;
	~Room() = default;

	INT32 GetMaxUserCount() {return mMaxUserCount;}

	INT32 GetCurUserCount() {return mCurUserCount;}

	INT32 GetRoomNumber() {return mRoomNum;}

	void Init(const INT32 roomIndex_,const INT32 maxUserCount_)
	{
		mRoomNum = roomIndex_;
		mMaxUserCount = maxUserCount_;
	}

	UINT16 EnterUser(User* user_)
	{
		if (mCurUserCount >= mMaxUserCount)
		{
			return (UINT16)ERROR_CODE::ENTER_ROOM_FULL_USER;
		}

		mUserList.push_back(user_);
		++mCurUserCount;

		user_->EnterRoom(mRoomNum);

		return (UINT16)ERROR_CODE::NONE;
	}

	void LeaveUser(User* leaveUser_)
	{
		mUserList.remove_if([ leaveUserID=leaveUser_->GetUserId()](User* pUser)
		{
			return leaveUserID == pUser->GetUserId();
		}
		);

		--mCurUserCount;
	}


private:

	INT32 mRoomNum = -1;

	std::list<User*> mUserList;

	INT32 mMaxUserCount = 0;

	UINT16 mCurUserCount = 0;
};