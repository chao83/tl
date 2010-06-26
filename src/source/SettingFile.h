#ifndef LCH_SETTING_FILE_H
#define LCH_SETTING_FILE_H

#include <stdheaders.h>
#include "FileStrFnc.h"


class CSettingFile
{
	typedef std::map<TSTRING,TSTRING> StrStrMap;	//һ����¼,�ļ��е�һ�С�
	typedef std::pair<TSTRING,StrStrMap> Section;	//һ�������¼���� [ ] ��ʼ��
	typedef std::vector<Section> Setting;
	Setting m_Settings; //���м�¼��
	bool m_bSettingChanged;
	TSTRING m_strFileName;
	//static const bool GetLine(FILE * file, TSTRING &strLine);
	//static void WriteStringToFile(const TSTRING & str, FILE * file);
	//static const TSTRING StripSpaces(const TSTRING & str);
	const bool Add(const TSTRING & strSection, const TSTRING & strIndex, const TSTRING & str);

public:
	const bool Read(); //��ȡ�ļ��� m_Settings��
	CSettingFile(const TSTRING & strFileName, bool bCreateIfNotFound = false);
	CSettingFile();
	~CSettingFile(void);

	int GetSection(const TSTRING & strSection, std::vector<TSTRING> & vStrKey, std::vector<TSTRING> & vStrValue);
	int TryGet(const TSTRING & strIndex, std::vector<TSTRING> & vStr);
	bool Get(const TSTRING & strSection, const TSTRING & strIndex, TSTRING & strDst);
	bool Set(const TSTRING & strSection, const TSTRING & strIndex, const TSTRING & strSrc, bool bCreate = true);
	bool AddSection(const TSTRING & strSection);	//��� Section
	bool ClearSection(const TSTRING & strSection);
	bool Save();
	bool SaveAs(const TSTRING & strFileName);
	bool DisCard();
	void Clear();
};

#endif
