#include "StdAfx.h"
#include <assert.h>
#include <auto_pointers.h>
#include "SettingFile.h"


CSettingFile::CSettingFile()
: m_bSettingChanged(false)
{
}


CSettingFile::CSettingFile(const TSTRING & strFileName, bool bCreateIfNotFound)
: m_bSettingChanged(false), m_strFileName(strFileName)
{
	if( !file_ptr(m_strFileName.c_str(), TEXT("rb"))){
		if (bCreateIfNotFound) {
			file_ptr newFile (m_strFileName.c_str(), TEXT("wb"));
			if (newFile) {
				_fputtc(0xfeff, newFile);
			}
		}
	}
	else {
		Read();
	}
}


CSettingFile::~CSettingFile(void)
{
	Save();
	Clear();
}


//! ���������ļ�
bool CSettingFile::Save()
{
	return m_bSettingChanged && SaveAs(m_strFileName);
}


//! ���������ļ�Ϊָ���ļ���
bool CSettingFile::SaveAs(const TSTRING & strFileName)
{
	file_ptr file(strFileName.c_str(), TEXT("wb"));
	if (NULL == file) {
		return false;
	}
	m_strFileName = strFileName;

	_fputtc(0xfeff, file);
	for (Setting::size_type i = 0; i < m_Settings.size(); ++i) {
		if (m_Settings[i].first.length() > 0) {
			WriteStringToFile(_T("[") + m_Settings[i].first + _T("]\r\n"), file);
		}
		for (StrStrMap::const_iterator iter = m_Settings[i].second.begin(); iter != m_Settings[i].second.end(); ++iter) {
			WriteStringToFile(iter->first + _T("=") + iter->second + _T("\r\n"), file);
		}

		WriteStringToFile(_T("\r\n"), file);
	}
	m_bSettingChanged = false;
	return true;
}


//! �����������
void CSettingFile::Clear()
{
	m_Settings.clear();
	m_Settings.push_back(Section());
}

/*
//!���ļ���ȡһ�С�

//! \param file FILE *�ļ�ָ�룬�����
//! \param strLine TSTRING ���� ��������
//! \return �����ļ�ĩβ����false�� ���򷵻� true ��
const bool CSettingFile::GetLine(FILE * file, TSTRING &strLine)
{

	#ifdef UNICODE
		wint_t ch(0);
	#else
		int ch(0);
	#endif
	strLine = _T("");

	ch = _fgettc(file);

	while (ch != _TEOF && ch != '\r' && ch != '\n') {
		strLine += ch;
		ch = _fgettc(file);
	}
	if ('\r' == ch) {
		ch = _fgettc(file);
		if (ch != '\n' && ch != _TEOF)
			_ungettc(ch, file);
	}

	return (ch != _TEOF);
}
//*/

//! ���ļ���������
const bool CSettingFile::Read()
{
	Clear();

	file_ptr file(m_strFileName.c_str(), TEXT("rb"));
	if (NULL == file) {
		return false;
	}
	fseek(file, 0, SEEK_SET);
	int chUnicode = _fgettc(file);
	if (chUnicode != 0xfeff) {
		return false;
	}

	//��ʼ�� m_Settings
	TSTRING strLine;
	TSTRING strItemKey;
	TSTRING strItemValue;
	while ( GetLine(file, strLine) ) {
		strLine = StripSpaces(strLine);
		if (strLine.empty()) {
			continue;
		}

		if ( '[' == strLine[0]) {
			// section
			TSTRING::size_type pos = strLine.find(']');
			if (pos != strLine.npos) {
				m_Settings.push_back(Section());
				m_Settings[m_Settings.size() - 1].first = StripSpaces(strLine.substr(1, pos - 1));
			}
		}
		else {
			// item
			TSTRING::size_type pos = strLine.find( '=' );
			if ( pos != strLine.npos && pos > 0 ) {
				strItemKey = StripSpaces(strLine.substr(0, pos));
				strItemValue = StripSpaces(strLine.substr(pos + 1));
				m_Settings[m_Settings.size() - 1].second[strItemKey] = strItemValue;
			}
		}
	}
	return true;
}


