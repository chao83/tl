
#include "stdafx.h"
#include <auto_pointers.h>
#include "resource.h"
#include "SettingFile.h"
#include "language.h"

CSettingFile & Settings();

const TCHAR * g_strEnglishLngArray[] = {
	//Ӣ�Ľ��� English Interface
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
	_T("Use Mid Click")

	// , 0  //	��NULL��β��
};

static CLng g_lng(g_strEnglishLngArray, 1000);

void InitLanguage()
{
	Settings().AddSection(sectionGeneral);
	TSTRING strLanguage;
	if (! Settings().Get(sectionGeneral, keyLanguage, strLanguage)) {
		strLanguage = _T("");
		Settings().Set(sectionGeneral, keyLanguage, strLanguage,true);
	}
	SetLanguageFile(strLanguage.c_str());
}

const TCHAR * GetLang(const TCHAR * strSrc)
{
	return g_lng.GetLang(strSrc);
}

const TCHAR * GetLang(const char * strSrc)
{
	const int n = strlen(strSrc)+1;
	wukong::Arr<wchar_t> str(new TCHAR[n]);
	memset(str.Get(), 0, sizeof(TCHAR)*n);
	MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,strSrc,-1,str.Get(), n);
	return g_lng.GetLang(str.Get());
}

bool SetLanguageFile(const TCHAR * szFileName)
{
	TSTRING strFile(szFileName);
	if(!file_ptr(strFile.c_str(), TEXT("rb")) && strFile.find('\\') == strFile.npos) {
		strFile = _T(".\\Lng\\") + strFile;
	}
	return g_lng.SetLngFile(strFile.c_str());
}

///////////////////////////////////////////////////////////////////
//////////////
//////////////        CLng ��ʵ��
//////////////
///////////////////////////////////////////////////////////////////
template <unsigned int size>
CLng::CLng(const TCHAR * (&szDefault)[size], Id startId)
:LNG_BEGIN(startId),
LNG_END(startId + size),
m_default(_default),
m_index(_index)
{
	Init(size, szDefault);
}

//private init function, called by ctr
void CLng::Init(unsigned int size, const TCHAR **szDefault)
{
	for(unsigned int i = 0; i < size; ++i) {
		_default[LNG_BEGIN + i] = szDefault[i];
		assert(_index.find(szDefault[i]) == _index.end());
		_index[szDefault[i]] = LNG_BEGIN + i;
	}
}
//! ��ȡ��ǰ������ָ���� ID ��Ӧ���ַ���
const TCHAR * const CLng::GetLang(const Id langId) const
{
	if(langId < LNG_BEGIN || langId >= LNG_END)
		return g_strEmpty;

	IdLngMap::const_iterator lngIter(m_lng.find(langId));
	if(lngIter != m_lng.end())
		return lngIter->second.c_str();
	else {
		return m_default.find(langId)->second.c_str();
	}
}

//��Ҫ�������GetLang()�滻_T(), ���Ƕ�̬������
const TCHAR * const CLng::GetLang(const TCHAR * const strSrc) const
{
	LngIdMap::const_iterator iter = m_index.find(strSrc);
	if(iter != m_index.end()) {
		return GetLang(iter->second);
	}
	else
		return strSrc;

}


//! ���½�������
bool CLng::SetLngFile(const TCHAR * szFileName, bool bUseDefaultOnFailure)
{
	file_ptr fileLng(szFileName, _T("rb"));
	if(fileLng) {
		if (_fgettc(fileLng) == 0xfeff) {
			m_lng = m_default;
			LanguageFromFile(fileLng);
			return true;
		}
	//#ifdef _DEBUG
	//	else {
	//		ShowError(_T("Not UNICODE FILE"));
	//	}
	//#endif
	}
	if (bUseDefaultOnFailure) {
		m_lng = m_default;
	}
	return false;
}


//!���ļ�����������ԡ�
int CLng::LanguageFromFile(FILE *pFile)
{
	IdLngMap & lngMap = m_lng;
	assert(pFile);
	int nItems = 0;
	const int nBuf = 1024;//����󳤶�
	TCHAR buf[nBuf] = {0};
	int i = 0;//ƫ�ƣ�������ַ�����
	int iStartOfPath = 0; // �˵�Ŀ��·��
#ifdef UNICODE
	wint_t ch;
#else
	int ch;
#endif

	while(i < nBuf && (ch = _fgettc(pFile)) != _TEOF)
	{
		// ȥ�����׵Ŀհ�
		if (0 == i)
			while(_istspace(ch) && ch != '\r' && ch != '\n')
				ch = _fgettc(pFile);

		buf[i] = (TCHAR)ch;
		switch (buf[i])
		{
		case '=':
			if (0 == iStartOfPath) {
				// ȥ�� �����п�ͷ�Ŀհ׷���
				while((ch = _fgettc(pFile))!=_TEOF && _istspace(ch) && ch != '\r' && ch != '\n') ;
				_ungettc(ch,pFile);

				buf[i] = '\0';
				// ȥ�� ���ƺ���Ŀհ�
				while(i > 0 && _istspace(buf[--i])) buf[i] = '\0';
				if (i < 0) {
					// ���и�ʽ��������
					while((ch = _fgettc(pFile)) !='\r' && ch != '\n');// �����з����
					if ((ch = _fgettc(pFile)) != '\n' && '\r' != ch)
						_ungettc(ch, pFile);
					i = 0;
					break;
				}
				else {
					i += 2;
					iStartOfPath = i;
				}
				break;
			}
			// else ���� ��ע�� ��������
		case ';':
			// �ֺű�ʾ ��ע��
			while((ch = _fgettc(pFile)) !='\r' && ch != '\n'&& ch != (int)_TEOF) ;
				// �����з����
			if ((ch = _fgettc(pFile)) != '\n' && '\r' != ch)
				_ungettc(ch, pFile);
			if (i == 0) {
				iStartOfPath = 0;
				continue;
			}
		case '\n':
		case '\r':
			if (('\r' == ch || ch == '\n') && i == 0) {
				// ���� ����
				if ((ch = _fgettc(pFile)) != '\n' && '\r' != ch)
					_ungettc(ch, pFile);
				iStartOfPath = 0;
				continue;
			}

			// �����з����
			if ((ch = _fgettc(pFile)) != '\n' && '\r' != ch)
				_ungettc(ch, pFile);

			// ���ַ���������
			buf[i] = '\0';
			while(_istspace(buf[--i]) && buf[i] != '\0')//ȥ����ĩβ�հ�
				buf[i] = '\0';

			const TCHAR * pName;

			if(0 < iStartOfPath && *(buf + iStartOfPath)) {
				//����
				pName = buf;
				while(*pName && _istspace(*pName)) ++pName;

				if(*pName) {
					Id id = _ttoi(pName);
					if (id >= LNG_BEGIN && id < LNG_END) {
						lngMap[id] = buf + iStartOfPath;
						++nItems;
					}
				}
			}
			memset(buf, 0, sizeof(buf));
			i = 0;
			iStartOfPath = 0;
			break;
		default:
			//��ͨ�ַ�
			++i; //�����ַ��� buf ��
			break;
		}
	}
	return nItems;
}
