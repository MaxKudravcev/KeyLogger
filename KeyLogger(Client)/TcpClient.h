#pragma once
#include <string>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")


class TcpClient
{
public:
	TcpClient(std::wstring ipAddr, std::wstring port);
	BOOLEAN Connect(std::wstring ipAddr, std::wstring port);

private:
	std::wstring ipAddr;
	std::wstring port;
	SOCKET connectionSocket = INVALID_SOCKET;
};

