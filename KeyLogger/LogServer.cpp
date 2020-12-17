#include <iostream>
#include "TcpServer.h"

std::wstring GetCurrentDirectory(std::wstring executable) {
	return executable.substr(0, executable.find_last_of(L'\\'));
}

int wmain(int argc, wchar_t* argv[])
{
	std::locale::global(std::locale("ru_RU.UTF-8"));

	int maxClients = std::stoi(argv[2]);
	std::wstring gLogDirectory = GetCurrentDirectory(argv[0]) + L"\\Logs\\";
	
	TcpServer server(argv[1], maxClients, gLogDirectory);
	server.StartServer();

	std::cout << "Type /quit to stop the server...\n";
	std::string command;
	std::cin >> command;
	
	while (command != "/quit")
	{
		std::cout << "Unrecognized command.\n";
		std::cin >> command;
	}
	std::cout << "Stopping the server...\n";
	server.StopServer();
	
	return 0;
}