// コマンドラインオブジェクト                                  since 1997.09.01
//	CCmdLine.h:コマンドラインクラスヘッダー                    Written by KoRoN
//                                                                 Version 1.00
//                                                                   1997.09.01

#ifndef		CCMDLINE_INCLUDED

// ClassCommandLine ///////////////////////////////////////////////////////1.00
class ClassCommandLine {
public:
	int		Argc();
	char*	Argv( int argc );

	ClassCommandLine();
	~ClassCommandLine();
private:
	int		Analyze();

	int		nArgc;
	char**	ppszArgv;
	char*	szCommandLine;
};
//////////////////////////////////////////////KoRoN/1997.09.01/KoRoN/1997.09.01


//  ///////////////////////////////////////////////////////////////////////1.00
//////////////////////////////////////////////KoRoN/1997.09.01/KoRoN/1997.09.01


#define		CCMDLINE_INCLUDED
#endif
