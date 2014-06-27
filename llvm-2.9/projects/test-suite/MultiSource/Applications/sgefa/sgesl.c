  /***************************************************************
 ******************************************************************
****	    Gaussian Elimination with partial pivoting.		****
****	This file contains the solution routine SGESL		****
 ******************************************************************
  ****************************************************************/

#include "ge.h"
static char SGESLSid[] = "@(#)sgesl.c	1.1  2/4/86";

int sgesl( a, ipvt, b, job )
struct FULL *a;
int 	    *ipvt, job;
float	    b[];
/*
    PURPOSE
	SGESL solves the real system
	a * x = b  or  trans(a) * x = b
	using the factors computed by SGECO or SGEFA.

    INPUT
	a	A pointer to the FULL matrix structure containing the factored
		matrix.  See the definition of FULL in ge.h.
	ipvt    The pivot vector (of length a->cd) from SGECO or SGEFA.
	b       The right hand side vector (of length a->cd).
	job     = 0         to solve  a*x = b ,
		= nonzero   to solve  trans(a)*x = b  where
			    trans(a)  is the transpose.

    OUTPUT
	b       The solution vector x. 

    REMARKS
	Error condition:
	A division by zero will occur if the input factor contains a
	zero on the diagonal.  Technically this indicates singularity
	but it is often caused by improper arguments or improper
	setting of lda .  It will not occur if the subroutines are
	called correctly and if sgeco has set rcond .gt. 0.0
	or sgefa has set info .eq. 0 .
*/
{
  float	t;
  float	*akk, *mik, *uik, *bi;
  register int i, k;
  int	l, n, nm1;

  n   = a->cd;
  nm1 = n - 1;

  /* job = 0 , solve  a * x = b.  */
  if( job == 0 ) {
    /* Forward elimination. Solve l*y = b. */
    for( k=0; k<nm1; k++, ipvt++ ) {
      akk = a->pd[k] + k;		/* akk points to a(k,k). */

      /* Interchange b[k] and b[l] if necessary. */
      l = *ipvt;
      t = b[l];
      if( l != k ) {
	b[l] = b[k];
	b[k] = t;
      }
      for( i=k+1, mik=akk+1; i<n; i++, mik++ )
	b[i] += (*mik)*t;
    }

    /* Back substitution.  Solve  u*x = y. */
    for( k=nm1; k>=0; k-- ) {
      akk = a->pd[k] +k;
      b[k] /= (*akk);
      for( i=0, uik=a->pd[k]; i<k; i++, uik++ )
	b[i] -= (*uik)*b[k];
    }
    return 0;
  }

  /* job = nonzero.  Solve  trans(a) * x = b. */
  /* First solve trans(u)*y = b. */
  for( k=0; k<n; k++ ) {
    akk = a->pd[k] + k;
    for( i=0, t=0.0, uik=a->pd[k], bi=b; i<k; i++, uik++, bi++ )
      t += (*uik)*(*bi);
    b[k] = (b[k] - t) / (*akk);
  }

  /* b now contains y. */
  /* Solve trans(l)*x = y. */
  ipvt += n-2;
  for( k=n-2; k>=0; k--, ipvt-- ) {
    for( i=k+1, t=0.0, mik=a->pd[k]+k+1, bi=b+k+1; i<n; i++, mik++, bi++ )
      t += (*mik)*(*bi);
    b[k] += t;
    
    /* Interchange b(k) and b(ipvt(k)) if necessary. */
    l    = *ipvt;
    if( l == k ) continue; 
    t    = b[l];
    b[l] = b[k];
    b[k] = t;
  }
  return 0;
}
