// レジストリオブジェクト                                      since 1997.09.01
//	CReg.cpp:ClassRegistoryコード                              Written by KoRoN
//                                                                 Version 1.00
//                                                                   1997.09.01
// Last Change: 22-Aug-2011.

#define		WIN32_LEAN_AND_MEAN

#include	<windows.h>
#include	"CReg.h"


// コンストラクタ /////////////////////////////////////////////////////////1.00
ClassRegistory::ClassRegistory(char* pszKeyName, HKEY hKey)
{
	DWORD	dwDisposition;

	nErrorCode = RegCreateKeyEx(
		hKey,
		pszKeyName,
		NULL,
		"",
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&(ClassRegistory::hKey),
		&dwDisposition
		);
	nErrorCode = NULL;
}
//////////////////////////////////////////////KoRoN/1997.09.01/KoRoN/1997.09.01


// デストラクタ ///////////////////////////////////////////////////////////1.00
ClassRegistory::~ClassRegistory()
{
	RegCloseKey(hKey);
}
//////////////////////////////////////////////KoRoN/1997.09.01/KoRoN/1997.09.01


// GetErrorCode ///////////////////////////////////////////////////////////1.00
long ClassRegistory::GetErrorCode()
{
	return nErrorCode;
}
//////////////////////////////////////////////KoRoN/1997.09.01/KoRoN/1997.09.01


// SetValue(DWORD) ////////////////////////////////////////////////////////1.01
	long
ClassRegistory::SetValue(char *pszValueName, DWORD dwValue)
{
	nErrorCode = RegSetValueEx(
		hKey,
		pszValueName,
		0,
		REG_DWORD,
		(CONST BYTE *)&dwValue,
		sizeof(DWORD)
		);
	return nErrorCode == ERROR_SUCCESS ? CREG_SUCCESS : CREG_ERROR;
}
//////////////////////////////////////////////KoRoN/1997.09.01/KoRoN/1997.09.01


// GetValue(DWORD) ////////////////////////////////////////////////////////1.00
	long
ClassRegistory::GetValue(char *pszValueName, DWORD *dwValue)
{
	DWORD	dwType = REG_DWORD;
	DWORD	dwSize = sizeof(DWORD);

	nErrorCode = RegQueryValueEx(
		hKey,
		pszValueName,
		NULL,
		&dwType,
		(LPBYTE)dwValue,
		&dwSize
		);
	if (nErrorCode != ERROR_SUCCESS)
		return CREG_ERROR;
	return CREG_SUCCESS;
}
//////////////////////////////////////////////KoRoN/1997.09.01/KoRoN/1997.09.01


// SetValue(char*) ////////////////////////////////////////////////////////1.00
long ClassRegistory::SetValue(char *pszValueName, char *pszValue)
{
	nErrorCode = RegSetValueEx(
		hKey,
		pszValueName,
		0,
		REG_SZ,
		(CONST BYTE *)pszValue,
		lstrlen(pszValue)
		);
	if (nErrorCode != ERROR_SUCCESS) return CREG_ERROR;
	return CREG_SUCCESS;
}
//////////////////////////////////////////////KoRoN/1997.09.01/KoRoN/1997.09.01


// GetValue(char*) ////////////////////////////////////////////////////////1.00
long ClassRegistory::GetValue(char *pszValueName,
										char *pszValue, DWORD dwSize)
{
	DWORD	dwType = REG_SZ;

	nErrorCode = RegQueryValueEx(
		hKey,
		pszValueName,
		NULL,
		&dwType,
		(LPBYTE)pszValue,
		&dwSize
		);
	if (nErrorCode != ERROR_SUCCESS) return CREG_ERROR;
	return CREG_SUCCESS;
}

// GetValueLength() ///////////////////////////////////////////////////////1.00
long ClassRegistory::GetValueLength(char *pszValueName, DWORD *dwSize)
{
	DWORD	dwType = REG_SZ;

	nErrorCode = RegQueryValueEx(
		hKey,
		pszValueName,
		NULL,
		&dwType,
		(LPBYTE)NULL,
		dwSize
		);
	if (nErrorCode != ERROR_SUCCESS) return CREG_ERROR;
	return CREG_SUCCESS;
}
//////////////////////////////////////////////KoRoN/1997.09.01/KoRoN/1997.09.01


// Flush() ////////////////////////////////////////////////////////////////1.00
	long
ClassRegistory::FlushKey()
{
	return ::RegFlushKey(hKey);
}
//////////////////////////////////////////////KoRoN/2000.09.24/KoRoN/2000.09.24


//  ///////////////////////////////////////////////////////////////////////1.00
//////////////////////////////////////////////KoRoN/2000.09.24/KoRoN/2000.09.24
// vi:ts=4 sts=4 sw=4 tw=0
