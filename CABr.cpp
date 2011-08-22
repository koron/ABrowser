// vi:set ts=4 sts=4 sw=4 tw=0:
//
// ANSIBrowser�N���X                                           since 1997.09.03
//	CABr.cpp:ClassABrowser�R�[�h                               Written by KoRoN
//                                                                  Version 1.1
// Last Change: 30-Oct-2004.

#include "ABrowser.h"
#include "CABr.h"
#include "CReg.h"
#include "str_repl.h"
#include "EdCmdDlg.h"

extern		char*				szInfStatus;

static char *s_szClassName = "ABrowser";


//
// �G�f�B�^�I��
// �_�C�A���O���g�p���āA���[�U�ɃG�f�B�^���w�肳����B
//
	bool
ClassABrowser::selectEditor()
{
	EditorCommandDialog ecd;
	ecd.doModal(hWnd);
	return true;
}

//
// �G�f�B�^�Ńt�@�C�����J��
// ���݃u���E�Y���̃t�@�C�����w�肳��Ă���G�f�B�^��p���ċN������B
//
	bool
ClassABrowser::openEditor()
{
	// There is a file opened?
	if (lstrlen(szFileName) == 0
#if 0
			|| m_bEditOpened
#endif
	  )
	{
		_RPT0(_CRT_WARN, "Cannot open file\n");
		return false;
	}

	// Get editor command format from registry.
	DWORD dwBufSize = 0;
	CRegKey regEditCmd;
	if (regEditCmd.Create(HKEY_CURRENT_USER, REGKEY_BASE) != ERROR_SUCCESS)
	{
		_RPT0(_CRT_WARN, "regEditCmd.Open() failed\n");
		return false;
	}

	// Set default editor command, if there are no registy entry.
	char *pszFmt = DEFAUTL_REG_EDITCMD;
	if (regEditCmd.QueryValue((LPTSTR)0, REGKEY_EDITCMD, &dwBufSize)
			!= ERROR_SUCCESS)
	{
	    regEditCmd.SetValue(pszFmt, REGKEY_EDITCMD);
		dwBufSize = lstrlen(pszFmt) + 1;
	}

	// Copy of editor command from registry.
	pszFmt = new char[dwBufSize];
	if (!pszFmt)
	{
		regEditCmd.Close();
		return false;
	}
	regEditCmd.QueryValue(pszFmt, REGKEY_EDITCMD, &dwBufSize);
	regEditCmd.Close();

	// ���ϐ���W�J����B
	DWORD len = ExpandEnvironmentStrings(pszFmt, 0, 0);
	char *newcmd = len > 0 ? new char[len] : NULL;
	if (newcmd)
	{
		ExpandEnvironmentStrings(pszFmt, newcmd, len);
		_RPT0(_CRT_WARN, "ExpandEnvironmentStrings()\n");
		_RPT1(_CRT_WARN, "    FROM: %s\n", pszFmt);
		_RPT1(_CRT_WARN, "    TO:   %s\n", newcmd);
		delete[] pszFmt; pszFmt = NULL;
		pszFmt = newcmd;
	}

	// Replace "$" on format string with filename lines or etc.
	int nline = nLineView + 1;
	replace_data rep_data[] = {
		{'f', 's', (void*)szFileName},
		{'l', 'd', (void*)&nline},
		{'\0','\0', 0},
	};
	dwBufSize = str_replace(0, 0, pszFmt, rep_data);
	char *pszCmd = new char[dwBufSize];

	if (pszCmd)
	{
		dwBufSize = str_replace(pszCmd, dwBufSize, pszFmt, rep_data);
		_RPT1(_CRT_WARN, "Editor command: %s\n", pszCmd);

		// Execute editor command
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		if (!CreateProcess(NULL, pszCmd, NULL, NULL, FALSE,
				NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
			_RPT1(_CRT_WARN, "CreateProcess() failed: %s\n", pszCmd);
		else
			m_bEditOpened = true;
		delete[] pszCmd;
	}
	delete[] pszFmt;

	return true;
}

// �R���X�g���N�^ /////////////////////////////////////////////////////////1.00
//		�^�C�g���p�̃��������m�ۂ��A�^�C�g����ݒ�B
//		���W�X�g�������[�h���Ă���B
ClassABrowser::ClassABrowser(char* szDefTitle)
: ClassCommandLine(), ClassABrowserView(), ClassStatusWindow()
{
	hWnd		= NULL;
	hPrevWnd	= FindWindowEx(NULL, NULL, s_szClassName, NULL);
	szTitle		= new char[MAX_PATH];
	ClassABrowser::szDefTitle = szDefTitle;
	lstrcpy(szTitle, szDefTitle);

	m_bEditOpened = false;
}
//////////////////////////////////////////////KoRoN/1997.09.03/KoRoN/1997.09.03


// �f�X�g���N�^ ///////////////////////////////////////////////////////////1.00
//		���W�X�g���̃Z�[�u�A�^�C�g���p�̃������̈�̔j��
ClassABrowser::~ClassABrowser()
{
	delete[]		szTitle;
}
//////////////////////////////////////////////KoRoN/1997.09.03/KoRoN/1997.09.03


// RegLoad ////////////////////////////////////////////////////////////////1.00
//		���W�X�g�������[�h���Ă���B
//		���[�h���Ă���̂̓E�B���h�E�̈ʒu�ƃT�C�Y�B
//		���W�X�g�����Ȃ��ꍇ�̓f�t�H���g��ݒ�B
void ClassABrowser::RegLoad()
{
	ClassRegistory	crWindow(REGKEY_WINDOW);

	// ���j���[�E�X�e�[�^�X�o�[�̕\�����
	if (crWindow.GetValue("window", &dwWindow))
		dwWindow = CAB_WINDOW_MENU | CAB_WINDOW_STATUS;
	// �E�B���h�E�ʒu�E�T�C�Y
	if (crWindow.GetValue("Xposition", (DWORD*)&rectWindow.left))
		goto REGLOAD_WINDOW_ERR;
	if (crWindow.GetValue("Yposition", (DWORD*)&rectWindow.top))
		goto REGLOAD_WINDOW_ERR;
	if (crWindow.GetValue("width", (DWORD*)&rectWindow.right))
		goto REGLOAD_WINDOW_ERR;
	if (crWindow.GetValue("hight", (DWORD*)&rectWindow.bottom))
		goto REGLOAD_WINDOW_ERR;

	if (hPrevWnd)
	{
		rectWindow.left		= CW_USEDEFAULT;
		rectWindow.top		= CW_USEDEFAULT;
	}

	return;
REGLOAD_WINDOW_ERR:
	SetRect(&rectWindow, CW_USEDEFAULT, CW_USEDEFAULT,
										CW_USEDEFAULT, CW_USEDEFAULT);
}
//////////////////////////////////////////////KoRoN/1997.09.03/KoRoN/1997.09.03


// RegSave ////////////////////////////////////////////////////////////////1.01
//		���W�X�g�����Z�[�u���Ă���B
//		�Z�[�u����̂̓E�B���h�E�̈ʒu�T�C�Y�B
	void
ClassABrowser::RegSave()
{
	if (hPrevWnd)
		return;

	ClassRegistory	crWindow(REGKEY_WINDOW);
	// ���j���[�E�X�e�[�^�X�o�[�̕\�����
	crWindow.SetValue("window",		(DWORD)dwWindow				);
	// �E�B���h�E�ʒu�E�T�C�Y
	crWindow.SetValue("Xposition",	(DWORD)rectWindow.left		);
	crWindow.SetValue("Yposition",	(DWORD)rectWindow.top		);
	crWindow.SetValue("width",		(DWORD)rectWindow.right		);
	crWindow.SetValue("hight",		(DWORD)rectWindow.bottom	);
}
//////////////////////////////////////////////KoRoN/1997.09.03/KoRoN/2000.09.24


// Create /////////////////////////////////////////////////////////////////1.00
//		WM_CREATE���󂯂ČĂ΂��B
//		��{�N���X�ɂ��r���[�E�B���h�E�ƃX�e�[�^�X�E�B���h�E���쐬���A
//		���������Ă���B����Ɉ������������ꍇ�ɂ́A������t�@�C���Ƃ���
//		���߂��A�I�[�v�����Ă���B
int ClassABrowser::Create(HWND hWnd)
{
	// �X�e�[�^�X�E�B���h�E���쐬
	SetPart(24); // �^�u�J�������\��
	SetPart(48); // ���s�R�[�h�\��
	SetPart(64); // �G���R�[�h�\��
	SetPart(96); // �s�ԍ��\��
	ClassStatusWindow::Init(hWnd);
	MenuSelect();

	// �r���[�E�B���h�E���쐬
	ClassABrowserView::Init(hWnd);

	// ���j���[�E�X�e�[�^�X�E�B���h�E�̕\�����
//	if (!(dwWindow & CAB_WINDOW_MENU)) ShowMenu();
	if (!(dwWindow & CAB_WINDOW_STATUS)) ShowStatus();
	// �J���[���]���[�h�̐ݒ�
	if (dwWindow & CAB_WINDOW_REVERSE)
								ChangeColorMode(CTF_COLORMODE_WHITEBACK);

	return 0;
}
//////////////////////////////////////////////KoRoN/1997.09.03/KoRoN/1997.10.03


// Size ///////////////////////////////////////////////////////////////////1.00
//		WM_SIZE�ɉ����B
//		�������g�̑傫��������View�ƃX�e�[�^�X�o�[�̑傫�����v�Z���A
//		����ɂ��E�B���h�E�̑傫����ς���B
int ClassABrowser::Size()
{
	RECT	rect, rectStatus;
	GetClientRect(hWnd, &rect);
	GetClientRect(ClassStatusWindow::hWnd, &rectStatus);
	rect.left		= 0;
	rect.top		= 0;
	rect.bottom		= rect.bottom -
		((SetStatus() == CSW_STATUS_SHOW) ? rectStatus.bottom : 0);
	MoveWindow(ClassABrowserView::hWnd,
				rect.left, rect.top, rect.right, rect.bottom, TRUE);
	ClassStatusWindow::Size(rect.left, rect.bottom,
											rect.right, rect.bottom);
	return 0;
}
//////////////////////////////////////////////KoRoN/1997.09.03/KoRoN/1997.09.03


// Destroy ////////////////////////////////////////////////////////////////1.00
//		WM_DESTROY�ɉ�������B�E�B���h�E�̈ʒu��ۑ��p�Ɏ擾���Ă����B
int ClassABrowser::Destroy()
{
	if (!IsIconic(hWnd) && !IsZoomed(hWnd))
	{
		// �E�B���h�E�̈ʒu�E�T�C�Y�擾
		GetWindowRect(hWnd, &rectWindow);
		rectWindow.right	= rectWindow.right - rectWindow.left;
		rectWindow.bottom	= rectWindow.bottom - rectWindow.top;
	}

	// ���j���[�E�X�e�[�^�X�o�[�̕\����Ԏ擾
	dwWindow = 
		(GetMenu(hWnd) ? CAB_WINDOW_MENU : 0) |
		((SetStatus() == CSW_STATUS_SHOW) ? CAB_WINDOW_STATUS : 0) |
		(dwColorMode ? CAB_WINDOW_REVERSE : 0);

	// ���W�X�g���̃Z�[�u
	RegSave();

	PostQuitMessage(0);
	return 0;
}
//////////////////////////////////////////////KoRoN/1997.09.03/KoRoN/1997.09.03


// Paint //////////////////////////////////////////////////////////////////1.00
//		WM_PAINT�ɉ����B
//		�A�v���P�[�V�����E�B���h�E�̍X�V�v���ɂ�������B
//		�Ƃ��ɂ��ׂ����͂Ȃ��B
int ClassABrowser::Paint()
{
	// ��ʂ̍X�V
	PAINTSTRUCT		ps;
	BeginPaint(hWnd, &ps);
	EndPaint(hWnd, &ps);
	return 0;
}
//////////////////////////////////////////////KoRoN/1997.09.03/KoRoN/1997.09.03


// OpenFile ///////////////////////////////////////////////////////////////1.00
//		�t�@�C���̃I�[�v���֐�����1
//		�t�@�C���I���_�C�A���O���J���A�t�@�C�������擾�B
//		����Ɋ�Â��āA�I�[�v���֐�2���g�p���ăt�@�C�����I�[�v���B
int ClassABrowser::OpenFile(void)
{
	// �I�[�v���t�@�C���\���̂̐錾�Ə�����
	OPENFILENAME		ofn; {
		memset(&ofn, 0, sizeof(OPENFILENAME));
		ofn.lStructSize	= sizeof(OPENFILENAME);
		ofn.hwndOwner	= hWnd;
		ofn.lpstrFile	= szFileName;
		ofn.nMaxFile	= MAX_PATH;
		ofn.Flags		= OFN_FILEMUSTEXIST;
	}

	// �t�@�C���I���_�C�A���O�̎��s
	if (!GetOpenFileName(&ofn)) return 0;

	return OpenFile(szFileName);
}
//////////////////////////////////////////////KoRoN/1997.09.03/KoRoN/1997.09.03


// OpenFile ///////////////////////////////////////////////////////////////1.00
//		�t�@�C���I�[�v���̊֐�����2
//		�t�@�C�������^����ꂽ������̂��̂��J���A�^�C�g����ݒ肵�Ă���B
//		�t�@�C���I�[�v���̏����̎��̂͊�{�N���XView���Q�ƁB
int ClassABrowser::OpenFile(char* szOpenFile)
{
	m_bEditOpened = false;
	ClassABrowserView::OpenFile(szOpenFile);
	SetInformation();
	return 0;
}
//////////////////////////////////////////////KoRoN/1997.09.03/KoRoN/1997.09.03


// SetInformation /////////////////////////////////////////////////////////1.00
//		�l�X�ȏ����E�B���h�E�ɐݒ肵�Ă���B
//		�^�C�g���Ɋւ��Ă͂���������Ɨǂ��f�U�C�����������̂����c
void ClassABrowser::SetInformation()
{
	// �E�B���h�E�^�C�g���ݒ�
	GetFileTitle(szFileName, szTitle, MAX_PATH);
	lstrcat(szTitle, " ---/ ");
	SetWindowText(hWnd, lstrcat(szTitle, szDefTitle));

	// �����R�[�h���X�e�[�^�X�o�[�ɕ\��
	switch (dwFilter)
	{
		case CTF_FILTER_AUTO:		SetText(-2, "Auto"		, 0);	break;
		case CTF_FILTER_BIN:		SetText(-2, "Binary"	, 0);	break;
		case CTF_FILTER_ANSI:		SetText(-2, "ANSI"		, 0);	break;
		case CTF_FILTER_SJIS:		SetText(-2, "ShiftJIS"	, 0);	break;
		case CTF_FILTER_EUC:		SetText(-2, "euc-jp"	, 0);	break;
		case CTF_FILTER_JIS:		SetText(-2, "JIS"		, 0);	break;
		case CTF_FILTER_UNICODE:	SetText(-2, "Unicode"	, 0);	break;
		case CTF_FILTER_UCS2LE:		SetText(-2, "UCS-2LE"	, 0);	break;
		case CTF_FILTER_UCS2BE:		SetText(-2, "UCS-2BE"	, 0);	break;
		case CTF_FILTER_UTF8:		SetText(-2, "UTF-8"		, 0);	break;
	}

	// ���s�R�[�h���X�e�[�^�X�o�[�ɕ\��
	switch (GetReturnCode())
	{
		case CTF_RETURNCODE_CR:		SetText(-3, "CR", 0);		break;
		case CTF_RETURNCODE_LF:		SetText(-3, "LF", 0);		break;
		case CTF_RETURNCODE_CRLF:	SetText(-3, "CRLF", 0);		break;
		case CTF_RETURNCODE_MIX:	SetText(-3, "MIX", 0);		break;
		default:					SetText(-3, "noneCR", 0);	break;
	}

	// �^�u�J���������X�e�[�^�X�o�[�ɕ\��
	char buf[16];
	sprintf(buf, "%d", ChangeTabColumn());
	SetText(-4, buf, 0);
}
//////////////////////////////////////////////KoRoN/1997.09.03/KoRoN/1997.09.03


// DropFile ///////////////////////////////////////////////////////////////1.01
//		WM_DROPFILES�ɉ����B
//		�h���b�v���ꂽ�t�@�C�����I�[�v�����āA
//		�������g���A�N�e�B�u�ɂ��ăt�H�[�J�X���擾����B
	int
ClassABrowser::DropFile(HANDLE hDrop)
{
	DragQueryFile((HDROP)hDrop, 0, szFileName, MAX_PATH);
	DragFinish((HDROP)hDrop);

	OpenFile(szFileName);
#if 0
	EnableWindow(hWnd, TRUE);
	SetActiveWindow(hWnd);
	SetFocus(hWnd);
#else
	SetForegroundWindow(hWnd);
#endif
	return 0;
}
//////////////////////////////////////////////KoRoN/1997.09.03/KoRoN/2000.09.24


// SetLineNumber //////////////////////////////////////////////////////////1.00
//		���ݕ\�����Ă���s���Ɋ�Â��āA�X�e�[�^�X�o�[�ɕ\���B
//		���݂̂Ƃ���A��{�N���X��View��WM_PAINT��������
//		�Ăяo���Ă��炤�������@���v���t���Ă��Ȃ��B
int ClassABrowser::SetLineNumber()
{
	char szOutBuffer[16];
	wsprintf(szOutBuffer, "%6d/%6d", nLineView + 1, nTextLine);
	return SetText(-1, szOutBuffer);
}
//////////////////////////////////////////////KoRoN/1997.09.05/KoRoN/1997.09.06


/*
 * AutoHeight
 *	�E�B���h�E�̍������A��ʃT�C�Y�ɍ��킹�ă��T�C�Y����B���T�C�Y����66�s�A
 *	55, 44�s�A33�s�A22�s�A11�s����A��ʂ̃T�C�Y���ň�ԑ傫�����̂��I�΂�
 *	��B
 */
	int
ClassABrowser::
AutoHeight()
{
	static int nreq[] = {66, 55, 44, 33, 22, 11};
	int height, lines = 66;
	POINT point[2];
	RECT rect, rectStatus, rectScreen;

	GetWindowRect(hWnd, &rect);
	GetWindowRect(ClassStatusWindow::hWnd, &rectStatus);
	GetClientRect(GetDesktopWindow(), &rectScreen);

	// �E�B���h�E����������ɂ͂ݏo�Ă���ꍇ�͋�������
	if (rect.left < rectScreen.left)
	{
		rect.right += rectScreen.left - rect.left;
		rect.left = rectScreen.left;
	}
	if (rect.top < rectScreen.top)
	{
		rect.right += rectScreen.top - rect.top;
		rect.top = rectScreen.top;
	}
	MoveWindow(hWnd, rect.left, rect.top,
			rect.right - rect.left, rect.bottom - rect.top, TRUE);

	// �f�X�N�g�b�v�̍����𒴂��Ȃ��悤�ɁA�\������s����ݒ�B
	int screen_height = rectScreen.bottom - rectScreen.top;
	int status_height = rectStatus.bottom - rectStatus.top;
	for (int i = 0; ; ++i)
	{
		height = (nreq[i] + 1) * tm.tmHeight + 3
			+ ((SetStatus() == CSW_STATUS_SHOW) ? status_height : 0);
		if (height <= screen_height || nreq[i] <= 11)
			break;
	}

	// ���m�ȃE�B���h�E�T�C�Y�̌v�Z
	GetClientRect(hWnd, &rect);
	point[0].x = rect.left;
	point[0].y = rect.top;
	point[1].x = rect.right;
	point[1].y = rect.bottom;
	MapWindowPoints(hWnd, HWND_DESKTOP, point, 2);
	rect.left	= point[0].x;
	rect.top	= point[0].y;
	rect.right	= point[1].x;
	rect.bottom	= point[0].y + height;
	AdjustWindowRectEx(&rect, GetWindowLong(hWnd, GWL_STYLE),
			GetMenu(hWnd) != NULL, GetWindowLong(hWnd, GWL_EXSTYLE));

	// �E�B���h�E���E�������ɂ͂ݏo�Ă���ꍇ�͋�������
	if (rect.right > rectScreen.right)
	{
		rect.left -= rect.right - rectScreen.right;
		rect.right = rectScreen.right;
	}
	if (rect.bottom > rectScreen.bottom)
	{
		rect.top -= rect.bottom - rectScreen.bottom;
		rect.bottom = rectScreen.bottom;
	}
	
	MoveWindow(hWnd, rect.left, rect.top,
			rect.right - rect.left, rect.bottom - rect.top, TRUE);
	return 0;
}

// AutoWidth //////////////////////////////////////////////////////////////1.00
//		�E�B���h�E�̕����A�e�L�X�g�̍ő咷�ɂ��킹�ă��T�C�Y����
int ClassABrowser::AutoWidth()
{
	int		nRequestWidth =	((nMaxColumn > 0 ? nMaxColumn : 80) + 6)
		* tm.tmAveCharWidth;
	int		border = GetSystemMetrics(SM_CXSIZEFRAME) * 2;
	int		scroll = GetSystemMetrics(SM_CXVSCROLL);
	nRequestWidth += border * 2 + scroll;

	RECT rect, rectScreen;
	GetWindowRect(hWnd, &rect);
	GetClientRect(GetDesktopWindow(), &rectScreen);
	// �E�B���h�E����������ɂ͂ݏo�Ă���ꍇ�͋�������
	if (rect.left < rectScreen.left)
	{
		rect.right += rectScreen.left - rect.left;
		rect.left = rectScreen.left;
	}
	if (rect.top < rectScreen.top)
	{
		rect.right += rectScreen.top - rect.top;
		rect.top = rectScreen.top;
	}

	rect.right = rect.left + nRequestWidth;

	// �E�B���h�E���E�������ɂ͂ݏo�Ă���ꍇ�͋�������
	if (rect.right > rectScreen.right)
	{
		rect.left -= rect.right - rectScreen.right;
		rect.right = rectScreen.right;
	}
	if (rect.bottom > rectScreen.bottom)
	{
		rect.top -= rect.bottom - rectScreen.bottom;
		rect.bottom = rectScreen.bottom;
	}

	MoveWindow(hWnd, rect.left, rect.top,
			rect.right - rect.left, rect.bottom - rect.top, TRUE);

	return 0;
}
//////////////////////////////////////////////KoRoN/1997.09.05/KoRoN/1997.09.05


// MenuSelect /////////////////////////////////////////////////////////////1.00
//		WM_MENUSELECT�ɉ����B
//		�I�΂ꂽ�R�}���h�ɉ����āA�X�e�[�^�X�o�[�ɃR�����g��\������B
UINT uItems[] = {
	IDC_FILE_OPEN,
	IDC_FONT_SELECT,
	IDC_EXIT,
	IDC_TAB_CHANGE,
	IDC_VERSION,
	IDC_WIN_WIDTH,
	IDC_WIN_HEIGHT,
	IDC_QUERYALL,
	IDC_TEXT_COPY,
	IDC_MANUAL,
	IDC_FILTER_SJIS,
	IDC_FILTER_EUC,
	IDC_FILTER_JIS,
	IDC_FILTER_UNICODE,
	IDC_FILTER_UTF8,
	IDC_FILTER_AUTO,
	IDC_FINDNEWPREV,
	IDC_FINDNEW,
	IDC_FINDPREV,
	IDC_FIND,
	IDC_SHOW_STATUS,
	IDC_SHOW_MENU,
	IDC_COLOR_REVERSE,
	IDC_EDITOR_SELECT,
	IDC_EDITOR_KICK,
	IDC_WEB_GOOGLE,
	0
};

int ClassABrowser::MenuSelect(UINT uItem)
{
	UINT	*pItems = uItems;
	char	sz[256];

	while (*pItems != 0 && *pItems != uItem)
		pItems++;
	if (*pItems)
		LoadString(g_hInstance, *pItems, sz, 256 -1);
	else
		lstrcpy(sz, szInfStatus);
	SetText(0, sz, SBT_NOBORDERS, TRUE);

	return 0;
}
//////////////////////////////////////////////KoRoN/1997.09.25/KoRoN/1997.10.17


// ChangeFilter ///////////////////////////////////////////////////////////1.00
//		�e�L�X�g�̊����R�[�h�������I�ɐ؂�ւ���
//		��{�N���XClassABrowserView�̊֐���ǂ񂾌�A
//		�X�e�[�^�X�o�[�̕\����ς��Ă���B
int ClassABrowser::ChangeFilter(DWORD dwFilter)
{
	ClassABrowserView::ChangeFilter(dwFilter);
	SetInformation();
	return 0;
}
//////////////////////////////////////////////KoRoN/1997.10.03/KoRoN/1997.10.03


// InitMenu ///////////////////////////////////////////////////////////////1.00
//	WM_INITMENU�ɉ�������B
//		���j���[�����݂̏�Ԃɂ��킹�ď��������郁�\�b�h�B
int ClassABrowser::InitMenu()
{
	// �X�e�[�^�X�o�[�̃`�F�b�N
	MenuStatus(IDC_SHOW_STATUS,
		((SetStatus() == CSW_STATUS_SHOW) ? MFS_CHECKED : 0));

	// �J���[���]�̃`�F�b�N
	MenuStatus(IDC_COLOR_REVERSE, (dwColorMode ? MFS_CHECKED : 0));

	// �����󋵂�ύX
	MenuStatus(IDC_FILTER_SJIS,
		((dwFilter == CTF_FILTER_SJIS) ? MFS_CHECKED : 0));
	MenuStatus(IDC_FILTER_EUC,
		((dwFilter == CTF_FILTER_EUC) ? MFS_CHECKED : 0));
	MenuStatus(IDC_FILTER_JIS,
		((dwFilter == CTF_FILTER_JIS) ? MFS_CHECKED : 0));
	MenuStatus(IDC_FILTER_UNICODE,
			(dwFilter == CTF_FILTER_UNICODE || dwFilter == CTF_FILTER_UCS2LE
			 || dwFilter== CTF_FILTER_UCS2BE) ? MFS_CHECKED : 0);
	MenuStatus(IDC_FILTER_UTF8,
		((dwFilter == CTF_FILTER_UTF8) ? MFS_CHECKED : 0));

	// �R�s�[�󋵂̕ύX
	UINT textSelected = dwQuerySize > 0 ? 0 : MFS_GRAYED;
	MenuStatus(IDC_TEXT_COPY, textSelected);
	MenuStatus(IDC_WEB_GOOGLE, textSelected);

	// �t�@�C�������邩�Ȃ����ɂ���ĕς�郁�j���[����
	UINT uFlag = dwFileSize ? 0 : MF_GRAYED;
	MenuStatus(IDC_QUERYALL, uFlag);
	MenuStatus(IDC_GO_TAIL, uFlag);
	MenuStatus(IDC_GO_HEAD, uFlag);
	MenuStatus(IDC_FIND, uFlag);
	MenuStatus(IDC_FINDNEW, uFlag);
	MenuStatus(IDC_FINDPREV, uFlag);
	MenuStatus(IDC_FINDNEWPREV, uFlag);

	return 0;
}
//////////////////////////////////////////////KoRoN/1997.10.10/KoRoN/1997.10.10


// MenuStatus /////////////////////////////////////////////////////////////1.00
void ClassABrowser::MenuStatus(UINT uItem, UINT uFlag)
{
	char	sz[64];

	GetMenuString(hMenu, uItem, sz, 64, MF_BYCOMMAND);
	ModifyMenu(hMenu, uItem, MF_BYCOMMAND | uFlag, uItem, sz);
}
//////////////////////////////////////////////KoRoN/1997.10.10/KoRoN/1997.10.10


// ShowStatus /////////////////////////////////////////////////////////////1.00
int ClassABrowser::ShowStatus()
{
	if (SetStatus() == CSW_STATUS_SHOW) SetStatus(CSW_STATUS_HIDE);
	else SetStatus(CSW_STATUS_SHOW);

	return Size();
}
//////////////////////////////////////////////KoRoN/1997.10.10/KoRoN/1997.10.10


// ShowMenu ///////////////////////////////////////////////////////////////1.00
int ClassABrowser::ShowMenu()
{
	if (GetMenu(hWnd)) SetMenu(hWnd, NULL);
	else SetMenu(hWnd, hMenu);

	return 0;
}
//////////////////////////////////////////////KoRoN/1997.10.10/KoRoN/1997.10.10


// Find ///////////////////////////////////////////////////////////////////1.00
int ClassABrowser::Find(char *psz, bool reverse)
{
	if (ClassABrowserView::Find(psz, reverse))
	{
		char sz[CAV_FINDTEXT_MAX + 12];
		lstrcpy(sz, "����������:");
		lstrcat(sz, szFindText);
		SetText(0, sz, SBT_NOBORDERS);
	}
	return 0;
}
//////////////////////////////////////////////KoRoN/1997.10.12/KoRoN/1997.10.12


// Init ///////////////////////////////////////////////////////////////////1.01
//	�A�v���P�[�V�����̏��������ɌĂ΂�āA
//	�N���X�E�E�B���h�E���쐬�����肷��B
extern	char*	szAppName;
extern	LRESULT	CALLBACK ClassABrowserProc(HWND, UINT, WPARAM, LPARAM);

	HWND
ClassABrowser::Init(HINSTANCE hInst)
{
	ClassABrowser::hInst = hInst;

	// �E�B���h�E�N���X�̓o�^
	WNDCLASS	wc;
	{
		wc.style			= NULL;
		wc.lpfnWndProc		= ClassABrowserProc;
		wc.cbClsExtra		= NULL;
		wc.cbWndExtra		= NULL;
		wc.hInstance		= hInst;
		wc.hIcon			= LoadIcon(hInst, MAKEINTRESOURCE(IDI_ABROWSER));
		wc.hCursor			= LoadCursor(0, IDC_ARROW);
		wc.hbrBackground	= NULL;
		wc.lpszMenuName		= NULL;
		wc.lpszClassName	= s_szClassName;
	}
	if (!RegisterClass(&wc))
		return 0;

	// ���W�X�g���̃��[�h
	RegLoad();

	// ���C���E�B���h�E�̍쐬
	hMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU_MAIN));
	if (!(hWnd = CreateWindowEx(
					WS_EX_ACCEPTFILES,
					s_szClassName,
					szAppName,
					WS_OVERLAPPEDWINDOW,
					rectWindow.left,
					rectWindow.top,
					rectWindow.right,
					rectWindow.bottom,
					NULL,
					(dwWindow & CAB_WINDOW_MENU) ? hMenu : NULL,
					hInst, NULL)
		))
		return 0;
	ShowWindow(hWnd, SW_SHOWNORMAL);
	UpdateWindow(hWnd);

	if (Argv(1))
		OpenFile(Argv(1));

	return hWnd;
}
//////////////////////////////////////////////KoRoN/1997.10.15/KoRoN/1997.10.15


//  ///////////////////////////////////////////////////////////////////////1.00
//////////////////////////////////////////////KoRoN/1997.10.15/KoRoN/1997.10.15
