// vi:set ts=4 sts=4 sw=4 tw=0:
//
// ClassTextFileクラス                                         since 1997.08.30
//	CTxtFile.cpp:クラスコード                                  Written by KoRoN
//                                                                 Version 1.00
// Last Change: 30-Mar-2002.

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winnls.h>
#include <stdio.h>
#include <crtdbg.h>
#include "CTxtFile.h"

#define READ_BUFFER_SIZE 65536

// コンストラクタ /////////////////////////////////////////////////////////1.00
ClassTextFile::ClassTextFile()
{
	cmLineAddress	= new CMem();
	cmLineColor		= new CMem();
	cmOutline		= new CMem();
	cmOutlineColor	= new CMem();
	cmTextBuf		= new CMem();
	dwColorMode		= CTF_COLORMODE_BLACKBACK;
	dwFileSize		= 0;
	dwFilter		= CTF_FILTER_DEFAULT;
	dwFilterValid	= CTF_FILTER_AUTO;
	dwReturnCR		= 0;
	dwReturnLF		= 0;
	nMaxColumn		= 0;
	nTabColumn		= CTF_TABCOLUMN_DEFAULT;
	nTextLine		= 0;
	nWrapColumn		= CTF_WRAPCOLUMN_DEFAULT;
	szFileName		= new char[MAX_PATH];
	lstrcpy(szFileName, "");
}
//////////////////////////////////////////////KoRoN/1997.08.30/KoRoN/1997.08.30


// デストラクタ ///////////////////////////////////////////////////////////1.00
ClassTextFile::~ClassTextFile()
{
	delete[] szFileName;
}
//////////////////////////////////////////////KoRoN/1997.09.05/KoRoN/1997.09.05


// Open ///////////////////////////////////////////////////////////////////1.00
int ClassTextFile::Open(char* szOpenFile, DWORD dwFilter)
{
	_RPT2(_CRT_WARN, "ClassTextFile::Open(szOpenFile=%s, dwFilter=%d)\n",
			szOpenFile, dwFilter);
	/* 現在出力中のファイルも読めるように */
	FILE *fp = fopen(szOpenFile, "rb");
	if (!fp)
	{
		_RPT1(_CRT_WARN, "  Failed fopen(%s)\n", szOpenFile);
		dwFileSize = 0;
		nTextLine	= 0;
		lstrcpy(szFileName, "");
		return 1;
	}
	else
	{
		int estimate_size = -1;

		/* ファイルサイズ取得 */
		if (fseek(fp, 0, SEEK_END) == 0)
		{
			estimate_size = ftell(fp);
			_RPT1(_CRT_WARN, "  ftell()=%d\n", estimate_size);
		}
		rewind(fp);

		if (estimate_size < 0)
		{
			char buf[READ_BUFFER_SIZE];
			int readed_size = 0;
			while (1)
			{
				int size = fread(buf, sizeof(buf[0]),
						sizeof(buf) / sizeof(buf[0]), fp);
				if (size <= 0)
					break;
				char *ptr = (char*)cmTextBuf->GetPointer(readed_size + size);
				if (!ptr)
					break;
				memcpy(ptr + readed_size, buf, size);
				readed_size += size;
				if (size < sizeof(buf) / sizeof(buf[0]))
					break;
			}
			dwFileSize = readed_size;
		}
		else
		{
			_RPT0(_CRT_WARN, "  Full block fread()\n");
			char *ptr = (char*)cmTextBuf->GetPointer(estimate_size);
			if (ptr)
				dwFileSize = fread(ptr, 1, estimate_size, fp);
		}
		fclose(fp);
		strcpy(szFileName, szOpenFile);
	}

	// 後処理
	UnfoldBackspace();
	Filter(dwFilter);
	CountLine();

	return 0;
}
//////////////////////////////////////////////KoRoN/1997.08.30/KoRoN/1997.08.31


// CountLine //////////////////////////////////////////////////////////////1.00
long ClassTextFile::CountLine(void)
{
	nTextLine	= 0;
	Analyze();
	Analyze();
	cmOutline->GetPointer(sizeof(char) * (nMaxColumn + 1));
	cmOutlineColor->GetPointer(sizeof(CTF_COLORDATA) * (nMaxColumn + 1));
	return 0;
}
//////////////////////////////////////////////KoRoN/1997.08.30/KoRoN/1997.08.30


// ChangeTabColumn ////////////////////////////////////////////////////////1.00
long ClassTextFile::ChangeTabColumn(long nRequestTab)
{
	if (nRequestTab == CTF_TABCOLUMN_GET)
		goto CHANGETABCOLUMN_END;
	nTabColumn = nRequestTab;
	CountLine();
CHANGETABCOLUMN_END:
	return nTabColumn;
}
//////////////////////////////////////////////KoRoN/1997.08.30/KoRoN/1997.08.30


// GenerateLine ///////////////////////////////////////////////////////////2.00
long ClassTextFile::GenerateLine(long nLine)
{
	if (!nTextLine) return -1;			// バッファが無かった場合

	char* ptrWrite = (char*) cmOutline->GetPointer();
	CTF_COLORDATA* pcd = (CTF_COLORDATA*) cmOutlineColor->GetPointer();

	// 要求された行が存在しない場合の処理
	if (nLine < 0 || nLine >= nTextLine)
	{
		*ptrWrite = '\0';
		pcd->nLength = 0;
		return -1;
	}

	char	*ptrRead= *(((char**)cmLineAddress->GetPointer()) + nLine);
	char	*ptrEnd	=
		(nLine + 1 >= nTextLine)
			? ((char*)cmTextBuf->GetPointer()) + dwFileSize
			: *(((char**)cmLineAddress->GetPointer()) + nLine + 1);
	int		fTab	= 0;
	int		fEnd	= 0;
	long	column	= 0;
	char	ch;
	CTF_COLORDATA	cd = * (
		((CTF_COLORDATA*) cmLineColor->GetPointer()) + nLine
		);
	cd.nLength = 0;

	while (!fEnd && ptrRead < ptrEnd)
	{
		// 行あふれ処理
		if ((nWrapColumn != CTF_WRAPCOLUMN_OFF) &&
								(column >= nWrapColumn))
		{
			fEnd = 2;
			continue;
		}

		// タブ処理
		if (fTab)
		{
			*ptrWrite++ = ' ';
			cd.nLength++;
			if (!(++column % nTabColumn)) fTab = 0;
			continue;
		}

		// データ解析下準備
		ch		= *ptrRead++;

		if (0 <= ch && ch <= 31 || ch == 0x7F)
		{
			switch (ch)
			{
			case '\r':
				if (*ptrRead != '\n') fEnd = 1;
				break;
			case '\n':
				fEnd = 1; break;
			case '\t':
				fTab = 1; break;
			case 0x1B:
				if (cd.nLength) *pcd++ = cd;
				ptrRead = UnfoldEscapeSeq(ptrRead - 1, &cd);
				cd.nLength = 0;
				break;
			}
		}else{
			*ptrWrite++ = ch;
			cd.nLength++;
			column++;
		}
	}

	if (cd.nLength) *pcd++ = cd;
	*pcd = cd;
	pcd->nLength = (fEnd != 1) ? -1 : 0;
	*ptrWrite = '\0';

	ColorFilter();

	return 0;
}
//////////////////////////////////////////////KoRoN/1997.08.30/KoRoN/1997.09.05


// UnfoldBackspace ////////////////////////////////////////////////////////1.00
void ClassTextFile::UnfoldBackspace()
{
	if (!dwFileSize)
		return;

	char	*ptrRead = (char*) cmTextBuf->GetPointer();
	char	*ptrWrite = ptrRead;
	char	ch, chPrev;
	DWORD	dwReadSize = 0, dwWriteSize = 0;

	while (dwReadSize < dwFileSize)
	{
		ch = *ptrRead++;
		dwReadSize++;
		chPrev = (dwWriteSize == 0)? '\n' : *(ptrWrite - 1);
		if (ch == '\b')
		{
			if (chPrev != '\r' && chPrev != '\n')
			{
				ptrWrite--;
				dwWriteSize--;
			}
		}else{
			*ptrWrite++ = ch;
			dwWriteSize++;
		}
	}

	dwFileSize = dwWriteSize;
}
//////////////////////////////////////////////KoRoN/1997.08.31/KoRoN/1997.08.31

