#ifndef TRAYSTARTLANGUAGEHEADER
#define TRAYSTARTLANGUAGEHEADER

#ifndef UNICODE //目前强制Unicode实现
#define UNICODE
#endif

#include <stdheaders.h>


//全局作用域的函数调用唯一的语言类对象。
const TCHAR * const g_strEmpty = _T("");
const TCHAR * GetLang(const wchar_t * strSrc) ;
bool SetLanguageFile(const TCHAR * );

#undef _LNG
#define _LNG(str) GetLang(L ## #str)

#endif //TRAYSTARTLANGUAGEHEADER