//! ��ȡ ���� �ڵ����е��ֶ�����
int CSettingFile::GetSection(const TSTRING & strSection, std::vector<TSTRING> & vStrKey, std::vector<TSTRING> & vStrValue)
{
	vStrKey.clear();
	vStrValue.clear();
	for (Setting::size_type i = 0; i < m_Settings.size(); ++i) {
		if (m_Settings[i].first == strSection) {
			for (StrStrMap::const_iterator iter = m_Settings[i].second.begin(); iter != m_Settings[i].second.end(); ++iter ) {
				vStrKey.push_back(iter->first);
				vStrValue.push_back(iter->second);
			}
			return static_cast<int>(vStrKey.size());
		}
	}
	return 0;
}


//! ����ƥ��������ֶε�����
int CSettingFile::TryGet(const TSTRING & strIndex, std::vector<TSTRING> & vStr)
{
	vStr.clear();
	StrStrMap::const_iterator iter;
	for (Setting::size_type i = 0; i < m_Settings.size(); ++i) {
		iter = m_Settings[i].second.find(strIndex);
		if (iter != m_Settings[i].second.end()) {
			vStr.push_back(iter->second);
		}
	}
	return static_cast<int>(vStr.size());
}


//! ��ȡָ���ֶε�����
bool CSettingFile::Get(const TSTRING & strSection, const TSTRING & strIndex, TSTRING & str)
{
	str = _T("");
	for (Setting::size_type i = 0; i < m_Settings.size(); ++i) {
		if (m_Settings[i].first == strSection) {
			StrStrMap::const_iterator iter = m_Settings[i].second.find(strIndex);
			if (iter != m_Settings[i].second.end()){
				str = iter->second;
				return true;
			}
			break;
		}
	}
	return false;
}


//! ��Ӷ���
bool CSettingFile::AddSection(const TSTRING & strSection)
{
	for (Setting::size_type i = 0; i < m_Settings.size(); ++i) {
		if (m_Settings[i].first == strSection) {
			return false;
		}
	}
	m_Settings.push_back(Section());//�հ�
	m_Settings[m_Settings.size() - 1].first = strSection;
	m_bSettingChanged = true;
	return true;
}


//! ��ն�������
bool CSettingFile::ClearSection(const TSTRING & strSection)
{
	for (Setting::size_type i = 0; i < m_Settings.size(); ++i) {
		if (m_Settings[i].first == strSection) {
			m_Settings[i].second.clear();
			m_bSettingChanged = true;
			return true;
		}
	}
	return false;
}


//! ����ָ��������ָ���ֶε�ֵ��
bool CSettingFile::Set(const TSTRING & strSection, const TSTRING & strIndex, const TSTRING & str, bool bCreate)
{
	if (bCreate){
		return Add(strSection, strIndex, str);
	}
	else {
		for (Setting::size_type i = 0; i < m_Settings.size(); ++i) {
			if (m_Settings[i].first == strSection) {
				StrStrMap::iterator iter = m_Settings[i].second.find(strIndex);
				if (iter != m_Settings[i].second.end()){
					iter->second = str;
					m_bSettingChanged = true;
					return true;
				}
				break;
			}
		}
		return false;
	}
}


//! ��ָ������������ֶΣ�
const bool CSettingFile::Add(const TSTRING & strSection, const TSTRING & strIndex, const TSTRING & str)
{
	for (Setting::size_type i = 0; i < m_Settings.size(); ++i) {
		if (m_Settings[i].first == strSection) {
			if (m_Settings[i].second[strIndex] != str) {
				m_Settings[i].second[strIndex] = str;
				m_bSettingChanged = true;
			}
			return true;
		}
	}
	return false;
}



//! �����ϴα������������и���
bool CSettingFile::DisCard()
{
	return Read();
}

/*
void CSettingFile::WriteStringToFile(const TSTRING & str, FILE * file)
{
	fwrite(str.c_str(), sizeof(TSTRING::value_type), str.length(), file);
}


//! ȥ���ַ��� ��β �Ŀհס�
const TSTRING CSettingFile::StripSpaces(const TSTRING & inStr)
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
// */

