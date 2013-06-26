
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
#include "MM.h"

Matrix MakeMatrix(int i)
{
  int j,k;
  Matrix M;

  M=newMatrix();

  for (j=0;j<n;j++)
    for (k=j;k<n;k++)
      if (abs(k-j)>i) M[j][k] = M[k][j] = 0.0;
      else M[j][k] = M[k][j] = 4.0/(5.0*sqrt(2.0*M_PI))*
	exp(-(8.0/25.0)*(j-k)*(j-k));
  return M;
}
	    
Matrix newMatrix(void)
{
  Matrix M;
  int i;

  M = (Matrix)malloc(sizeof(double *)*n);
  for (i=0;i<n;i++)
    M[i] = (double *)malloc(sizeof(double)*n);

  return M;
}

Vector newVector(void)
{
  Vector v;

  v = (Vector) malloc(sizeof(double)*n);
  
  return v;
}

void matrixMult(Matrix C,Matrix A, Matrix B)
{
  int i,j,k;

  for (i=0;i<n;i++)
    for (j=0;j<n;j++)
      {
	C[i][j] = 0.0;
	for (k=0;k<n;k++)
	  C[i][j] += A[i][k]*B[k][j];
      }
}

void matrixTranspose(Matrix A)
{
  int i,j;
  double a;

  for (i=0;i<n;i++)
    for (j=i+1;j<n;j++)
      {
	a = A[i][j];
	A[i][j] = A[j][i];
	A[j][i] = a;
      }
}

Matrix newIdMatrix(void)
{
  Matrix C;

  C = newMatrix();

  MakeID(C);

  return C;
}

void MakeID(Matrix A)
{
  int l,j;

  for (j=0;j<n;j++)
    for (l=j;l<n;l++)
      if (j==l) A[j][l]=1;
      else A[j][l]=A[l][j]=0.0;
}

void freeMatrix(Matrix A)
{
  int i;

  for (i=0;i<n;i++)
    free(A[i]);

  free(A);
}

double NormInf(Matrix A)
{
  double a,b;
  int i,j;

  a = 0.0;
  for (i=0;i<n;i++)
    {
      b = 0.0;
      for (j=0;j<n;j++)
	b += fabs(A[i][j]);
      if (b>a) a=b;
    }

  return a;
}

double NormOne(Matrix A)
{
  double a,b;
  int i,j;

  a = 0.0;
  for (j=0;j<n;j++)
    {
      b = 0.0;
      for (i=0;i<n;i++)
	b += fabs(A[i][j]);
      if (b>a) a=b;
    }

  return a;
}

