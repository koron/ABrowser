// vi:set ts=4 sts=4 sw=4 tw=0:
//
// ANSI text Browser                                           since 1997.08.28
//	CABrView.cpp:ClassABrowserView�N���X�R�[�h                 Written by KoRoN
//                                                                 Version 1.10
// Last Change: 22-Aug-2011.

#include "ABrowser.h"
#include "CABrView.h"


extern		HINSTANCE			hInstance;
extern		LRESULT CALLBACK	ClassABrowserViewProc
										(HWND, UINT, WPARAM, LPARAM);


// �R���X�g���N�^ /////////////////////////////////////////////////////////1.00
//		�N�G���[�p�̃J���[�ݒ�A�t�H���g�̗p�ӁB
//		���W�X�g���̃��[�h�B�e�����o�̏��������s�Ȃ��Ă���B
ClassABrowserView::ClassABrowserView()
: ClassTextFile(), ClassTextQuery(), ClassFindWindow(CAV_FINDTEXT_MAX)
{
	// �N�G���[�p�̃J���[������
	cdQuery.crBackground = GetSysColor(COLOR_HIGHLIGHT);
	cdQuery.crForeground = GetSysColor(COLOR_HIGHLIGHTTEXT);
	dwQuerySize = 0;
	fQueryOn = 0;

	// LOGFONT�\���̂̐錾�Ə�����
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight				= -19;
	lf.lfCharSet			= SHIFTJIS_CHARSET;
	lf.lfPitchAndFamily		= FIXED_PITCH | FF_MODERN;
	lstrcpy(lf.lfFaceName, "Terminal");

	// �\���ʒu�̐ݒ�
	nLineView		= 0;
	nLineViewMax	= 0;
	nLineViewPage	= 0;

	RegLoad();
	hFont		= NULL;
	hWnd		= NULL;
	hWndParent	= NULL;

	// �����p�p�����[�^�����ݒ�
	nFindColumn		= 0;
	nFindLine		= 0;
	lstrcpy(szFindText, "");
}
//////////////////////////////////////////////KoRoN/1997.08.28/KoRoN/1997.08.31


// �f�X�g���N�^ ///////////////////////////////////////////////////////////1.00
//		���ɂ�邱�Ƃ��Ȃ��̂ŁA���W�X�g���̃Z�[�u�B
ClassABrowserView::~ClassABrowserView()
{
	RegSave();
}
//////////////////////////////////////////////KoRoN/1997.08.29/KoRoN/1997.08.31


// LineOutSub /////////////////////////////////////////////////////////////1.00
//		�w�肳�ꂽ�J���[�Ńe�L�X�g��\������B
//		�T�u���[�`���Ƃ��Ă��Ȃ�L���Ȃ͂��B
void ClassABrowserView::LineOutSub(HDC hDC, long x, long y,
						char* pstr, long nLength, CTF_COLORDATA* pcd)
{
	if (!nLength) return;
	SetBkColor(hDC, pcd->crBackground);
	SetTextColor(hDC, pcd->crForeground);
	TextOut(hDC, x, y, pstr, nLength);

}
//////////////////////////////////////////////KoRoN/1997.09.23/KoRoN/1997.09.24


// LineOut ////////////////////////////////////////////////////////////////2.00
//		�w�肳�ꂽ��s���A�w�肳�ꂽDC�́A�w�肳�ꂽ�ʒu�ɕ\������B
//		�N�G���[�󋵁A�J���[��Ԃ�������ƕ\������D����́B
//		������������R�[�h��ς���������Ɨǂ��Ȃ邩��???�B
void ClassABrowserView::LineOut(HDC hDC, long x, long y,
								long line, int fRepaint)
{
	if (fRepaint)
		Rectangle(hDC, 0, y, rectWindow.right, y + tm.tmHeight);

	if (line < 0 || nTextLine <= line) return;

	GenerateLine(line);
	char*			psz = (char*) cmOutline->GetPointer();
	CTF_COLORDATA*	pcd = (CTF_COLORDATA*) cmOutlineColor->GetPointer();

	int				fEnd			= 0;
	int				fKanji			= 0;
	int				fQuery			= 0;
	int				fQueryChange	= 0;
	int				fColorChange	= 0;
	long			nColumn			= 0;
	long			nLength			= 0;
	long			nLengthColor	= 0;
	char			ch;
	char*			pstr			= psz;

	// �N�G���[�󋵏�����
	fQuery = IsInQuery(line, nColumn);

	while (!fEnd)
	{
		ch = *psz++;
		nColumn++;
		if (ch == '\0') fEnd = -1;
		else{
			nLength++;
			nLengthColor++;
			if (!fKanji && IsDBCSLeadByte(ch))
			{
				fKanji = 1;
				continue;
			}else fKanji = 0;
		}

		// �J���[�R�[�h�X�V�`�F�b�N
		if (nLengthColor && (nLengthColor >= pcd->nLength)) fColorChange = -1;

		// �N�G���[�X�V�`�F�b�N
		if ((!fQuery && IsInQuery(line, nColumn)) ||
			(fQuery && !IsInQuery(line, nColumn))) fQueryChange = -1;

		// �\�����[�`��
		if (fQueryChange || (!fQuery && fColorChange) || fEnd)
		{
			SIZE	size;
			LineOutSub(hDC, x, y, pstr, nLength, fQuery ? &cdQuery : pcd);
			GetTextExtentPoint32(hDC, pstr, nLength, &size);
			x += size.cx;
			pstr += nLength;
			nLength = 0;
		}

		// �J���[�R�[�h�X�V
		if (fColorChange)
		{
			fColorChange = 0;
			nLengthColor = 0;
			pcd++;
		}

		// �N�G���[�X�V
		if (fQueryChange)
		{
			fQueryChange = 0;
			fQuery = !fQuery;
		}
	}

	if (!pcd->nLength)
		LineOutSub(hDC, x, y, " ", 1, fQuery ? &cdQuery : pcd);

	return;
}
//////////////////////////////////////////////KoRoN/1997.08.29/KoRoN/1997.09.24


// Paint //////////////////////////////////////////////////////////////////1.00
//		WM_PAINT�ɉ����B
//		���������ꂽ�N���C�A���g�̈���擾���A
//		�ĕ`�悵�Ă���B
int ClassABrowserView::Paint()
{
	PAINTSTRUCT		ps;
	HDC				hDC = BeginPaint(hWnd, &ps);
										// DC�擾
	// �\���ʒu�̕␳
	if (nLineView < 0) nLineView = 0;
	else if (nLineView > nLineViewMax) nLineView = nLineViewMax;

	// �X�N���[���o�[�̏��ݒ�
	si.fMask	= SIF_DISABLENOSCROLL | SIF_POS;
	si.nPos		= (int)(nLineView / dblScroll);
	SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
	// ��ʂ̒[�ł̓X�N���[���o�[���g�p�s�ɂ���
	EnableScrollBar(hWnd, SB_VERT, ESB_ENABLE_BOTH);
	if (nLineView == 0)
		EnableScrollBar(hWnd, SB_VERT, ESB_DISABLE_UP);
	if (nLineView == nLineViewMax)
		EnableScrollBar(hWnd, SB_VERT, ESB_DISABLE_DOWN);

	PaintSub(hDC, ps.rcPaint.top / tm.tmHeight,
							ps.rcPaint.bottom / tm.tmHeight + 1, TRUE);

	EndPaint(hWnd, &ps);

	return 0;
}
//////////////////////////////////////////////KoRoN/1997.08.29/KoRoN/1997.08.31


// PaintSub ///////////////////////////////////////////////////////////////1.00
int ClassABrowserView::PaintSub(HDC hDC, long i, long to, int fRepaint)
{
	int		x			= CAV_LEFT_TEXTMARGIN * tm.tmAveCharWidth;
	int		line		= i + nLineView;
	HFONT	hOldFont	= (HFONT)SelectObject(hDC, hFont);
	HPEN	hOldPen		= (HPEN)SelectObject(hDC,
		GetStockObject(dwColorMode ? WHITE_PEN : BLACK_PEN));
	HBRUSH	hOldBrush	= (HBRUSH)SelectObject(hDC,
		GetStockObject(dwColorMode ? WHITE_BRUSH : BLACK_BRUSH));

	for (; i <= to; i++, line++)
		LineOut(hDC, x, i * tm.tmHeight, line, fRepaint);

	SelectObject(hDC, hOldFont);
	SelectObject(hDC, hOldPen);
	SelectObject(hDC, hOldBrush);

	return 0;
}
//////////////////////////////////////////////KoRoN/1997.10.17/KoRoN/1997.10.17


// Paint(�N�G���[�����p�̓���֐�) ////////////////////////////////////////1.00
//		Paint��ʍX�V�p�̃v���O����2�B
//		�Ƃɂ��������I�ɕ\����ς��������Ɏg���B
int ClassABrowserView::Paint(long nStartLine, long nEndLine)
{
	long	nTemp = nStartLine;

	// �J�n�_�ƏI���_�̔��f
	if (nStartLine > nEndLine)
	{
		nStartLine	= nEndLine;
		nEndLine	= nTemp;
		nTemp		= nStartLine;
	}

	// �\���s�ɂ��N���b�s���O
	long	nLineViewEnd = nLineView + nLineViewPage + 1;
	if (nEndLine < nLineView || nStartLine > nLineViewEnd) return 0;
	if (nTemp < nLineView) nTemp = nLineView;
	if (nEndLine > nLineViewEnd) nEndLine = nLineViewEnd;

	HDC		hDC = GetDC(hWnd);

	PaintSub(hDC, nTemp - nLineView, nEndLine - nLineView, FALSE);

	ReleaseDC(hWnd, hDC);

	return 0;
}
//////////////////////////////////////////////KoRoN/1997.09.24/KoRoN/1997.09.24


