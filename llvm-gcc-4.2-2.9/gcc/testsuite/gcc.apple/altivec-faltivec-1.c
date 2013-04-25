/* APPLE LOCAL file AltiVec */
/* { dg-do compile { target powerpc*-*-* } } */
/* { dg-options "-mcpu=G3 -O3 -finline-limit=9999 -faltivec -Wa,-force_cpusubtype_ALL -fdump-ipa-cgraph -S" } */
/* Inliner should not inline AltiVec(tm) functions when -faltivec is on.  */
/* <rdar://problem/3837835> Selective inlining of functions that use Altivec */
#include	<Carbon/Carbon.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<math.h>
/* #include	<altivec.h> */

#define	N	400
#define	N4	((N+3)/4)
#define	N8	((N+7)/8)

typedef union 
{
  signed short		sInt[8];
  vector signed short	vInt;
} IntegerToVector;

static signed long vIntDotProduct (vector signed short [], vector signed short [], long int);
static int mainInt();
static Ptr getMemory (size_t);

static int
mainInt()
{
  long int			n = N, n8 = N8, m, i, j;
  signed long			vDotProduct, sDotProduct;
  signed short		*sx, *sy;
  vector signed short	*x, *y;
  IntegerToVector		*sX, *sY;
	
  sx = (short *) getMemory( 4*(n+3) );
  if (sx == nil)
    return 0;
  sy = (short *) getMemory( 4*(n+3) );
  if (sy == nil)
    return 0;
		
  x = (vector signed short *) getMemory( n8*16 );
  if ( x == nil)
    return 0;
  y = (vector signed short *) getMemory( n8*16 );
  if ( y == nil)
    return 0;
	
  sX = (IntegerToVector *) getMemory( n8*16 );
  if (sX == nil)
    return 0;
  sY = (IntegerToVector *) getMemory( n8*16 );
  if (sY == nil)
    return 0;
	
  for ( i = 0; i < n; i++ )
    {
      sx[i] = ( signed short ) scalb(( M_PI * ( double ) ( i ) / ( double ) n ), 8) + 0.5;
      sy[i] = ( signed short ) scalb(( M_PI * ( double ) ( n - i ) / ( double ) n ), 8) + 0.5;
    }
	
  m = n % 8;
  if (m != 0)
    for (i = n; i < n + 8 - m; i++)
      {
	sx[i] = 0.0;
	sy[i] = 0.0;
      }

  for ( i = 0; i < n8; i++ )
    for ( j = 0; j < 8; j++ )
      {
	sX[i].sInt[j] = sx[i*8+j];
	sY[i].sInt[j] = sy[i*8+j];
      }

  for ( i = 0; i < n8; i++ )
    {
      x[i] = sX[i].vInt;
      y[i] = sY[i].vInt;
    }
		
  vDotProduct = vIntDotProduct ( x, y, n8 );

  printf ( "\nVector dot product = %10d\n", (int) vDotProduct );
	
  return 0;
}

static Ptr
getMemory ( size_t amount )
{
  Ptr	ptr;
	
  ptr = malloc(amount);
  if (ptr ==  nil)
    printf ("\nUnable to allocate sufficient memory.");
  return (ptr);
}

signed long
vIntDotProduct ( vector signed short x[], vector signed short y[], long int n )
{

  typedef union 
  {
    signed long			xElem[4];
    vector signed int	vWord;
  } WordToVector;
	
  long int i;

  vector signed int partialProduct, zero = ( vector signed int ) { 0,0,0,0 };
  WordToVector sum;

  partialProduct = zero;
	
  for ( i = 0; i < n ; i++ )
    partialProduct = vec_msums ( x[i], y[i], partialProduct );	
	
  sum.vWord = vec_sums( partialProduct, zero);
		
  return sum.xElem[3];
}

int
main()
{
  mainInt();
  exit(0);
}

/* { dg-final { global compiler_flags; if ![string match "*-m64 *" $compiler_flags]
   { scan-tree-dump-times "callee has AltiVec" 2 "cgraph" } } } */
/* { dg-final { global compiler_flags; if ![string match "*-m64 *" $compiler_flags]
   { scan-assembler-not "vIntDotProduct" } } } */
/* { dg-final { global compiler_flags; if ![string match "*-m64 *" $compiler_flags]
   { scan-assembler "mainInt" } } } */
