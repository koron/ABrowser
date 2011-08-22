// vi:set ts=4 sts=4 sw=4 tw=0:
//
// ANSIBrowserクラス                                           since 1997.09.03
//	CABr.h:ClassABrowserヘッダー                               Written by KoRoN
//                                                                 Version 1.01
// Last Change: 17-Oct-2003.

#ifndef			CLASS_ABROWSER_INCLUDED

#include		"CCmdLine.h"
#include		"CABrView.h"
#include		"CStatusW.h"

#define			CAB_WINDOW_MENU			0x00000001
#define			CAB_WINDOW_STATUS		0x00000002
#define			CAB_WINDOW_REVERSE		0x00000004

// ClassABrowser //////////////////////////////////////////////////////////1.00
class ClassABrowser
: ClassCommandLine, public ClassABrowserView, public ClassStatusWindow
{
	public:
		bool m_bEditOpened;
		bool canOpenEditor() { return !m_bEditOpened; }
		bool openEditor();
		bool selectEditor();

	public:
		HINSTANCE hInst;
		HWND Init(HINSTANCE  hInst);
		int			AutoHeight();
		int			AutoWidth();
		int			ChangeFilter(DWORD dwFilter);
		int			Create(HWND hWnd);
		int			Destroy();
		int			DropFile(HANDLE hDrop);
		int			Find(char *psz = NULL, bool reverse = false);
		int			InitMenu();
		int			MenuSelect(UINT uItem = 0);
		void		MenuStatus(UINT uItem, UINT uFlag);
		int			OpenFile(char* szOpenFile);
		int			OpenFile(void);
		int			Paint();
		int			SetLineNumber();
		int			ShowMenu();
		int			ShowStatus();
		int			Size();
		void		SetInformation();

		ClassABrowser(char* szDefTitle = "ABrowser");
		~ClassABrowser();

		HWND		hWnd;
		RECT		rectWindow;

	protected:

	private:
		HWND hPrevWnd;
		void		RegLoad();
		void		RegSave();

		DWORD		dwWindow;
		HMENU		hMenu;
		char*		szDefTitle;
		char*		szTitle;
};
//////////////////////////////////////////////KoRoN/1997.09.03/KoRoN/1997.10.14


#define			CLASS_ABROWSER_INCLUDED
#endif


//  ///////////////////////////////////////////////////////////////////////1.00
//////////////////////////////////////////////KoRoN/1997.09.03/KoRoN/1997.09.03
