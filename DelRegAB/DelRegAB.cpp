// Delete Registory of ABrowser                                since 1997.09.26
//	DelRegAB.cpp:エントリーコード                              Written by KoRoN
//                                                                 Version 1.00
//                                                                   1997.09.26
// Last Change: 01:23:51 25-Sep-2000.

#define		WIN32_LEAN_AND_MEAN

#include	<windows.h>
#include	"resource.h"

#define		DELREG_TEXT_BUFFER		1024
#define		DELREG_TITLE_BUFFER		64


HINSTANCE	hInstance = NULL;


// Message ////////////////////////////////////////////////////////////////1.00
	int
Message(UINT uIDtitle, UINT uIDtext, UINT uType)
{
	if (!hInstance)
		return 0;

	char	szText[DELREG_TEXT_BUFFER];
	char	szTitle[DELREG_TITLE_BUFFER];

	LoadString(hInstance, uIDtitle, szTitle, DELREG_TITLE_BUFFER);
	LoadString(hInstance, uIDtext, szText, DELREG_TEXT_BUFFER);
	return MessageBox(NULL, szText, szTitle, uType);
}
//////////////////////////////////////////////KoRoN/1997.09.26/KoRoN/1997.09.26


// Delete /////////////////////////////////////////////////////////////////1.00
	int
Delete()
{
	RegDeleteKey(HKEY_CURRENT_USER, "Software\\KoRoN\\ABrowser\\View");
	RegDeleteKey(HKEY_CURRENT_USER, "Software\\KoRoN\\ABrowser\\Font");
	RegDeleteKey(HKEY_CURRENT_USER, "Software\\KoRoN\\ABrowser\\Window");
	RegDeleteKey(HKEY_CURRENT_USER, "Software\\KoRoN\\ABrowser");
	return 0;
}
//////////////////////////////////////////////KoRoN/1997.09.26/KoRoN/2000.09.25


// WinMain ////////////////////////////////////////////////////////////////1.00
	int WINAPI
WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pszCmdLine, int iCmdShow)
{
	hInstance = hInst;

	int result = Message(IDS_DLG_TITLE, IDS_DLG_TEXT,
			MB_DEFBUTTON2 | MB_ICONQUESTION | MB_SETFOREGROUND | MB_YESNO);
	switch (result)
	{
		case IDYES:
			Delete();
			Message(IDS_DEL_TITLE, IDS_DEL_TEXT,
					MB_ICONEXCLAMATION | MB_OK | MB_SETFOREGROUND);
			break;

		default:
			Message(IDS_NODEL_TITLE, IDS_NODEL_TEXT,
					MB_ICONSTOP | MB_OK | MB_SETFOREGROUND);
			break;
	}

	return 0;
}
//////////////////////////////////////////////KoRoN/1997.09.26/KoRoN/1997.09.26


//  ///////////////////////////////////////////////////////////////////////1.00
//////////////////////////////////////////////KoRoN/1997.09.26/KoRoN/1997.09.26
// vi:ts=4 sts=4 sw=4 tw=0:
