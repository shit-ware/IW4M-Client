// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: clientdll
// Purpose: Displays global client broadcasts.
//
// Initial author: NTAuthority
// Started: 2011-07-03
// ==========================================================

#include "StdInc.h"
#include <ws2tcpip.h>

dvar_t* cg_hideBroadcast0;

static struct broadcastState_s
{
	bool initialized;
	SOCKET socket;
	WSAEVENT hConnectEvent;
} g_broadcast;

void Broadcast_Init()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) > 0)
	{
		return;
	}
	cg_hideBroadcast0 = Dvar_RegisterBool("cg_hideBroadcast0", false, 1, "Hide broadcasts of type '0'");

	g_broadcast.socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (g_broadcast.socket == INVALID_SOCKET)
	{
		OutputDebugString("invalid socket\n");
		Com_Printf(1, "Broadcast: Invalid socket\n");
		return;
	}

	ULONG nbTrue = TRUE;
	ioctlsocket(g_broadcast.socket, FIONBIO, &nbTrue);

	hostent* host = gethostbyname("localhost");
	if (!host)
	{
		g_broadcast.socket = INVALID_SOCKET;
		OutputDebugString("invalid host\n");
		Com_Printf(1, "Broadcast: Invalid host\n");
		return;
	}
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = *(DWORD*)host->h_addr;
	addr.sin_port = htons(3015);

	int retval = connect(g_broadcast.socket, (sockaddr*)&addr, sizeof(addr));
	if (retval == SOCKET_ERROR)
	{
		retval = WSAGetLastError();

		Com_Printf(1, va("Broadcast: Failed to connect to listen server: %i\n", retval));

		if (retval == WSAEWOULDBLOCK)
		{
			g_broadcast.hConnectEvent = WSACreateEvent();
			WSAEventSelect(g_broadcast.socket, g_broadcast.hConnectEvent, FD_CONNECT);
			retval = WaitForSingleObject(g_broadcast.hConnectEvent, 5000);

			if (retval != WAIT_OBJECT_0)
			{
				g_broadcast.socket = INVALID_SOCKET;
				OutputDebugString("connection timeout\n");
				Com_Printf(1, "Broadcast: connection timeout\n");
			}
			else
				WSACloseEvent(g_broadcast.hConnectEvent);
		}
	}
}

static DWORD* clientState = (DWORD*)0xB32630;
static int lastDid = 0;

#include <time.h>
char* va(char *format, ...);

void Broadcast_Frame()
{
	if (!g_broadcast.initialized)
	{
		Com_Printf(1, "Broadcast not already initialized, initiate it.\n");
		Broadcast_Init();
		g_broadcast.initialized = true;
	}


	if (g_broadcast.socket == INVALID_SOCKET)
	{
		Com_Printf(1, "Broadcast: Invalid socket.\n");
		return;
	}

	char buffer[2048];
	int retval = recv(g_broadcast.socket, buffer, sizeof(buffer), 0);

	if (retval == SOCKET_ERROR)
	{
		retval = WSAGetLastError();

		if (retval != WSAEWOULDBLOCK)
		{
			sprintf(buffer, "socket error %d\n", retval);
			g_broadcast.socket = INVALID_SOCKET;
			OutputDebugString(buffer);
		}

		return;
	}

	Com_Printf(1, va("Broadcast: %s \n", buffer));

	/*char* eol = strrchr(buffer, '\r\n');
	if (!eol)
	{
	Com_Printf(1, "Broadcast: End of Line not found.\n");
	return;
	}

	eol[0] = '\0';*/

	Cmd_TokenizeString(buffer);

	if (Cmd_Argc() == 0)
	{
		Com_Printf(1, "Broadcast: Argc is 0\n");
		return;
	}

	const char* command = Cmd_Argv(0);

	Com_Printf(1, "%s\n", command);

	if (!_stricmp(command, "chat"))
	{
		if (Cmd_Argc() >= 3)
		{
			if (Cmd_Argv(1)[0] == '0')
			{
				if (cg_hideBroadcast0 && cg_hideBroadcast0->current.boolean)
				{
					return;
				}
			}

			SV_GameSendServerCommand(-1, 0, va("%c \"%s\"", 104, Cmd_Argv(2)));
		}
	}
}