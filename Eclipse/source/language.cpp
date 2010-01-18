
#include <auto_pointers.h>
#include "resource.h"
#include "SettingFile.h"
#include "FileStrFnc.h"
#include "language.h"

class Language;

CSettingFile & Settings();



///////////////////////////////////////////////////////////////////
//////////////
//////////////		Language 类
//////////////
///////////////////////////////////////////////////////////////////
class Language
{
	typedef std::wstring StringType;
	typedef StringType::value_type CharType;
	typedef std::map<StringType, StringType> SSMap;
public:
	Language(void) {}
	template <unsigned int N> Language(const CharType * (&szArr)[N]) { LoadArray(szArr); }

	unsigned int Size() const { return m_ssmap.size(); }
	// SetFilter(const StringType & vStr);

	void Clear() { m_ssmap.clear(); }

	bool SetLngFile(const StringType & strFileName, const StringType & strSeparator = _T("==>"), const StringType & strLineComment = _T(";"));

	const StringType &GetStr(const StringType & strIndex) {
		SSMap::const_iterator pos = m_ssmap.find(strIndex);
		if (pos != m_ssmap.end()) {
			return pos->second; 	// found
		}
		return strIndex;	// not found
	}

	const CharType * GetCStr(const CharType * const szIndex) {
		SSMap::const_iterator pos = m_ssmap.find(szIndex);
		if (pos != m_ssmap.end()) {
			return pos->second.c_str(); 	// found
		}
		return szIndex;	// not found
	}

	unsigned int LoadArray(const CharType **strArray, const unsigned int N);

	template <unsigned int N>
	unsigned int LoadArray(const CharType * (&szArr)[N]) { return LoadArray(szArr, N); }

private:

	SSMap m_ssmap;

	// non-copyable
	Language(const Language &);
	Language & operator = (const Language &);
};


unsigned int Language::LoadArray(const CharType **strArray, const unsigned int N) {
	Clear();
	const unsigned int n = N/2;
	for (unsigned int i = 0; i < n; ++i) {
		m_ssmap[strArray[i*2]] = strArray[i*2+1];
	}
	return Size();
}


bool Language::SetLngFile(const StringType & strFileName, const StringType & strSeparator, const StringType & strLineComment) {
	if (strSeparator.empty() ||  (!strLineComment.empty() && _istspace(strLineComment[0])) ) {
		return false;
	}

	file_ptr file(_wfopen(strFileName.c_str(), L"rb"));

	// check unicode le file
	if (!file || fgetwc(file) != 0xfeff) {
		return false;
	}

	const StringType strSpaceChars(L" \t\r\n");
	StringType strLine;
	while (CFileStrFnc::GetLine(file, strLine)) {
		if (strLine.substr(0, strLineComment.length()) == strLineComment) {
			continue;
		}

		// analyze this line
		StringType::size_type pos = strLine.find(strSeparator);
		if (pos != StringType::npos) {
			// found
			StringType strSrc(strLine.substr(0, pos));
			CFileStrFnc::StripCharsAtEnds(strSrc, strSpaceChars);

			StringType strDst(strLine.substr(pos + strSeparator.length()));
			CFileStrFnc::StripCharsAtEnds(strDst, strSpaceChars);

			m_ssmap[strSrc] = strDst;
		}
	}
	return true;
}

//// end of Language class


// default language : English
const TCHAR * g_strEnglishLngArray[] = {
	//英文界面 English Interface
	_T("MENU_Exit"),							_T("E&xit"),
	_T("MENU_Edit_Cmd"),						_T("&Edit Command"),
	_T("MENU_Refresh"),							_T("&Refresh"),
	_T("MENU_Start_With_OS"),					_T("Start With &Windows"),
	_T("MENU_About"),							_T("&About"),
	_T("MENU_Select_Skin"),						_T("Select &Skin"),
	_T("MENU_Internal"),						_T("Internal"),
	_T("MENU_Language"),						_T("&Language"),
	_T("MENU_Option"),							_T("&Option"),
	_T("MENU_Use_MClick"),						_T("Use &Mid Click"),
	_T("MENU_Run"),								_T("&Run"),

	_T("STR_Run"),								_T("Run"),
	_T("STR_Failed"),							_T("Failed:"),
	_T("STR_Exit_Ask"),							_T("Exit Tray Launcher ?"),
	_T("STR_Confirm"),							_T("Confirm:"),
	_T("STR_Failed_To_Exec"),					_T("Failed To Execute:"),
	_T("STR_Single_Instance_Only"),				_T("Only One Instance Can Run."),
	_T("STR_lng_file_not_UNICODE"),				_T("The language file is not saved in UNICODE(LE)! Please save it in UNICODE."),
	_T("STR_cmd_file_not_UNICODE"),				_T("The command file is not saved in UNICODE(LE)! Please save it in UNICODE."),
	_T("STR_Failed_open_create_cmd_file"),		_T("Can not open or create the command file!"),
	_T("STR_Empty"),							_T("Empty"),
	_T("STR_Settings"),							_T("Settings"),
	_T("STR_Refreshing"),						_T("Refreshing..."),
	_T("")

};


static Language g_mainlng_impl(g_strEnglishLngArray);
// globla functions
Language & MainLng()
{
	return g_mainlng_impl;
}


void InitLanguage()
{
	Settings().AddSection(sectionGeneral);
	TSTRING strLanguage;
	if (! Settings().Get(sectionGeneral, keyLanguage, strLanguage)) {
		strLanguage.clear();
		Settings().Set(sectionGeneral, keyLanguage, strLanguage,true);
	}
	SetLanguageFile(strLanguage.c_str());
}

const TCHAR * GetLang(const TCHAR * strSrc)
{
	return MainLng().GetCStr(strSrc);
}

bool SetLanguageFile(const TCHAR * szFileName)
{
	// first, reset to default.
	MainLng().LoadArray(g_strEnglishLngArray);

	if (!szFileName || !*szFileName) {
		return true;
	}
	TSTRING strFile(szFileName);
	if(!file_ptr(strFile.c_str(), _T("rb")) && strFile.find('\\') == strFile.npos) {
		strFile = _T(".\\Lng\\") + strFile;
	}
	return MainLng().SetLngFile(strFile.c_str());
}

const TSTRING GetLngName(const TSTRING & strFileName)
{
	TSTRING strLngName; // return it

	TSTRING strFile(strFileName);
	if(!file_ptr(strFile.c_str(), _T("rb")) && strFile.find('\\') == strFile.npos) {
		strFile = _T(".\\Lng\\") + strFile;
	}

	file_ptr file(_wfopen(strFile.c_str(), L"rb"));

	// check unicode le file
	if (!file || fgetwc(file) != 0xfeff) {
		return strLngName;
	}

	const TSTRING strSpaceChars(L" \t\r\n");
	const TSTRING strKey(L";Language");
	const TSTRING strSeparator(L":");
	TSTRING strLine;
	while (CFileStrFnc::GetLine(file, strLine)) {
		// analyze this line
		TSTRING::size_type pos = strLine.find(strSeparator);
		if (pos != TSTRING::npos) {
			// found
			TSTRING strSrc(strLine.substr(0, pos));
			CFileStrFnc::StripCharsAtEnds(strSrc, strSpaceChars);
			if (strSrc == strKey)
			{
				strLngName = strLine.substr(pos + strSeparator.length());
				CFileStrFnc::StripCharsAtEnds(strLngName, strSpaceChars);
				break;
			}
		}
	}
	return strLngName;
}