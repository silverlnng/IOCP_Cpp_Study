
using System.Text;

namespace Csharp_Test_Client;


public partial class MainForm : Form
{
    ClientSimpleTCP Network = new ClientSimpleTCP();

    bool IsNetworkThreadRunning = false;
    bool IsBackGroundProcessRunning = false;

    System.Threading.Thread NetworkReadThread = null;

    System.Threading.Thread NetworkSendThread = null;

    PacketBufferManager PacketBuffer = new PacketBufferManager();

    Queue<PacketData> RecvPacketQueue = new Queue<PacketData>();

    Queue<byte[]> SendPacketQueue = new Queue<byte[]>();

    System.Windows.Forms.Timer dispatcherUITimer;

    public MainForm()
    {
        InitializeComponent();
    }

    // MainForm_Load 함수 : 폼이 메모리에 로드되어 화면에 나타나기 직전에 자동으로 실행되는 함수
    private void MainForm_Load(object sender, EventArgs e)
    {
        // 패킷버퍼 초기화

        // 스레드 시작

        IsNetworkThreadRunning=true;

        NetworkReadThread = new System.Threading.Thread(this.NetworkReadProcess);
        NetworkReadThread.Start();

        NetworkSendThread = new System.Threading.Thread(this.NetworkSendProcess);
        NetworkSendThread.Start();

        // UI 타이머 가동 :  dispatcherUITimer를 생성하고 0.1초 간격으로 BackGroundProcess가 실행되도록 설정합니다. 이는 수신된 패킷을 처리하고 UI 로그를 남기는 역할을 합니다.

        IsBackGroundProcessRunning=true;

        dispatcherUITimer = new System.Windows.Forms.Timer();

        dispatcherUITimer.Tick += new EventHandler(BackGroundProcess);

        dispatcherUITimer.Interval = 100;
        dispatcherUITimer.Start();

        BtnDisconnect.Enabled = false;

    }


    // 접속하기 버튼 클릭 함수
    private void BtnConnect_Click(object sender, EventArgs e)
    {
        string address = textBoxIP.Text;

        // 로컬 호스트 ip 사용이 되어있으면 그걸로 사용
        if (checkBoxLocalHostIP.Checked)
        {
            address = "127.0.0.1";
        }

        int port = Convert.ToInt32(textBoxPort.Text);

        // UI 의 ip,port 텍스트를 가져와서 
        if (Network.Connect(address, port))
        {
            labelStatus.Text = string.Format("{0}.서버에 접속 중", DateTime.Now);

            BtnConnect.Enabled = false;
            BtnDisconnect.Enabled = true;

            //DevLog 에 로그 남기기

        }
        else
        {
            labelStatus.Text = string.Format("{0}, 서버에 접속 실패", DateTime.Now);
        }

    }

    private void button_Echo_Click(object sender, EventArgs e)
    {

        if (string.IsNullOrEmpty(textSendText.Text))
        {
            MessageBox.Show("보낼 텍스트를 입력하세요");
            return;
        }

        // Encoding.UTF8 : 문자열 인코딩 방식 설정 + 문자열을 바이트 배열로 반환 
        var body = Encoding.UTF8.GetBytes(textSendText.Text);

        List<byte> dataSource = new List<byte>();

        // 1. 헤더의 2 바이트 :  PacketLength
        // 내용물+헤더크기 합친 길이를 UInt16 자료형(2바이트)으로 변환 , 그리고 int 를 네트워크 전송용 바이트 배열 byte[] 으로 변환

        dataSource.AddRange(BitConverter.GetBytes((UInt16)(body.Length + PacketDef.PACKET_HEADER_SIZE)));

        // 2. 헤더의 2 바이트 : PacketId
        dataSource.AddRange(BitConverter.GetBytes((UInt16)(PACKET_ID.DEV_ECHO)));

        // 3. 헤더의 1 바이트 : Type
        dataSource.AddRange(new byte[] { (byte)0 });

        // 헤더의 5바이트 먼저 채우고 내용물 을 넣음
        dataSource.AddRange(body);

        // 스트에 차곡차곡 쌓은 바이트들을 하나의 고정된 배열 byte[] 변환하여서 큐에 넣음

        SendPacketQueue.Enqueue(dataSource.ToArray());
    }

