// ホイールマウス                                              since 1997.10.03
//	CWhlMous.h:ClassWhellMouseヘッダー                         Written by KoRoN
//                                                                 Version 1.00
//                                                                   1997.10.03


#ifndef		CLASS_WHEELMOUSE_INCLUDED

// ClassWheelMouse ////////////////////////////////////////////////////////////
class ClassWheelMouse{
public:
	UINT	Translate( UINT& uMsg );
	UINT	Registe( UINT uMsgRequest = 0 );

	ClassWheelMouse();

protected:

private:
	UINT	uMsgTarget;
	UINT	uMsgTranslate;
};
//////////////////////////////////////////////KoRoN/1997.10.03/KoRoN/1997.10.03

#define		CLASS_WHEELMOUSE_INCLUDED
#endif


//  ///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////KoRoN/1997.10.03/KoRoN/1997.10.03
