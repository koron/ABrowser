/*
 * winapiex.h: Win32 API extention header
 *
 * Last change: 03:05:41 29-Jun-2000.
 * Written by:  Taro Muraoka  <koron@tka.att.ne.jp>
 *
 * Copyright (C) 1999 2000, KoRoN
 */
/*
 * Need to include <windows.h>
 */
#ifndef _INCLUDED_WINAPIEX
#define _INCLUDED_WINAPIEX

#define VERIFY_API(a, p) if (!a##p) ErrorMessageBox(#a, GetLastError())
#define VERIFY_API2(r, a, p) if (!(r = a##p)) ErrorMessageBox(#a, \
	GetLastError())

/*
 * Scroll bar events
 */
#define PROTO_ONHSCROLL(n) LRESULT n(HWND hWnd, \
	int nScrollCode, short nPos, HWND hwndScrollBar)
#define CALL_ONHSCROLL(n) n(hWnd, \
	(int)LOWORD(wParam), (short)HIWORD(wParam), (HWND)lParam)
#define PROTO_ONVSCROLL(n) LRESULT n(HWND hWnd, \
	int nScrollCode, short nPos, HWND hwndScrollBar)
#define CALL_ONVSCROLL(n) n(hWnd, \
	(int)LOWORD(wParam), (short)HIWORD(wParam), (HWND)lParam)

/*
 * Mouse events
 */
#define PROTO_ONLBUTTONDBLCLK(n) LRESULT n(HWND hWnd, \
	DWORD fwKeys, int xPos, int yPos);
#define CALL_ONLBUTTONDBLCLK(n) n(hWnd, (DWORD)wParam, \
	(int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam))
#define PROTO_ONLBUTTONDOWN(n) LRESULT n(HWND hWnd, \
	DWORD fwKeys, int xPos, int yPos)
#define CALL_ONLBUTTONDOWN(n) n(hWnd, (DWORD)wParam, \
	(int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam))
#define PROTO_ONLBUTTONUP(n) LRESULT n(HWND hWnd, \
	DWORD fwKeys, int xPos, int yPos)
#define CALL_ONLBUTTONUP(n) n(hWnd, (DWORD)wParam, \
	(int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam))
#define PROTO_ONMBUTTONDBLCLK(n) LRESULT \
    n(HWND hWnd, DWORD fwKeys, int xPos, int yPos);
#define CALL_ONMBUTTONDBLCLK(n) n(hWnd, (DWORD)wParam, \
	(int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam))
#define PROTO_ONMBUTTONDOWN(n) LRESULT n(HWND hWnd, \
	DWORD fwKeys, int xPos, int yPos)
#define CALL_ONMBUTTONDOWN(n) n(hWnd, (DWORD)wParam, \
	(int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam))
#define PROTO_ONMBUTTONUP(n) LRESULT n(HWND hWnd, \
	DWORD fwKeys, int xPos, int yPos)
#define CALL_ONMBUTTONUP(n) n(hWnd, (DWORD)wParam, \
	(int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam))
#define PROTO_ONRBUTTONDBLCLK(n) LRESULT n(HWND hWnd, \
	DWORD fwKeys, int xPos, int yPos);
#define CALL_ONRBUTTONDBLCLK(n) n(hWnd, (DWORD)wParam, \
	(int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam))
#define PROTO_ONRBUTTONDOWN(n) LRESULT n(HWND hWnd, \
	DWORD fwKeys, int xPos, int yPos)
#define CALL_ONRBUTTONDOWN(n) n(hWnd, (DWORD)wParam, \
	(int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam))
#define PROTO_ONRBUTTONUP(n) LRESULT n(HWND hWnd, \
	DWORD fwKeys, int xPos, int yPos)
#define CALL_ONRBUTTONUP(n) n(hWnd, (DWORD)wParam, \
	(int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam))
