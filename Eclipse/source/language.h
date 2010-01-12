#ifndef TRAYSTARTLANGUAGEHEADER
#define TRAYSTARTLANGUAGEHEADER

#ifndef UNICODE //Ŀǰǿ��Unicodeʵ��
#define UNICODE
#endif

#include <stdheaders.h>


//ȫ��������ĺ�������Ψһ�����������
const TCHAR * const g_strEmpty = _T("");
void InitLanguage();
const TCHAR * GetLang(const wchar_t * strSrc) ;
bool SetLanguageFile(const TCHAR * );

// SettingFile���ؼ���
typedef const TCHAR *const CSTR;
CSTR sectionGeneral = _T("general");
CSTR keyConformExit = _T("conformexit");
CSTR keyCommand = _T("command");
CSTR keyRunIcon = _T("runicon");
CSTR keyTrayIcon = _T("trayicon");
CSTR keyLanguage = _T("language");
CSTR keyShowHidden = _T("showhidden");
CSTR keyMClick = _T("midclick");
CSTR keySkin = _T("skin");
CSTR keyShowOpenHere = _T("ShowOpenHere");
CSTR sectionHistory = _T("history");
CSTR sectionColor = _T("color");

CSTR sectionHotkey = _T("hotkey");
CSTR keyHKMenu = _T("ShowMenu");
CSTR keyHKMenuAtMouse = _T("ShowMenuAtMouse");
CSTR keyHKContextMenu = _T("ShowContextMenu");
CSTR keyHKContextMenu_alt = _T("ShowContextMenu_alt");
CSTR keyHKRunDialog = _T("RunDialog");

#undef _LNG
#define _LNG(str) GetLang(L ## #str)

#endif //TRAYSTARTLANGUAGEHEADER
