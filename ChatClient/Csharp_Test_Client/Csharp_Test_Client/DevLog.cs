using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

// 로그 남기기 용도

namespace Csharp_Test_Client
{
    public class DevLog
    {
        static System.Collections.Concurrent.ConcurrentQueue<string> logMsgQueue = new System.Collections.Concurrent.ConcurrentQueue<string>();

        static Int64 출력가능_로그레벨 = (Int64)LOG_LEVEL.TRACE;

        static public void Init(LOG_LEVEL logLevel)
        {
            ChangeLogLevel(logLevel);
        }

        static public void ChangeLogLevel(LOG_LEVEL level)
        {
            Interlocked.Exchange(ref 출력가능_로그레벨, (int)level);
        }

        public static LOG_LEVEL CurrentLogLevel()
        {
            var  curLogLevel = (LOG_LEVEL)Interlocked.Read(ref 출력가능_로그레벨);

            return curLogLevel;
        }

        static public void Write(string msg,LOG_LEVEL logLevel = LOG_LEVEL.TRACE, [CallerFilePath] string fileName="",
            [CallerMemberName] string methodName="", [CallerLineNumber] int lineNumber= 0
            )
        {
            if(CurrentLogLevel() <=logLevel)
            {
                logMsgQueue.Enqueue(string.Format("{0} : {1} | {2}", DateTime.Now, methodName, msg));
            }
        }

    }

    public enum LOG_LEVEL : Int64
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        DISABLE
    }

}
