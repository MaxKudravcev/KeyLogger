#include "TcpServer.h"

#define BUFFSIZE 256

int TcpServer::maxClients;
volatile int TcpServer::currClients;
std::wstring TcpServer::currDir;

TcpServer::TcpServer(std::wstring port, int maxClients, std::wstring currDir)
{
	this->port = port;
	this->currDir = currDir;
	if (maxClients > 0)
		this->maxClients = maxClients;
	else
		this->maxClients = 10;
}

BOOLEAN TcpServer::StartServer()
{
	WSADATA wsaData;

	SOCKET listenSocket = INVALID_SOCKET;
	struct addrinfoW *result = NULL, hints;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData))
		return FALSE;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	if (GetAddrInfoW(NULL, port.c_str(), &hints, &result))
	{
		WSACleanup();
		return FALSE;
	}

	listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (listenSocket == INVALID_SOCKET)
	{
		FreeAddrInfoW(result);
		WSACleanup();
		return FALSE;
	}

	if (bind(listenSocket, result->ai_addr, result->ai_addrlen) == SOCKET_ERROR)
	{
		FreeAddrInfoW(result);
		closesocket(listenSocket);
		WSACleanup();
		return FALSE;
	}

	FreeAddrInfoW(result);

	if (listen(listenSocket, 5) == SOCKET_ERROR)
	{
		closesocket(listenSocket);
		WSACleanup();
		return FALSE;
	}

	currClients = 0;
	hAcceptThread = (HANDLE)_beginthreadex(NULL, 0, AcceptThreadRoutine, this, 0, NULL);
	this->listenSocket = listenSocket;
	return TRUE;
}

void TcpServer::StopServer()
{
	shutdown(listenSocket, SD_BOTH);
	closesocket(listenSocket);
	listenSocket = INVALID_SOCKET;
	WSACleanup();
	WaitForSingleObject(hAcceptThread, INFINITE);
	CloseHandle(hAcceptThread);
}

unsigned TcpServer::AcceptThreadRoutine(void* instance)
{
	std::vector<HANDLE> threads;
	std::cout << "Listening for connections..." << std::endl;
	TcpServer* server = (TcpServer *)instance;
	SOCKET client = INVALID_SOCKET;
	while ((client = accept(server->listenSocket, (sockaddr *)NULL, NULL)) && server->listenSocket != INVALID_SOCKET)
	{
		while (currClients >= maxClients)
			Sleep(1000);

		if (client != INVALID_SOCKET)
		{
			currClients++;
			std::cout << "New client [" << client << "] has connected." << std::endl;
			threads.push_back((HANDLE)_beginthreadex(NULL, 0, ClientThreadRoutine, (void*)client, 0, NULL));
		}
	}

	if (threads.size() > 0)
	{
		WaitForMultipleObjects(threads.size(), &threads[0], TRUE, INFINITE);
		for (HANDLE thread : threads)
			CloseHandle(thread);
	}
	return 0;
}

unsigned TcpServer::ClientThreadRoutine(void* clientSocket)
{
	SOCKET client = (SOCKET)clientSocket;
	
	SOCKADDR_IN client_info;
	int size = sizeof(client_info);
	getpeername(client, (struct sockaddr*)&client_info, &size);
	wchar_t ip[INET_ADDRSTRLEN];
	InetNtopW(AF_INET, &(client_info.sin_addr), ip, INET_ADDRSTRLEN);
	
	FileService fileService(currDir, ip);

	wchar_t buff[BUFFSIZE + 1] = { 0 };
	int bytesRecieved = 0;
	while ((bytesRecieved = recv(client, (char *)buff, BUFFSIZE * 2, 0)) > 0)
	{
		fileService.Append(buff);
		ZeroMemory(buff, BUFFSIZE);
		std::cout << "Recieved data from client [" << client << "]." << std::endl;
	}
	int errorcode = WSAGetLastError();
	std::cout << "Client [" << client << "] has disconnected." << std::endl;
	closesocket(client);
	return 0;
}