// ClientToColumn /////////////////////////////////////////////////////////1.00
//		�N���C�A���g���Wx��񐔂ɂȂ����Ă���B
long ClassABrowserView::ClientToColumn(long x)
{
	long nColumn = x / tm.tmAveCharWidth - CAV_LEFT_TEXTMARGIN;
	if (nColumn < 0) nColumn = 0;
	return nColumn;
}
//////////////////////////////////////////////KoRoN/1997.09.24/KoRoN/1997.09.24


// ClientToLine ///////////////////////////////////////////////////////////1.00
//		�N���C�A���g���Wy���΍s���ɂȂ����Ă���B
long ClassABrowserView::ClientToLine(long y)
{
	long nLine = y / tm.tmHeight;
	if (nLine < 0) nLine = 0;
	else if (nLine > nLineViewPage) nLine = nLineViewPage;
	return nLine + nLineView;
}
//////////////////////////////////////////////KoRoN/1997.09.24/KoRoN/1997.09.24


// FontSelect /////////////////////////////////////////////////////////////1.00
//		�t�H���ƃZ���N�g�_�C�A���O���g�p���āA
//		�t�H���g��I�������Ă���B
//		�t�H���g���I�����ꂽ��A�ĕ`��B
int ClassABrowserView::FontSelect()
{
	// CHOOSEFONT�\���̂̐錾�Ə�����
	CHOOSEFONT			cf; {
		memset(&cf, 0, sizeof(CHOOSEFONT));
		cf.lStructSize	= sizeof(CHOOSEFONT);
		cf.hwndOwner	= hWnd;
		cf.lpLogFont	= &lf;
		cf.Flags		= CF_FIXEDPITCHONLY | CF_SCREENFONTS |
			CF_TTONLY | CF_INITTOLOGFONTSTRUCT;
	}

	if (ChooseFont(&cf))
	{
		// �E�B���h�E���̍Đ���
		Information();
		// �E�B���h�E�̍ĕ`��
		InvalidateRect(hWnd, NULL, FALSE);
	}

	return 0;
}
//////////////////////////////////////////////KoRoN/1997.08.29/KoRoN/1997.08.29


// RegLoad ////////////////////////////////////////////////////////////////2.01
//		���W�X�g���̃��[�h
//		�t�H���g�ƃt�H���g�T�C�Y���Ăяo���B
	void
ClassABrowserView::RegLoad()
{
	// �t�H���g
	ClassRegistory	crFont("Software\\KoRoN\\ABrowser\\Font");
	DWORD	dwFontHeight;
	char	szName[ LF_FACESIZE ];
	if (!crFont.GetValue("size", &dwFontHeight))
		lf.lfHeight = (LONG) dwFontHeight;
	if (!crFont.GetValue("name", szName, LF_FACESIZE))
		lstrcpy(lf.lfFaceName, szName);
	// �^�u�T�C�Y
	ClassRegistory crView("Software\\KoRoN\\ABrowser\\View");
	DWORD dwTabColumn;
	if (!crView.GetValue("tab", &dwTabColumn))
		ChangeTabColumn(dwTabColumn);

	return;
}
//////////////////////////////////////////////KoRoN/1997.08.29/KoRoN/1997.09.01


// RegSave ////////////////////////////////////////////////////////////////1.20
//		���W�X�g���̃Z�[�u�B�t�H���g���ƃt�H���g�T�C�Y���Z�[�u�B
	void
ClassABrowserView::RegSave()
{
	// �t�H���g
	ClassRegistory	crFont("Software\\KoRoN\\ABrowser\\Font");
	crFont.SetValue("size",	(DWORD)lf.lfHeight	);
	crFont.SetValue("name",	lf.lfFaceName		);

	ClassRegistory crView("Software\\KoRoN\\ABrowser\\View");
	crView.SetValue("tab", (DWORD)ChangeTabColumn());
}
//////////////////////////////////////////////KoRoN/1997.08.29/KoRoN/2000.09.24


// Destroy ////////////////////////////////////////////////////////////////1.00
//		WM_DESTROY���󂯂Ă��邪�������Ă��Ȃ��B
int ClassABrowserView::Destroy()
{
	if (hFont) DeleteObject(hFont);
	PostQuitMessage(0);
	return 0;
}
//////////////////////////////////////////////KoRoN/1997.08.29/KoRoN/1997.08.30


// Scroll /////////////////////////////////////////////////////////////////2.00
//		�X�N���[�����̔ėp���\�b�h�B
//		�X�N���[���o�[�͌�ŃN���X�����������ǂ��B
//		����ɂ��X�N���[���֐����̂��̂́A�P���ȍ\���ɂ������B
	int
ClassABrowserView::Scroll(int msg, unsigned short int nLine)
{
	LONG nScrollLine;

	switch(msg)
	{
		case SB_THUMBTRACK:
			return JumpTo((long)(nLine * dblScroll));
		case SB_LINEDOWN:
			nScrollLine =  1;
			break;
		case SB_LINEUP:
			nScrollLine = -1;
			break;
		case SB_PAGEDOWN:
			nScrollLine =  nLineViewPage;
			break;
		case SB_PAGEUP:
			nScrollLine	= -nLineViewPage;
			break;
		default:
			return 0;
	}

	return Scroll(nScrollLine);
}
//////////////////////////////////////////////KoRoN/1997.08.30/KoRoN/1997.10.10


// Scroll /////////////////////////////////////////////////////////////////1.00
	int
ClassABrowserView::Scroll(long nLine)
{
	if (!nLine)
		return 0;

	RECT		rect;
	long		nLineViewNew = nLineView + nLine;

	if (nLineViewNew < 0)
		nLineViewNew = 0;
	else if (nLineViewNew > nLineViewMax)
		nLineViewNew = nLineViewMax;

	if (nLineView == nLineViewNew)
		return 0;
	nLine = nLineViewNew - nLineView;
	nLineView = nLineViewNew;

	GetClientRect(hWnd, &rect);
	ScrollWindowEx(hWnd, 0, -nLine * tm.tmHeight, NULL, &rect, NULL, NULL,
			SW_INVALIDATE | SW_ERASE);

	if (fQueryOn)
	{
		POINT pt;

		GetCursorPos(&pt);
		ScreenToClient(hWnd, &pt);
		long	nLine	= ClientToLine(pt.y);
		long	nColumn	= ClientToColumn(pt.x);
		long	nConclusion = nConclusionLine;
		SetConclusion(nLine, nColumn);
		Paint(nLine, nConclusion);
	}

	return 0;
}
//////////////////////////////////////////////KoRoN/1997.10.10/KoRoN/1997.10.10


// TabChange //////////////////////////////////////////////////////////////1.01
//		�^�u��ς��郁�\�b�h�B
//		2��8��4�Ńg�O�����ɐ؂�ւ��Ă���B
	int
ClassABrowserView::TabChange()
{
	int nTab = ChangeTabColumn();
	switch (nTab)
	{
		case 2:
			nTab = 4;
			break;
		default:
		case 4:
			nTab = 8;
			break;
		case 8:
			nTab = 2;
			break;
	}
	ChangeTabColumn(nTab);

	Information();
	InvalidateRect(hWnd, NULL, FALSE);

	return 0;
}
//////////////////////////////////////////////KoRoN/1997.08.30/KoRoN/2000.09.24


// Init ///////////////////////////////////////////////////////////////////1.00
//		�E�B���h�E�N���X�̓o�^�B���̂��̂̍쐬���s�Ȃ��Ă���B
//		�v���V�[�W���̎w������Ȃ���΂Ȃ�Ȃ��̂��A
//		���Ȃ�i�D���������̂Ƃ��낱�̕��@�����v���t���Ȃ��B
	int
ClassABrowserView::Init(HWND hWnd)
{
	// �e�E�B���h�E�̕ۑ�
	hWndParent				= hWnd;

	// �N���X�̓o�^
	WNDCLASS	wc;{
		wc.style			= CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc		= ClassABrowserViewProc;
		wc.cbClsExtra		= NULL;
		wc.cbWndExtra		= NULL;
		wc.hInstance		= g_hInstance;
		wc.hIcon			= NULL;
		wc.hCursor			= NULL;
		wc.hbrBackground	= NULL;
		wc.lpszMenuName		= NULL;
		wc.lpszClassName	= "ABrVIEW";
	}
	if (!RegisterClass(&wc)) return -1;

	// �E�B���h�E�̍쐬
	if (!(
		ClassABrowserView::hWnd = CreateWindowEx(
			WS_EX_CLIENTEDGE | WS_EX_TOPMOST | WS_EX_TRANSPARENT,
			"ABrVIEW",
			NULL,
			WS_CHILD | WS_MAXIMIZE | WS_VISIBLE | WS_VSCROLL,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			hWnd,
			NULL,
			g_hInstance,
			NULL)
		)) return -1;

	ShowWindow(ClassABrowserView::hWnd, SW_MAXIMIZE);
	UpdateWindow(ClassABrowserView::hWnd);

	return 0;
}
//////////////////////////////////////////////KoRoN/1997.09.01/KoRoN/1997.10.15


// PopupMenu //////////////////////////////////////////////////////////////1.00
//		���{�^���N���b�N�Ń|�b�v�A�b�v���j���[��\������B
//		���̎��̓����̏�Ԃɉ����āA�v�f��ύX���Ă���B
int ClassABrowserView::PopupMenu()
{
		HMENU	hMenu = CreatePopupMenu();

		AppendMenu(hMenu, MF_STRING | (dwQuerySize ? 0 : MF_GRAYED),
			IDC_TEXT_COPY, "�R�s�[(&C)\tCtrl+C");
		AppendMenu(hMenu, MF_STRING | (dwQuerySize ? 0 : MF_GRAYED),
			IDC_WEB_GOOGLE, "Google����(&G)\tCtrl+G");
		AppendMenu(hMenu, MF_SEPARATOR, 0, NULL); // �Z�p���[�^
		AppendMenu(hMenu, MF_STRING | (dwFileSize ? 0 : MF_GRAYED),
			IDC_QUERYALL, "�S����I��\tCtrl+A");
		AppendMenu(hMenu, MF_SEPARATOR, 0, NULL); // �Z�p���[�^
		// �����R�[�h
		AppendMenu(hMenu,
			MF_STRING | ((dwFilter == CTF_FILTER_SJIS) ? MFS_CHECKED : 0),
			IDC_FILTER_SJIS, "&ShiftJIS\tF6");
		AppendMenu(hMenu,
			MF_STRING | ((dwFilter == CTF_FILTER_EUC) ? MFS_CHECKED : 0),
			IDC_FILTER_EUC, "&euc-jp\tF7");
		AppendMenu(hMenu,
			MF_STRING | ((dwFilter == CTF_FILTER_JIS) ? MFS_CHECKED : 0),
			IDC_FILTER_JIS, "&JIS\tF8");
		AppendMenu(hMenu,
			MF_STRING | ((dwFilter == CTF_FILTER_UNICODE) ? MFS_CHECKED : 0),
			IDC_FILTER_UNICODE, "&Unicode\tF9");
		AppendMenu(hMenu,
			MF_STRING | ((dwFilter == CTF_FILTER_UTF8) ? MFS_CHECKED : 0),
			IDC_FILTER_UTF8, "&UTF-&8\tF10");
		AppendMenu(hMenu, MF_SEPARATOR, 0, NULL); // �Z�p���[�^
		// �E�B���h�E�̐ݒ�
		AppendMenu(hMenu, MF_STRING,
			IDC_TAB_CHANGE, "�^�u(2/4/8)�ؑ�\tTab");
		AppendMenu(hMenu, MF_STRING,
			IDC_FONT_SELECT, "�t�H���g(&N)\tCtrl+N");
		AppendMenu(hMenu, MF_SEPARATOR, 0, NULL); // �Z�p���[�^
		// �J���[���]���[�h�̐ݒ�
		AppendMenu(hMenu,
			MF_STRING | (dwColorMode ? MFS_CHECKED : 0),
			IDC_COLOR_REVERSE, "�J���[���](&R)\tCtrl+R");

		POINT	pt;
		GetCursorPos(&pt);
		TrackPopupMenu(hMenu,
			TPM_RIGHTBUTTON | TPM_TOPALIGN | TPM_LEFTALIGN,
			pt.x, pt.y, 0, hWnd, NULL);
		DestroyMenu(hMenu);

		return 0;
}
//////////////////////////////////////////////KoRoN/1997.09.01/KoRoN/1997.09.01


// SetQuery ///////////////////////////////////////////////////////////////1.00
//		�N�G���[���Z�b�g�����肷�郁�\�b�h
//		�I���J�n�A�I�𒆁A�I���I�����Ă��Ǘ����Ă���B
	int
ClassABrowserView::SetQuery(int mode, long x, long y)
{
	long	nLine	= ClientToLine(y);
	long	nColumn	= ClientToColumn(x);

	switch(mode)
	{
		case CAV_SQM_START:
			{
				if (fQueryOn)
					break;
				long	nOldStart	= nStartLine;
				long	nOldEnd		= nEndLine;
				SetOrigin(nLine, nColumn);
				Paint(nOldStart, nOldEnd);
				fQueryMode =
					(x < 3 * tm.tmAveCharWidth || x > 83 * tm.tmAveCharWidth)
					? CTQ_MODE_LINEAR : CTQ_MODE_NORMAL;
				Paint(nLine, nLine);
				fQueryOn			= -1;
				break;
			}
		case CAV_SQM_SET:
			{
				if (!fQueryOn)
					break;
				long	nConclusion = nConclusionLine;
				SetConclusion(nLine, nColumn);
				Paint(nLine, nConclusion);
				break;
			}
		case CAV_SQM_END:
			{
				if (!fQueryOn)
					break;

				long nConclusion = nConclusionLine;
				SetConclusion(nLine, nColumn);
				Paint(nLine, nConclusion);
				fQueryOn = 0;
				QueryGenerate(NULL);
				break;
			}
	}

	return 0;
}
//////////////////////////////////////////////KoRoN/1997.09.24/KoRoN/1997.09.24


// QueryGenerate //////////////////////////////////////////////////////////1.00
//		�I��̈�̑傫����Ԃ��B
//		�|�C���^���w�肳��Ă����ꍇ�ɂ́A�����֓��e���R�s�[����B
	DWORD
