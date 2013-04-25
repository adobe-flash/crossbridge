/* APPLE LOCAL file 5932809 */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-fblocks" } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#include <stdio.h>
void * _NSConcreteStackBlock[32];
void _Block_byref_assign_copy(void * dst, void *src){}
void _Block_byref_release(void*src){}

int main() {
   __block  int X = 1234;

   int (^CP)(void) = ^{ X = X+1;  return X; }; 
   CP();
   printf ("X = %d\n", X);
   return X - 1235;
}
