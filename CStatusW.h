// ステータスウィンドウ                                        since 1997.09.03
//	CStatus.h:ClassStatusWindowヘッダー                        Written by KoRoN
//                                                                 Version 1.00
//                                                                   1997.10.03

#ifndef		CLASS_STATUSWINDOW_INCDLUDED


#define		CSW_PARTS_MAX	254

#define		CSW_STATUS_NONE		0
#define		CSW_STATUS_SHOW		1
#define		CSW_STATUS_HIDE		2

// ClassStatusWindow //////////////////////////////////////////////////////1.00
class ClassStatusWindow {
public:
	int getStatus() { return nStatus; }
	HWND Init( HWND hParentWnd );
	int SetStatus( int nStatus = CSW_STATUS_NONE );
	int		SetText( int nPart, char* psz,
								DWORD dwOption = 0, int fRepaint = FALSE );
	~ClassStatusWindow();
	ClassStatusWindow();

	HWND	hWnd;

protected:
//	int		Create();
	int		SetPart( int nSize );
	int		Size( int x, int y, int width, int height );

private:
	int nStatus;
	int		nParts;
	int*	nPartSize;
	int*	nSepPos;
};
//////////////////////////////////////////////KoRoN/1997.09.03/KoRoN/1997.10.03


#define		CLASS_STATUSWINDOW_INCDLUDED
#endif
// vi:ts=4 sts=4 sw=4 tw=0:
