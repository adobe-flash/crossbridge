/* { dg-do compile { target "powerpc*-*-darwin*" } } */
/* Radar 3208244  */
#include "ppc_intrinsics.h"
#include <stdlib.h>

int main( void )
{
  int i,j;    
  int value;
  register int temp;
  double a,b,c,d;
  a = 1.;b=2.;c=3.;
        
  d = __fmadd(a,b,c);
  a = __fmadd(c,b,d);
  d = __fmadd(a,b,c);

  __lwsync();
  __nop();
  __lwsync();
  __nop();
  d = __fctidz(a);
  d = __fctidz(d);
  b = __fctid(a);
  b = __fctid(b);
  c = __fcfid(a);
  c = __fcfid(c);
  __dcbzl(32,&value);
  __dcbzl(&value,0);
        
  return a+b+c+d;
}


