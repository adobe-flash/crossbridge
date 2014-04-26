/* { dg-do run { target *-*-darwin* } } */
/* { dg-options "-O2 -fss-const-prop" } */
/* APPLE LOCAL file bitfield zero initialization */
/* <rdar://problem/3366203>:Setup Assistant crashes in SetModificationDateToPasteboard */
/* <rdar://problem/3379022>:GCC compiler error in bitfield handling */


typedef struct
{
  unsigned int  b01 : 1;
  unsigned int  b02 : 2;
  unsigned int  b03 : 1;
  unsigned int  b04 : 1;
  unsigned int  b05 : 1;
  unsigned int  b06 : 1;
  unsigned int  b07 : 3;
  unsigned int  b08 : 3;
  unsigned int  b09 : 3;
  unsigned int  b10 : 3;
  unsigned int  b11 : 8;
  unsigned int  b12 : 1;
  unsigned int  b13 : 4;
} test_struct;

void func( test_struct* s, int t )
{
  s->b12 = 0;
  s->b02 = 0;
}

main()
{
  test_struct r = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

  func (&r, 42);

  if (r.b01 != 1
      || r.b03 != 1
      || r.b04 != 1
      || r.b05 != 1
      || r.b06 != 1
      || r.b07 != 1
      || r.b08 != 1
      || r.b09 != 1
      || r.b10 != 1
      || r.b11 != 1
      || r.b13 != 1)
    {
      abort();
    }
  exit (0);
}
