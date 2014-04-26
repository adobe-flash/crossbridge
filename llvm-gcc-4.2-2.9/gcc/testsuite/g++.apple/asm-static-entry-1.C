/* APPLE LOCAL file CW asm blocks */
/* { dg-do run { target powerpc*-*-darwin* } } */
/* { dg-options "-fasm-blocks" } */
/* Radar 4211430 */

#include <stdio.h>

extern "C" void abort();

typedef unsigned long UInt32;

#define FUNCPTR_TO_UINT(f) ((UInt32)(&(f)))


/*
 *  CPP versions
 */ 
class TestAsmClass
{
    public:
        static UInt32  DoAsmCodeTest_CPP();

        static void  flush_cache_line_CPP(unsigned long inCacheLine);
        static void  flush_cache_line_CPP_end();
};

asm void TestAsmClass::flush_cache_line_CPP(
 register unsigned long inCacheLine)
{
 dcbst 0, r3
 sync

 icbi 0, r3
 isync
entry static flush_cache_line_CPP_end;
}

UInt32 TestAsmClass::DoAsmCodeTest_CPP()
{
 UInt32 mw_size = 0;

 mw_size = FUNCPTR_TO_UINT(flush_cache_line_CPP);
 mw_size = FUNCPTR_TO_UINT(flush_cache_line_CPP_end) - mw_size;

 return mw_size;
}

int main()
{
	TestAsmClass a;
	if (a.DoAsmCodeTest_CPP() != 16)
	  abort();
	return 0;
}
