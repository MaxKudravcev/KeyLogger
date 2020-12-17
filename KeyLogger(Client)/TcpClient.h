#pragma once

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <string>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <process.h>

#pragma comment(lib, "Ws2_32.lib")


class TcpClient
{
public:
	TcpClient(std::wstring ipAddr, std::wstring port, int buffSize);
	BOOLEAN Connect(std::wstring ipAddr, std::wstring port);
	void Disconnect();
	BOOLEAN SendBuff(std::wstring buff);
	BOOLEAN SendBuff(wchar_t wchar);

private:
	std::wstring ipAddr;
	std::wstring port;
	SOCKET connectionSocket = INVALID_SOCKET;
	std::wstringstream outStream;
	int innerBuffSize = 1024;
	int bytesInStream = 0;
	BOOLEAN FlushStream();
	BOOLEAN connectionStatus = FALSE;
	HANDLE hReconnectionThread;
	static unsigned __stdcall ReconnectionThreadRoutine(void* instance);
};

