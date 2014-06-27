
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
#include "MM.h"

/* Calculate givens transformation */
void Givens(double a,double b,double *s,double *c)
{
  double t;
  if (b==0.0) {*c=1;*s=0;}
  else if (fabs(b)>fabs(a))
    {
      t = -a/b;
      *s = 1/sqrt(1+t*t);
      *c = (*s)*t;
    }
  else
    {
      t = -b/a;
      *c = 1/sqrt(1+t*t);
      *s = (*c)*t;
    }
  /* printf("%f  %f  %f   %f\n",a,b,(*c)*a-(*s)*b,(*c)*b+(*s)*a); */
}

int sign(double a)
{
  if (a<0) return -1;
  return 1;
}


void ApplyRGivens(Matrix U,double s, double c,int i,int j)
{  /* U = U*G, G=Givens(i,j,theta) */  
  int k;
  double t1,t2;

  for (k=0;k<n;k++) 
    { 
      t1=U[k][i]; t2=U[k][j];
      U[k][i] = c*t1-s*t2; U[k][j] = s*t1+c*t2;
    }
}
    

Matrix QRiterate(Matrix A, Matrix U)
{
  double c,s,t,a,b,app,aqq,apq,a1p,a1q,a4p,a4q;
  double d,mu,x,z,t1,t2;

  int i,j,k,notdone=1,p,q,l,m;

  while (notdone)
    {
      /* First examine if any offdiagonal is small enough to elimante */
      for (i=0;i<n-1;i++)
	if (fabs(A[i+1][i])<(fabs(A[i][i])+fabs(A[i+1][i+1]))*epsilon)
	  A[i+1][i]=A[i][i+1]=0.0;

      /* Find the boundaries for the QR step */
      q=n-1;
      while ((q>0) && (A[q-1][q]==0.0)) q--;
      if (q==0) 
	{
	  notdone=0;
	}
      else
	{
	  p=q;
	  while ((p>0) && (A[p-1][p]!=0.0)) p--;
	}
      
      if (!notdone) break;
      /* printf("%e   %i\n",A[q-1][q],q); */
      /* Now we do the QR step on the submatrice A[p..q][p..q] */
      
      /* First calculate the shift */
      d = (A[q-1][q-1]-A[q][q])/2; t=A[q][q-1]; t=t*t;
      mu = A[q][q]-(t/(d+sign(d)*sqrt(d*d+t)));
      x = A[p][p]-mu;
      z = A[p+1][p];

      /* Now QR faktorise */
      for (i=p;i<q;i++)
	{
	  /* Find the givens rotation */
	  Givens(x,z,&s,&c);

	  /* l=max(p,i-1); */

	  l = (i-1>p)?i-1:p;

	  /* m=min(q,i+2); */

	  m = (q<i+2)?q:i+2;

	  for (k=l;k<=m;k++) 
	    {
	      t1=A[i][k]; t2=A[i+1][k];
	      A[i][k] = c*t1-s*t2; A[i+1][k] = s*t1+c*t2;
	    }
	  
	  for (k=l;k<=m;k++) 
	    {
	      t1=A[k][i]; t2=A[k][i+1];
	      A[k][i] = c*t1-s*t2; A[k][i+1] = s*t1+c*t2;
	    }


	  /* Store the givens in U */
	  /* U = G.T()*U */
	  ApplyRGivens(U,s,c,i,i+1);

	  /* And find the next pair of troublemakers */
	  if (i<q-1)
	    {
	      x=A[i+1][i];
	      z=A[i+2][i];
	    }
	}
    }

}
