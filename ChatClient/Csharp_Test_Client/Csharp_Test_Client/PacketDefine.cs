using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Csharp_Test_Client
{
    class PacketDef
    {
        public const Int16 PACKET_HEADER_SIZE = 5;

        public const int MAX_USER_ID_BYTE_LENGTH = 33;
        public const int MAX_USER_PW_BYTE_LENGTH = 33;
        public const int MAX_CHAT_MSG_SIZE = 257;
    }

    public enum PACKET_ID : ushort
    {
        DEV_ECHO = 1,

        // 로그인
        LOGIN_REQ = 201,
        LOGIN_RES = 202,

        // 룸입장
        ROOM_ENTER_REQ = 206,
        ROOM_ENTER_RES = 207,
        ROOM_NEW_USER_NTF=208,
        ROOM_USER_LIST_NTF=209,

        ROOM_LEAVE_REQ=215,
        ROOM_LEAVE_RES=216,
        ROOM_LEAVE_USER_NTF=217,

        // 룸 채팅
        ROOM_CHAT_REQ=221,
        ROOM_CHAT_RES=222,
        ROOM_CHAT_NOTIFY=223,

    }
}
