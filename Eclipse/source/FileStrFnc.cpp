
#include "FileStrFnc.h"

CFileStrFnc::CFileStrFnc(void)
{
}

CFileStrFnc::~CFileStrFnc(void)
{
}

//!从文件读取一行。

//! \param file FILE *文件指针，输出。
//! \param strLine TSTRING 类型 保存结果。
//! \return 读到文件末尾返回false； 否则返回 true 。
bool CFileStrFnc::GetLine(FILE * file, TSTRING &strLine) 
{

#ifdef UNICODE
	wint_t ch(0);
#else
	int ch(0);
#endif
	strLine.clear();

	ch = _fgettc(file);

	while (ch != _TEOF && ch != '\r' && ch != '\n') {
		strLine += ch;
		ch = _fgettc(file);
	}
	if ('\r' == ch && (	ch = _fgettc(file) ) != '\n' && ch != _TEOF ) {
			_ungettc(ch, file);
	}
	return (ch != _TEOF);
}


bool CFileStrFnc::ReadLine(FILE *file, TSTRING & strLine) {
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


TSTRING & CFileStrFnc::ToLowerCase(TSTRING &str)
{
	TSTRING::size_type size = str.length();
	for (TSTRING::size_type i = 0; i < size; ++i) {
		str[i] = _totlower(str[i]);
	}
	return str;
}

//! 去掉字符串 首尾 的空白。
const TSTRING CFileStrFnc::StripSpaces(const TSTRING & inStr)
{
	TSTRING::size_type iStart(0);
	TSTRING::size_type iEnd(inStr.size());
	while (iStart < iEnd && _istspace(inStr[iStart])) {
		++iStart;
	}
	while (iStart < iEnd && _istspace(inStr[iEnd - 1])){
		--iEnd;
	}
	return inStr.substr(iStart, iEnd - iStart);
}


bool CFileStrFnc::StripCharsAtEnds(TSTRING & str, const TSTRING & chars)
{
	const TSTRING::size_type begin = str.find_first_not_of(chars);
	if (begin == TString::npos) {
		str.clear();
	}
	else {
		str.erase(0, begin);
		const TSTRING::size_type end = str.find_last_not_of(chars);
		assert (end != TString::npos);
		str.erase(end + 1);
	}
	return true;
}


bool CFileStrFnc::GetLastFileTime(const TCHAR * szFN, FILETIME *pSTCreate, FILETIME *pSTAccess,  FILETIME *pSTWrite) {
	bool r = false;
	HANDLE hFile = CreateFile(szFN, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE != hFile) {		
		r = !!GetFileTime(hFile, pSTCreate, pSTAccess, pSTWrite);
		CloseHandle(hFile);
	}
	return r;

}
