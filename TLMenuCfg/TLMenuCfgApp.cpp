/***************************************************************
 * Name:      TLMenuCfgApp.cpp
 * Purpose:   Code for Application Class
 * Author:     ()
 * Created:   2010-10-31
 * Copyright:  ()
 * License:
 **************************************************************/

#include "wx_pch.h"
#include "TLMenuCfgApp.h"
#include <wx/snglinst.h>

//(*AppHeaders
#include "TLMenuCfgMain.h"
#include <wx/image.h>
//*)


bool IsOnlyInstance()
{
	static wxSingleInstanceChecker checker(::wxGetUserId() + _T("lch_TL_MenuCfg_Single_Instance_checker"));
	return (!checker.IsAnotherRunning());
}


IMPLEMENT_APP(TLMenuCfgApp);

bool TLMenuCfgApp::OnInit()
{
	if (!IsOnlyInstance())
	{
		return false;
	}
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    TLMenuCfgDialog Dlg(0);
    SetTopWindow(&Dlg);
    Dlg.ShowModal();
    wxsOK = false;
    }
    //*)
    return wxsOK;

}
