#pragma once
#include <string>
#include <vector>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <process.h>
#include <iostream>
#include "FileService.h"

#pragma comment(lib, "ws2_32.lib")
class TcpServer
{
public:
	TcpServer(std::wstring port, int maxClients);
	BOOLEAN StartServer();
	void StopServer();

private:
	std::wstring port;
	SOCKET listenSocket = INVALID_SOCKET;
	HANDLE hAcceptThread;

	static int maxClients;
	static volatile int currClients;
	static unsigned __stdcall AcceptThreadRoutine(void* instance);
	static unsigned __stdcall ClientThreadRoutine(void* clientSocket);
};

