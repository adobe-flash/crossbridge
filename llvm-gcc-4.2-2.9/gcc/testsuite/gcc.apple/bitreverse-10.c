/* APPLE LOCAL file */
/* { dg-do run { target powerpc*-*-darwin* } } */
/* { dg-options "-std=c99" } */
extern void abort();
typedef struct TestStateNorm
{
    unsigned int fBlah : 7;     //
    unsigned int fUnused : 2;                // Currently Unused
    unsigned int : 0;
    unsigned int fBlah2 : 7;     //
    unsigned int fUnused2: 2;                // Currently Unused
} TestStateNorm;

TestStateNorm v1norm = { 5, 1, 6, 2 };

union {
  TestStateNorm tsn;
  unsigned int raw[2];
} unorm = { 5, 1, 6, 2 };

#pragma reverse_bitfields on
#pragma ms_struct on
typedef struct TestState
{
    unsigned int fBlah : 7;     //
    unsigned int fUnused : 2;                // Currently Unused
    unsigned int : 0;
    unsigned int fBlah2 : 7;     //
    unsigned int fUnused2 : 2;                // Currently Unused
} TestState;

TestState v1 = { 5, 1, 6, 2 };

union {
  TestState ts;
  unsigned int raw[2];
} u = { 5, 1, 6, 2 };

int main(){
  if (v1norm.fBlah != 5) abort ();
  if (v1norm.fUnused != 1) abort ();
  if (v1norm.fBlah2 != 6) abort ();
  if (v1norm.fUnused2 != 2) abort ();

  if (u.ts.fBlah != 5) abort ();
  if (u.ts.fUnused != 1) abort ();
  if (u.ts.fBlah2 != 6) abort ();
  if (u.ts.fUnused2 != 2) abort ();

  if (unorm.tsn.fBlah != 5) abort ();
  if (unorm.tsn.fUnused != 1) abort ();
  if (unorm.tsn.fBlah2 != 6) abort ();
  if (unorm.tsn.fUnused2 != 2) abort ();

  if (v1.fBlah != 5) abort ();
  if (v1.fUnused != 1) abort ();
  if (v1.fBlah2 != 6) abort ();
  if (v1.fUnused2 != 2) abort ();

  if (unorm.raw[0] != 0x0a800000) abort();
  if (unorm.raw[1] != 0x0d000000) abort();
  if (u.raw[0] != 0x00000085) abort();
  if (u.raw[1] != 0x00000106) abort();
  return 0;
}
