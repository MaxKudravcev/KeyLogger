#include "TcpClient.h"

TcpClient::TcpClient(std::wstring ipAddr, std::wstring port, int buffSize)
{
	this->ipAddr = ipAddr;
	this->port = port;
	if (buffSize > 0)
		this->innerBuffSize = buffSize;
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
	this->connectionStatus = TRUE;
	return TRUE;
}

void TcpClient::Disconnect()
{
	this->FlushStream();
	closesocket(this->connectionSocket);
	this->connectionSocket = INVALID_SOCKET;
	this->connectionStatus = FALSE;
	WSACleanup();
}

BOOLEAN TcpClient::SendBuff(std::wstring buff)
{
	if (buff.empty())
		return FALSE;

	this->outStream << buff;
	this->bytesInStream += buff.length();
	if (this->bytesInStream > this->innerBuffSize)
		return this->FlushStream();
	return TRUE;
}

BOOLEAN TcpClient::FlushStream()
{
	if (this->connectionStatus)
	{
		this->outStream << L"\0";
		this->bytesInStream++;
		int result = send(this->connectionSocket, (const char*)this->outStream.str().c_str(), this->bytesInStream * sizeof(wchar_t), 0);
		if (result == SOCKET_ERROR)
		{
			this->connectionStatus = FALSE;
			this->hReconnectionThread = (HANDLE)_beginthreadex(NULL, 0, this->ReconnectionThreadRoutine, this, 0, NULL);
			return FALSE;
		}
		this->bytesInStream = 0;
		this->outStream.str(L"");
		return TRUE;
	}
	return FALSE;
}

unsigned TcpClient::ReconnectionThreadRoutine(void* instance)
{
	TcpClient *client = (TcpClient *)instance;
	while (!client->Connect(L"", L""))
		Sleep(1000);
	client->connectionStatus = TRUE;
	CloseHandle(client->hReconnectionThread);
	return 0;
}