ClassABrowserView::QueryGenerate(char* pWrite)
{
	dwQuerySize = 0;
	if (!dwFileSize)
		return 0;

	long	nLine			= nStartLine;
	long	nLineEnd		= nEndLine;

	for (;nLine <= nLineEnd; nLine++)
	{
		GenerateLine(nLine);
		long	nColumn			= 0;
		int		fEnd			= 0;
		int		fKanji			= 0;
		int		fQuery			= IsInQuery(nLine, nColumn);
		char*	psz				= (char*) cmOutline->GetPointer();
		char	ch;

		while (!fEnd)
		{
			// �R�[�h�̓ǂݏo��&NULL�����̌��o
			if ((ch = *psz++) == '\0')
			{
				fEnd = -1;
				if (fQuery)
				{
					dwQuerySize += 2;
					if (pWrite)
					{
						*pWrite++ = '\r';
						*pWrite++ = '\n';
					}
				}
				continue;
			}

			// �N�G���[���ł���Ώo��
			if (fQuery)
			{
				dwQuerySize++;
				if (pWrite) *pWrite++ = ch;
			}
			nColumn++;					// �J���������ɐi�߂�

			// ������2�o�C�g�ڂ͋����I�ɏ�������
			if (!fKanji && IsDBCSLeadByte(ch))
			{
				fKanji = -1;
				continue;
			}else fKanji = 0;


			// �N�G���[�X�V�`�F�b�N
			if (!fQuery && IsInQuery(nLine, nColumn))
				fQuery	= -1;
			else if (fQuery && !IsInQuery(nLine, nColumn))
			{
				fQuery	= 0;
				fEnd	= -1;
			}
		}
	}

	// �f�[�^���쐬�����ꍇ��NULL������t��
	if (dwQuerySize)
	{
		dwQuerySize++;
		if (pWrite) *pWrite = '\0';
	}

	return dwQuerySize;
}
//////////////////////////////////////////////KoRoN/1997.09.24/KoRoN/1997.10.10


// CopyToClipBoard ////////////////////////////////////////////////////////1.00
//		�I������Ă���͈͂�����΁A
//		������R�s�[���ăN���b�v�{�[�h�ɓn���B
	int
ClassABrowserView::CopyToClipboard()
{
	if (!dwQuerySize)
		return 0;

	// ���������m�ۂ��ăN���b�v�{�[�h�p�̃f�[�^���쐬
	HGLOBAL hMem	= GlobalAlloc(GMEM_MOVEABLE, dwQuerySize);
	char* pstr		= (char*)GlobalLock(hMem);
	QueryGenerate(pstr);
	GlobalUnlock(hMem);

	OpenClipboard(hWnd);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hMem);
	CloseClipboard();

	return 0;
}
//////////////////////////////////////////////KoRoN/1997.09.24/KoRoN/1997.09.25


// MouseMove //////////////////////////////////////////////////////////////1.00
//		WM_MOUSEMOVE���󂯂�B
//		�J�[�\�����s�w��p�̈�ɓ��������ɁA
//		�J�[�\������ɕς��邽�߂̃��\�b�h
	int
ClassABrowserView::MouseMove(long x, long y)
{
	SetCursor(LoadCursor(0,
				(x < tm.tmAveCharWidth * 3 || x > tm.tmAveCharWidth * 83)
				? IDC_ARROW : IDC_IBEAM));
	return 0;
}
//////////////////////////////////////////////KoRoN/1997.09.25/KoRoN/1997.09.25


// Timer //////////////////////////////////////////////////////////////////1.00
//		WM_TIMER���󂯂�B
//		�h���b�O���̃J�[�\���ʒu�ɂ��X�N���[������B
	int
ClassABrowserView::Timer()
{
	if (!fQueryOn)
		return 0;

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(hWnd, &pt);
	long	nLine = pt.y / tm.tmHeight;

	if (pt.y < 0)
		nLine--;

	if (nLine <= 0)
		Scroll(nLine - 1);
	if (nLine >= nLineViewPage)
		Scroll(nLine - nLineViewPage + 1);

	return 0;
}
//////////////////////////////////////////////KoRoN/1997.09.25/KoRoN/1997.09.25


// OpenFile ///////////////////////////////////////////////////////////////1.00
//		�V�����e�L�X�g�t�@�C�����J������A�����R�[�h��ς����肷��B���ۂ̏�
//		���͊�{�N���X��TextFile�ɁB
	int
ClassABrowserView::OpenFile(char* szOpenFile, DWORD dwFilter)
{
	HCURSOR hcurOld = GetCursor();
	// �}�E�X�J�[�\�������v��
	SetCursor(LoadCursor(0, IDC_WAIT));

	Open(szOpenFile, dwFilter);

	// �E�B���h�E���̍Đ���
	Information();

	if (dwFilter == CTF_FILTER_AUTO)
		nLineView = 0;
	// �N�G���[�̃��Z�b�g
	SetOrigin(0, 0);
	fQueryMode = CTQ_MODE_NORMAL;
	QueryGenerate();

	SetCursor(hcurOld);
	InvalidateRect(hWnd, NULL, FALSE);

	return 0;
}
//////////////////////////////////////////////KoRoN/1997.10.03/KoRoN/1997.10.03


