using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Csharp_Test_Client
{
    struct PacketData
    {
        public Int16 DataSize;
        public Int16 PacketID;
        public SByte Type;
        public byte[] BodyData;
    }

    public class LoginReqPacket
    {
        byte[] UserID = new byte[PacketDef.MAX_USER_ID_BYTE_LENGTH];

        byte[] UserPW = new byte[PacketDef.MAX_USER_PW_BYTE_LENGTH];

        public void SetValue(string userID, string userPW)
        {
            Encoding.UTF8.GetBytes(userID).CopyTo(UserID, 0);

            Encoding.UTF8.GetBytes(userPW).CopyTo(UserPW, 0);
        }

        public byte[] ToBytes()
        {
            List<byte> dataSource = new List<byte>();
            dataSource.AddRange(UserID);
            dataSource.AddRange(UserPW);
            return dataSource.ToArray();
        }

    }

    public class LoginResPacket
    {
        public UInt16 Result;

        public bool FromBytes(byte[] bodyData)
        {
           
            Result = BitConverter.ToUInt16(bodyData, 0);
            return true;
        }
    }

    public class RoomEnterReqPacket
    {
        int RoomNumber;
        
        public void SetValue(int roomNumber)
        {
            RoomNumber = roomNumber;
        }

        public byte[] ToBytes()
        {
            List<byte> dataSource = new List<byte>();

            dataSource.AddRange(BitConverter.GetBytes(RoomNumber));

            return dataSource.ToArray();
        }
    }

    public class RoomEnterResPacket
    {
        public UInt16 Result;
        public bool FromBytes(byte[] bodyData)
        {
            Result = BitConverter.ToUInt16(bodyData, 0);
            return true;
        }
    }

    public class RoomChatReqPacket
    {
        byte[] Msg = new byte[PacketDef.MAX_CHAT_MSG_SIZE];

        public void SetValue(string msg)
        {
            Encoding.UTF8.GetBytes(msg).CopyTo(Msg, 0);
        }

        public byte[] ToBytes()
        {
            List<byte> dataSource = new List<byte>();
            dataSource.AddRange(Msg);
            return dataSource.ToArray();
        }

    }

    public class RoomChatResPacket
    {
        public UInt16 Result;
        public bool FromBytes(byte[] bodyData)
        {
           
            Result = BitConverter.ToUInt16(bodyData, 0);
            return true;
        }
    }

    public class RoomChatNotifyPacket
    {
        byte[] UserID = new byte[PacketDef.MAX_USER_ID_BYTE_LENGTH];
        byte[] Msg = new byte[PacketDef.MAX_CHAT_MSG_SIZE];
        public bool FromBytes(byte[] bodyData)
        {
            Array.Copy(bodyData, 0, UserID, 0, PacketDef.MAX_USER_ID_BYTE_LENGTH);
            Array.Copy(bodyData, PacketDef.MAX_USER_ID_BYTE_LENGTH, Msg, 0, PacketDef.MAX_CHAT_MSG_SIZE);
            return true;
        }
        public string GetUserID()
        {
            return Encoding.UTF8.GetString(UserID).TrimEnd('\0');
        }
        public string GetMsg()
        {
            return Encoding.UTF8.GetString(Msg).TrimEnd('\0');
        }
    }

    public class UpdateClientScoreReqPacket
    {
        public int Score;
        
        public byte[] ToBytes()
        {
            List<byte> dataSource = new List<byte>();
            dataSource.AddRange(BitConverter.GetBytes(Score));
            return dataSource.ToArray();
        }
    }
    

}
