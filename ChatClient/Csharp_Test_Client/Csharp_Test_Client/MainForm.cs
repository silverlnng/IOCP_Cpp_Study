

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

    private void mainForm_Load(object sender, EventArgs e)
    {
        // 패킷버퍼 초기화


        // ReadThread 
        NetworkReadThread = new System.Threading.Thread(this.NetworkReadProcess);

        // SendThread


    }


    void NetworkReadProcess()
    {

        while(IsNetworkThreadRunning)
        {
            if (Network.IsConnected() == false)
            {
                // 인위적으로 (1 ms 동안) Block 상태를 만들어 주는 것
                System.Threading.Thread.Sleep(1);
                continue;
            }

            //(2) 핵심! 여기서 멈춥니다.
            // ClientSimpleTcp.Receive() 내부에서 socket.Receive()를 호출하는데,
            // 이 함수는 '동기(Synchronous)' 방식이라 데이터가 없으면 코드 진행을 멈춥니다.

            var recvData = Network.Receive();

            if(recvData !=null)
            {
                // TCP는 데이터의 경계가 없기 때문에 바로 처리하지 안고 버퍼에 데이터를 넣음
                // write(byte[] data, int pos, int size)
                PacketBuffer.Write(recvData.Item2, 0, recvData.Item1);

                // [중요] 버퍼에 쌓인 데이터로 '완전한 패킷'을 만들 수 있는지 반복해서 확인
                while (true)
                {


                }


            }
            else
            {

            }

        }

    }

}