/*
// ChangeWrapColumn ///////////////////////////////////////////////////////1.00
long ClassTextFile::ChangeWrapColumn(long nRequestWrap)
{
	if (nRequestWrap == CTF_WRAPCOLUMN_GET) goto CHANGEWRAPCOLUMN_END;
	if (nRequestWrap >= CTF_WRAPCOLUMN_MINIMUM)
		nRequestWrap = CTF_WRAPCOLUMN_DEFAULT;
	nWrapColumn = nRequestWrap;
	CountLine();
CHANGEWRAPCOLUMN_END:
	return nWrapColumn;
}
//////////////////////////////////////////////KoRoN/1997.08.31/KoRoN/1997.08.31
*/

// UnfoldEscapeSeq ////////////////////////////////////////////////////////2.00
char* ClassTextFile::UnfoldEscapeSeq(char *str, CTF_COLORDATA* cd)
{
	char		ch;

	if (*str != 0x1B) return str;
	if (*++str != '[') return str;
	char *esStart = ++str;
	while (!IsCharAlpha(ch = *str++)) if (0 <= ch && ch <= 31) return str - 1;

	switch (ch)
	{
	case 'm':
		int nColor = 0;{
			while ((ch = *esStart++) >= '0' && ch <= '9')
				nColor = nColor * 10 + (ch - '0');
		}
		if (nColor == 0)
		{
			cd->nLength = CTF_CD_LENGTH_DEFAUTL;
			cd->crForeground = CTF_CD_FOREGROUND_DEFAULT;
			cd->crBackground = CTF_CD_BACKGROUND_DEFAULT;
		}
		else if (30 <= nColor && nColor <= 37)
			cd->crForeground = nColor - 30;
		else if (40 <= nColor && nColor <= 47)
			cd->crBackground = nColor - 40;
		break;
	}

	return str;
}
//////////////////////////////////////////////KoRoN/1997.08.31/KoRoN/1997.09.05


// Analyze ////////////////////////////////////////////////////////////////1.00
// シーケンシャルテキストデータの解読
int ClassTextFile::Analyze(void)
{
	if (dwFileSize == 0) return nTextLine = 0;

	char**			ptrAdrs;
	CTF_COLORDATA*	ptrColor;

	char*	ptrRead		= (char*)	cmTextBuf->GetPointer();
	char*	ptrEnd		= ptrRead + dwFileSize;
	int		fNewLine	= 1;
	int		fKanji		= 0;
	int		fTab		= 0;
	long	line		= 0;
	long	column		= 0;
	unsigned char		ch, chNext;

	// カラーデータの初期設定
	CTF_COLORDATA		cd; {
		cd.nLength = CTF_CD_LENGTH_DEFAUTL;
		cd.crForeground = CTF_CD_FOREGROUND_DEFAULT;
		cd.crBackground = CTF_CD_BACKGROUND_DEFAULT;
	}

	// 改行コード判定用の変数の初期化
	dwReturnCR		= 0;
	dwReturnLF		= 0;

	nMaxColumn = 0;
	if (!nTextLine)
	{
		ptrAdrs = NULL;
		ptrColor = NULL;
	}else{
		ptrAdrs		= (char**)
			cmLineAddress->GetPointer(sizeof(char*) * nTextLine);
		ptrColor	= (CTF_COLORDATA*)
			cmLineColor->GetPointer(sizeof(CTF_COLORDATA) * nTextLine);
	}

	while (ptrRead < ptrEnd)
	{
		// ラップ処理
		if ((nWrapColumn != CTF_WRAPCOLUMN_OFF) &&
								(column >= nWrapColumn))
		{
			fNewLine = 1;
			ptrRead	-= fKanji;
		}

		// 改行処理
		if (fNewLine)
		{
			if (ptrAdrs)	*ptrAdrs++	= ptrRead;
			if (ptrColor)
			{
				cd.nLength = column;
				*ptrColor++	= cd;
			}
			fNewLine	= 0;
			fKanji		= 0;
			fTab		= 0;
			line++;
			column		= 0;
		}

		// タブ処理
		if (fTab)
		{
			if (!(++column % nTabColumn)) fTab = 0;
			if (column > nMaxColumn) nMaxColumn = column;
			continue;
		}

		// データ解析下準備
		ch = *ptrRead++;
		chNext = (ptrRead >= ptrEnd)? '\0' : *ptrRead;

		if (0 <= ch && ch <= 31 || ch == 0x7F)
		{
			fKanji = 0;
			switch (ch)
			{
			case '\r':
				dwReturnCR++;
				if (chNext != '\n') fNewLine = 1;
				break;
			case '\n':
				dwReturnLF++;
				fNewLine = 1; break;
			case '\t':
				fTab = 1; break;
			case 0x1B:
				ptrRead = UnfoldEscapeSeq(ptrRead - 1, &cd);
				break;
			}
		}else{
			column++;
			fKanji = (!fKanji && IsDBCSLeadByte(ch))? 1 : 0;
		}

		if (column > nMaxColumn) nMaxColumn = column;
	}

	return  nTextLine = line;
}
//////////////////////////////////////////////KoRoN/1997.09.05/KoRoN/1997.09.05


