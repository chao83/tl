#ifndef TRAYSTARTLANGUAGEHEADER
#define TRAYSTARTLANGUAGEHEADER

#ifndef UNICODE //目前强制Unicode实现
#define UNICODE
#endif

#include <stdheaders.h>

const int LNG__BEGIN = 1000;

//全局作用域的函数调用唯一的语言类对象。

const TCHAR * const g_strEmpty = _T("");
void InitLanguage();
const TCHAR * GetLang(const wchar_t * strSrc) ;
const TCHAR * GetLang(const char * strSrc) ;
bool SetLanguageFile(const TCHAR * );
// SettingFile　关键字
typedef const TCHAR *const CSTR;
CSTR sectionGeneral = _T("general");
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

// 多国语言映射，
class CLng
{
public:
	typedef unsigned int Id;
private:
	typedef std::map<Id, tString> IdLngMap;
	typedef std::map<tString, Id> LngIdMap;
	
	//初始化
	void Init(unsigned int size, const TCHAR ** szDefault);
	//禁用的功能
	CLng(const CLng &);
	CLng & operator = (const CLng &) ;

public:
	//构造函数
	template <unsigned int n>
	CLng(const TCHAR * (&szDefault)[n], Id startId = 10000);
	~CLng(){}
	const TCHAR * const GetLang(const TCHAR * const szSrc) const;
	const TCHAR * const GetLang(const Id id) const;
	bool SetLngFile(const TCHAR * szLngFile, const bool bUseDefaultOnFailure = true);
	const Id Begin() const { return LNG_BEGIN; }
	const Id End() const { return LNG_END; }
private:
	const Id LNG_BEGIN;
	const Id LNG_END;
	IdLngMap _default;		//default language
	LngIdMap _index;		//default index
	//static const IdLngMap MapFromArray(unsigned int size, const TCHAR * szArr[], Id idStart);
	//static const LngIdMap IndexMap(const IdLngMap & idLngMap);
	const IdLngMap & m_default;		//default language
	const LngIdMap & m_index;		//default index
	IdLngMap m_lng;

	int LanguageFromFile(FILE *pFile);
};

#endif //TRAYSTARTLANGUAGEHEADER
