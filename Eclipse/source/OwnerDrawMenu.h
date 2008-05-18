#ifndef OWNER_DRAW_MENU_H
#define OWNER_DRAW_MENU_H

#pragma once
#include <stdheaders.h>

#include <auto_pointers.h>
#include "MsgMap.h"

typedef unsigned int IDTYPE;
typedef HMENU MENUTYPE;
typedef HICON ICONTYPE;
typedef HWND WINDOWTYPE;


//! 自绘菜单类
class COwnerDrawMenu
{
	AutoHwnd m_hWnd;

	bool m_bUseAble;

public:

	COwnerDrawMenu(ICONTYPE hIconCheck = NULL);
	virtual ~COwnerDrawMenu(void);
	virtual int Display(int x, int y, WINDOWTYPE hWnd = NULL, UINT uFlag = TPM_LEFTALIGN);
	const UINT_PTR GetCurrentItem() {return SelID();}
	const MENUTYPE Menu(){return m_hMenu;};
	const TCHAR * Name (const UINT_PTR indexKey)
	{
		const TCHAR * p = ItemName((IDTYPE)indexKey);
		if (!p && IsMenu((MENUTYPE)indexKey))
			return MenuName((MENUTYPE)indexKey);
		return p;
	}
	const TCHAR * ItemName(const IDTYPE indexKey)	{return GetStr(m_ItemName,indexKey);};
	const TCHAR * MenuName(const MENUTYPE indexKey)	{return GetStr(m_MenuName,indexKey);};
	bool SetName(IDTYPE ID, const TSTRING & strNewName);
	bool SetName(MENUTYPE hSubMenu, const TSTRING & strNewName);
	bool SetNameByPos(int i, const TSTRING & strNewName);
	BOOL Insert(IDTYPE ID,const TCHAR * strName, UINT pos = static_cast<UINT>(-1), ICONTYPE hIcon = NULL);
	BOOL Insert(MENUTYPE hSubMenu,const TCHAR * strName, UINT pos = static_cast<UINT>(-1), ICONTYPE hIcon = NULL);
	void AddStaticIcon(IDTYPE ID,ICONTYPE hIcon);
	void AddStaticIcon(MENUTYPE hSub, ICONTYPE hIcon);
	int Reset();
	UINT SetColor(int index, UINT value);//设置颜色
	void UpdateRoot();

	//! 判断字符串是否以 给定的字符串结尾。
	static bool IsStrEndWith(const TSTRING & strSrc, const TSTRING & strMatchThis, bool bMatchCase = true);

protected:
	UINT_PTR & SelID(){return m_selID;};

	typedef std::map<IDTYPE, TSTRING> IdStrMap;
	typedef IdStrMap :: const_iterator IdStrIter;

	typedef std::map<MENUTYPE, TSTRING> MenuStrMap;
	typedef MenuStrMap :: const_iterator MenuStrIter;

	typedef std::map<IDTYPE,ICONTYPE> IdIconMap;
	typedef IdIconMap::const_iterator IdIconIter;

	typedef std::map<MENUTYPE,ICONTYPE> MenuIconMap;
	typedef MenuIconMap::const_iterator MenuIconIter;

	enum {MENUSIDE = 6, MENUHEIGHT = 22, MENUICON = 16, MENUSEP = 5,MAXMENUWIDTH = 500,NBUF = 1024, SHELL_MAX_ERROR_VALUE = 32};
	IdStrMap & ItemNameMap() {return m_ItemName;};
	MenuStrMap & MenuNameMap() {return m_MenuName;};


	// 模板的成员函数定义

	template <class To,class From> static const To ForceCast(From from)
	{
		return reinterpret_cast<To>(from);
	};

	template<class IdMenu> bool AddToMap(std::map<IdMenu,TSTRING> &strMap, IdMenu indexKey, const TSTRING & str)
	{
		if (str.empty())
			return false;
		strMap[indexKey] = str;
		return true;
	}

	template<class IdMenu> static const TCHAR * GetStr(std::map<IdMenu,TSTRING> &strMap, IdMenu indexKey)
	{
		typename std::map<IdMenu,TSTRING>::const_iterator it = strMap.find(indexKey);
		if (it == strMap.end())
			return 0;
		return it->second.c_str();
	}


	void Destroy(void);
	bool AccordingToState(DRAWITEMSTRUCT * pDI);

private:
	COwnerDrawMenu(const COwnerDrawMenu &);//禁止复制构造
	COwnerDrawMenu & operator = (const COwnerDrawMenu &) ;//禁止赋值
#ifdef _DEBUG
	bool IsParentMenu(MENUTYPE hMenu,UINT_PTR uItemID);
#endif
	int FillRectWithColor(HDC hdc,RECT & rect,COLORREF color);
	void TransformMenu(MENUTYPE hSubMenu);

