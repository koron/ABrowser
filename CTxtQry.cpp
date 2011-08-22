// Class Text Query                                            since 1997.09.24
//	CTxtQry.cpp:ClassTextQueryクラスコード                     Written by KoRoN
//                                                                 Version 1.00
//                                                                   1997.10.13

#include	"CTxtQry.h"


// コンストラクタ /////////////////////////////////////////////////////////1.00
ClassTextQuery::ClassTextQuery(){
	fQueryMode			= CTQ_MODE_NORMAL;
	nConclusionColumn	= 0;
	nConclusionLine		= 0;
	nOriginColumn		= 0;
	nOriginLine			= 0;

	nEndColumn			= 0;
	nEndLine			= 0;
	nStartColumn		= 0;
	nStartLine			= 0;
}
//////////////////////////////////////////////KoRoN/1997.09.24/KoRoN/1997.09.24


// IsInQuery //////////////////////////////////////////////////////////////1.00
int ClassTextQuery::IsInQuery( long nLine, long nColumn){
	switch( fQueryMode ){
	// ノーマルモードの場合
	case CTQ_MODE_NORMAL:
		// クエリー以前の判定
		if( nLine < nStartLine ) return 0;
		if( nLine == nStartLine && nColumn < nStartColumn ) return 0;
		// クエリー以降の判定
		if( nEndLine < nLine ) return 0;
		if( nLine == nEndLine && nEndColumn <= nColumn ) return 0;
		break;
	// ライナーモードの場合
	case CTQ_MODE_LINEAR:
	default:
		if( nLine < nStartLine || nLine > nEndLine ) return 0;
		break;
	}

	return -1;
}
//////////////////////////////////////////////KoRoN/1997.09.24/KoRoN/1997.09.24


// SetOrigin //////////////////////////////////////////////////////////////1.00
int ClassTextQuery::SetOrigin( long nLine, long nColumn ){
	nOriginLine		= nLine;
	nOriginColumn	= nColumn;
	return SetConclusion( nLine, nColumn );
}
//////////////////////////////////////////////KoRoN/1997.09.24/KoRoN/1997.09.24


// SetConclusion //////////////////////////////////////////////////////////1.00
int ClassTextQuery::SetConclusion( long nLine, long nColumn ){
	nConclusionLine		= nLine;
	nConclusionColumn	= nColumn;

	nStartLine		= nOriginLine;
	nStartColumn	= nOriginColumn;
	nEndLine		= nConclusionLine;
	nEndColumn		= nConclusionColumn;

	if( nStartLine > nEndLine ||
		(nStartLine == nEndLine && nStartColumn > nEndColumn) ){
		nStartLine		= nConclusionLine;
		nStartColumn	= nConclusionColumn;
		nEndLine		= nOriginLine;
		nEndColumn		= nOriginColumn;
	}

	return 0;
}
//////////////////////////////////////////////KoRoN/1997.09.24/KoRoN/1997.09.24


//  ///////////////////////////////////////////////////////////////////////1.00
//////////////////////////////////////////////KoRoN/1997.09.25/KoRoN/1997.09.25
