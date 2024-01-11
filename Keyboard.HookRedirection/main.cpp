


#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <iostream>
#include <winnt.h>
#include <vector>
int shift_active();
int capital_active();
char VirtKeyToAscii(DWORD wVirtKey, DWORD wScanCode);


enum Keys
{
	ShiftKey = 16,
	Capital = 20,
};
struct KeyRedirect
{
	WORD KeyDown;
	std::vector<WORD> KeysRedirect;
};

using namespace std;

HHOOK hHook{ NULL };

KBDLLHOOKSTRUCT* kbdStruct;

void _KBDLLToString(KBDLLHOOKSTRUCT* kb) {
	cout << VirtKeyToAscii(kb->vkCode, kb->scanCode) << ": " << kb->vkCode << "," << kb->scanCode << endl;
}

std::vector<KeyRedirect> KeysRedirect = {};




LRESULT CALLBACK _keyboard_hook(const int code, const WPARAM wParam, const LPARAM lParam) {
	if (wParam == WM_KEYDOWN) {
		kbdStruct = (KBDLLHOOKSTRUCT*)lParam;


	//	cout << "scanCode:" << kbdStruct->scanCode << "  \"" << VirtKeyToAscii(kbdStruct->vkCode , kbdStruct->scanCode) << "\"" << "  vkCode:" << kbdStruct->vkCode << endl;

		_KBDLLToString(kbdStruct);
		for (KeyRedirect n : KeysRedirect) {
			if (kbdStruct->vkCode != n.KeyDown)
				break;

			
			cout << "Key: " << kbdStruct->vkCode << " Redirect: ";
			for (DWORD keysr : n.KeysRedirect) {
				cout << keysr << " ";


				INPUT inputs[1] = {};
				ZeroMemory(inputs, sizeof(inputs));
				inputs[0].type = INPUT_KEYBOARD;
				inputs[0].ki.time = 0;
				inputs[0].ki.wVk = keysr;
			 

				UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
				if (uSent != ARRAYSIZE(inputs))
				{
					cout << (L"Failed: 0x%x\n", HRESULT_FROM_WIN32(GetLastError())) << endl;
				}

			}
			for (DWORD keysr : n.KeysRedirect) {
				cout << keysr << " ";


				INPUT inputs[1] = {};
				ZeroMemory(inputs, sizeof(inputs));
				inputs[0].type = INPUT_KEYBOARD;
				inputs[0].ki.time = 0;
				inputs[0].ki.wVk = keysr;
				inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;

				UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
				if (uSent != ARRAYSIZE(inputs))
				{
					cout << (L"Failed: 0x%x\n", HRESULT_FROM_WIN32(GetLastError())) << endl;
				}

			}

			cout << endl;



 
			 
			return 1;
		}
	}

	return CallNextHookEx(hHook, code, wParam, (LPARAM)(kbdStruct));
}



int main()
{
	KeysRedirect.push_back(KeyRedirect{ 49, {91} });

	 

	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, _keyboard_hook, NULL, 0);
	if (hHook == NULL) {
		std::cout << "Keyboard hook failed!" << std::endl;
	}


	 
	 
	while (GetMessage(NULL, NULL, 0, 0));
	if(hHook)
		UnhookWindowsHookEx(hHook);
	return 0;
}

char VirtKeyToAscii(DWORD wVirtKey , DWORD wScanCode) {
	BYTE lpKeyState[256];
	char result;
	GetKeyboardState(lpKeyState);
	lpKeyState[Keys::ShiftKey] = 0;
	lpKeyState[Keys::Capital] = 0;
	if (shift_active()) {
		lpKeyState[Keys::ShiftKey] = 0x80;
	}
	if (capital_active()) {
		lpKeyState[Keys::Capital] = 0x01;
	}
	ToAscii(wVirtKey, wScanCode, lpKeyState, (LPWORD)&result, 0);
	return result;
}
int shift_active() {
	return GetKeyState(VK_LSHIFT) < 0 || GetKeyState(VK_RSHIFT) < 0;
}

int capital_active() {
	return (GetKeyState(VK_CAPITAL) & 1) == 1;
}
