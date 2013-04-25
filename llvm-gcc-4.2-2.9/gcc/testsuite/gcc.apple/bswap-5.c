/* APPLE LOCAL file bswap support */
/* These cases should NOT generate bswap. */
/* { dg-do compile } */
/* { dg-options "-O0 -std=gnu99" } */

typedef unsigned int uint32;
typedef unsigned long long uint64;
extern void abort(void);
uint32 f (uint32) __attribute__((noinline));
uint64 llf (uint64) __attribute__((noinline));
unsigned int Reverse4 (volatile unsigned int) __attribute__((noinline));
uint32 Reverse5 (uint32) __attribute__((noinline));
unsigned int Reverse6 (unsigned int) __attribute__((noinline));
uint64 llReverse4 (volatile uint64) __attribute__((noinline));
unsigned long long llReverse5 (unsigned long long) __attribute__((noinline));
uint64 llReverse6 (uint64) __attribute__((noinline));

static int count = 0;
static int llcount = 0; 
uint32 f(uint32 x) { count++; return x; }
uint64 llf(uint64 x) { llcount++; return x; }

unsigned int Reverse4 (volatile unsigned int x) {   /* does NOT produce Rev */
  return ((x & 0x0000ff00) <<8) | ((x & 0x00ff0000) >> 8) | ((x & 0xff000000) >> 24) | ((x & 0x000000ff) << 24);
}
uint32 Reverse5 (uint32 x) {    /* does NOT produce Rev */
  return ((f(x) & 0x0000ff00) <<8) | ((f(x) & 0x00ff0000) >> 8) | ((f(x) & 0xff000000) >> 24) | ((f(x) & 0x000000ff) << 24);
}
unsigned int Reverse6 (unsigned int x) {
  return ((x & 0x0000ff00) <<8) | ((x & 0x0000ff00) >> 8) | ((x & 0xff000000) >> 24) | ((x & 0x000000ff) << 24);
}

uint64 llReverse4 (volatile uint64 x) {
  return ((x&0xFF00000000000000ull)>>56) | ((x&0x00FF000000000000ull)>>40) | ((x&0x0000FF0000000000ull)>>24) | ((x&0x000000FF00000000ull)>> 8) 
	| ((x&0x00000000FF000000ull)<< 8) | ((x&0x0000000000FF0000ull)<<24) | ((x&0x000000000000FF00ull)<<40) | ((x&0x00000000000000FFull)<<56);
}
unsigned long long llReverse5 (unsigned long long x) {
  return ((llf(x)&0xFF00000000000000ull)>>56) | ((llf(x)&0x00FF000000000000ull)>>40) | ((llf(x)&0x0000FF0000000000ull)>>24) | ((llf(x)&0x000000FF00000000ull)>> 8) 
	| ((llf(x)&0x00000000FF000000ull)<< 8) | ((llf(x)&0x0000000000FF0000ull)<<24) | ((llf(x)&0x000000000000FF00ull)<<40) | ((llf(x)&0x00000000000000FFull)<<56);
}
uint64 llReverse6 (uint64 x) {
  return ((x&0xFF00000000000000ull)>>56) | ((x&0x00FF000000000000ull)>>40) | ((x&0x0000FF0000000000ull)>>24) |  ((x&0x000000000000ff00ull)>> 8) 
	| ((x&0x00000000FF000000ull)<< 8) | ((x&0x0000000000FF0000ull)<<24) | ((x&0x000000000000FF00ull)<<40) | ((x&0x00000000000000FFull)<<56);
}
int main() {
  unsigned int x = 0x12345678;
  unsigned long long y = 0x123456789abcdef0ull;
  if (Reverse4(x) != 0x78563412)
    abort();
  if (Reverse5(x) != 0x78563412)
    abort();
  if (count != 4)
    abort();
  if (Reverse6(x) != 0x78560056)
    abort();
  if (llReverse4(y) != 0xf0debc9a78563412ull)
    abort();
  if (llReverse5(y) != 0xf0debc9a78563412ull)
    abort();
  if (llcount != 8)
    abort();
  if (llReverse6(y) != 0xf0debc9a005634deull)
    abort();
  return 0;
}

/* { dg-final { if [ istarget arm*-*-darwin* ] { scan-assembler-not "\\\trev" } } } */
/* { dg-final { if [ istarget i?86-*-darwin* ] { scan-assembler-not "\\\tbswap" } } } */

