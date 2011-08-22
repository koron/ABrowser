// vi:set ts=4 sts=4 sw=4 tw=0:
//
// ANSI text Browser                                           since 1997.08.28
//	AB_main.cpp:�G���g���[�R�[�h                               Written by KoRoN
//                                                                 Version 1.10
// Last Change: 30-Oct-2004.

#define		WIN32_LEAN_AND_MEAN
#include "ABrowser.h"
#include <zmouse.h>
#include "CABr.h"
#include"CWhlMous.h"


#define		WM_WHEELROLL	WM_USER + 1
#define		WM_FINDNEXT		WM_USER + 2


char	*szAppName		= APPLICATION_NAME;

char	*szInfVersion	= INFO_VERSION;
char	*szInfMail		= INFO_EMAIL;
char	*szInfURL		= INFO_URL;
char	*szInfStatus	= INFO_STATUS;

HINSTANCE		g_hInstance;
ClassABrowser	*ABro;


// �֐��v���g�^�C�v�錾 ///////////////////////////////////////////////////1.00
LRESULT CALLBACK ClassABrowserProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ClassABrowserViewProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndVerDlgProc(HWND, UINT, WPARAM, LPARAM);

int		InitClass(HINSTANCE hInst);
HWND	InitWindow(HINSTANCE hInst, int iCmdShow);
//////////////////////////////////////////////KoRoN/1997.08.28/KoRoN/1997.09.04


// DlgVersionProc /////////////////////////////////////////////////////////1.00
	LRESULT CALLBACK
WndVerDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
			SetFocus(hDlg);
			SetWindowText(GetDlgItem(hDlg, IDC_STA_TITLE),	szInfVersion);
			SetWindowText(GetDlgItem(hDlg, IDC_EDIT_MAIL),	szInfMail);
			SetWindowText(GetDlgItem(hDlg, IDC_EDIT_URL),	szInfURL);
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
					EndDialog(hDlg, IDOK);
					return TRUE;
			}
			break;
	}
	return FALSE;
}
//////////////////////////////////////////////KoRoN/1997.09.04/KoRoN/1997.09.04


	static int
tab_toggle(ClassABrowser *ab)
{
	int retval = ab->TabChange();
	ab->SetInformation();
	return retval;
}

// ClassABrowserViewProc //////////////////////////////////////////////////1.10
	LRESULT CALLBACK
ClassABrowserViewProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	POINTS	ptsMouse = MAKEPOINTS(lParam);

	switch (uMsg)
	{
		// �T�C�Y�ύX
		case WM_SIZE:
			return ABro->Information();

			// �^�C�}�[���b�Z�[�W
		case WM_TIMER:
			return ABro->Timer();

			// �}�E�X�ړ����̏���
		case WM_MOUSEMOVE:
			if (wParam & MK_LBUTTON)
				return ABro->SetQuery(CAV_SQM_SET, ptsMouse.x, ptsMouse.y);
			else return ABro->MouseMove(ptsMouse.x, ptsMouse.y);

			// �}�E�X���N���b�N�̏���
		case WM_LBUTTONDOWN:
			SetCapture(hWnd);
			return ABro->SetQuery(CAV_SQM_START, ptsMouse.x, ptsMouse.y);
		case WM_LBUTTONUP:
			ReleaseCapture();
			return ABro->SetQuery(CAV_SQM_END, ptsMouse.x, ptsMouse.y);

			// �}�E�X�E�N���b�N���̏���
		case WM_RBUTTONDOWN:
			if (!(wParam & MK_LBUTTON)) ABro->PopupMenu();
			return 0;

			// �����X�N���[��
		case WM_VSCROLL:
			return ABro->Scroll((int)LOWORD(wParam),
					(unsigned short int)HIWORD(wParam));

			// �R�}���h
		case WM_COMMAND:
			{
				switch (LOWORD(wParam))
				{
					// �J���[���]
					case IDC_COLOR_REVERSE:
						return ABro->ChangeColorMode();
						// �S���I��
					case IDC_QUERYALL:
						return ABro->SetQueryAll();
						// �����R�[�h�̋����ύX
					case IDC_FILTER_SJIS:
						return ABro->ChangeFilter(CTF_FILTER_SJIS);
					case IDC_FILTER_EUC:
						return ABro->ChangeFilter(CTF_FILTER_EUC);
					case IDC_FILTER_JIS:
						return ABro->ChangeFilter(CTF_FILTER_JIS);
					case IDC_FILTER_UNICODE:
						return ABro->ChangeFilter(CTF_FILTER_UNICODE);
					case IDC_FILTER_UTF8:
						return ABro->ChangeFilter(CTF_FILTER_UTF8);
					case IDC_FILTER_AUTO:
						return ABro->ChangeFilter(CTF_FILTER_AUTO);
						// �N���b�v�{�[�h�w�R�s�[
					case IDC_TEXT_COPY:
						return ABro->CopyToClipboard();
						// �t�H���g�Z���N�g
					case IDC_FONT_SELECT:
						return ABro->FontSelect();
						// �^�u�`�F���W
					case IDC_TAB_CHANGE:
						return tab_toggle(ABro);
					case IDC_WEB_GOOGLE:
						return ABro->OpenGoogle();
				}
				break;
			}

			// �X�V
		case WM_PAINT:
			ABro->ClassABrowserView::Paint();
			ABro->SetLineNumber();
			return 0;

			// �쐬
		case WM_CREATE:
			SetTimer(hWnd, 1, 80, NULL);
			return 0;

			// �I������
		case WM_DESTROY:
			KillTimer(hWnd, 1);
			PostQuitMessage(0);
			return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
//////////////////////////////////////////////KoRoN/1997.08.28/KoRoN/1997.10.15


// WinProc ////////////////////////////////////////////////////////////////1.00
	LRESULT CALLBACK
ClassABrowserProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static	ClassWheelMouse		cWheel;

	cWheel.Translate(uMsg);
	ABro->ClassFindWindow::Translate(uMsg);

	switch (uMsg)
	{
		// ��������
		case WM_FINDNEXT:
			{
				ABro->ClassFindWindow::Destroy();
				// �}�Y���R�[�h�B�}�W�b�N�i���o�[2�͋t����������\���B
				int direction = ABro->IsFindNext(lParam);
 				if (direction)
					ABro->Find(ABro->ClassFindWindow::Get(), direction == 2);
			}
			return 0;

			// ���j���[�C�j�V�����C�Y
		case WM_INITMENU:
			return ABro->InitMenu();

			// ���j���[�Z���N�g
		case WM_MENUSELECT:
			return ABro->MenuSelect((UINT)LOWORD(wParam));

			// �z�C�[����]
		case WM_WHEELROLL:
		case WM_MOUSEWHEEL:
			{
				long	nDirection = (long) wParam;
				return ABro->Scroll((nDirection > 0)? -3 : 3);
			}

			// �t�@�C���̃h���b�O�ɂ��I�[�v��
		case WM_DROPFILES:
			return ABro->DropFile((HANDLE) wParam);

			// �R�}���h
		case WM_COMMAND:
			{
				switch (LOWORD(wParam))
				{
					case IDC_EDITOR_SELECT:
						ABro->selectEditor();
						break;
					case IDC_EDITOR_KICK:
						ABro->openEditor();
						break;
					// �J���[���]
					case IDC_COLOR_REVERSE:
						return ABro->ChangeColorMode();

						// ����
					case IDC_FIND:
						ABro->Find();
						return 0;
					case IDC_FINDNEW:
						ABro->ClassFindWindow::Create(hWnd, FR_HIDEMATCHCASE
								| FR_HIDEWHOLEWORD | FR_DOWN);
						return 0;
					case IDC_FINDPREV:
						ABro->Find(0, true);
						return 0;
					case IDC_FINDNEWPREV:
						ABro->ClassFindWindow::Create(hWnd, FR_HIDEMATCHCASE
								| FR_HIDEWHOLEWORD);
						return 0;

						// ���j���[�̕\���E��\���ؑ�
					case IDC_SHOW_MENU:
						return ABro->ShowMenu();
						// �X�e�[�^�X�o�[�̕\���E��\���ؑ�
					case IDC_SHOW_STATUS:
						return ABro->ShowStatus();
						// �ړ��R�}���h
					case IDC_GO_HEAD:
						return ABro->JumpTo(0);
					case IDC_GO_TAIL:
						return ABro->JumpTo(-1);
						// �S���I��
					case IDC_QUERYALL:
						return ABro->SetQueryAll();
						// �����R�[�h�̋����ύX
					case IDC_FILTER_SJIS:
						return ABro->ChangeFilter(CTF_FILTER_SJIS);
					case IDC_FILTER_EUC:
						return ABro->ChangeFilter(CTF_FILTER_EUC);
					case IDC_FILTER_JIS:
						return ABro->ChangeFilter(CTF_FILTER_JIS);
					case IDC_FILTER_UNICODE:
						return ABro->ChangeFilter(CTF_FILTER_UNICODE);
					case IDC_FILTER_UTF8:
						return ABro->ChangeFilter(CTF_FILTER_UTF8);
					case IDC_FILTER_AUTO:
						return ABro->ChangeFilter(CTF_FILTER_AUTO);
						// �N���b�v�{�[�h�w�R�s�[
					case IDC_TEXT_COPY:
						return ABro->CopyToClipboard();
					case IDC_WEB_GOOGLE:
						return ABro->OpenGoogle();
						// �E�B���h�E����������
					case IDC_WIN_HEIGHT:
						return ABro->AutoHeight();
						// �E�B���h�E����������
					case IDC_WIN_WIDTH:
						return ABro->AutoWidth();
						// �o�[�W�������\��
					case IDC_VERSION:
						MessageBeep(MB_ICONASTERISK);
						DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_VERSION),
								hWnd, (DLGPROC)WndVerDlgProc);
						return 0;
						// �X�N���[���֌W
					case IDC_LINEDOWN:
						return ABro->Scroll(SB_LINEDOWN	, 0);
					case IDC_LINEUP:
						return ABro->Scroll(SB_LINEUP		, 0);
					case IDC_PAGEDOWN:
						return ABro->Scroll(SB_PAGEDOWN	, 0);
					case IDC_PAGEUP:
						return ABro->Scroll(SB_PAGEUP		, 0);
						// �t�H���g�Z���N�g
					case IDC_FONT_SELECT:
						return ABro->FontSelect();
						// �^�u�`�F���W
					case IDC_TAB_CHANGE:
						return tab_toggle(ABro);
						// �t�@�C���I�[�v��
					case IDC_FILE_OPEN:
						return ABro->OpenFile();
						// �I��
					case IDC_EXIT:
						DestroyWindow(hWnd);
						return 0;
				}
				break;
			}

			// �X�V
		case WM_PAINT:
			return ABro->Paint();

			// �T�C�Y
		case WM_SIZE:
			return ABro->Size();

			// �쐬
		case WM_CREATE:
			cWheel.Registe(WM_WHEELROLL);
			ABro->ClassFindWindow::Registe(WM_FINDNEXT);
			ABro->Create(hWnd);
			return 0;

			// �I������
		case WM_DESTROY:
			return ABro->Destroy();
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
//////////////////////////////////////////////KoRoN/1997.08.28/KoRoN/1997.09.25


// WinMain ////////////////////////////////////////////////////////////////1.00
	int WINAPI
WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pszCmdLine, int iCmdShow)
{
	MSG				msg;
	HWND			hWnd;
	HACCEL			hAccel;
	ClassABrowser	ABrowser(szAppName);
	ABro =			&ABrowser;

	g_hInstance	= hInst;
	if (!(hWnd = ABro->Init(hInst)))
		return FALSE;

	hAccel = LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ACCEL_MAIN));

	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		if (ABro->ClassFindWindow::Message(&msg))
			continue;
		if (TranslateAccelerator(hWnd, hAccel, &msg))
			continue;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}
//////////////////////////////////////////////KoRoN/1997.08.28/KoRoN/2000.09.24


//  ///////////////////////////////////////////////////////////////////////1.00
//////////////////////////////////////////////KoRoN/1997.09.25/KoRoN/1997.09.25
