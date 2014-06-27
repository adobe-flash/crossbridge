  /***************************************************************
 ******************************************************************
****		      Test routine for SGEFA.C			****
 ******************************************************************
  ****************************************************************/

#include <stdio.h>
#include <math.h>
#include "ge.h"

#define BIG	 1.0e38		/* Largest representable float. */
#define SMALL	 1.0e-45	/* Smallest (in absolute value) representable float. */
#define MATPRINT 7		/* Matricies of size <= MATPRINT are printed out. */
#define VECPRINT 7	    	/* Vectors of size <= VECPRINT are printed out. */
#ifndef SCALE
#define SCALE	 1		/* System sizes are scaled by this amount. */
#endif
main()
{
  register int i, j, k;
  struct FULL a;

  /* Storage for rhs, rhs of trans(A)x and solution. */
  float *b, *bt, *x, anorm, *col, t;
  double err, snrm2();
  int 	*ipvt, retval, test_case = 0;
  void  matdump(), ivecdump(), fvecdump();
  
  /* Loop over the test cases. */
  /* Initilize matrix. */
  while( !matgen( &a, &x, &b, &bt, &ipvt, ++test_case, SCALE ) ) {
  
      /* Check that system size is reasonable. */
      if( a.rd > MAXCOL || a.cd > MAXCOL ) {
	  fprintf(stderr,"Matrix row dim (%d) or column dim (%d) too big.\n",a.rd,a.cd);
	  exit( 1 );
      }
      /* Calculate the 1 norm of A. */
      for( j=0, anorm=0.0; j<a.cd; j++ ) {
	  for( i=0, col=a.pd[j], t=0.0; i<a.rd; i++, col++ ) 
	    t += (*col<0.0 ? -*col : *col );
	  anorm = ( anorm > t ? anorm : t );
      }
      printf("One-Norm(A) ---------- %e.\n", anorm );


    /* Test SGEFA. */
    retval = sgefa( &a, ipvt );

    /* For a successful return from SGEFA test SGESL. */
    if( retval ) 
      ;//printf("Zero Column %d found \n", retval );
    else {
      /* Solve system. */
      (void)sgesl( &a, ipvt, b, 0 );
      if( a.rd <= MATPRINT )
	  (void) matdump( &a, "FACTORED MATRIX FOLLOWS" );
      if( a.rd <= VECPRINT ) {
	  (void)fvecdump( x, a.rd, "True Solution");
	  (void)fvecdump( b, a.rd, "Solution");
      }
      (void)vexopy( a.rd, b, x, b, 2 );
      err = snrm2( a.rd, b, 1 );
      //printf(" For Ax=b.    Absolute error = %e.  Relative error = %e.\n",
      //       err, err/snrm2( a.rd, x, 1 ) );

      /* Solve transpose system. */
      (void)sgesl( &a, ipvt, bt, 1 );
      if( a.rd <= VECPRINT ) {
	  (void)fvecdump( bt, a.rd, "Solution to transposed system");
      }
      (void)vexopy( a.rd, bt, x, bt, 2 );
      err = snrm2( a.rd, bt, 1 );
      //printf(" For A^Tx=b.  Absolute error = %e.  Relative error = %e.\n",
      //       err, err/snrm2( a.rd, x, 1 ) );
    }
  }				/* End of while loop over test cases. */
  return 0;
}				/* End of MAIN */

