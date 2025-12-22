using System;
using System.Net;
using System.Net.Sockets;

namespace Csharp_Test_Client
{
    class ClientSimpleTCP
    {
        public Socket Sock = null;
        public string LatestErrorMsg;

        //소켓 연결
        public bool Connect(string ip, int port)
        {
            try
            {
                // 받은 ip string 주소를 IPAddress 객체로 파싱
                IPAddress serverIP = IPAddress.Parse(ip);

                int serverPort = port;

                // 1. 소켓 생성
                Sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

                // 2. 서버의 엔드포인트(IP 주소와 포트) 지정 + 연결시도
                // connect 함수 : 동기메서드 . 연결이 완료될때까지 블로킹
                Sock.Connect(new IPEndPoint(serverIP, serverPort));


                if (Sock == null || Sock.Connected==false)
                {
                    return false;
                }
                return true;
            }
            catch (Exception ex)
            {
                // 예외 발생시 오류메시지 저장
                LatestErrorMsg = ex.Message;
                return false;
            }
        }

        public Tuple<int, byte[]> Receive()
        {
            try
            {
                byte[] ReadBuffer = new byte[2048];

                var nRecv = Sock.Receive(ReadBuffer, 0, ReadBuffer.Length, SocketFlags.None);

                if(nRecv == 0)
                {
                    return null;
                }
                return Tuple.Create(nRecv, ReadBuffer);
            }
            catch(SocketException se)
            {
                LatestErrorMsg = se.Message;
            }

            return null;
        }

        public void Send(byte[] sendData)
        {
            try
            {
                // 연결되어있으면
                if (Sock != null && Sock.Connected)
                {
                    Sock.Send(sendData,0,sendData.Length,SocketFlags.None);
                }
                else 
                {
                    LatestErrorMsg = "먼저 채팅서버에 접속하세요! ";
                }

            }
            catch (SocketException se)
            {
                LatestErrorMsg = se.Message;
            }
        }

        public bool IsConnected()
        {
            // 소켓이 있고 , 연결되어있으면 true
            return (Sock != null && Sock.Connected)?
                true:false;
        }
    }
}
