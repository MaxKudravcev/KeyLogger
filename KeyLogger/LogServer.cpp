#include <iostream>
#include "TcpServer.h"

int main()
{
	TcpServer server(L"5050", 10);
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