int matgen( a, x, b, bt, ipvt, test_case, scale )
struct FULL *a;
float	    **x, **b, **bt;
int	    **ipvt, test_case, scale;
/*
  This routine generates test matrices for the SGE routines.
  
  INPUT
 test_case	Switch to type of matrix to generate.
 		1, 2, 3 => Hilbert slices of various sizes.  Note
		due to the memory allocalion local to this routine
		test_case=1 must be run before any of the others.
		4, 5    => monoelemental test.
  
 scale          Sizes of systems are scaled according to scale.

  OUTPUT
  a		The matrix stored in the FULL structure.
  x		Generated solution.
  b		Generated right hand side.
  bt		Generated rhs of trans(A)x.
  ipvt		A pointer to an array of ints.

  RETURN VALUE
  0 => everything went O.K.
*/
{
  register int i, j;
  int  n;
  float *col, tl, tu;
  char *malloc();
  void free(), matdump(), ivecdump(), fvecdump();

  if( test_case>1 ) {			/* Free up memory used in the last test. */
    printf("\n\n**********************************************************************\n");
    for( j=0; j<a->rd; j++ )
      free( a->pd[j] );
    free( *x );
    free( *b );
    free( *bt );
  }

  /* Determine the test to generate. */
  switch( test_case ) {
  case 1:				/* Hilbert slice of various sizes. */
  case 2:
  case 3:
    n = 3*test_case*scale;		/* Set system size. */
    a->cd = n;				/* Set column and row dimensions. */
    a->rd = n;
  
    if( get_space( a, x, b, bt, ipvt ) )/* Get the space needed for vectors. */
      return( 1 );

    printf("Hilbert Slice.  Test case %d of size %d.\n", test_case, n );
    for( j=0; j<n; j++ ) {
      for( i=0, col=a->pd[j]; i<n; i++, col++ ) {
	*col = 0.0;
	if( i>=(j-3) && i<=(j+2) ) *col = 1.0/(float)(i+j+1);
      }
    }
    break;

  case 4:				/* Monoemenental test (NOT SCALED). */
  case 5:
    n=1;
    a->cd = n;				/* Set column and row dimensions. */
    a->rd = n;
  
    if( get_space( a, x, b, bt, ipvt ) )/* Get the space needed for vectors. */
      return( 1 );

    printf("Monoelemental.  Test case %d of size %d.\n", test_case, n );
    *a->pd[0] = ( test_case == 4 ? 3.0 : 0.0 ); 
    break;

  case 6:				/* Tridiagional of various types. */
  case 7:
  case 8:
    n = 15*scale;
    a->cd = n;				/* Set column and row dimensions. */
    a->rd = n;
  
    if( get_space( a, x, b, bt, ipvt ) )/* Get the space needed for vectors. */
      return( 1 );

    printf("Tridiagional.  Test case %d of size %d.\n", test_case, n );
    tu = 1.0;
    tl = 1.0;
    if( test_case == 7 ) tl = 100.0;
    if( test_case == 8 ) tu = 100.0;
    for( j=0; j<n; j++ ) {
      for( i=0, col=a->pd[j]; i<n; i++, col++ ) {
	*col = 0.0;
	if( i==j ) *col = 4.0;
	else if( i==j-1 ) *col = tl;
	else if( i==j+1 ) *col = tu;
      }
    }
    break;

  case 9:				/* Rank one. */
    n = 5*scale;
    a->cd = n;				/* Set column and row dimensions. */
    a->rd = n;
  
    if( get_space( a, x, b, bt, ipvt ) )/* Get the space needed for vectors. */
      return( 1 );

    printf("Rank One.  Test case %d of size %d.\n", test_case, n );
    for( j=0; j<n; j++ ) {
      for( i=0, col=a->pd[j]; i<n; i++, col++ ) {
	*col = (float) pow( 10.0, (double)(i-j) );
      }
    }
    break;

  case 10:				/* Zero column. */
    n = 4*scale;
    a->cd = n;				/* Set column and row dimensions. */
    a->rd = n;
  
    if( get_space( a, x, b, bt, ipvt ) )/* Get the space needed for vectors. */
      return( 1 );

    printf("Zero Column.  Test case %d of size %d.\n", test_case, n );
    for( j=0; j<n; j++ ) {
      for( i=0, col=a->pd[j]; i<n; i++, col++ ) {
	tu   = (float)(j-2);
	tl   = (float)(i+1);
	*col = tu/tl;
      }
    }
    break;

  case 11:				/* Upper Triangular. */
    n = 6*scale;
    a->cd = n;				/* Set column and row dimensions. */
    a->rd = n;
  
    if( get_space( a, x, b, bt, ipvt ) )/* Get the space needed for vectors. */
      return( 1 );

    printf("Upper Triangular.  Test case %d of size %d.\n", test_case, n );
    for( j=0; j<n; j++ ) 
      for( i=0, col=a->pd[j]; i<n; i++, col++ ) 
	*col = ( i>j ? 0.0 : (float)(i-j+1) );
    break;

  case 12:				/* Lower Triangular. */
    n = 6*scale;
    a->cd = n;				/* Set column and row dimensions. */
    a->rd = n;
  
    if( get_space( a, x, b, bt, ipvt ) )/* Get the space needed for vectors. */
      return( 1 );

    printf("Lower Triangular.  Test case %d of size %d.\n", test_case, n );
    for( j=0; j<n; j++ ) 
      for( i=0, col=a->pd[j]; i<n; i++, col++ ) 
	*col = ( i<j ? 0.0 : (float)(i-j+1) );
    break;

  case 13:				/* Near Overflow. */
    n = 5*scale;
    a->cd = n;				/* Set column and row dimensions. */
    a->rd = n;
  
    if( get_space( a, x, b, bt, ipvt ) )/* Get the space needed for vectors. */
      return( 1 );

    printf("Near Overflow.  Test case %d of size %d. BIG = %e\n", test_case, n, BIG );
    tl = (float)(n*n);
    for( j=0; j<n; j++ ) 
      for( i=0, col=a->pd[j]; i<n; i++, col++ ) {
	tu = (float)(j+1)/(float)( i>j ? i+1 : j+1 );		/* A number <= 1.0 */
	*col = BIG*tu/tl;
      }
    break;

  case 14:				/* Near Underflow. */
    n = 5*scale;
    a->cd = n;				/* Set column and row dimensions. */
    a->rd = n;
  
    if( get_space( a, x, b, bt, ipvt ) )/* Get the space needed for vectors. */
      return( 1 );

    /* Assumes that BIG < 1/SMALL */
    printf("Near Underflow.  Test case %d of size %d. SMALL = %e\n", test_case, n, 1.0/BIG );
    tl = (float)(n*n);
    for( j=0; j<n; j++ ) 
      for( i=0, col=a->pd[j]; i<n; i++, col++ ) {
	tu = (float)( i>j ? i+1 : j+1 )/(float)(j+1);	/* A number >= 1.0 */
	*col = tu*tl/BIG;
      }
    break;

  default:
    printf("MATGEN: All tests complete.\n");
    return( 1 );
    break;

  }

  /* Generate solution. */
  **x = 1.0;
  if( n>1 ) **bt = 0.0;
  if( n>2 ) {
    for( i=2, col=(*x)+2; i<n; i++, col++ )
      *col = - *(col-2);
  }

  /* Generate rhs. */
  if( matvec( a, *x, *b, 0 ) ) {
    printf("MATGEN: Error in matvec.\n");
    return( 1 );
  }

  /* Generate rhs of transposed system. */
  if( matvec( a, *x, *bt, 1 ) ) {
    printf("MATGEN: Error in matvec.\n");
    return( 1 );
  }
  if( n<=MATPRINT ) 
    (void) matdump( a, "MATRIX FOLLOWS" );
  if( n<=VECPRINT ) {
    (void) fvecdump( *x,  n, "SOLUTION" );
    (void) fvecdump( *b,  n, "RIGHT HAND SIDE" );
    (void) fvecdump( *bt, n, "TRANSPOSE RIGHT HAND SIDE" );
  }
  return( 0 );
}				/* End of MATGEN */

