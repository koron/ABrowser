/* vi:set ts=4 sts=4 sw=4 tw=0: */
// コマンドラインオブジェクト                                  since 1997.09.01
//	CCmdLine.cpp:コマンドラインクラスコード                    Written by KoRoN
//                                                                 Version 1.00
// Last Change: 19:27:23 04-May-2001.

#define		WIN32_LEAN_AND_MEAN

#include	<windows.h>
#include	"CCmdLine.h"


// コマンドライン /////////////////////////////////////////////////////////1.00
ClassCommandLine::ClassCommandLine()
{
	// szCommandLineの初期化
	char	*pszCommandLine = GetCommandLine();
	szCommandLine = new char[lstrlen(pszCommandLine) + 1];
	lstrcpy(szCommandLine, pszCommandLine);
	// その他の変数を初期化
	ppszArgv = NULL;
	Analyze();
	ppszArgv = new char*[nArgc];
	Analyze();
}
//////////////////////////////////////////////KoRoN/1997.09.01/KoRoN/1997.09.02


// デストラクタ ///////////////////////////////////////////////////////////1.00
ClassCommandLine::~ClassCommandLine()
{
	delete[] szCommandLine;
	delete[] ppszArgv;
}
//////////////////////////////////////////////KoRoN/1997.09.01/KoRoN/1997.09.02


// Analyze ////////////////////////////////////////////////////////////////1.00
int ClassCommandLine::Analyze()
{
	bool	fQuate		= false;
	bool	fHunk		= false;
	char*	pch			= szCommandLine;
	char**	ppszArgv	= ClassCommandLine::ppszArgv;
	char	ch;
	nArgc				= 0;

	while (ch = *pch)
	{
		if (!fHunk)
		{
			switch (ch)
			{
			case '"':
				fHunk = true;
				fQuate = true;
				nArgc++;
				if (ppszArgv)
					*ppszArgv++ = pch + 1;
				break;
			case '\t':
			case ' ':
				break;
			default:
				fHunk = true;
				fQuate = false;
				nArgc++;
				if (ppszArgv)
					*ppszArgv++ = pch;
				break;
			}
		}
		else
		{
			if ((!fQuate && ch == ' ') || (fQuate && ch == '"'))
			{
				fHunk = false;
				fQuate = false;
				if (ppszArgv)
					*pch = '\0';
			}
		}
		++pch;
	}

	return nArgc;
}
//////////////////////////////////////////////KoRoN/1997.09.02/KoRoN/1997.09.02


// Argv ///////////////////////////////////////////////////////////////////1.00
char* ClassCommandLine::Argv(int argc)
{
	return (argc >= nArgc) ? NULL : *(ppszArgv + argc);

}
//////////////////////////////////////////////KoRoN/1997.09.02/KoRoN/1997.09.02


// Argc ///////////////////////////////////////////////////////////////////1.00
int ClassCommandLine::Argc()
{
	return nArgc;
}
//////////////////////////////////////////////KoRoN/1997.09.02/KoRoN/1997.09.02


//  ///////////////////////////////////////////////////////////////////////1.00
//////////////////////////////////////////////KoRoN/1997.09.02/KoRoN/1997.09.02
