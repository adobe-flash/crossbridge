
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
#include "QRfact.h"
#include "print.h"

void ApplyGivens(Matrix A,double s,double c, int i, int j,int start,int end)
{ /* A = G.t()AG, G = Givens(i,j,theta); */
  double t1,t2;
  int k;

  for (k=start;k<=end;k++)
    { /* A = G.t()*A */
      t1=A[i][k]; t2=A[j][k];
      A[i][k] = c*t1-s*t2; A[j][k] = s*t1+c*t2;
    }
  
  for (k=start;k<=end;k++)
    { /* A = A*G */
      t1=A[k][i]; t2=A[k][j];
      A[k][i] = c*t1-s*t2; A[k][j] = s*t1+c*t2;
    }
}

Matrix Jacobi(Matrix A,int bw)
{
  double a,b,s,c;
  int i,j,k,l,m;
  Matrix U;

  U = newIdMatrix();

  for (i=bw;i>=2;i--)
    { /* Remove one bandwith at a time */
      for (j=0;j<n-i;j++)
	{ /* Iterate through the diagonal */
	  
	  /* Calculate the Givens to zero A[j][j+i] */
	  Givens(A[j][j+i-1],A[j][j+i],&s,&c);

	  /* Now apply that givens */

	  /* A = G.t()*A*G */
	  ApplyGivens(A,s,c,j+i-1,j+i,j,(j+2*i<n)?j+2*i:n-1);
	  /* U = U*G */
	  ApplyRGivens(U,s,c,j+i-1,j+i);

	  /* Now the bandwith is i+1, and we want it to return to i */
	  /* so we chase the unwanted non-zero element down the diagonal */
	  /* The unwanted non-zero element is A[j+i-1][j+2i+1] */

	  /* Check(A,U,bw); */

	  m = j+i;
	  while (m<n-i)
	    {
	      /* We determine the givens that will zero the unwanted
		 non-zero element */

	      Givens(A[m-1][m+i-1],A[m-1][m+i],&s,&c);

	      /* And apply that givens */
	      
	      /* A = G.t()*A*G */
	      ApplyGivens(A,s,c,m+i-1,m+i,m-1,(m+2*i<n)?m+2*i:n-1);
	      /* U = U*G */
	      ApplyRGivens(U,s,c,m+i-1,m+i); 

	      /* Check(A,U,bw); */

	      m += i;
	    }

	  /* Now the ith band is zero in the first j positions */
	}
    }
  return U;
}
