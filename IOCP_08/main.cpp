#include "ChatServer.h"

const UINT16 SERVER_PORT = 11021;
const UINT16 MAX_CLIENT = 3; // 총 접속 할수있는 클라이언트 수
const UINT32 MAX_IO_WORKER_THREAD = 4; // 쓰레드 풀에 넣을 쓰레드

int main()
{
	ChatServer server;

	// 소켓을 초기화 
	server.Init(MAX_IO_WORKER_THREAD);

	// 소켓과 서버주소를 연결하고 등록
	server.BindAndListen(SERVER_PORT);

	server.Run(MAX_CLIENT);

}