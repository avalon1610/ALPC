#include <stdio.h>
#include "..\ALPC\ALPC.h"

#pragma comment(lib,"alpc.lib")

void wait()
{
	MSG msg;
	while (GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

int main()
{
	runServer(L"\\myServer");
	wait();
}