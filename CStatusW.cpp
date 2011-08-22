// ステータスウィンドウ                                        since 1997.09.03
//	CStatus.cpp:ClassStatusWindowコード                        Written by KoRoN
//                                                                 Version 1.00
//                                                                   1997.10.10
// Last Change: 14:56:41 10-Mar-2001.

#define		WIN32_LEAN_AND_MEAN

#include	<windows.h>
#include	<commctrl.h>
#include	"CStatusW.h"
#pragma comment(lib, "comctl32.lib")


// コンストラクタ /////////////////////////////////////////////////////////1.00
ClassStatusWindow::ClassStatusWindow()
{
	hWnd = NULL;
	nParts = 0;
	nPartSize	= new int[CSW_PARTS_MAX];
	nSepPos		= new int[CSW_PARTS_MAX + 1];
	nStatus		= CSW_STATUS_SHOW;

	for (int i = 0; i < CSW_PARTS_MAX; i++)
	{
		nPartSize[i]	= 0;
		nSepPos[i]	= 0;
	}
}
//////////////////////////////////////////////KoRoN/1997.09.03/KoRoN/1997.09.03


// デストラクタ ///////////////////////////////////////////////////////////1.00
ClassStatusWindow::~ClassStatusWindow()
{
	delete[] nPartSize;
	delete[] nSepPos;
}
//////////////////////////////////////////////KoRoN/1997.09.05/KoRoN/1997.09.05


// Size ///////////////////////////////////////////////////////////////////1.00
int ClassStatusWindow::Size(int x, int y, int width, int height)
{
	int		z = width - GetSystemMetrics(SM_CXVSCROLL);
	int		i = nParts - 1;
	for (nSepPos[nParts] = z; i >= 0; i--)
		nSepPos[i] = z -= nPartSize[i];
	SendMessage(hWnd, SB_SETPARTS, nParts + 1, (LPARAM)nSepPos);
	MoveWindow(hWnd, x, y, width, height, TRUE);

	return 0;
}
//////////////////////////////////////////////KoRoN/1997.09.03/KoRoN/1997.09.03


// SetPart ////////////////////////////////////////////////////////////////1.00
int ClassStatusWindow::SetPart(int nSize)
{
	if (nParts >= CSW_PARTS_MAX || nSize <= 0) return -1;
	nPartSize[nParts++] = nSize;
	return nParts;
}
//////////////////////////////////////////////KoRoN/1997.09.05/KoRoN/1997.09.05


// SetText ////////////////////////////////////////////////////////////////1.00
int ClassStatusWindow::SetText(int nPart, char* psz,
										DWORD dwOption, int fRepaint)
{
	if (nPart < 0)
		nPart = nParts + 1 + nPart;

	SendMessage(hWnd, SB_SETTEXT,
			(WPARAM)nPart | (WPARAM)dwOption, (LPARAM)psz);

	if (fRepaint)
		InvalidateRect(hWnd, NULL, TRUE);
	return 0;
}
//////////////////////////////////////////////KoRoN/1997.09.06/KoRoN/1997.09.06


// SetStatus //////////////////////////////////////////////////////////////1.00
int ClassStatusWindow::SetStatus(int nStatus)
{
	switch (nStatus)
	{
	case CSW_STATUS_SHOW:
		ShowWindow(hWnd, SW_SHOWNORMAL);
		break;
	case CSW_STATUS_HIDE:
		ShowWindow(hWnd, SW_HIDE);
		break;
	case CSW_STATUS_NONE:
	default:
		nStatus = ClassStatusWindow::nStatus;
		break;
	}

	return ClassStatusWindow::nStatus = nStatus;
}
//////////////////////////////////////////////KoRoN/1997.10.10/KoRoN/1997.10.10


// Init ///////////////////////////////////////////////////////////////////1.00
    HWND
ClassStatusWindow::Init(HWND hParentWnd)
{
	if (!hWnd)
	{
		hWnd = CreateStatusWindow(
				WS_CHILD | WS_VISIBLE | CCS_BOTTOM | SBARS_SIZEGRIP,
				NULL, hParentWnd, 101);
	}

	return hWnd;
}
//////////////////////////////////////////////KoRoN/1997.10.15/KoRoN/1997.10.15


//  ///////////////////////////////////////////////////////////////////////1.00
//////////////////////////////////////////////KoRoN/1997.10.15/KoRoN/1997.10.15
// vi:ts=4 sts=4 sw=4 tw=0:
