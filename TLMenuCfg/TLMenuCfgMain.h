/***************************************************************
 * Name:      TLMenuCfgMain.h
 * Purpose:   Defines Application Frame
 * Author:     (lichao)
 * Created:   2010-10-31
 * Copyright:  (lichao)
 * License:	GPL 3.0
 **************************************************************/

#ifndef TLMENUCFGMAIN_H
#define TLMENUCFGMAIN_H

//(*Headers(TLMenuCfgDialog)
#include <wx/srchctrl.h>
#include <wx/treectrl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

#include "MenuData.h"

class TLMenuCfgDialog: public wxDialog
{
    public:

        TLMenuCfgDialog(wxWindow* parent,wxWindowID id = -1);
        virtual ~TLMenuCfgDialog();

    private:

        //(*Handlers(TLMenuCfgDialog)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnbtnUpClick(wxCommandEvent& event);
        void OnInit(wxInitDialogEvent& event);
        void OnTreeMenuSelectionChanged(wxTreeEvent& event);
        void OnbtnSaveClick(wxCommandEvent& event);
        void OnTreeMenuSelChanging(wxTreeEvent& event);
        void OntxtTargetText(wxCommandEvent& event);
        void OntxtNameOrFilterText(wxCommandEvent& event);
        void OntxtIconText(wxCommandEvent& event);
        void OnbtnReloadClick(wxCommandEvent& event);
        void OnbtnDownClick(wxCommandEvent& event);
        void OnbtnDelClick(wxCommandEvent& event);
        void OnbtnNewDirClick(wxCommandEvent& event);
        void OnbtnNewItemClick(wxCommandEvent& event);
        //*)

        //(*Identifiers(TLMenuCfgDialog)
        static const long ID_STATICTEXT2;
        static const long ID_SEARCHCTRL1;
        static const long ID_TREECTRL_MENU;
        static const long ID_BITMAPBUTTON1;
        static const long ID_BITMAPBUTTON3;
        static const long ID_BITMAPBUTTON4;
        static const long ID_BITMAPBUTTON5;
        static const long ID_BITMAPBUTTON2;
        static const long ID_CHECKBOX4;
        static const long ID_CHECKBOX5;
        static const long ID_STATICTEXT1;
        static const long ID_CHECKBOX1;
        static const long ID_TEXTCTRL1;
        static const long ID_BITMAPBUTTON6;
        static const long ID_STATICTEXT3;
        static const long ID_CHECKBOX2;
        static const long ID_TEXTCTRL2;
        static const long ID_STATICTEXT4;
        static const long ID_TEXTCTRL3;
        static const long ID_BITMAPBUTTON7;
        static const long ID_BUTTON3;
        static const long ID_BUTTON4;
        static const long ID_BUTTON2;
        static const long ID_BUTTON7;
        static const long ID_BUTTON6;
        //*)

        //(*Declarations(TLMenuCfgDialog)
        wxBitmapButton* m_btnFindTarget;
        wxBitmapButton* m_btnDown;
        wxBitmapButton* m_btnNewDir;
        wxStaticText* StaticText2;
        wxBitmapButton* m_btnDel;
        wxSearchCtrl* m_search;
        wxButton* m_btnSave;
        wxBitmapButton* BitmapButton2;
        wxBitmapButton* m_btnUp;
        wxStaticText* StaticText3;
        wxBitmapButton* m_btnNewItem;
        wxTreeCtrl* m_TreeMenu;
        wxTextCtrl* m_txtTarget;
        wxButton* m_btnOK;
        wxCheckBox* m_flgSep;
        wxButton* m_btnCancel;
        wxCheckBox* m_flgTitle;
        wxBoxSizer* BoxSizer1;
        wxCheckBox* m_flgWildCard;
        wxStaticText* m_stcTarget;
        wxTextCtrl* m_txtIcon;
        wxStaticText* StaticText4;
        wxButton* m_btnApply;
        wxButton* m_btnReload;
        wxTextCtrl* m_txtNameOrFilter;
        wxCheckBox* m_flgMenu;
        //*)

        DECLARE_EVENT_TABLE()

		bool InfoChgFlg() const { return m_bInfoUnsaved; }
		void InfoChgFlg(const bool val);
		bool ReadItemInfo();
		bool SaveItemInfo();
		void CheckFlg(wxCheckBox* ctrl, const bool val);
		void UpdateFlgs();

		bool m_bInfoUnsaved;	//!< if there is unsaved change
		CMenuData m_menuData;
		wxImageList m_iconlist;
};

#endif // TLMENUCFGMAIN_H
