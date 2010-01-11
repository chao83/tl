
#include <auto_pointers.h>
#include "resource.h"
#include "SettingFile.h"
#include "FileStrFnc.h"
#include "language.h"

CSettingFile & Settings();

const TCHAR * g_strEnglishLngArray[] = {
	//英文界面 English Interface
//*
	_T("MENU_Exit"),							_T("E&xit"),
	_T("MENU_Edit_Cmd"),						_T("&Edit Command"),
	_T("MENU_Refresh"),							_T("&Refresh"),
	_T("MENU_Start_With_OS"),					_T("Start With &Windows"),
	_T("MENU_About"),							_T("&About"),
	_T("MENU_Select_Skin"),						_T("Select &Skin"),
	_T("MENU_Internal"),						_T("Internal"),
	_T("MENU_Language"),						_T("&Language"),
	_T("MENU_Option"),							_T("Option"),
	_T("MENU_Use_MClick"),						_T("Use Mid Click"),
	_T("MENU_Run"),								_T("Run"),


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

/*/
	_T("E&xit"),
	_T("&Edit Command"),
	_T("&Refresh"),
	_T("Start With &Windows"),
	_T("&About"),
	_T("Failed:"),
	_T("Run"),
	_T("Exit Tray Launcher ?"),
	_T("Confirm:"),
	_T("Failed To Execute:"),
	_T("Only One Instance Can Run."),
	_T("The language file is not saved in UNICODE(LE)! Please save it in UNICODE."),
	_T("The command file is not saved in UNICODE(LE)! Please save it in UNICODE."),
	_T("Can not open or create the command file!"),
	_T("Empty"),
	_T("Settings"),
	_T("Select &Skin"),
	_T("Internal"),
	_T("&Language"),
	_T("Option"),
	_T("Use Mid Click"),
	_T("Refreshing...")
//*/
};

static Language g_lng(g_strEnglishLngArray);


unsigned int Language::LoadDefault(const wchar_t **strArray, const unsigned int N) {
	Reset();
	const unsigned int n = N/2;
	for (unsigned int i = 0; i < n; ++i) {
		Set(strArray[i*2], strArray[i*2+1]);
	}
	return Size();
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
	return g_lng.GetCStr(strSrc);
}

bool SetLanguageFile(const TCHAR * szFileName)
{
	// 先初始化为默认值
	g_lng.LoadArray(g_strEnglishLngArray);

	if (!szFileName || !*szFileName) {
		return true;
	}
	TSTRING strFile(szFileName);
	if(!file_ptr(strFile.c_str(), TEXT("rb")) && strFile.find('\\') == strFile.npos) {
		strFile = _T(".\\Lng\\") + strFile;
	}
	return g_lng.SetLngFile(strFile.c_str());
}


///////////////////////////////////////////////////////////////////
//////////////
//////////////		CLng 类实现
//////////////
///////////////////////////////////////////////////////////////////
bool Language::SetLngFile(const TString & strFileName, const TString & strSeparator, const TString & strLineComment) {
	if (strSeparator.empty() ||  (!strLineComment.empty() && _istspace(strLineComment[0])) ) {
		return false;
	}

	file_ptr file(_wfopen(strFileName.c_str(), L"rb"));

	// check unicode le file
	if (!file || fgetwc(file) != 0xfeff) {
		return false;
	}

	const TString strSpaceChars(L" \t\r\n");
	TString strLine;
	while (CFileStrFnc::GetLine(file, strLine)) {
		if (strLine.substr(0, strLineComment.length()) == strLineComment) {
			continue;
		}

		// analyze this line
		TString::size_type pos = strLine.find(strSeparator);
		if (pos != TString::npos) {

			// found
			TString strSrc(strLine.substr(0, pos));
			CFileStrFnc::StripCharsAtEnds(strSrc, strSpaceChars);
			if (!m_bApplyFilter || Find(strSrc)) {

				// valid source string
				TString strDst(strLine.substr(pos + strSeparator.length()));
				CFileStrFnc::StripCharsAtEnds(strDst, strSpaceChars);
				Set(strSrc, strDst);
			}
		}
	}
	return true;
}
