
#include "stdafx.h"
#include <shellapi.h>
#include <shlwapi.h>
#include "resource.h"
#include "language.h"
#include "MenuWithIcon.h"
#include "GDIWavePic.h"
#include "RunDlg.h"
#include "MsgMap.h"
#include "SettingFile.h"

CSettingFile & Settings()
{
	static CSettingFile s_setting(_T("TL.ini"), true);
	return s_setting;
}

class CBLocker
{
	bool & m_value;
	CBLocker & operator = (const CBLocker &);
public:
	explicit CBLocker(bool & bV, bool value = true):m_value(bV) { m_value = value; }
	~CBLocker() { m_value ^= true; }
};

const int UM_MIDCLICK = WM_USER + 4;
class CHook
{
	HINSTANCE m_hinstDLL;
public:
	CHook(){
		m_hinstDLL = LoadLibrary(_T("TL.dll"));
		if (m_hinstDLL) {
			typedef bool (*F)(HINSTANCE);
			F OpenHook = (F)GetProcAddress(m_hinstDLL, "OpenHook");
			if (!OpenHook || !OpenHook(m_hinstDLL) ) {
					FreeLibrary(m_hinstDLL);
					m_hinstDLL = 0;
			}
		}
	}
	~CHook(){
		if (m_hinstDLL) {
			typedef void (*F)();
			F CloseHook = (F)GetProcAddress(m_hinstDLL, "CloseHook");
			if (CloseHook) {
				CloseHook();
			}
			FreeLibrary(m_hinstDLL);
			m_hinstDLL = 0;
		}
	}
	operator bool() {return !!m_hinstDLL;}
};

bool SwitchHook(bool bSet = false, bool bOn = true) {
	static Ptr<CHook> g_hook;
	if (bSet) {
		if (bOn) {
			g_hook = new CHook;
		}
		else {
			g_hook.Reset();
		}
	}
	return g_hook.Get() && static_cast<bool>(*g_hook);
}
const HINSTANCE ThisHinstGet();
//extern TCHAR g_lpCmdLine[MAX_PATH];
extern const TCHAR *szWindowClass;

const UINT ID_TASKBARICON = 10;
const UINT UM_ICONNOTIFY = WM_USER + 1;


TSTRING g_fileName = TEXT(".\\TLCmd.txt");

icon_ptr & GTrayIcon()
{
	static icon_ptr s_trayIcon;
	return s_trayIcon;
}


void ShowHiddenJudge(CMenuWithIcon *pMenu)
{
	TSTRING strShowHidden(_T("false"));
	if(Settings().Get(sectionGeneral, keyShowHidden, strShowHidden) && (strShowHidden == _T("true") || strShowHidden == _T("1")))	{
		pMenu->ShowHidden() = true;
	}
	else {
		pMenu->ShowHidden() = false;
		if(strShowHidden != _T("0"))
			Settings().Set(sectionGeneral, keyShowHidden, _T("0"), true);
	}
}



bool MClickJudge()
{
	TSTRING strValue(_T("false"));
	if (!Settings().Get(sectionGeneral, keyMClick, strValue) || (strValue != _T("true") && strValue != _T("1")) ) {
		if (strValue != _T("0")) {
			Settings().Set(sectionGeneral, keyMClick, _T("0"), true);
		}
		return false;
	}
	return true;
}




typedef LRESULT (*MsgProc)(HWND, UINT, WPARAM, LPARAM);
typedef std::map<const UINT,MsgProc> MsgMap;
typedef MsgMap :: const_iterator MsgIter;


enum EHOTKEYS {
	HOTKEYBEGIN = 1,
	HOTKEYPOPMENU = HOTKEYBEGIN,
	HOTKEYPOPSYSMENU,
	HOTKEYPOPEXECUTE,
	HOTKEYMIDCLICK,
	HOTKEYPOPSYSMENU_ALTER,
	HOTKEYEND
};


//UINT_PTR g_lastMenuSel = 0;

Ptr<COwnerDrawMenu> g_pSysTray;
Ptr<CMenuWithIcon> g_pTray;

bool ExecuteEx(const TSTRING & strToBeExecuted, const TCHAR * pOpr = NULL, HWND = NULL);


CMsgMap & TheMsgMap()
{
	static CMsgMap msgMap;
	return msgMap;
}

bool & IgnoreUser() 
{
	static bool s_bIgnoreUser = false;
	return s_bIgnoreUser;
}

//（消息）函数声明
LRESULT  MsgCreate		(HWND, UINT, WPARAM, LPARAM);
LRESULT  MsgIconNotify	(HWND, UINT, WPARAM, LPARAM);
LRESULT  MsgDestroy		(HWND, UINT, WPARAM, LPARAM);
LRESULT  MsgTaskbarCreated	(HWND, UINT, WPARAM, LPARAM);
LRESULT  MsgHotKey(HWND, UINT, WPARAM, LPARAM);
LRESULT  MsgClose(HWND, UINT, WPARAM, LPARAM);
LRESULT  MsgNewInstance(HWND, UINT, WPARAM, LPARAM);
LRESULT  MsgMidClick(HWND, UINT, WPARAM, LPARAM);
LRESULT  MsgEndSession(HWND, UINT, WPARAM, LPARAM);


BOOL  CALLBACK About(HWND, UINT, WPARAM, LPARAM);


enum MENU_ID_TYPE{BASE = 2000,CMDID_START = BASE,AUTOSTART = BASE,SEPRATER,RELOAD,EDITCMDS,EXIT,ABOUT, OPTION, MCLICK,RUNDLG,
	SKINIDSTART = 2050, SKINIDEND = 3000, LNGIDSTART = SKINIDEND+1, LNGIDEND = 3500, CMDID_END = LNGIDEND, MENUID_START = CMDID_END};
enum AUTORUN{AR_ADD,AR_REMOVE,AR_CHECK};
int AutoStart(AUTORUN);


int SetHotKeys(bool bReset = false)
{
	if (bReset)	{

	}
	return 0;
}


int BuildMenuFromFile(const TCHAR * strFile)
{
	file_ptr file(strFile,TEXT("rb"));
	int nItems = 0; //菜单项个数,返回值

	if(!file) {
		nItems = -1; // 打开文件错误
	}
	else if (_fgettc(file) != 0xfeff) {
		MessageBox(NULL, GetLang(_T("The command file is not saved in UNICODE(LE)! Please save it in UNICODE.")),NULL,MB_OK);
	}
	else {
		file.Reset();
		nItems = g_pTray->LoadMenuFromFile(strFile, MENUID_START);
		//nItems = g_pTray->BuildMenu(file,MENUID_START); //todo 也做成 非 成员函数??
	}
	return nItems;
}

void SetLanguage(const TSTRING & lngFile)
{
	bool bDefault = true;
	if (!SetLanguageFile(lngFile.c_str())) {
		Settings().Set(sectionGeneral, keyLanguage, _T(""),true);
	}
	else {
		Settings().Set(sectionGeneral, keyLanguage, lngFile,true);
		bDefault = false;
	}
	int iMenuPos = 0;
	g_pSysTray->SetNameByPos(iMenuPos,GetLang(_T("E&xit")));
	g_pSysTray->SetNameByPos(++iMenuPos,GetLang(_T("&Edit Command")));
	g_pSysTray->SetNameByPos(++iMenuPos,GetLang(_T("&Refresh")));
	g_pSysTray->SetNameByPos(++iMenuPos,GetLang(_T("Select &Skin")));
	g_pSysTray->SetNameByPos(++iMenuPos,GetLang(_T("&Language")));
	g_pSysTray->SetNameByPos(++iMenuPos,TSTRING(GetLang(_T("Run"))) + ( bDefault ? _T(" ...  Ctrl+LWin") : _T(" ...") ) );
	g_pSysTray->SetNameByPos(++iMenuPos,GetLang(_T("Use Mid Click")));
	g_pSysTray->SetNameByPos(++iMenuPos,GetLang(_T("Start With &Windows")));
	g_pSysTray->SetNameByPos(++iMenuPos,GetLang(_T("&About")));
	g_pSysTray->SetName(LNGIDSTART, GetLang(_T("Internal")));
	g_pSysTray->SetName(SKINIDSTART, GetLang(_T("Internal")));
	g_pSysTray->UpdateRoot();

	HMENU hLngMenu = GetSubMenu(g_pSysTray->Menu(), 4);
	if(IsMenu(hLngMenu)) {
		unsigned int count = GetMenuItemCount(hLngMenu);
		for (unsigned int i = 0; i < count; ++i) {
			CheckMenuItem( hLngMenu, i, MF_BYPOSITION | MF_UNCHECKED);
		}
		if (lngFile.length() && lngFile != _T("")) {
			for (unsigned int i = 1; i < count; ++i) {
				if (lngFile == g_pSysTray->Name(GetMenuItemID(hLngMenu, i))) {
					CheckMenuItem( hLngMenu, i, MF_BYPOSITION | MF_CHECKED);
					return;
				}
			}
		}
		CheckMenuItem(hLngMenu, 0, MF_BYPOSITION | MF_CHECKED);
	}
}

// Get the count of an array of any type
template <class TItem, unsigned int N>
inline unsigned int ArrayN (const TItem (&)[N])
{
	return N;
}