int get_space( a, x, b, bt, ipvt )
struct FULL *a;
float       **x, **b, **bt;
int         **ipvt;
/*
  This routine gets space for the above vectors.
*/
{
  char *malloc();
  register int i,j;

  /* Get space for the columns. */
  for( j=0; j<a->rd; j++ ) {
    a->pd[j] = (float *)malloc( a->cd*sizeof( float ) );
    if( a->pd[j] == NULL ) {
      printf("GET_SPACE: Can't get enouph space for matricies...\n");
      return( 1 );
    }
  }
  *x    = (float *)malloc( a->cd*sizeof( float ) );
  *b    = (float *)malloc( a->cd*sizeof( float ) );
  *bt   = (float *)malloc( a->cd*sizeof( float ) );
  *ipvt = (int *)malloc( a->cd*sizeof( int ) );
  if( *x == NULL || *b == NULL || *bt == NULL || *ipvt == NULL) {
      printf("GET_SPACE: Can't get enouph space for vectors...\n");
      return( 1 );
    }
  return( 0 );
}

int matvec( a, x, b, job)
struct FULL *a;
float	    *x, *b;
int	    job;
/*
  This routine calculates b = a*x (if job=0 and b=trans(a)x else)
  via a column oriented approach.  It is most efficient if the matrix 
  is stored by columns.   a is a matrix (not its transpose, even when 
  job is nonzero) and x, b are vectors of the appropriate size.

  RETURNS
  Nonzero if something goes wrong.
*/
{
  register int i, j;
  float *px, *pb, *col, *row;
  
  /* Check input. */
  if( (a->cd < 1) || (a->rd < 1) ) return( 1 );

  /* Job non-zero => do b = trans(A)x. */
  if( job ) {
    /* Loop over (transposed columns) rows. */
    for( i=0, pb=b; i<a->rd; i++, pb++ ) {
      for( j=0, row=a->pd[i], px=x, *pb=0.0; j<a->cd; j++, px++, row++ )
	*pb += (*row)*(*px);
    }
    return( 0 );
  }

  /* Job zero => do b = Ax. */
  /* Loop over columns. */
  for( i=0, px=x, pb=b, col=a->pd[0]; i<a->rd; i++, pb++, col++)
    *pb = (*col)*(*px);
  for( j=1; j<a->cd; j++ ) {
    for( i=0, px=x+j, pb=b, col=a->pd[j]; i<a->rd; i++, pb++, col++)
      *pb += (*col)*(*px);
  }
  return( 0 );
}				/* End of MATVEC */

