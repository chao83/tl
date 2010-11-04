/***************************************************************
 * Name:      TLMenuCfgMain.cpp
 * Purpose:   Code for Application Frame
 * Author:     ()
 * Created:   2010-10-31
 * Copyright:  ()
 * License:
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
	:m_bInfoUnsaved(false)
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
	m_TreeMenu = new wxTreeCtrl(this, ID_TREECTRL_MENU, wxDefaultPosition, wxSize(198,333), wxTR_DEFAULT_STYLE, wxDefaultValidator, _T("ID_TREECTRL_MENU"));
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
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Target"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	BoxSizer10->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_flgWildCard = new wxCheckBox(this, ID_CHECKBOX1, _("IsWildCard (*Mode)"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	m_flgWildCard->SetValue(false);
	m_flgWildCard->Disable();
	BoxSizer10->Add(m_flgWildCard, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer11->Add(BoxSizer10, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer17 = new wxBoxSizer(wxHORIZONTAL);
	m_txtTarget = new wxTextCtrl(this, ID_TEXTCTRL1, _("Text"), wxDefaultPosition, wxSize(456,24), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	BoxSizer17->Add(m_txtTarget, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BitmapButton1 = new wxBitmapButton(this, ID_BITMAPBUTTON6, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FIND")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON6"));
	BitmapButton1->SetDefault();
	BoxSizer17->Add(BitmapButton1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
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

void TLMenuCfgDialog::OnbtnUpClick(wxCommandEvent& event)
{
}

void TLMenuCfgDialog::OnInit(wxInitDialogEvent& event)
{
	wxTreeItemId idRoot = m_TreeMenu->AddRoot(_T("Root"));

	for (int i = 0; i < 10; ++i)
	{
		wxTreeItemId item = m_TreeMenu->AppendItem(idRoot, wxString::Format(_T("Item %d"), i+1));
		m_TreeMenu->SetItemData(item, new MenuItemData(	wxString::Format(_T("Name %d"), i+1),
		                        wxString::Format(_T("Target %d"), i+1),

		                        wxString::Format(_T("Icon %d"), i+1)
		                                              ));

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
	if (InfoChgFlg())
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

void TLMenuCfgDialog::OntxtTargetText(wxCommandEvent& event)
{
	InfoChgFlg(true);
}

void TLMenuCfgDialog::OntxtNameOrFilterText(wxCommandEvent& event)
{
	InfoChgFlg(true);
}

void TLMenuCfgDialog::OntxtIconText(wxCommandEvent& event)
{
	InfoChgFlg(true);
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
	assert(InfoChgFlg());

	bool ret = false;
	wxTreeItemId item = m_TreeMenu->GetSelection();

	if (item.IsOk())
	{
		if (wxTreeItemData *pData = m_TreeMenu->GetItemData(item))
		{
			assert(dynamic_cast<MenuItemData *> (pData));
			MenuItemData *p = static_cast<MenuItemData *> (pData);
			p->Name(m_txtNameOrFilter->GetValue());
			p->Target(m_txtTarget->GetValue());
			p->IconPath(m_txtIcon->GetValue());

			InfoChgFlg(false);
			ret = true;
		}
	}

	return ret;
}
