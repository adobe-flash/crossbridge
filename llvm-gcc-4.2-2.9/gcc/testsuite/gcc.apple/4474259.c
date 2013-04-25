/* APPLE LOCAL file 4474259 */
/* { dg-do run { target i*86-*-darwin* } } */
/* { dg-options "-fasm-blocks -O2" } */
/* Insure the compiler output is acceptable to the assembler.  */
t1()
{
  int   rowfraclo[2];
  int   colfraclo[2];
  _asm
    {
      movd    rowfraclo, mm6
      movd    colfraclo, mm3
    }
}
main ()
{
  return 0;
}
