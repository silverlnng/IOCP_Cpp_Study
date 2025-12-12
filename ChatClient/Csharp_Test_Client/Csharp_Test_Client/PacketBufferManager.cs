using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Csharp_Test_Client
{
    class PacketBufferManager
    {
        int BufferSize = 0;
        int ReadPos = 0;
        int WritePos = 0;

        int HeaderSize = 0;
        int MaxPacketSize = 0;

        byte[] PacketData;
        byte[] PacketDataTemp;

        public bool Init(int size, int headerSize, int maxPacketSize)
        {
            if(size<(maxPacketSize*2) || size<1 || headerSize<1 || maxPacketSize<1)
            {
                // 잘못된 크기 설정일때 는 false
                return false;
            }

            // 크기 설정

            BufferSize = size;
            PacketData = new byte[size];
            PacketDataTemp = new byte[size];

            HeaderSize = headerSize;
            MaxPacketSize = maxPacketSize;

            return true;
        }

        public bool Write(byte[] data,int pos,int size)
        {
            // data : 원본 데이터 / data.length : 원본 데이터의 길이
            // pos : 가져올 데이터의 시작위치 , size : 가져올데이터의 길이
            if(data ==null || data.Length < pos +size )
            {
                // pos 부터 size 만큼 꺼낼려면  data.Length 가 더커야함 
                // 

                return false;
            }

            var remainBufferSize = BufferSize - WritePos;

            if (remainBufferSize < size)
            {
                return false;
            }


            // BlockCopy : C# 의 가장 빠른 메모리 복사 함수. C++ 의 memcpy
            // (원본배열, 원본시작위치, 목적지배열, 목적지시작위치, 복사할크기)
            Buffer.BlockCopy(data, pos, PacketData, WritePos, size);

            // 데이터를 이어서 쓸 위치를 size 만큼 증가시켜서 , 다음데이터가 그 뒤에 붙을수 있게 함
            WritePos += size;

            // 다음번에 쓸 공간이 부족한가 ?
            if(NextFree()==false)
            {
                // 그럼 앞쪽 빈 공간을 없애고 데이터를 앞으로 당기자
                BufferRelocate();
            }
            return true;
        }


        bool NextFree()
        {
            // 버퍼에 남아있는 공간
            var enableWriteSize = BufferSize - WritePos;

            if(enableWriteSize < MaxPacketSize)
            {
                return false;
            }
            return true;
        }

        void BufferRelocate()
        {
           /*           
           [이미 읽어서 필요 없는 데이터][처리해야 할 데이터][빈 공간]
           0                             ReadPos WritePos      BufferSize
           | -----------------------------| ---------------------| -------------|
                                   */


            var enableReadSize = WritePos - ReadPos;

            // (원본배열, 원본시작위치, 목적지배열, 목적지시작위치, 복사할크기)

            // 임시배열 (PacketDataTemp) 에 0번째 부터 처리해야할데이터만 넣음  
            Buffer.BlockCopy(PacketData, ReadPos, PacketDataTemp, 0, enableReadSize);

            // PacketData 의 0번째 부터 PacketDataTemp 에 있던걸 넣음
            Buffer.BlockCopy(PacketDataTemp,0,PacketData,0, enableReadSize);

        }

    }
}
