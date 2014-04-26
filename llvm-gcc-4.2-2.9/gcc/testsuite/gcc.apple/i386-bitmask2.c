/* APPLE LOCAL file radar 4645709 */
/* { dg-do compile { target "i?86-*-*" } } */
/* { dg-options "-Oz" } */
/* { dg-final { scan-assembler-not "and.*0xffffff00" } } */
unsigned char lut[256];

void foo( int count )
{
  int j;
	
  unsigned int *srcptr, *dstptr;
  for (j = 0; j < count; j++) {
    unsigned int tmp = *srcptr;
    unsigned int alpha = (tmp&255);
    tmp &= 0xffffff00;
    alpha =lut[alpha];
    tmp |= alpha<<0;
    *dstptr = tmp;
  }
}
/* APPLE LOCAL file radar 4645709 */

