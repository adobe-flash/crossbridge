/* APPLE LOCAL file bswap support */
/* { dg-do run } */
/* { dg-options "-O2 -std=gnu99" } */

extern void abort(void);
typedef unsigned int uint32;
typedef unsigned long long uint64;
uint32 Reverse1 (uint32) __attribute__((noinline));
uint32 Reverse2 (uint32) __attribute__((noinline));
unsigned int Reverse3 (unsigned int) __attribute__((noinline));
uint64 llReverse1 (uint64) __attribute__((noinline));
uint64 llReverse2 (uint64) __attribute__((noinline));
unsigned long long llReverse3 (unsigned long long) __attribute__((noinline));

uint32 Reverse1 (uint32 x) {
  return (x<<24) | ((x<<8) & 0x00ff0000) | ((x>>8) & 0x0000ff00) | (x>>24);
}
uint32 Reverse2 (uint32 x) {
  return ((x<<24) & 0xff000000) | ((x<<8) & 0x00ff0000) | ((x>>8) & 0x0000ff00) | ((x>>24) & 0x000000ff);
}
unsigned int Reverse3 (unsigned int x) {
  return ((x & 0x0000ff00) <<8) | ((x & 0x00ff0000) >> 8) | ((x & 0xff000000) >> 24) | ((x & 0x000000ff) << 24);
}

uint64 llReverse1 (uint64 x) {
  return ((x&0xFF00000000000000ull)>>56) | ((x&0x00FF000000000000ull)>>40) | ((x&0x0000FF0000000000ull)>>24) | ((x&0x000000FF00000000ull)>> 8) 
	| ((x&0x00000000FF000000ull)<< 8) | ((x&0x0000000000FF0000ull)<<24) | ((x&0x000000000000FF00ull)<<40) | ((x&0x00000000000000FFull)<<56);
}
uint64 llReverse2 (uint64 x) {
  return (x>>56) | ((x&0x00FF000000000000ull)>>40) | ((x&0x0000FF0000000000ull)>>24) | ((x&0x000000FF00000000ull)>> 8) 
	| ((x&0x00000000FF000000ull)<< 8) | ((x&0x0000000000FF0000ull)<<24) | ((x&0x000000000000FF00ull)<<40) | (x<<56);
}
unsigned long long llReverse3 (unsigned long long x) {
  return ((x<<40)&0x00FF000000000000ull) | ((x<<24)&0x0000FF0000000000ull) 
	| ((x>>8)&0x00000000FF000000ull) | ((x>>24)&0x0000000000FF0000ull) |  ((x<<56)&0xFF00000000000000ull) | ((x<<8)&0x000000FF00000000ull) 
        | ((x>>40)&0x000000000000FF00ull) | ((x>>56)&0x00000000000000FFull);
}

int main() {
  unsigned int x = 0x12345678;
  unsigned long long y = 0x123456789abcdef0ull;
  if (Reverse1(x) != 0x78563412)
    abort();
  if (Reverse2(x) != 0x78563412)
    abort();
  if (Reverse3(x) != 0x78563412)
    abort();
  if (llReverse1(y) != 0xf0debc9a78563412ull)
    abort();
  if (llReverse2(y) != 0xf0debc9a78563412ull)
    abort();
  if (llReverse3(y) != 0xf0debc9a78563412ull)
    abort();
  return 0;
}