//! 设置菜单皮肤
void SetMenuSkin(const TSTRING & skinSubDir)
{
	const int nPicPerItem = 3;
	if (skinSubDir.empty() || skinSubDir == _T("")) {
		HBITMAP hBit[nPicPerItem] = {0};
		g_pTray->SetSkin(0, hBit, hBit, hBit, 0);
	}
	else {
		const TCHAR * szSkinBk[] = {TEXT("bk.bmp"), TEXT("bkLeft.bmp"), TEXT("bkRight.bmp")};
		const TCHAR * szSkinSelBk[] = {TEXT("selbk.bmp"), TEXT("selbkLeft.bmp"), TEXT("selbkRight.bmp")};
		const TCHAR * szSkinSep[] = {TEXT("sep.bmp"), TEXT("sepLeft.bmp"), TEXT("sepRight.bmp")};
		TSTRING strSkinPath = _T(".\\skin\\") + skinSubDir + _T("\\");
		TSTRING strSkinPicPath;
		HBITMAP hBitBk[nPicPerItem];
		HBITMAP hBitSel[nPicPerItem];
		HBITMAP hSep[nPicPerItem];
		assert(ArrayN(hBitBk) == ArrayN(szSkinBk));
		assert(ArrayN(hSep) == ArrayN(szSkinSep));
		assert(ArrayN(hBitSel) == ArrayN(szSkinSelBk));
		for (int i = 0; i < nPicPerItem; ++i) {
			strSkinPicPath = strSkinPath + szSkinBk[i];
			hBitBk[i] = (HBITMAP)LoadImage(ThisHinstGet(), strSkinPicPath.c_str(), IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		}
		for (int i = 0; i < nPicPerItem; ++i) {
			strSkinPicPath = strSkinPath + szSkinSelBk[i];
			hBitSel[i] = (HBITMAP)LoadImage(ThisHinstGet(), strSkinPicPath.c_str(), IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		}
		for (int i = 0; i < nPicPerItem; ++i) {
			strSkinPicPath = strSkinPath + szSkinSep[i];
			hSep[i] = (HBITMAP)LoadImage(ThisHinstGet(), strSkinPicPath.c_str(), IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		}
		strSkinPicPath = strSkinPath + TEXT("side.bmp");
		HBITMAP hSide = ((HBITMAP)LoadImage(ThisHinstGet(),strSkinPicPath.c_str(),IMAGE_BITMAP,0,0,LR_LOADFROMFILE));
		strSkinPicPath = strSkinPath + TEXT("tital.bmp");
		HBITMAP hTital = ((HBITMAP)LoadImage(ThisHinstGet(),strSkinPicPath.c_str(),IMAGE_BITMAP,0,0,LR_LOADFROMFILE));

		g_pTray->SetSkin(hSide, hBitBk, hBitSel, hSep, hTital);
	}

	HMENU hSkinMenu = GetSubMenu(g_pSysTray->Menu(), 3);
	if(IsMenu(hSkinMenu)) {
		unsigned int count = GetMenuItemCount(hSkinMenu);
		for (unsigned int i = 0; i < count; ++i) {
			CheckMenuItem( hSkinMenu, i, MF_BYPOSITION | MF_UNCHECKED);
		}
		if (skinSubDir.length()) {
			for (unsigned int i = 1; i < count; ++i) {
				if (skinSubDir == g_pSysTray->Name(GetMenuItemID(hSkinMenu, i))) {
					CheckMenuItem( hSkinMenu, i, MF_BYPOSITION | MF_CHECKED);
					Settings().Set(sectionGeneral, keySkin, skinSubDir, true);
					return;
				}
			}
		}
		CheckMenuItem(hSkinMenu, 0, MF_BYPOSITION | MF_CHECKED);
		Settings().Set(sectionGeneral, keySkin, _T(""), true);
	}
}


// 操作托盘图标,添加,更新或删除
void Systray(const HWND hWnd, const DWORD dwMessage, ICONTYPE hIcon = NULL)
{
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = ID_TASKBARICON;
	nid.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;
	nid.uCallbackMessage = UM_ICONNOTIFY;
	nid.hIcon = hIcon ? hIcon : LoadIcon(ThisHinstGet(), MAKEINTRESOURCE(IDI_TRAYSTART));
	TCHAR str[] = _T("Tray Launcher");
	int nCount = sizeof(str)/sizeof(TCHAR);
	for (int i = 0; i < nCount; ++i) {
		nid.szTip[i] = str[i];
	}
	Shell_NotifyIcon(dwMessage, &nid);
}

void ShowMenu(const POINT * p = NULL, bool bLast = false)
{
	if (!IgnoreUser()) {
		static POINT s_point = {0,0};
		if(p) {
			s_point = *p;
		}
		else if (!bLast) {
			GetCursorPos(&s_point);
		}
		if (g_pTray->Display(s_point.x, s_point.y) ) {
			SetProcessWorkingSetSize(GetCurrentProcess(),static_cast<DWORD>(-1), static_cast<DWORD>(-1));
		}
	}
}
void UpdataRunDlgCheck()
{
	const int ipos = 5;
	if ( GHdlgRun() && IsWindowVisible( GHdlgRun() ) ) {
		CheckMenuItem(g_pSysTray->Menu(),ipos, MF_BYPOSITION | MF_CHECKED );
	}
	else {
		CheckMenuItem(g_pSysTray->Menu(),ipos, MF_BYPOSITION | MF_UNCHECKED );
	}
}
//显示运行对话框
void ShowRunDlg()
{
	if (IgnoreUser()) return;

	if (!GHdlgRun()) {
		GHdlgRun() = CreateDialog(ThisHinstGet(),MAKEINTRESOURCE(IDD_RUN), NULL, RunDlgProc);
		ShowWindow(GHdlgRun(), SW_SHOWNORMAL);
	}
	else if (IsWindowVisible(GHdlgRun())) {
		ShowWindow(GHdlgRun(), SW_HIDE);
	}
	else {
		ShowWindow(GHdlgRun(), SW_SHOW);
	}
}

void ShowAbout()
{
	CBLocker locker(IgnoreUser());
	assert(IgnoreUser());
	DialogBox(ThisHinstGet(), MAKEINTRESOURCE(IDD_ABOUTBOX), NULL, About);
}

int MyProcessCommand(HWND hWnd, int id)
{
	if(IgnoreUser() || id >= MENUID_START || id < CMDID_START)
		return id;
	switch (id) {
		case ABOUT:
			if (GHdlgRun() && IsWindowVisible(GHdlgRun())) {
				ShowRunDlg();
				ShowAbout();
				//IgnoreUser() = true;
				//DialogBox(ThisHinstGet(), MAKEINTRESOURCE(IDD_ABOUTBOX), NULL, About);
				//IgnoreUser() = false;
				ShowRunDlg();
			}
			else {
				ShowAbout();
			}
			assert(!IgnoreUser());

			break;
		case EXIT:
			IgnoreUser() = (IDYES == MessageBox(NULL, GetLang(_T("Exit Tray Launcher ?")),GetLang(_T("Confirm:")),MB_YESNO | MB_TOPMOST));//true;
			if (IgnoreUser()) {
				if (GHdlgRun()) {
					DestroyWindow(GHdlgRun());
					GHdlgRun() = NULL;
				}
				DestroyWindow(hWnd);
			}
			return 0;
			//break;
		case EDITCMDS:
			if(!ShellSuccess(ShellExecute(NULL,NULL,g_fileName.c_str(),NULL,NULL,SW_SHOW))) {
				//执行命令失败
				if(ShellSuccess(ShellExecute(NULL,_T("open"),_T("notepad.exe"),g_fileName.c_str(),NULL,SW_SHOW)))
					 break;
				 MessageBox(NULL,GetLang(_T("Can not open or create the command file!")),NULL,MB_ICONERROR);
			}
			break;
		case RELOAD:
			if (Settings().Read())
				ShowHiddenJudge(g_pTray.Get());
			BuildMenuFromFile(g_fileName.c_str());
			ShowMenu(0,true);
			break;
		case RUNDLG:
			ShowRunDlg();
			break;
		case AUTOSTART:
			if (GetMenuState(g_pSysTray->Menu(),AUTOSTART,MF_BYCOMMAND) & MF_CHECKED) {
				if (0 == AutoStart(AR_REMOVE))
					CheckMenuItem(g_pSysTray->Menu(),AUTOSTART,MF_BYCOMMAND | MF_UNCHECKED);
			}
			else {
				if (1 == AutoStart(AR_ADD))
					CheckMenuItem(g_pSysTray->Menu(),AUTOSTART,MF_BYCOMMAND | MF_CHECKED);
			}
			break;
		case OPTION:
			//todo Now it's for test only
			if(!ShellSuccess(ShellExecute(NULL,NULL, _T(".\\TL.ini"),NULL,NULL,SW_SHOW))) {
				//执行命令失败
				ShellSuccess(ShellExecute(NULL,_T("open"),_T("notepad.exe"), _T(".\\TL.ini"), NULL,SW_SHOW));
			}
			break;
		case MCLICK:
			//
			if ( SwitchHook(true, !SwitchHook() ) ) {
				CheckMenuItem(g_pSysTray->Menu(),MCLICK,MF_BYCOMMAND | MF_CHECKED);
				Settings().Set(sectionGeneral, keyMClick, _T("1"));
			}
			else {
				CheckMenuItem(g_pSysTray->Menu(),MCLICK,MF_BYCOMMAND | MF_UNCHECKED);
				Settings().Set(sectionGeneral, keyMClick, _T("0"));
			}
			break;
		case SKINIDSTART:
			SetMenuSkin(_T(""));
			break;
		case LNGIDSTART:
			SetLanguage(_T(""));
			break;
		default:
			if (id > SKINIDSTART && id < SKINIDEND) {
				const TCHAR * pSkinDir = g_pSysTray->Name(id);
				if (pSkinDir && *pSkinDir)
					SetMenuSkin(pSkinDir);
			}
			else if (id > LNGIDSTART && id < LNGIDEND) {
				const TCHAR * pLngFile = g_pSysTray->Name(id);
				if (pLngFile && *pLngFile)
					SetLanguage(pLngFile);
			}
			break;
	}
	SetProcessWorkingSetSize(GetCurrentProcess(),static_cast<DWORD>(-1), static_cast<DWORD>(-1));
	return id;
}


// 初始化，添加消息映射
void InitMsgMap()
{
	TheMsgMap().Add(WM_CREATE,		&MsgCreate);
	TheMsgMap().Add(WM_DESTROY,		&MsgDestroy);
	TheMsgMap().Add(UM_ICONNOTIFY,	&MsgIconNotify);
	TheMsgMap().Add(WM_HOTKEY,		&MsgHotKey);
	TheMsgMap().Add(WM_CLOSE,		&MsgClose);
	TheMsgMap().Add(UM_NEWINSTANCE, &MsgNewInstance);
	TheMsgMap().Add(UM_MIDCLICK,	&MsgMidClick);
	TheMsgMap().Add(WM_ENDSESSION,	&MsgEndSession);
}


//! 处理来自另一个进程的通知
LRESULT  MsgNewInstance(HWND hWnd, UINT, WPARAM, LPARAM)
{
	Sleep(100);//等待发送方退出。
	Systray(hWnd,NIM_ADD,GTrayIcon().Get());
	ShowMenu();
	return 0;
}

//! 处理来自另一个进程的通知
LRESULT  MsgMidClick(HWND hWnd, UINT, WPARAM bDown, LPARAM)
{
	if(bDown || (GetKeyState(VK_LBUTTON)&0x8000) ) {
		if (!IgnoreUser()) {
			POINT point = {0,0};
			GetCursorPos(&point);
			UpdataRunDlgCheck();
			MyProcessCommand(hWnd, g_pSysTray->Display(point.x,point.y));
		}
	}
	else {
		Sleep(100);
		ShowMenu();
	}
	return 0;
}


//! 处理关机，注销等 通知
LRESULT  MsgEndSession(HWND, UINT, WPARAM wParam, LPARAM) // WM_ENDSESSION
{
	if (wParam) {
		// 即将关闭会话
		Settings().Save();
	}
	return 0;
}

//
//  函数: ProcMessage(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
// 分配消息
LRESULT CALLBACK ProcMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result(0xdeadbeef);
	if (TheMsgMap().ProcessMessage(hWnd, message, wParam, lParam, &result))
		return result;
	else
		return DefWindowProc(hWnd, message, wParam, lParam);
}


bool AddHotkey(HWND hWnd, int id, UINT fsModifiers, UINT vk)
{
	if (!RegisterHotKey(hWnd, id, fsModifiers, vk)) {
		MessageBox(NULL,_T("Register Hot Key Failed!"),NULL,MB_OK);
		return false;
	}
	return true;
}


#ifdef _DEBUG
void _DbgTest()
{
	SHFILEINFO sfi = {0};

	SHGetFileInfo(_T("e:\\"), FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi),SHGFI_TYPENAME);
	SHGetFileInfo(_T("e:\\"), FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi),SHGFI_DISPLAYNAME);
	//const Ptr<int> b = a;
	return;
}
#endif

