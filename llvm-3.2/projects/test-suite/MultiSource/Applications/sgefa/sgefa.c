  /***************************************************************
 ******************************************************************
****	    Gaussian Elimination with partial pivoting.		****
****	This file contains the factorization routine SGEFA	****
 ******************************************************************
  ****************************************************************/

#include "ge.h"
static char SGEFASid[] = "@(#)sgefa.c	1.1  2/4/86";

int sgefa( a, ipvt )
struct FULL *a;
int	    *ipvt;
/*
    PURPOSE
	SGEFA factors a real matrix by gaussian elimination.

    REMARKS
	SGEFA is usually called by SGECO, but it can be called
	directly with a saving in time if  rcond  is not needed.
	(time for SGECO) = (1 + 9/n)*(time for SGEFA) .

    INPUT
	a	A pointer to the FULL matrix structure.  
		See the definition in ge.h.

    OUTPUT
	a	A pointer to the FULL matrix structure containing 
		an upper triangular matrix and the multipliers
		which were used to obtain it.
		The factorization can be written  a = l*u  where
		l  is a product of permutation and unit lower
		triangular matrices and  u  is upper triangular.
	ipvt	An integer vector (of length a->cd) of pivot indices.

    RETURNS
		= -1  Matrix is not square.
		=  0  Normal return value.
		=  k  if  u(k,k) .eq. 0.0 .  This is not an error
		      condition for this subroutine, but it does
		      indicate that sgesl or sgedi will divide by zero
		      if called.  Use  rcond  in sgeco for a reliable
		      indication of singularity.

    ROUTINES
	blas ISAMAX
*/
{
  register int  i, j;
  int		isamax(), k, l, nm1, info, n;
  float		t, *akk, *alk, *aij, *mik;

  /* Gaussian elimination with partial pivoting. */
  if( a->cd != a->rd ) return( -1 );
  n    = a->cd;
  nm1  = n - 1;
  akk  = a->pd[0];
  info = 0;				/* Assume nothing will go wrong! */
  if( n < 2 ) goto CLEAN_UP;

  /*  Loop over Diagional */
  for( k=0; k<nm1; k++, ipvt++ ) {

    /* Find index of max elem in col below the diagional (l = pivot index). */
    akk   = a->pd[k] + k;
    l     = isamax( n-k, akk, 1 ) + k;
    *ipvt = l;

    /* Zero pivot implies this column already triangularized. */
    alk = a->pd[k] + l;
    if( *alk == 0.0e0) {
      info = k;
      continue;
    }

    /* Interchange a(k,k) and a(l,k) if necessary. */
    if( l != k ) {
      t    = *alk;
      *alk = *akk;
      *akk = t;
    }

    /* Compute multipliers for this column. */
    t = -1.0e0 / (*akk);
    for( i=k+1, mik = akk+1; i<n; i++, mik++ )
      *mik *= t;

    /* Column elimination with row indexing. */
    for( j=k+1; j<n; j++ ) {

      /* Interchange a(k,j) and a(l,j) if necessary. */
      t = pelem(a,k,j);
      if( l != k ) {
	pelem(a,k,j) = pelem(a,l,j);
	pelem(a,l,j) = t;
	t = pelem(a,k,j);
      }
      for( i=k+1, aij=a->pd[j]+k+1, mik=akk+1; i<n; i++, aij++, mik++ ) 
	*aij += t*(*mik);
    }
  }				/* End of for k loop */
  
 CLEAN_UP: 
  *ipvt = nm1;
  if( *akk == 0.0e0 ) info = n;
  return( info );
}
