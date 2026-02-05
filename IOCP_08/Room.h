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

	void NotifyChat(INT32 clientIndex_,const char* userID_,const char* msg_)
	{
		ROOM_CHAT_NOTIFY_PACKET roomChatNotifyPacket;
		
		roomChatNotifyPacket.PacketId = (UINT16)PACKET_ID::ROOM_CHAT_NOTIFY;
		roomChatNotifyPacket.PacketLength = sizeof(roomChatNotifyPacket);

		CopyMemory(roomChatNotifyPacket.Msg, msg_, sizeof(roomChatNotifyPacket.Msg));
		CopyMemory(roomChatNotifyPacket.UserID, userID_, sizeof(roomChatNotifyPacket.UserID));
		SendToAllUsers(sizeof(roomChatNotifyPacket), (char*)&roomChatNotifyPacket, clientIndex_, false);

	}

	std::function<void(UINT32, UINT16, char*)> SendPacketFunc;

private:

	void SendToAllUsers(const UINT16 dataSize_,char* data_,const INT32 passUserIndex,bool exceptMe)
	{
		for (auto& pUser : mUserList)
		{
			if (pUser == nullptr) { continue; }

			if(exceptMe && pUser->GetNetConnIdx() == passUserIndex)
			{
				continue;
			}

			SendPacketFunc((UINT32)pUser->GetNetConnIdx(), (UINT32)dataSize_, data_);
		}
	}

	INT32 mRoomNum = -1;

	std::list<User*> mUserList;

	INT32 mMaxUserCount = 0;

	UINT16 mCurUserCount = 0;
};