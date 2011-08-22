// レジストリオブジェクト                                      since 1997.09.01
//	CReg.h:ClassRegistoryヘッダー                              Written by KoRoN
//                                                                 Version 1.00
//                                                                   1997.09.25
// Last Change: 00:30:54 25-Sep-2000.

#ifndef		CREG_INCLUDED

#define		CREG_SUCCESS			FALSE
#define		CREG_ERROR				TRUE


// ClassRegistory /////////////////////////////////////////////////////////1.00
class ClassRegistory
{
	public:
		long	GetErrorCode();
		long	GetValue(char *pszValueName, char *pszValue, DWORD dwSize);
		long	GetValue(char *pszValueName, DWORD *dwValue);
		long	SetValue(char *pszValueName, char *pszValue);
		long	SetValue(char *pszValueName, DWORD dwValue = 0);
		long	FlushKey();

		ClassRegistory(char* pszKeyName, HKEY hKey = HKEY_CURRENT_USER);
		~ClassRegistory();

	private:
		HKEY	hKey;
		long	nErrorCode;
};
//////////////////////////////////////////////KoRoN/1997.09.01/KoRoN/1997.09.25


//  ///////////////////////////////////////////////////////////////////////1.00
//////////////////////////////////////////////KoRoN/1997.09.01/KoRoN/1997.09.01


#define		CREG_INCLUDED
#endif
// vi:ts=4 sts=4 sw=4 tw=0
