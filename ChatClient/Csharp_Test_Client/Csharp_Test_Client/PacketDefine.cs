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

    }

    public enum PACKET_ID : ushort
    {
        DEV_ECHO = 1
    }
}
