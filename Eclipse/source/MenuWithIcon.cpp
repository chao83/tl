
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

//! 按照指定的字符(ch)分割输入字符串(inStr)，输出到指定向量(vStr). 空字符串也有效。
//! 异常版本
unsigned int GetSeparatedString(const TSTRING & inStr, const TSTRING::value_type ch, std::vector<TSTRING> & vStr)
{
	//估计向量长度。
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
//! 按照指定的字符(ch)分割输入字符串(inStr)，输出到指定向量(vStr). 空字符串也有效。

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

//判断 null 输入。
unsigned int GetSeparatedString(const TSTRING::value_type * inStr, const TSTRING::value_type ch, std::vector<TSTRING> & vStr)
{
	if (!inStr) {
		vStr.clear();
		vStr.push_back(_T(""));
		return 1;
	}
	return GetSeparatedString(TSTRING(inStr), ch, vStr);

}

//! 构造函数 CMenuWithIcon
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


//! 显示
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
	else	//菜单项
		SelID() = uItem;
	BuildDynamic(reinterpret_cast<MENUTYPE>(SelID()));
	return 0;
}


//! 绘制菜单。
bool CMenuWithIcon::DrawItem(DRAWITEMSTRUCT * pDI)
{
	//两个可能的类型，菜单与项
	IDTYPE iMaybeID = pDI->itemID;
	MENUTYPE hMaybeMenu = MatchRect(pDI);
	assert(!hMaybeMenu || hMaybeMenu == (HMENU)iMaybeID);
	if (!hMaybeMenu && (iMaybeID < m_startID || iMaybeID >= m_ID) ) {
		hMaybeMenu = (HMENU)iMaybeID;
	}
	AccordingToState(pDI);

	// 主菜单 和 子菜单
	const TCHAR * pPath = NULL;
	if ( ! IsMenu(hMaybeMenu))
		pPath = Cmd(iMaybeID);

	// 菜单图标
	ICONTYPE hIcon = NULL;
	if (hMaybeMenu && IsMenu(hMaybeMenu))
		hIcon = MenuIcon(hMaybeMenu);
	else
		hIcon = ItemIcon(iMaybeID);

	if (hIcon)
		DrawIconEx(pDI->hDC,pDI->rcItem.left + MENUBLANK,pDI->rcItem.top + MENUBLANK/2 ,hIcon,MENUICON,MENUICON,0,NULL,DI_NORMAL|DI_COMPAT);
	else if (hMaybeMenu && IsMenu(hMaybeMenu)) {
		// 子菜单图标
		ICONTYPE hIconSub = NULL;
		bool bDraw = false;//成功绘制动态子菜单
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
			else if (!(pDI->itemState & ODS_SELECTED)) // 没有打开
				hIconSub = m_hIconClose;
			else
				hIconSub = m_hIconOpen;
			if(hIconSub)
				DrawIconEx(pDI->hDC,pDI->rcItem.left + MENUBLANK,pDI->rcItem.top + MENUBLANK/2 ,hIconSub,MENUICON,MENUICON, 0,NULL,DI_NORMAL);
		}
	}
	else if(!pPath) {
		//标题
		DrawText(pDI->hDC,Name(iMaybeID),-1,&(pDI->rcItem),DT_CENTER | DT_SINGLELINE |DT_VCENTER);
		return true;
	}
	else if ( IsStaticMenu(MENUTYPE(pDI->hwndItem)) || IsDynamicMenu(MENUTYPE(pDI->hwndItem)) || IsExpanedMenu(MENUTYPE(pDI->hwndItem))) {
		//尝试动态菜单项（文件）
		SHFILEINFO sfi = {0};
		//memset(m_psfi.Get(),0,sizeof(SHFILEINFO));
		HIMAGELIST hImgList = (HIMAGELIST)SHGetFileInfo(Cmd(iMaybeID),
				FILE_ATTRIBUTE_NORMAL,
				&sfi,
				sizeof(SHFILEINFO),
				SHGFI_SYSICONINDEX | SHGFI_SMALLICON
				//读文件，目前知道 .ico 图标文件的图标需要读文件获取，但对其他一些文件	会失败
				// | SHGFI_USEFILEATTRIBUTES //不读文件，取图标，这样  目前看来   不会失败
				);
		if (!sfi.iIcon) {
			hImgList = (HIMAGELIST)SHGetFileInfo(Cmd(iMaybeID),
					FILE_ATTRIBUTE_NORMAL,
					&sfi,
					sizeof(SHFILEINFO),
					SHGFI_SYSICONINDEX | SHGFI_SMALLICON
					| SHGFI_USEFILEATTRIBUTES //不读文件，取图标，这样  目前看来   不会失败
					);

		}
		assert(sfi.iIcon);
		if (sfi.iIcon) {
			ImageList_DrawEx(hImgList,sfi.iIcon,pDI->hDC,pDI->rcItem.left + MENUBLANK,pDI->rcItem.top + MENUBLANK/2,MENUICON,MENUICON,CLR_NONE,CLR_NONE,ILD_NORMAL);
			//DrawIconEx(pDI->hDC,pDI->rcItem.left + MENUBLANK,pDI->rcItem.top + MENUBLANK/2 ,sfi.hIcon,MENUICON,MENUICON,0,NULL,DI_NORMAL|DI_COMPAT);
		}
	}
	else
		//显示默认未知文件图标
		DrawIconEx(pDI->hDC,pDI->rcItem.left + MENUBLANK,pDI->rcItem.top + MENUBLANK/2 ,m_hIconUnknowFile,MENUICON,MENUICON,0,NULL,DI_NORMAL|DI_COMPAT);

	pDI->rcItem.left += MENUHEIGHT + (MENUBLANK << 1);

	const TCHAR *str = ( hMaybeMenu && IsMenu(hMaybeMenu) ) ? MenuName(hMaybeMenu) : ItemName(iMaybeID);
	if(str && *str) {
		DrawText(pDI->hDC, str,-1,&(pDI->rcItem),DT_LEFT | DT_SINGLELINE |DT_VCENTER);
	}
	return true;
}


