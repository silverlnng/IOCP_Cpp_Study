#pragma once

#define WIN32_LEAN_AND_MEAN
// Windows API 헤더 파일을 포함하기 전에 선언하여 암호화, DDE, RPC, 셸 및 Windows 소켓과 같이 덜 자주 사용되는 API를 제외시켜 헤더 파일의 크기를 줄이고 컴파일 속도를 향상시키는 전처리기 지시문

#include <windows.h>


struct PacketInfo
{
	UINT32 ClientIndex = 0;
	UINT16 PacketId = 0;
	UINT16 DataSize = 0;
	char* pDataPtr = nullptr;

	// 패킷의 기본생성자들은 0 으로 채움
};

enum class PACKET_ID : UINT16
{
	// 시스템
	SYS_USER_CONNECT =11,
	SYS_USER_DISCONNECT =12,
	SYS_END=30,

	//DB 
	DB_END =199,

	// 클라이어트
	LOGIN_REQUEST = 201,
	LOGIN_RESPONSE = 202,

	ROOM_ENTER_REQUEST = 215,
	ROOM_ENTER_RESPONSE =216,

	ROOM_LEAVE_REQUEST =215,
	ROOM_LEAVE_RESPONSE = 216,

	ROOM_CHAT_REQUEST = 221,
	ROOM_CHAT_REPONSE = 222,
	ROOM_CHAT_NOTIFY = 223,

};



#pragma pack(push,1) 

	// #pragma pack(push,1) 중요  !! 메모리 정렬(Padding) 하지 말고 , 빈틈없이 1 바이트 단위로 꽉 채워 만들라는 명령어 
	// 이게 없으면 컴파일러가 속도를 위해서 2바이트와 1바이트 사이에 빈공간을 넣어서 헤더의 크기가 6 ,8 바이트가 될수있음
		// 정확하게 5 바이트로 만들기 위한 명령어.

struct PACKET_HEADER
{

	UINT16 PacketLengeh;
	UINT16 PacketId;
	UINT8 Type; // 압축 여부 , 암호화 여부 등 속성을 알아내는 값

	// 헤더 2+2+1= 5 바이트 , 클라이언트도 이 약속을 알고지켜야함.
};

const UINT32 PACKET_HEADER_LENGTH = sizeof(PACKET_HEADER);

//// 로그인 요청 //////////
const int MAX_USER_ID_LEN = 32;
const int MAX_USER_PW_LEN = 32;

struct LOGIN_REQUEST_PACKET : public PACKET_HEADER
{
	char UserID[MAX_USER_ID_LEN +1];
	char UserPW[MAX_USER_PW_LEN +1];
};

const size_t LOGIN_REQUEST_PACKET_SIZE = sizeof(LOGIN_REQUEST_PACKET);

struct LOGIN_RESPONSE_PACKET : public PACKET_HEADER
{
	UINT16 Result;
};

// 룸에 들어가기 요청
struct ROOM_ENTER_REQUEST_PACKET : public PACKET_HEADER
{
	INT32 RoomNumber;
};

struct ROOM_ENTER_RESPONSE_PACKET : public PACKET_HEADER
{
	INT16 Result;
};
