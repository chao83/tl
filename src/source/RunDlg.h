#ifndef TRAYLAUNCHER_RUNDLG_L
#define TRAYLAUNCHER_RUNDLG_L

icon_ptr & GRunIcon();
HWND & GHdlgRun();

const UINT UM_UPDATEHINT = WM_USER + 2;

BOOL  CALLBACK RunDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

bool ShellSuccess(HINSTANCE hInst);

class CToolTip
{
	CToolTip(HWND hOwner, HWND hTool);
	~CToolTip();

	HWND m_hwndToolTip;
};

//! 设置历史记录
void InitHistory(const std::vector<TSTRING>& vStrHis);

#endif // TRAYLAUNCHER_RUNDLG_L
