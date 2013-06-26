
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
#include "Triang.h"
#include "MM.h"
#include "print.h"


double norm(Matrix A,int col,int rs,int re)
{
  double t=0.0;
  int i;

  for (i=rs;i<=re;i++)
    t += A[i][col]*A[i][col];
  
  return sqrt(t);
}

void House(Matrix A,Vector v,int col,int sr,int er)
{
  double a,b;
  int m;

  a=norm(A,col,sr,er);
  b=1/(A[sr][col]+sign(A[sr][col])*a);

  for (m=sr+1;m<=er;m++)
	v[m]=A[m][col]*b;
      
  v[sr] = 1.0;
}

double xty(Vector x,Vector y,int s,int e)
{
  double t=0.0;
  int i;

  for (i=s;i<=e;i++)
    t += x[i]*y[i];

  return t;
}

Matrix Trianglelise(Matrix A,int i)
{
  Matrix U,P,T;

  double a,b;
  Vector v,w,p;
  
  int j,k,l,m,h;

  /* Initialize U to be the I */
  P = newMatrix();
  U = newIdMatrix();

  v=(Vector)malloc(sizeof(double)*n);
  w=(Vector)malloc(sizeof(double)*n);
  p=(Vector)malloc(sizeof(double)*n);

  if (i<2) return A;
  l=i; /* This is the number of non-zero off diagonal entries */ 
  for (j=0;j<n-2;j++) /* Iterate through the columns */
    { 

      /* This is to save time, h = min(j+l+2,n-1) */
      h = (j+l+i<n-1)?j+l+i:n-1;

      /* Find the householder vektor */
      House(A,v,j,j+1,h);

      /* Now v[j+1:j+l] contains the householder vektor */

      /* Well, we need to apply the thing pre and 
	 post multiplikative */

      /* b = 1/(v.t()*v) */
      b=1.0/xty(v,v,j+1,h);

      for (m=j;m<=h;m++)
	p[m] = 2.0*xty(A[m],v,j+1,h)*b; 

      a=xty(p,v,j+1,h)*b;
      /* w = p - a*v */
      for (m=j+1;m<=h;m++)
	w[m] = p[m] - a*v[m];

      /* Update A[j+1:h][j+1:h], and utilize that we        */
      /* know A to be symmetric before and after the update */
      for (m=j+1;m<=h;m++)
	for (k=m;k<=h;k++)
	  {
	    A[m][k] -= v[m]*w[k] + w[m]*v[k];
	    A[k][m] = A[m][k];
	  }

      /* Calculating the off diagonal entry */

      A[j+1][j] = A[j][j+1] = A[j][j+1] - p[j];

      /* And zero the rest of the column/row */
      for (m=j+2;m<=h;m++)
	A[j][m] = A[m][j] = 0.0; 
      
      /* Calculate U*P */
      /* Only update colums j+1 through j+l */
      /* Remember that U is NOT symmetric */
      /* TIME l*n+n*2*l = 3*n*l */

      for (m=0;m<n;m++)
	w[m]=2.0*b*xty(U[m],v,j+1,h);

      for (m=0;m<n;m++)
	for (k=j+1;k<=h;k++)
	  U[m][k] -= w[m]*v[k]; 

      /* Update l */
      if (j+l+(i-1)<n-1) l += (i-1); else l = (n-1)-(j+1);
      /* Check(A,U,i); */
    }

  free(v);
  free(w);
  free(p);
  return U;

}


