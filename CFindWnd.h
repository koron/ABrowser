// 検索ウィンドウクラス                                        since 1997.10.12
//	CFindWnd.h:ClassFindWindowヘッダー                         Written by KoRoN
//                                                                 Version 1.00
//                                                                   1997.10.12


#ifndef		CLASS_FINDWINDOW_INCLUDED

#include	<commdlg.h>


// ClassFindWindow ////////////////////////////////////////////////////////1.00
class ClassFindWindow {
public:
	int IsFindNext( LPARAM lParam );
	void Destroy();
	int Message( LPMSG lpMsg );
	void Create( HWND hOwnerWnd, DWORD dwFlag );
	char* Get();
	UINT Translate( UINT& uMsg );
	UINT Registe( UINT uMsg );
	ClassFindWindow( WORD length = 512 );
	~ClassFindWindow();

protected:

private:
	HWND hWnd;
	FINDREPLACE fr;
	char *pszFind;
	WORD wFindLength;
	UINT uMsgTarget;
	UINT uMsgTranslate;
};
//////////////////////////////////////////////KoRoN/1997.10.12/KoRoN/1997.10.12

#define		CLASS_FINDWINDOW_INCLUDED
#endif


//  ///////////////////////////////////////////////////////////////////////1.00
//////////////////////////////////////////////KoRoN/1997.10.12/KoRoN/1997.10.12
