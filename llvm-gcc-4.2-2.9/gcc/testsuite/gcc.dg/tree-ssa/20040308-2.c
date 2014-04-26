/* APPLE LOCAL file lno */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-phiopt1-details -fdump-tree-tailc-details" } */
/* LLVM LOCAL test not applicable */
/* { dg-require-fdump "" } */


int f(int i)
{
  int result;
  result = t(i);
  if (result)
    return result;
  return 0;
}

/* We should convert one COND_EXPRs into straightline code.  */
/* { dg-final { scan-tree-dump-times "straightline" 1 "phiopt1" } } */
/* Also we should have found that the call to t is tail called.   */
/* { dg-final { scan-tree-dump-times "Found tail call" 1 "tailc" } } */

