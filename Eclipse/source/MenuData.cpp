#include "stdafx.h"

#include "MenuData.h"

CItem::CItem( Prm strName, Prm strPath, Prm strEx):m_str(3){
		m_str[0] = strName;
		m_str[1] = strPath;
		m_str[2] = strEx;
}

bool CItem::OutPut(FILE * pFile, TCHAR pad, int nPad) {
	if (!pFile)
		return false;
	if (!(Name() == Empty() && Path() == Empty())) {
		WriteStringToFile(tString(nPad, pad) + Name() + _T(" = ") + Path(), pFile);
		if (Ex().length()) {
			WriteStringToFile(tString(3,'|') + Ex(), pFile);
		}
	}
	return true;
}



CMenuData::CMenuData( Prm strName, Prm strPath, Prm strEx)
:CItem(strName,strPath,strEx)
{}

CMenuData::~CMenuData() { Clear();}

void CMenuData::Clear() {
	for (Ui i = 0; i < m_sub.size(); ++i) {
		delete m_sub[i];
		m_sub[i] = 0;
	}
	m_sub.clear();
}

bool CMenuData::AddItem (Ui pos, Prm strName, Prm strPath, Prm strEx) {
	if ( pos >= 0 && pos <= m_sub.size() ) {
		CItem * p = new CItem(strName, strPath, strEx);
		m_sub.insert(m_sub.begin() + pos, p);
		return true;
	}
	return false;
}

bool CMenuData::AddMenu(Ui pos, Prm strName, Prm strPath, Prm strEx) {
	if ( pos >= 0 && pos <= m_sub.size() ) {
		CMenuData * p = new CMenuData(strName, strPath, strEx);
		m_sub.insert(m_sub.begin() + pos, p);
		return true;
	}
	return false;
}


bool CMenuData::Remove(Ui pos) {
	const bool br (pos > 0 && pos < m_sub.size());
	if (br){
		delete m_sub[pos];
		m_sub.erase(m_sub.begin() + pos);
	}
	return br;
}

bool CMenuData::SaveAs(CRTS strFileName, TCHAR pad, int nPad, int step) {
	wukong::file_ptr outfile(strFileName.c_str(), _T("wb"));
	bool r(false);
	if (outfile.Get()) {
		_fputtc(0xfeff, outfile.Get());
		r = OutPut(outfile, pad, nPad, step);
	}
	return r;
}

int CMenuData::Load(CRTS strFileName) {
	wukong::file_ptr file(strFileName.c_str(), _T("rb"));
	int r(0);
	if (file.Get() && _fgettc(file.Get()) == 0xfeff){
		Clear();
		r = LoadFile(file.Get());
	}
	return r;
}

bool CMenuData::OutPut(FILE * pFile, TCHAR pad, int nPad, int step) {
	if (!pFile)
		return false;
	for (Ui i = 0; i < m_sub.size(); ++i) {
		if (IsMenu(i)) {
			WriteStringToFile(tString(nPad, pad)+_T(">"), pFile);

			Item(i)->CItem::OutPut(pFile, '\t', 1);
			WriteStringToFile(_T("\r\n"), pFile);

			Menu(i)->OutPut(pFile, pad, nPad + step);

			WriteStringToFile(tString(nPad, pad) + _T("<\r\n"), pFile);
		}
		else {
			Item(i)->OutPut(pFile, pad, nPad);
			_fputtc('\r', pFile);_fputtc('\n', pFile);
		}
	}
	return true;
}
int CMenuData::LoadFileAlter(FILE * pFile)
{
	assert(pFile);
	int nItems = 0;
	tString strLine;
	bool bEof = false;
	tString strName,strPath;
	while(!bEof) {
		bEof = !GetLine(pFile, strLine);
		strLine = StripSpaces(strLine);
		if (strLine.empty() && !bEof) {
			AddItem(Count(), Empty(), Empty() ); //  separater
			continue;
		}
		
		strLine = StripSpaces( strLine.substr( 0, strLine.find(';') ) );
		if (strLine.empty()) { // comments 
			continue;
		}

		const tString::size_type pos = strLine.find('=',0);
		strName = StripSpaces( strLine.substr(0,pos) );
		strPath = (pos == tString::npos) ? Empty() : StripSpaces( strLine.substr(pos + 1) );

		switch (strLine[0]) {
			case '>' :
			case '{' :
				strName = StripSpaces(strName.substr(1));
				
				if (AddMenu(Count(), strName, strPath ) ) {
					assert(IsMenu(Count()-1));
					Menu(Count()-1)->LoadFile(pFile);
				}
				break;
			case '<' :
			case '}' :
				return nItems;
				break;
			default:
				nItems += AddItem(Count(), strName, strPath);
				break;
		}
	}

	return nItems;
}
int CMenuData::LoadFile(FILE *pFile) {
	return LoadFileAlter(pFile);
/*
	assert(pFile);

	int nItems = 0;
	const int NBUF = 1024;
	TCHAR buf[NBUF] = {0};
	int i = 0;//偏移，读入的字符个数
	int iStartOfPath = 0; // 菜单目标路径

#ifdef UNICODE
	typedef wint_t CH;
#else
	typedef int CH;
#endif
	CH ch = 0xfeff; // not EOF or WEOF

	while(i < NBUF && ch != _TEOF) {
		ch = _fgettc(pFile);
		// 去掉行首的空白
		if (0 == i)
			while(_istspace(ch) && ch != '\r' && ch != '\n')
				ch = _fgettc(pFile);

		buf[i] = (TCHAR)ch;
		switch (buf[i])
		{
		case '=':
			if (0 == iStartOfPath) {
				// 去掉 命令行开头的空白符号
				while((ch = _fgettc(pFile))!=_TEOF && _istspace(ch) && ch != '\r' && ch != '\n') ;
				_ungettc(ch,pFile);

				buf[i] = '\0';
				// 去掉 名称后面的空白
				while(i > 0 && _istspace(buf[--i])){
					buf[i] = '\0';
				}
				if (i < 0) // 本行格式错误跳过
				{
					while((ch = _fgettc(pFile)) !='\r' && '\n' != ch);// 处理换行符类别
					if ( '\r' == ch && (ch = _fgettc(pFile)) != '\n')
						_ungettc(ch, pFile);
					i = 0;
				}
				else {	i += 2;
					iStartOfPath = i;
				}
			}
			else {
				//从第二个开始的等号 "=" 当成普通字符
				++i; //读入字符到 buf 中
			}
			break;
		case ';':
			// 分号表示 行注释
			while((ch = _fgettc(pFile)) !='\r' && ch != '\n'&& ch != (int)_TEOF) {
				;
			}
			// 处理换行符类别
			if ('\r' == ch && (ch = _fgettc(pFile)) != '\n')
				_ungettc(ch, pFile);
			if (i == 0) {
				// 整行都是注释，跳过。
				iStartOfPath = 0;
				continue;//继续循环读取字符。
			}
			else { ; }
			// 继续向下

		case '\r':
			if ((ch = _fgettc(pFile)) != '\n')
				_ungettc(ch, pFile);// 处理换行符类别
			// go on
		case '\n':
		case _TEOF:
			if (0 == i) {
				if ( _TEOF != buf[i] ) {
					AddItem(Count(), Empty(), Empty() ); // 分割
					iStartOfPath = 0;
				}
				continue;
			}

			// 本字符串结束，
			buf[i] = '\0';
			while(_istspace(buf[--i]) && buf[i] != '\0')//去掉行末尾空白
				buf[i] = '\0';

			switch(*buf)
			{
				const TCHAR * pName;
			case '>':
			case '{':
				// 子菜单
				pName = buf + 1;
				while(*pName && _istspace(*pName)) ++pName;

				if (AddMenu(Count(), pName, iStartOfPath > 0 ? buf + iStartOfPath : Empty() ) ) {
					//int a = Count();
					assert(IsMenu(Count()-1));
					Menu(Count()-1)->LoadFile(pFile);
				}
				break;
			case '<':
			case '}':
				// 子菜单结束
				return nItems;

			default:
				if(0 < iStartOfPath) {
					pName = buf;
					while(*pName && _istspace(*pName)) ++pName;
					nItems += AddItem(Count(), pName, buf + iStartOfPath);
				}
				else {
					nItems += AddItem(Count(), buf, Empty() );
				}
				break;

			} // end if switch(*buf)

			memset(buf, 0, sizeof(buf));
			i = 0;
			iStartOfPath = 0;
			break;
		default:
			//普通字符
			++i; //读入字符到 buf 中
			break;
		} //end of switch(buf[i])
	}//end of while
	return nItems;
//	*/
}

