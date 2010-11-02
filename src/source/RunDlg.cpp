////////////////////////////////////////////////////////////////////////////////
//////////
//////////						"����" �Ի���ʵ�ֲ���
//////////
////////////////////////////////////////////////////////////////////////////////
//

#include <commctrl.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include "resource.h"
#include "language.h"
#include "MenuWithIcon.h"
#include "SettingFile.h"
#include "RunDlg.h"

extern Ptr<CMenuWithIcon> g_pTray;
const HINSTANCE ThisHinstGet();

const TSTRING::size_type HISTORYSIZE(50);
std::vector<TSTRING> & StrHis()
{
	static std::vector<TSTRING> vStrHis;
	return vStrHis;
};

//! �ж��ַ����Ƿ��� �������ַ�����β��
using ns_file_str_ops::IsStrEndWith;

//! ����ָ�����ַ�(ch)�ָ������ַ���(inStr)�������ָ������(vStr). ���ַ���Ҳ��Ч��
unsigned int GetSeparatedString(const TSTRING & inStr, const TSTRING::value_type ch, std::vector<TSTRING> & vStr);

icon_ptr & GRunIcon()
{
	static icon_ptr s_runIcon;
	return s_runIcon;
}


HWND & GHdlgRun()
{
	static HWND s_hDlgRun = NULL;
	return s_hDlgRun;
}


bool ShellSuccess(const HINSTANCE hInst)
{
	return reinterpret_cast<int>(hInst) > 32;
}

int QuoteString(TSTRING &str, const TSTRING::value_type ch = '\"', bool bProcessEmptyString = false)
{
	if (str.empty() ) {
		if (bProcessEmptyString ) {
			str = _T("\"\"");
		}
	}
	else {
		if (*str.begin() != ch) {
			str.insert(str.begin(), ch);
		}
		if (*str.rbegin() != ch) {
			str.push_back(ch);
		}
	}
	return 1;
}

//! Write history list to setting file
void UpdateHistoryRecordsInFile()
{
	Settings().AddSection(sectionHistory);
	Settings().ClearSection(sectionHistory);
	//�������ü�¼
	for (std::vector<TSTRING>::size_type i = 0; i < StrHis().size(); ++i) {
		TCHAR sz[8] = {'0',0};
		_itot(static_cast<int>(i+1), sz + (i < 9), 10);

		Settings().Set(sectionHistory, sz , StrHis()[i],true);
	}
}


//! ������ʷ��¼
bool AddToHis(std::vector<TSTRING>& vStrHis, const TSTRING & strNew, TSTRING::size_type iHisSize)
{
	const TSTRING::size_type iSize = vStrHis.size();
	//�������У����Ƿ����,����ھ��Ƶ���ʼ
	for (TSTRING::size_type i = 0; i < iSize; ++i) {
		if (vStrHis[i] == strNew) {
			//�ƶ�����ͷ
			std::rotate(vStrHis.begin(), vStrHis.begin() + i, vStrHis.begin() + i + 1);
			return true;
		}
	}

	// ԭ��û�У������µ�
	if (iSize < iHisSize) {
		//������������һ�������
		vStrHis.push_back(strNew);
	}
	else {
		vStrHis.back() = strNew;
	}

	//���һ����ǰ��ǰ������κ���
	std::rotate(vStrHis.begin(), vStrHis.end() - 1, vStrHis.end());

	return true;
}


void InitHistory(const std::vector<TSTRING>& vStrHis)
{
	for (std::vector<TSTRING>::const_reverse_iterator ri = vStrHis.rbegin(); ri != vStrHis.rend(); ++ri) {
		AddToHis(StrHis(), *ri, HISTORYSIZE);
	}
}


//! ����������Ͳ���
void GetCmdAndParam(const TSTRING& const_strCmdParam, TSTRING& strCmd, TSTRING& strParam)
{
	TSTRING strCmdParam(const_strCmdParam);
	strCmd = g_strEmpty;
	strParam = g_strEmpty;

	//ȥ�������հ�
	if (strCmdParam.length() && (_istspace(*strCmdParam.begin()) || _istspace(*strCmdParam.rbegin()) )) {
		strCmdParam = ns_file_str_ops::StripSpaces(strCmdParam);
	}

	if( ! strCmdParam.empty()) {
		TSTRING::size_type pos;
		if(strCmdParam[0] == '\"') {
			if ((pos = strCmdParam.find('\"',1)) != TSTRING::npos) {
				strCmd = strCmdParam.substr(1,pos - 1);
				strParam = strCmdParam.substr(pos);
				strParam = strParam.substr(1);
			}
			else if (strCmdParam.length() > 1) {
				strCmd = strCmdParam.substr(1);
			}
		}
		else {
			pos = strCmdParam.find(' ');
			strCmd = strCmdParam.substr(0,pos);
			if(pos != TSTRING::npos) {
				strParam = strCmdParam.substr(pos);
			}
		}

		if (strCmd.length() && (_istspace(*strCmd.begin()) || _istspace(*strCmd.rbegin()) )) {
			strCmd = ns_file_str_ops::StripSpaces(strCmd);
		}
		if (strParam.length() && (_istspace(*strParam.begin()) || _istspace(*strParam.rbegin()) ) ){
			strParam = ns_file_str_ops::StripSpaces(strParam);
		}
	}
}


