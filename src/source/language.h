#ifndef TRAYSTARTLANGUAGEHEADER
#define TRAYSTARTLANGUAGEHEADER

#ifndef UNICODE //Ŀǰǿ��Unicodeʵ��
#define UNICODE
#endif

#include <stdheaders.h>


//ȫ��������ĺ�������Ψһ�����������
const TCHAR * const g_strEmpty = _T("");
const TCHAR * GetLang(const wchar_t * strSrc) ;
bool SetLanguageFile(const TCHAR * );

#undef _LNG
#define _LNG(str) GetLang(L ## #str)

#endif //TRAYSTARTLANGUAGEHEADER
