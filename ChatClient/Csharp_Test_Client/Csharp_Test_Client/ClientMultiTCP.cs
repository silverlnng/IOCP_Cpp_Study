using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;


// 대규모 테스트를 위한 멀티 TCP 클라이언트
// 기존의 방식을 비동기 로 변경

namespace Csharp_Test_Client
{
    class ClientMultiTCP
    {
        public Socket Sock = null;

        private PacketBufferManager _packetBuffer = new PacketBufferManager();
        private byte[] _tempBuffer = new byte[1024 * 2]; // 수신용 임시 버퍼

        // 완성된 패킷을 담아둘 큐 (mainForm의 구조 계승)
        public Queue<PacketData> RecvPacketQueue = new Queue<PacketData>();
        public object LockObj = new object();

        public string LatestErrorMsg;

        // 1. 비동기 접속 시작
        public bool Connect(string ip, int port)
        {
            try
            {
                IPAddress serverIP = IPAddress.Parse(ip);

                Sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

                // [추가] Nagle 알고리즘 비활성화 (즉시 전송)
                Sock.NoDelay = true;

                // 비동기 연결 시도
                Sock.BeginConnect(new IPEndPoint(serverIP, port), new AsyncCallback(ConnectCallback), Sock);
                return true;
            }
            catch (Exception ex)
            {
                LatestErrorMsg = ex.Message;
                return false;
            }
        }

        // 접속 완료 콜백
        private void ConnectCallback(IAsyncResult ar)
        {
            try
            {
                Socket client = (Socket)ar.AsyncState;
                client.EndConnect(ar);

                if (client.Connected)
                {
                    DevLog.Write("서버 접속 성공 (비동기)", LOG_LEVEL.INFO);
                    // 접속 성공 즉시 비동기 수신 시작
                    StartReceive();
                }
            }
            catch (Exception ex)
            {
                DevLog.Write($"접속 실패: {ex.Message}", LOG_LEVEL.ERROR);
            }
        }

        // 2. 비동기 수신 예약
        private void StartReceive()
        {
            try
            {
                if (IsConnected())
                {
                    // 데이터를 받으면 ReceiveCallback을 실행하도록 예약
                    Sock.BeginReceive(_tempBuffer, 0, _tempBuffer.Length, SocketFlags.None,
                                     new AsyncCallback(ReceiveCallback), Sock);
                }
            }
            catch (Exception ex)
            {
                DevLog.Write($"수신 예약 실패: {ex.Message}", LOG_LEVEL.ERROR);
            }
        }

        private void ReceiveCallback(IAsyncResult ar)
        {
            try
            {
                Socket client = (Socket)ar.AsyncState;
                int nRecv = client.EndReceive(ar);

                if (nRecv > 0)
                {
                    // 받은 데이터를 패킷 버퍼에 쓰기 [6]
                    _packetBuffer.Write(_tempBuffer, 0, nRecv);

                    // 버퍼에서 완성된 패킷들을 모두 꺼내기 [7, 8]
                    while (true)
                    {
                        var data = _packetBuffer.Read();
                        if (data.Count < 1) break;

                        var packet = new PacketData();
                        packet.DataSize = (short)(data.Count - PacketDef.PACKET_HEADER_SIZE);
                        packet.PacketID = BitConverter.ToInt16(data.Array, data.Offset + 2);
                        packet.Type = (SByte)data.Array[(data.Offset + 4)];
                        packet.BodyData = new byte[packet.DataSize];
                        Buffer.BlockCopy(data.Array, (data.Offset + PacketDef.PACKET_HEADER_SIZE),
                                         packet.BodyData, 0, packet.DataSize);

                        lock (LockObj)
                        {
                            RecvPacketQueue.Enqueue(packet);
                        }
                    }

                    // 다음 수신을 위해 다시 예약 (재귀적 반복)
                    StartReceive();
                }
                else
                {
                    Close(); // 서버에서 접속을 끊은 경우
                }
            }
            catch (Exception ex)
            {
                DevLog.Write($"수신 처리 오류: {ex.Message}", LOG_LEVEL.ERROR);
                Close();
            }
        }

        // 3. 비동기 송신
        public void Send(byte[] sendData)
        {
            try
            {
                if (IsConnected())
                {
                    Sock.BeginSend(sendData, 0, sendData.Length, SocketFlags.None,
                                  new AsyncCallback(SendCallback), Sock);
                }
            }
            catch (Exception ex)
            {
                DevLog.Write($"송신 오류: {ex.Message}", LOG_LEVEL.ERROR);
            }
        }

        private void SendCallback(IAsyncResult ar)
        {
            try
            {
                Socket client = (Socket)ar.AsyncState;
                client.EndSend(ar);
            }
            catch (Exception ex)
            {
                DevLog.Write($"송신 완료 처리 오류: {ex.Message}", LOG_LEVEL.ERROR);
            }
        }

        public void Close()
        {
            if (Sock != null && Sock.Connected)
            {
                //Sock.Shutdown(SocketShutdown.Both);

                // 비동기 수신처리도 종료하기 

                Sock.Close();
            }
        }

        public bool IsConnected() => (Sock != null && Sock.Connected);
    }
}
