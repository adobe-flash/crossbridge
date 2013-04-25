/* APPLE LOCAL file 4163069 */
/* Test semantics of #pragma pack.
   Contributed by Mike Stump <mrs@apple.com>  */

/* { dg-do run { target *-*-darwin* } } */

#include <stdio.h>

#pragma pack(push, 1)
struct
{
  int a;
  short b;
} ShouldBeSixBytes;

#pragma pack(push, 8)
struct
{
  int a;
  short b;
} ShouldBeEightBytes;

/* Does a "push" in Apple's standard GCC, should here, too. */
#pragma pack(push, 4)
struct
{
  char a;
  short b;
} ShouldBeFourBytes;

/* Should be popped back to eight bytes.  */
#pragma pack()
struct
{
  int a;
  short b;
} ShouldBeEightBytesToo;

/* Should be popped back to one byte packing.  */
#pragma pack(pop)
struct
{
  char a;
  char b;
  char c;
} ShouldBeThreeBytes;

/* pop to native packing.  */
#pragma pack(pop)
struct
{
  char a;
  short b;
} ShouldBeFourBytesToo;   /* (well, four bytes on 32-bit PowerPC. YMMV.) */

int nerrs = 0;

static void
dotest (const char *name, int trueOrFalse)
{
  if (! trueOrFalse)
    ++nerrs;
  printf ("test %s: %s\n", name, trueOrFalse ? "passed" : "failed");
}

int main(void)
{
  dotest ("push 1", sizeof (ShouldBeSixBytes) == 6);
  dotest ("push 8", sizeof (ShouldBeEightBytes) == 8);
  dotest ("push 4", sizeof (ShouldBeFourBytes) == 4);
  dotest (" pop 8", sizeof (ShouldBeEightBytesToo) == 8);
  dotest (" pop 1", sizeof (ShouldBeThreeBytes) == 3);
  dotest (" pop n", sizeof (ShouldBeFourBytesToo) == 4);

  return nerrs;
}
