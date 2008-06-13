
#include "stdafx.h"
#include <stdexcept>
#include <shellapi.h>
#include <commctrl.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <algorithm>
#include <direct.h>
#include "resource.h"
#include "language.h"
#include "FileStrFnc.h"

#include "MenuWithIcon.h"
/*

//! ����ָ�����ַ�(ch)�ָ������ַ���(inStr)�������ָ������(vStr). ���ַ���Ҳ��Ч��
//! �쳣�汾
unsigned int GetSeparatedString(const TSTRING & inStr, const TSTRING::value_type ch, std::vector<TSTRING> & vStr)
{
	//�����������ȡ�
	vStr.resize((inStr.length() >> 2) + 2);
	std::vector<TSTRING>::size_type n(0);
	TSTRING::size_type iStartPos = 0, iEndPos = 0;
	do {
		iEndPos = inStr.find(ch, iStartPos);
		try{
			vStr.at(n) = inStr.substr(iStartPos, iEndPos - iStartPos);
		}
		catch (std::out_of_range) {
			// vector is too small, enlarge it and store the new value.
			vStr.resize( vStr.size() + vStr.size()/2);
			assert(vStr.size() > n);
			vStr[n] = inStr.substr(iStartPos, iEndPos - iStartPos);
		}
		++n;
		iStartPos = iEndPos + 1;
	} while (iEndPos != TSTRING::npos);

	vStr.resize(n);
	return n;
}
/*/
//! ����ָ�����ַ�(ch)�ָ������ַ���(inStr)�������ָ������(vStr). ���ַ���Ҳ��Ч��

unsigned int GetSeparatedString(const TSTRING & inStr, const TSTRING::value_type ch, std::vector<TSTRING> & vStr)
{
	vStr.clear();
	TSTRING::size_type iStartPos = 0;
	TSTRING::size_type iEndPos = inStr.find(ch, iStartPos);
	while (iEndPos != TSTRING::npos) {
		vStr.push_back(inStr.substr(iStartPos, iEndPos - iStartPos));
		iStartPos = iEndPos + 1;
		iEndPos = inStr.find(ch, iStartPos);
	}
	vStr.push_back(inStr.substr(iStartPos));
	return vStr.size();
}
// */

//�ж� null ���롣
unsigned int GetSeparatedString(const TSTRING::value_type * inStr, const TSTRING::value_type ch, std::vector<TSTRING> & vStr)
{
	if (!inStr) {
		vStr.clear();
		vStr.push_back(_T(""));
		return 1;
	}
	return GetSeparatedString(TSTRING(inStr), ch, vStr);

}

//! ���캯�� CMenuWithIcon
CMenuWithIcon::CMenuWithIcon(ICONTYPE hOpen,ICONTYPE hClose,ICONTYPE hUnknownFile,const TCHAR *szEmpty)
:COwnerDrawMenu(),
m_hIconOpen(hOpen),m_hIconClose(hClose),m_hIconUnknowFile(hUnknownFile),
m_startID(0),m_ID(0),m_strEmpty (_T("Empty")),m_dynamicStartID(0),
m_bShowHidden(false),m_bFilterEmptySubMenus(true),m_menuData ( new CMenuData(_T("root")) )
{
	if (szEmpty && *szEmpty)
		m_strEmpty = szEmpty;
}


CMenuWithIcon::~CMenuWithIcon(void)
{
	Destroy();
}


//! ��ʾ
int CMenuWithIcon::Display(int x, int y, WINDOWTYPE hWnd, UINT uFlag)
{
	int id(Super::Display(x,y, hWnd, uFlag));
	if (!IsWindow(hWnd)) {
		TryProcessCommand(id);
		DestroyDynamic();
	}
	return id;
}



LRESULT CMenuWithIcon::MenuSelect(MENUTYPE hMenu,UINT uItem,UINT uFlags)
{
	if((uFlags & MF_GRAYED) || (uFlags & MF_DISABLED))
		return 0;
	if(uFlags & MF_POPUP) {
		SelID() = (UINT_PTR)GetSubMenu(hMenu,uItem);
	}
	else	//�˵���
		SelID() = uItem;
	BuildDynamic(reinterpret_cast<MENUTYPE>(SelID()));
	return 0;
}


