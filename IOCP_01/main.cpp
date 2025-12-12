#include "IOCompletionPort.h"

const UINT16 SERVER_PORT = 11021;
const UINT16 MAX_CLIENT = 100;

int main()
{
	IOCompletionPort ioCompletionPort;

	//소켓을 초기화
	ioCompletionPort.InitSocket();

	// 소켓과 서버주소를 연결하고 등록 
	ioCompletionPort.BindandListen(SERVER_PORT);

	ioCompletionPort.StartServer(MAX_CLIENT);

	printf("아무키나 누를 때 까지 대기합니다\n");

	// getchar() : Blocking 역할
	getchar();

	ioCompletionPort.DestroyThread();
	return 0;
}