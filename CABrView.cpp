// vi:set ts=4 sts=4 sw=4 tw=0:
//
// ANSI text Browser                                           since 1997.08.28
//	CABrView.cpp:ClassABrowserViewクラスコード                 Written by KoRoN
//                                                                 Version 1.10
// Last Change: 22-Aug-2011.

#include "ABrowser.h"
#include "CABrView.h"


extern		HINSTANCE			hInstance;
extern		LRESULT CALLBACK	ClassABrowserViewProc
										(HWND, UINT, WPARAM, LPARAM);


// コンストラクタ /////////////////////////////////////////////////////////1.00
//		クエリー用のカラー設定、フォントの用意。
//		レジストリのロード。各メンバの初期化を行なっている。
ClassABrowserView::ClassABrowserView()
: ClassTextFile(), ClassTextQuery(), ClassFindWindow(CAV_FINDTEXT_MAX)
{
	// クエリー用のカラー初期化
	cdQuery.crBackground = GetSysColor(COLOR_HIGHLIGHT);
	cdQuery.crForeground = GetSysColor(COLOR_HIGHLIGHTTEXT);
	dwQuerySize = 0;
	fQueryOn = 0;

	// LOGFONT構造体の宣言と初期化
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight				= -19;
	lf.lfCharSet			= SHIFTJIS_CHARSET;
	lf.lfPitchAndFamily		= FIXED_PITCH | FF_MODERN;
	lstrcpy(lf.lfFaceName, "Terminal");

	// 表示位置の設定
	nLineView		= 0;
	nLineViewMax	= 0;
	nLineViewPage	= 0;

	RegLoad();
	hFont		= NULL;
	hWnd		= NULL;
	hWndParent	= NULL;

	// 検索用パラメータ初期設定
	nFindColumn		= 0;
	nFindLine		= 0;
	lstrcpy(szFindText, "");
}
//////////////////////////////////////////////KoRoN/1997.08.28/KoRoN/1997.08.31


// デストラクタ ///////////////////////////////////////////////////////////1.00
//		特にやることがないので、レジストリのセーブ。
ClassABrowserView::~ClassABrowserView()
{
	RegSave();
}
//////////////////////////////////////////////KoRoN/1997.08.29/KoRoN/1997.08.31


