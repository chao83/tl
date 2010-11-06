/***************************************************************
 * Name:      TLMenuCfgMain.cpp
 * Purpose:   Code for Application Frame
 * Author:     (lichao)
 * Created:   2010-10-31
 * Copyright:  (lichao)
 * License: GPL 3.0
 **************************************************************/

#include "wx_pch.h"
#include "TLMenuCfgMain.h"
#include <wx/msgdlg.h>
#include "MenuItemData.h"

//(*InternalHeaders(TLMenuCfgDialog)
#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)

//helper functions
enum wxbuildinfoformat {
	short_f, long_f
};

wxString wxbuildinfo(wxbuildinfoformat format)
{
	wxString wxbuild(wxVERSION_STRING);

	if (format == long_f )
	{
#if defined(__WXMSW__)
		wxbuild << _T("-Windows");
#elif defined(__UNIX__)
		wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
		wxbuild << _T("-Unicode build");
#else
		wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
	}

	return wxbuild;
}

//(*IdInit(TLMenuCfgDialog)
const long TLMenuCfgDialog::ID_STATICTEXT2 = wxNewId();
const long TLMenuCfgDialog::ID_SEARCHCTRL1 = wxNewId();
const long TLMenuCfgDialog::ID_TREECTRL_MENU = wxNewId();
const long TLMenuCfgDialog::ID_BITMAPBUTTON1 = wxNewId();
const long TLMenuCfgDialog::ID_BITMAPBUTTON3 = wxNewId();
const long TLMenuCfgDialog::ID_BITMAPBUTTON4 = wxNewId();
const long TLMenuCfgDialog::ID_BITMAPBUTTON5 = wxNewId();
const long TLMenuCfgDialog::ID_BITMAPBUTTON2 = wxNewId();
const long TLMenuCfgDialog::ID_CHECKBOX4 = wxNewId();
const long TLMenuCfgDialog::ID_CHECKBOX5 = wxNewId();
const long TLMenuCfgDialog::ID_STATICTEXT1 = wxNewId();
const long TLMenuCfgDialog::ID_CHECKBOX1 = wxNewId();
const long TLMenuCfgDialog::ID_TEXTCTRL1 = wxNewId();
const long TLMenuCfgDialog::ID_BITMAPBUTTON6 = wxNewId();
const long TLMenuCfgDialog::ID_STATICTEXT3 = wxNewId();
const long TLMenuCfgDialog::ID_CHECKBOX2 = wxNewId();
const long TLMenuCfgDialog::ID_TEXTCTRL2 = wxNewId();
const long TLMenuCfgDialog::ID_STATICTEXT4 = wxNewId();
const long TLMenuCfgDialog::ID_TEXTCTRL3 = wxNewId();
const long TLMenuCfgDialog::ID_BITMAPBUTTON7 = wxNewId();
const long TLMenuCfgDialog::ID_BUTTON3 = wxNewId();
const long TLMenuCfgDialog::ID_BUTTON4 = wxNewId();
const long TLMenuCfgDialog::ID_BUTTON2 = wxNewId();
const long TLMenuCfgDialog::ID_BUTTON7 = wxNewId();
const long TLMenuCfgDialog::ID_BUTTON6 = wxNewId();
//*)

BEGIN_EVENT_TABLE(TLMenuCfgDialog,wxDialog)
	//(*EventTable(TLMenuCfgDialog)
	//*)
END_EVENT_TABLE()

