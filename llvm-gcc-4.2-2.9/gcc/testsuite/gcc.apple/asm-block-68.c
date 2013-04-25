/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin*} } */
/* { dg-options { -fasm-blocks } } */
/* <rdar://problem/7763139> */

#include <stdio.h>

#if !__x86_64__ && !__i386__
#error compile with -arch x86_64 or -arch i386
#endif

int main()
{
  float a = 1.0;
  float *aPtr = NULL;
  double b = 2.0;
  double *bPtr = NULL;
  
  __asm__
    {
#if __x86_64__
        lea     rax,a
	mov     aPtr,rax
	lea     rax,b
	mov     bPtr,rax
#elif __i386__
	lea     eax,a
	mov     aPtr,eax
	lea     eax,b
	mov     bPtr,eax
#endif
	}
  
  printf("a: %f, b: %lf\n", *aPtr, *bPtr);
  
  return 0;
}