// 以下是不同消息相应的处理函数 : Msg....(HWND, UINT, WPARAM, LPARAM) ;
// WM_CREATE
LRESULT  MsgCreate(HWND hWnd, UINT /*message*/, WPARAM /* wParam */, LPARAM /* lParam */)
{
	//*
#ifdef _DEBUG
	_DbgTest();
#endif

	// */
	for (int i = CMDID_START; i < CMDID_END; ++i) {
		if (IsMenu( (HMENU)i ) )
			DestroyMenu( (HMENU)i );
	}
	UINT WM_TASKBARCREATED = RegisterWindowMessage(_T("TaskbarCreated")); // 获取托盘重建消息，恢复托盘图标。
	if (WM_TASKBARCREATED != 0)
		TheMsgMap().Add(WM_TASKBARCREATED, &MsgTaskbarCreated);

	ICONTYPE hIconCheck = (ICONTYPE)LoadImage(ThisHinstGet(), MAKEINTRESOURCE(IDI_RIGHT),IMAGE_ICON,0,0,LR_DEFAULTCOLOR);

	g_pSysTray = new COwnerDrawMenu(hIconCheck);
	g_pSysTray->Insert(EXIT,GetLang(_T("E&xit")));
	g_pSysTray->Insert(EDITCMDS,GetLang(_T("&Edit Command")));
	g_pSysTray->Insert(RELOAD,GetLang(_T("&Refresh")));
	g_pSysTray->Insert(RUNDLG,(TSTRING(GetLang(_T("Run"))) + _T("\t  Ctrl+Win")).c_str());
	g_pSysTray->Insert(MCLICK,GetLang(_T("Use Mid Click")));
	g_pSysTray->Insert(AUTOSTART,GetLang(_T("Start With &Windows")));
	g_pSysTray->Insert(ABOUT,GetLang(_T("&About")));
	//*

	//构造skin选项
	HMENU hSkinMenu = CreatePopupMenu();
	if (IsMenu(hSkinMenu) ) {
		InsertMenu(hSkinMenu, static_cast<UINT>(-1), MF_BYPOSITION | MF_STRING, SKINIDSTART, GetLang(_T("Internal")));

		WIN32_FIND_DATA fd = {0};
		HANDLE handle = INVALID_HANDLE_VALUE;
		const TCHAR * const pSearch = _T(".\\skin\\*");
		handle = FindFirstFile(pSearch,&fd);
		if (handle != INVALID_HANDLE_VALUE) {
			std::map<TSTRING,TSTRING> nameName;
			do {
				//只处理文件夹， //跳过 "." 和 ".." 目录
				if((!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) ||
					fd.cFileName[0] == '.' )
					continue;

				////文件名作为菜单名时，其中的 '&' 扩展成  '&&'
				TSTRING strFileName(fd.cFileName);
				TCHAR ch('&');
				TSTRING & str = strFileName;
				TSTRING::size_type pos = 0;
				while((pos = str.find(ch,pos)) != str.npos) {
					str.insert(pos,1,ch);
					pos+=2;
				}

				// 忽略大小写
				TSTRING strNameLower(strFileName);
				TSTRING::size_type size = str.length();
				for (TSTRING::size_type i = 0; i < size; ++i) {
					str[i] = _totlower(str[i]);
				}

				nameName[strNameLower] = strFileName;

			} while (FindNextFile(handle,&fd));
			FindClose(handle);


			int id = SKINIDSTART;
			std::map<TSTRING,TSTRING>::iterator itStr,itName;
			for (itName = nameName.begin(); itName != nameName.end() && ++id < SKINIDEND; ++itName) {
				InsertMenu(hSkinMenu,static_cast<UINT>(-1), MF_BYPOSITION | MF_STRING, id, itName->second.c_str());
			}
		}
		g_pSysTray->Insert(hSkinMenu, GetLang(_T("Select &Skin")), 3,(ICONTYPE)LoadImage(ThisHinstGet(), MAKEINTRESOURCE(IDI_SKIN),IMAGE_ICON,0,0,LR_DEFAULTCOLOR));
	}


	//构造language
	HMENU hLngMenu = CreatePopupMenu();
	if (IsMenu(hLngMenu) ) {
		InsertMenu(hLngMenu, static_cast<UINT>(-1), MF_BYPOSITION | MF_STRING, LNGIDSTART, GetLang(_T("Internal")));

		WIN32_FIND_DATA fd = {0};
		HANDLE handle = INVALID_HANDLE_VALUE;
		const TCHAR * const pSearch = _T(".\\Lng\\*.lng");
		handle = FindFirstFile(pSearch,&fd);
		if (handle != INVALID_HANDLE_VALUE) {
			std::map<TSTRING,TSTRING> nameName;
			do {
				//跳过目录
				if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					continue;

				////文件名作为菜单名时，其中的 '&' 扩展成  '&&'
				TSTRING strFileName(fd.cFileName);
				TCHAR ch('&');
				TSTRING & str = strFileName;
				TSTRING::size_type pos = 0;
				while((pos = str.find(ch,pos)) != str.npos) {
					str.insert(pos,1,ch);
					pos+=2;
				}

				// 忽略大小写
				TSTRING strNameLower(strFileName);
				TSTRING::size_type size = str.length();
				for (TSTRING::size_type i = 0; i < size; ++i) {
					str[i] = _totlower(str[i]);
				}

				nameName[strNameLower] = strFileName;

			} while (FindNextFile(handle,&fd));
			FindClose(handle);

			int id = LNGIDSTART;
			std::map<TSTRING,TSTRING>::iterator itStr,itName;
			for (itName = nameName.begin(); itName != nameName.end() && ++id < LNGIDEND; ++itName) {
				InsertMenu(hLngMenu,static_cast<UINT>(-1), MF_BYPOSITION | MF_STRING, id, itName->second.c_str());
			}
			//g_pSysTray->Insert(hLngMenu, GetLang(_T("&Language")), 4,(ICONTYPE)LoadImage(ThisHinstGet(), MAKEINTRESOURCE(IDI_LNG),IMAGE_ICON,0,0,LR_DEFAULTCOLOR));
		}
		g_pSysTray->Insert(hLngMenu, GetLang(_T("&Language")), 4,(ICONTYPE)LoadImage(ThisHinstGet(), MAKEINTRESOURCE(IDI_LNG),IMAGE_ICON,0,0,LR_DEFAULTCOLOR));

	}
//#ifdef _DEBUG
//		InsertMenu(hOptionMenu, 0, MF_BYPOSITION | MF_STRING, OPTION, GetLang(_T("Settings")));
//#endif

		//g_pSysTray->Insert(hOptionMenu, GetLang(_T("Option")), 3,(ICONTYPE)LoadImage(ThisHinstGet(), MAKEINTRESOURCE(IDI_OPTION),IMAGE_ICON,0,0,LR_DEFAULTCOLOR));

//#ifdef _DEBUG
//		g_pSysTray->AddStaticIcon(OPTION,(ICONTYPE)LoadImage(ThisHinstGet(), MAKEINTRESOURCE(IDI_OPTION),IMAGE_ICON,0,0,LR_DEFAULTCOLOR));
//#endif

		//EnableMenuItem(hOptionMenu,0,MF_BYPOSITION | MF_ENABLED);
	// */
	g_pSysTray->AddStaticIcon(EXIT,(ICONTYPE)LoadImage(ThisHinstGet(), MAKEINTRESOURCE(IDI_EXIT),IMAGE_ICON,0,0,LR_DEFAULTCOLOR));
	g_pSysTray->AddStaticIcon(EDITCMDS,(ICONTYPE)LoadImage(ThisHinstGet(), MAKEINTRESOURCE(IDI_EDIT),IMAGE_ICON,0,0,LR_DEFAULTCOLOR));
	g_pSysTray->AddStaticIcon(RELOAD,(ICONTYPE)LoadImage(ThisHinstGet(), MAKEINTRESOURCE(IDI_REFRESH),IMAGE_ICON,0,0,LR_DEFAULTCOLOR));
	g_pSysTray->AddStaticIcon(ABOUT,(ICONTYPE)LoadImage(ThisHinstGet(), MAKEINTRESOURCE(IDI_PENCIL),IMAGE_ICON,0,0,LR_DEFAULTCOLOR));

	ICONTYPE hIconClose = (ICONTYPE)LoadImage(ThisHinstGet(), MAKEINTRESOURCE(IDI_CLOSE),IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
	ICONTYPE hIconOpen = (ICONTYPE)LoadImage(ThisHinstGet(), MAKEINTRESOURCE(IDI_OPEN),IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
	ICONTYPE hIconUnknownFile = (ICONTYPE)LoadImage(ThisHinstGet(), MAKEINTRESOURCE(IDI_UNKNOWN),IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
	g_pTray = new CMenuWithIcon(hIconOpen,hIconClose,hIconUnknownFile, GetLang(_T("Empty")));

	// set skin
	TSTRING strValue;
	if(!Settings().Get(sectionGeneral, keySkin, strValue)) {
		strValue = _T("");
	}
	SetMenuSkin(strValue);
	// set Lng
	if(!Settings().Get(sectionGeneral, keyLanguage, strValue)) {
		strValue = _T("");
	}
	SetLanguage(strValue);

	ShowHiddenJudge(g_pTray.Get());

	TSTRING strFileName(g_fileName);
	if(Settings().Get(sectionGeneral, keyCommand, strFileName))	{
		g_fileName = strFileName;
	}
	else {
		Settings().Set(sectionGeneral, keyCommand, g_fileName, true);
	}

	BuildMenuFromFile(g_fileName.c_str());

	if (AutoStart(AR_CHECK) == 1)
		CheckMenuItem(g_pSysTray->Menu(), AUTOSTART,MF_BYCOMMAND | MF_CHECKED);
	else
		CheckMenuItem(g_pSysTray->Menu(), AUTOSTART,MF_BYCOMMAND | MF_UNCHECKED);

	AddHotkey(hWnd,HOTKEYPOPMENU,MOD_ALT | MOD_WIN,VK_LWIN);
	AddHotkey(hWnd,HOTKEYPOPSYSMENU,MOD_ALT | MOD_WIN,VK_RWIN);
	AddHotkey(hWnd,HOTKEYPOPEXECUTE,MOD_WIN | MOD_CONTROL, VK_LWIN);
	AddHotkey(hWnd,HOTKEYMIDCLICK,MOD_SHIFT | MOD_WIN, VK_LWIN);
	AddHotkey(hWnd,HOTKEYPOPSYSMENU_ALTER,MOD_ALT | MOD_CONTROL | MOD_WIN, VK_LWIN);
	

	//尝试读取用户自定义图标
	TSTRING strIcon;
	if (!Settings().Get(sectionGeneral, keyRunIcon, strIcon)) {
		strIcon = TEXT(".\\TLRun.ico");
		Settings().Set(sectionGeneral, keyRunIcon, strIcon,true);
	}
	GRunIcon() = (ICONTYPE)LoadImage(ThisHinstGet(),strIcon.c_str(),IMAGE_ICON,0,0,LR_LOADFROMFILE);

	if (!Settings().Get(sectionGeneral, keyTrayIcon, strIcon)) {
		strIcon = TEXT(".\\TLTray.ico");
		Settings().Set(sectionGeneral, keyTrayIcon, strIcon,true);
	}
	GTrayIcon() = (ICONTYPE)LoadImage(ThisHinstGet(),strIcon.c_str(),IMAGE_ICON,0,0,LR_LOADFROMFILE);
	Systray(hWnd,NIM_ADD,GTrayIcon().Get());

	// 读取历史记录
	std::vector<TSTRING> vHisKey, vHisValue;
	Settings().GetSection(sectionHistory, vHisKey, vHisValue);
	InitHistory(vHisValue);


	if ( SwitchHook(true, MClickJudge()) ) {
		CheckMenuItem(g_pSysTray->Menu(),MCLICK,MF_BYCOMMAND | MF_CHECKED);
	}
	else {
		CheckMenuItem(g_pSysTray->Menu(),MCLICK,MF_BYCOMMAND | MF_UNCHECKED);
	}
	SetProcessWorkingSetSize(GetCurrentProcess(),static_cast<DWORD>(-1), static_cast<DWORD>(-1));

	return 0;
}


//! 关闭消息
LRESULT  MsgClose(HWND, UINT, WPARAM, LPARAM)
{	// 屏蔽关闭消息
	return 0;
}

//! 全局快捷键
LRESULT MsgHotKey(HWND hWnd, UINT /*message*/, WPARAM wParam, LPARAM /*lParam*/)
{
	if(IgnoreUser() || wParam < HOTKEYBEGIN || wParam >= HOTKEYEND) {
		;
	}
	else {
		POINT point;
		switch (wParam) {
			case HOTKEYMIDCLICK:
				ShowMenu();
				break;
			case HOTKEYPOPMENU://左键菜单
				point.x = 0;
				point.y = 0;
				ShowMenu(&point);
				break;
			case HOTKEYPOPSYSMENU://右键菜单
			case HOTKEYPOPSYSMENU_ALTER:
				UpdataRunDlgCheck();
				MyProcessCommand(hWnd, g_pSysTray->Display(0, 0));
				break;
			case HOTKEYPOPEXECUTE://显示运行对话框
				ShowRunDlg();
				break;
			default:
				break;
		}
	}
	return 0;
}


//! explorer 重启后恢复托盘图标。
LRESULT  MsgTaskbarCreated(HWND hWnd, UINT /*message*/, WPARAM /*wParam*/, LPARAM /* lParam */)
{
	Sleep(2000);
	Systray(hWnd,NIM_ADD,GTrayIcon().Get());
	return 0;
}



//! 退出程序
LRESULT  MsgDestroy(HWND hWnd, UINT /*message*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	IgnoreUser() = true;
	Systray(hWnd,NIM_DELETE,GTrayIcon().Get());
	//GTrayIcon().Free();
	//GRunIcon().Free();

	for (int i = HOTKEYBEGIN; i < HOTKEYEND; ++i)
		UnregisterHotKey(hWnd,i);
	g_pSysTray.Reset();
	g_pTray.Reset();
	PostQuitMessage(0);
	return 0;
}


//! 托盘图标操作。
LRESULT  MsgIconNotify(HWND hWnd, UINT /*message*/, WPARAM /*wParam*/, LPARAM lParam)
{
	if (!IgnoreUser()) {
		static bool bLBtnDown = false;
		static bool bRBtnDown = false;
		static POINT ptLast;

		switch(lParam) {
			case WM_LBUTTONDOWN:
				bLBtnDown = true;
				bRBtnDown = false;
				GetCursorPos(&ptLast);
				break;
			case WM_RBUTTONDOWN:
				bRBtnDown = true;
				bLBtnDown = false;
				GetCursorPos(&ptLast);
				break;
			case WM_MOUSEMOVE:
				if (bLBtnDown || bRBtnDown) {
					POINT point;
					GetCursorPos(&point);
					if (point.x != ptLast.x || point.y != ptLast.y)
						bLBtnDown = bRBtnDown = false;
				}
				break;
			case WM_LBUTTONUP:
				if (bLBtnDown) {
					bLBtnDown = false;
					ShowMenu();
				}
				break;
			case WM_RBUTTONUP:
				if (bRBtnDown) {
					bRBtnDown = false;
					POINT point = {0,0};
					GetCursorPos(&point);
					UpdataRunDlgCheck();
					MyProcessCommand(hWnd, g_pSysTray->Display(point.x, point.y));
				}
				break;
			default:
				break;
		}
	}
	return 0;
}


//! 将指定窗口移动到屏幕中央
void CentralWindow(const HWND hWnd,const HWND hParentWnd = NULL)
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	int ww = rect.right - rect.left;
	int wh = rect.bottom - rect.top;
	GetWindowRect(hParentWnd ? hParentWnd : GetDesktopWindow(), &rect);
	int dw = rect.right - rect.left;
	int dh = rect.bottom - rect.top;

	MoveWindow(hWnd,rect.left + ((dw - ww)>>1), rect.top + ((dh - wh)>>1), ww, wh, TRUE);
   // 总在最上。
//	SetWindowPos(hWnd, HWND_TOPMOST, rect.left + ((dw - ww)>>1), rect.top + ((dh - wh)>>1), ww, wh,SWP_SHOWWINDOW);
}

int QuoteString(TSTRING &str, const TSTRING::value_type ch = '\"', bool bProcessEmpty = false);
//! 切换开机启动
int AutoStart(AUTORUN action)
{
	int result = 0;
	HKEY hKeyRun;
	TCHAR strSubKey[] = TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER,strSubKey,0,KEY_ALL_ACCESS,&hKeyRun)) {
		result = -1;
	}
	else {
		TCHAR name[] = TEXT("Tray Launcher");
		TCHAR path[MAX_PATH] = {0};
		DWORD len = MAX_PATH;

		switch (action) {
			case AR_ADD:
				len = GetModuleFileName(NULL,path,len);
				if (len > 0) {
					TSTRING str (path);
					QuoteString(str);
					result = ( ERROR_SUCCESS == RegSetValueEx(hKeyRun,name,0,REG_SZ,(const BYTE *)str.c_str(),sizeof(TCHAR)*(int)(str.size()+1) ) );
				}
				break;
			case AR_REMOVE:
				RegDeleteValue(hKeyRun,name);
				break;
			case AR_CHECK:
				if (ERROR_SUCCESS == RegQueryValueEx(hKeyRun,name,NULL,NULL, reinterpret_cast<LPBYTE>(path), &len)) {

					TCHAR path2[MAX_PATH] = {0};
					GetModuleFileName(NULL,path2,len);
					TSTRING str (path2);
					QuoteString(str);
					if (str == path) {
						result = 1;
					}
					else
						result = 2;
				}
				break;
			default:
				break;
		}
		RegCloseKey(hKeyRun);
	}
	return result;
}



