#ifndef TRAYSTARTLANGUAGEHEADER
#define TRAYSTARTLANGUAGEHEADER

#ifndef UNICODE //目前强制Unicode实现
#define UNICODE
#endif

#include <stdheaders.h>


//全局作用域的函数调用唯一的语言类对象。
const TCHAR * const g_strEmpty = _T("");
void InitLanguage();
const TCHAR * GetLang(const wchar_t * strSrc) ;
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



//! String to String map, used for multilingual programs
template <class TString>
class StringMap
{
	typedef TString string_type;
	typedef typename TString::value_type char_type;
	typedef std::map<TString, TString> SSMap; // storage type
public:
	StringMap(void) {}
	~StringMap(void) {}

	unsigned int Size() const { return m_strMap.size(); }

	void Clear() { m_strMap.clear(); }

	void ClearValues() {
		for (typename SSMap::iterator it = m_strMap.begin(); it != m_strMap.end(); ++it) {
			it->second.clear();
		}
	}

	bool Find(const TString & strSrc) const {
		return m_strMap.find(strSrc) != m_strMap.end();
	}

	const TString & Get(const TString & strSrc, const bool bIgnoreEmptyResult = true) const {
		typename SSMap::const_iterator it(m_strMap.find(strSrc));
		return (it == m_strMap.end() || (bIgnoreEmptyResult && it->second.empty())) ? strSrc : it->second;
	}

	const char_type * Get(const char_type * const strSrc, const bool bIgnoreEmptyResult = true) const {
		typename SSMap::const_iterator it(m_strMap.find(strSrc));
		return (it == m_strMap.end() || (bIgnoreEmptyResult && it->second.empty())) ? strSrc : it->second.c_str();
	}

	void Set(const TString &strSrc, const TString &strDst) {
		m_strMap[strSrc] = strDst;
	}
private:
	SSMap m_strMap; //!< store string-string map
};

// 多国语言映射，

class Language : private StringMap<std::wstring>
{
	typedef std::wstring TString;
public:
	Language(void):m_bApplyFilter(false) {}
	template <unsigned int N>
	Language(const wchar_t * (&szArr)[N]):m_bApplyFilter(true)	{ LoadDefault(szArr, N); }

	void Reset() {
		if (m_bApplyFilter){
			ClearValues();
		} else {
			Clear();
		}
	}
	bool SetLngFile(const TString & strFileName, const TString & strSeparator = _T("==>"), const TString & strLineComment = _T(";"));
	const TString &GetStr(const TString & strIndex) {
		return Get(strIndex);
	}
	const wchar_t * GetCStr(const wchar_t * const szIndex) {
		return Get(szIndex);
	}

private:
	unsigned int LoadDefault(const wchar_t **strArray, const unsigned int N) {
		Reset();
		for (unsigned int i = 0; i < N; ++i) {
			Set(strArray[i], strArray[i]);
		}
		return Size();
	}
	const bool m_bApplyFilter;

	// non-copyable
	Language(const Language &);
	Language & operator = (const Language &);

};

#undef _LNG
#define _LNG(str) GetLang(L ## str)

#endif //TRAYSTARTLANGUAGEHEADER
