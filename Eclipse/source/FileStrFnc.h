#ifndef LCH_FILE_STR_FNC_H
#define LCH_FILE_STR_FNC_H

#include <stdheaders.h>
// an collection of funtions for file and string/wstring
class CFileStrFnc
{
public:
	static bool GetLine(FILE * file, TSTRING &strLine);
	static inline void WriteStringToFile(const TSTRING & str, FILE * file)
	{
		fwrite(str.c_str(), sizeof(TSTRING::value_type), str.length(), file);
	}
	static const TSTRING StripSpaces(const TSTRING & str);

	//! ×ª»»µ½Ð¡Ð´×ÖÄ¸¡£
	static TSTRING & ToLowerCase(TSTRING &str);
	static TSTRING LowerCase(const TSTRING &str) {
		TSTRING s(str);
		return ToLowerCase(s);
	}

protected:
	CFileStrFnc(void);
	~CFileStrFnc(void);
};

#endif //LCH_FILE_STR_FNC_H
