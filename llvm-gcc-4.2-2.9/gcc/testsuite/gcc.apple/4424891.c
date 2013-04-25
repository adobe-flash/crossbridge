/* APPLE LOCAL file 4424891 */
/* { dg-do compile { target "i?86-*-darwin*" } } */
/* { dg-options "-mdynamic-no-pic -march=nocona -O1 -mfpmath=sse" } */
double xx;
void ddp (long long *bbcount, long ecount) {
  xx = (ecount * 100.0 / *bbcount);
}
