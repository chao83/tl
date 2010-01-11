// TrayStart.cpp : 定义应用程序的入口点。
//


#ifdef USE_GDIPLUS
	#include <objbase.h>
	#include <gdiplus.h>
#endif

#include "MsgMap.h"
#include "TrayStart.h"
#include "language.h"
//#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE & GhInstRef()
{
	static HINSTANCE g_hInst;
	return g_hInst;
}							// 当前实例
void ThisHinstSet(HINSTANCE hInst)
{
	GhInstRef() = hInst;
}
const HINSTANCE ThisHinstGet()
{
	return GhInstRef();
}

//TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
const TCHAR *szWindowClass = _T("TrayLauncher_WindowClass");			// 主窗口类名
//TCHAR g_lpCmdLine[MAX_PATH]={0};
HWND & GHdlgRun();	//运行对话框


// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK ProcMessage(HWND, UINT, WPARAM, LPARAM);


void MustBeFirstInstance(const TCHAR * pTitle = NULL)
{
	HANDLE  hMutex;
	TCHAR Name[MAX_PATH] = TEXT("Lch_TestIfFirstInstance_");
	if (pTitle)
		memcpy(Name + _tcslen(Name),pTitle,_tcslen(pTitle) < MAX_PATH - 1 - _tcslen(Name) ?_tcslen(pTitle) : MAX_PATH - 1 - _tcslen(Name));
	hMutex = CreateMutex(
				NULL,			// No security descriptor
				FALSE,			// Mutex object not owned
				Name);			// Object name

	if (NULL == hMutex || ERROR_ALREADY_EXISTS == GetLastError())
	{
		HWND hPre = ::FindWindow(szWindowClass, NULL);
		if (hPre) {
			SetForegroundWindow(hPre);
			SendNotifyMessage(hPre,UM_NEWINSTANCE, 0,0);
			SetForegroundWindow(hPre);
		}
		else {
			MessageBox(NULL, _LNG("STR_Single_Instance_Only"), pTitle, MB_ICONERROR);
		}
		exit(0);
	}
}
#ifdef USE_GDIPLUS
class GDIplusIniter{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR		   gdiplusToken;
public:
	GDIplusIniter() {
		// Initialize GDI+.
		Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	}
	~GDIplusIniter() {
		Gdiplus::GdiplusShutdown(gdiplusToken);
	}
};
#endif
//! WinMain 程序入口
int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE /*hPrevInstance*/,
					 LPSTR	/*lpCmdLine*/,
					 int	   nCmdShow)
{
#ifdef USE_GDIPLUS
	const GDIplusIniter init;
#endif
	TCHAR path[MAX_PATH] = {0};
	int n = GetModuleFileName(NULL,path,MAX_PATH);
	while(n > 0 && path[n] != '\\') --n;
	path[n] = '\0';
	SetCurrentDirectory(path);

	InitLanguage();
	MustBeFirstInstance(_T("Tray Launcher"));
	MSG msg;

	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow)) {
		return FALSE;
	}

	//HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TRAYSTART));

	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0)) {
		if ((!GHdlgRun() || !IsDialogMessage(GHdlgRun(), &msg)) //&& !TranslateAccelerator(msg.hwnd, hAccelTable, &msg)
			) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return static_cast<int>(msg.wParam);
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
//  注释:
//
//	仅当希望
//	此代码与添加到 Windows 95 中的“RegisterClassEx”
//	函数之前的 Win32 系统兼容时，才需要此函数及其用法。调用此函数十分重要，
//	这样应用程序就可以获得关联的
//	“格式正确的”小图标。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= ProcMessage;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TRAYSTART));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}


void InitMsgMap();


//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
BOOL InitInstance(HINSTANCE hInstance, int /*nCmdShow*/)
{
   HWND hWnd;

   ThisHinstSet(hInstance); // 将实例句柄存储在全局变量中

   InitMsgMap();

   hWnd = CreateWindow(szWindowClass, NULL, WS_OVERLAPPED,
	  CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd) {
	  return FALSE;
   }
   return TRUE;
}