// ChangeFilter ///////////////////////////////////////////////////////////1.00
//		�t�B���^�[���w�肵��OpenFile���\�b�h���Ă�ł���B
//		OpenFile���\�b�h�͎w�肵�Ȃ���΁A
//		�I�[�g�Ŋ����R�[�h�𔻒肷��悤�ɂȂ��Ă���B
int ClassABrowserView::ChangeFilter(DWORD dwFilter)
{
	if (dwFileSize)
		OpenFile(szFileName, dwFilter);
	return 0;
}
//////////////////////////////////////////////KoRoN/1997.10.03/KoRoN/1997.10.03


// SetQueryAll ////////////////////////////////////////////////////////////1.00
//		�t�@�C���S���̈�I�����郁�\�b�h�B
	int
ClassABrowserView::SetQueryAll()
{
	if (!dwFileSize)
		return 0;

	fQueryOn = 0;
	SetOrigin(0, 0);
	fQueryMode = CTQ_MODE_LINEAR;
	SetConclusion(nTextLine - 1, 0);
	QueryGenerate();

	InvalidateRect(hWnd, NULL, FALSE);

	return 0;
}
//////////////////////////////////////////////KoRoN/1997.10.07/KoRoN/1997.10.07


// JumpTo /////////////////////////////////////////////////////////////////1.00
//		�\���ʒu��ς���B
	int
ClassABrowserView::JumpTo(long nLine)
{
	if (nLine < 0 || nLine > nLineViewMax)
		nLine = nLineViewMax;
	return Scroll(nLine - nLineView);
}
//////////////////////////////////////////////KoRoN/1997.10.07/KoRoN/1997.10.10


// FindText ///////////////////////////////////////////////////////////////1.00
// pszFind��NULL�̎��́A�p������
	int
ClassABrowserView::Find(char *pszFind, bool reverse)
{
	// �t�@�C����������ΏI��
	if (!dwFileSize)
		return 0;

	// �V�K���ǂ����̔���E����
	if (pszFind)
	{
		// �V�K�����̏ꍇ
		nFindLine		= nLineView;
		nFindColumn		= reverse ? -1 : 0;
		if (lstrlen(pszFind) < CAV_FINDTEXT_MAX)
			lstrcpy(szFindText, pszFind);
	}
	long	nWordLength = lstrlen(szFindText);
	if (!nWordLength)
		return 0;

	long nLine		= (nFindLine < 0) ? nTextLine + nFindLine : nFindLine;
	long nColumn	= nFindColumn;
	int		fFind		= 0;

	if (reverse)
	{
		nColumn -= nWordLength + 1;
		if (nColumn < 0)
			--nLine;
	}
	while (!fFind && (0 <= nLine && nLine < nTextLine))
	{
		GenerateLine(nLine);
		char* psz = (char*) cmOutline->GetPointer();
		long	nLength = lstrlen(psz);
		nColumn = (nColumn < 0) ? nLength - nWordLength + nColumn : nColumn;

		while (!fFind &&
				(0 <= nColumn && nColumn <= nLength - nWordLength))
		{
			if (CompareString(LOCALE_USER_DEFAULT, 0, psz + nColumn,
							nWordLength, szFindText, nWordLength)
					== CSTR_EQUAL)
			{
				fFind = -1;
				fQueryOn = 0;

				SetOrigin(nLine, nColumn);
				fQueryMode = CTQ_MODE_NORMAL;
				SetConclusion(nLine, nColumn + nWordLength);
				QueryGenerate();
				InvalidateRect(hWnd ,NULL, FALSE);

				JumpTo(nLine);
				nFindLine		= nLine;
				nFindColumn		= nColumn + nWordLength;
			}
			if (reverse)
			{
				int i;
				--nColumn;
				for (i = 0; i < nColumn; ++i)
					if (IsDBCSLeadByte(psz[i]))
						++i;
				nColumn -= i - nColumn;
			}
			else
			{
				if (IsDBCSLeadByte(*(psz + nColumn)))
					nColumn++;
				nColumn++;
			}
		}

		// �s����
		if (!reverse)
		{
			nColumn = 0;
			++nLine;
		}
		else
		{
			nColumn = -1;
			--nLine;
		}
	}

	if (!fFind)
	{
		if (reverse)
		{
			nFindLine = -1;
			nFindColumn = -1;
		}
		else
		{
			nFindLine		= 0;
			nFindColumn		= 0;
		}
		SetOrigin(0, 0);
		fQueryMode = CTQ_MODE_NORMAL;
		QueryGenerate();
		InvalidateRect(hWnd, NULL, FALSE);

		char msg[256], title[64];
		LoadString(g_hInstance, IDS_FINDERR_TITLE, title, sizeof(title) - 1);
		LoadString(g_hInstance,
				(reverse ? IDS_FINDERR_TOP : IDS_FINDERR_BOTTOM),
				msg, sizeof(msg) - 1);
		MessageBox(hWnd, msg, title, MB_OK | MB_ICONINFORMATION);
	}

	return 1;
}
//////////////////////////////////////////////KoRoN/1997.10.11/KoRoN/1997.10.11


// ChangeColorMode ////////////////////////////////////////////////////////1.00
int ClassABrowserView::ChangeColorMode(DWORD dwMode)
{
	if (dwMode == CTF_COLORMODE_TOGLE)
		dwColorMode = (dwColorMode == CTF_COLORMODE_BLACKBACK)
			? CTF_COLORMODE_WHITEBACK : CTF_COLORMODE_BLACKBACK;
	else dwColorMode = dwMode;

	InvalidateRect(hWnd, NULL, FALSE);
	
	return 0;
}
//////////////////////////////////////////////KoRoN/1997.10.13/KoRoN/1997.10.13



// Information ////////////////////////////////////////////////////////////1.00
//		��ʕ`��̂��߂ɕK�v�ɂȂ邳�܂��܂ȏ��������W�߂ĕۑ����Ă����B
int ClassABrowserView::Information()
{
	// �N���C�A���g�̈���擾
	GetClientRect(hWnd, &rectWindow);

	// �t�H���g���擾
	HDC		hDC			= GetDC(hWnd);
	if (hFont)
		DeleteObject(hFont);
	hFont = CreateFontIndirect(&lf);
	HFONT	hOldFont	= (HFONT)SelectObject(hDC, hFont);
	GetTextMetrics(hDC, &tm);
	SelectObject(hDC, hOldFont);
	ReleaseDC(hWnd, hDC);

	// �ő�\���ʌv�Z
	nLineViewPage = (long)(rectWindow.bottom / tm.tmHeight);
	// �ő�\���ʒu�v�Z�E�␳
	nLineViewMax = nTextLine - nLineViewPage;
	if (nLineViewMax < 0)
		nLineViewMax = 0;

	// �X�N���[���o�[�̏��ݒ�
	si.cbSize	= sizeof(SCROLLINFO);
	si.fMask	= SIF_DISABLENOSCROLL | SIF_PAGE | SIF_RANGE;
	si.nMin		= 0;
	si.nMax		= nTextLine - 1;
	si.nPage	= nLineViewPage;
	// �X�N���[���o�[16bits�̕ǂ����z���邽�߂̕��@
	dblScroll = 1.0;
	if (si.nMax > 65535)
	{
		dblScroll = si.nMax / 65535.0;
		si.nMax		= 65535;
		si.nPage	= (int) (nLineViewPage / dblScroll);
	}
	SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

	return 0;
}
//////////////////////////////////////////////KoRoN/1997.10.15/KoRoN/2000.09.24

#define QUERY_GOOGLE "http://www.google.com/search?hl=ja&lr=lang_ja&ie=Shift_JIS&oe=Shift_JIS&num=50&q=%s"
#define URLENCODE_THROUGH "'()*-._0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"

	static int
url_encode(char* dst, const char* src)
{
	if (dst == NULL)
	{
		int len = 0;
		while (1)
		{
			int ch = ((int)*src++) & 0xff;
			if (ch == 0)
				return len + 1;
			else if (strchr(URLENCODE_THROUGH, ch) != NULL || ch == ' ')
				len += 1;
			else
				len += 3;
		}
	}
	else
	{
		int len = 0;
		while (1)
		{
			int ch = ((int)*src++) & 0xff;
			if (ch == 0)
			{
				dst[len++] = '\0';
				return len;
			}
			if (strchr(URLENCODE_THROUGH, ch) != NULL)
				dst[len++] = ch;
			else if (ch == ' ')
				dst[len++] = '+';
			else
				len += sprintf(&dst[len], "%%%02x", ch);
		}
	}
}

	int
ClassABrowserView::OpenGoogle()
{
	if (dwQuerySize <= 0)
		return 0;
	int len = 16 + strlen(QUERY_GOOGLE);
	char* word = new char[dwQuerySize];
	QueryGenerate(word);
	{
		int reqlen = url_encode(NULL, word);
		char* tmp = new char[reqlen];
		url_encode(tmp, word);
		delete[] word;
		word = tmp;
		len += reqlen;
	}
	char* url = new char[len];
	_snprintf(url, len, QUERY_GOOGLE, word);
	_RPT1(_CRT_WARN, "OpenGoogle: %s\r\n", url);
	ShellExecute(NULL, NULL, url, NULL, NULL, SW_SHOW);
	delete[] url;
	delete[] word;
	return 1;
}