    void NetworkReadProcess()
    {
        const Int16 PacketHeaderSize = PacketDef.PACKET_HEADER_SIZE;

        while (IsNetworkThreadRunning)
        {
            if (Network.IsConnected() == false)
            {
                // 인위적으로 (1 ms 동안) Block 상태를 만들어 주는 것
                System.Threading.Thread.Sleep(1);
                continue;
            }

            //(2) 핵심! 여기서 멈춥니다.
            // ClientSimpleTcp.Receive() 내부에서 socket.Receive()를 호출하는데,
            // 이 socket.Receive() 함수는 '동기(Synchronous)' 방식이라 데이터가 없으면 코드 진행을 멈춥니다.

            var recvData = Network.Receive();

            if (recvData != null)
            {
                // TCP는 데이터의 경계가 없기 때문에 바로 처리하지 안고 버퍼에 데이터를 넣음
                // write(byte[] data, int pos, int size)
                PacketBuffer.Write(recvData.Item2, 0, recvData.Item1);

                // [중요] 버퍼에 쌓인 데이터로 '완전한 패킷'을 만들 수 있는지 반복해서 확인
                while (true)
                {
                    var data = PacketBuffer.Read();

                    if(data.Count <1)
                    {
                        break;
                    }

                    var packet = new PacketData();
                    packet.DataSize = (short) (data.Count - PacketHeaderSize);

                    // 패킷헤더 
                    // 0~1 : UINT16 PacketLength
                    // 2~3 : UINT16 PacketId
                    // 4 : UINT8 Type

                    // 시작점에서 2 바이트 까지를 Int16 으로 변환
                    packet.PacketID = BitConverter.ToInt16(data.Array, data.Offset + 2);

                    // 
                    packet.Type = (SByte)data.Array[(data.Offset + 4)];

                    // 버퍼에 헤더를 건너띄고( 5번 인덱스 부터 시작점으로 복사) , 내용물 바디만 복사함
                    Buffer.BlockCopy(data.Array, (data.Offset + PacketHeaderSize), packet.BodyData,0, (data.Count - PacketHeaderSize));

                    // 공유자원 RecvPacketQueue 에 락을 걸고 
                    // RecvPacketQueue 은 dispatcherUITimer에 의해서 작동하고 있는 BackGroundProcess 함수에서 계속 접근중 (UI 스레드 위에서 작동). => 그래서 공유자원
                    lock (RecvPacketQueue)  
                    {
                        RecvPacketQueue.Enqueue(packet);
                    }

                }

                // DevLog 에 로그 남기기

            }
            else // recvData == null 이면
            {
                
                // 서버와 접속 종료
            }

        }
    }

    void NetworkSendProcess()
    {
        while (IsNetworkThreadRunning)
        {
            System.Threading.Thread.Sleep(1);

            // 연결확인 
            if (Network.IsConnected() == false)
            {
                continue;
            }

            // C# 에서 lock 문은 블록이 끝날때 자동을 잠금을 해체

            lock (SendPacketQueue)
            {
                if (SendPacketQueue.Count > 0)
                {
                    var packet = SendPacketQueue.Dequeue();
                    Network.Send(packet);
                }
            }

        }
    }

    void BackGroundProcess(object sender, EventArgs e)
    {
        // dispatcherUITimer 에 의해서 주기적으로 호출되는 함수
        // System.Windows.Forms.Timer는 UI 스레드에서 Tick 이벤트를 발생시키므로, 이 코드는 UI 스레드 위에서 작동

        try
        {
            // 공유자원 RecvPacketQueue에 락을 걸고 접근 
        }
        catch
        {
            
        }

    }
}
