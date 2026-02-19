using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Web;
using System.Windows.Forms;

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

    // 다중 접속 테스트용 클라이언트 리스트
    List<ClientMultiTCP> MultiClientList = new List<ClientMultiTCP>();

    public MainForm()
    {
        InitializeComponent();
    }

    // MainForm_Load 함수 : 폼이 메모리에 로드되어 화면에 나타나기 직전에 자동으로 실행되는 함수
    private void MainForm_Load(object sender, EventArgs e)
    {
        // 패킷버퍼 초기화
        PacketBuffer.Init((8096 * 10), PacketDef.PACKET_HEADER_SIZE, 1024);
        // 스레드 시작

        IsNetworkThreadRunning = true;

        NetworkReadThread = new System.Threading.Thread(this.NetworkReadProcess);
        NetworkReadThread.Start();

        NetworkSendThread = new System.Threading.Thread(this.NetworkSendProcess);
        NetworkSendThread.Start();

        // UI 타이머 가동 :  dispatcherUITimer를 생성하고 0.1초 간격으로 BackGroundProcess가 실행되도록 설정합니다. 이는 수신된 패킷을 처리하고 UI 로그를 남기는 역할을 합니다.

        IsBackGroundProcessRunning = true;

        dispatcherUITimer = new System.Windows.Forms.Timer();
        dispatcherUITimer.Tick += new EventHandler(BackGroundProcess);
        dispatcherUITimer.Interval = 100;
        dispatcherUITimer.Start();

        BtnDisconnect.Enabled = false;

        SetPacketHandler();
        DevLog.Write("프로그램 시작 !!!", LOG_LEVEL.INFO);
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

    private void BtnDisconnect_Click(object sender, EventArgs e)
    {
        SetDisconnected();
        Network.Close();
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

                    if (data.Count < 1)
                    {
                        break;
                    }

                    var packet = new PacketData();
                    packet.DataSize = (short)(data.Count - PacketHeaderSize);

                    // 패킷헤더 
                    // 0~1 : UINT16 PacketLength
                    // 2~3 : UINT16 PacketId
                    // 4 : UINT8 Type

                    // 시작점에서 2 바이트 까지를 Int16 으로 변환
                    packet.PacketID = BitConverter.ToInt16(data.Array, data.Offset + 2);

                    // 
                    packet.Type = (SByte)data.Array[(data.Offset + 4)];
                    packet.BodyData = new byte[packet.DataSize];
                    // 버퍼에 헤더를 건너띄고( 5번 인덱스 부터 시작점으로 복사) , 내용물 바디만 복사함
                    Buffer.BlockCopy(data.Array, (data.Offset + PacketHeaderSize), packet.BodyData, 0, (data.Count - PacketHeaderSize));

                    // 공유자원 RecvPacketQueue 에 락을 걸고 
                    // RecvPacketQueue 은 dispatcherUITimer에 의해서 작동하고 있는 BackGroundProcess 함수에서 계속 접근중 (UI 스레드 위에서 작동). => 그래서 공유자원
                    lock (RecvPacketQueue)
                    {
                        RecvPacketQueue.Enqueue(packet);
                    }

                }

                // DevLog 에 로그 남기기
                DevLog.Write($"받은 데이터 크기: {recvData.Item1}", LOG_LEVEL.INFO);
            }
            else // recvData == null 이면
            {
                Network.Close();
                SetDisconnected();
                DevLog.Write("서버와 접속 종료 !!!", LOG_LEVEL.INFO);
                // 서버와 접속 종료
            }

        }
    }

    // 스레드에 의해서 실행되는 함수

    void NetworkSendProcess()
    {
        while (IsNetworkThreadRunning)
        {
            //System.Threading.Thread.Sleep(1);

            // 연결확인 
            if (Network.IsConnected() == false)
            {
                System.Threading.Thread.Sleep(10);
                continue;
            }

            // C# 에서 lock 문은 블록이 끝날때 자동을 잠금을 해체
            byte[] packet = null;

            lock (SendPacketQueue)
            {
                if (SendPacketQueue.Count > 0)
                {
                    packet = (byte[])SendPacketQueue.Dequeue();
                }
            }

            // 2. Lock 밖에서 전송 (IO 지연이 다른 스레드를 방해하지 않음)
            if (packet != null)
            {
                Network.Send(packet);
                // 패킷을 보냈다면 쉬지 않고 즉시 다음 루프로 진입하여 처리 속도 극대화
            }
            else
            {
                // 3. 보낼 데이터가 없을 때만 CPU 휴식 (Spin Lock 방지)
                System.Threading.Thread.Sleep(1);
            }

        }
    }

    void BackGroundProcess(object sender, EventArgs e)
    {
        // dispatcherUITimer 에 의해서 주기적으로 호출되는 함수
        // System.Windows.Forms.Timer는 UI 스레드에서 Tick 이벤트를 발생시키므로, 이 코드는 UI 스레드 위에서 작동

        ProcessLog();

        try
        {
            var packet = new PacketData();

            // 공유자원 RecvPacketQueue에 락을 걸고 접근 
            lock (RecvPacketQueue)
            {
                if (RecvPacketQueue.Count > 0)
                {
                    packet = RecvPacketQueue.Dequeue();
                }
            }

            if (packet.PacketID != 0)
            {
                PacketProcess(packet);
            }
        }
        catch (Exception ex)
        {
            MessageBox.Show(string.Format("ReadPacketQueueProcess . error : {0}", ex.Message));
        }

    }

    private void ProcessLog()
    {
        // 너무 이 작업만 할 수 없으므로 일정 작업 이상을 하면 일단 패스한다.

        // UI 에 출력
        int logWorkCount = 0;

        while (IsBackGroundProcessRunning)
        {
            System.Threading.Thread.Sleep(1);

            string msg;

            if (DevLog.GetLog(out msg))
            {
                ++logWorkCount;
                if (listBoxLog.Items.Count > 523)
                {
                    listBoxLog.Items.Clear();
                }

                listBoxLog.Items.Add(msg);
                listBoxLog.SelectedIndex = listBoxLog.Items.Count - 1;
            }
            else
            {
                break;
            }

            if (logWorkCount > 10)
            {
                break;
            }
        }
    }

    public void SetDisconnected()
    {
        if (BtnConnect.Enabled == false)
        {
            BtnConnect.Enabled = true;
            BtnDisconnect.Enabled = false;
        }

        SendPacketQueue.Clear();

        listBoxRoomChatMsg.Items.Clear();
        listBoxRoomUserList.Items.Clear();

        labelStatus.Text = "서버와 접속이 끊어짐";

    }

    public void PostSendPacket(PACKET_ID packetID, byte[] bodyData)
    {
        if (Network.IsConnected() == false)
        {
            DevLog.Write("서버에 연결이 되어있지 않습니다", LOG_LEVEL.ERROR);
            return;
        }

        Int16 bodyDataSize = 0;

        if (bodyData != null)
        {
            bodyDataSize = (Int16)bodyData.Length;
        }

        var packetSize = bodyDataSize + PacketDef.PACKET_HEADER_SIZE;

        List<byte> dataSource = new List<byte>();

        // 헤더 5바이트 채우기 (총크기 2바이트, 패킷ID 2바이트, 타입 1바이트)

        dataSource.AddRange(BitConverter.GetBytes((UInt16)packetSize));

        dataSource.AddRange(BitConverter.GetBytes((UInt16)packetID));

        dataSource.AddRange(new byte[] { (byte)0 });

        // 헤더 채우고 바디(내용물) 채우기

        if (bodyData != null)
        {
            dataSource.AddRange(bodyData);
        }

        SendPacketQueue.Enqueue(dataSource.ToArray());
    }

    private void BtnLogin_Click(object sender, EventArgs e)
    {
        var loginReqPacket = new LoginReqPacket();

        loginReqPacket.SetValue(textBoxUserID.Text, textBoxUserPW.Text);

        // textBoxUserID , textBoxUserPW 를 직렬화 시켜서 (바이트 배열로 만들어서) 보냄

        PostSendPacket(PACKET_ID.LOGIN_REQ, loginReqPacket.ToBytes());

        DevLog.Write($"로그인 요청: {textBoxUserID.Text} , {textBoxUserPW.Text}");
    }


    private void btn_RoomEnter_Click(object sender, EventArgs e)
    {
        var requestPacket = new RoomEnterReqPacket();

        requestPacket.SetValue(Convert.ToInt32(textBoxRoomNumber.Text));

        PostSendPacket(PACKET_ID.ROOM_ENTER_REQ, requestPacket.ToBytes());

        DevLog.Write($"룸 입장 요청: {textBoxRoomNumber.Text} 번");

    }

    private void btn_RoomLeave_Click(object sender, EventArgs e)
    {
        PostSendPacket(PACKET_ID.ROOM_LEAVE_REQ, null);

        DevLog.Write($"방 나가기 요청:  {textBoxRoomNumber.Text} 번");
    }

    void AddRoomUserList(Int64 userUniqueId, string userID)
    {
        var msg = $"{userUniqueId} : {userID}";
        listBoxRoomUserList.Items.Add(msg);
    }

    void RemoveRoomUserList(Int64 userUniqueId)
    {
        // TO DO
    }

    private void btnRoomChat_Click(object sender, EventArgs e)
    {
        // TO DO
    }

    private void BtnMultiConnect_Click(object sender, EventArgs e)
    {
        // 테스트 클라이언트 인원수 만큼 ClientMultiTCP 객체를 생성하고 접속 시도
        // 1. textBoxClientNum에서 접속할 인원수 가져오기
        int clientCount = 0;

        if (int.TryParse(textBoxClientNum.Text, out clientCount) == false)
        {
            DevLog.Write("접속 인원수가 올바르지 않습니다.", LOG_LEVEL.ERROR);
            return;
        }

        // 2. 서버 주소 및 포트 정보 취득 (기존 btnConnect_Click 로직 활용) [3]
        string address = textBoxIP.Text;
        if (checkBoxLocalHostIP.Checked)
        {
            address = "127.0.0.1";
        }
        int port = Convert.ToInt32(textBoxPort.Text);

        //DevLog.Write($"{clientCount}명의 클라이언트 접속 시작...", LOG_LEVEL.INFO);

        // 별도 스레드에서 병렬 처리
        Task.Run(() =>
        {
            //Parallel.For 으로 작업량을 나누어서 여러 스레드에서 동시에 접속 시도
            Parallel.For(0, clientCount, i =>
            {
                var multiClient = new ClientMultiTCP();
                if (multiClient.Connect(address, port))
                {
                    lock (MultiClientList) // 리스트 접근 시 동기화 필요
                    {
                        MultiClientList.Add(multiClient);
                    }
                }
            });

            DevLog.Write($"총 {MultiClientList.Count}명의 접속 요청이 완료되었습니다.", LOG_LEVEL.INFO);

            // TODO : 접속다되면 바로 에코메세지 보내기

        });
    }

    private void BtnMultiDisConnect_Click(object sender, EventArgs e)
    {
        // 1. 현재 관리 중인 다중 클라이언트 수가 0명인지 확인
        if (MultiClientList.Count == 0)
        {
            DevLog.Write("종료할 다중 접속 클라이언트가 없습니다.", LOG_LEVEL.WARN);
            return;
        }

        int disconnectedCount = 0;

        try
        {
            // 2. 리스트를 순회하며 모든 클라이언트의 접속 종료
            foreach (var client in MultiClientList)
            {
                if (client != null)
                {
                    // 소켓과 스트림을 닫는 Close() 메서드 호출 [1]
                    client.Close();
                    disconnectedCount++;
                }
            }

            // 3. 관리 리스트 완전히 비우기
            MultiClientList.Clear();

            // 4. UI 및 로그 업데이트
            DevLog.Write($"{disconnectedCount}명의 클라이언트 접속을 한꺼번에 종료했습니다.", LOG_LEVEL.INFO);

            // 기존 소스의 SetDisconnectd 로직을 참고하여 UI 상태 반영 [2, 3]
            labelStatus.Text = $"{DateTime.Now}. 다중 접속 종료 완료";
        }
        catch (Exception ex)
        {
            DevLog.Write($"다중 접속 종료 중 오류 발생: {ex.Message}", LOG_LEVEL.ERROR);
        }
    }

    private void Btn_Multi_Echo_Click(object sender, EventArgs e)
    {

        // 1.보낼 텍스트가 있는지 확인(기존 소스 button1_Click 로직 활용)[1]
        /*if (string.IsNullOrEmpty(textSendText.Text))
        {
            MessageBox.Show("보낼 에코 텍스트를 입력하세요");
            return;
        }*/


        // [수정 후] 현재 시간(Ticks, 8바이트 정수)을 보냄
        long currentTicks = DateTime.Now.Ticks;
        byte[] body = BitConverter.GetBytes(currentTicks);

        // 2. 다중 클라이언트 리스트가 비어있는지 확인
        if (MultiClientList.Count == 0)
        {
            DevLog.Write("메시지를 보낼 클라이언트가 없습니다. 먼저 다중 접속을 수행하세요.", LOG_LEVEL.WARN);
            return;
        }

        // 3. 에코 패킷 데이터 생성 (Packet.txt 및 mainForm.txt의 규격 준수) [2-4]
        //var body = Encoding.UTF8.GetBytes(textSendText.Text);

        var bodyDataSize = (Int16)body.Length;
        var packetSize = (UInt16)(bodyDataSize + PacketDef.PACKET_HEADER_SIZE); // 헤더 5바이트 포함 [2, 4]

        List<byte> dataSource = new List<byte>();
        dataSource.AddRange(BitConverter.GetBytes(packetSize));                    // 전체 크기 [1]
        dataSource.AddRange(BitConverter.GetBytes((UInt16)PACKET_ID.DEV_ECHO));    // 패킷 ID: 1 [2, 3]
        dataSource.AddRange(new byte[] { (byte)0 });                               // 타입 (1바이트) [3, 5]
        dataSource.AddRange(body);                                                  // 실제 데이터 [3]

        byte[] sendData = dataSource.ToArray();
        int sendCount = 0;

        // 4. 리스트의 모든 클라이언트에게 패킷 전송
        foreach (var client in MultiClientList)
        {
            if (client.IsConnected())
            {
                // 비동기 Send 호출
                client.Send(sendData);
                sendCount++;
            }
        }

        // 5. 결과 로그 남기기 [6, 7]
        DevLog.Write($"{sendCount}명의 클라이언트가 에코 메시지를 전송했습니다: {textSendText.Text}", LOG_LEVEL.INFO);
    }

    private void btnMultiChat_Click(object sender, EventArgs e)
    {
        // 1.보낼 채팅 메시지가 있는지 확인(기존 btnRoomChat_Click 로직 참조 [1])
        if (string.IsNullOrEmpty(textBoxRoomSendMsg.Text))
        {
            MessageBox.Show("채팅 메시지를 입력하세요");
            return;
        }

        // 2. 전송할 다중 클라이언트가 있는지 확인
        if (MultiClientList.Count == 0)
        {
            DevLog.Write("채팅을 보낼 클라이언트가 없습니다. 먼저 다중 접속을 수행하세요.", LOG_LEVEL.WARN);
            return;
        }

        // 3. 채팅 요청 패킷 생성 (Packet.txt [2] 참조)
        // 모든 클라이언트가 동일한 메시지를 보내므로 루프 밖에서 한 번만 생성합니다.
        var chatReq = new RoomChatReqPacket();
        chatReq.SetValue(textBoxRoomSendMsg.Text);
        byte[] bodyData = chatReq.ToBytes();

        // 4. 패킷 헤더 + 바디 직렬화 (mainForm.txt PostSendPacket 로직 참조 [3]-[4])
        // 패킷 구조: [TotalSize(2)][PacketID(2)][Type(1)][BodyData(...)]
        List<byte> dataSource = new List<byte>();

        // 전체 크기 = 바디 크기 + 헤더 크기(5)
        UInt16 packetSize = (UInt16)(bodyData.Length + PacketDef.PACKET_HEADER_SIZE);

        dataSource.AddRange(BitConverter.GetBytes(packetSize));
        dataSource.AddRange(BitConverter.GetBytes((UInt16)PACKET_ID.ROOM_CHAT_REQ)); // ID: 221 [5]
        dataSource.AddRange(new byte[] { (byte)0 }); // Type
        dataSource.AddRange(bodyData);

        byte[] sendPacket = dataSource.ToArray();
        int sendCount = 0;

        // 5. 리스트의 모든 클라이언트에게 전송
        foreach (var client in MultiClientList)
        {
            // ClientMultiTcp 클래스의 IsConnected와 Send 메서드 사용 (대화 기록 기반)
            if (client.IsConnected())
            {
                client.Send(sendPacket);
                sendCount++;
            }
        }

        // 6. 결과 로그 기록
        DevLog.Write($"{sendCount}명의 클라이언트가 채팅 패킷을 전송했습니다: {textBoxRoomSendMsg.Text}", LOG_LEVEL.INFO);
    }

    private void BtnMultiLogin_Click(object sender, EventArgs e)
    {
        // 1. 접속된 클라이언트가 있는지 확인
        if (MultiClientList.Count == 0)
        {
            DevLog.Write("접속된 멀티 클라이언트가 없습니다.", LOG_LEVEL.ERROR);
            return;
        }

        DevLog.Write($"{MultiClientList.Count}명 로그인 요청 시작...", LOG_LEVEL.INFO);

        for (int i = 0; i < MultiClientList.Count; i++)
        {
            var client = MultiClientList[i];

            if (client.IsConnected() == false) { continue; }

            try
            {
                // DB에 넣은 "User_0", "User_1" ... 형식과 일치시킴
                string userId = $"ID_{i}";
                string userPw = $"pass_{i}"; // DB에 넣은 비밀번호와 일치

                // 로그인 요청 패킷 전송
                var loginReq = new LoginReqPacket();
                loginReq.SetValue(userId, userPw);

                // C. 공통 함수로 전송용 바이트 배열 생성
                byte[] sendData = MakePacketBuffer(PACKET_ID.LOGIN_REQ, loginReq.ToBytes());

                // D. 해당 클라이언트 소켓으로 직접 전송 [3]
                client.Send(sendData);
            }
            catch (Exception ex)
            {
                DevLog.Write($"Index[{i}] 로그인 요청 실패: {ex.Message}", LOG_LEVEL.ERROR);
            }
            //DevLog.Write($"{MultiClientList.Count}명 의 로그인 요청");
        }
    }

    public byte[] MakePacketBuffer(PACKET_ID packetID, byte[] bodyData)
    {
        // 1. 바디 크기 계산
        Int16 bodyDataSize = 0;
        if (bodyData != null)
        {
            bodyDataSize = (Int16)bodyData.Length;
        }

        // 2. 전체 패킷 크기 계산 (헤더 + 바디)
        var packetSize = bodyDataSize + PacketDef.PACKET_HEADER_SIZE;

        // 3. 리스트에 바이트 추가 (기존 PostSendPacket 로직 활용)
        List<byte> dataSource = new List<byte>();
        dataSource.AddRange(BitConverter.GetBytes((UInt16)packetSize));
        dataSource.AddRange(BitConverter.GetBytes((UInt16)packetID));
        dataSource.AddRange(new byte[] { (byte)0 }); // Type

        if (bodyData != null)
        {
            dataSource.AddRange(bodyData);
        }

        return dataSource.ToArray();
    }

    private void BtnUpdateScoreMulti_Click(object sender, EventArgs e)
    {
        // 1. 로그인된 클라이언트 확인
        if (MultiClientList.Count == 0)
        {
            DevLog.Write("로그인된 클라이언트가 없습니다.", LOG_LEVEL.WARN);
            return;
        }

        // 현재 리스트를 복사하거나 참조를 가져옴 (테스트 도중 접속/해제 금지)
        int clientCount = MultiClientList.Count;

        DevLog.Write($"{clientCount}명 점수 업데이트 요청 스레드 시작...", LOG_LEVEL.INFO);

        // 2. 별도의 스레드(Task)에서 전송 로직 수행
        Task.Run(() =>
        {
            Random rnd = new Random();
            int sendCount = 0;

            foreach (var client in MultiClientList)
            {
                // 연결된 클라이언트만 전송
                if (client.IsConnected())
                {
                    // 랜덤 점수 생성 (100 ~ 10000)
                    int randomScore = rnd.Next(100, 10000);

                    // 패킷 생성 및 직렬화
                    var scorePacket = new UpdateClientScoreReqPacket();
                    scorePacket.Score = randomScore;

                    // MakePacketBuffer는 MainForm의 메소드이므로 스레드 안전성 주의 필요.
                    // 만약 MakePacketBuffer가 UI 컨트롤을 건드리지 않는다면 사용 가능하지만,
                    // 안전하게 직접 바이트 배열을 만드는 것이 좋습니다.
                    List<byte> dataSource = new List<byte>();
                    byte[] bodyData = scorePacket.ToBytes();

                    // 헤더 + 바디 조립 (PacketDefine.cs 참조)
                    UInt16 packetSize = (UInt16)(bodyData.Length + PacketDef.PACKET_HEADER_SIZE);
                    dataSource.AddRange(BitConverter.GetBytes(packetSize));
                    dataSource.AddRange(BitConverter.GetBytes((UInt16)PACKET_ID.UPDATE_CLIENT_SCORE_REQ));
                    dataSource.AddRange(new byte[] { (byte)0 }); // Type
                    dataSource.AddRange(bodyData);

                    // 전송 (ClientMultiTCP.Send는 내부적으로 BeginSend 사용 [3])
                    client.Send(dataSource.ToArray());
                    sendCount++;

                    // [중요] 부하 조절 (Throttling)
                    // 100명에게 보낼 때마다 10ms 정도 쉬어줍니다.
                    // PC의 네트워크 버퍼가 가득 차서 전송 실패(WSAENOBUFS)가 나는 것을 방지합니다.
                    if (sendCount % 100 == 0)
                    {
                        Thread.Sleep(10);
                    }
                }
            }

            // 3. 완료 로그 (DevLog는 ConcurrentQueue를 사용하므로 스레드 안전함 [4])
            DevLog.Write($"[완료] 총 {sendCount}명의 점수 업데이트 요청을 전송했습니다.", LOG_LEVEL.INFO);
        });
    }
}