void matdump( a, head )
struct FULL *a;
char	    *head;
/*
  This routine prints out a FULL matrix with headding head.
*/
{
  register int i, j;
  int k, jj, ncolmod, ncolrem;

  ncolmod = (a->cd)/6;
  ncolrem = a->cd - ncolmod*6;

  printf("%s\n", head );
  for( i=0;  i<a->rd;  i++) {
    printf("%3d|", i );
    j = 0;
    for( k=0; k<ncolmod; k++ ) {
      if( k ) printf("    ");
      for( jj=0;  jj<6;  jj++, j++) 
	printf("%12.4e", pelem(a,i,j));
      printf("\n");
    }
    
    /* Clean up remainder of this row. */
    for( jj=0;  jj<ncolrem;  jj++, j++) 
      printf("%12.4e", pelem(a,i,j));
    printf("\n");
    }
  printf("\n");
}				/* End of MATDUMP */

void fvecdump( vec, len, head )
float *vec;
int   len;
char  *head;
/*
  This routine prints out a float vector `vec' of length `len' and
  a headding of `head'.
*/
{
  register int i, j, count;
  int lenmod, lenrem;

  lenmod = len/6;
  lenrem = len - lenmod*6;

  printf("%s\n", head );
  count = 0;
  for( j=0; j<lenmod; j++ ) {
    printf("%3d|", count );
    for( i=0; i<6; i++, vec++, count++ )
      printf("%12.4e", *vec );
    printf("\n");
  }
  /* Clean up loop. */
  if( lenrem ) {
    printf("%3d|", count );
    for( i=0; i<lenrem; i++, vec++ )
      printf("%12.4e", *vec );
    printf("\n");
  }
  printf("\n");
}				/* End of FVECDUMP */

void ivecdump( vec, len, head )
int  *vec;
int  len;
char *head;
/*
  This routine prints out a float vector `vec' of length `len' and
  a headding of `head'.
*/
{
  register int i, j, count;
  int lenmod, lenrem;

  lenmod = len/9;
  lenrem = len - lenmod*6;

  printf("%s\n", head );
  count = 0;
  for( j=0; j<lenmod; j++ ) {
    printf("%3d|", count );
    for( i=0; i<9; i++, vec++, count++ )
      printf("%8d", *vec );
    printf("\n");
  }
  /* Clean up loop. */
  if( lenrem ) {
    printf("%3d|", count );
    for( i=0; i<lenrem; i++, vec++ )
      printf("%8d", *vec );
    printf("\n");
  }
  printf("\n");
}
