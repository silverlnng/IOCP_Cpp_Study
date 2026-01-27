using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Csharp_Test_Client
{
    //  클라이언트 애플리케이션에서 서버로부터 전달받은 네트워크 패킷을 효율적으로 분류하고 처리하기 위한 구조를 담고 있습니다.
    // 핵심 기법은 딕셔너리 자료구조를 활용하여 각 패킷의 고유 ID와 그에 대응하는 실행 함수를 연결함으로써, 수신된 데이터의 종류에 따라 적절한 로직이 자동으로 호출되도록 설계한 것입니다.
    // 로그인 결과 확인, 채팅 메시지 수신, 사용자 목록 업데이트와 같은 개별적인 이벤트별 처리 메서드들이 정의되어 있어, 복잡한 통신 데이터를 체계적으로 관리하고 사용자 인터페이스에 반영하는 메시지 라우팅 시스템의 역할을 수행합니다.
    public partial class MainForm
    {
        Dictionary<PACKET_ID, Action<byte[]>> PacketFuncDic = new Dictionary<PACKET_ID, Action<byte[]>>();

        void SetPacketHandler()
        {
            PacketFuncDic.Add(PACKET_ID.DEV_ECHO, PacketProcess_DevEcho);
        }

        void PacketProcess(PacketData packet)
        {
            var packetType = (PACKET_ID)packet.PacketID;

            if (PacketFuncDic.ContainsKey(packetType))
            {
                PacketFuncDic[packetType](packet.BodyData);
            }
            else
            {

            }
        }

        void PacketProcess_DevEcho(byte[] bodyData)
        {
            // [수정 전] 문자열 출력
            //DevLog.Write($"Echo:  {Encoding.UTF8.GetString(bodyData)}");

            // [수정 후] 타임스탬프 계산 및 Latency 출력
            try
            {
                // 1. Body 데이터 길이가 8바이트(Int64)인지 확인 (방어 코드)
                if (bodyData.Length >= 8)
                {
                    // 2. 패킷에 담겨 있던 '보낸 시간' 꺼내기
                    long sendTicks = BitConverter.ToInt64(bodyData, 0);

                    // 3. '현재 시간' 구하기
                    long currentTicks = DateTime.Now.Ticks;

                    // 4. 차이 계산 (10,000 Ticks = 1ms)
                    TimeSpan elapsedSpan = new TimeSpan(currentTicks - sendTicks);
                    double latencyMs = elapsedSpan.TotalMilliseconds;

                    // 5. 로그 출력
                    // 주의: 수천 명이 동시에 받으면 로그창이 멈출 수 있으므로, 
                    // 특정 조건(예: Latency가 100ms 이상 튀었을 때)에만 찍거나 
                    // 별도 카운터로 평균을 내는 것이 좋습니다.

                    // 예시: 무조건 출력
                    DevLog.Write($"[Latency] {latencyMs:F4} ms");
                }
                else
                {
                    // 기존 문자열 방식과 섞여 있을 경우를 대비
                    DevLog.Write($"Echo(Text): {Encoding.UTF8.GetString(bodyData)}");
                }
            }
            catch (Exception ex)
            {
                DevLog.Write($"Echo Process Error: {ex.Message}", LOG_LEVEL.ERROR);
            }
        }

        void PacketProcess_LoginResponse(byte[] bodyData)
        {

        }

        void PacketProcess_RoomChatResponse(byte[] bodyData)
        {
            var resPacket = new RoomChatResPacket();
            resPacket.FromBytes(bodyData);

            var errorCode = (ERROR_CODE)resPacket.Result;

            var msg = $"방 채팅 요청 결과: {(ERROR_CODE)resPacket.Result}";

            if(errorCode== ERROR_CODE.ERROR_NONE)
            {
                DevLog.Write(msg, LOG_LEVEL.ERROR);
            }
            else
            {
                
            }


        }

        void AddRoomChatMessageList(string userID, string msg)
        {

        }

    }
}