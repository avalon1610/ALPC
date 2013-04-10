#pragma once

#include "UserModeDefs.h"
#include "ntlpcapi.h"
#include <tchar.h>
#include <stdio.h>

void Connect(TCHAR *ServerName);
void runServer(TCHAR *ServerName);

#define InitializeAlpcPortAttributes(p,f,s,mml,mb,mpu,mss,mvs,mtss,dot) { \
	(p)->Flags = f; \
	(p)->SecurityQos = s; \
	(p)->MaxMessageLength = mml; \
	(p)->MemoryBandwidth = mb; \
	(p)->MaxPoolUsage = mpu; \
	(p)->MaxSectionSize = mss; \
	(p)->MaxViewSize = mvs; \
	(p)->MaxTotalSectionSize = mtss;\
	(p)->DupObjectTypes = dot; \
}
typedef struct _CLIENT_ENTRY
{
	LIST_ENTRY List;
	HANDLE ClientHandle; // data port
	REMOTE_PORT_VIEW ClientView;
} CLIENT_ENTRY,*PCLIENT_ENTRY;

typedef struct _SERVER_INFO
{
	HANDLE LPCPortHandle;	// connection port
	HANDLE SectionHandle;
	PORT_VIEW ServerView;
}SERVER_INFO,*PSERVER_INFO;

typedef struct _CLIENT_INFO
{
	HANDLE ServerHandle;	//data port
	PORT_VIEW ClientView;
	REMOTE_PORT_VIEW ServerView;
}CLIENT_INFO,*PCLIENT_INFO;

HANDLE SectionHandle;;
SERVER_INFO si;
CLIENT_INFO ci;

typedef struct _TRANSFERRED_MESSAGE
{
	PORT_MESSAGE	Header;
	ULONG			Command;
	BOOLEAN			UseSection;
	WCHAR			Message[128];
} TRANSFERRED_MESSAGE,*PTRANSFERRED_MESSAGE;

CRITICAL_SECTION cs;


