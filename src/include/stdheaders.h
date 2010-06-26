#ifndef LCH_STD_HEADRE_H
#define LCH_STD_HEADRE_H
#include <cstdio>
#include <tchar.h>
#include <vector>
#include <map>
#include <list>
#include <algorithm>
#include <string>

#include <auto_handle.h>
#include <cassert>

template<bool _Unicode> class CharSetType { public : typedef std::wstring tString; };
template <> class CharSetType<false> { public : typedef std::string tString; };
template <> class CharSetType<true> { public : typedef std::wstring tString; };

#ifdef UNICODE
	//wukong::MustBeTrue<sizeof(*_T("")) == sizeof(wchar_t)>::result;
#endif

typedef CharSetType<sizeof(*_T("")) == sizeof(wchar_t)>::tString TSTRING;
typedef TSTRING tString, TString;

#endif // LCH_STD_HEADRE_H

