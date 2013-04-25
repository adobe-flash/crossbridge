/* { dg-do compile } */
/* APPLE LOCAL testsuite nested function */
/* { dg-options "-fnested-functions" } */

void foo (void) { 
  void nested () {} 
}
