  /***************************************************************
  *****************************************************************
 *******************************************************************
*****								*****
*****				BLAS				*****
*****		Basic Linear Algebra Subroutines		*****
*****	      Written in the C Programming Language.		*****
*****								*****
*****	Functions include:					*****
*****	isamax, saxpy, saxpyx, scopy, sdot, snrm2,		****
*****	vexopy, vfill						*****
*****								*****
 *******************************************************************
  *****************************************************************
   ***************************************************************/
#include <math.h>
#ifndef SMALLsp
#define SMALLsp		1.0e-45		/* Smallest (pos) binary float */
#endif
#ifndef HUGEsp
#define HUGEsp		1.0e+38		/* Largest binary float */
#endif
int isamax( n, sx, incx )
float	*sx;
int	n, incx;
/*
    PURPOSE
        Finds the index of element having max. absolute value.

    INPUT
	n	Number of elements to check.
	sx	Vector to be checked.
	incx	Every incx-th element is checked.

*/
{
  float	smax = 0.0e0;
  int	i, istmp = 0;

#ifndef abs
#define abs(x) ((x)>0.0?(x):-(x)) 
#endif
  if( n <= 1 ) return( istmp );
  if( incx != 1 ) {
    /* Code for increment not equal to 1. */
    if( incx < 0 ) sx = sx + ((-n+1)*incx + 1);
    istmp = 0;
    smax  = abs( *sx );
    sx += incx;
    for( i=1; i<n; i++, sx+=incx ) 
      if( abs( *sx ) > smax ) {
	istmp = i;
	smax  = abs( *sx );
      }
    return( istmp );
  }
  /* Code for increment equal to 1. */
  istmp = 0;
  smax  = abs(*sx);
  sx++;
  for( i=1; i<n; i++, sx++ ) 
    if( abs( *sx ) > smax ) { 
      istmp = i;
      smax  = abs( *sx );
    }
  return( istmp );
}
 
void saxpy( n, sa, sx, incx, sy, incy )
float *sx, *sy, sa;
int   n, incx, incy;
/*
  PURPOSE
    Vector times a scalar plus a vector.  sy = sy + sa*sx.

  INPUT
    n		Number of elements to multiply.
    sa		Scalar to multiply by.
    sx		Pointer to float vector to scale.
    incx	Storage incrament for sx.
    sy		Pointer to float vector to add.
    incy	Storage incrament for sy.

  OUTPUT
    sy		sy = sy + sa*sx
*/
{
  register int i;

  if( n<=0 || sa==0.0 ) return;
  if( incx == incy ) {
    if( incx == 1 ) {
      /* Both increments = 1 */
      for( i=0; i<n; i++,sy++,sx++ )
	*sy += sa*(*sx);
      return;
    }
    if( incx>0 ) {
      /* Equal, positive, non-unit increments. */
      for( i=0; i<n; i++,sx+=incx,sy+=incx )
	*sy += sa*(*sx);
      return;
    }
  }
  /* Unequal or negative increments. */
  if( incx < 0 ) sx += ((-n+1)*incx + 1);
  if( incy < 0 ) sy += ((-n+1)*incy + 1);
  for( i=0; i<n; i++,sx+=incx,sy+=incy ) 
    *sy += sa*(*sx);
}
 
void saxpyx( n, sa, sx, incx, sy, incy )
float *sx, *sy, sa;
int   n, incx, incy;
/*
  PURPOSE
    Vector times a scalar plus a vector.  sx = sy + sa*sx.

  INPUT
    n		Number of elements to multiply.
    sa		Scalar to multiply by.
    sx		Pointer to float vector to scale.
    incx	Storage incrament for sx.
    sy		Pointer to float vector to add.
    incy	Storage incrament for sy.

  OUTPUT
    sx		sx = sy + sa*sx
*/
{
  register i;

  if( n<=0 || sa==0.0 ) return;
  if( incx == incy ) {
    if( incx == 1 ) {
      /* Both increments = 1 */
      for( i=0; i<n; i++, sx++, sy++ )
	*sx = *sy + sa*(*sx);
      return;
    }
    if( incx>0 ) {
      /* Equal, positive, non-unit increments. */
      for( i=0; i<n; i++, sx+=incx, sy+=incx)
	*sx = *sy + sa*(*sx);
      return;
    }
  }
  /* Unequal or negative increments. */
  if( incx < 0 ) sx += ((-n+1)*incx + 1);
  if( incy < 0 ) sy += ((-n+1)*incy + 1);
  for( i=0; i<n; i++,sx+=incx,sy+=incy ) 
    *sx = *sy + sa*(*sx);
}

void scopy( n, sx, incx, sy, incy )
float  *sx, *sy;
int     n, incx, incy;
/*
    PURPOSE
        Copies vector sx into vector sy.
 
    INPUT
        n    Number of components to copy.
	sx   Source vector
	incx Index increment for sx.
        incy Index increment for sy.
 
    OUTPUT
        sy   Destination vector.
*/
{
  register int i;

  if( n<1  ) return;
  if( incx == incy ) {
    if( incx == 1 ) {
      /* Both increments = 1 */
      for( i=0; i<n; i++ )
	*(sy++) = *(sx++);
      return;
    }
    if( incx > 0 ) {
      /* Equal, positive, non-unit increments. */
      for( i=0; i<n; i++, sx+=incx, sy+=incx)
	*sy = *sx;
      return;
    }
  }
  /* Non-equal or negative increments. */
  if( incx < 0 ) sx += ((-n+1)*incx + 1);
  if( incy < 0 ) sy += ((-n+1)*incy + 1);
  for( i=0; i<n; i++,sx+=incx,sy+=incy ) 
    (*sx) = (*sy);
  return;
}
double sdot( n, sx, incx, sy, incy )
float	*sx, *sy;
int	n, incx, incy;
/*
    PURPOSE
        Forms the dot product of a vector.

    INPUT
        n       Number of elements to sum.
        sx      Address of first element of x vector.
        incx    Incrament for the x vector.
        sy      Address of first element of y vector.
        incy    incrament for the y vector.

    OUPUT
        sdot    Dot product x and y.  Double returned
		due to `C' language features.
*/
{
  register i;
  float	stemp = 0.0e0;

  if( n<1 ) return( stemp );
  if( incx == incy ) {
    if( incx == 1 ) {
      /* Both increments = 1 */
      for( i=0; i<n; i++, sx++, sy++ )
	stemp += (*sx)*(*sy);
      return( stemp );
    }
    if( incx>0 ) {
      /* Equal, positive, non-unit increments. */
      for( i=0; i<n; i++, sx+=incx, sy+=incx)
	stemp += (*sx)*(*sy);
      return( stemp );
    }
  }
  /* Unequal or negative increments. */
  if( incx < 0 ) sx += ((-n+1)*incx + 1);
  if( incy < 0 ) sy += ((-n+1)*incy + 1);
  for( i=0; i<n; i++,sx+=incx,sy+=incy ) 
    stemp += (*sx)*(*sy);
  return( stemp );
}				/* End of ---SDOT--- */

double snrm2( n, sx, incx )
float	*sx;
int	n, incx;
/*
    PURPOSE
        Computes the Euclidean norm of sx while being
	very careful of distructive underflow and overflow.

    INPUT
        n       Number of elements to use.
        sx      Address of first element of x vector.
        incx    Incrament for the x vector (>0).

    OUPUT
        snrm2   Euclidean norm of sx.  Returns double
		due to `C' language features.
    REMARKS
        This algorithm proceeds in four steps.
	1) scan zero components.
	2) do phase 2 when component is near underflow.
*/
{
  register int i;
  int	 phase = 3;
  double sum = 0.0e0, cutlo, cuthi, hitst, r1mach();
  float xmax;

  if( n<1 || incx<1 ) return( sum );

  cutlo = sqrt( SMALLsp/r1mach() );			/* Calculate near underflow */
  cuthi = sqrt( HUGEsp );				/* Calculate near  overflow */
  hitst = cuthi/(double) n;
  i     = 0;

  /* Zero Sum. */
  while( *sx == 0.0 && i<n ) {
    i++;
    sx += incx;
  }
  if( i>=n ) return( sum );

START:
  if( abs( *sx ) > cutlo ) {
    for( ; i<n; i++, sx+=incx ) {		/* Loop over elements. */
      if( abs( *sx ) > hitst ) goto GOT_LARGE;  
      sum += (*sx) * (*sx);
    }
    sum = sqrt( sum );
    return( sum );				/* Sum completed normaly. */
  }
  else {					/* Small sum prepare for phase 2. */
    xmax  = abs( *sx );
    sx += incx;
    i++;
    sum += 1.0;
    for( ; i<n; i++, sx+=incx ) {
      if( abs( *sx ) > cutlo ) {		/* Got normal elem.  Rescale and process. */
	sum = (sum*xmax)*xmax;
	goto START;
      }
      if( abs( *sx ) > xmax ) {
	sum  = 1.0 + sum*(xmax /(*sx))*(xmax /(*sx));
	xmax = abs( *sx );
	continue;
      }
      sum += ((*sx)/xmax)*((*sx)/xmax);
    }
    return( (double)xmax*sqrt( sum ) );
  }						/* End of small sum. */

 GOT_LARGE:
  sum  = 1.0 + (sum/(*sx))/(*sx);		/* Rescale and process. */
  xmax = abs( *sx );
  sx   += incx;
  i++;
  for( ; i<n; i++, sx+=incx ) {
    if( abs( *sx ) > xmax ) {
      sum  = 1.0 + sum*(xmax /(*sx))*(xmax /(*sx));
      xmax = abs( *sx );
      continue;
    }
    sum += ((*sx)/xmax)*((*sx)/xmax);
  }
  return( (double)xmax*sqrt( sum ) );		/* End of small sum. */
}						/* End of ---SDOT--- */

