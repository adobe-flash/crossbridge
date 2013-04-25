/* APPLE LOCAL begin radar 4120689 */
/* { dg-do compile { target i?86-*-* } } */
/* { dg-options "-O1 -msse2" } */
void idct_sse2as(void)
{
    int i;
    short dst[64]  __attribute__((aligned(16))) ;

    __asm__ __volatile__ ("movdqa" " %%" "xmm1" ", %0" : "=m" (*(dst + 0*8)) : );
}

int main()
{
    idct_sse2as();
    return 0;
}
/* APPLE LOCAL end radar 4120689 */

