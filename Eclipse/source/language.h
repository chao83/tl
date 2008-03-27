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

const TCHAR *const sectionGeneral = _T("general");
const TCHAR *const keyCommand = _T("command");
const TCHAR *const keyRunIcon = _T("runicon");
const TCHAR *const keyTrayIcon = _T("trayicon");
const TCHAR *const keyLanguage = _T("language");
const TCHAR *const keyShowHidden = _T("showhidden");
const TCHAR *const keyMClick = _T("midclick");
const TCHAR *const keySkin = _T("skin");
const TCHAR *const sectionHistory = _T("history");
const TCHAR *const sectionColor = _T("color");

// 多国语言映射，
class CLng
{
private:
	typedef unsigned int Id;
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
	const TCHAR *GetLang(const TCHAR * szSrc);
	const TCHAR *GetLang(Id id);
	bool SetLngFile(const TCHAR * szLngFile, bool bUseDefaultOnFailure = true);
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
