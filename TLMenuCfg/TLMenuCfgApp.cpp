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

//(*AppHeaders
#include "TLMenuCfgMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(TLMenuCfgApp);

bool TLMenuCfgApp::OnInit()
{
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
