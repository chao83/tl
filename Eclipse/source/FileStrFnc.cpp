#include "stdafx.h"

#include "FileStrFnc.h"

CFileStrFnc::CFileStrFnc(void)
{
}

CFileStrFnc::~CFileStrFnc(void)
{
}

//!���ļ���ȡһ�С�

//! \param file FILE *�ļ�ָ�룬�����
//! \param strLine TSTRING ���� ��������
//! \return �����ļ�ĩβ����false�� ���򷵻� true ��
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


TSTRING & CFileStrFnc::ToLowerCase(TSTRING &str)
{
	TSTRING::size_type size = str.length();
	for (TSTRING::size_type i = 0; i < size; ++i) {
		str[i] = _totlower(str[i]);
	}
	return str;
}

//! ȥ���ַ��� ��β �Ŀհס�
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

bool CFileStrFnc::GetLastFileTime(const TCHAR * szFN, FILETIME *pSTCreate, FILETIME *pSTAccess,  FILETIME *pSTWrite) {
	bool r = false;
	HANDLE hFile = CreateFile(szFN, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE != hFile) {		
		r = !!GetFileTime(hFile, pSTCreate, pSTAccess, pSTWrite);
		CloseHandle(hFile);
	}
	return r;

}