//! 设定菜单大小
int CMenuWithIcon::MeasureItem(MEASUREITEMSTRUCT *pMI)
{
	COwnerDrawMenu::MeasureItem(pMI);
	if (pMI->itemID)
		pMI->itemWidth += MENUBLANK * 3;
	return 0;
}


//! 获取快捷方式（.lnk文件）图标
ICONTYPE CMenuWithIcon::GetShortCutIcon(LPCTSTR lpszLinkFile, bool bIcon32)
{
	if (!m_comInited && !m_comInited.Init()) {
		return 0;
	}
	ICONTYPE hReturn = NULL;//返回值

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


//! 添加子菜单，strPath 用于获取图标
bool CMenuWithIcon::AddSubMenu(MENUTYPE hMenu,MENUTYPE hSubMenu,const tString & strName, const tString & strPath, EICONGETTYPE needIcon)
{
	assert(strName.length());
	AddToMap(MenuNameMap(),hSubMenu, strName);
	InsertMenu(hMenu,(UINT)-1,MF_BYPOSITION | MF_OWNERDRAW | MF_POPUP,(UINT_PTR)hSubMenu,NULL);//MenuName(hSubMenu));//

	if (needIcon) {
		if ( ! IsDynamicMenu(hSubMenu) ) {
			//静态目录或文件中的强制目录，必须加入图标
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
				//在系统中找不到，加入我的记录。
				m_SubMenuIcons.Add(hSubMenu, GetIcon(strPath,needIcon));
			}
		}
	}
	return true;
}


