
/****
    Copyright (C) 1996 McGill University.
    Copyright (C) 1996 McCAT System Group.
    Copyright (C) 1996 ACAPS Benchmark Administrator
                       benadmin@acaps.cs.mcgill.ca

    This program is free software; you can redistribute it and/or modify
    it provided this copyright notice is maintained.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
****/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "dbisect.h"
#include "allocvector.h"


void test_matrix(int n, double *C, double *B)
/* Symmetric tridiagonal matrix with diagonal

     c_i = i^4,  i = (1,2,...,n)

     and off-diagonal elements

     b_i = i-1,    i = (2,3,...n).
     It is possible to determine small eigenvalues of this matrix, with the
     same relative error as for the large ones. 
*/
{
  int i;
    
  for(i=0; i<n; i++) {
    B[i] = (double) i;
    C[i] = (double ) (i+1)*(i+1);
    C[i] = C[i] * C[i];
  }
}


int main(int argc,char *argv[])
{
  int rep,n,k,i,j;
  double eps,eps2;
  double *D,*E,*beta,*S;

  scanf("%d",&rep);
  scanf("%d",&n);
  scanf("%lf",&eps);

  dallocvector(n,&D);
  dallocvector(n,&E);
  dallocvector(n,&beta);
  dallocvector(n,&S);  
  
  for (j=0; j<rep; j++) {
    test_matrix(n,D,E);
    
    for (i=0; i<n; i++) {
      beta[i] = E[i] * E[i];
      S[i] = 0.0;
    }
    
    E[0] = beta[0] = 0;  
      dbisect(D,E,beta,n,1,n,eps,&eps2,&k,&S[-1]);    
    
  }
  
  for(i=1; i<n; i++)
    printf("%5d %.15e\n",i+1,S[i]); 

  printf("eps2 = %e,  k = %d\n",eps2,k);

  return 0;
}



