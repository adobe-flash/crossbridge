/* LLVM LOCAL - whole file */
/* APPLE LOCAL file 5932809 */
/* { dg-options "-fblocks" } */
/* { dg-do compile } */

__block  int X; /* { dg-error "__block attribute on 'X' not allowed, only allowed on local variables" } */

int foo(__block int param) { /* { dg-error "__block attribute can be specified on variables only" } */
  __block int OK = 1;

  extern __block double extern_var; /* { dg-error "__block attribute on 'extern_var' not allowed, only allowed on local variables" } */
  if (X) {
    static __block char * pch; /* { dg-error "__block attribute on 'pch' not allowed, only allowed on local variables" } */
  }
  return OK - 1;
}