double r1mach()
/* ---------------------------------------------------------------------
        This routine computes the unit roundoff for single precision 
	of the machine.  This is defined as the smallest positive 
	machine number u such that  1.0 + u .ne. 1.0
	Returns a double due to `C' language features.
----------------------------------------------------------------------*/
{
    float u = 1.0e0, comp;
 
    do {
        u *= 0.5e0;
        comp = 1.0e0 + u;
    }
    while( comp != 1.0e0 );
    return( (double)u*2.0e0 );
}
/*-------------------- end of function r1mach ------------------------*/
 
int min0( n, a, b, c, d, e, f, g, h, i, j, k, l, m, o, p )
/*
    PURPOSE
        Determine the minimum of the arguments a-p.
 
    INPUT
        n       Number of arguments to check 1 <= n <= 15.
        a-p     Integer arguments of which the minumum is desired.
 
    RETURNS
        min0    Minimum of a thru p.
*/
int n, a, b, c, d, e, f, g, h, i, j, k, l, m, o, p;
{
    int mt;
 
    if( n < 1 || n > 15 ) return( -1 );
    mt = a;
    if( n == 1 ) return( mt );
 
    if( mt > b ) mt = b;
    if( n == 2 ) return( mt );
 
    if( mt > c ) mt = c;
    if( n == 3 ) return( mt );
 
    if( mt > d ) mt = d;
    if( n == 4 ) return( mt );
 
    if( mt > e ) mt = e;
    if( n == 5 ) return( mt );
 
    if( mt > f ) mt = f;
    if( n == 6 ) return( mt );
 
    if( mt > g ) mt = g;
    if( n == 7 ) return( mt );
 
    if( mt > h ) mt = h;
    if( n == 8 ) return( mt );
 
    if( mt > i ) mt = i;
    if( n == 9 ) return( mt );
 
    if( mt > j  ) mt = j;
    if( n == 10 ) return( mt );
 
    if( mt > k  ) mt = k;
    if( n == 11 ) return( mt );
 
    if( mt > l  ) mt = l;
    if( n == 12 ) return( mt );
 
    if( mt > m ) mt = m;
    if( n == 13 ) return( mt );
 
    if( mt > o  ) mt = o;
    if( n == 14 ) return( mt );
 
    if( mt > p  ) mt = p;
    return( mt );
}
int sscal( n, sa, sx, incx )
int     n, incx;
float  sa, *sx;
/*
    PURPOSE
        Scales a vector by a constant.
 
    INPUT
        n    Number of components to scale.
        sa   Scale value.
        sx   Vector to scale.
        incx Every incx-th element of sx will be scaled.
 
    OUTPUT
        sx   Scaled vector.
*/
{
  int i;
 
  if( n < 1 ) return( 1 );

  /* Code for increment not equal to 1.*/
  if( incx != 1 ) {
    if( incx < 0 ) sx += (-n+1)*incx;
    for( i=0; i<n; i++, sx+=incx )
	  *sx *= sa;
        return 0;
  }
  /*  Code for unit increment. */
  for( i=0; i<n; i++, sx++ )
    *sx *= sa;
  return 0;
}
void vexopy( n, v, x, y, itype )
int	n, itype;
float	*v, *x, *y;
/*
  Purpose:
    To operate on the vectors x and y.

  Input:
    n		Number of elements to scale.
    x		First operand vector.
    y		Second operand vector.
    itype	Type of operation to perform:
		itype = 1 => '+'
		itype = 2 => '-'

  Output:
    v		Result vector of x op y.
*/
{
  register int i;

  if( n<1 ) return;

  if( itype == 1 )	/* ADDITION. */
    for( i=0; i<n; i++, x++, y++, v++ )
      *v = *x + *y;
  else			/* SUBTRACTION. */
    for( i=0; i<n; i++, x++, y++, v++ )
      *v = *x - *y;
}
void vfill( n, v, val )
int	n;
float	*v, val;
/*
  Purpose
    To fill the FLOAT vector v with the FLOAT value val.
    Make sure that if you pass a value for val that you cast
    it to float, viz.,
		vfill( 100, vector, (float)0.0 );
*/
{
  register int i;

  if( n<1 ) return;
  for( i=0; i<n; i++, v++ )
    *v = val;
}
