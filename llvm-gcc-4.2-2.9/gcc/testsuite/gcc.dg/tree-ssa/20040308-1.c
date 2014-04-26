/* APPLE LOCAL file lno */
/* { dg-do compile } */
/* { dg-options "-O1 -fdump-tree-phiopt1-details" } */
/* LLVM LOCAL test not applicable */
/* { dg-require-fdump "" } */

int t( int i)
{
  int j;
  if(i ==0)
   j = 1;
  else
   j = 0;
  
  
  return j;
}

/* We should convert one COND_EXPRs into straightline code.  */
/* { dg-final { scan-tree-dump-times "straightline" 1 "phiopt1" {xfail *-*-*  } } } */