#define PROTO_ONMOUSEMOVE(n) LRESULT n(HWND hWnd, \
	DWORD fwKeys, int xPos, int yPos)
#define CALL_ONMOUSEMOVE(n) n(hWnd, (DWORD)wParam, \
	(int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam))

#define PROTO_ONINITMENU(n) LRESULT n(HWND hWnd, HMENU hmenu)
#define CALL_ONINITMENU(n) n(hWnd, (HMENU)wParam)
#define PROTO_ONCREATE(n) LRESULT n(HWND hWnd, CREATESTRUCT *cs)
#define CALL_ONCREATE(n) n(hWnd, (LPCREATESTRUCT*)lParam)
#define PROTO_ONCLOSE(n) LRESULT n(HWND hWnd)
#define CALL_ONCLOSE(n) n(hWnd)
#define PROTO_ONDESTROY(n) LRESULT n(HWND hWnd)
#define CALL_ONDESTROY(n) n(hWnd)
#define PROTO_ONQUIT(n) LRESULT n(HWND hWnd, int nExitCode)
#define CALL_ONQUIT(n) n(hWnd, (int)wParam)
#define PROTO_ONPAINT(n) LRESULT n(HWND hWnd, HDC hdc)
#define CALL_ONPAINT(n) n(hWnd, (HDC)wParam)
#define PROTO_ONSIZING(n) LRESULT n(HWND hWnd, WPARAM fwSide, LPRECT lprc)
#define CALL_ONSIZING(n) n(hWnd, wParam, (LPRECT)lParam)
#define PROTO_ONSYSCOMMAND(n) LRESULT n(HWND hWnd, \
	DWORD uCmdType, int xPos, int yPos)
#define CALL_ONSYSCOMMAND(n) n(hWnd, (DWORD)wParam, \
	(int)(short)LOWORD(lParam), (int)(short)HIWORD(yPos))
#define PROTO_ONCOMMAND(n) LRESULT n(HWND hWnd, \
	WORD wNotifyCode, WORD wID, HWND hWndCtl)
#define CALL_ONCOMMAND(n) n(hWnd, \
	HIWORD(wParam), LOWORD(wParam), (HWND)lParam)
#define PROTO_ONKEYDOWN(n) LRESULT n(HWND hWnd, \
	int nVirtKey, LPARAM lKeyData)
#define CALL_ONKEYDOWN(n) n(hWnd, (int)wParam, lParam)
#define PROTO_ONKEYUP(n) LRESULT n(HWND hWnd, int nVirtKey, LPARAM lKeyData)
#define CALL_ONKEYUP(n) n(hWnd, (int)wParam, lParam)
#define PROTO_WINPROC(n) LRESULT CALLBACK n(HWND hWnd, \
	UINT uMsg, WPARAM wParam, LPARAM lParam)


#ifdef __cplusplus
extern "C" {
#endif
    BITMAPINFO* WINAPI Init_BITMAPINFO(BITMAPINFO *bi,
	int width, int height, int depth);
    PIXELFORMATDESCRIPTOR* WINAPI
	Init_PIXELFORMATDESCRIPTOR(PIXELFORMATDESCRIPTOR *pfd);
    WNDCLASSEX* WINAPI Init_WNDCLASSEX(WNDCLASSEX *wcex);
    int __cdecl hwritef(HANDLE handle, const char *format, ...);
    void WINAPI ErrorMessageBox(char *str, DWORD dwCode);
    void WINAPI NormalMessageBox(char *title, char *msg, UINT type);
    DWORD WINAPI GetExecuteFileName(DWORD length, char* path, char** file);
#ifdef _INC_COMMDLG
    OPENFILENAME* WINAPI Init_OPENFILENAME(OPENFILENAME *ofn);
#endif
#ifdef __cplusplus
}
#endif

#endif /* _INCLUDED_WINAPIEX */
/* vi:ts=8 sts=4 sw=4 tw=0:
 */
