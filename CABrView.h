// vi:set ts=4 sts=4 sw=4 tw=0:
//
// ANSI text Browser                                           since 1997.08.28
//	CABrView.h:ClassABrowserViewÉwÉbÉ_Å[                       Written by KoRoN
//                                                                 Version 1.00
// Last Change: 30-Oct-2004.

#ifndef		CLASS_ABROWSERVIEW_INCLUDED


#include	"CReg.h"
#include	"CTxtFile.h"
#include	"CTxtQry.h"
#include	"CFindWnd.h"


#define		CAV_LEFT_TEXTMARGIN		3

#define		CAV_SQM_START			-1
#define		CAV_SQM_SET				0
#define		CAV_SQM_END				1

#define		CAV_FINDTEXT_MAX		100


// ClassABrowserView //////////////////////////////////////////////////////1.00
class ClassABrowserView 
: protected ClassTextFile, protected ClassTextQuery,
  public ClassFindWindow
{
public:
	int Information();
	int ChangeColorMode( DWORD dwMode = CTF_COLORMODE_TOGLE );
	int	Find(char *psz = NULL, bool reverse = false);
	int			ChangeFilter( DWORD dwFilter );
	int			CopyToClipboard();
	int			Init( HWND hWnd );
	int			Destroy();
	int			FontSelect();
	int			JumpTo( long nLine );
	int			MouseMove( long x, long y );
	int			OpenFile( char* szOpenFile,
									DWORD dwFilter = CTF_FILTER_AUTO );
	int			Paint();
	int			Paint( long nStartLine, long nEndLine );
	int			PopupMenu();
	int			Scroll( long nLine = 0 );
	int			Scroll( int msg, unsigned short int nLine );
	int			SetQuery( int mode, long x, long y );
	int			SetQueryAll();
	int			TabChange();
	int			Timer();
	int			OpenGoogle();

	HWND		hWnd;
	HWND		hWndParent;

	ClassABrowserView();
	~ClassABrowserView();

protected:
	DWORD		QueryGenerate( char* pWrite = NULL );

	DWORD		dwQuerySize;
	long		nLineView;
	char		szFindText[ CAV_FINDTEXT_MAX ];
	TEXTMETRIC	tm;

private:
	int PaintSub( HDC hDC, long i, long to, int fRepaint );
	SCROLLINFO si;
	double dblScroll;
	long		ClientToColumn( long x );
	long		ClientToLine( long y );
	void		LineOut( HDC hDC, long x, long y,
									long line, int fRepaint );
	void		LineOutSub( HDC hDC, long x, long y,
					char* pstr, long nLength, CTF_COLORDATA* pcd );
	void		RegSave();
	void		RegLoad();

	HFONT hFont;
	long		nFindColumn;
	long		nFindLine;
	CTF_COLORDATA cdQuery;
	int			fQueryOn;
	LOGFONT		lf;
	long		nLineViewMax;
	long		nLineViewPage;
	RECT		rectWindow;
};
//////////////////////////////////////////////KoRoN/1997.08.28/KoRoN/1997.09.25


//  ///////////////////////////////////////////////////////////////////////1.00
//////////////////////////////////////////////KoRoN/1997.09.25/KoRoN/1997.09.25


#define			CLASS_ABROWSERVIEW_INCLUDED
#endif
