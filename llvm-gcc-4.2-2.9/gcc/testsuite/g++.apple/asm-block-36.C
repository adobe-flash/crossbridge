/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* APPLE LOCAL x86_64 */
/* { dg-require-effective-target ilp32 } */
/* { dg-options { -fasm-blocks -msse3 } } */
/* Radar 4371958 */

#define SourceArray(row, col)  (sPtr [(row) * sRowBytes + (col)])
void foo() {
  char *sPtr[20];
  int sRowBytes = 1;
  SourceArray(0, 0);
  asm {
    nop
    ;           w2 = SourceArray(rowIndex + 1, colIndex * sColBytes) +
    nop
  }
}

void
RefBNDecimateH1() { }
#define DoOldDecimateH RefBNDecimateH1
void X_DecimateH (int cols) {
  if ( cols < 8 )
    {
      DoOldDecimateH ();
      return;
    }
  asm { call DoOldDecimateH }
}