// FilterEUCtoSJIS ////////////////////////////////////////////////////////1.00
DWORD ClassTextFile::FilterEUCtoSJIS()
{
	if (!dwFileSize)
		return 0;

	char*	ptrRead		= (char*) cmTextBuf->GetPointer();
	char*	ptrWrite	= ptrRead;
	DWORD	dwReadSize	= 0;
	DWORD	dwWriteSize	= 0;
	int		fKanji		= 0; 
	unsigned char	ch, chPrev = '\0';

	while (dwReadSize < dwFileSize)
	{
		ch = *ptrRead++;
		dwReadSize++;

		if (!fKanji)
		{
			if (0xA1 <= ch && ch <= 0xFE)
			{
				fKanji = (ch & 0x01) ? 1 : 2;
				ch = (ch + ((ch <= 0xDD) ? 0x61 : 0xE2)) / 2;
			}
			else if (ch == 0x8e) // 半角カタカナ
			{
				ch = *ptrRead++;
				dwReadSize++;
			}
		}else{
			if (0xA1 <= ch && ch <= 0xFE)
			{
				if (fKanji == 1)
				{
					ch -= 0x61;
					if (0x7F <= ch) ch++;
				}else ch -= 0x02;
			}
			fKanji = 0;
		}

		*ptrWrite++ = ch;
		dwWriteSize++;
	}

	return dwFileSize = dwWriteSize;
}
//////////////////////////////////////////////KoRoN/1997.09.25/KoRoN/1997.09.25


// FilterAutoSelect ///////////////////////////////////////////////////////1.00
	DWORD
ClassTextFile::
FilterAutoSelect()
{
	if (!dwFileSize)
		return CTF_FILTER_DEFAULT;

	unsigned char	ch;
	unsigned char*	ptr	= (unsigned char*) cmTextBuf->GetPointer();
	DWORD	dwReadSize	= 0;
	DWORD	dwSJIS		= 0;
	DWORD	dwEUC		= 0;
	DWORD	dwNotJIS	= 0;
	DWORD	dwESC		= 0;
	DWORD	dwUni[2]	= {0, 0};
	DWORD	dwNotUni	= 0;
	DWORD	dwNotUTF8	= 0;
	DWORD	dwUTF8		= 0;
	DWORD	dwNumUTF8	= 0;
	int		ucs_le		= 0;
	int		ucs_be		= 0;
	int		offset = 0;

	// UCSファイルはサイズが2の倍数である。
	if (dwFileSize & 1)
		dwNotUni = 1;

	// BOMが存在する場合はLEかBEかチェックしておく。
	else if (dwFileSize > 1)
	{
		if (ptr[0] == 0xFF && ptr[1] == 0xFE)
			ucs_le = 1;
		else if (ptr[0] == 0xFE && ptr[1] == 0xFF)
			ucs_be = 1;
	}

	while (dwReadSize < dwFileSize)
	{
		ch = *ptr++;
		dwReadSize++;

		if (ch == '\0')
			++dwUni[offset & 1];
		if (ch == 0x1B)
			dwESC++;
		if (0x81 <= ch && ch <= 0x9F)
			dwSJIS++;
		if (0x80 <= ch && ch <= 0xFF)
			dwNotJIS++;
		if (0xA1 <= ch && ch <= 0xDF)
			dwEUC++;

		if (!dwNotUTF8)
		{
			if (ch == 0xFE || ch == 0xFF)
				dwNotUTF8 = 1;
			else if (dwUTF8 > 0)
			{
				if ((ch & 0xC0) == 0x80)
				{
					if (--dwUTF8 == 0)
						++dwNumUTF8;
				}
				else
					dwNotUTF8 = 1;
			}
			else if ((ch & 0xC0) == 0x80)
				dwNotUTF8 = 1;
			else if ((ch & 0xE0) == 0xC0)
				dwUTF8 = 1;
			else if ((ch & 0xF0) == 0xE0)
				dwUTF8 = 2;
			else if ((ch & 0xF8) == 0xF0)
				dwUTF8 = 3;
			else if ((ch & 0xFC) == 0xF8)
				dwUTF8 = 4;
			else if ((ch & 0xFE) == 0xFC)
				dwUTF8 = 5;
		}

		++offset;
	}

	// Is UNICODE ?
	if (!dwNotUni)
	{
		if (ucs_le || (!ucs_be && dwUni[0] < dwUni[1] / 10))
			return CTF_FILTER_UCS2LE;
		else if (ucs_be || (!ucs_le && dwUni[0] / 10 > dwUni[1]))
			return CTF_FILTER_UCS2BE;
	}

	// Is JIS or ANISI ?
	if (dwNotJIS == 0)
		if (dwESC > 0)
			return CTF_FILTER_JIS;
		else
			return CTF_FILTER_ANSI;

	// Is UTF-8 ?
	if (!dwNotUTF8 && dwUTF8 == 0 && dwNumUTF8 > 0)
		return CTF_FILTER_UTF8;

	if (dwSJIS >= dwNotJIS / 10)
		return CTF_FILTER_SJIS;
	if (dwNotJIS / 2 < dwEUC)
		return CTF_FILTER_EUC;

	return CTF_FILTER_BIN;
}
//////////////////////////////////////////////KoRoN/1997.09.25/KoRoN/1997.09.26


// Filter /////////////////////////////////////////////////////////////////1.00
	void
ClassTextFile::
Filter(DWORD dwFilterRequest)
{
	dwFilter = dwFilterRequest;
	if (dwFilter == CTF_FILTER_AUTO)
		dwFilter = FilterAutoSelect();

	switch (dwFilter)
	{
		case CTF_FILTER_UNICODE:
		case CTF_FILTER_UCS2LE:
		case CTF_FILTER_UCS2BE:
		case CTF_FILTER_UTF8:
			FilterUNICODEtoSJIS();
			break;
		case CTF_FILTER_JIS:
			FilterJIStoEUC();
		case CTF_FILTER_EUC:
			FilterEUCtoSJIS();
			break;
		default:
			break;
	}
}
//////////////////////////////////////////////KoRoN/1997.09.25/KoRoN/1997.09.25

	unsigned int
