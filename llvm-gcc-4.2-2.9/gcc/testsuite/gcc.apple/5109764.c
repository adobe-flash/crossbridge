/* APPLE LOCAL file radar 5109764 */
/* { dg-do compile } */
/* { dg-options "-O1" } */
unsigned int foo (double dbl1) {
  char bigarray[32768] ;
  unsigned int ticks;
  ticks = (unsigned int)(dbl1 * 1000.0);
  return baar (&bigarray, ticks);
}