//! ������������ʾ��Ϣ
void SetHint(HWND hDlg, ICONTYPE hIcon, const TCHAR *pHint)
{
	if (reinterpret_cast<LRESULT>(hIcon) != SendMessage(GetDlgItem(hDlg, IDC_IMG_ICON),STM_GETICON, reinterpret_cast<WPARAM>(hIcon),0))
		SendMessage(GetDlgItem(hDlg, IDC_IMG_ICON),STM_SETICON, reinterpret_cast<WPARAM>(hIcon),0);
	SetDlgItemText(hDlg, IDC_EDT_PATH, pHint);
}

//! ���ݺ�׺���ж��ļ��Ƿ�Ϊ��ִ���ļ�.
bool IsPathExe(const TSTRING & path) {

	return  IsStrEndWith(path,_T(".exe"),false) ||
			IsStrEndWith(path,_T(".cmd"),false) ||
			IsStrEndWith(path,_T(".bat"),false) ||
			IsStrEndWith(path,_T(".pif"),false) ||
			IsStrEndWith(path,_T(".scr"),false) ||
			IsStrEndWith(path,_T(".com"),false) ||
			IsStrEndWith(path,_T(".scf"),false);
}


//! ��ע���HKLM��ָ�����Ӽ���������ִ���ļ�
bool SearchRegkeyForExe(const TSTRING & strCmdParam,
						const TSTRING & inStrSubKey,
						const TCHAR * pItemName,
						icon_ptr & s_hIcon,
						TSTRING & strHint,
						bool bNameOnly)
{
		TSTRING strCmd,strParam;
		GetCmdAndParam(strCmdParam,strCmd, strParam);
		//����ע���
		bool bKeyFound = false;

		TSTRING strSubKey(inStrSubKey);
		TSTRING strFileName (strCmd);
		if (bNameOnly && strFileName.find('\\') != strFileName.npos)
			strFileName = strFileName.substr(strFileName.find_last_of('\\'));
		if (IsStrEndWith(strFileName, _T(".exe"),false)) {
			strSubKey += strFileName;
		}
		else {
			strSubKey += (strFileName + _T(".exe"));
		}

		HKEY hKeyAppPath;
		if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE,strSubKey.c_str(),0,KEY_READ,&hKeyAppPath)) {


			static const int iCmdSize = MAX_PATH;
			TCHAR szInput[iCmdSize] = {0};
			TCHAR *path = szInput;

			DWORD dwLength (iCmdSize);
			DWORD dwType (REG_NONE);
			if (ERROR_SUCCESS == RegQueryValueEx(hKeyAppPath, pItemName, NULL, &dwType, reinterpret_cast<LPBYTE>(path), &dwLength) &&
				REG_SZ == dwType)
			{
				s_hIcon.Reset();
				if(path)
					s_hIcon = g_pTray->GetBigIcon(path);
				if (!s_hIcon.Get())
					s_hIcon = g_pTray->GetBigIcon(strCmd);

				strHint = path;
				QuoteString(strHint);
				if(strParam.length()) {
					strHint += _T(" ") + strParam;
				}
				bKeyFound = true;
			}

			RegCloseKey(hKeyAppPath);
		}
	return bKeyFound;
}

//! չ�����·��
bool ExpandRelativePaths(tString & src) {

	bool bExpanded = false;
	bool bQuote = false;
	TSTRING strCmd(src),strParam;
	if ( ! src.empty() && src[0] == '\"') {
		GetCmdAndParam(src, strCmd, strParam);
		bQuote = true;
	}

	if ( ( strCmd.length() > 1 && strCmd.substr(0,2) == _T(".\\") ) ||
		( strCmd.length() > 2 && strCmd.substr(0,3) == _T("..\\") ) )
	{
		int nBuf = GetFullPathName(strCmd.c_str(),0,0,0);
		Arr<TCHAR> buf ( new TCHAR[nBuf + 1] );
		if (GetFullPathName(strCmd.c_str(),nBuf, buf.Get(), 0)) {
			src = buf.Get();
		}
		if (bQuote)
			QuoteString(src);
		if ( ! strParam.empty())
			src += _T(" ") + strParam;
		bExpanded = true;
	}
	return bExpanded;
}

