
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

/************************************************************************/
/*  author :   Mikkel Damsgaard                                         */
/*             Kirsebaerhaven 85b                                        */
/*                                                                      */
/*             DK-8520 Lystrup                                          */
/*             email mikdam@daimi.aau.dk                                */
/*                                                                      */
/*  files :                                                             */
/*  Divsol.c           QRfact.h           Divsol.h           Jacobi.c   */     
/*  Jacobi.h           Triang.c           print.c            MM.c       */   
/*  Triang.h           print.h            MM.h               QRfact.c   */   
/*  main.c             main.h                                           */
/*                                                                      */
/*  It calculates the eigenvalues for 4 different matrixes. It does not */
/*  take any input; those 4 matrixes are calculated by MakeMatrix       */
/*  function. Output is given as 4 files: val2, val3, val4, val5, that  */
/*  contains the eigenvalues for each of the matrixes.                  */
/*                                                                      */
/************************************************************************/
#include "main.h"
#include "QRfact.h"
#include "Triang.h"
#include "Jacobi.h"
#include <stdio.h>
#include <string.h>
Matrix A,Q,U;

int comp(const double *a,const double *b)
{
  if (fabs(*a)<fabs(*b)) return 1;
  else if (fabs(*a)>fabs(*b)) return -1;
  else return 0;
}

int main ()
{
  double a,b,c,d;
  int i,j,k,l,m;
  Vector v,u,z,w;
  Matrix V,T,X,Z;
  FILE *vec;
  char filename[20],num[3];

  for (l=2;l<=5;l++) 
    {
      strcpy(filename,"val");
      sprintf(num,"%i\0",l);
      strcat(filename,num);
      /* printf("filename = %s\n",filename); */

      A = MakeMatrix(l);
      /*      if (l==5) printf("%e\n",A[0][5]); */


      /* Bauer-Fike */
      /*      printf("%i : Norm af E = %e\n",l,2*A[0][l]); */

      /*      U = Trianglelise(A,l); */
      U = Jacobi(A,l); 
      QRiterate(A,U); 

      v = newVector();
      for (i=0;i<n;i++)
	v[i] = A[i][i];
      
      qsort(v,n,sizeof(double),(int (*)(const void*,const void*))comp);
      
      for (i=0;i<n;i++)
	fprintf(stdout,"%i %e\n",i,v[i]);

      /*      printf("sigma1 = %e, sigman = %e, k2 = %e\n",
	      v[0],v[n-1],v[0]/v[n-1]); */

      /*      printf("Kondition af U: %e\n",NormOne(U)*NormInf(U)); */

      
      /*      Check(A,U,l);      */
      
      if (l==6)
	{ /* Get the egenvector for the 2 largest and 
	     the 2 smallest egenvalues */
	  
	  for (i=0;i<n;i++)
	    {
/*	      j = (i>=2)?n-i+1:i; */

	      k=0;
	      while (v[i]!=A[k][k]) k++;

	      strcpy(filename,"vec");
	      sprintf(num,"%i\0",i);
	      strcat(filename,num);
	      printf("filename = %s\n",filename);
	      
	      for (m=0;m<n;m++)
		fprintf(stdout,"%i %e\n",m,U[m][k]);
	    }

	}

      freeMatrix(U);
      freeMatrix(A);
    }
  return 0;
}

void Check(Matrix A, Matrix U, int l)
{  /* TEST SUITE */

  Matrix X,T;
  double a;
  int i,j;

  X = newMatrix();
  T = MakeMatrix(l);
  

  /* Beregn U^TXU */
  matrixMult(X,T,U); 
  
  matrixTranspose(U);
  
  matrixMult(T,U,X); 

  matrixTranspose(U);
  a = 0.0;
  for (i=0;i<n;i++)
    for (j=0;j<n;j++)
      a += (A[i][j]-T[i][j])*(A[i][j]-T[i][j]);
  
  printf("Step: %i !! The frobenius norm of X-T is %e\n",l,sqrt(a));
  
  a = 0.0;
  for (i=0;i<n;i++)
    for (j=i+1;j<n;j++)
      a += fabs(A[i][j]-A[j][i]);
  
  printf("Is A symmetric? %e\n",a);

  /*
     printMatrix(A);
     
     printMatrix(T);
     
     printMatrix(U); */


  printf("\n\n");
  freeMatrix(X);
  freeMatrix(T);
}