ClassTextFile::
utf8ptr_to_ucs4(unsigned char* ptr, int* len)
{
	if ((ptr[0] & 0x80) == 0x00)
	{
		if (len)
			*len = 1;
		return (unsigned int)(ptr[0] & 0x7F);
	}
	else if ((ptr[0] & 0xE0) == 0xC0)
	{
		if (len)
			*len = 2;
		return (unsigned int)(ptr[0] & 0x1F) << 6
			| (unsigned int)(ptr[1] & 0x3F);
	}
	else if ((ptr[0] & 0xF0) == 0xE0)
	{
		if (len)
			*len = 3;
		return (unsigned int)(ptr[0] & 0x0F) << 12
			| (unsigned int)(ptr[1] & 0x3F) << 6
			| (unsigned int)(ptr[2] & 0x3F);
	}
	else if ((ptr[0] & 0xF8) == 0xF0)
	{
		if (len)
			*len = 4;
		return (unsigned int)(ptr[0] & 0x07) << 18
			| (unsigned int)(ptr[1] & 0x3F) << 12
			| (unsigned int)(ptr[2] & 0x3F) << 6
			| (unsigned int)(ptr[3] & 0x3F);
	}
	else if ((ptr[0] & 0xFC) == 0xF8)
	{
		if (len)
			*len = 5;
		return (unsigned int)(ptr[0] & 0x03) << 24
			| (unsigned int)(ptr[1] & 0x3F) << 18
			| (unsigned int)(ptr[2] & 0x3F) << 12
			| (unsigned int)(ptr[3] & 0x3F) << 6
			| (unsigned int)(ptr[4] & 0x3F);
	}
	else if ((ptr[0] & 0xFE) == 0xFC)
	{
		if (len)
			*len = 6;
		return (unsigned int)(ptr[0] & 0x01) << 30
			| (unsigned int)(ptr[1] & 0x3F) << 24
			| (unsigned int)(ptr[2] & 0x3F) << 18
			| (unsigned int)(ptr[3] & 0x3F) << 12
			| (unsigned int)(ptr[4] & 0x3F) << 6
			| (unsigned int)(ptr[5] & 0x3F);
	}

	if (len)
		*len = 1;
	return (unsigned int)' ';
}

/*
 * UNICODEをSJISに変換する。APIを大量に使うので遅いけど簡単。
 */
	DWORD
ClassTextFile::
FilterUNICODEtoSJIS()
{
	if (!dwFileSize)
		return 0;

	unsigned char*	ptrRead		= (unsigned char*) cmTextBuf->GetPointer();
	unsigned char*	ptrWrite	= ptrRead;
	DWORD	dwReadSize	= 0;
	DWORD	dwWriteSize	= 0;
	int		endian_flag	= 0;

	// BOMでエンディアンをチェックし、スキップする
	if (dwFileSize > 1 &&
			(dwFilter == CTF_FILTER_UNICODE || dwFilter == CTF_FILTER_UCS2LE
			 || dwFilter == CTF_FILTER_UCS2BE))
	{
		// エンディアンの指定によらず、頭の
		if (ptrRead[0] == 0xFE && ptrRead[1] == 0xFF)
		{
			dwReadSize += 2;
			ptrRead += 2;
			endian_flag = 1; // BIG ENDIAN
		}
		else if (ptrRead[0] == 0xFF && ptrRead[1] == 0xFE)
		{
			dwReadSize += 2;
			ptrRead += 2;
			endian_flag = 0; // LITTLE ENDIAN
		}
	}
	else if (dwFileSize > 2 && dwFilter == CTF_FILTER_UTF8)
	{
		if (ptrRead[0] == 0xEF && ptrRead[1] == 0xBB && ptrRead[2] == 0xBF)
		{
			dwReadSize += 3;
			ptrRead += 3;
		}
	}

	// エンディアンが指定されていたときは強制設定
	if (dwFilter == CTF_FILTER_UCS2BE)
		endian_flag = 1;
	else if (dwFilter == CTF_FILTER_UCS2LE)
		endian_flag = 0;

	while (dwReadSize < dwFileSize - 1)
	{
		int len = 2;
		char buf[3];
		unsigned short readBuf;

		if (dwFilter == CTF_FILTER_UTF8)
			readBuf = (unsigned short)utf8ptr_to_ucs4(ptrRead, &len);
		else
			readBuf = endian_flag ?
				(unsigned short)ptrRead[1] | (unsigned short)ptrRead[0] << 8
				:(unsigned short)ptrRead[0] | (unsigned short)ptrRead[1] << 8;
		dwReadSize += len;
		ptrRead += len;

		len = WideCharToMultiByte(CP_ACP, NULL,
				&readBuf, 1, buf, sizeof(buf), NULL, NULL);
		memcpy(ptrWrite, buf, len);
		ptrWrite += len;
		dwWriteSize += len;
	}

	if (dwFilter == CTF_FILTER_UNICODE)
		dwFilter = endian_flag ? CTF_FILTER_UCS2BE : CTF_FILTER_UCS2LE;
	return dwFileSize = dwWriteSize;
}

