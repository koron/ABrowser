/*
 * winapiex.c: Win32 API extention
 *
 * Last change: 10:11:04 15-Jul-2000.
 *
 * Copyright (C) 1999 2000, KoRoN
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>
#include <stdlib.h>
#include "winapiex.h"

/*
 * Init_BITMAPINFO
 */
    BITMAPINFO* WINAPI
Init_BITMAPINFO(BITMAPINFO *bi, int width, int height, int depth)
{
    int size;

    if (!bi)
	return NULL;
    if (width < 1 || height < 1)
	return NULL;
    switch (depth)
    {
	case 1:
	    size = (width + 7) >> 3;
	    break;
	case 4:
	    size = (width + 1) >> 1;
	    break;
	case 8:
	    size = width;
	    break;
	case 24:
	    size = width * 3;
	    break;
	case 32:
	    size = width * 4;
	    break;
	default:
	    return NULL;
    }

    ZeroMemory((PVOID)bi, sizeof(BITMAPINFOHEADER));
    bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi->bmiHeader.biWidth = width;
    bi->bmiHeader.biHeight = height;
    bi->bmiHeader.biPlanes = 1;
    bi->bmiHeader.biBitCount = depth;
    bi->bmiHeader.biCompression = BI_RGB;
    bi->bmiHeader.biSizeImage = ((size + 3) & ~3) * height;
    return bi;
}

/*
 * Init_OPENFILENAME
 */
    OPENFILENAME* WINAPI
Init_OPENFILENAME(OPENFILENAME *ofn)
{
    static char null_file[] = "";
    static char all_filter[] = "All Files (*.*)\0*.*\0\0";
    ZeroMemory((PVOID)ofn, sizeof(OPENFILENAME));
    ofn->lStructSize = sizeof(OPENFILENAME);
    ofn->lpstrFilter = all_filter;
    ofn->nMaxFile = MAX_PATH;
    return ofn;
}

/*
 * Init_WNDCLASSEX
 */
    WNDCLASSEX* WINAPI
Init_WNDCLASSEX(WNDCLASSEX *wcex)
{
    wcex->cbSize	= sizeof(WNDCLASSEX);
    wcex->style		= CS_OWNDC;
    wcex->lpfnWndProc	= NULL;
    wcex->cbClsExtra	= 0;
    wcex->cbWndExtra	= 0;
    wcex->hInstance	= NULL;
    wcex->hIcon		= LoadIcon(NULL, IDI_WINLOGO);
    wcex->hCursor	= LoadCursor(NULL, IDC_ARROW);
    wcex->hbrBackground	= NULL;
    wcex->lpszMenuName	= NULL;
    wcex->lpszClassName	= "KoRoN Window";
    wcex->hIconSm	= NULL;
    return wcex;
}

/*
 * hwritef
 */
#define HWRITEBUFFER 4000
    int __cdecl
hwritef(HANDLE handle, const char *format, ...)
{
    DWORD dwWrite, dwWritten;
    char buffer[HWRITEBUFFER];
    va_list args;
    if (!handle)
	return 0;
    va_start(args, format);
    dwWrite = wvsprintf((LPTSTR)buffer, (LPCTSTR)format, args);
    if (dwWrite >= HWRITEBUFFER)
    {
	NormalMessageBox("winapiex::hwritef",
		"Buffer overflow.", MB_ICONERROR);
	return dwWrite;
    }
    WriteFile(handle, buffer, dwWrite, &dwWritten, 0);
    return dwWritten;
}

/*
 * ErrorMessageBox
 */
    void WINAPI
ErrorMessageBox(char *str, DWORD dwCode)
{
    void *buffer;
    FormatMessage(
	    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
	    NULL, dwCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	    (LPTSTR)&buffer, 0, NULL);
    MessageBox(NULL, (LPCTSTR)buffer, str,
	    MB_OK | MB_APPLMODAL | MB_ICONERROR);
    LocalFree(buffer);
}

/*
 * NormalMessageBox
 */
    void WINAPI
NormalMessageBox(char *title, char *msg, UINT type)
{
    if(type == (UINT)NULL) type = MB_ICONINFORMATION;
    MessageBox(NULL, msg, title, MB_OK | MB_APPLMODAL | type);
}

/*
 * GetExecuteFileName
 *
 * Get current executing file name.
 */
    DWORD WINAPI
GetExecuteFileName(DWORD length, char* path, char** file)
{
    int len;
    char buf[MAX_PATH];
    char delimiter = ' ';
    char *cmdline = GetCommandLine();

    if (*cmdline == '"')
    {
	++cmdline;
	delimiter = '"';
    }
    for (len = 0; cmdline[len] && cmdline[len] != delimiter; ++len);
    if (len >= MAX_PATH)
	return 0;
    CopyMemory(buf, cmdline, len);
    buf[len] = '\0';
    return GetFullPathName(buf, length, path, file);
}


/*****************************************************************************
 * OpenGL extension                                                          *
 *****************************************************************************/

/*
 * Init_PIXELFORMATDESCRIPTOR
 */
    PIXELFORMATDESCRIPTOR* WINAPI
Init_PIXELFORMATDESCRIPTOR(PIXELFORMATDESCRIPTOR *pfd)
{
    /*ZeroMemory((PVOID)pfd, sizeof(PIXELFORMATDESCRIPTOR));*/
    pfd->nSize			= sizeof(PIXELFORMATDESCRIPTOR);
    pfd->nVersion		= 1;
    pfd->dwFlags		=
	PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd->iPixelType		= PFD_TYPE_RGBA;
    pfd->cColorBits		= 24;
    pfd->cRedBits		= 0;
    pfd->cRedShift		= 0;
    pfd->cGreenBits		= 0;
    pfd->cGreenShift		= 0;
    pfd->cBlueBits		= 0;
    pfd->cBlueShift		= 0;
    pfd->cAlphaBits		= 0;
    pfd->cAlphaShift		= 0;
    pfd->cAccumBits		= 0;
    pfd->cAccumRedBits		= 0;
    pfd->cAccumGreenBits	= 0;
    pfd->cAccumBlueBits		= 0;
    pfd->cAccumAlphaBits	= 0;
    pfd->cDepthBits		= 32;
    pfd->cStencilBits		= 8;
    pfd->cAuxBuffers		= 0;
    pfd->iLayerType		= PFD_MAIN_PLANE;
    pfd->bReserved		= 0;
    pfd->dwLayerMask		= 0;
    pfd->dwVisibleMask		= 0;
    pfd->dwDamageMask		= 0;
    return pfd;
}

/* vi:ts=8 sts=4 sw=4 tw=0
 */