// LineOutSub /////////////////////////////////////////////////////////////1.00
//		指定されたカラーでテキストを表示する。
//		サブルーチンとしてかなり有効なはず。
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
//		指定された一行を、指定されたDCの、指定された位置に表示する。
//		クエリー状況、カラー状態をきちんと表示する優れもの。
//		もしかしたらコードを変えたらもっと良くなるかも???。
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

	// クエリー状況初期化
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

		// カラーコード更新チェック
		if (nLengthColor && (nLengthColor >= pcd->nLength)) fColorChange = -1;

		// クエリー更新チェック
		if ((!fQuery && IsInQuery(line, nColumn)) ||
			(fQuery && !IsInQuery(line, nColumn))) fQueryChange = -1;

		// 表示ルーチン
		if (fQueryChange || (!fQuery && fColorChange) || fEnd)
		{
			SIZE	size;
			LineOutSub(hDC, x, y, pstr, nLength, fQuery ? &cdQuery : pcd);
			GetTextExtentPoint32(hDC, pstr, nLength, &size);
			x += size.cx;
			pstr += nLength;
			nLength = 0;
		}

		// カラーコード更新
		if (fColorChange)
		{
			fColorChange = 0;
			nLengthColor = 0;
			pcd++;
		}

		// クエリー更新
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
//		WM_PAINTに応答。
//		無効化されたクライアント領域を取得し、
//		再描画している。
int ClassABrowserView::Paint()
{
	PAINTSTRUCT		ps;
	HDC				hDC = BeginPaint(hWnd, &ps);
										// DC取得
	// 表示位置の補正
	if (nLineView < 0) nLineView = 0;
	else if (nLineView > nLineViewMax) nLineView = nLineViewMax;

	// スクロールバーの情報設定
	si.fMask	= SIF_DISABLENOSCROLL | SIF_POS;
	si.nPos		= (int)(nLineView / dblScroll);
	SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
	// 画面の端ではスクロールバーを使用不可にする
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


// Paint(クエリー調整用の特殊関数) ////////////////////////////////////////1.00
//		Paint画面更新用のプログラム2。
//		とにかく部分的に表示を変えたい時に使う。
int ClassABrowserView::Paint(long nStartLine, long nEndLine)
{
	long	nTemp = nStartLine;

	// 開始点と終了点の判断
	if (nStartLine > nEndLine)
	{
		nStartLine	= nEndLine;
		nEndLine	= nTemp;
		nTemp		= nStartLine;
	}

	// 表示行によるクリッピング
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
//		クライアント座標xを列数になおしている。
long ClassABrowserView::ClientToColumn(long x)
{
	long nColumn = x / tm.tmAveCharWidth - CAV_LEFT_TEXTMARGIN;
	if (nColumn < 0) nColumn = 0;
	return nColumn;
}
//////////////////////////////////////////////KoRoN/1997.09.24/KoRoN/1997.09.24


// ClientToLine ///////////////////////////////////////////////////////////1.00
//		クライアント座標yを絶対行数になおしている。
long ClassABrowserView::ClientToLine(long y)
{
	long nLine = y / tm.tmHeight;
	if (nLine < 0) nLine = 0;
	else if (nLine > nLineViewPage) nLine = nLineViewPage;
	return nLine + nLineView;
}
//////////////////////////////////////////////KoRoN/1997.09.24/KoRoN/1997.09.24


// FontSelect /////////////////////////////////////////////////////////////1.00
//		フォンとセレクトダイアログを使用して、
//		フォントを選択させている。
//		フォントが選択されたら、再描画。
int ClassABrowserView::FontSelect()
{
	// CHOOSEFONT構造体の宣言と初期化
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
		// ウィンドウ情報の再生成
		Information();
		// ウィンドウの再描画
		InvalidateRect(hWnd, NULL, FALSE);
	}

	return 0;
}
//////////////////////////////////////////////KoRoN/1997.08.29/KoRoN/1997.08.29


// RegLoad ////////////////////////////////////////////////////////////////2.01
//		レジストリのロード
//		フォントとフォントサイズを呼び出す。
	void
ClassABrowserView::RegLoad()
{
	// フォント
	ClassRegistory	crFont("Software\\KoRoN\\ABrowser\\Font");
	DWORD	dwFontHeight;
	char	szName[ LF_FACESIZE ];
	if (!crFont.GetValue("size", &dwFontHeight))
		lf.lfHeight = (LONG) dwFontHeight;
	if (!crFont.GetValue("name", szName, LF_FACESIZE))
		lstrcpy(lf.lfFaceName, szName);
	// タブサイズ
	ClassRegistory crView("Software\\KoRoN\\ABrowser\\View");
	DWORD dwTabColumn;
	if (!crView.GetValue("tab", &dwTabColumn))
		ChangeTabColumn(dwTabColumn);

	return;
}
//////////////////////////////////////////////KoRoN/1997.08.29/KoRoN/1997.09.01


// RegSave ////////////////////////////////////////////////////////////////1.20
//		レジストリのセーブ。フォント名とフォントサイズをセーブ。
	void
ClassABrowserView::RegSave()
{
	// フォント
	ClassRegistory	crFont("Software\\KoRoN\\ABrowser\\Font");
	crFont.SetValue("size",	(DWORD)lf.lfHeight	);
	crFont.SetValue("name",	lf.lfFaceName		);

	ClassRegistory crView("Software\\KoRoN\\ABrowser\\View");
	crView.SetValue("tab", (DWORD)ChangeTabColumn());
}
//////////////////////////////////////////////KoRoN/1997.08.29/KoRoN/2000.09.24


// Destroy ////////////////////////////////////////////////////////////////1.00
//		WM_DESTROYを受けているが何もしていない。
int ClassABrowserView::Destroy()
{
	if (hFont) DeleteObject(hFont);
	PostQuitMessage(0);
	return 0;
}
//////////////////////////////////////////////KoRoN/1997.08.29/KoRoN/1997.08.30


// Scroll /////////////////////////////////////////////////////////////////2.00
//		スクロール専門の汎用メソッド。
//		スクロールバーは後でクラス化した方が良い。
//		それによりスクロール関数そのものは、単純な構造にしたい。
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
//		タブを変えるメソッド。
//		2と8と4でトグル式に切り替えている。
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
//		ウィンドウクラスの登録。そのものの作成を行なっている。
//		プロシージャの指定をしなければならないのが、
//		かなり格好悪いが今のところこの方法しか思い付かない。
	int
ClassABrowserView::Init(HWND hWnd)
{
	// 親ウィンドウの保存
	hWndParent				= hWnd;

	// クラスの登録
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

	// ウィンドウの作成
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
//		左ボタンクリックでポップアップメニューを表示する。
//		その時の内部の状態に応じて、要素を変更している。
int ClassABrowserView::PopupMenu()
{
		HMENU	hMenu = CreatePopupMenu();

		AppendMenu(hMenu, MF_STRING | (dwQuerySize ? 0 : MF_GRAYED),
			IDC_TEXT_COPY, "コピー(&C)\tCtrl+C");
		AppendMenu(hMenu, MF_STRING | (dwQuerySize ? 0 : MF_GRAYED),
			IDC_WEB_GOOGLE, "Google検索(&G)\tCtrl+G");
		AppendMenu(hMenu, MF_SEPARATOR, 0, NULL); // セパレータ
		AppendMenu(hMenu, MF_STRING | (dwFileSize ? 0 : MF_GRAYED),
			IDC_QUERYALL, "全文を選択\tCtrl+A");
		AppendMenu(hMenu, MF_SEPARATOR, 0, NULL); // セパレータ
		// 漢字コード
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
		AppendMenu(hMenu, MF_SEPARATOR, 0, NULL); // セパレータ
		// ウィンドウの設定
		AppendMenu(hMenu, MF_STRING,
			IDC_TAB_CHANGE, "タブ(2/4/8)切替\tTab");
		AppendMenu(hMenu, MF_STRING,
			IDC_FONT_SELECT, "フォント(&N)\tCtrl+N");
		AppendMenu(hMenu, MF_SEPARATOR, 0, NULL); // セパレータ
		// カラー反転モードの設定
		AppendMenu(hMenu,
			MF_STRING | (dwColorMode ? MFS_CHECKED : 0),
			IDC_COLOR_REVERSE, "カラー反転(&R)\tCtrl+R");

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
//		クエリーをセットしたりするメソッド
//		選択開始、選択中、選択終了総てを管理している。
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
//		選択領域の大きさを返す。
//		ポインタが指定されていた場合には、そこへ内容をコピーする。
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
			// コードの読み出し&NULL文字の検出
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

			// クエリー内であれば出力
			if (fQuery)
			{
				dwQuerySize++;
				if (pWrite) *pWrite++ = ch;
			}
			nColumn++;					// カラムを次に進める

			// 漢字の2バイト目は強制的に処理する
			if (!fKanji && IsDBCSLeadByte(ch))
			{
				fKanji = -1;
				continue;
			}else fKanji = 0;


			// クエリー更新チェック
			if (!fQuery && IsInQuery(nLine, nColumn))
				fQuery	= -1;
			else if (fQuery && !IsInQuery(nLine, nColumn))
			{
				fQuery	= 0;
				fEnd	= -1;
			}
		}
	}

	// データが作成される場合はNULL文字を付加
	if (dwQuerySize)
	{
		dwQuerySize++;
		if (pWrite) *pWrite = '\0';
	}

	return dwQuerySize;
}
//////////////////////////////////////////////KoRoN/1997.09.24/KoRoN/1997.10.10


// CopyToClipBoard ////////////////////////////////////////////////////////1.00
//		選択されている範囲があれば、
//		それをコピーしてクリップボードに渡す。
	int
ClassABrowserView::CopyToClipboard()
{
	if (!dwQuerySize)
		return 0;

	// メモリを確保してクリップボード用のデータを作成
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
//		WM_MOUSEMOVEを受ける。
//		カーソルが行指定用領域に入った時に、
//		カーソルを矢印に変えるためのメソッド
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
//		WM_TIMERを受ける。
//		ドラッグ時のカーソル位置によりスクロールする。
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
//		新しいテキストファイルを開いたり、漢字コードを変えたりする。実際の処
//		理は基本クラスのTextFileに。
	int
ClassABrowserView::OpenFile(char* szOpenFile, DWORD dwFilter)
{
	HCURSOR hcurOld = GetCursor();
	// マウスカーソルを時計に
	SetCursor(LoadCursor(0, IDC_WAIT));

	Open(szOpenFile, dwFilter);

	// ウィンドウ情報の再生成
	Information();

	if (dwFilter == CTF_FILTER_AUTO)
		nLineView = 0;
	// クエリーのリセット
	SetOrigin(0, 0);
	fQueryMode = CTQ_MODE_NORMAL;
	QueryGenerate();

	SetCursor(hcurOld);
	InvalidateRect(hWnd, NULL, FALSE);

	return 0;
}
//////////////////////////////////////////////KoRoN/1997.10.03/KoRoN/1997.10.03


// ChangeFilter ///////////////////////////////////////////////////////////1.00
//		フィルターを指定してOpenFileメソッドを呼んでいる。
//		OpenFileメソッドは指定しなければ、
//		オートで漢字コードを判定するようになっている。
int ClassABrowserView::ChangeFilter(DWORD dwFilter)
{
	if (dwFileSize)
		OpenFile(szFileName, dwFilter);
	return 0;
}
//////////////////////////////////////////////KoRoN/1997.10.03/KoRoN/1997.10.03


// SetQueryAll ////////////////////////////////////////////////////////////1.00
//		ファイル全域を領域選択するメソッド。
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
//		表示位置を変える。
	int
ClassABrowserView::JumpTo(long nLine)
{
	if (nLine < 0 || nLine > nLineViewMax)
		nLine = nLineViewMax;
	return Scroll(nLine - nLineView);
}
//////////////////////////////////////////////KoRoN/1997.10.07/KoRoN/1997.10.10


// FindText ///////////////////////////////////////////////////////////////1.00
// pszFindがNULLの時は、継続検索
	int
ClassABrowserView::Find(char *pszFind, bool reverse)
{
	// ファイルが無ければ終了
	if (!dwFileSize)
		return 0;

	// 新規かどうかの判定・処理
	if (pszFind)
	{
		// 新規検索の場合
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

		// 行送り
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
//		画面描画のために必要になるさまざまな情報をかき集めて保存しておく。
int ClassABrowserView::Information()
{
	// クライアント領域情報取得
	GetClientRect(hWnd, &rectWindow);

	// フォント情報取得
	HDC		hDC			= GetDC(hWnd);
	if (hFont)
		DeleteObject(hFont);
	hFont = CreateFontIndirect(&lf);
	HFONT	hOldFont	= (HFONT)SelectObject(hDC, hFont);
	GetTextMetrics(hDC, &tm);
	SelectObject(hDC, hOldFont);
	ReleaseDC(hWnd, hDC);

	// 最大表示量計算
	nLineViewPage = (long)(rectWindow.bottom / tm.tmHeight);
	// 最大表示位置計算・補正
	nLineViewMax = nTextLine - nLineViewPage;
	if (nLineViewMax < 0)
		nLineViewMax = 0;

	// スクロールバーの情報設定
	si.cbSize	= sizeof(SCROLLINFO);
	si.fMask	= SIF_DISABLENOSCROLL | SIF_PAGE | SIF_RANGE;
	si.nMin		= 0;
	si.nMax		= nTextLine - 1;
	si.nPage	= nLineViewPage;
	// スクロールバー16bitsの壁を乗り越えるための方法
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
