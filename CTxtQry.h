// Class Text Query                                            since 1997.09.24
//	CTxtQry.h:ClassTextQueryクラスヘッダー                     Written by KoRoN
//                                                                 Version 1.10
//                                                                   1997.10.13


#ifndef		CLASS_TEXTQUERY_INCLUDED

#define		CTQ_MODE_NORMAL				1
#define		CTQ_MODE_LINEAR				2

// ClassTextQuery /////////////////////////////////////////////////////////1.10
class ClassTextQuery {
public:
	int			IsInQuery( long nLine, long nColumn = 0 );
	int			SetConclusion( long nLine, long nColumn );
	int			SetOrigin( long nLine, long nColumn );

	ClassTextQuery();

protected:
	int			fQueryMode;

	long		nConclusionLine;
	long		nEndColumn;
	long		nEndLine;
	long		nStartColumn;
	long		nStartLine;

private:
	long		nConclusionColumn;
	long		nOriginColumn;
	long		nOriginLine;
};
//////////////////////////////////////////////KoRoN/1997.09.24/KoRoN/1997.10.13

#endif
#define		CLASS_TEXTQUERY_INCLUDED


//  ///////////////////////////////////////////////////////////////////////1.00
//////////////////////////////////////////////KoRoN/1997.09.24/KoRoN/1997.09.24
