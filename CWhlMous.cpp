// ホイールマウス                                              since 1997.10.03
//	CWhlMous.cpp:ClassWhellMouseコード                         Written by KoRoN
//                                                                 Version 1.00
//                                                                   1997.10.03
// Last Change: 12:43:00 24-Sep-2000.

#define		WIN32_LEAN_AND_MEAN

#include	<windows.h>
#include	<zmouse.h>
#include	"CWhlMous.h"


// コンストラクタ /////////////////////////////////////////////////////////////
ClassWheelMouse::ClassWheelMouse()
{
	uMsgTarget		= RegisterWindowMessage("MSWHEEL_ROLLMSG");
	uMsgTranslate	= uMsgTarget;
}
//////////////////////////////////////////////KoRoN/1997.10.03/KoRoN/1997.10.03


// RegisterMessage ////////////////////////////////////////////////////////////
UINT ClassWheelMouse::Registe(UINT uMsg)
{
	if (uMsg == 0) goto END_REGISTER_MESSAGE;
	uMsgTranslate = uMsg;
END_REGISTER_MESSAGE:
	return uMsgTranslate;
}
//////////////////////////////////////////////KoRoN/1997.10.03/KoRoN/1997.10.03


// Translate //////////////////////////////////////////////////////////////////
UINT ClassWheelMouse::Translate(UINT& uMsg)
{
	uMsg = (uMsg == uMsgTarget) ? uMsgTranslate : uMsg;
	return uMsg;
}
//////////////////////////////////////////////KoRoN/1997.10.03/KoRoN/1997.10.03


//  ///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////KoRoN/1997.10.03/KoRoN/1997.10.03
// vi:ts=4 sts=4 sw=4 tw=0
