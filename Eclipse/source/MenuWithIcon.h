#ifndef MENU_WITH_ICON_H
#define MENU_WITH_ICON_H

#include "OwnerDrawMenu.h"
#include "MenuData.h"

//! ��ʼ�� com ���࣬����һ������Ϳ����ˡ�Ҳ�����ֶ���ʼ��
class ComIniter
{
	bool m_bSuccess;
public:
	ComIniter(bool bInit = true):m_bSuccess(false){
		if (bInit) {
			Init();
		}
	}
	~ComIniter() { UnInit(); }
	operator bool() { return m_bSuccess; }
	bool Init() {
		return m_bSuccess = m_bSuccess || (SUCCEEDED(CoInitializeEx(NULL,COINIT_APARTMENTTHREADED)));
	}
	void UnInit() {
		if (m_bSuccess) {
			CoUninitialize();
			m_bSuccess = false;
		}
	}
};

// ��ͼ�������˵���
class CMenuWithIcon : public COwnerDrawMenu
{
	typedef COwnerDrawMenu Super;

public:
	CMenuWithIcon(ICONTYPE hOpen = NULL,ICONTYPE hClose = NULL,ICONTYPE hUnknownFile = NULL,const TCHAR * szEmpty = NULL);
	~CMenuWithIcon(void);

	virtual int Display(int x, int y, WINDOWTYPE hWnd = NULL, UINT uFlag = TPM_LEFTALIGN);
private:
	bool DrawItem(DRAWITEMSTRUCT * pDI);
	int MeasureItem(MEASUREITEMSTRUCT *pMI);
	LRESULT MenuSelect(MENUTYPE hMenu,UINT uItem,UINT uFlags);
public:
	ICONTYPE GetBigIcon(const unsigned int id, int index = 0){
		ICONTYPE result = 0;
		if (m_startID <= id && id < m_dynamicStartID) {
			if (m_ItemIconPath.find(id) != m_ItemIconPath.end())
				result = GetIcon(m_ItemIconPath[id],FILEFOLDERICON,index,true);
			else
				result = GetIcon(Cmd(id),FILEFOLDERICON,index,true);
		}
		return result;
	}
	ICONTYPE GetBigIcon(const tString & path, int index = 0){return GetIcon(path,FILEFOLDERICON,index,true);}
	//! ���ز˵����Ӧ�������еĲ���
	const TCHAR * Param(const IDTYPE nID){return GetStr(m_ItemParam,nID);};
	//! ���ز˵����Ӧ��������(��������)
	const TCHAR * Cmd(const IDTYPE nID){return GetStr(m_ItemCmd,nID);};
	unsigned int Find(const TSTRING& strName, TSTRING& strPath);
	unsigned int FindAllBeginWith(const TSTRING& strBeginWith,std::vector<TSTRING> &vStrName, bool bAllowDup = false);
	bool TryProcessCommand(unsigned int id);
	const TSTRING GetCurrentCommandLine(unsigned int nSysID);
	int ItemIDCount(){return m_ID - m_startID;};
	void DestroyDynamic(void);
	int BuildDynamic(MENUTYPE hSubMenu);
	bool & ShowHidden() {return m_bShowHidden;};
	
	int LoadMenuFromFile(const tString & strFileName, UINT uStartID);
	//int BuildMenu(FILE *pFile,UINT uStartID);

	//! �ж��ַ����Ƿ��� �������ַ�����β��
	using Super::IsStrEndWith;

private:
	int Reset(void);
	CMenuWithIcon(const CMenuWithIcon &){};

	static TSTRING & DoubleChar(TSTRING & str, const TSTRING::value_type ch);
	static TSTRING & ToLowerCase(TSTRING &str);

	enum EICONGETTYPE{NOICON, FILEFOLDERICON, FILEICON};

	//typedef std::map<TSTRING, MENUTYPE> StrMenuMap;
	typedef std::map<TSTRING, TSTRING> StrStrMap;

	enum {MENUBLANK = COwnerDrawMenu::MENUHEIGHT - MENUICON};
	const ICONTYPE ItemIcon(const int nID)
	{
		return m_MenuItemIcons[nID];
	}
	const ICONTYPE MenuIcon(const MENUTYPE hSubMenu)
	{
		return m_SubMenuIcons[hSubMenu];
	}

	bool AddSubMenu(MENUTYPE hMenu,MENUTYPE hSubMenu,const tString & strName, const tString & strPath, EICONGETTYPE needIcon = FILEFOLDERICON);
	int AddMenuItem(MENUTYPE hMenu, const tString & strName, const tString & inStrPath, EICONGETTYPE needIcon = FILEFOLDERICON, const tString & strIcon = _T(""));
	int MultiAddMenuItem(MENUTYPE hMenu, const tString & inStrPath,const tString & strName);
	//int BuildMenu(FILE * pFile, MENUTYPE hMenu);
	int DynamicBuild(MENUTYPE hMenu);
	void Destroy(void);
	ICONTYPE GetIcon(const tString & strPath, EICONGETTYPE needIcon = FILEFOLDERICON,int iconIndex = 0, bool bIcon32 = false);
	ICONTYPE GetShortCutIcon(LPCTSTR lpszLinkFile, bool bIcon32 = true);

