
#include <auto_pointers.h>
#include "resource.h"
#include "SettingFile.h"
#include "FileStrFnc.h"
#include "language.h"

CSettingFile & Settings();

const TCHAR * g_strEnglishLngArray[] = {
	//英文界面 English Interface
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

};

static Language g_lng(g_strEnglishLngArray);


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
	if (!szFileName || !*szFileName) {
		g_lng.Reset();
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
	
	// reset to default to clear the previous language.
	Reset();

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
