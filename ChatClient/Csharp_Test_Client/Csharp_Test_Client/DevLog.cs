using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

// 로그 남기기 용도

// 제공된 코드는 C# 기반의 개발 로그 시스템을 구현한 것으로, DevLog라는 정적 클래스를 중심으로 작동합니다.
// 이 시스템의 핵심은 ConcurrentQueue를 사용하여 여러 스레드에서 안전하게 로그 메시지를 축적하는 것입니다.
// 로그는 LOG_LEVEL 열거형을 통해 세분화된 심각도(TRACE, DEBUG 등)를 가지며,
// 출력가능_로그레벨 변수를 기준으로 현재 설정된 레벨보다 심각한 로그만 큐에 추가되도록 제어됩니다.
// 또한 Write 함수는 로그 메시지뿐만 아니라 호출된 파일 경로, 메서드 이름, 줄 번호와 같은 호출자 정보를 자동으로 기록하는 기능을 포함하고 있어 개발자들이 디버깅 시 유용하게 사용할 수 있습니다.

namespace Csharp_Test_Client
{
    public class DevLog
    {
        // ConcurrentQueue<string> : 안전한 스레드 큐 . 여러 스레드에세 동시에 안전하게 문자열을 추가하고 ,제거 할수있고 내부적을 동기화를 처리. 데이터 손실없이 선입선출 방식으로 데이터 관리 
        // ConcurrentQueue<string> 를 사용하여 스레드 안전한 로그 메시지 큐 구현

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
            // 
            if(CurrentLogLevel() <=logLevel)
            {
                logMsgQueue.Enqueue(string.Format("{0} : {1} | {2}", DateTime.Now, methodName, msg));
            }
        }

        static public bool GetLog(out string msg)
        {
            if(logMsgQueue.TryDequeue(out msg))
            {
                return true;
            }
            return false;
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
