#ifndef LCH_MENU_DATA_H
#define LCH_MENU_DATA_H

#include <string>
#include <vector>
#include <auto_handle.h>

#include "FileStrFnc.h"

class CMenuData;
class CItem : protected CFileStrFnc
{
	friend class CMenuData;
protected:
	typedef tString TS;
	typedef const TS CS;
	typedef const TS & CRTS;
	typedef TS & RTS;
	typedef CRTS Prm;
	typedef unsigned int Ui;
	inline static CS Empty() {return _T("");}
public:
	explicit CItem( Prm strName, Prm strPath = Empty(), Prm strEx = Empty() );

	virtual ~CItem() {}

	Prm Name() const { return Get(0); }
	Prm Path() const { return Get(1); }
	Prm Ex() const { return Get(2);}
	Prm Icon() const {return Ex().length()?Ex():Path();}

	TS & Name() { return Get(0); }
	TS & Path() { return Get(1); }
	Prm Name(Prm strName) { return Get(0) = strName; }
	Prm Path(Prm strPath) { return Get(1) = strPath; }
	Prm Ex(Prm strEx) { return Get(2) = strEx; }
	Prm Icon(Prm strEx) { return Get(2) = strEx; }

private:
	virtual bool OutPut(FILE * pFile, wchar_t pad = '\t', int nPad = 0) ;
	Prm Get(Ui n) const { return m_str.at(n); }
	TS & Get(Ui n) { return m_str.at(n); }

private:
	std::vector<TS> m_str; // { name, path }
	//TS::value_type m_sep;// '='

};


class CMenuData : public CItem//, private CFileStrFnc
{
private:
	std::vector<CItem*> m_sub;
	CMenuData & operator = (const CMenuData &) ;

	CItem * _Get(Ui pos) {	return pos < m_sub.size() ? m_sub[pos] : 0; }
public:
	using CItem::Name;
	using CItem::Path;

	explicit CMenuData( Prm strName, Prm strPath = Empty(), Prm strEx = Empty() );
	~CMenuData();
	Ui Count() const { return m_sub.size(); }

	CMenuData * Menu(Ui pos) { return dynamic_cast<CMenuData*>(_Get(pos)); }
	CItem * Item(Ui pos) { return _Get(pos); }
	//CItem & Ref(Ui pos) {
	//	return *m_sub.at(pos);
	//}
	bool IsMenu(Ui pos) { return Menu(pos) != 0; }

	void Clear(); 

	bool AddItem (Ui pos, Prm strName, Prm strPath, Prm strEx = Empty()) ;

	bool AddMenu(Ui pos, Prm strName, Prm strPath, Prm strEx = Empty());

	bool Remove(Ui pos) ;

	bool SaveAs(CRTS strFileName, TCHAR pad = '\t', int nPad = 0, int step = 1);

	int Load(CRTS strFileName);

protected:
	bool OutPut(FILE * pFile, TCHAR pad = '\t', int nPad = 0, int step = 1) ;
	int LoadFile(FILE *pFile) ;
	int LoadFileAlter(FILE *pFile) ;
	
};

#endif