//! ������������ʾ��Ϣ��ͼ���·��
void UpdateHint(HWND hDlg, icon_ptr & s_hIcon, ICONTYPE hIconDefault = NULL)
{
	assert(!s_hIcon.Get());

	bool bFound(true);
	const unsigned int MINCMDLEN = 1;
	static const int iCmdSize = MAX_PATH;
	static TCHAR szInput[iCmdSize];
	TSTRING strHint(g_strEmpty);//Ҫ��ʾ��

	if(!SendMessage(GetDlgItem(hDlg, IDC_CBORUN), WM_GETTEXT, iCmdSize, (LPARAM)szInput)) {
		SetHint(hDlg,hIconDefault,g_strEmpty);
		return;
	}
	TSTRING strCmdParam(szInput);

	TSTRING strPath;// cmd + param
	//ֻ���� MINCMDLEN ���ַ����ϵ�
	if (strCmdParam.length() < MINCMDLEN) {
		SetHint(hDlg,hIconDefault,g_strEmpty);
		return;
	}
	else if(unsigned int uID = g_pTray->Find(strCmdParam,strPath)) {
		//�����˵����ƣ�ƥ�������û�����
		// ���Է���������
		TSTRING strCmdWithinPath,strParamWithinPath;
		GetCmdAndParam(strPath, strCmdWithinPath, strParamWithinPath);
		s_hIcon = g_pTray->GetBigIcon(uID);//strCmdWithinPath);
		strHint = strPath;
	}
	else {
		//  . && ..
		ExpandRelativePaths(strCmdParam);
		// ���Է���������
		TSTRING strCmd,strParam;
		GetCmdAndParam(strCmdParam, strCmd, strParam);

		//ֻ���� MINCMDLEN ���ַ����ϵ�
		if (strCmd.length() < MINCMDLEN) {
			SetHint(hDlg,hIconDefault,g_strEmpty);
			return;
		}

		// �㷺ƥ��

		//�����˵����ƣ�ƥ������������������������
		if ( (strCmd != strCmdParam) && 0 != (uID = g_pTray->Find(strCmd,strPath)) ) {
			// ���Է���������
			TSTRING strCmdWithinPath,strParamWithinPath;
			GetCmdAndParam(strPath, strCmdWithinPath, strParamWithinPath);
			s_hIcon = g_pTray->GetBigIcon(uID);//strCmdWithinPath);

			strHint = strCmdWithinPath;
			QuoteString(strHint);
			if ( ! strParamWithinPath.empty())
				strHint += _T(" ") + strParamWithinPath;
			if( ! strParam.empty())
				strHint += _T(" ") + strParam;
		}
		else {
			bool bKeyFound = false;
			//������ִ���ļ�
			// ����·��
			if (strCmd.find('\\') != strCmd.npos) {
				DWORD dwType(0);
				if (GetBinaryType(strCmd.c_str(), &dwType))
				{
					s_hIcon = g_pTray->GetBigIcon(strCmd);
					strHint = strCmd;
					QuoteString(strHint);
					if( ! strParam.empty())
						strHint += _T(" ") + strParam;
					bKeyFound = true;
				}
			}

			//����ע���
			if (!bKeyFound)
				bKeyFound = SearchRegkeyForExe(strCmdParam, _T("Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\"), NULL, s_hIcon, strHint, false);
			if (!bKeyFound && strCmd.find('\\') == strCmd.npos && strCmd.find(' ') == strCmd.npos) {
				// ����PATH��������
				DWORD nSize = GetEnvironmentVariable(_T("PATH"), NULL, 0);
				if (nSize) {
					Arr<TCHAR> pPath (new TCHAR[nSize]);
					if (GetEnvironmentVariable(_T("PATH"), pPath.Get(), nSize) == nSize - 1) {
						const TSTRING strPathEnv(pPath.Get());
						std::vector<TSTRING> vPath;
						GetSeparatedString(strPathEnv, ';', vPath);

						for (std::vector<TSTRING>::size_type i = 0; i < vPath.size(); ++i) {
							if (vPath[i].empty())
								continue;
							TSTRING strMaybe(vPath[i]);
							if (*strMaybe.rbegin() == '\\')
								strMaybe += strCmd;
							else
								strMaybe += (_T("\\") + strCmd);
							if (! IsStrEndWith(strCmd, _T("exe"), false))
								strMaybe += _T(".exe");
							DWORD dwType(0);
							if (GetBinaryType(strMaybe.c_str(), &dwType))
							{
								s_hIcon = g_pTray->GetBigIcon(strMaybe);
								strHint = strMaybe;
								QuoteString(strHint);
								if( ! strParam.empty())
									strHint = strHint + _T(" ") + strParam;
								bKeyFound = true;
								break;
							}
						}
					}
				}
			}
			if (!bKeyFound) {
				TCHAR *path = szInput;
				memset(path,0,sizeof(TCHAR)*iCmdSize);
				DWORD dwSize = iCmdSize;
				HRESULT hres = AssocQueryString(ASSOCF_OPEN_BYEXENAME,
								 ASSOCSTR_EXECUTABLE,
								 strCmd.c_str(),
								 NULL,
								 path,
								 &dwSize);

				if ( (S_OK == hres) || (ShellSuccess(FindExecutable(strCmd.c_str(),NULL,path)) && *path) ) {
					s_hIcon = g_pTray->GetBigIcon(strCmd);
					if (!s_hIcon.Get() && dwSize)
						s_hIcon = g_pTray->GetBigIcon(path);
					if (S_OK == hres)
						strHint = path;
					else
						strHint = strCmd;
					QuoteString(strHint);

					if( ! strParam.empty())
						strHint += _T(" ") + strParam;
				}
				else {
					bool bOkAsFull = false;
					// ƥ��������Ϊ���������������������Ҳ���Ǽ�����ͷ�� ""
					if (strCmdParam.find('\"') == tString::npos) {
						s_hIcon = g_pTray->GetBigIcon(strCmdParam);
						if (s_hIcon.Get()) {
							strHint = strCmdParam;
							QuoteString(strHint);
							bOkAsFull = true;
						}
					}
					if (!bOkAsFull) {
						s_hIcon = g_pTray->GetBigIcon(strCmd);
						strHint = strCmdParam;
						if ( ! strHint.empty() && strHint[0] != '\"')
							QuoteString(strHint);
						bFound = false;
					}
				}
			}
		}
	}

	if (bFound){
		//����ע���
		TSTRING strSubKey(_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\"));
		SearchRegkeyForExe(strCmdParam, strSubKey, _T("Debugger"), s_hIcon, strHint, true);
	}
	//��ʾĿ���ͼ���·����
	SetHint(hDlg,s_hIcon.Get()?s_hIcon.Get():hIconDefault,strHint.c_str());
	return;
}


int MyGetDlgItemText(HWND hDlg,int id, TCHAR *szCommand, int iCmdSize)
{
	memset(szCommand,0,sizeof(TCHAR)*iCmdSize);
	return static_cast<int>(SendMessage(GetDlgItem(hDlg, id), WM_GETTEXT, iCmdSize, (LPARAM)szCommand));
}


int MyGetDlgItemTextBeforeCursor(HWND hDlg,int id,TSTRING& str)
{
	const int iCmdSize(MAX_PATH);
	TCHAR szCommandHere[iCmdSize];
	MyGetDlgItemText(hDlg, id,szCommandHere,iCmdSize);

	DWORD dwSel = static_cast<DWORD>(SendMessage(GetDlgItem(hDlg, id), CB_GETEDITSEL, 0, 0));
	str = szCommandHere;
	str = str.substr(0,LOWORD(dwSel));
	return static_cast<int>(str.length());
}

//! ����������
bool Execute(const TSTRING & strToBeExecuted, const TCHAR * pOpr = NULL)
{
	// ������ ·����������Ŀ¼
	TSTRING strCmd,strParam;
	GetCmdAndParam(strToBeExecuted, strCmd, strParam);
	TSTRING strDir;
	TSTRING::size_type pos = strCmd.find_last_of('\\');
	if (strCmd.npos != pos) {
		strDir = strCmd.substr(0,pos);
	}
	return ShellSuccess(ShellExecute(NULL,pOpr,strCmd.c_str(), strParam.c_str(), strDir.c_str(), SW_SHOW));
}

//! ����������
bool ExecuteEx(const TSTRING & strToBeExecuted, const TCHAR * pOpr = NULL, HWND hwnd = NULL)
{
	TSTRING strCmd,strParam;
	GetCmdAndParam(strToBeExecuted, strCmd, strParam);
	TSTRING strDir;
	const TSTRING::size_type posDirEnd = strCmd.find_last_of('\\');
	if (strCmd.npos != posDirEnd) {
		strDir = strCmd.substr(0, posDirEnd);
	}

	SHELLEXECUTEINFO sei = {0};
	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.hwnd = hwnd;
	sei.fMask = SEE_MASK_INVOKEIDLIST|SEE_MASK_FLAG_NO_UI;
	sei.lpVerb = pOpr;
	sei.lpFile = strCmd.c_str();
	if (IsPathExe(strCmd)) {
		// ��չ \"  - > \"\"\"
		TSTRING::size_type pos = strParam.find('\"');
		while (pos != strParam.npos) {
			strParam.insert(strParam.begin() + pos, '\"');
			strParam.insert(strParam.begin() + pos, '\"');
			pos += 3;
			pos = strParam.find('\"', pos);
		}
		sei.lpParameters = strParam.c_str();
	}
	else
		sei.lpParameters = NULL;
	sei.lpDirectory = strDir.c_str();
	sei.nShow = SW_SHOWNORMAL;
	ShellExecuteEx(&sei);
	return ShellSuccess(sei.hInstApp);
}

//! ����tooltip
HWND WINAPI CreateTT(HWND hwndOwner, HWND hwndTool)
{
	//INITCOMMONCONTROLSEX icex;
	HWND		hwndTT;
	TOOLINFO	ti;
	// Load the ToolTip class from the DLL.
	//icex.dwSize = sizeof(icex);
	//icex.dwICC  = ICC_BAR_CLASSES;

	//if(!InitCommonControlsEx(&icex))
	//   return NULL;

	// Create the ToolTip control.
	hwndTT = CreateWindow(TOOLTIPS_CLASS, TEXT(""),
						  WS_POPUP,
						  CW_USEDEFAULT, CW_USEDEFAULT,
						  CW_USEDEFAULT, CW_USEDEFAULT,
						  NULL, (HMENU)NULL, ThisHinstGet(),
						  NULL);

	RECT rect = {0};
	//GetWindowRect (hwndTool, &rect);
	// Prepare TOOLINFO structure for use as tracking ToolTip.
	ti.cbSize = sizeof(TOOLINFO) - sizeof(void*);
	ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS | TTF_CENTERTIP;// | TTF_TRACK | TTF_ABSOLUTE;
	ti.hwnd   = hwndOwner;
	ti.uId	= (UINT)hwndTool;
	ti.hinst  = NULL;
	ti.lpszText  = LPSTR_TEXTCALLBACK;
	ti.rect.left = rect.left;
	ti.rect.top = rect.top;
	ti.rect.bottom = rect.bottom;
	ti.rect.right = rect.right;

	// Add the tool to the control, displaying an error if needed.
	if(!SendMessage(hwndTT,TTM_ADDTOOL,0,(LPARAM)&ti)){
		return NULL;
	}
	SendMessage(hwndTT,TTM_ACTIVATE, TRUE,0);
	return hwndTT;
}

const unsigned int FINDFILE = 1, FINDDIR = 2, FINDHIDE = 4, FINDALL = FINDFILE | FINDDIR | FINDHIDE;
// mode 0: all, 1: file only, No dir  2: dir only, No file
int FindFiles(const tString & strSearch, std::vector<tString> & vStr, unsigned int mode = FINDFILE | FINDDIR) {
	tString strShowHidden;
	if(Settings().Get(sectionGeneral, keyShowHidden, strShowHidden) && (strShowHidden == _T("true") || strShowHidden == _T("1")))	{
		mode |= FINDHIDE;
	}

	WIN32_FIND_DATA fd = {0};
	const TCHAR *f = fd.cFileName;
	int iFound = 0;
	HANDLE handle = FindFirstFile(strSearch.c_str(),&fd); // ϵͳ�Ỻ������������fullPath���ԸĶ���  Ans��Ӧ���ǵ�. ��Ҫð�գ��½�һ���ɡ�
	if (handle != INVALID_HANDLE_VALUE) {
		std::map<tString, tString> nameName;
		do {
			if( ( f[0] == '.' && f[1] == '\0' )|| ( f[0] == '.' && f[1] == '.' && f[2] == '\0') )
				continue;
			if((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(mode & FINDDIR))
				continue;
			if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(mode & FINDFILE))
				continue;

			if (!(mode & FINDHIDE) && (fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
				continue;

			// ���Դ�Сд, ����
			TSTRING strNameLower(f);
			const TSTRING::size_type len = strNameLower.length();
			for (TSTRING::size_type i = 0; i < len; ++i) {
				strNameLower[i] = _totlower(strNameLower[i]);
			}
			nameName[strNameLower] = f;
			++iFound;
		}while(FindNextFile(handle,&fd));

		FindClose(handle);
		for (std::map<tString, tString>::const_iterator it = nameName.begin(); it != nameName.end(); ++it) {
			vStr.push_back(it->second);
		}

	}
	return iFound;
}

//! �����û��������������ܵ�����Լ�·���µ��ļ�

//! \param vStr : vector<tString>, ���׷�ӵ�����
//! \return : int, �ҵ��ĸ���
int SearchToBuildList(const tString & strSrc, std::vector<tString> & vStr, bool bAllowDup = true)
{
	int iFound = 0;
	//������һ����ĸ�� '\"'
	bool bQuote( !strSrc.empty() && '\"' == strSrc[0]);

	tString::size_type pos = strSrc.find_last_of(_T("\\"));
	// ������� *, ?, ����ʼλ�õ� '\"' , �ͽ�ֹ����
	if (pos != tString::npos && tString::npos == strSrc.find_first_of(_T("*?")) && strSrc.find('\"',1) == tString::npos) {

		tString strDir(strSrc.substr(bQuote, pos+1-bQuote)); // end with '\\'
		std::vector<tString> vFound;

		tString strSearch(strDir);

		ExpandRelativePaths(strSearch);
		if ( 2 < strSearch.length() && strSearch.substr(1,2) == _T(":\\")) {
			strSearch += strSrc.substr(pos + 1) + _T("*");
			iFound = FindFiles(strSearch, vFound);
			const tString strQuote = bQuote?_T("\""):g_strEmpty;
			if (bAllowDup) {
				for (int i = 0; i < iFound; ++i) {
					vStr.push_back(strQuote + strDir + vFound[i]);
				}
			}
			else {
				tString strDest;
				for (int i = 0; i < iFound; ++i) {
					strDest = strQuote + strDir + vFound[i];
					if (std::find(vStr.begin(), vStr.end(), strDest) == vStr.end())
						vStr.push_back(strDest);
				}
			}
		}
	}

	return iFound;
}

//! �����ִ�Сд���Ƚ��ַ������
bool EqualNoCase(const tString & s1, const tString & s2) {
	return (s1.length() == s2.length() && IsStrEndWith(s1,s2, false) );
}

template <class T>
void MyUniqueVector(std::vector<T> & v) {
	bool bChange = true;
	while (bChange) {
		try {
			for (int r = v-1; r >= 0; --r) {
				for (int i = 0; i < r; ++i) {
					if (v[i] == v[r]) {
						bChange = true;
						throw(1);
					}
				}
			}
			bChange = false;
		}
		catch(...){	}
	}
}

//! ִ�жԻ��� ����Ϣ����������
BOOL  CALLBACK RunDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	const int iCmdSize(MAX_PATH);
	static TCHAR szCommand[iCmdSize] = {0};
	static TCHAR szHint[iCmdSize] = {0};
	static AutoHwnd hwndTT;
	//�Ի��������
	static int xPos = 300;
	static int yPos = 400;

	//static ICONTYPE s_hIcon = NULL;
	static icon_ptr s_hIcon;
	static ICONTYPE s_hIconDefault = GRunIcon().Get()? GRunIcon().Get():LoadIcon(ThisHinstGet(), MAKEINTRESOURCE(IDI_UNKNOWN));

	static TSTRING strEditLast(g_strEmpty);//���ڽ�������
	static TSTRING strUserInput(g_strEmpty);//���ڽ�������

	static int iFoundLast = -1;//��һ������������ƥ����������� -1 ��ʾ����������.

	switch (message) {
		case WM_INITDIALOG:
			strUserInput = g_strEmpty;
			SendMessage(GetDlgItem(hDlg, IDC_CBORUN),CB_SETMINVISIBLE,10,0);
			SendMessage(GetDlgItem(hDlg, IDC_CBORUN),CB_LIMITTEXT, 512,0); // �������볤��
			SetWindowText(hDlg, _LNG(STR_Run));
			SetDlgItemText(hDlg, IDC_CBORUN, szCommand);
			SetDlgItemText(hDlg, IDC_BTNRUN, _LNG(STR_Run));
			UpdateHint(hDlg,s_hIcon,s_hIconDefault);
			SetWindowPos(hDlg,HWND_TOPMOST,xPos,yPos,0,0,SWP_NOSIZE);

			for (unsigned int i = 0; i < StrHis().size(); ++i)
				SendMessage(GetDlgItem(hDlg, IDC_CBORUN),CB_ADDSTRING ,0,(LPARAM)StrHis()[i].c_str());
			DragAcceptFiles( hDlg, TRUE);

			if (hwndTT = CreateTT(hDlg, GetDlgItem(hDlg, IDC_EDT_PATH)) ) {
				SendMessage(hwndTT,
					WM_SETFONT,
					SendMessage(GetDlgItem(hDlg, IDC_EDT_PATH), WM_GETFONT, 0,0),
					FALSE);
			}
			return TRUE;
	//		break;
		case WM_SHOWWINDOW:
			if (wParam) {
				// Show
				SendMessage(GetDlgItem(hDlg, IDC_CBORUN),CB_SETEDITSEL,0,MAKELONG(0,-1));
				AllowSetForegroundWindow(GetCurrentProcessId());
				SetForegroundWindow(hDlg);
				SetFocus(GetDlgItem(hDlg, IDC_CBORUN));
			}
			return TRUE;
	//		break;
		case WM_MOVING:
			xPos = reinterpret_cast<RECT *>(lParam)->left;		// horizontal position
			yPos = reinterpret_cast<RECT *>(lParam)->top;		// vertical position
			return TRUE;
		case UM_UPDATEHINT:
			s_hIcon.Reset();
			UpdateHint(hDlg,s_hIcon,s_hIconDefault);
			return TRUE;
		case WM_DESTROY:
			DragAcceptFiles( hDlg, FALSE);
			//ɨβ���洢��������
			iFoundLast = -1;
			MyGetDlgItemText(hDlg, IDC_CBORUN,szCommand,iCmdSize);
			//����ͼ��
			s_hIcon.Reset();
			return TRUE;
		case WM_DROPFILES:
			if (HDROP hDrop = reinterpret_cast<HDROP>(wParam)) {
				const unsigned int iStrSize = MAX_PATH;
				TCHAR szFile[iStrSize];
				if (DragQueryFile(hDrop, 0, szFile, iStrSize)) {
					TSTRING str (szFile);
					QuoteString(str);
					SendMessage(GetDlgItem(hDlg, IDC_CBORUN), WM_SETTEXT,  0, (LPARAM)str.c_str());
				}
				DragFinish(hDrop);
				SendMessage(hDlg, UM_UPDATEHINT, 0,0);
			}
			return TRUE;
		case WM_NOTIFY:
			if (LPNMHDR pnmh = (LPNMHDR) lParam) {
				switch (pnmh->code)
				{
				case TTN_GETDISPINFO:
					{
						LPNMTTDISPINFO pInfo = (LPNMTTDISPINFO) lParam;
						if ((pInfo->uFlags & TTF_IDISHWND) && GetDlgItem(hDlg, IDC_EDT_PATH) ==  (HWND)(pnmh->idFrom)) {
							memset (szHint, 0, sizeof(szHint));
							if (MyGetDlgItemText(hDlg, IDC_EDT_PATH, szHint, iCmdSize) ){
								pInfo->lpszText = szHint;
								pInfo->hinst = NULL;
							}
						}
					}
					break;
				case TTN_SHOW:
					if(hwndTT){
						SetWindowPos(hwndTT, HWND_TOPMOST,100, 0, 0, 0, SWP_NOSIZE|SWP_NOACTIVATE);
					}
					break;
				default:
					break;
				}
			}
			return TRUE;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_BTNRUN:
					if (MyGetDlgItemText(hDlg, IDC_CBORUN,szCommand,iCmdSize)) {
						ShowWindow(hDlg, SW_HIDE); // ������
						TSTRING strEdit(szCommand);
						if (strEdit == _T("-clear") || strEdit == _T(":clear")) {
							StrHis().clear();
							UpdateHistoryRecordsInFile();
							//����б�
							int iNum = static_cast<int>(SendMessage(GetDlgItem(hDlg, IDC_CBORUN),CB_GETCOUNT ,0,0));
							for (int i = 0; i < iNum; ++i)
								SendMessage(GetDlgItem(hDlg, IDC_CBORUN),CB_DELETESTRING ,(WPARAM)0,0);
							DestroyWindow(hDlg);
							memset(szCommand,0,sizeof(szCommand));
							GHdlgRun() = NULL;
							SetProcessWorkingSetSize(GetCurrentProcess(),static_cast<DWORD>(-1), static_cast<DWORD>(-1));
							return TRUE;
						}

						//ִ��������
						bool bSuccessShell = false;
						TCHAR pathFound[MAX_PATH] = {0};
						if(GetDlgItemText(hDlg,IDC_EDT_PATH,pathFound,MAX_PATH)>0 && *pathFound) {
							bSuccessShell = Execute(pathFound);
						}
						bSuccessShell = bSuccessShell || Execute(szCommand);
						if (!bSuccessShell) {
							//ִ������ʧ��
							MessageBox(hDlg,szCommand, _LNG(Failed To Execute:),MB_ICONERROR);
							ShowWindow(hDlg, SW_SHOW); //����ʧ�ܣ� ������ʾ����
							SendMessage(GetDlgItem(hDlg, IDC_CBORUN),CB_SETEDITSEL,0,MAKELONG(0,-1));
							SetFocus(GetDlgItem(hDlg, IDC_CBORUN));
							return TRUE;
						}
						else {
							//ִ�гɹ���������ʷ�б�
							if (AddToHis(StrHis(), szCommand, HISTORYSIZE))
								UpdateHistoryRecordsInFile();
							if (GetKeyState(VK_SHIFT)&0x8000)
								return TRUE;
						}
					}
					//��������ִ�У����ٴ���
					//break;
				case IDCANCEL:
					//���ٴ���
					DestroyWindow(hDlg);
					GHdlgRun() = NULL;
					SetProcessWorkingSetSize(GetCurrentProcess(),static_cast<DWORD>(-1), static_cast<DWORD>(-1));
					return TRUE;

				case IDC_CBORUN:
					//�༭���֪ͨ��Ϣ��
					switch (HIWORD(wParam))	{
						case CBN_EDITUPDATE:	//�༭������ݸ�����,���� //? �ƺ�ֻ���û�������
							MyGetDlgItemText(hDlg, IDC_CBORUN,szCommand,iCmdSize);
							strUserInput = szCommand;
							if ( ! strUserInput.empty()) {
								const TSTRING & strEdit(strUserInput);//�û�����ı༭��ă��ݡ�
								int iEditSize = static_cast<int>(strEdit.size());

								//�û���ĩβ�����ַ�
								DWORD dwSel = static_cast<DWORD>(SendMessage(GetDlgItem(hDlg, IDC_CBORUN), CB_GETEDITSEL, 0, 0));
								const bool bAppendChar ( iEditSize > static_cast<int>(strEditLast.size())
									&& EqualNoCase(strEdit.substr(0,strEditLast.size()), strEditLast)
									&& LOWORD(dwSel) == HIWORD(dwSel)
									&& LOWORD(dwSel) == iEditSize
									);
								if (!bAppendChar){
									//����������
									if ( SendMessage(GetDlgItem(hDlg, IDC_CBORUN),CB_GETDROPPEDSTATE, 0,0)) {
										LRESULT dwSelState = SendMessage(GetDlgItem(hDlg, IDC_CBORUN),CB_GETEDITSEL,0,0);
										SendMessage(GetDlgItem(hDlg, IDC_CBORUN),CB_SHOWDROPDOWN ,false,0);
										SetDlgItemText(hDlg, IDC_CBORUN, szCommand);
										SendMessage(GetDlgItem(hDlg, IDC_CBORUN),CB_SETEDITSEL,0,dwSelState);
									}
									iFoundLast = -1;
								}
								else if (iFoundLast != 0 || strEdit.length() <= 3 || *strEdit.rbegin() == '\\') //ֻ����ĩβ������
								{
									//�û��������ַ�
									//������ʷ��¼���Ͳ˵����ơ�
									int iFound = 0; //ͳ��ƥ�����,���Ǽ򵥵�ֻ��һ��

									std::vector<TSTRING> vStrNameFound;
									if (iFoundLast > 0 && *strEdit.rbegin() != '\\') {
										//ֱ�ӹ��˵�ǰ�б�
										int count = static_cast<int>(SendMessage(GetDlgItem(hDlg, IDC_CBORUN),CB_GETCOUNT,0,0));
										tString strList;
										for (int i = 0; i < count; ++i) {
											if(SendMessage(GetDlgItem(hDlg, IDC_CBORUN), static_cast<unsigned int>(CB_GETLBTEXT), i, (LPARAM) szCommand)) {
												strList = szCommand;
												if (strList.length() >= strEdit.length() && EqualNoCase(strEdit, strList.substr(0, strEdit.length()))) {
													vStrNameFound.push_back(strList);
													++iFound;
												}
											}
										}

									}
									else {
										// ��������
										iFound += SearchToBuildList(strEdit, vStrNameFound);

										for (unsigned int i = 0; i < StrHis().size(); ++i) {
											if (EqualNoCase(StrHis()[i].substr(0,iEditSize),strEdit) &&
												std::find(vStrNameFound.begin(),vStrNameFound.end(),StrHis()[i]) == vStrNameFound.end()	)
											{
												vStrNameFound.push_back(StrHis()[i]);
												++iFound;
											}
										}

										iFound += g_pTray->FindAllBeginWith(strEdit,vStrNameFound);
									}

									// disable redraw
									SendMessage(GetDlgItem(hDlg, IDC_CBORUN), WM_SETREDRAW, FALSE, 0);

									//����б��ؼ���Ҳ��ѱ༭�����
									SendMessage(GetDlgItem(hDlg, IDC_CBORUN),CB_RESETCONTENT ,0,0);

									//�ؽ��б��ؼ�,

									std::vector<TSTRING>::size_type iNum = vStrNameFound.size();
									for (unsigned int i = 0; i < iNum; ++i) {
										if (CB_ERR == SendMessage(GetDlgItem(hDlg, IDC_CBORUN),CB_FINDSTRINGEXACT ,static_cast<WPARAM>(-1),(LPARAM)vStrNameFound[i].c_str()))
											SendMessage(GetDlgItem(hDlg, IDC_CBORUN),CB_ADDSTRING ,0,(LPARAM)vStrNameFound[i].c_str());
									}

									iNum = StrHis().size();
									for (unsigned int i = 0; i < iNum; ++i) {
										if (CB_ERR == SendMessage(GetDlgItem(hDlg, IDC_CBORUN),CB_FINDSTRINGEXACT ,static_cast<WPARAM>(-1),(LPARAM)StrHis()[i].c_str()))
											SendMessage(GetDlgItem(hDlg, IDC_CBORUN),CB_ADDSTRING ,0,(LPARAM)StrHis()[i].c_str());
									}

									//�ҵ���������ʱ����ʾ������
									SendMessage(GetDlgItem(hDlg, IDC_CBORUN),CB_SHOWDROPDOWN ,(iFound > 1)?1:0, 0);
									// enable redraw
									SendMessage(GetDlgItem(hDlg, IDC_CBORUN), WM_SETREDRAW, TRUE, 0);

									if (iFound) {
										SendMessage(GetDlgItem(hDlg, IDC_CBORUN),CB_SETCURSEL ,0,0);
										//�����û����루��Ҫ�Ǵ�Сд��
										//TSTRING strEditNew(strEdit + vStrNameFound[0].substr(strEdit.size()));
										//SetDlgItemText(hDlg, IDC_CBORUN, strEditNew.c_str());
									} else {
										SetDlgItemText(hDlg, IDC_CBORUN, strEdit.c_str());
									}
									SendMessage(GetDlgItem(hDlg, IDC_CBORUN),CB_SETEDITSEL,0,MAKELONG(iEditSize,-1));
									iFoundLast = iFound;
								}
							}
							strEditLast = strUserInput;//szCommand;

							return TRUE;//break;

						case CBN_SELCHANGE:

							int index;  index = static_cast<int>(SendMessage(GetDlgItem(hDlg, IDC_CBORUN),CB_GETCURSEL,0,0));
							int count;  count = static_cast<int>(SendMessage(GetDlgItem(hDlg, IDC_CBORUN),CB_GETCOUNT,0,0));
							if (count && (index < 0 || index >= count))
								SendMessage(GetDlgItem(hDlg, IDC_CBORUN),CB_SETCURSEL,0,0);
							if (SendMessage(GetDlgItem(hDlg, IDC_CBORUN), static_cast<unsigned int>(CB_GETLBTEXT), index, (LPARAM) szCommand)) { //MyGetDlgItemText(hDlg, IDC_CBORUN,szCommand,iCmdSize)) {
								tString str(szCommand);
								if (IsStrEndWith(str.substr(0, strUserInput.length()), strUserInput, false)){
									PostMessage(GetDlgItem(hDlg, IDC_CBORUN),CB_SETEDITSEL,0,MAKELONG(strUserInput.length(),-1));
								}
							}

							//break;
						case CBN_EDITCHANGE:
							PostMessage(hDlg,UM_UPDATEHINT,0,0);
							return TRUE;//break;
						default:
							break;
					}
					break;
				case IDC_IMG_ICON:
					switch (HIWORD(wParam))
					{
					case STN_DBLCLK: //˫��
						//�������� -> ���а�
						if (OpenClipboard(hDlg)) {

							EmptyClipboard();

							TCHAR pathFound[MAX_PATH] = {0};
							const int cch = GetDlgItemText(hDlg,IDC_EDT_PATH,pathFound,MAX_PATH);
							if (cch) {
								HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (cch + 1) * sizeof(TCHAR));
								if(hglbCopy) {
										// Lock the handle and copy the text to the buffer.
										void * lptstrCopy = GlobalLock(hglbCopy);
										memcpy(lptstrCopy, pathFound,
											(cch+1) * sizeof(TCHAR));
										GlobalUnlock(hglbCopy);

									#ifdef UNICODE
										SetClipboardData(CF_UNICODETEXT, hglbCopy);
									#else
										SetClipboardData(CF_TEXT, hglbCopy);
									#endif
								}
							}
							CloseClipboard();
						}
						break;
					default:
						break;
					}
					break;
				default:
					break;
			}
			break;
		case WM_SYSCOMMAND:
			if (SC_KEYMENU == wParam && 0x0d == lParam) {
				if (MyGetDlgItemText(hDlg, IDC_CBORUN,szCommand,iCmdSize)) {
					TSTRING strEdit(szCommand);
					if (strEdit == _T("-clear") || strEdit == _T(":clear")) {
						return TRUE;
					}

					//ִ��������,�����Լ�
					bool bSuccessShell = false;
					TCHAR pathFound[MAX_PATH] = {0};
					if(GetDlgItemText(hDlg,IDC_EDT_PATH,pathFound,MAX_PATH)>0 && *pathFound) {
						bSuccessShell = ExecuteEx(pathFound, _T("properties"), GHdlgRun());
					}
					if (!bSuccessShell) {
						bSuccessShell = ExecuteEx(szCommand,_T("properties"), GHdlgRun());
					}
					if (!bSuccessShell) {
						//ִ������ʧ��
						MessageBox(hDlg,szCommand, _LNG(Failed To Execute:), MB_ICONERROR);
						SendMessage(GetDlgItem(hDlg, IDC_CBORUN), CB_SETEDITSEL, 0, MAKELONG(0,-1));
						SetFocus(GetDlgItem(hDlg, IDC_CBORUN));
						return TRUE;
					}
				}
			}
			break;
		default:
			break;
	}

	MyGetDlgItemTextBeforeCursor(hDlg, IDC_CBORUN, strEditLast);
	if (iFoundLast != -1 && strEditLast.empty())
		iFoundLast = -1;

//	static tString s_strEditLast;
//	if (s_strEditLast != strEditLast) {
//		s_strEditLast = strEditLast;
//	}
//	static int lastMsg, lastW, lastL;
//	lastMsg = message;
//	lastW = wParam;
//	lastL = lParam;

	return FALSE;
}