TLMenuCfgDialog::TLMenuCfgDialog(wxWindow* parent,wxWindowID id)
	:m_bInfoUnsaved(false), m_menuData(_T("root"))
{
	//(*Initialize(TLMenuCfgDialog)
	wxBoxSizer* BoxSizer4;
	wxBoxSizer* BoxSizer6;
	wxBoxSizer* BoxSizer15;
	wxBoxSizer* BoxSizer19;
	wxBoxSizer* BoxSizer5;
	wxBoxSizer* BoxSizer10;
	wxBoxSizer* BoxSizer7;
	wxBoxSizer* BoxSizer8;
	wxBoxSizer* BoxSizer13;
	wxBoxSizer* BoxSizer2;
	wxBoxSizer* BoxSizer11;
	wxBoxSizer* BoxSizer16;
	wxBoxSizer* BoxSizer18;
	wxBoxSizer* BoxSizer12;
	wxBoxSizer* BoxSizer14;
	wxBoxSizer* BoxSizer17;
	wxBoxSizer* BoxSizer9;
	wxBoxSizer* BoxSizer3;

	Create(parent, wxID_ANY, _("Tray Launcher Command Editor"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	SetClientSize(wxSize(693,416));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	BoxSizer4 = new wxBoxSizer(wxVERTICAL);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("\"Menu\""), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	BoxSizer2->Add(StaticText2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_search = new wxSearchCtrl(this, ID_SEARCHCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_SEARCHCTRL1"));
	BoxSizer2->Add(m_search, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer4->Add(BoxSizer2, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
	m_TreeMenu = new wxTreeCtrl(this, ID_TREECTRL_MENU, wxDefaultPosition, wxSize(198,333), wxTR_HIDE_ROOT|wxTR_DEFAULT_STYLE, wxDefaultValidator, _T("ID_TREECTRL_MENU"));
	BoxSizer8->Add(m_TreeMenu, 1, wxTOP|wxBOTTOM|wxLEFT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer5 = new wxBoxSizer(wxVERTICAL);
	m_btnUp = new wxBitmapButton(this, ID_BITMAPBUTTON1, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_GO_UP")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON1"));
	BoxSizer5->Add(m_btnUp, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_btnDown = new wxBitmapButton(this, ID_BITMAPBUTTON3, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_GO_DOWN")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON3"));
	m_btnDown->SetDefault();
	BoxSizer5->Add(m_btnDown, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_btnNewDir = new wxBitmapButton(this, ID_BITMAPBUTTON4, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_NEW_DIR")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON4"));
	m_btnNewDir->SetDefault();
	BoxSizer5->Add(m_btnNewDir, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_btnNewItem = new wxBitmapButton(this, ID_BITMAPBUTTON5, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_NEW")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON5"));
	m_btnNewItem->SetDefault();
	BoxSizer5->Add(m_btnNewItem, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_btnDel = new wxBitmapButton(this, ID_BITMAPBUTTON2, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_DELETE")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON2"));
	m_btnDel->SetDefault();
	BoxSizer5->Add(m_btnDel, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer8->Add(BoxSizer5, 0, wxTOP|wxBOTTOM|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
	BoxSizer4->Add(BoxSizer8, 1, wxTOP|wxBOTTOM|wxLEFT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer3->Add(BoxSizer4, 0, wxTOP|wxBOTTOM|wxLEFT|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
	BoxSizer7 = new wxBoxSizer(wxVERTICAL);
	BoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
	m_flgMenu = new wxCheckBox(this, ID_CHECKBOX4, _("IsSubMenu"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX4"));
	m_flgMenu->SetValue(false);
	m_flgMenu->Disable();
	BoxSizer9->Add(m_flgMenu, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_flgSep = new wxCheckBox(this, ID_CHECKBOX5, _("IsSeperater"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX5"));
	m_flgSep->SetValue(false);
	m_flgSep->Disable();
	BoxSizer9->Add(m_flgSep, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer7->Add(BoxSizer9, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 6);
	BoxSizer11 = new wxBoxSizer(wxVERTICAL);
	BoxSizer10 = new wxBoxSizer(wxHORIZONTAL);
	m_stcTarget = new wxStaticText(this, ID_STATICTEXT1, _("Target"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	BoxSizer10->Add(m_stcTarget, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_flgWildCard = new wxCheckBox(this, ID_CHECKBOX1, _("IsWildCard (*Mode)"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	m_flgWildCard->SetValue(false);
	m_flgWildCard->Disable();
	BoxSizer10->Add(m_flgWildCard, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer11->Add(BoxSizer10, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer17 = new wxBoxSizer(wxHORIZONTAL);
	m_txtTarget = new wxTextCtrl(this, ID_TEXTCTRL1, _("Text"), wxDefaultPosition, wxSize(456,24), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	BoxSizer17->Add(m_txtTarget, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_btnFindTarget = new wxBitmapButton(this, ID_BITMAPBUTTON6, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FIND")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON6"));
	m_btnFindTarget->SetDefault();
	BoxSizer17->Add(m_btnFindTarget, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer11->Add(BoxSizer17, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer7->Add(BoxSizer11, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer13 = new wxBoxSizer(wxVERTICAL);
	BoxSizer12 = new wxBoxSizer(wxHORIZONTAL);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Name / Filter"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	BoxSizer12->Add(StaticText3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_flgTitle = new wxCheckBox(this, ID_CHECKBOX2, _("IsTitle"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
	m_flgTitle->SetValue(false);
	m_flgTitle->Disable();
	BoxSizer12->Add(m_flgTitle, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer13->Add(BoxSizer12, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer18 = new wxBoxSizer(wxHORIZONTAL);
	m_txtNameOrFilter = new wxTextCtrl(this, ID_TEXTCTRL2, _("Text"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	BoxSizer18->Add(m_txtNameOrFilter, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer13->Add(BoxSizer18, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer7->Add(BoxSizer13, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer15 = new wxBoxSizer(wxVERTICAL);
	BoxSizer14 = new wxBoxSizer(wxHORIZONTAL);
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Customize Icon"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	BoxSizer14->Add(StaticText4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer15->Add(BoxSizer14, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer19 = new wxBoxSizer(wxHORIZONTAL);
	m_txtIcon = new wxTextCtrl(this, ID_TEXTCTRL3, _("Text"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL3"));
	BoxSizer19->Add(m_txtIcon, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BitmapButton2 = new wxBitmapButton(this, ID_BITMAPBUTTON7, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FIND")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON7"));
	BoxSizer19->Add(BitmapButton2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer15->Add(BoxSizer19, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer7->Add(BoxSizer15, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer16 = new wxBoxSizer(wxHORIZONTAL);
	m_btnSave = new wxButton(this, ID_BUTTON3, _("Save"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
	BoxSizer16->Add(m_btnSave, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_btnReload = new wxButton(this, ID_BUTTON4, _("Reload"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON4"));
	BoxSizer16->Add(m_btnReload, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer7->Add(BoxSizer16, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer7->Add(-1,-1,1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
	m_btnOK = new wxButton(this, ID_BUTTON2, _("OK"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	BoxSizer6->Add(m_btnOK, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 4);
	m_btnApply = new wxButton(this, ID_BUTTON7, _("Apply"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON7"));
	BoxSizer6->Add(m_btnApply, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_btnCancel = new wxButton(this, ID_BUTTON6, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON6"));
	BoxSizer6->Add(m_btnCancel, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer7->Add(BoxSizer6, 0, wxALIGN_RIGHT|wxALIGN_BOTTOM, 5);
	BoxSizer3->Add(BoxSizer7, 1, wxTOP|wxBOTTOM|wxRIGHT|wxEXPAND|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
	BoxSizer1->Add(BoxSizer3, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->SetSizeHints(this);
	Center();

	Connect(ID_TREECTRL_MENU,wxEVT_COMMAND_TREE_SEL_CHANGED,(wxObjectEventFunction)&TLMenuCfgDialog::OnTreeMenuSelectionChanged);
	Connect(ID_TREECTRL_MENU,wxEVT_COMMAND_TREE_SEL_CHANGING,(wxObjectEventFunction)&TLMenuCfgDialog::OnTreeMenuSelChanging);
	Connect(ID_BITMAPBUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TLMenuCfgDialog::OnbtnUpClick);
	Connect(ID_BITMAPBUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TLMenuCfgDialog::OnbtnDownClick);
	Connect(ID_BITMAPBUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TLMenuCfgDialog::OnbtnDelClick);
	Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&TLMenuCfgDialog::OntxtTargetText);
	Connect(ID_TEXTCTRL2,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&TLMenuCfgDialog::OntxtNameOrFilterText);
	Connect(ID_TEXTCTRL3,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&TLMenuCfgDialog::OntxtIconText);
	Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TLMenuCfgDialog::OnbtnSaveClick);
	Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TLMenuCfgDialog::OnbtnReloadClick);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TLMenuCfgDialog::OnQuit);
	Connect(wxID_ANY,wxEVT_INIT_DIALOG,(wxObjectEventFunction)&TLMenuCfgDialog::OnInit);
	//*)
}

TLMenuCfgDialog::~TLMenuCfgDialog()
{
	//(*Destroy(TLMenuCfgDialog)
	//*)
}

void TLMenuCfgDialog::OnQuit(wxCommandEvent& event)
{
	Close();
}

void TLMenuCfgDialog::OnAbout(wxCommandEvent& event)
{
	wxString msg = wxbuildinfo(long_f);
	wxMessageBox(msg, _("Welcome to..."));
}


namespace {

//! \brief read name, path, and icon path from CItem
//!
//! \param mi const CItem& : the menu item to read from
//! \param strName TSTRING& : save name
//! \param strPath TSTRING& : save path
//! \param strIcon TSTRING& : sava icon path
//! \return void
//! Called only in this file.
//!
void GetMenuStrings(const CItem &mi, TSTRING &strName, TSTRING &strPath, TSTRING &strIcon)
{
	TSTRING strSep(_T("|||"));
	strName = mi.Name();
	strPath = (mi.Path());
	TSTRING::size_type sepPos = strPath.find(strSep);

	if (TSTRING::npos != sepPos)
	{
		strIcon = ns_file_str_ops::StripSpaces( strPath.substr(sepPos + strSep.length()) );
		strPath = ns_file_str_ops::StripSpaces( strPath.substr(0, sepPos) );

		if (!strIcon.empty() && '\"' == strIcon[0]) {
			TSTRING::size_type pos = strIcon.find('\"', 1);
			strIcon = strIcon.substr(1, pos == TSTRING::npos ? pos : pos - 1);
		}
	}
}

//! \brief update the display of tree item according to its item data
//!
//! \param tree wxTreeCtrl&, the tree ctrl to update
//! \param id wxTreeItemId, the item to update
//! \return void
//!
//!
void UpdateDisplayName(wxTreeCtrl &tree, wxTreeItemId id)
{
	assert(id.IsOk());
	MenuItemData * md(static_cast<MenuItemData*>(tree.GetItemData(id)));
	assert(md && dynamic_cast<MenuItemData*>(tree.GetItemData(id)));

	TSTRING strDisplay(md->Name());

	if (strDisplay.empty())
	{
		strDisplay = _T("< ??? >");

		if (md->Target().empty() && md->IconPath().empty())
		{
			strDisplay = _T("----------------");
		}
	}

	tree.SetItemText(id, strDisplay);
}

//! \brief transfer menu item info (name,path and icon) to tree item data.
//!
//! \param mi const CItem&, menu item to read
//! \param tree wxTreeCtrl&, ref of the tree
//! \param id wxTreeItemId, id of the tree item.
//! \return void
//! this function deal with a menu item(leaf), not a sub-menu.
//!
void MenuDataToTree(const CItem &mi, wxTreeCtrl &tree, wxTreeItemId id)
{
	assert(id.IsOk());
	TSTRING strName, strPath, strIcon;
	GetMenuStrings(mi, strName, strPath, strIcon);

	tree.SetItemData(id, new MenuItemData(strName, strPath, strIcon));
	UpdateDisplayName(tree, id);
}

//! \brief transfer sub-menu info (name,path and icon) to tree item data.
//!
//! \param mi const CMenuData&, submenu to read
//! \param tree wxTreeCtrl&, ref of the tree
//! \param id wxTreeItemId, id of the tree item
//! \return void
//! this function will create tree items for children of mi recursively.
//!
void MenuDataToTree(const CMenuData &mi, wxTreeCtrl &tree, wxTreeItemId id)
{
	assert(id.IsOk());

	tree.SetItemData(id, new MenuItemData(mi.Name(), _T(""), mi.Icon()));

	for (unsigned int i = 0; i < mi.Count(); ++i)
	{
		wxTreeItemId idsub(tree.AppendItem(id, mi.Item(i)->Name()));

		if (mi.IsMenu(i)) { MenuDataToTree(*mi.Menu(i), tree, idsub); }
		else { MenuDataToTree(*mi.Item(i), tree, idsub); }
	}

}

bool MoveItem(wxTreeCtrl &tree, wxTreeItemId from, wxTreeItemId to, const bool before = true)
{
	if(!from.IsOk() || !to.IsOk() || tree.HasChildren(from))
	{
		return false;
	}

	wxTreeItemId parent(tree.GetItemParent(to));

	if (!parent)
	{
		return false;
	}

	tree.Freeze(); //disable screen update

	wxTreeItemId pos(before ? tree.GetPrevSibling(to) : to);

	// add a new item, copy 'from' to new item.
	if (pos)
	{
		tree.SelectItem(tree.InsertItem(parent, pos, tree.GetItemText(from), -1, -1, tree.GetItemData(from)));
	}
	else
	{
		tree.SelectItem(tree.PrependItem(parent, tree.GetItemText(from), -1, -1, tree.GetItemData(from)));
	}

	// clear old data;
	tree.SetItemData(from, NULL);
	tree.Delete(from);

	tree.Thaw();//enable screen update

	return true;
}

} // end of namespace <unnamed>

void TLMenuCfgDialog::OnInit(wxInitDialogEvent& event)
{
	m_menuData.Load(_T("TLCmd.txt"));

	wxTreeItemId idRoot = m_TreeMenu->AddRoot(m_menuData.Name());

	MenuDataToTree(m_menuData, *m_TreeMenu, idRoot);
}

void TLMenuCfgDialog::InfoChgFlg(const bool val)
{
	if (val != m_bInfoUnsaved)
	{
		m_bInfoUnsaved = val;

		if (!val)
		{
			wxFont font(m_txtTarget->GetFont());
			font.SetWeight(wxNORMAL);

			m_txtTarget->SetFont(font);
			m_txtNameOrFilter->SetFont(font);
			m_txtIcon->SetFont(font);
		}
	}
}

void TLMenuCfgDialog::OnTreeMenuSelectionChanged(wxTreeEvent& event)
{
	wxTreeItemId item = event.GetItem();

	if (item.IsOk())
	{
		assert(item == m_TreeMenu->GetSelection());
		// update detail info
		ReadItemInfo();

		UpdateFlgs();
	}
}

void TLMenuCfgDialog::OnbtnSaveClick(wxCommandEvent& event)
{
	if(InfoChgFlg())
	{
		SaveItemInfo();
	}
}

void TLMenuCfgDialog::OnTreeMenuSelChanging(wxTreeEvent& event)
{
	if (InfoChgFlg() && event.GetOldItem().IsOk())
	{
		switch(wxMessageBox(_T("Menu_Item_Changed"), _T("Confirm") , wxYES_NO | wxCANCEL))
		{
		case wxCANCEL:
			event.Veto();
			break;
		case wxNO:
			// do Nothing, change item without saving.
			break;
		case wxYES:
			SaveItemInfo();
			break;
		default:
			assert(false);
		}
	}
}

void TLMenuCfgDialog::CheckFlg(wxCheckBox* ctrl, const bool val)
{
	assert(ctrl);

	if (ctrl->GetValue() != val)
	{
		ctrl->SetValue(val);
		wxFont font(ctrl->GetFont());
		font.SetWeight(val ? wxBOLD : wxNORMAL);
		ctrl->SetFont(font);
	}
}

void TLMenuCfgDialog::OntxtTargetText(wxCommandEvent& event)
{
	InfoChgFlg(true);

	wxFont font(m_txtTarget->GetFont());

	if (font.GetWeight() != wxBOLD)
	{
		font.SetWeight(wxBOLD);
		m_txtTarget->SetFont(font);
	}

	UpdateFlgs();
}

void TLMenuCfgDialog::OntxtNameOrFilterText(wxCommandEvent& event)
{
	InfoChgFlg(true);

	wxFont font(m_txtNameOrFilter->GetFont());

	if (font.GetWeight() != wxBOLD)
	{
		font.SetWeight(wxBOLD);
		m_txtNameOrFilter->SetFont(font);
	}

	UpdateFlgs();

}

void TLMenuCfgDialog::OntxtIconText(wxCommandEvent& event)
{
	InfoChgFlg(true);

	wxFont font(m_txtIcon->GetFont());

	if (font.GetWeight() != wxBOLD)
	{
		font.SetWeight(wxBOLD);
		m_txtIcon->SetFont(font);
	}

	UpdateFlgs();
}

void TLMenuCfgDialog::UpdateFlgs()
{
	wxTreeItemId item = m_TreeMenu->GetSelection();
	const bool isMenu = item.IsOk() && m_TreeMenu->HasChildren(item);
	const bool isItem = !isMenu;

	CheckFlg(m_flgMenu, isMenu);

	CheckFlg(m_flgWildCard, isItem && m_txtTarget->GetValue().Right(1) == _T("*"));
	CheckFlg(m_flgTitle, isItem && m_txtTarget->IsEmpty() && !m_txtNameOrFilter->IsEmpty());

	CheckFlg(m_flgSep,
	         isItem &&
	         m_txtTarget->IsEmpty() &&
	         m_txtIcon->IsEmpty() &&
	         m_txtNameOrFilter->IsEmpty());

	CheckFlg(m_flgSep,
	         isItem &&
	         m_txtTarget->IsEmpty() &&
	         m_txtIcon->IsEmpty() &&
	         m_txtNameOrFilter->IsEmpty());

	// target editable only for items, Not for submenus.
	m_stcTarget->Enable(isItem);
	m_txtTarget->Enable(isItem);
	m_btnFindTarget->Enable(isItem);

}

void TLMenuCfgDialog::OnbtnReloadClick(wxCommandEvent& event)
{
	if(InfoChgFlg())
	{
		ReadItemInfo();
	}
}

bool TLMenuCfgDialog::ReadItemInfo()
{
	bool ret = false;
	wxTreeItemId item(m_TreeMenu->GetSelection());

	if (item.IsOk())
	{
		if (wxTreeItemData *pData = m_TreeMenu->GetItemData(item))
		{
			assert(dynamic_cast<MenuItemData*>(pData));
			MenuItemData *p = static_cast<MenuItemData*>(pData);
			m_txtNameOrFilter->SetValue(p->Name());
			m_txtTarget->SetValue(p->Target());
			m_txtIcon->SetValue(p->IconPath());
		}
		else
		{
			m_txtNameOrFilter->Clear();
			m_txtTarget->Clear();
			m_txtIcon->Clear();
		}

		InfoChgFlg(false);
		ret = true;
	}

	return ret;
}


bool TLMenuCfgDialog::SaveItemInfo()
{
	bool ret = false;
	wxTreeItemId item = m_TreeMenu->GetSelection();

	if (item.IsOk())
	{
		assert(InfoChgFlg());

		if (wxTreeItemData *pData = m_TreeMenu->GetItemData(item))
		{
			assert(dynamic_cast<MenuItemData *> (pData));
			MenuItemData *p = static_cast<MenuItemData *> (pData);
			p->Name(m_txtNameOrFilter->GetValue());
			p->Target(m_txtTarget->GetValue());
			p->IconPath(m_txtIcon->GetValue());
			UpdateDisplayName(*m_TreeMenu, item);

			InfoChgFlg(false);
			ret = true;
		}
	}

	return ret;
}

void TLMenuCfgDialog::OnbtnUpClick(wxCommandEvent& event)
{
	wxTreeItemId item(m_TreeMenu->GetSelection());

	if (item)
	{
		MoveItem(*m_TreeMenu, item, m_TreeMenu->GetPrevSibling(item));
	}
}


void TLMenuCfgDialog::OnbtnDownClick(wxCommandEvent& event)
{
	wxTreeItemId item(m_TreeMenu->GetSelection());

	if (item)
	{
		MoveItem(*m_TreeMenu, item, m_TreeMenu->GetNextSibling(item), false);
	}
}

void TLMenuCfgDialog::OnbtnDelClick(wxCommandEvent& event)
{
	wxTreeItemId item = m_TreeMenu->GetSelection();

	if (item.IsOk())
	{
		if (!m_TreeMenu->HasChildren(item) ||
		        wxMessageBox(_T("DELETE_MENU"), _T("LNG_Confirm"), wxYES_NO) == wxYES)
		{
			m_TreeMenu->Delete(item);
		}
	}
}
