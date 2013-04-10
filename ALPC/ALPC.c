#include "ALPC.h"
#pragma comment(lib,"ntdll.lib")

void Connect(TCHAR *ServerName)
{
	HANDLE ConnectionHandle = si.LPCPortHandle;
	UNICODE_STRING usPortName;
	NTSTATUS status;
	OBJECT_ATTRIBUTES oa;
	PORT_MESSAGE ConnectionMessage;
	ULONG BufferLength = sizeof(PORT_MESSAGE);
	ALPC_PORT_ATTRIBUTES apa;


	RtlInitUnicodeString(&usPortName,ServerName);
	status = NtAlpcConnectPort(&ConnectionHandle,
							   &usPortName,
							   &oa,
							   0x40000,//PortAttributes
							   0,	//Flags
							   0,	//RequiredServerSid
							   &ConnectionMessage,
							   &BufferLength,
							   0,	//OutMessageAttributes
							   0,	//InMessageAttributes
							   0);	//timeout
	if (!NT_SUCCESS(status))
	{
		printf("NtAlpcConnectPort error:%X",status);
		return;
	}

	printf("Connected to Server:%S",ServerName);
}

NTSTATUS AcceptConnectPort (PPORT_MESSAGE ConnectionRequest,PVOID Attr)
{
	NTSTATUS status;
	HANDLE DataPortHandle;
	PVOID PortContext = NULL;

	HANDLE ConnectionHandle = si.LPCPortHandle;
	EnterCriticalSection(&cs);
	if (Attr)
	{
	}

	status = NtAlpcAcceptConnectPort(&DataPortHandle,
										ConnectionHandle,
										0,									// Flags
										0,									// ObjectAttributes
										0,									// PortAttributes
										PortContext,
										ConnectionRequest,
										(PALPC_MESSAGE_ATTRIBUTES)Attr,
										TRUE);
	if(NT_SUCCESS(status))
	{
		// save some information
		LeaveCriticalSection(&cs);
	}
	else
	{
		LeaveCriticalSection(&cs);
		if (Attr)
			NtAlpcDeleteSectionView(ConnectionHandle, 0, Attr);
	}

	return status;
}

void ServerProc(SERVER_INFO *si)
{
	NTSTATUS status;
	ULONG count = 0;
	//PORT_VIEW ServerView;
	HANDLE DataPortHandle = NULL;
	//REMOTE_PORT_VIEW ClientView;
	//BOOL isExist;
	PVOID KeepRunning;
	HANDLE ClientHandle = NULL;

	//for NtAlpcSendWaitReceivePort
	HANDLE ConnectionHandle = si->LPCPortHandle;
	PORT_MESSAGE /*SendMessage,*/ReceiveMessage;
	ALPC_MESSAGE_ATTRIBUTES SendMessageAttributes,ReceiveMessageAttributes;
	ULONG BufferLength = sizeof(PORT_MESSAGE);
	LARGE_INTEGER timeout = {0};

	// AlpcInitializeMessageAttribute
	//ALPC_MESSAGE_ATTRIBUTES Buffer;
	ULONG RequiredBufferSize;

	// AlpcGetMessageAttribute
	PVOID attr;
	status = AlpcInitializeMessageAttribute(0x60000000, //AttributeFlags ??
											&SendMessageAttributes,
											0x2C,	//BufferSize maybe sizeof(ALPC_MESSAGE_ATTRIBUTES)??
											&RequiredBufferSize);
	if (!NT_SUCCESS(status))
	{
		printf("AlpcInitializeMessageAttribute error:%X\n",status);
		return;
	}
	do 
	{
		while (TRUE)
		{
			printf("Server Running...");
			status = NtAlpcSendWaitReceivePort(ConnectionHandle,
											   0,//0x10000,			//Flags, why 0x10000
											   0,//&SendMessage,
											   0,//&SendMessageAttributes,
											   &ReceiveMessage,
											   &BufferLength,
											   &ReceiveMessageAttributes,
											   0 /*timeout*/);
			

			if (status == 0x00000102) // timeout)
			{
				printf("NtAlpcSendWaitReceivePort Timeout");
				break;
			}
			//if something then break here
			// **
		
			attr = AlpcGetMessageAttribute(&ReceiveMessageAttributes,ValidAttributes /*Attributes Flags ?? */);
			status = AcceptConnectPort(&ReceiveMessage,attr);
		}

		KeepRunning = AlpcGetMessageAttribute(&ReceiveMessageAttributes,KeepRunningAttributes);

	} while (KeepRunning);
	
}

void runServer(TCHAR *ServerName)
{
	UNICODE_STRING usPortName;
	HANDLE hConnectPort = NULL;
	OBJECT_ATTRIBUTES oa;
	NTSTATUS status;
	ALPC_PORT_ATTRIBUTES apa;
	SECURITY_QUALITY_OF_SERVICE sqos;
	LARGE_INTEGER SectionSize = {0x9000};
	HANDLE hThread;
	ULONG Flags = 0; //猜的
	SIZE_T MaxMessageLength = PORT_MAXIMUM_MESSAGE_LENGTH;

	//以下都是乱填的
	SIZE_T MemoryBandwidth = 128;
	SIZE_T MaxPoolUsage = 128;
	SIZE_T MaxSectionSize = 512;
	SIZE_T MaxViewSize = sizeof(PORT_VIEW);
	SIZE_T MaxTotalSectionSize = 512;
	ULONG DupObjectType = 0;

	InitializeCriticalSection(&cs);

	RtlZeroMemory(&sqos,sizeof(SECURITY_QUALITY_OF_SERVICE));
	InitializeAlpcPortAttributes(&apa,Flags,sqos,MaxMessageLength,MemoryBandwidth,MaxPoolUsage,\
		MaxSectionSize,MaxViewSize,MaxTotalSectionSize,DupObjectType);
	//RtlZeroMemory(&apa,sizeof(ALPC_PORT_ATTRIBUTES));

	RtlInitUnicodeString(&usPortName,ServerName);
	InitializeObjectAttributes(&oa,&usPortName,0x200,0,NULL);

	status = NtAlpcCreatePort(&hConnectPort,&oa,&apa);
	if (!NT_SUCCESS(status))
	{
		printf("NtAlpcCreatePort error:%X\n",status);
		return;
	}

	
	status = ZwCreateSection(&SectionHandle,
							 SECTION_MAP_READ | SECTION_MAP_WRITE,
							 NULL,	//backed by the pagefile
							 &SectionSize,
							 PAGE_READWRITE,
							 SEC_COMMIT,
							 NULL);

	if (!NT_SUCCESS(status))
	{
		printf("ZwCreateSection error:%X\n",status);
		return;
	}

	/*
	ALPC_DATA_VIEW_ATTR adva;
	adva.Flags = 0;	//unknown
	adva.SectionHandle = SectionHandle;
	adva.ViewBase = 0;
	adva.ViewSize = SectionSize.LowPart;
	status = NtAlpcCreateSectionView(hConnectPort,0,&adva);
	if (!NT_SUCCESS(status))
	{
		printf("NtAlpcCreateSectionView error:%X\n",status);
		return;
	}
	*/

	si.LPCPortHandle = hConnectPort;
	si.SectionHandle = SectionHandle;
	if (!(hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)&ServerProc,(LPVOID)&si,0,NULL)))
	{
		printf("CreateThread error:%d\n",GetLastError());
		return;
	}

}