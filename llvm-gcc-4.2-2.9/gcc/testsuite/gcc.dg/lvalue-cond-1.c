/* APPLE LOCAL file non lvalue assign */
/* Allow assignments to conditional expressions, as long as the second and third
   operands are already lvalues.  */
/* Author: Ziemowit Laski <zlaski@apple.com> */
/* { dg-options "-fnon-lvalue-assign" } */
/* { dg-do run } */

#include <stdlib.h>

int g1 = 3, g2 = 5;

void assign_val1 (int which, int value) {
  (which ? g1 : g2) = value;  /* { dg-warning "target of assignment not really an lvalue" } */
}

void assign_val2 (int which) {
  (which ? g1 : g2)++;  /* { dg-warning "target of assignment not really an lvalue" } */
}

int main(void) {
   assign_val1 (0, 15);
   if (g1 != 3 || g2 != 15)
     abort ();

   assign_val2 (1);
   if (g1 != 4 || g2 != 15)
     abort ();

   return 0;
}
