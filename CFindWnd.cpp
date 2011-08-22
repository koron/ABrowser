// vi:set ts=4 sts=4 sw=4 tw=0:
//
// 検索ウィンドウクラス                                        since 1997.10.12
//	CFindWnd.cpp:ClassFindWindowコード                         Written by KoRoN
//                                                                 Version 1.00
//                                                                   1997.10.12
// Last Change: 13:37:31 11-Mar-2001.

#define		WIN32_LEAN_AND_MEAN

#include	<windows.h>
#include	"CFindWnd.h"

// コンストラクタ /////////////////////////////////////////////////////////1.00
ClassFindWindow::ClassFindWindow(WORD length)
{
	wFindLength = length;
	memset(&fr, 0, sizeof(FINDREPLACE));
	hWnd = 0;
	pszFind = new char[length];
	memset(pszFind, 0, length);
	uMsgTarget = RegisterWindowMessage(FINDMSGSTRING);
	uMsgTranslate = uMsgTarget;
}
//////////////////////////////////////////////KoRoN/1997.10.12/KoRoN/1997.10.12


// デストラクタ ///////////////////////////////////////////////////////////1.00
ClassFindWindow::~ClassFindWindow()
{
	delete[] pszFind;
}
//////////////////////////////////////////////KoRoN/1997.10.12/KoRoN/1997.10.12


// Registe ////////////////////////////////////////////////////////////////1.00
UINT ClassFindWindow::Registe(UINT uMsg)
{
	if (uMsg == 0)
		goto END_REGISTER_MESSAGE;
	uMsgTranslate = uMsg;
END_REGISTER_MESSAGE:
	return uMsgTranslate;
}
//////////////////////////////////////////////KoRoN/1997.10.12/KoRoN/1997.10.12


// Translate //////////////////////////////////////////////////////////////1.00
UINT ClassFindWindow::Translate(UINT& uMsg)
{
	uMsg = (uMsg == uMsgTarget) ? uMsgTranslate : uMsg;
	return uMsg;
}
//////////////////////////////////////////////KoRoN/1997.10.12/KoRoN/1997.10.12


// Get ////////////////////////////////////////////////////////////////////1.00
char* ClassFindWindow::Get()
{
	return pszFind;
}
//////////////////////////////////////////////KoRoN/1997.10.12/KoRoN/1997.10.12


// Create /////////////////////////////////////////////////////////////////1.00
void ClassFindWindow::Create(HWND hOwnerWnd, DWORD dwFlag)
{
	fr.lStructSize		= sizeof(FINDREPLACE);
	fr.hwndOwner		= hOwnerWnd;
	fr.lpstrFindWhat	= pszFind;
	fr.wFindWhatLen		= wFindLength;
	fr.Flags			= dwFlag;

	hWnd = FindText(&fr);
}
//////////////////////////////////////////////KoRoN/1997.10.12/KoRoN/1997.10.12


// Destroy ////////////////////////////////////////////////////////////////1.00
void ClassFindWindow::Destroy()
{
	if (hWnd) DestroyWindow(hWnd);
	hWnd = 0;
}
//////////////////////////////////////////////KoRoN/1997.10.12/KoRoN/1997.10.12


// Message ////////////////////////////////////////////////////////////////1.00
int ClassFindWindow::Message(LPMSG lpMsg)
{
	if (!hWnd) return 0;
	if (!IsDialogMessage(hWnd, lpMsg)) return 0;
	return 1;
}
//////////////////////////////////////////////KoRoN/1997.10.12/KoRoN/1997.10.12


// IsFindNext /////////////////////////////////////////////////////////////1.00
int ClassFindWindow::IsFindNext(LPARAM lParam)
{
	LPFINDREPLACE lpfr = (LPFINDREPLACE) lParam;
	return (lpfr->Flags & FR_FINDNEXT) ? (lpfr->Flags & FR_DOWN ? 1 : 2) : 0;
}
//////////////////////////////////////////////KoRoN/1997.10.12/KoRoN/1997.10.12


//  ///////////////////////////////////////////////////////////////////////1.00
//////////////////////////////////////////////KoRoN/1997.10.12/KoRoN/1997.10.12
