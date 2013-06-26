/* SCCS ID @(#)ge.h	1.1		2/4/86 */
  /***************************************************************
 ******************************************************************
****	 Matrix data structure(s) for Gaussian Elimination	****
 ******************************************************************
  ****************************************************************/
/* 
   This file contains the definitions of the structures used in
   various algorithms for doing Gaussian Elimination.

   The following gives an array (of length 10) of pointers to floats.
     float *a[10]; 
   Now assume that each a[i] points to space for an array of floats (gotten
   by a call to malloc, say).
   Then the following is true:
        a[i]       can be thought of as a pointer to the i-th array of floats,
        *(a[i]+j)  is the j-th element of the i-th array.

   The following shows how to reference things for the definition of the FULL
   structure given below.
      a->cd is the value of (as apposed to a pointer to) the column dimension.
      a->rd is the value of (as apposed to a pointer to) the row dimension.
      a->pd[j] is a pointer to the j-th column (an array of floats).
      *(a->pd[j]+i) is the i-th element of the j-th column, viz., a(i,j).
   Here we think, as is natural in C, of all matrices and vectors indexed 
   from 0 insead of 1.
*/

#define MAXCOL 1000	/* Maximum number of Columns. */

struct FULL {		/* Struct definition for the FULL matrix structure. */
  int	cd;		/* Column dimension of the matrix. */
  int	rd;		/* Row Dimension of the matrix. */
  float	*pd[MAXCOL]; 	/* Array of pointers to the columns of a matrix. */
};

/* The following macro will get a(r,c) from a matrix in the FULL structure. */
#define elem(a,r,c)	(*(a.pd[(c)]+(r)))

/* The following macro will get a(r,c) from a pointer to a matrix 
   in the FULL structure. */
#define pelem(a,r,c)	(*(a->pd[(c)]+(r)))
