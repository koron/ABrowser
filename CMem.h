// メモリ制御オブジェクト                                      since 1997.08.29
//	CMem.h:Memoryクラスヘッダー                                Written by KoRoN
//                                                                 Version 1.00
//                                                                   1997.08.29
// Last Change: 15:46:18 28-Sep-2000.

#ifndef CMEM_INCLUDED


// クラスCMem /////////////////////////////////////////////////////////////1.00
class CMem
{
	public:
		void* GetPointer(unsigned long ulSize = 0);
		CMem(unsigned long ulSize = 0);
		~CMem();
	private:
		void* RequireMemory(unsigned long ulSize);
		unsigned long size;
		void* ptr;
};
//////////////////////////////////////////////KoRoN/1997.08.29/KoRoN/1997.08.29


//  ///////////////////////////////////////////////////////////////////////1.00
//////////////////////////////////////////////KoRoN/1997.08.29/KoRoN/1997.08.29

#define	CMEM_INCLUDED
#endif
// vi:ts=4 sts=4 sw=4 tw=0:
