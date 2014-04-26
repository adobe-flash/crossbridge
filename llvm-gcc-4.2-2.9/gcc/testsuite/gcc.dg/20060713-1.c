/* APPLE LOCAL file 4538899 mainline candidate */
/* { dg-do compile { target { powerpc*-*-* && lp64 } } } */
/* { dg-do compile { target { powerpc*-apple-* } } } */
/* { dg-options "-fPIC -m64" } */
char *ptr = 0;
char array[100];
void
f()
{
  ptr = &array[0x100000000ULL];	/* A 33-bit constant.  */
}
