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
        Dictionary<PACKET_ID,Action<byte[]>> PacketFuncDic = new Dictionary<PACKET_ID, Action<byte[]>>();

        void SetPacketHandler()
        {

        }

        void PacketProcess(PacketData packet)
        {

        }

    }
}
