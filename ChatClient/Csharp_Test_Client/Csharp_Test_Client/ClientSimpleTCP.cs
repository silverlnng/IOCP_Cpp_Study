using System;
using System.Net;
using System.Net.Sockets;


//이 소스 코드는 C# 환경에서 TCP 프로토콜을 활용해 서버와 데이터를 주고받는 기초적인 네트워크 클라이언트 클래스의 구조를 보여줍니다.
//핵심적인 기능은 특정 IP 주소와 포트 번호를 통해 서버에 접속하고, 준비된 버퍼를 이용해 데이터를 송수신한 뒤 통신이 끝나면 안전하게 소켓을 닫는 일련의 과정을 포함합니다.
//특히 수신 과정에서 발생하는 블로킹(지연) 현상을 고려하여 설계되었으며, 작업 중 발생하는 예외 상황을 관리하기 위해 별도의 에러 메시지 저장 메커니즘을 갖추고 있습니다.
//전체적으로 이 코드는 안정적인 통신 상태를 유지하면서 외부 서버와 정보를 교환하기 위한 동기식 통신 인터페이스의 표준적인 형태를 제안합니다.


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

        // Receive() 함수 : 동기방식
        // MainForm 의 NetworkReadThread 에서 작동
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
