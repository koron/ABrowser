/* vi:set ts=8 sts=4 sw=4 tw=0: */
/*
 * EdCmdDlg.cpp - Editor command setting dialog
 *
 * Last Change: 13:46:56 11-Mar-2001.
 * Written by:  Muraoka Taro  <koron@tka.att.ne.jp>
 *
 * Copy right (C) 2000 KoRoN
 */
#define WIN32_LEAN_AND_MEAN
#include "EdCmdDlg.h"
#include "winapiex.h"

EditorCommandDialog::EditorCommandDialog()
{
    m_hWnd = 0;
}

EditorCommandDialog::~EditorCommandDialog()
{
}

    int
EditorCommandDialog::doModal(HWND hwndParent)
{
    return DialogBoxParam(GetModuleHandle(NULL),
	    MAKEINTRESOURCE(IDD_EDITORCMD), hwndParent, (DLGPROC)dlgProc,
	    (LPARAM)this);
}

    LRESULT
EditorCommandDialog::onInitDialog(HWND hwndCtrl)
{
    CRegKey regEditCmd;

    if (regEditCmd.Create(HKEY_CURRENT_USER, REGKEY_BASE) == ERROR_SUCCESS)
    {
	DWORD dwBufSize;
	char *pszFmt;

	for (int loop = 0; loop < 2; ++loop)
	{
	    if (regEditCmd.QueryValue((LPTSTR)0, REGKEY_EDITCMD, &dwBufSize)
		    == ERROR_SUCCESS)
	    {
		char *pszFmt = new char[dwBufSize];
		regEditCmd.QueryValue(pszFmt, REGKEY_EDITCMD, &dwBufSize);
		SetDlgItemText(m_hWnd, IDC_EXECMD_FORMAT, pszFmt);
		delete[] pszFmt;
		break;
	    }
	    else if (!loop)
	    {
		pszFmt = DEFAUTL_REG_EDITCMD;
		regEditCmd.SetValue(pszFmt, REGKEY_EDITCMD);
	    }
	}
	regEditCmd.Close();
    }
    else
	_RPT0(_CRT_ASSERT, "EditorCommandDialog::Cannot create registy\r\n");

    char buf[1024];
    LoadString(GetModuleHandle(NULL), IDS_SPECIAL_CHAR, buf, 1024);
    SetDlgItemText(m_hWnd, IDC_SPECIAL_CHAR, buf);

    SetFocus(m_hWnd);

    return TRUE;
}

    LRESULT
EditorCommandDialog::onCommand(WORD wNotifyCode, WORD wID, HWND hwndCtrl)
{
    switch (wID)
    {
	case IDC_EXE_BROWSE:
	    {
		char filename[MAX_PATH];
		OPENFILENAME ofn;

		Init_OPENFILENAME(&ofn);
		filename[0] = '\0';
		ofn.hwndOwner = m_hWnd;
		ofn.lpstrFilter = "Program to edit {*.exe}\0*.exe\0\0";
		ofn.lpstrFile = filename;
		ofn.lpstrDefExt = "exe";
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
		if (GetOpenFileName(&ofn))
		    SendMessage(GetDlgItem(m_hWnd, IDC_EXECMD_FORMAT),
			    WM_SETTEXT, 0, (LPARAM)filename);
	    }
	    break;
	case IDOK:
	    {
		int len = SendMessage(GetDlgItem(m_hWnd, IDC_EXECMD_FORMAT),
			WM_GETTEXTLENGTH, 0, 0) + 1;
		char *pszFmt = new char[len];

		if (pszFmt)
		{
		    if (!GetDlgItemText(m_hWnd, IDC_EXECMD_FORMAT,
				pszFmt, len))
			_RPT1(_CRT_WARN, "Cannot get format\r\n", len);
		    else
		    {
			_RPT1(_CRT_WARN, "format=%s\r\n", pszFmt);
			CRegKey regEditCmd;
			if (regEditCmd.Create(HKEY_CURRENT_USER, REGKEY_BASE)
				== ERROR_SUCCESS)
			{
			    regEditCmd.SetValue(pszFmt, REGKEY_EDITCMD);
			    regEditCmd.Close();
			}
			else
			    _RPT0(_CRT_ASSERT, "Cannot create registy key\r\n");
		    }
		    delete[] pszFmt;
		}
	    }
	case IDCANCEL:
	    EndDialog(m_hWnd, 0);
	    return TRUE;
    }
    return FALSE;
}

    LRESULT CALLBACK
EditorCommandDialog::dlgProc(HWND hDlg, UINT uMsg, WPARAM wParam,
	LPARAM lParam)
{
    EditorCommandDialog* p_ecd = (EditorCommandDialog*)GetWindowLong(hDlg,
	    GWL_USERDATA);

    if (!p_ecd)
    {
	if (uMsg == WM_INITDIALOG)
	{
	    SetWindowLong(hDlg, GWL_USERDATA, lParam);
	    p_ecd = (EditorCommandDialog*)lParam;
	    p_ecd->m_hWnd = hDlg;
	    return p_ecd->onInitDialog((HWND)wParam);
	}
	return FALSE;
    }

    switch (uMsg)
    {
	case WM_COMMAND:
	    return p_ecd->onCommand(HIWORD(wParam), LOWORD(wParam),
		    (HWND)lParam);
    }
    return FALSE;
}
