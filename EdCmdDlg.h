/*
 * EdCmdDlg.h - Editor command setting dialog
 *
 * Last Change: 13:04:15 30-Sep-2000.
 * Written by:  Muraoka Taro  <koron@tka.att.ne.jp>
 *
 * Copy right (C) 2000 KoRoN
 */
#ifndef _INC_AB_EDCMDDLG
#define _INC_AB_EDCMDDLG

#include "ABrowser.h"

class EditorCommandDialog
{
    public:
	EditorCommandDialog();
	virtual ~EditorCommandDialog();
	int doModal(HWND hwndParent);
    public:
	virtual LRESULT onInitDialog(HWND hwndCtrl);
	virtual LRESULT onCommand(WORD wNotifyCode, WORD wID, HWND hwndCtrl);
    private:
	HWND m_hWnd;
    private:
	static LRESULT CALLBACK dlgProc(HWND hDlg, UINT uMsg, WPARAM wParam,
		LPARAM lParam);
	enum { IDD = IDD_EDITORCMD };
};

#endif /* _INC_AB_EDCMDDLG */
/* vi:ts=8 sts=4 sw=4 tw=0
 */
