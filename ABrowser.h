/* vi:set ts=8 sts=4 sw=4 tw=0 */
/*
 * ABrowser.h - ABrowser united header
 *
 * Last Change: 22-Aug-2011.
 * Written by:  Muraoka Taro  <koron@tka.att.ne.jp>
 */
#ifndef ABROWSER_H
#define ABROWSER_H

#include <windows.h>
#include <commdlg.h>
#include <commctrl.h>
#include <stdio.h>
#ifdef __MINGW32__
#define _RPT0(a1, a2) fprintf(stderr, a2)
#define _RPT1(a1, a2, a3) fprintf(stderr, a2, a3)
#define _RPT2(a1, a2, a3, a4) fprintf(stderr, a2, a3, a4)
#else
#include <crtdbg.h>
#endif
#include "resource.h"

#define ABROWSER_VERSION "1.3.7"
#define APPLICATION_NAME "ANSI text Browser"
#define INFO_VERSION "Åwñ˚âLç¿Åx Version " ABROWSER_VERSION " (31-Oct-2004)"
#define INFO_EMAIL "koron@tka.att.ne.jp"
#define INFO_URL "http://www.kaoriya.net/"
#define INFO_STATUS "ñ˚âLç¿ Version " ABROWSER_VERSION

#define REGKEY_BASE "Software\\KoRoN\\ABrowser"
#define REGKEY_EDITCMD "EditorCommand"
#define REGKEY_WINDOW REGKEY_BASE "\\Window"
#define DEFAUTL_REG_EDITCMD "notepad.exe \"$f\""

extern HINSTANCE g_hInstance;

#endif /* ABROWSER_H */
