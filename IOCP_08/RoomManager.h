#pragma once
#include "Room.h"

class RoomManager
{
public:
	RoomManager() = default;
	~RoomManager() = default;
	
	void Init(const INT32 beginRoomNumber_, const INT32 maxRoomCount_, const INT32 maxRoomUserCount_)
	{
		mBeginRoomNumber = beginRoomNumber_;
		mMaxRoomCount = maxRoomCount_;
		mEndRoomNumber = beginRoomNumber_ + maxRoomCount_;

		mRoomList = std::vector<Room*>(maxRoomCount_);

		for (auto i =0;i<maxRoomCount_;i++)
		{
			mRoomList[i] = new Room();
			mRoomList[i]->SendPacketFunc = SendPacketFunc;
			mRoomList[i]->Init((beginRoomNumber_ + i), maxRoomUserCount_);
		}

	}

	UINT GetMaxRoomCount()
	{
		return mMaxRoomCount;
	}

	UINT16 EnterUser(INT32 roomNumber_,User* user_)
	{
		auto pRoom = GetRoomByRoomNumber(roomNumber_);
		if(pRoom == nullptr)
		{
			return (UINT16)ERROR_CODE::ROOM_INVALID_INDEX;
		}
		return pRoom->EnterUser(user_);
	}

	INT16 LeaveUser(INT32 roomNumber_,User* user_)
	{
		auto pRoom = GetRoomByRoomNumber(roomNumber_);
		if (pRoom == nullptr)
		{
			return (UINT16)ERROR_CODE::ROOM_INVALID_INDEX;
		}
		user_->SetDomainState(User::DOMAIN_STATE::LOGIN);
		pRoom->LeaveUser(user_);
		return (UINT16)ERROR_CODE::NONE;
	}

	Room* GetRoomByRoomNumber(const INT32 roomNumber_)
	{
		if (roomNumber_ < mBeginRoomNumber || roomNumber_ >= mEndRoomNumber)
		{
			return nullptr;
		}
		INT32 roomIndex = roomNumber_ - mBeginRoomNumber;
		return mRoomList[roomIndex];
	}

	std::function<void(UINT32, UINT16, char*)> SendPacketFunc;

private:
	std::vector<Room*> mRoomList;
	INT32 mBeginRoomNumber = 0;
	INT32 mMaxRoomCount = 0;
	INT32 mEndRoomNumber = 0;
};