// FilterJIStoEUC /////////////////////////////////////////////////////////1.10
DWORD ClassTextFile::FilterJIStoEUC()
{
	if (!dwFileSize)
		return 0;

	char*	ptrRead		= (char*) cmTextBuf->GetPointer();
	char*	ptrWrite	= ptrRead;
	DWORD	dwReadSize	= 0;
	DWORD	dwWriteSize	= 0;
	int		fKanji		= 0;
	unsigned char	ch, chNext, chNext2;

	while (dwReadSize < dwFileSize)
	{
		ch		= *ptrRead++;
		chNext	= *ptrRead;
		chNext2	= (++dwReadSize < dwFileSize) ? *(ptrRead + 1) : 0;

		switch (ch)
		{
		case 0x1B:
			if (chNext == 0x24 && chNext2 == 0x42)
			{
				fKanji = -1;
				ptrRead += 2;
				dwReadSize += 2;
				break;
			}
			else if (chNext == 0x28 &&
								(0x40 <= chNext2 && chNext2 <= 0x7E))
			{
				fKanji = 0;
				ptrRead += 2;
				dwReadSize += 2;
				break;
			}
		default:
			if (fKanji && ch >= ' ')
				ch |= 0x80;
			*ptrWrite++ = ch;
			dwWriteSize++;
		}
	}

	return dwFileSize = dwWriteSize;
}
//////////////////////////////////////////////KoRoN/1997.09.25/KoRoN/1997.10.03


// GetReturnCode //////////////////////////////////////////////////////////1.00
int ClassTextFile::GetReturnCode()
{
	double		dblSum = dwReturnCR + dwReturnLF;

	if (!dblSum)
		return CTF_RETURNCODE_NONE;

	if (dwReturnCR / dblSum < .1)
		return CTF_RETURNCODE_LF;
	if (dwReturnLF / dblSum < .1)
		return CTF_RETURNCODE_CR;

	dblSum = (double)dwReturnCR / dwReturnLF;
	if (.8 < dblSum && dblSum < 1.2f)
		return CTF_RETURNCODE_CRLF;
	return CTF_RETURNCODE_MIX;
}
//////////////////////////////////////////////KoRoN/1997.10.09/KoRoN/1997.10.09


// ColorFilter ////////////////////////////////////////////////////////////1.00
COLORREF	crTable[2][8] = {
	{
		PALETTEINDEX(0), PALETTEINDEX(13),
		PALETTEINDEX(14), PALETTEINDEX(15),
		PALETTEINDEX(16), PALETTEINDEX(17),
		PALETTEINDEX(18), PALETTEINDEX(19)
	},
	{
		PALETTEINDEX(19), PALETTEINDEX(13),
		PALETTEINDEX(2), PALETTEINDEX(3),
		PALETTEINDEX(16), PALETTEINDEX(17),
		PALETTEINDEX(6), PALETTEINDEX(0)
	}
};

void ClassTextFile::ColorFilter()
{
	CTF_COLORDATA* pcd = (CTF_COLORDATA*) cmOutlineColor->GetPointer();

	do
	{
		pcd->crForeground = crTable[dwColorMode][pcd->crForeground];
		pcd->crBackground = crTable[dwColorMode][pcd->crBackground];
	}
	while ((pcd++)->nLength > 0);

}
//////////////////////////////////////////////KoRoN/1997.10.12/KoRoN/1997.10.12


//  ///////////////////////////////////////////////////////////////////////1.00
//////////////////////////////////////////////KoRoN/1997.10.12/KoRoN/1997.10.12
