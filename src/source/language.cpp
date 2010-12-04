
#include <auto_pointers.h>
#include "resource.h"
#include "SettingFile.h"
#include "FileStrFnc.h"
#include "language.h"


///////////////////////////////////////////////////////////////////
//////////////
//////////////		Language ��
//////////////
///////////////////////////////////////////////////////////////////
class Language
{
	typedef int IdType;
	typedef std::wstring StringType;
	typedef StringType::value_type CharType;
	typedef std::map<IdType, StringType> IdStrMap;
	typedef std::map<StringType, IdType> StrIdMap;
public:
	Language(void):m_curId(e_idStart) {}
	template <unsigned int N> Language(const CharType * (&szArr)[N]) { LoadArray(szArr); }

	unsigned int Size() const { return m_lng.size(); }
	// SetFilter(const StringType & vStr);

	void Clear() { m_lng.clear(); }

	const IdType GetLngId(const StringType & str, const bool bCreateIfNotFound = false)
	{
		IdType id = IdType();
		if (m_id.find(str) != m_id.end())
		{
			id = m_id[str];
		}
		else if (bCreateIfNotFound)
		{
			id = NewId();
			m_id[str] = id;
		}
		return id;
	}

	bool SetLngFile(const StringType & strFileName, const StringType & strSeparator = _T("==>"), const StringType & strLineComment = _T(";"));
/*
	const StringType &GetStr(const IdType & id) {
		IdStrMap::const_iterator pos = m_lng.find(id);
		if (pos != m_lng.end()) {
			return pos->second; 	// found
		}
		return g_strEmpty;	// not found
	}
//*/
	const CharType * GetCStr(const IdType id) {
		IdStrMap::const_iterator pos = m_lng.find(id);
		if (pos != m_lng.end()) {
			return pos->second.c_str(); 	// found
		}
		return GetEmptyString().c_str();	// not found
	}

	unsigned int LoadArray(const unsigned int N, const CharType **strArray, const bool bAddIdIfNotExist = false);

	template <unsigned int N>
	unsigned int LoadArray(const CharType * (&szArr)[N], const bool bAddIdIfNotExist = false) { return LoadArray(N, szArr, bAddIdIfNotExist); }

private:
	inline static const StringType & GetEmptyString()
	{
		static const StringType str;
		return str;
	}
	IdType NewId() { return ++m_curId; }
	IdStrMap m_lng;
	StrIdMap m_id;
	enum {e_idStart = 10000};
	IdType m_curId;

	// non-copyable
	Language(const Language &);
	Language & operator = (const Language &);
};


unsigned int Language::LoadArray(const unsigned int N, const CharType **strArray, const bool bAddIdIfNotExist) {
	Clear();
	const unsigned int n = N/2;
	for (unsigned int i = 0; i < n; ++i) {
		m_lng[GetLngId(strArray[i*2], bAddIdIfNotExist)] = strArray[i*2+1];
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
	while (ns_file_str_ops::GetLine(file, strLine)) {
		if (strLine.substr(0, strLineComment.length()) == strLineComment) {
			continue;
		}

		// analyze this line
		StringType::size_type pos = strLine.find(strSeparator);
		if (pos != StringType::npos) {
			// found
			StringType strSrc(strLine.substr(0, pos));
			ns_file_str_ops::StripCharsAtEnds(strSrc, strSpaceChars);

			StringType strDst(strLine.substr(pos + strSeparator.length()));
			ns_file_str_ops::StripCharsAtEnds(strDst, strSpaceChars);

			m_lng[GetLngId(strSrc)] = strDst;
		}
	}
	m_lng.erase(0); // delete the invalid one.
	return true;
}

//// end of Language class


// default language : English
const TCHAR * g_strEnglishLngArray[] = {
	//Ӣ�Ľ��� English Interface
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

	// Menu Editor GUI
	_T("STR_DlgTitle"),							_T("TrayLauncher Command Editor"),

	_T("BTN_OK"),								_T("OK"),
	_T("BTN_Cancel"),							_T("Cancel"),
	_T("BTN_Apply"),							_T("Apply"),
	_T("BTN_Close"),							_T("Close"),
	_T("BTN_Save"),								_T("Save"),
	_T("BTN_Reload"),							_T("Reload"),

	_T("BTN_IsMenu"),							_T("Is Submenu"),
	_T("BTN_IsSep"),							_T("As Separator"),
	_T("BTN_IsTitle"),							_T("As Title"),
	_T("BTN_IsWildCard"),						_T("Wildcard mode"),

	_T("STC_Menu"),								_T("Commands:"),
	_T("STC_Target"),							_T("Target Path:"),
	_T("STC_DispName"),							_T("Display Name:"),
	_T("STC_Filter"),							_T("Filter:"),
	_T("STC_IconPath"),							_T("Use a custom icon:"),

	_T("STR_MyComputer"),						_T("*Computer*"),
	_T("STR_DisplayName"),						_T("[Display Name]"),
	_T("STR_PathToTarget"),						_T("[Path To Target]"),
	_T("STR_Choose_Target"),					_T("Choose The Target"),
	_T("STR_Choose_Icon"),						_T("Choose The Icon"),
	_T("STR_Ask_Save_Item_Info"),				_T("Save the change for this command?"),
	_T("STR_Exit_Ask_Save"),					_T("Save the change(s) before exit?"),
	_T("STR_Ask_Delete_Menu"),					_T("Delete the menu and all its sub items?"),

	_T("STR_Invalid_NameOrFilter"),				_T("Invalide input: '<', '>' and '=' are not allowed!"),
	_T("STR_Err_Del_Only_Child"),				_T("Can't delete the only child!"),
	_T("")

};


// globla functions
Language & MainLng()
{
	static Language s_mainlng(g_strEnglishLngArray);
	return s_mainlng;
}


const TCHAR * GetLang(const TCHAR * strSrc)
{
	return MainLng().GetCStr(MainLng().GetLngId(strSrc));
}

bool SetLanguageFile(const TCHAR * szFileName)
{
	// first, reset to default.
	MainLng().LoadArray(g_strEnglishLngArray, true);

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
	while (ns_file_str_ops::GetLine(file, strLine)) {
		// analyze this line
		TSTRING::size_type pos = strLine.find(strSeparator);
		if (pos != TSTRING::npos) {
			// found
			TSTRING strSrc(strLine.substr(0, pos));
			ns_file_str_ops::StripCharsAtEnds(strSrc, strSpaceChars);
			if (strSrc == strKey)
			{
				strLngName = strLine.substr(pos + strSeparator.length());
				ns_file_str_ops::StripCharsAtEnds(strLngName, strSpaceChars);
				break;
			}
		}
	}
	return strLngName;
}