	MENUTYPE m_hMenu;
	ICONTYPE m_hIconCheck;
	IdStrMap m_ItemName;
	MenuStrMap m_MenuName;
	UINT_PTR m_selID;
	enum e_ClrIndex{ClrIndex_Bk,ClrIndex_BkGrayed,ClrIndex_BkDisabled,
					ClrIndex_Fr,ClrIndex_FrGrayed,ClrIndex_FrDisabled,
					ClrIndex_Sel,ClrIndex_SelBorder,
					ClrIndex_Side,ClrIndex_Separator,
					ClrIndex_Num};
	std::vector<COLORREF> m_vClrs;

	IdIconMap m_IdIcon;
	MenuIconMap m_MenuIcon;


	class CWindowClass
	{
		CWindowClass(const CWindowClass &);
		CWindowClass & operator = (const CWindowClass &);
		const TSTRING m_strWndClassName;
		typedef LRESULT (CALLBACK *WindowProcessor)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	public:
		CWindowClass(HINSTANCE hInstance, const TCHAR * szWindowClass, WindowProcessor WndProc);
		operator const TCHAR * (){return m_strWndClassName.c_str();}
	};
	static CWindowClass s_windowClass;

private:
	// 静态成员，处理窗口消息。

	static const TCHAR *szMenuWindowClass;

	static LRESULT  MsgDrawItem(HWND, UINT, WPARAM, LPARAM);
	static LRESULT  MsgMeasureItem(HWND, UINT, WPARAM, LPARAM);
	static LRESULT  MsgMenuChar(HWND, UINT, WPARAM, LPARAM);
	static LRESULT  MsgMenuSelect(HWND, UINT, WPARAM, LPARAM);

	static CMsgMap msgMap;

	static LRESULT CALLBACK MyWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	typedef std::map<COwnerDrawMenu * , bool> MenuObjects;// count total menus
	static MenuObjects s_Menus;

	void AddThis() { s_Menus[this] = true; }
	void RemoveThis() { s_Menus.erase(this); }

protected:
	MENUTYPE MatchRect(const DRAWITEMSTRUCT * pDI);
	virtual bool DrawItem(DRAWITEMSTRUCT * pDI);
	virtual int MeasureItem(MEASUREITEMSTRUCT *pMI);
	virtual LRESULT MenuChar(MENUTYPE hMenu,TCHAR ch);
	virtual LRESULT MenuSelect(MENUTYPE hMenu,UINT uItem,UINT uFlags);

public:
	typedef HBITMAP BITMAPTYPE;
	void SetSkin(BITMAPTYPE hSide, const BITMAPTYPE (&hBk)[3], const BITMAPTYPE (&hSelBk)[3], const BITMAPTYPE (&hSep)[3], BITMAPTYPE hTitalPic);

private:
	// skin about

	MemDC_H m_hSkinDC;

	void DrawSkin(HDC hdc, const RECT & rect,
							  BITMAPTYPE hMid, int iMidW, int iMidH,
							  BITMAPTYPE hLeft, int iLeftW,
							  BITMAPTYPE hRight, int iRightW);
	typedef gdi_ptr<HBITMAP> SkinPic;
	SkinPic m_hSidePic;	//侧边背景

	SkinPic m_hSepPic;	 //分隔符, 中间
	int m_sepPicHeight;
	int m_sepPicWidth;

	SkinPic m_hSepPicLeft;//分隔符, 左边
	int m_sepLeftWidth;
	SkinPic m_hSepPicRight;//分隔符, 右边
	int m_sepRightWidth;

	SkinPic m_hBkPic;   //背景,中间
	int m_BkHeight;
	int m_BkWidth;
	SkinPic m_hBkPicLeft;   //背景,左边
	int m_BkLeftWidth;
	SkinPic m_hBkPicRight;   //背景,右边
	int m_BkRightWidth;

	SkinPic m_hSelBkPic;   //选中的背景,中间
	int m_selBkHeight;
	int m_selBkWidth;

	SkinPic m_hSelBkPicLeft;   //选中的背景,左边
	int m_selBkLeftWidth;
	SkinPic m_hSelBkPicRight;   //选中的背景,右边
	int m_selBkRightWidth;

	SkinPic m_hTitalPic;
	int m_titalPicWidth;
};

#endif // OWNER_DRAW_MENU_H
