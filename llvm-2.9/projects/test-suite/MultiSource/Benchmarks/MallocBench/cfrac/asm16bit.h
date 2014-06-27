/*
 * HP-UX C compiler conventions
 *
 * Args pushed right-to-left; caller pops args on return
 * Function result returned in d0 or d0(msb) d1(lsb) pair
 * Called function must preserve all registers except d0,d1,a0,a1
 * C Registers are allocated from top-to-bottem in text from d7-d2, a5-a2
 */ 
#ifdef __STDC__
extern digit memaddw(digitPtr, digitPtr, digitPtr, posit);
extern digit memsubw(digitPtr, digitPtr, digitPtr, posit);

extern digit memincw(digitPtr, accumulator);
extern digit memdecw(digitPtr, accumulator);

extern digit memmulw(digitPtr, digitPtr, posit, digitPtr, posit);

extern digit memdivw(digitPtr, digitPtr, posit, digitPtr);
extern digit memdivw1(digitPtr, digitPtr, posit, digit);
extern digit memmulw1(digitPtr, digitPtr, posit, digit);
extern digit memmodw1(digitPtr, posit, digit);

extern void  memlsrw(digitPtr, posit);
#else
extern digit memaddw();
extern digit memsubw();

extern digit memincw();
extern digit memdecw();

extern digit memmulw();

extern digit memdivw();
extern digit memdivw1();
extern digit memmulw1();
extern digit memmodw1();

extern void  memlsrw();
#endif
