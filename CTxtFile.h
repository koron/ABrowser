// vi:set ts=4 sts=4 sw=4 tw=0:
// ClassTextFileクラス                                         since 1997.08.30
//	CTxtFile.cpp:クラスヘッダー                                Written by KoRoN
//                                                                 Version 1.00
//                                                                   1997.10.03

#ifndef		CLASS_TEXTFILE_INCLUDED


#include	"CMem.h"


#define		CTF_FILTER_GET				0x00000000
#define		CTF_FILTER_AUTO				0x00000001
#define		CTF_FILTER_BIN				0x00000002
#define		CTF_FILTER_ANSI				0x00000004
#define		CTF_FILTER_SJIS				0x00000008
#define		CTF_FILTER_EUC				0x00000010
#define		CTF_FILTER_JIS				0x00000020
#define		CTF_FILTER_UNICODE			0x00000040
#define		CTF_FILTER_UCS2LE			0x00000080
#define		CTF_FILTER_UCS2BE			0x00000100
#define		CTF_FILTER_UTF8				0x00000200
#define		CTF_FILTER_DEFAULT			CTF_FILTER_AUTO

#define		CTF_TABCOLUMN_GET			0
#define		CTF_TABCOLUMN_DEFAULT		4

#define		CTF_WRAPCOLUMN_GET			-1
#define		CTF_WRAPCOLUMN_OFF			0
#define		CTF_WRAPCOLUMN_DEFAULT		80
#define		CTF_WRAPCOLUMN_MINIMUM		80

#define		CTF_COLOR_DEFAULT			0x0F
#define		CTF_CD_FOREGROUND_DEFAULT	7
#define		CTF_CD_BACKGROUND_DEFAULT	0
#define		CTF_CD_LENGTH_DEFAUTL		0

#define		CTF_RETURNCODE_MIX			-1
#define		CTF_RETURNCODE_NONE			0
#define		CTF_RETURNCODE_CR			1
#define		CTF_RETURNCODE_LF			2
#define		CTF_RETURNCODE_CRLF			3

#define		CTF_COLORMODE_TOGLE			0xFFFFFFFF
#define		CTF_COLORMODE_BLACKBACK		0x00000000
#define		CTF_COLORMODE_WHITEBACK		0x00000001

#define		CTF_ANSI_TO_RGB( color )\
	PALETTEINDEX( ((color) == 0 )? 0 : (color) + 12 )


// CTF_COLORDATA //////////////////////////////////////////////////////////1.00
typedef struct _CTF_COLORDATA
{
	long		nLength;
	COLORREF	crForeground;
	COLORREF	crBackground;
} CTF_COLORDATA;
//////////////////////////////////////////////KoRoN/1997.09.05/KoRoN/1997.09.05


// ClassTextFile //////////////////////////////////////////////////////////1.00
class ClassTextFile
{
public:
	long	ChangeTabColumn( long nRequestTab = CTF_TABCOLUMN_GET );
//	long	ChangeWrapColumn( long nRequestWrap = CTF_WRAPCOLUMN_GET );
	long	GenerateLine( long nLine );
	int		GetReturnCode();
	int		Open( char* szOpenFile, DWORD dwFilter = CTF_FILTER_AUTO );

	ClassTextFile();
	~ClassTextFile();

protected:
	CMem*	cmOutline;
	CMem*	cmOutlineColor;
	DWORD	dwColorMode;
	DWORD	dwFileSize;
	DWORD	dwFilter;
	long	nMaxColumn;
	long	nTextLine;
	char*	szFileName;

private:
	unsigned int utf8ptr_to_ucs4(unsigned char* ptr, int *len = NULL);
	int		Analyze( void );
	void	ColorFilter();
	long	CountLine( void );
	void	Filter( DWORD dwFilterRequest = CTF_FILTER_AUTO );
	DWORD	FilterAutoSelect();
	DWORD	FilterUNICODEtoSJIS();
	DWORD	FilterEUCtoSJIS();
	DWORD	FilterJIStoEUC();
	void	UnfoldBackspace();
	char*	UnfoldEscapeSeq( char* str, CTF_COLORDATA *cd );

	CMem*	cmLineAddress;
	CMem*	cmLineColor;
	CMem*	cmTextBuf;
	DWORD	dwFilterValid;
	DWORD	dwReturnCR;
	DWORD	dwReturnLF;
	long	nTabColumn;
	long	nWrapColumn;
};
//////////////////////////////////////////////KoRoN/1997.08.30/KoRoN/1997.09.05


//  ///////////////////////////////////////////////////////////////////////1.00
//////////////////////////////////////////////KoRoN/1997.09.05/KoRoN/1997.09.05


#define		CLASS_TEXTFILE_INCLUDED
#endif