//! 添加一个菜单项,负责去掉 前导空白
int CMenuWithIcon::AddMenuItem(MENUTYPE hMenu, const tString & strName, const tString & inStrPath, EICONGETTYPE needIcon, const tString & strIcon)
{
	// ID的起始位置 CMDS;
	Arr<TCHAR> strPath;
	int a = 0;
	a = sizeof(strPath);
	if(int inLen = inStrPath.length()) {
		strPath= new TCHAR [inLen + 1];
		memcpy(strPath.Get(),inStrPath.c_str(), sizeof(TCHAR)*(inLen + 1));
	}
	assert(strName.length());

	// 单一菜单项
	AddToMap(ItemNameMap(),m_ID, strName);
	InsertMenu(hMenu,(UINT)-1,MF_BYPOSITION | MF_OWNERDRAW,m_ID,NULL);//ItemName(m_ID));//

	if(!strPath.Get() || !strPath[0]) {
		// 处理标题
		EnableMenuItem(hMenu,m_ID,MF_BYCOMMAND | MF_DISABLED);
	}
	else {
		// 判断是否有命令行参数。
		// 文件路径加引号
		TCHAR *strCmd = strPath.Get();
		if (*strCmd != '\"')
			AddToMap(m_ItemCmd, m_ID, strPath.Get());
		else {
			while(*(++strCmd) && *strCmd != '\"') ; //处理双引号

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
			// 非动态菜单项，要图标
			const TCHAR *szIconPath = strIcon.empty()?Cmd(m_ID):strIcon.c_str();
			
			AddToMap(m_ItemIconPath, m_ID, strIcon);
			if ( ! IsStaticMenu(hMenu) && ! IsDynamicMenu(hMenu)) {
				m_MenuItemIcons.Add(m_ID,GetIcon(szIconPath, needIcon));
			}
			else  {
				//动态，不在系统列表中的才存储
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


//! 添加通配符表示的菜单项
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

	// 末尾格式：“\*”，“\*.*”，“\**”,其他格式不处理
	// 新模式\*+* :一次性直接展开，但是不保存图标
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

	while(_istspace(*p)) ++p; //去掉空白

	return MultiModeBuildMenu(hMenu,p,strName,mode);
// */

}


//! public 动态实时构造指定子菜单的内容,
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


//! private 动态实时构造指定子菜单的内容,

//! \note 只是生成子菜单和菜单项，子菜单不展开，
//! 想要展开子菜单，对该子菜单调用本函数。
int CMenuWithIcon::DynamicBuild(MENUTYPE hMenu)
{
	// 需要记录每个动态的子菜单和菜单项，//并动态 销毁。

	//跳过无效菜单和已经构造完毕的菜单。
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

	while (*strPath && _istspace(*strPath)) ++strPath;//去掉空白
	assert (strPath[_tcslen(strPath) - 1] =='*');

	return MultiModeBuildMenu(hMenu,strPath,(strName && *strName)?strName:_T("*"),EDYNAMIC);
}


//! 处理菜单命令，调用相应的命令行
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

		// 获取目录的路径
		GetDirectory(nSysID,strWorkPath);
		if ('.' == *pCmd) {	//相对路径
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
			//执行命令失败
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

		// 获取目录的路径
		GetDirectory(nSysID,strWorkPath);
		if ('.' == *pCmd) {	//相对路径
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


//! 从打开的文件构造菜单；
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

	//构造 <名称,ID> 映射
	TSTRING strName;
	TSTRING::size_type size = 0;
	for (IDTYPE i = m_startID; i < m_dynamicStartID; ++i) {
		strName = ItemName(i);
		ToLowerCase(strName);

		//移除每次出现的第一个 &
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
		//截断;
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
			// 特殊模式: \\** , 表示我的电脑
			if (pMenu->Item(index)->Path()[0] == '\\' && pMenu->Item(index)->Path() != _T("\\\\**"))
				continue;// filter begin with '\\' but not "\\**"
		
			// 常规菜单项
			const tString & strPath = pMenu->Item(index)->Path();
			const tString strSep(_T("|||"));
			
			if (!strPath.empty() && 
				*(strPath.rbegin())=='*') {// 匹配通配符
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
					);//统计菜单项总数			
			}
		}
		else if ( pMenu->Item(index)->Name().length() ) {
			//当成标题
			nItems += AddMenuItem(hMenu,pMenu->Item(index)->Name(),_T(""));
		}
		else {
			// empty, saperator
			InsertMenu(hMenu,(UINT)-1,MF_BYPOSITION | MF_OWNERDRAW | MF_SEPARATOR,0,0);
		}

	}
	return nItems;
}


//! 获取文件目录，会截断输入字符串
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


//! 销毁动态元素
void CMenuWithIcon::DestroyDynamic()
{
	if (m_ID == m_dynamicStartID)
		return;

	// 1 清理 动态菜单项
	for (IDTYPE i = m_dynamicStartID; i < m_ID; ++i) {
		//清理图标，命令，名称
		m_ItemCmd.erase(i);
		ItemNameMap().erase(i);
	}

	// 2 清理子菜单记录
	for (MenuStrIter it = m_DynamicPath.begin(); it != m_DynamicPath.end(); ++it) {
		// 清理图标
		m_SubMenuIcons.Remove(it->first);
		// 清理名称记录
		MenuNameMap().erase(it->first);
	}
	// 销毁动态菜单
	for (MenuStrIter it = m_StaticPath.begin(); it != m_StaticPath.end(); ++it) {
		const int n = GetMenuItemCount(it->first);
		for (int j = 0; j < n; ++j)
			DeleteMenu(it->first,0,MF_BYPOSITION);
	}

	// 清除动态的通配符记录
	m_DynamicMenuWildcard.clear();
	// 清除动态子菜单记录
	m_DynamicPath.clear(); //ClearMap(m_DynamicPath);

	// 重新计数
	m_ID = m_dynamicStartID;
}


//! 清理一些东西，仅供Reset() 和 析构函数 调用
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


//! 根据路径获取 16×16 或 32×32 图标
ICONTYPE CMenuWithIcon::GetIcon(const tString & strPath, EICONGETTYPE needIcon, int iconIndex, bool bIcon32)
{
	const TCHAR *pPath = strPath.c_str();// 
	if(!*pPath || NOICON == needIcon)
		return NULL;
	ICONTYPE hIcon = NULL;
/*
	// 新方式，两次GetFileInfo ,这样的图标存起来可能和直接画出来不同，
	// 对快捷方式 ICONTYPE 是带有箭头的，index可以是没有箭头的,
	// 还是用老办法取 ICONTYPE 吧

	if (!m_bCoInited)
		return NULL;
	SHFILEINFO sfi = {0};
	SHGetFileInfo(pPath,
		FILE_ATTRIBUTE_NORMAL,
		&sfi,
		sizeof(SHFILEINFO),
		SHGFI_ICON | SHGFI_SMALLICON
		| (FILEICON == needIcon ? SHGFI_USEFILEATTRIBUTES : 0) // 不尝试读文件，只根据文件扩展名获取关联图标，这样得不到文件夹的正确图标。
		);//
	hIcon = sfi.hIcon;
	if (!hIcon && FILEFOLDERICON == needIcon)
			return GetIcon(pPath, FILEICON, iconIndex);

/*/
	if (bIcon32)
		ExtractIconEx(pPath,iconIndex, &hIcon,NULL,1); //假设文件包含图标
	else
		ExtractIconEx(pPath,iconIndex, NULL,&hIcon,1); //假设文件包含图标
	if(hIcon)
		return hIcon;

	if (IsStrEndWith(pPath, _T(".lnk"), false)) {
		// 快捷方式,主要是尝试去掉快捷方式箭头,否则(often .url files)直接用下面的SHGetFileInfo
		hIcon = GetShortCutIcon(pPath,bIcon32);
		return hIcon;
	}

	////文件的关联图标,文件夹的图标
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
		| (FILEICON == needIcon ? SHGFI_USEFILEATTRIBUTES : 0) // 不尝试读文件，只根据文件扩展名获取关联图标，这样得不到文件夹的正确图标。
		);//
	hIcon = sfi.hIcon;

	if (!hIcon) {
		// 文件路径用的是缩写，如 "notepad"
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
		// 现在还没有图标，可能：文件根本不存在
	}
// */
	return hIcon;

}


//! 清除所以菜单项
int CMenuWithIcon::Reset()
{
	Destroy();
	COwnerDrawMenu::Reset(); // 0
	(*m_menuData).Clear();
	return 0;
}


//! 添加通配符记录，返回索引。
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

//! 跟据索引，返回通配符。
const TCHAR * CMenuWithIcon::GetWildcard(int index)
{
	if (index < 0 || index >= static_cast<int>(m_Wildcard.size()))
		return NULL;
	return m_Wildcard[index].c_str();
}


//! 转换到小写字母。
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
				//记录动态菜单

				//第一级动态菜单，不需要销毁
				AddToMap(m_StaticPath, hSubMenu, strDrive+_T("*"));//static 表示不删除的 dynamic
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
//! 通配符菜单项构造函数，用于多种模式
//!\param mode ：file，folder，expand，expandnow等模式
//!
int CMenuWithIcon::MultiModeBuildMenuImpl(MENUTYPE hMenu, const tString & inStrPathForSearch, const tString & strName, const std::vector<TSTRING> & vStrFilter, EBUILDMODE mode,bool bNoFileIcon/* = false*/)
{
	assert(vStrFilter.size());
	if (_T("\\\\*") == inStrPathForSearch && EEXPAND == mode) {
		return BuildMyComputer(hMenu, strName);
	}

	const TCHAR * pSearch = inStrPathForSearch.c_str();//strPath 包含最后一个*，用于搜索条件
	int result = 0;
	WIN32_FIND_DATA fd = {0};
	HANDLE handle = INVALID_HANDLE_VALUE;
	const TCHAR *f = fd.cFileName;
	// NBUF = 1024; 继承
	TCHAR fullPath[NBUF] = {0};
	size_t len = _tcslen(pSearch)-1;

	memcpy(fullPath,pSearch, len * sizeof(TCHAR));
	fullPath[len] = '\0';	// len : 主目录路径(包括反斜线)长度

	int nDynamicSubMenus = 0;
	int iDynamicInc = 0;
	if (EDYNAMIC == mode)
		iDynamicInc = 1;

	//std::vector<TSTRING> vStrFilter;
	if (EEXPAND == mode || EEXPANDNOW == mode || EDYNAMIC == mode) {

		// 添加所有子目录为子菜单。
		handle = FindFirstFile(pSearch,&fd);
		if (handle != INVALID_HANDLE_VALUE) {
			StrStrMap namePath;
			StrStrMap nameName;
			do {
				//文件或子目录（不带反斜线）的完整路径，
				memcpy(fullPath + len,f,(1 + _tcslen(f))*sizeof(TCHAR));

				//只处理文件夹，跳过非文件夹 //跳过 "." 和 ".." 目录
				if((!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) ||
					(f[0] == '.' && f[1] == '\0' || f[0] == '.' && f[1] == '.' && f[2] == '\0'))
					continue;

				//判断隐藏文件。
				if (!m_bShowHidden && (fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
					continue;

				nDynamicSubMenus += iDynamicInc;

				size_t ilen = _tcslen(fullPath);
				fullPath[ilen] = '\0';

				////文件名作为菜单名时，其中的 '&' 扩展成  '&&'
				TSTRING strFileName(f);
				DoubleChar(strFileName, '&');

				// 忽略大小写
				TSTRING strNameLower(strFileName);
				ToLowerCase(strNameLower);

				nameName[strNameLower] = strFileName;
				namePath[strFileName] = fullPath;

			} while (FindNextFile(handle,&fd));

			FindClose(handle);

			// 构造排序后的子菜单
			MENUTYPE hSubMenu = NULL;
			TSTRING strWildCardPath;
			StrStrMap::iterator itStr,itName;
			bool bStaticMenu = IsStaticMenu(hMenu);
			switch (mode) {
				case EDYNAMIC:
					for (itName = nameName.begin(); itName != nameName.end(); ++itName) {
						hSubMenu = CreatePopupMenu();
						strWildCardPath = namePath[itName->second] + _T("\\*");
						//记录动态菜单

						AddToMap(m_DynamicPath, hSubMenu, strWildCardPath);

						// 记录通配符
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
						//记录动态菜单
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
						//记录动态菜单

						//第一级动态菜单，不需要销毁
						AddToMap(m_StaticPath, hSubMenu, strWildCardPath);//static 表示不删除的 dynamic
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

	// 以strName为条件，找当前目录中的文件

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
		handle = FindFirstFile(strSearch.c_str(),&fd); // 系统会缓存搜索条件？fullPath可以改动？  Ans：应该是的. 不要冒险，新建一个吧。
		if (handle != INVALID_HANDLE_VALUE) {
			do {
				if(f[0] == '.' && f[1] == '\0' || f[0] == '.' && f[1] == '.' && f[2] == '\0')
					continue;
				if((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && EFOLDER != mode)
					continue;

				if (!m_bShowHidden && (fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
					continue;

				////文件名作为菜单名时，其中的 '&' 扩展成  '&&'
				TSTRING strFileName(f);
				if (IsStrEndWith(strFileName, _T(".lnk"), false))
					strFileName.resize(strFileName.size() - 4);
				DoubleChar(strFileName, '&');

				// 排序忽略大小写
				TSTRING strNameLower(strFileName);
				ToLowerCase(strNameLower);
				// 多次使用通配符，防止重复的步骤，虽然新的会替换旧的。
				if (nameName.find(strNameLower) == nameName.end()) {
					nameName[strNameLower] = strFileName;

					//文件或子目录（不带反斜线）的完整路径，
					memcpy(fullPath + len,f,(1 + _tcslen(f))*sizeof(TCHAR));
					namePath[strFileName] = fullPath;
				}

			}while(FindNextFile(handle,&fd));

			FindClose(handle);
		}
	}


	// 构造排序后的菜单项目
	EICONGETTYPE iconGetType = FILEFOLDERICON;
	if (bNoFileIcon || EDYNAMIC == mode)
		iconGetType = NOICON;//不要存储a图标
	StrStrMap::iterator itName;
	for (itName = nameName.begin(); itName != nameName.end(); ++itName)
		result += AddMenuItem(hMenu,itName->second,namePath[itName->second],iconGetType);

	namePath.clear();
	nameName.clear();

	if (EDYNAMIC == mode && 0 == nDynamicSubMenus && 0 == result) {
		AddMenuItem(hMenu,m_strEmpty, _T(""));//动态菜单，添加标题“空”
	}
	return result;
}


//! 找出完全匹配,根据名称找命令和图标。
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


//! 找出部分匹配，加入到指定字符串向量末尾，已存在的跳过。
unsigned int CMenuWithIcon::FindAllBeginWith(const TSTRING& strBeginWith,std::vector<TSTRING> &vStrName)
{
	if(strBeginWith.empty() || strBeginWith.length()>=MAX_PATH)
		return 0;
	TSTRING strSearch(strBeginWith);
	ToLowerCase(strSearch);
	TSTRING::size_type size = strSearch.length();

	unsigned int iFound = 0;
	std::map<TSTRING,IDTYPE>::const_iterator iter;
	for(iter = m_NameIdMap.begin(); iter != m_NameIdMap.end(); ++iter) { //m_NameIdMap是按照字母表顺序的
		if(iter->first.length() >= size && iter->first.substr(0,size) == strSearch) {
			bool bIgnoreThis = false;
			for (std::vector<TSTRING>::size_type i = 0; i < vStrName.size(); ++i) {
				if (vStrName[i] == iter->first) {
					bIgnoreThis = true;
					break;
				}
			}
			if (!bIgnoreThis) {
				const TCHAR * pCmd = Cmd(iter->second); //用于排除标题。
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

	//构造 <名称,ID> 映射
	TSTRING strName;
	TSTRING::size_type size = 0;
	for (IDTYPE i = m_startID; i < m_dynamicStartID; ++i) {
		strName = ItemName(i);
		ToLowerCase(strName);

		//移除每次出现的第一个 &
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
		//截断;
		strName.resize(strName.size() - move);

		m_NameIdMap[strName] = i;
	}
	return nStaticMenus;
}

//! 从打开的文件构造菜单；递归构造子菜单; 负责去掉空白。

//! 私有函数，传入的文件 FILE * 必须已经打开
//! 否则就会出错失败。
int CMenuWithIcon::BuildMenu(FILE * pFile, MENUTYPE hMenu)
{
	assert(pFile);
	int nItems = 0;

	TCHAR buf[NBUF] = {0};
	int i = 0;//偏移，读入的字符个数
	int iStartOfPath = 0; // 菜单目标路径

#ifdef UNICODE
	wint_t ch;
#else
	int ch;
#endif

	while(i < NBUF && (ch = _fgettc(pFile)) != _TEOF) {
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
					while((ch = _fgettc(pFile)) !='\r' && ch != '\n');// 处理换行符类别
					if ((ch = _fgettc(pFile)) != '\n' && '\r' != ch)
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
			if ((ch = _fgettc(pFile)) != '\n' && '\r' != ch)
				_ungettc(ch, pFile);
			if (i == 0) {
				// 整行都是注释，跳过。
				iStartOfPath = 0;
				continue;//继续循环读取字符。
			}
			else { ; }
			// 继续向下
		case '\n':
		case '\r':
			if (('\r' == ch || ch == '\n') && i == 0) {
				// 空行，当成分隔线
				InsertMenu(hMenu,(UINT)-1,MF_BYPOSITION | MF_OWNERDRAW | MF_SEPARATOR,0,0);
				if ((ch = _fgettc(pFile)) != '\n' && '\r' != ch)
					_ungettc(ch, pFile);
				iStartOfPath = 0;
				continue;//目前和break是同意的，break是跳出switch，正好到循环尾部了，continue是继续循环。
			}

			// 处理换行符类别
			if ((ch = _fgettc(pFile)) != '\n' && '\r' != ch)
				_ungettc(ch, pFile);

			// 本字符串结束，
			buf[i] = '\0';
			while(_istspace(buf[--i]) && buf[i] != '\0')//去掉行末尾空白
				buf[i] = '\0';

			switch(*buf)
			{
				MENUTYPE hSubMenu;
				const TCHAR * pName;
			//case '[':
			case '>':
			case '{':
				// 子菜单
				pName = buf + 1;
				while(*pName && _istspace(*pName)) ++pName;
				if(!*pName) // 没写名字,缺省加问号
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
				// 子菜单结束
				return nItems;

			default:
				if(0 < iStartOfPath) {
					// 常规菜单项

					// 尝试匹配通配符
					TCHAR * strPath = buf + iStartOfPath;
					if (strPath && *strPath && strPath[_tcslen(strPath)-1]=='*')
						nItems += MultiAddMenuItem(hMenu,strPath,buf);
					else {
						pName = buf;
						while(*pName && _istspace(*pName)) ++pName;
						if(!*pName) {
							// 没写名字,缺省加问号
							pName = _T("< ??? >");
						}
						nItems += (AddMenuItem(hMenu,pName, buf + iStartOfPath));//统计菜单项总数
					}
				}
				else //if (buf[0] != '\0' && buf[0] != ';' && (!_istspace(buf[0])))//当成标题
					nItems += AddMenuItem(hMenu,buf,_T(""));
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
}


// */