//! ���Ʋ˵���
bool CMenuWithIcon::DrawItem(DRAWITEMSTRUCT * pDI)
{
	//�������ܵ����ͣ��˵�����
	IDTYPE iMaybeID = pDI->itemID;
	MENUTYPE hMaybeMenu = MatchRect(pDI);
	assert(!hMaybeMenu || hMaybeMenu == (HMENU)iMaybeID);
	if (!hMaybeMenu && (iMaybeID < m_startID || iMaybeID >= m_ID) ) {
		hMaybeMenu = (HMENU)iMaybeID;
	}
	AccordingToState(pDI);

	// ���˵� �� �Ӳ˵�
	const TCHAR * pPath = NULL;
	if ( ! IsMenu(hMaybeMenu))
		pPath = Cmd(iMaybeID);

	// �˵�ͼ��
	ICONTYPE hIcon = NULL;
	if (hMaybeMenu && IsMenu(hMaybeMenu))
		hIcon = MenuIcon(hMaybeMenu);
	else
		hIcon = ItemIcon(iMaybeID);

	if (hIcon)
		DrawIconEx(pDI->hDC,pDI->rcItem.left + MENUBLANK,pDI->rcItem.top + MENUBLANK/2 ,hIcon,MENUICON,MENUICON,0,NULL,DI_NORMAL|DI_COMPAT);
	else if (hMaybeMenu && IsMenu(hMaybeMenu)) {
		// �Ӳ˵�ͼ��
		ICONTYPE hIconSub = NULL;
		bool bDraw = false;//�ɹ����ƶ�̬�Ӳ˵�
		if (IsDynamicMenu(hMaybeMenu)) {
			TSTRING str(m_DynamicPath[hMaybeMenu]);
			str.resize(str.size()-2);
			SHFILEINFO sfi = {0};
			HIMAGELIST hImgList = (HIMAGELIST)SHGetFileInfo(str.c_str(),
					FILE_ATTRIBUTE_NORMAL,
					&sfi,
					sizeof(SHFILEINFO),
					SHGFI_SYSICONINDEX | SHGFI_SMALLICON
					);
			bDraw = (0 != (sfi).iIcon);
			if (bDraw)
				ImageList_DrawEx(hImgList,sfi.iIcon,pDI->hDC,pDI->rcItem.left + MENUBLANK,pDI->rcItem.top + MENUBLANK/2,MENUICON,MENUICON,CLR_NONE,CLR_NONE,ILD_NORMAL);

		}
		if (!bDraw) {
			if (pDI->itemState & ODS_GRAYED || pDI->itemState & ODS_DISABLED)
				hIconSub = NULL;
			else if (!(pDI->itemState & ODS_SELECTED)) // û�д�
				hIconSub = m_hIconClose;
			else
				hIconSub = m_hIconOpen;
			if(hIconSub)
				DrawIconEx(pDI->hDC,pDI->rcItem.left + MENUBLANK,pDI->rcItem.top + MENUBLANK/2 ,hIconSub,MENUICON,MENUICON, 0,NULL,DI_NORMAL);
		}
	}
	else if(!pPath) {
		//����
		DrawText(pDI->hDC,Name(iMaybeID),-1,&(pDI->rcItem),DT_CENTER | DT_SINGLELINE |DT_VCENTER);
		return true;
	}
	else if ( IsStaticMenu(MENUTYPE(pDI->hwndItem)) || IsDynamicMenu(MENUTYPE(pDI->hwndItem)) || IsExpanedMenu(MENUTYPE(pDI->hwndItem))) {
		//���Զ�̬�˵���ļ���
		SHFILEINFO sfi = {0};
		//memset(m_psfi.Get(),0,sizeof(SHFILEINFO));
		HIMAGELIST hImgList = (HIMAGELIST)SHGetFileInfo(Cmd(iMaybeID),
				FILE_ATTRIBUTE_NORMAL,
				&sfi,
				sizeof(SHFILEINFO),
				SHGFI_SYSICONINDEX | SHGFI_SMALLICON
				//���ļ���Ŀǰ֪�� .ico ͼ���ļ���ͼ����Ҫ���ļ���ȡ����������һЩ�ļ�	��ʧ��
				// | SHGFI_USEFILEATTRIBUTES //�����ļ���ȡͼ�꣬����  Ŀǰ����   ����ʧ��
				);
		if (!sfi.iIcon) {
			hImgList = (HIMAGELIST)SHGetFileInfo(Cmd(iMaybeID),
					FILE_ATTRIBUTE_NORMAL,
					&sfi,
					sizeof(SHFILEINFO),
					SHGFI_SYSICONINDEX | SHGFI_SMALLICON
					| SHGFI_USEFILEATTRIBUTES //�����ļ���ȡͼ�꣬����  Ŀǰ����   ����ʧ��
					);

		}
		assert(sfi.iIcon);
		if (sfi.iIcon) {
			ImageList_DrawEx(hImgList,sfi.iIcon,pDI->hDC,pDI->rcItem.left + MENUBLANK,pDI->rcItem.top + MENUBLANK/2,MENUICON,MENUICON,CLR_NONE,CLR_NONE,ILD_NORMAL);
			//DrawIconEx(pDI->hDC,pDI->rcItem.left + MENUBLANK,pDI->rcItem.top + MENUBLANK/2 ,sfi.hIcon,MENUICON,MENUICON,0,NULL,DI_NORMAL|DI_COMPAT);
		}
	}
	else
		//��ʾĬ��δ֪�ļ�ͼ��
		DrawIconEx(pDI->hDC,pDI->rcItem.left + MENUBLANK,pDI->rcItem.top + MENUBLANK/2 ,m_hIconUnknowFile,MENUICON,MENUICON,0,NULL,DI_NORMAL|DI_COMPAT);

	pDI->rcItem.left += MENUHEIGHT + (MENUBLANK << 1);

	const TCHAR *str = ( hMaybeMenu && IsMenu(hMaybeMenu) ) ? MenuName(hMaybeMenu) : ItemName(iMaybeID);
	if(str && *str) {
		DrawText(pDI->hDC, str,-1,&(pDI->rcItem),DT_LEFT | DT_SINGLELINE |DT_VCENTER);
	}
	return true;
}


//! �趨�˵���С
int CMenuWithIcon::MeasureItem(MEASUREITEMSTRUCT *pMI)
{
	COwnerDrawMenu::MeasureItem(pMI);
	if (pMI->itemID)
		pMI->itemWidth += MENUBLANK * 3;
	return 0;
}


//! ��ȡ��ݷ�ʽ��.lnk�ļ���ͼ��
ICONTYPE CMenuWithIcon::GetShortCutIcon(LPCTSTR lpszLinkFile, bool bIcon32)
{
	if (!m_comInited && !m_comInited.Init()) {
		return 0;
	}
	ICONTYPE hReturn = NULL;//����ֵ

	IShellLink * psl;
	void *ppsl = &psl;

	// Get a pointer to the IShellLink interface.
	HRESULT hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
							IID_IShellLink, static_cast<LPVOID*>(ppsl));
	if (SUCCEEDED(hres)) {
		IPersistFile* ppf;
		void *pppf = &ppf;

		// Get a pointer to the IPersistFile interface.
		hres = psl->QueryInterface(IID_IPersistFile, static_cast<LPVOID*>(pppf));

		if (SUCCEEDED(hres)) {
		#ifndef UNICODE
			wchar_t szLinkFile[MAXPATH] = {0};
			MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,lpszLinkFile,-1,szLinkFile,MAX_PATH);
			lpszLinkFile = &szLinkFile;
		#endif
			TCHAR szTgtPath[MAX_PATH];
			TCHAR szIconPath[MAX_PATH];
			int Iindex = 0;
			if (SUCCEEDED(ppf->Load(lpszLinkFile, STGM_READ)) &&
				SUCCEEDED(psl->Resolve(NULL,SLR_NO_UI | SLR_NOSEARCH | SLR_NOTRACK)) &&
				SUCCEEDED(psl->GetPath(szTgtPath, MAX_PATH, NULL, SLGP_SHORTPATH)) &&
				SUCCEEDED(psl->GetIconLocation(szIconPath, MAX_PATH, &Iindex)) )
			{
				const TCHAR * UsedPath = *szIconPath?szIconPath:szTgtPath;
				hReturn = GetIcon(UsedPath, FILEFOLDERICON, Iindex, bIcon32);
			}
			// Release the pointer to the IPersistFile interface.
			ppf->Release();
		}
		// Release the pointer to the IShellLink interface.
		psl->Release();
	}
	return hReturn;
}

//  */


//! ����Ӳ˵���strPath ���ڻ�ȡͼ��
bool CMenuWithIcon::AddSubMenu(MENUTYPE hMenu,MENUTYPE hSubMenu,const tString & strName, const tString & strPath, EICONGETTYPE needIcon)
{
	assert(strName.length());
	AddToMap(MenuNameMap(),hSubMenu, strName);
	InsertMenu(hMenu,(UINT)-1,MF_BYPOSITION | MF_OWNERDRAW | MF_POPUP,(UINT_PTR)hSubMenu,NULL);//MenuName(hSubMenu));//

	if (needIcon) {
		if ( ! IsDynamicMenu(hSubMenu) ) {
			//��̬Ŀ¼���ļ��е�ǿ��Ŀ¼���������ͼ��
			m_SubMenuIcons.Add(hSubMenu,GetIcon(strPath,needIcon));
		}
		else {
			SHFILEINFO sfi = {0};

			SHGetFileInfo(strPath.c_str(),
					FILE_ATTRIBUTE_NORMAL,
					&sfi,
					sizeof(SHFILEINFO),
					SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
			if (!sfi.iIcon) {
				//��ϵͳ���Ҳ����������ҵļ�¼��
				m_SubMenuIcons.Add(hSubMenu, GetIcon(strPath,needIcon));
			}
		}
	}
	return true;
}


//! ���һ���˵���,����ȥ�� ǰ���հ�
int CMenuWithIcon::AddMenuItem(MENUTYPE hMenu, const tString & strName, const tString & inStrPath, EICONGETTYPE needIcon, const tString & strIcon)
{
	// ID����ʼλ�� CMDS;
	Arr<TCHAR> strPath;
	int a = 0;
	a = sizeof(strPath);
	if(int inLen = inStrPath.length()) {
		strPath= new TCHAR [inLen + 1];
		memcpy(strPath.Get(),inStrPath.c_str(), sizeof(TCHAR)*(inLen + 1));
	}
	assert(strName.length());

	// ��һ�˵���
	AddToMap(ItemNameMap(),m_ID, strName);
	InsertMenu(hMenu,(UINT)-1,MF_BYPOSITION | MF_OWNERDRAW,m_ID,NULL);//ItemName(m_ID));//

	if(!strPath.Get() || !strPath[0]) {
		// �������
		EnableMenuItem(hMenu,m_ID,MF_BYCOMMAND | MF_DISABLED);
	}
	else {
		// �ж��Ƿ��������в�����
		// �ļ�·��������
		TCHAR *strCmd = strPath.Get();
		if (*strCmd != '\"')
			AddToMap(m_ItemCmd, m_ID, strPath.Get());
		else {
			while(*(++strCmd) && *strCmd != '\"') ; //����˫����

			if(!*strCmd)
				AddToMap(m_ItemCmd, m_ID, strPath.Get());
			else {
				*strCmd = '\0';
				AddToMap(m_ItemCmd, m_ID, strPath.Get() + 1);
				while(*++strCmd && _istspace(*strCmd)) ;
				if(*strCmd)
					AddToMap(m_ItemParam, m_ID, strCmd);
			}
		}

		if (needIcon != NOICON) {
			// �Ƕ�̬�˵��Ҫͼ��
			const TCHAR *szIconPath = strIcon.empty()?Cmd(m_ID):strIcon.c_str();
			
			AddToMap(m_ItemIconPath, m_ID, strIcon);
			if ( ! IsStaticMenu(hMenu) && ! IsDynamicMenu(hMenu)) {
				m_MenuItemIcons.Add(m_ID,GetIcon(szIconPath, needIcon));
			}
			else  {
				//��̬������ϵͳ�б��еĲŴ洢
				SHFILEINFO sfi = {0};
				SHGetFileInfo(szIconPath,
						FILE_ATTRIBUTE_NORMAL,
						&sfi,
						sizeof(SHFILEINFO),
						SHGFI_SYSICONINDEX | SHGFI_SMALLICON
						| (FILEICON == needIcon ? SHGFI_USEFILEATTRIBUTES : 0)
						);
				if (!sfi.iIcon) {
					m_MenuItemIcons.Add(m_ID,GetIcon(szIconPath,needIcon));
				}
			}
		}
	}

	++m_ID;
	return 1;
}


//! ���ͨ�����ʾ�Ĳ˵���
int CMenuWithIcon::MultiAddMenuItem(MENUTYPE hMenu, const tString & inStrPath,const tString & strName)
{
	//assert (strName.length());
	if(inStrPath.empty())
		return 0;
	TSTRING strPath(inStrPath);
	if(strPath.find('/') != TSTRING::npos) {
		assert (strPath == inStrPath);
		std::replace(strPath.begin(), strPath.end(), '/','\\');
	}
	const tString::size_type len = strPath.length();

	// ĩβ��ʽ����\*������\*.*������\**��,������ʽ������
	// ��ģʽ\*+* :һ����ֱ��չ�������ǲ�����ͼ��
//* 
	assert (*strPath.rbegin() == '*');
	EBUILDMODE mode = EFOLDER;
	if(IsStrEndWith(strPath, _T("\\**"), false)){
		strPath = strPath.substr(0, len - 1);
		mode = EEXPAND;
	}
	else if (IsStrEndWith(strPath, _T("\\*+*"), false)){
		mode = EEXPANDNOW;
		strPath = strPath.substr(0, len - 2);
	}
	else if(IsStrEndWith(strPath, _T("\\*.*"), false)){
		mode = EFILE;
		strPath = strPath.substr(0, len - 2);
	}

	if (EFOLDER == mode && strPath[len-2] != '\\') {
		return 0;
	}
	while (strPath.length() && _istspace(strPath[0]))
		strPath = strPath.substr(1);

	return MultiModeBuildMenu(hMenu,strPath,strName,mode);

/*/
	assert (p[len - 1] =='*');

	EBUILDMODE mode = EFOLDER;
	if(IsStrEndWith(p, _T("\\**"), false)){
		p[_tcslen(p) - 1] = '\0';
		mode = EEXPAND;
	}
	else if (IsStrEndWith(p, _T("\\*+*"), false)){
		mode = EEXPANDNOW;
		p[_tcslen(p) - 2] = '\0';
	}
	else if(IsStrEndWith(p, _T("\\*.*"), false)){
		mode = EFILE;
		p[_tcslen(p) - 2] = '\0';
	}

	if (EFOLDER == mode && p[len-2] != '\\') {
		return 0;
	}

	while(_istspace(*p)) ++p; //ȥ���հ�

	return MultiModeBuildMenu(hMenu,p,strName,mode);
// */

}


//! public ��̬ʵʱ����ָ���Ӳ˵�������,
int CMenuWithIcon::BuildDynamic(MENUTYPE hSubMenu)
{
	int result(0);
	MENUTYPE hSel = (MENUTYPE)SelID();
	if(!IsMenu(hSel) || hSubMenu != hSel) {
		result = 0;
	}
	else if (m_StaticPath.find(hSel) != m_StaticPath.end()) {
		result = DynamicBuild(hSel);
	}
	else if (m_DynamicPath.find(hSel) != m_DynamicPath.end()) {
		result = DynamicBuild(hSel);
	}
	return result;
}


//! private ��̬ʵʱ����ָ���Ӳ˵�������,

//! \note ֻ�������Ӳ˵��Ͳ˵���Ӳ˵���չ����
//! ��Ҫչ���Ӳ˵����Ը��Ӳ˵����ñ�������
int CMenuWithIcon::DynamicBuild(MENUTYPE hMenu)
{
	// ��Ҫ��¼ÿ����̬���Ӳ˵��Ͳ˵��//����̬ ���١�

	//������Ч�˵����Ѿ�������ϵĲ˵���
	if (!IsMenu(hMenu) || GetMenuItemCount(hMenu) > 0)
		return 0;

	const TCHAR *strPath = NULL;
	if (IsStaticMenu(hMenu))
		strPath = GetStr(m_StaticPath,hMenu);
	else {
		assert (IsDynamicMenu(hMenu)) ;
		strPath = GetStr(m_DynamicPath,hMenu);
	}

	if(!strPath || !*strPath)
		return 0;

	const TCHAR *strName = NULL;
	if (IsStaticMenu(hMenu))
		strName = GetWildcard(m_StaticMenuWildcard[hMenu]);
	else {
		assert  (IsDynamicMenu(hMenu)) ;
		strName = GetWildcard(m_DynamicMenuWildcard[hMenu]);
	}

	while (*strPath && _istspace(*strPath)) ++strPath;//ȥ���հ�
	assert (strPath[_tcslen(strPath) - 1] =='*');

	return MultiModeBuildMenu(hMenu,strPath,(strName && *strName)?strName:_T("*"),EDYNAMIC);
}


//! ����˵����������Ӧ��������
bool CMenuWithIcon::TryProcessCommand(unsigned int nSysID)
{
	if (nSysID < m_startID || nSysID >= m_ID)
		return false;
	bool result (true);

	const TCHAR * pCmd = Cmd(nSysID);
	if (pCmd && *pCmd) {
		Arr<TCHAR> pEnough;
		TCHAR path[MAX_PATH] ={0};
		TSTRING strWorkPath;

		// ��ȡĿ¼��·��
		GetDirectory(nSysID,strWorkPath);
		if ('.' == *pCmd) {	//���·��
			TCHAR * FinalName;
			int length = GetFullPathName(pCmd,MAX_PATH,path,&FinalName);
			if (length > MAX_PATH) {
				pEnough = new TCHAR[length];
				if (GetFullPathName(pCmd,MAX_PATH,path,&FinalName) > 0)
					pCmd = pEnough.Get();
			}
			else if (length > 0) {
				pCmd = path;
			}
			if (pCmd && *pCmd && '.' != *pCmd) {
				strWorkPath = pCmd;
				strWorkPath = strWorkPath.substr(0,FinalName - pCmd);
			}
		}

		if( ForceCast<int, HINSTANCE>(ShellExecute(NULL, NULL, pCmd, Param(nSysID), strWorkPath.c_str(), SW_SHOW)) <= SHELL_MAX_ERROR_VALUE) {
			//ִ������ʧ��
			EnableMenuItem(Menu(),nSysID,MF_BYCOMMAND | MF_GRAYED);
			MessageBox(NULL, Cmd(nSysID), GetLang(_T("Failed:")), MB_ICONERROR);
			result = false;
		}
	}
	return result;
}

const TSTRING CMenuWithIcon::GetCurrentCommandLine(unsigned int nSysID)
{
	if (nSysID < m_startID || nSysID >= m_ID)
		return false;

	const TCHAR * pCmd = Cmd(nSysID);
	if (pCmd && *pCmd) {
		Arr<TCHAR> pEnough;
		TCHAR path[MAX_PATH] ={0};
		TSTRING strWorkPath;

		// ��ȡĿ¼��·��
		GetDirectory(nSysID,strWorkPath);
		if ('.' == *pCmd) {	//���·��
			TCHAR * FinalName;
			int length = GetFullPathName(pCmd,MAX_PATH,path,&FinalName);
			if (length > MAX_PATH) {
				pEnough = new TCHAR[length];
				if (GetFullPathName(pCmd,MAX_PATH,path,&FinalName) > 0)
					pCmd = pEnough.Get();
			}
			else if (length > 0) {
				pCmd = path;
			}
			if (pCmd && *pCmd && '.' != *pCmd) {
				strWorkPath = pCmd;
				strWorkPath = strWorkPath.substr(0,FinalName - pCmd);
			}
		}
	}
	return TSTRING(pCmd);
}


//! �Ӵ򿪵��ļ�����˵���
int CMenuWithIcon::LoadMenuFromFile(const tString & strFileName, UINT uStartID)
{
	Reset();
	if (!m_menuData.Get())
		return 0;
	m_menuData->Load(strFileName);

	m_startID = uStartID;
	m_ID = m_startID;
	const int nStaticMenus = BuildMenuFromMenuData(m_menuData.Get(), Menu());
	
	m_dynamicStartID = m_ID;

	//���� <����,ID> ӳ��
	TSTRING strName;
	TSTRING::size_type size = 0;
	for (IDTYPE i = m_startID; i < m_dynamicStartID; ++i) {
		strName = ItemName(i);
		ToLowerCase(strName);

		//�Ƴ�ÿ�γ��ֵĵ�һ�� &
		size = strName.length();
		TSTRING::size_type move = 0;
		for (TSTRING::size_type j = 0; j < size; ++j) {
			if(strName[j] == '&') {
				++move;
				++j;
			}
			if(move)
				strName[j-move] = strName[j];
		}
		//�ض�;
		strName.resize(strName.size() - move);

		m_NameIdMap[strName] = i;
	}

	return nStaticMenus;
}

int CMenuWithIcon::BuildMenuFromMenuData(CMenuData * pMenu, MENUTYPE hMenu)
{
	int nItems(0);
	for (unsigned int index = 0; index < pMenu->Count(); ++index) {
		if (pMenu->IsMenu(index)) {
			MENUTYPE hSubMenu = CreateMenu();
			if (!hSubMenu) 
				continue;			
			nItems += BuildMenuFromMenuData(pMenu->Menu(index), hSubMenu);
			
			if ( GetMenuItemCount(hSubMenu) <= 0 && m_bFilterEmptySubMenus ) {
				DestroyMenu(hSubMenu);
			}
			else {
				AddSubMenu(hMenu,hSubMenu,pMenu->Item(index)->Name().c_str(), pMenu->Item(index)->Path());
				if (GetMenuItemCount(hSubMenu) <= 0) {
					assert (GetSubMenu(hMenu,GetMenuItemCount(hMenu)-1) == hSubMenu);
					EnableMenuItem(hMenu,GetMenuItemCount(hMenu)-1,MF_BYPOSITION | MF_GRAYED);
					//for(int i = 0; i < GetMenuItemCount(hMenu); ++i)
					//	if(GetSubMenu(hMenu,i) == hSubMenu) {
					//		EnableMenuItem(hMenu,i,MF_BYPOSITION | MF_GRAYED);
					//		break;
					//	}
				}
			}
		}
		else if ( pMenu->Item(index)->Path().length() > 1) {
			// ����ģʽ: \\** , ��ʾ�ҵĵ���
			if (pMenu->Item(index)->Path()[0] == '\\' && pMenu->Item(index)->Path() != _T("\\\\**"))
				continue;// filter begin with '\\' but not "\\**"
		
			// ����˵���
			const tString & strPath = pMenu->Item(index)->Path();
			const tString strSep(_T("|||"));
			
			if (!strPath.empty() && 
				*(strPath.rbegin())=='*') {// ƥ��ͨ���
				nItems += MultiAddMenuItem(hMenu,strPath,pMenu->Item(index)->Name());
			}
			else {
				tString::size_type sepPos = strPath.find(strSep) ;
				tString strIcon = tString::npos == sepPos? _T("") : CFileStrFnc::StripSpaces ( strPath.substr(sepPos + strSep.length()) );
				if (strIcon.length()) {
					if ('\"' == strIcon[0]) {
						tString::size_type pos = strIcon.find('\"', 1);
						strIcon = strIcon.substr(1, pos == tString::npos ? pos : pos - 1);

					}
				}
				nItems += AddMenuItem( hMenu,
					pMenu->Item(index)->Name().empty() ? _T("< ??? >") : pMenu->Item(index)->Name() , 
					sepPos == tString::npos ? strPath : CFileStrFnc::StripSpaces( strPath.substr(0,sepPos ) ), 
					FILEFOLDERICON,
					strIcon
					);//ͳ�Ʋ˵�������			
			}
		}
		else if ( pMenu->Item(index)->Name().length() ) {
			//���ɱ���
			nItems += AddMenuItem(hMenu,pMenu->Item(index)->Name(),_T(""));
		}
		else {
			// empty, saperator
			InsertMenu(hMenu,(UINT)-1,MF_BYPOSITION | MF_OWNERDRAW | MF_SEPARATOR,0,0);
		}

	}
	return nItems;
}


//! ��ȡ�ļ�Ŀ¼����ض������ַ���
bool CMenuWithIcon::GetDirectory(const IDTYPE nID, TSTRING & strWorkPath)
{
	strWorkPath = Cmd(nID);
	TSTRING::size_type pos = strWorkPath.find_last_of('\\');
	bool r = (strWorkPath.npos != pos);
	if (r) {
		strWorkPath.resize(pos);
	}
	return r;
}


//! ���ٶ�̬Ԫ��
void CMenuWithIcon::DestroyDynamic()
{
	if (m_ID == m_dynamicStartID)
		return;

	// 1 ���� ��̬�˵���
	for (IDTYPE i = m_dynamicStartID; i < m_ID; ++i) {
		//����ͼ�꣬�������
		m_ItemCmd.erase(i);
		ItemNameMap().erase(i);
	}

	// 2 �����Ӳ˵���¼
	for (MenuStrIter it = m_DynamicPath.begin(); it != m_DynamicPath.end(); ++it) {
		// ����ͼ��
		m_SubMenuIcons.Remove(it->first);
		// �������Ƽ�¼
		MenuNameMap().erase(it->first);
	}
	// ���ٶ�̬�˵�
	for (MenuStrIter it = m_StaticPath.begin(); it != m_StaticPath.end(); ++it) {
		const int n = GetMenuItemCount(it->first);
		for (int j = 0; j < n; ++j)
			DeleteMenu(it->first,0,MF_BYPOSITION);
	}

	// �����̬��ͨ�����¼
	m_DynamicMenuWildcard.clear();
	// �����̬�Ӳ˵���¼
	m_DynamicPath.clear(); //ClearMap(m_DynamicPath);

	// ���¼���
	m_ID = m_dynamicStartID;
}


//! ����һЩ����������Reset() �� �������� ����
void CMenuWithIcon::Destroy(void)
{
	m_ItemIconPath.clear();
	m_ItemCmd.clear();
	m_ItemParam.clear();
	m_StaticPath.clear();
	m_ExpanedMenu.clear();
	m_NameIdMap.clear();

	m_StaticMenuWildcard.clear();
	m_Wildcard.clear();

	m_MenuItemIcons.Clear();
	m_SubMenuIcons.Clear();

	m_ID = m_startID;
	m_dynamicStartID = m_ID;
}


//! ����·����ȡ 16��16 �� 32��32 ͼ��
ICONTYPE CMenuWithIcon::GetIcon(const tString & strPath, EICONGETTYPE needIcon, int iconIndex, bool bIcon32)
{
	const TCHAR *pPath = strPath.c_str();// 
	if(!*pPath || NOICON == needIcon)
		return NULL;
	ICONTYPE hIcon = NULL;
/*
	// �·�ʽ������GetFileInfo ,������ͼ����������ܺ�ֱ�ӻ�������ͬ��
	// �Կ�ݷ�ʽ ICONTYPE �Ǵ��м�ͷ�ģ�index������û�м�ͷ��,
	// �������ϰ취ȡ ICONTYPE ��

	if (!m_bCoInited)
		return NULL;
	SHFILEINFO sfi = {0};
	SHGetFileInfo(pPath,
		FILE_ATTRIBUTE_NORMAL,
		&sfi,
		sizeof(SHFILEINFO),
		SHGFI_ICON | SHGFI_SMALLICON
		| (FILEICON == needIcon ? SHGFI_USEFILEATTRIBUTES : 0) // �����Զ��ļ���ֻ�����ļ���չ����ȡ����ͼ�꣬�����ò����ļ��е���ȷͼ�ꡣ
		);//
	hIcon = sfi.hIcon;
	if (!hIcon && FILEFOLDERICON == needIcon)
			return GetIcon(pPath, FILEICON, iconIndex);

/*/
	if (bIcon32)
		ExtractIconEx(pPath,iconIndex, &hIcon,NULL,1); //�����ļ�����ͼ��
	else
		ExtractIconEx(pPath,iconIndex, NULL,&hIcon,1); //�����ļ�����ͼ��
	if(hIcon)
		return hIcon;

	if (IsStrEndWith(pPath, _T(".lnk"), false)) {
		// ��ݷ�ʽ,��Ҫ�ǳ���ȥ����ݷ�ʽ��ͷ,����(often .url files)ֱ���������SHGetFileInfo
		hIcon = GetShortCutIcon(pPath,bIcon32);
		return hIcon;
	}

	////�ļ��Ĺ���ͼ��,�ļ��е�ͼ��
	if (!m_comInited && !m_comInited.Init()) {
		return hIcon;
	}
	SHFILEINFO sfi = {0};
	SHGetFileInfo(pPath,
		FILE_ATTRIBUTE_NORMAL,
		&sfi,
		sizeof(SHFILEINFO),
		SHGFI_ICON
		| (bIcon32 ? SHGFI_LARGEICON : SHGFI_SMALLICON)
		| (FILEICON == needIcon ? SHGFI_USEFILEATTRIBUTES : 0) // �����Զ��ļ���ֻ�����ļ���չ����ȡ����ͼ�꣬�����ò����ļ��е���ȷͼ�ꡣ
		);//
	hIcon = sfi.hIcon;

	if (!hIcon) {
		// �ļ�·���õ�����д���� "notepad"
		TCHAR path[MAX_PATH] ={0};
		DWORD dwSize = MAX_PATH;
	#ifdef VCPPCOMPILING
		HRESULT hres = AssocQueryString(ASSOCF_OPEN_BYEXENAME,
						 ASSOCSTR_EXECUTABLE,
						 pPath,
						 NULL,
						 path,
						 &dwSize);
	#else
		HRESULT hres = AssocQueryString(0x00000002,//ASSOCF_OPEN_BYEXENAME,// Windows api and gcc .h file do not match
						 static_cast<ASSOCSTR>(2),	//static_cast<ASSOCSTR>(2), // Windows api and gcc .h file do not match
						 pPath,
						 NULL,
						 path,
						 &dwSize);
	#endif

		if ((S_OK == hres) || ((ForceCast<int, HINSTANCE>(FindExecutable(pPath,NULL,path))) > SHELL_MAX_ERROR_VALUE && *path)) {
			if (bIcon32)
				ExtractIconEx(path,0, &hIcon,NULL,1);
			else
				ExtractIconEx(path,0, NULL,&hIcon,1);

		}		
		// ���ڻ�û��ͼ�꣬���ܣ��ļ�����������
	}
// */
	return hIcon;

}


//! ������Բ˵���
int CMenuWithIcon::Reset()
{
	Destroy();
	COwnerDrawMenu::Reset(); // 0
	(*m_menuData).Clear();
	return 0;
}


//! ���ͨ�����¼������������
int CMenuWithIcon::AddWildcard(const tString & str)
{
	if (str.length()) {
		int n = static_cast<int>(m_Wildcard.size());
		for (int i = 0; i < n; ++i)
			if (m_Wildcard[i] == TSTRING(str))
				return i;
		m_Wildcard.push_back(str);
		return static_cast<int>(m_Wildcard.size()) - 1;
	}
	return -1;
}

//! ��������������ͨ�����
const TCHAR * CMenuWithIcon::GetWildcard(int index)
{
	if (index < 0 || index >= static_cast<int>(m_Wildcard.size()))
		return NULL;
	return m_Wildcard[index].c_str();
}


//! ת����Сд��ĸ��
TSTRING & CMenuWithIcon::ToLowerCase(TSTRING &str)
{
	TSTRING::size_type size = str.length();
	for (TSTRING::size_type i = 0; i < size; ++i) {
		str[i] = _totlower(str[i]);
	}
	return str;
}



TSTRING & CMenuWithIcon::DoubleChar(TSTRING & str, const TSTRING::value_type ch)
{
	TSTRING::size_type pos = 0;
	while((pos = str.find(ch,pos)) != str.npos) {
		str.insert(pos,1,ch);
		pos+=2;
	}
	return str;
}

int CMenuWithIcon::MultiModeBuildMenu(MENUTYPE hMenu, const tString & inStrPathForSearch, const tString & inStrName, EBUILDMODE mode,bool bNoFileIcon/* = false*/)
{
	tString strName(inStrName);
	if (!strName.length())
		strName = _T("*");
	std::vector<TSTRING> vStrFilter;
	GetSeparatedString(strName, '|', vStrFilter);
	std::sort(vStrFilter.begin(), vStrFilter.end());
	std::unique(vStrFilter.begin(), vStrFilter.end());
	return MultiModeBuildMenuImpl(hMenu, inStrPathForSearch, strName, vStrFilter,mode, bNoFileIcon);
}

int CMenuWithIcon::BuildMyComputer(MENUTYPE hMenu, const tString & strName)
{	
	int n = 0;
	unsigned long uDriveMask = GetLogicalDrives();
	assert(GetLogicalDrives() == _getdrives());
	
	if (uDriveMask) {
		tString strDrive(_T("A:\\"));
		
		// disable a: and b:
		uDriveMask >>= 2;
		strDrive[0] += 2;

		TCHAR szVolName[MAX_PATH+1] = {0};
		TCHAR szFSName[MAX_PATH+1] = {0};
		tString strMenuName;

		MENUTYPE hSubMenu = 0;
		while (uDriveMask) {
			if (uDriveMask & 1) {
				++n;
				hSubMenu = CreatePopupMenu();
				//��¼��̬�˵�

				//��һ����̬�˵�������Ҫ����
				AddToMap(m_StaticPath, hSubMenu, strDrive+_T("*"));//static ��ʾ��ɾ���� dynamic
				m_StaticMenuWildcard[hSubMenu] = AddWildcard(strName);

				strMenuName = _T("[&") + strDrive + _T("] ");
				tString strVolName;
				if (GetVolumeInformation(strDrive.c_str(), szVolName, MAX_PATH + 1, 0,0,0, szFSName, MAX_PATH + 1) && *szVolName) {
					strVolName = szVolName;
				}
				else {
					SHFILEINFO sfi;
					SHGetFileInfo(strDrive.c_str(), FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi),SHGFI_TYPENAME);
					strVolName = sfi.szTypeName;
				}
				DoubleChar(strVolName, '&');
				strMenuName += strVolName;
				AddSubMenu(hMenu, hSubMenu, strMenuName, strDrive);
			}

			++strDrive[0];
			uDriveMask >>= 1;
      }

	}
	return n;
}
//! ͨ����˵���캯�������ڶ���ģʽ
//!\param mode ��file��folder��expand��expandnow��ģʽ
//!
int CMenuWithIcon::MultiModeBuildMenuImpl(MENUTYPE hMenu, const tString & inStrPathForSearch, const tString & strName, const std::vector<TSTRING> & vStrFilter, EBUILDMODE mode,bool bNoFileIcon/* = false*/)
{
	assert(vStrFilter.size());
	if (_T("\\\\*") == inStrPathForSearch && EEXPAND == mode) {
		return BuildMyComputer(hMenu, strName);
	}

	const TCHAR * pSearch = inStrPathForSearch.c_str();//strPath �������һ��*��������������
	int result = 0;
	WIN32_FIND_DATA fd = {0};
	HANDLE handle = INVALID_HANDLE_VALUE;
	const TCHAR *f = fd.cFileName;
	// NBUF = 1024; �̳�
	TCHAR fullPath[NBUF] = {0};
	size_t len = _tcslen(pSearch)-1;

	memcpy(fullPath,pSearch, len * sizeof(TCHAR));
	fullPath[len] = '\0';	// len : ��Ŀ¼·��(������б��)����

	int nDynamicSubMenus = 0;
	int iDynamicInc = 0;
	if (EDYNAMIC == mode)
		iDynamicInc = 1;

	//std::vector<TSTRING> vStrFilter;
	if (EEXPAND == mode || EEXPANDNOW == mode || EDYNAMIC == mode) {

		// ���������Ŀ¼Ϊ�Ӳ˵���
		handle = FindFirstFile(pSearch,&fd);
		if (handle != INVALID_HANDLE_VALUE) {
			StrStrMap namePath;
			StrStrMap nameName;
			do {
				//�ļ�����Ŀ¼��������б�ߣ�������·����
				memcpy(fullPath + len,f,(1 + _tcslen(f))*sizeof(TCHAR));

				//ֻ�����ļ��У��������ļ��� //���� "." �� ".." Ŀ¼
				if((!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) ||
					(f[0] == '.' && f[1] == '\0' || f[0] == '.' && f[1] == '.' && f[2] == '\0'))
					continue;

				//�ж������ļ���
				if (!m_bShowHidden && (fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
					continue;

				nDynamicSubMenus += iDynamicInc;

				size_t ilen = _tcslen(fullPath);
				fullPath[ilen] = '\0';

				////�ļ�����Ϊ�˵���ʱ�����е� '&' ��չ��  '&&'
				TSTRING strFileName(f);
				DoubleChar(strFileName, '&');

				// ���Դ�Сд
				TSTRING strNameLower(strFileName);
				ToLowerCase(strNameLower);

				nameName[strNameLower] = strFileName;
				namePath[strFileName] = fullPath;

			} while (FindNextFile(handle,&fd));

			FindClose(handle);

			// �����������Ӳ˵�
			MENUTYPE hSubMenu = NULL;
			TSTRING strWildCardPath;
			StrStrMap::iterator itStr,itName;
			bool bStaticMenu = IsStaticMenu(hMenu);
			switch (mode) {
				case EDYNAMIC:
					for (itName = nameName.begin(); itName != nameName.end(); ++itName) {
						hSubMenu = CreatePopupMenu();
						strWildCardPath = namePath[itName->second] + _T("\\*");
						//��¼��̬�˵�

						AddToMap(m_DynamicPath, hSubMenu, strWildCardPath);

						// ��¼ͨ���
						if (bStaticMenu)
							m_DynamicMenuWildcard[hSubMenu] = m_StaticMenuWildcard[hMenu];
						else {
							assert (IsDynamicMenu(hMenu));
							m_DynamicMenuWildcard[hSubMenu] = m_DynamicMenuWildcard[hMenu];
						}

						AddSubMenu(hMenu, hSubMenu, itName->second, namePath[itName->second],FILEFOLDERICON);

					}
					break;
				case EEXPANDNOW:
					for (itName = nameName.begin(); itName != nameName.end(); ++itName) {
						hSubMenu = CreatePopupMenu();
						strWildCardPath = namePath[itName->second] + _T("\\*");
						//��¼��̬�˵�
						MultiModeBuildMenuImpl(hSubMenu,strWildCardPath.c_str(),strName, vStrFilter, EEXPANDNOW,true);

						if (GetMenuItemCount(hSubMenu) > 0) {
							AddSubMenu(hMenu,hSubMenu,itName->second, namePath[itName->second]);
							AddToMap(m_ExpanedMenu, hSubMenu, strWildCardPath);
						}
						else {
							DestroyMenu(hSubMenu);
							hSubMenu = NULL;
						}
					}
					break;
				case EEXPAND:
					for (itName = nameName.begin(); itName != nameName.end(); ++itName) {
						hSubMenu = CreatePopupMenu();
						strWildCardPath = namePath[itName->second] + _T("\\*");
						//��¼��̬�˵�

						//��һ����̬�˵�������Ҫ����
						AddToMap(m_StaticPath, hSubMenu, strWildCardPath);//static ��ʾ��ɾ���� dynamic
						m_StaticMenuWildcard[hSubMenu] = AddWildcard(strName);

						AddSubMenu(hMenu, hSubMenu, itName->second, namePath[itName->second]);
					}
					break;
				default:
					assert(false);
			}

			namePath.clear();
			nameName.clear();
		}
	}

	// ��strNameΪ�������ҵ�ǰĿ¼�е��ļ�

	StrStrMap namePath;
	StrStrMap nameName;
	for (std::vector<TSTRING>::size_type i = 0; i < vStrFilter.size(); ++i) {

		if (!vStrFilter[i].length() && vStrFilter.size() > 1) {
			continue;
		}
		else if ( vStrFilter[i].length() )
			memcpy(fullPath + len,vStrFilter[i].c_str(),(1 + vStrFilter[i].length())*sizeof(TCHAR));
		else
			fullPath[len] = '*', fullPath[len+1] = '\0';

		const TSTRING strSearch(fullPath);
		handle = FindFirstFile(strSearch.c_str(),&fd); // ϵͳ�Ỻ������������fullPath���ԸĶ���  Ans��Ӧ���ǵ�. ��Ҫð�գ��½�һ���ɡ�
		if (handle != INVALID_HANDLE_VALUE) {
			do {
				if(f[0] == '.' && f[1] == '\0' || f[0] == '.' && f[1] == '.' && f[2] == '\0')
					continue;
				if((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && EFOLDER != mode)
					continue;

				if (!m_bShowHidden && (fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
					continue;

				////�ļ�����Ϊ�˵���ʱ�����е� '&' ��չ��  '&&'
				TSTRING strFileName(f);
				if (IsStrEndWith(strFileName, _T(".lnk"), false))
					strFileName.resize(strFileName.size() - 4);
				DoubleChar(strFileName, '&');

				// ������Դ�Сд
				TSTRING strNameLower(strFileName);
				ToLowerCase(strNameLower);
				// ���ʹ��ͨ�������ֹ�ظ��Ĳ��裬��Ȼ�µĻ��滻�ɵġ�
				if (nameName.find(strNameLower) == nameName.end()) {
					nameName[strNameLower] = strFileName;

					//�ļ�����Ŀ¼��������б�ߣ�������·����
					memcpy(fullPath + len,f,(1 + _tcslen(f))*sizeof(TCHAR));
					namePath[strFileName] = fullPath;
				}

			}while(FindNextFile(handle,&fd));

			FindClose(handle);
		}
	}


	// ���������Ĳ˵���Ŀ
	EICONGETTYPE iconGetType = FILEFOLDERICON;
	if (bNoFileIcon || EDYNAMIC == mode)
		iconGetType = NOICON;//��Ҫ�洢aͼ��
	StrStrMap::iterator itName;
	for (itName = nameName.begin(); itName != nameName.end(); ++itName)
		result += AddMenuItem(hMenu,itName->second,namePath[itName->second],iconGetType);

	namePath.clear();
	nameName.clear();

	if (EDYNAMIC == mode && 0 == nDynamicSubMenus && 0 == result) {
		AddMenuItem(hMenu,m_strEmpty, _T(""));//��̬�˵�����ӱ��⡰�ա�
	}
	return result;
}


//! �ҳ���ȫƥ��,���������������ͼ�ꡣ
unsigned int CMenuWithIcon::Find(const TSTRING & strName, TSTRING & strPath)
{
	if(strName.empty() || strName.length() >= MAX_PATH)
		return 0;
	TSTRING strSearch(strName);
	ToLowerCase(strSearch);

	std::map<TSTRING,IDTYPE>::const_iterator iter;
	if((iter = m_NameIdMap.find(strName)) != m_NameIdMap.end() && Cmd(iter->second)) {
		strPath = Cmd(iter->second);
		if(strPath.length() && strPath[0] != '\"')
			strPath = _T("\"") + strPath + _T("\"");
		if(Param(iter->second))
			strPath = strPath + _T(" ") + Param(iter->second);
		return iter->second;
	}
	return 0;
}


//! �ҳ�����ƥ�䣬���뵽ָ���ַ�������ĩβ���Ѵ��ڵ�������
unsigned int CMenuWithIcon::FindAllBeginWith(const TSTRING& strBeginWith,std::vector<TSTRING> &vStrName)
{
	if(strBeginWith.empty() || strBeginWith.length()>=MAX_PATH)
		return 0;
	TSTRING strSearch(strBeginWith);
	ToLowerCase(strSearch);
	TSTRING::size_type size = strSearch.length();

	unsigned int iFound = 0;
	std::map<TSTRING,IDTYPE>::const_iterator iter;
	for(iter = m_NameIdMap.begin(); iter != m_NameIdMap.end(); ++iter) { //m_NameIdMap�ǰ�����ĸ��˳���
		if(iter->first.length() >= size && iter->first.substr(0,size) == strSearch) {
			bool bIgnoreThis = false;
			for (std::vector<TSTRING>::size_type i = 0; i < vStrName.size(); ++i) {
				if (vStrName[i] == iter->first) {
					bIgnoreThis = true;
					break;
				}
			}
			if (!bIgnoreThis) {
				const TCHAR * pCmd = Cmd(iter->second); //�����ų����⡣
				if (pCmd && *pCmd) {
					++iFound;
					vStrName.push_back(iter->first);
				}
			}
		}
	}
	return iFound;
}

//////////////////////////////////////////////////////////////////////////////

/*
int CMenuWithIcon::BuildMenu(FILE * pFile,UINT uStartID)
{
	Reset();
	if(!pFile) {
		return 0;
	}

	m_startID = uStartID;
	m_ID = m_startID;
	const int nStaticMenus = BuildMenu(pFile,Menu());
	m_dynamicStartID = m_ID;

	//���� <����,ID> ӳ��
	TSTRING strName;
	TSTRING::size_type size = 0;
	for (IDTYPE i = m_startID; i < m_dynamicStartID; ++i) {
		strName = ItemName(i);
		ToLowerCase(strName);

		//�Ƴ�ÿ�γ��ֵĵ�һ�� &
		size = strName.length();
		TSTRING::size_type move = 0;
		for (TSTRING::size_type j = 0; j < size; ++j) {
			if(strName[j] == '&') {
				++move;
				++j;
			}
			if(move)
				strName[j-move] = strName[j];
		}
		//�ض�;
		strName.resize(strName.size() - move);

		m_NameIdMap[strName] = i;
	}
	return nStaticMenus;
}

//! �Ӵ򿪵��ļ�����˵����ݹ鹹���Ӳ˵�; ����ȥ���հס�

//! ˽�к�����������ļ� FILE * �����Ѿ���
//! ����ͻ����ʧ�ܡ�
int CMenuWithIcon::BuildMenu(FILE * pFile, MENUTYPE hMenu)
{
	assert(pFile);
	int nItems = 0;

	TCHAR buf[NBUF] = {0};
	int i = 0;//ƫ�ƣ�������ַ�����
	int iStartOfPath = 0; // �˵�Ŀ��·��

#ifdef UNICODE
	wint_t ch;
#else
	int ch;
#endif

	while(i < NBUF && (ch = _fgettc(pFile)) != _TEOF) {
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
				while(i > 0 && _istspace(buf[--i])){
					buf[i] = '\0';
				}
				if (i < 0) // ���и�ʽ��������
				{
					while((ch = _fgettc(pFile)) !='\r' && ch != '\n');// �����з����
					if ((ch = _fgettc(pFile)) != '\n' && '\r' != ch)
						_ungettc(ch, pFile);
					i = 0;
				}
				else {	i += 2;
					iStartOfPath = i;
				}
			}
			else {
				//�ӵڶ�����ʼ�ĵȺ� "=" ������ͨ�ַ�
				++i; //�����ַ��� buf ��
			}
			break;
		case ';':
			// �ֺű�ʾ ��ע��
			while((ch = _fgettc(pFile)) !='\r' && ch != '\n'&& ch != (int)_TEOF) {
				;
			}
			// �����з����
			if ((ch = _fgettc(pFile)) != '\n' && '\r' != ch)
				_ungettc(ch, pFile);
			if (i == 0) {
				// ���ж���ע�ͣ�������
				iStartOfPath = 0;
				continue;//����ѭ����ȡ�ַ���
			}
			else { ; }
			// ��������
		case '\n':
		case '\r':
			if (('\r' == ch || ch == '\n') && i == 0) {
				// ���У����ɷָ���
				InsertMenu(hMenu,(UINT)-1,MF_BYPOSITION | MF_OWNERDRAW | MF_SEPARATOR,0,0);
				if ((ch = _fgettc(pFile)) != '\n' && '\r' != ch)
					_ungettc(ch, pFile);
				iStartOfPath = 0;
				continue;//Ŀǰ��break��ͬ��ģ�break������switch�����õ�ѭ��β���ˣ�continue�Ǽ���ѭ����
			}

			// �����з����
			if ((ch = _fgettc(pFile)) != '\n' && '\r' != ch)
				_ungettc(ch, pFile);

			// ���ַ���������
			buf[i] = '\0';
			while(_istspace(buf[--i]) && buf[i] != '\0')//ȥ����ĩβ�հ�
				buf[i] = '\0';

			switch(*buf)
			{
				MENUTYPE hSubMenu;
				const TCHAR * pName;
			//case '[':
			case '>':
			case '{':
				// �Ӳ˵�
				pName = buf + 1;
				while(*pName && _istspace(*pName)) ++pName;
				if(!*pName) // ûд����,ȱʡ���ʺ�
				{
					pName = _T("< ??? >");
				}
				hSubMenu = CreatePopupMenu();
				nItems += BuildMenu(pFile,hSubMenu);

				if (GetMenuItemCount(hSubMenu) <= 0 && m_bFilterEmptySubMenus) {
					DestroyMenu(hSubMenu);
				}
				else {
					AddSubMenu(hMenu,hSubMenu,pName,iStartOfPath > 0 ? buf + iStartOfPath : NULL);
					if (GetMenuItemCount(hSubMenu) <= 0) {
						for(int i = 0; i < GetMenuItemCount(hMenu); ++i)
							if(GetSubMenu(hMenu,i) == hSubMenu) {
								EnableMenuItem(hMenu,i,MF_BYPOSITION | MF_GRAYED);
								break;
							}
					}
				}
				break;
			//case ']':
			case '<':
			case '}':
				// �Ӳ˵�����
				return nItems;

			default:
				if(0 < iStartOfPath) {
					// ����˵���

					// ����ƥ��ͨ���
					TCHAR * strPath = buf + iStartOfPath;
					if (strPath && *strPath && strPath[_tcslen(strPath)-1]=='*')
						nItems += MultiAddMenuItem(hMenu,strPath,buf);
					else {
						pName = buf;
						while(*pName && _istspace(*pName)) ++pName;
						if(!*pName) {
							// ûд����,ȱʡ���ʺ�
							pName = _T("< ??? >");
						}
						nItems += (AddMenuItem(hMenu,pName, buf + iStartOfPath));//ͳ�Ʋ˵�������
					}
				}
				else //if (buf[0] != '\0' && buf[0] != ';' && (!_istspace(buf[0])))//���ɱ���
					nItems += AddMenuItem(hMenu,buf,_T(""));
				break;

			} // end if switch(*buf)

			memset(buf, 0, sizeof(buf));
			i = 0;
			iStartOfPath = 0;
			break;
		default:
			//��ͨ�ַ�
			++i; //�����ַ��� buf ��
			break;

		} //end of switch(buf[i])

	}//end of while
return nItems;
}


// */