////////////////////////////////////////////////////////////////////////////////
//////////
//////////						"关于" 对话框部分
//////////
////////////////////////////////////////////////////////////////////////////////



// “关于”框的消息处理程序。
BOOL  CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	const UINT uTimerID = 1;
	static bool bCloseWindow = true;
	static bool bUserLogo = false;
	static bool bRain = true;
	static Ptr<CGDIWavePic> pWavepic;

	const int wLimit = 82;
	const int hLimit = 110;
	const int iTimerInterval = 19;
	static int iTimerEnter = 0;
	static int iTimerDrawEvery = 0;
	static short DropDepth = 16;
	static RECT rcPic = {0,0,1,1};
	static int rgbDst[3] = {160,160,240};

	HDC hdc = NULL;
	RECT rectClient = {0,0,1,1};
	bool bResult = false;

	switch (message) {
		case WM_TIMER:
			if (// bUserLogo && 
				bRain && (++iTimerEnter > iTimerDrawEvery)) {
				iTimerEnter = 0;
				DWORD dwTime = GetTickCount() & 0xffffff;
				srand((dwTime << 5) - dwTime);// srand(dwTime*31);
				pWavepic->Drop( rcPic.left + rand() % (rcPic.right - rcPic.left + 1),
					rcPic.top + rand() % (rcPic.bottom - rcPic.top + 1),
					rand() % (DropDepth << 3), rand() % 3 + 1);
			}

			hdc = GetDC(hDlg);

			GetClientRect(hDlg,&rectClient);

			pWavepic->DrawWave(hdc);

			ReleaseDC(hDlg,hdc);
			bResult = TRUE;
			break;

		case WM_INITDIALOG:
			SetWindowLong(hDlg,GWL_STYLE,WS_POPUP);

			CentralWindow(hDlg,NULL);
			GetClientRect(hDlg,&rectClient);
			SetWindowPos(hDlg,HWND_TOPMOST,0,0,	((rectClient.right-rectClient.left) >> 1 ) << 1,
				((rectClient.bottom-rectClient.top) >> 1 ) << 1, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_FRAMECHANGED);

			bCloseWindow = false;
			SetTimer(hDlg, uTimerID, iTimerInterval, NULL);

			//if (pWavepic) {
			//	bResult = TRUE;
			//	break;
			//}
			//else 
			if (NULL != (hdc = GetWindowDC(hDlg))) {
				SetBkMode(hdc, TRANSPARENT);
				// 允许 TLLogo.bmp 自定义关于对话框 logo
				gdi_ptr<HBITMAP> hBitmap ((HBITMAP)LoadImage(ThisHinstGet(),TEXT("TLLogo.bmp"),IMAGE_BITMAP,0,0,LR_LOADFROMFILE));
				if (hBitmap) {
					BITMAP bmp = {0};
					GetObject(hBitmap,sizeof(bmp),&bmp);
					DropDepth = 16;
					bUserLogo = true;
					if(bmp.bmHeight > hLimit - 2 || bmp.bmWidth > wLimit - 2) {
						hBitmap = NULL;
					}
				}
				if (!hBitmap) {

					bUserLogo = false;
					MemDC_H hMemDC(CreateCompatibleDC(hdc));
					hBitmap = CreateCompatibleBitmap(hdc,32,32);
					HBITMAP hold = (HBITMAP)SelectObject(hMemDC,hBitmap);
					ICONTYPE icon = LoadIcon(ThisHinstGet(), MAKEINTRESOURCE(IDI_TRAYSTART));
					DrawIcon(hMemDC,0,0,icon);

					SelectObject(hMemDC,hold);
					DropDepth = 8;
				}
				if (hBitmap) {
					BITMAP bmp = {0};
					GetObject(hBitmap,sizeof(bmp),&bmp);
					RECT rect = {0,0,bmp.bmWidth,bmp.bmHeight};
					POINT point = {(wLimit - bmp.bmWidth) >> 1, (hLimit - bmp.bmHeight) >> 1};
					iTimerDrawEvery = (32*32*32*16) / (iTimerInterval * bmp.bmHeight * bmp.bmWidth + 1);
					SetRect(&rcPic, point.x,point.y, point.x + rect.right - rect.left, point.y + rect.bottom - rect.top);
					pWavepic = new CGDIWavePic(hdc,hBitmap,&rect);
					pWavepic->SetDrawPos(point.x, point.y);
				}
				ReleaseDC(hDlg,hdc);
			}
			bResult = TRUE;
			break;

		case WM_LBUTTONDOWN:
			pWavepic->Drop(LOWORD(lParam),HIWORD(lParam),DropDepth << 3, 2);
			bResult = TRUE;
			break;

		case WM_LBUTTONDBLCLK:
			//switch raining
			bRain ^= ( LOWORD(lParam) >= rcPic.left && LOWORD(lParam) < rcPic.right &&
				HIWORD(lParam) >= rcPic.top && HIWORD(lParam) < rcPic.bottom );
			bResult = TRUE;
			break;

		case WM_RBUTTONDOWN:
			bCloseWindow = true;
			bResult = TRUE;
			break;

		case WM_MOUSEMOVE:
			bCloseWindow = false;
			if (wParam & MK_LBUTTON) {
				pWavepic->Drop(LOWORD(lParam),HIWORD(lParam),DropDepth << 2, 2);
			}
			bResult = TRUE;
			break;

		case WM_RBUTTONUP:
			if(!bCloseWindow) {
				bResult = TRUE;
				break;
			}
			//else go on , no break here
		case WM_COMMAND:
			EndDialog(hDlg, LOWORD(wParam));
			bResult = TRUE;
			break;
		case WM_ERASEBKGND:
			GetClientRect(hDlg,&rectClient);
			int iSmallEdge;
			if((iSmallEdge = rectClient.right) > rectClient.bottom)
				iSmallEdge = rectClient.bottom;
			iSmallEdge -= (iSmallEdge >> 1);
			if (iSmallEdge > 1) {
				const int rsrc(255),gsrc(255),bsrc(255);
				COLORREF clr;
				RECT rc;
				for (int i = 0; i < iSmallEdge; ++i)
				{
					SetRect(&rc,i,i,rectClient.right - i, rectClient.bottom - i);
					clr = RGB(
						(rsrc * (iSmallEdge - 1 - i) + rgbDst[0] * i)/(iSmallEdge - 1),
						(gsrc * (iSmallEdge - 1 - i) + rgbDst[1] * i)/(iSmallEdge - 1),
						(bsrc * (iSmallEdge - 1 - i) + rgbDst[2] * i)/(iSmallEdge - 1)
						);

					FillRect(reinterpret_cast<HDC>(wParam),&rc, gdi_ptr<HBRUSH>(CreateSolidBrush(clr)));
				}
			}
			else {
				FillRect(reinterpret_cast<HDC>(wParam),&rectClient, gdi_ptr<HBRUSH>(CreateSolidBrush(RGB(0xD0,0xE0,0xF0))));
			}

			bResult = TRUE;//已经消除
			break;
		case WM_CTLCOLORSTATIC:

			SetBkMode(reinterpret_cast<HDC>(wParam),TRANSPARENT);
			return reinterpret_cast<BOOL>(GetStockObject(NULL_BRUSH));

		case WM_DESTROY:
			KillTimer(hDlg,uTimerID);

			pWavepic.Reset();

			//std::rotate(rgbDst,rgbDst+2,rgbDst+3);
			bResult = TRUE;
			break;
		default:
			bResult = FALSE;
			break;
	}
	return bResult;
}
