using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

// 로그 남기기 용도

namespace Csharp_Test_Client
{
    public class DevLog
    {
        static System.Collections.Concurrent.ConcurrentQueue<string> logMsgQueue = new System.Collections.Concurrent.ConcurrentQueue<string>();

        static Int64 출력가능_로그레벨 = (Int64)LOG_LEVEL.TRACE;

        static public void ChangeLogLevel(LOG_LEVEL level)
        {
            Interlocked.Exchange(ref 출력가능_로그레벨, (int)level);
        }

        static public void Write()
        {

        }

    }

    public enum LOG_LEVEL : Int64
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        DISABLE,
    }

}
