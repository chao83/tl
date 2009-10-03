
#include <auto_pointers.h>
#include "resource.h"
#include "SettingFile.h"
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
	Reset();
	const TString strSpaceChars(L" \t\r\n");
	TString strLine;
	while (ReadLine(file, strLine)) {
		if (strLine.substr(0, strLineComment.length()) == strLineComment) {
			continue;
		}
		// analyze this line
		TString::size_type pos = strLine.find(strSeparator);
		if (pos != TString::npos) {
			// found
			TString strSrc(strLine.substr(0, pos));
			StripCharsAtEnds(strSrc, strSpaceChars);
			if (!m_bApplyFilter || Find(strSrc)) {
				// valid source string
				TString strDst(strLine.substr(pos + strSeparator.length()));
				StripCharsAtEnds(strDst, strSpaceChars);
				Set(strSrc, strDst);
			}
		}
	}
	return true;
}

bool Language::ReadLine(FILE *file, TString & strLine) {
	strLine.clear();
	wchar_t ch = fgetwc(file);
	while (ch != WEOF) {
		strLine += ch;

		if ('\r' == ch) {
			ch = fgetwc(file);
			if ('\n' == ch) {
				strLine += static_cast<TString::value_type>('\n');
			}
			else {
				ungetwc(ch, file);
			}
			break;
		}
		else if ('\n' == ch) {
			break;
		}
		ch = fgetwc(file);
	}
	return !strLine.empty();
};

bool Language::StripCharsAtEnds(TString & str, const TString & chars)
{
	const TString::size_type begin = str.find_first_not_of(chars);
	if (begin == TString::npos) {
		str.clear();
	}
	else {
		str.erase(0, begin);
		const TString::size_type end = str.find_last_not_of(chars);
		assert (end != TString::npos);
		str.erase(end + 1);
	}
	return true;
}