	bool GetDirectory(const IDTYPE nID, TSTRING & strWorkPath);

	// ģ���ඨ�壬��� �˵�-ͼ�� ����
	template <class CKey, class CValue>
	class CNoNullMap
	{
	public:
		bool Add(const CKey & key, const CValue & value) {
			if (m_map.find(key) != m_map.end()) {
				DestroyIcon(m_map[key]);
			}
			if (value) {
				m_map[key] = value;
				return true;
			}
			return false;
		}
		void Remove(const CKey & key)
		{
			if (m_map.find(key) != m_map.end()) {
				DestroyIcon(m_map[key]);
			}
			m_map.erase(key);
		}
		void Clear()
		{
			for (typename CMap::iterator it = m_map.begin(); it != m_map.end(); ++it) {
				DestroyIcon(it->second);
			}
			m_map.clear();
		}
		CValue operator [] (const CKey & key)
		{
			typename CMap::const_iterator pos(m_map.find(key));
			if (pos != m_map.end())
				return pos->second;
			return NULL;
		}
	private:
		typedef std::map<CKey,CValue> CMap;
		CMap m_map;
	};
	typedef CNoNullMap<IDTYPE,HICON> CIdIconMap;
	typedef CNoNullMap<MENUTYPE,HICON> CMenuIconMap;

	bool IsStaticMenu(MENUTYPE hMenu)
	{
		return m_StaticPath.find(hMenu) != m_StaticPath.end();
	}
	bool IsDynamicMenu(MENUTYPE hMenu)
	{
		return m_DynamicPath.find(hMenu) != m_DynamicPath.end();
	}
	bool IsExpanedMenu(MENUTYPE hMenu)
	{
		return m_ExpanedMenu.find(hMenu) != m_ExpanedMenu.end();
	}
	// ���ͨ�����¼������������ʧ�ܷ��� -1.
	int AddWildcard(const tString & str);
	// ��������������ͨ�����û�з��� NULL
	const TCHAR * GetWildcard(int index);


private :
	enum EBUILDMODE {EFILE,EFOLDER,EEXPAND,EEXPANDNOW,EDYNAMIC};
	int GetFilteredFileList(const TSTRING & inStrPathForSearch, const TCHAR * szFilter, std::vector<TSTRING> & vResult);
	int MultiModeBuildMenuImpl(MENUTYPE hMenu, const tString & inStrPathForSearch, const tString & strName, const std::vector<TSTRING> & vStrFilter, EBUILDMODE mode,bool bNoFileIcon);
	int MultiModeBuildMenu(MENUTYPE, const tString & strPath, const tString & strName, EBUILDMODE mode,bool bNoFileIcon = false);


	std::map<TSTRING, IDTYPE> m_NameIdMap;//���ڲ������ƺ�����Ķ�Ӧ��ϵ,ȫ��Сд��ĸ
	ICONTYPE m_hIconOpen;
	ICONTYPE m_hIconClose;
	ICONTYPE m_hIconUnknowFile;

	IdStrMap m_ItemCmd;
	IdStrMap m_ItemParam;
	IdStrMap m_ItemIconPath;

	CIdIconMap m_MenuItemIcons;
	CMenuIconMap m_SubMenuIcons;

	IDTYPE m_startID;
	IDTYPE m_ID;
	ComIniter m_comInited; //com �Ƿ��Ѿ���ʼ��

	//typedef const std::vector<TSTRING> wildcard_type;
	//std::vector<wildcard_type> m_Wildcard;//�ļ�ͨ�������
	std::vector<TSTRING> m_Wildcard;//�ļ�ͨ�������

	TSTRING m_strEmpty;
	std::map<MENUTYPE,int> m_StaticMenuWildcard;// ��̬Ŀ¼���ļ�ͨ�������
	std::map<MENUTYPE,int> m_DynamicMenuWildcard;// ��̬Ŀ¼���ļ�ͨ�������
	MenuStrMap m_StaticPath;//������� �� ��̬·��
	MenuStrMap m_DynamicPath;// ����ʱ��̬�����·��
	MenuStrMap m_ExpanedMenu;//������ģ����洢ͼ�ꡣ
	IDTYPE m_dynamicStartID;//��¼��̬�˵�����ʼ����������

	bool m_bShowHidden;
	bool m_bFilterEmptySubMenus;

	Ptr<CMenuData> m_menuData;
	//
	int BuildMenuFromMenuData(CMenuData *pMenu, MENUTYPE hMenu);
	int BuildMyComputer(MENUTYPE hMenu, const tString & strName);

};

#endif //MENU_WITH_ICON_H
