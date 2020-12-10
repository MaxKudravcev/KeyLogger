#include "TcpClient.h"

TcpClient::TcpClient(std::wstring ipAddr, std::wstring port)
{
	this->ipAddr = ipAddr;
	this->port = port;
}

BOOLEAN TcpClient::Connect(std::wstring ipAddr, std::wstring port)
{
	WSADATA wsaData;
	SOCKET connectSocket = INVALID_SOCKET;
	struct addrinfoW *result = NULL, *ptr = NULL, hints;
	
	if (ipAddr.empty())
		ipAddr = this->ipAddr;
	if (port.empty())
		ipAddr = this->port;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData))
		return FALSE;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	
	if (GetAddrInfoW(ipAddr.c_str(), port.c_str(), &hints, &result) != 0)
	{
		WSACleanup();
		return FALSE;
	}

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (connectSocket == INVALID_SOCKET)
			return FALSE;

		if (connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen))
		{
			closesocket(connectSocket);
			connectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	FreeAddrInfoW(result);
	if (connectSocket == INVALID_SOCKET)
	{
		WSACleanup();
		return FALSE;
	}

	this->connectionSocket = connectSocket;
	return TRUE;
}
