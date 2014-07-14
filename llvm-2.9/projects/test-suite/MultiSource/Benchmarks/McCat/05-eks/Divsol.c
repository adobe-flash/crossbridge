
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
#include "QRfact.h"
#include "Triang.h"
#include "Divsol.h"


void HouseMatrix(Matrix H,Vector v, int start, int end)
{
  int i,j;
  double a;

  a = 2.0/xty(v,v,start,end);
  MakeID(H);
  for (i=start;i<=end;i++)
    for (j=start;j<=end;j++)
      H[i][j] -= a*v[i]*v[j];
}

void ApplyHouse(Matrix A,Vector v, int start, int end)
{
  Matrix M,H;

  M = newMatrix();
  H = newMatrix();

  HouseMatrix(H,v,0,n-1);

	  /* Apply it A=H*A*H */
  matrixMult(M,A,H);
  matrixMult(A,H,M);

  freeMatrix(H);
  freeMatrix(M);
}

void WeirdHouse(Matrix A,Vector v,int row,int sc,int ec)
{
  int i;
  double a,b;

  a = 0.0;

  for (i=sc;i<=ec;i++)
    a += A[row][i]*A[row][i];

  /* a is the 2-norm squared of A(row,sc:ec) */

  b=1/(A[row][ec]+sign(A[row][ec])*sqrt(a));

  for (i=sc;i<ec;i++)
    v[i] = A[row][i]*b;

  v[ec] = 1.0;
}

Matrix DivideAndSolve(Matrix A,int p)
{
  double a,b,d,e,s,c,mu,i;
  int h,j,k,l,m,o,rowstartt,colstartt,rowendt,colendt;
  Vector v,u,x,y,z;
  Matrix B,C,D,E,Q,U,H;

  U = newIdMatrix();
  H = newMatrix();
  v = newVector();

  i = p+1;

  rowstartt = i; 
  colstartt = 0;

  while (rowstartt<n)
    {
      rowendt = (rowstartt+i-1<n-1)?rowstartt+i-1:n-1; 
      colstartt = rowstartt-i;
      colendt = colstartt+i-1;
      
      /* Now T_i has dimension (p-h)x(p-h) */

      /* First zero all but row 1 in T_i */

      for (m=colstartt;m<=colendt;m++)
	{
	  if (norm(A,m,rowstartt,rowendt)!=0.0)
	    {
	      /* Find Householder(A(h:p,m)) */
	      House(A,v,m,rowstartt,rowendt);
	      for (o=0;o<rowstartt;o++)
		v[o]=0.0;
	      for (o=rowendt+1;o<n;o++)
		v[o]=0.0;
	      ApplyHouse(A,v,rowstartt,rowendt);
	    }
	  printf("m=%i, rowstart=%i, rowend=%i\n",m,rowstartt,rowendt);
	  printVector(v);
	  printMatrix(A);
	}

      /* Now zero all but the last entry in row 1 */

      WeirdHouse(A,v,rowstartt,colstartt,colendt);

      /* Apply the HouseHolder */

      ApplyHouse(A,v,colstartt,colendt);

      /* Now T_i has one single non-zero entry */

      /* Iterate with explicit shift to zero the last
	 non-zero entry */

      while (A[rowstartt][colendt]>
	     (A[rowstartt-1][colendt]-A[rowstartt][colendt+1])*epsilon)
	{
	  printMatrix(A);
	  /* Wilkonson Shift?? */
	  d  =(A[rowstartt-1][colendt]-A[rowstartt][colendt+1])/2.0;
	  b = A[rowstartt][colendt];
	  mu = A[rowstartt][colendt+1]+d-sign(d)*sqrt(d*d+b*b);

	  /* Determine the givens */
	  Givens(A[rowstartt-1][colendt]-mu,
		 A[rowstartt][colendt],&s,&c);
	  
	  /* Apply the givens */
	  ApplyGivens(A,s,c,rowstartt-1,rowstartt,0,n-1);
	  printf("%e\n",A[colstartt][colendt]);
	}

      rowstartt+=i;
      colstartt+=i;
    }				    
}
