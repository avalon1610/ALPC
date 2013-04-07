#include <stdio.h>
#include "..\ALPC\ALPC.h"

#pragma comment(lib,"alpc.lib")

int main()
{
	runServer(L"\\myServer");
}