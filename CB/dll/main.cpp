#include "main.h"

LRESULT CALLBACK MouseProc(int nCode,
		WPARAM wParam,
		LPARAM lParam
		)
{
	if (!(GetKeyState(VK_SHIFT) & 0x8000) && (HC_ACTION == nCode || HC_NOREMOVE == nCode)) {
		HWND hPre(0);
		switch (wParam) {
			case WM_MBUTTONUP:
				//����Ϣ
				hPre = ::FindWindow( L"TrayLauncher_WindowClass", NULL);
				if (hPre) {
					SetForegroundWindow(hPre);
					PostMessage(hPre, WM_USER + 4, GetKeyState(VK_LBUTTON) & 0x8000, 0);
					return TRUE;
				}
				break;
			default :
				break;
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

HHOOK &GHook()
{
	static HHOOK g_hHook;
	return g_hHook;
}

bool OpenHook(HINSTANCE hInst)
{
	if (!GHook()) {
		GHook() = SetWindowsHookEx(WH_MOUSE, MouseProc, hInst, 0);
	}
	return GHook();
}
void CloseHook(void)
{
	if (GHook()) {
		UnhookWindowsHookEx( GHook() );
	}

}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // attach to process
            // return FALSE to fail DLL load
            break;

        case DLL_PROCESS_DETACH:
            // detach from process
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            break;
    }
    return TRUE; // succesful
}
