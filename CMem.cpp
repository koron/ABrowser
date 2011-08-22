// メモリ制御オブジェクト                                      since 1997.08.29
//	CMem.cpp:CMemクラスコード                                  Written by KoRoN
//                                                                 Version 1.00
//                                                                   1997.08.28
// Last Change: 04-Feb-2002.

#include <memory.h>
#include "CMem.h"

// コンストラクタ /////////////////////////////////////////////////////////1.00
CMem::CMem(unsigned long ulSize)
{
	if (ulSize = 0)
		ptr = 0;
	else
		ptr = (void*) new char[ulSize];
	size = (ptr == 0)? 0 : ulSize;
}
//////////////////////////////////////////////KoRoN/1997.08.29/KoRoN/1997.08.29


// GetPointer /////////////////////////////////////////////////////////////1.00
	void*
CMem::GetPointer(unsigned long ulSize)
{
	return RequireMemory(ulSize);
}
//////////////////////////////////////////////KoRoN/1997.08.29/KoRoN/1997.08.29


// RequireMemory //////////////////////////////////////////////////////////1.00
	void*
CMem::RequireMemory(unsigned long ulSize)
{
	if (ulSize > size)
	{
		// メモリの内容は保持
		char *newptr = new char[ulSize];
		if (newptr)
		{
			memcpy(newptr, ptr, size);
			delete[] ptr;
			ptr = (void*)newptr;
			size = newptr ? ulSize : 0;
		}
		else
			return 0;
	}
	return ptr;
}
//////////////////////////////////////////////KoRoN/1997.08.29/KoRoN/1997.08.29


// デストラクタ ///////////////////////////////////////////////////////////1.00
CMem::~CMem()
{
	delete[] ptr;
}
//////////////////////////////////////////////KoRoN/1997.08.29/KoRoN/1997.08.29


//  ///////////////////////////////////////////////////////////////////////1.00
//////////////////////////////////////////////KoRoN/1997.08.29/KoRoN/1997.08.29
// vi:ts=4 sts=4 sw=4 tw=0:
