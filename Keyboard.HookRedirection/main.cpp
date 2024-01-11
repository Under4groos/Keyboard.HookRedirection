


#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <iostream>
#include "KeyRedirect.cpp"
#include <vector>


using namespace std;

std::vector<KeyRedirect> KeysRedirect = {};
 

HHOOK hHook{ NULL };
KBDLLHOOKSTRUCT* kbdStruct;
LRESULT CALLBACK keyboard_hook(const int code, const WPARAM wParam, const LPARAM lParam) {
	if (wParam == WM_KEYDOWN) {
		kbdStruct = (KBDLLHOOKSTRUCT*)lParam;
	
		 
        cout << "scanCode:"<< kbdStruct->scanCode  << "  vkCode:" << kbdStruct->vkCode << endl;


        for (KeyRedirect n : KeysRedirect) {
            if (kbdStruct->vkCode != n.KeyDown)
                break;

            INPUT inputs[1] = {};
            ZeroMemory(inputs, sizeof(inputs));
            inputs[0].type = INPUT_KEYBOARD;
            inputs[0].ki.time = 0;

            inputs[0].ki.wVk = n.KeyRedirect;
            inputs[0].ki.dwFlags = 0;

            UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
            if (uSent != ARRAYSIZE(inputs))
            {
                cout << (L"Failed: 0x%x\n", HRESULT_FROM_WIN32(GetLastError())) << endl;
            }
            cout << "Redirect:" << n.KeyDown << " to " << n.KeyRedirect << endl;
            return 1;
        }

        
       
			 

		
	}
    
	return CallNextHookEx(hHook, code, wParam, (LPARAM)(kbdStruct));
}

int main()
{
    KeysRedirect.push_back(KeyRedirect{ 49,50 });
    for (KeyRedirect n : KeysRedirect) {
        cout << "KeyDown: " << n.KeyDown << "  KeyRedirect: " << n.KeyRedirect << endl;
    }

	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboard_hook, NULL, 0);
	if (hHook == NULL) {
		std::cout << "Keyboard hook failed!" << std::endl;
	}

	while (GetMessage(NULL, NULL, 0, 0));
	return 0;
}