/* APPLE LOCAL file 5932809 */
/* { dg-options "-fblocks" } */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

void _Block_byref_release(void*src){}

#include <stdio.h>

int main() {
   __block  int X = 1234;
   __block  const char * message = "HELLO\n";

   X = X - 1234;

   printf ("%s\n", message);
   return X;
}
