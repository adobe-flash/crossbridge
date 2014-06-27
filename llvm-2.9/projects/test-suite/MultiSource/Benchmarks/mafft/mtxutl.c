#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "mtxutl.h"

void MtxuntDouble( double **mtx, int n )
{
    int i, j;
    for( i=0; i<n; i++ ) for( j=0; j<n; j++ ) mtx[i][j] = 0.0;
    for( i=0; i<n; i++ ) mtx[i][i] = 1.0;
}

void MtxmltDouble( double **mtx1, double **mtx2, int n )
{
    int i, j, k;
    double s, *tmp;

	tmp = (double *)calloc( n, sizeof( double ) );
    for( i=0; i<n; i++ ) 
    {
        for( k=0; k<n; k++ ) tmp[k] = mtx1[i][k];
        for( j=0; j<n; j++ ) 
        {
            s = 0.0;
            for( k=0; k<n; k++ ) s += tmp[k] * mtx2[k][j];
            mtx1[i][j] = s;
        }
    }
	free( tmp );
}

char *AllocateCharVec( int l1 )
{
	char *cvec;
	
	cvec = (char *)calloc( l1, sizeof( char ) );
	if( cvec == NULL )
	{
		fprintf( stderr, "Cannot allocate %d character vector.\n", l1 );
		exit( 1 );
	}
	return( cvec );
}
	
#if 0
void ReallocateCharMtx( char **mtx, int l1, int l2 )
{
	int i;
	char *bk = (char *)malloc( l2+1 ); // hontou ha iranai
	if( bk == NULL )
	{
		fprintf( stderr, "Cannot allocate bk in ReallocateCharMtx\n" );
		exit( 1 );
	}
	for( i=0; i<l1; i++ ) 
	{
#if 1
		strcpy( bk, mtx[i] );
		mtx[i] = (char *)realloc( mtx[i], (l2+1) * sizeof( char ) );
		if( mtx[i] == NULL )
		{
			fprintf( stderr, "Cannot reallocate %d x %d character matrix.\n", l1, l2 );
		}
		if( strcmp( bk, mtx[i] ) ) // hontou ha iranai
		{
			fprintf( stderr, "changed!! \n%s\n \nto\n%s\n in realloc..\n", bk, mtx[i] );
			strcpy( mtx[i], bk );
		}
#else
		strcpy( bk, mtx[i] );
		free( mtx[i] );
		mtx[i] = (char *)calloc( (l2+1), sizeof( char ) );
		strcpy( mtx[i], bk );
#endif
	}
	free( bk ); // hontou ha iranai
} 
#else
void ReallocateCharMtx( char **mtx, int l1, int l2 )
{
	int i;
	for( i=0; i<l1; i++ ) 
	{
		mtx[i] = (char *)realloc( mtx[i], (l2+1) * sizeof( char ) );
		if( mtx[i] == NULL )
		{
			fprintf( stderr, "Cannot reallocate %d x %d character matrix.\n", l1, l2 );
		}
	}
} 
#endif

char **AllocateCharMtx( int l1, int l2 )
{
	int i;
	char **cmtx;
	
	cmtx = (char **)calloc( l1+1, sizeof( char * ) );
	if( cmtx == NULL )
	{
		fprintf( stderr, "Cannot allocate %d x %d character matrix.\n", l1, l2 );
		exit( 1 );
	}   
	if( l2 )
	{
		for( i=0; i<l1; i++ ) 
		{
			cmtx[i] = AllocateCharVec( l2 );
		}
	}
	cmtx[l1] = NULL;
	return( cmtx );
} 

void FreeCharMtx( char **mtx )
{
/*
	char **x;
	x = mtx;
	while( *x != NULL ) free( *x++ );
	free( mtx );
*/
	int i;
	for( i=0; mtx[i]; i++ ) 
	{
		free( mtx[i] );
	}
	free( mtx );
}

float *AllocateFloatVec( int l1 )
{
	float *vec;

	vec = (float *)calloc( (unsigned int)l1, sizeof( float ) );
	if( vec == NULL )
	{
		fprintf( stderr, "Allocation error ( %d fload vec )\n", l1 );
		exit( 1 );
	}
	return( vec );
}

void FreeFloatVec( float *vec )
{
	free( (char *)vec );
}

float **AllocateFloatHalfMtx( int ll1 )
{
	float **mtx;
	int i;

	mtx = (float **)calloc( (unsigned int)ll1+1, sizeof( float * ) );
	if( mtx == NULL )
	{
		fprintf( stderr, "Allocation error ( %d fload halfmtx )\n", ll1 );
		exit( 1 );
	}
	for( i=0; i<ll1; i++ )
	{
		mtx[i] = (float *)calloc( ll1-i, sizeof( float ) );
		if( !mtx[i] )
		{
			fprintf( stderr, "Allocation error( %d floathalfmtx )\n", ll1 );
			exit( 1 );
		}
	}
	mtx[ll1] = NULL;
	return( mtx );
}

float **AllocateFloatMtx( int ll1, int ll2 )
{
	float **mtx;
	int i;

	mtx = (float **)calloc( (unsigned int)ll1+1, sizeof( float * ) );
	if( mtx == NULL )
	{
		fprintf( stderr, "Allocation error ( %d x %d fload mtx )\n", ll1, ll2 );
		exit( 1 );
	}
	if( ll2 )
	{
		for( i=0; i<ll1; i++ )
		{
			mtx[i] = (float *)calloc( ll2, sizeof( float ) );
			if( !mtx[i] )
			{
				fprintf( stderr, "Allocation error( %d x %d floatmtx )\n", ll1, ll2 );
				exit( 1 );
			}
		}
	}
	mtx[ll1] = NULL;
	return( mtx );
}

void FreeFloatHalfMtx( float **mtx, int n )
{
	int i;

	for( i=0; i<n; i++ ) 
	{
		if( mtx[i] ) FreeFloatVec( mtx[i] );
	}
	free( mtx );
}
void FreeFloatMtx( float **mtx )
{
	int i;

	for( i=0; mtx[i]; i++ ) 
	{
		FreeFloatVec( mtx[i] );
	}
	free( mtx );
}

int *AllocateIntVec( int ll1 )
{
	int *vec;

	vec = (int *)calloc( ll1, sizeof( int ) );
	if( vec == NULL )
	{	
		fprintf( stderr, "Allocation error( %d int vec )\n", ll1 );
		exit( 1 );
	}
	return( vec );
}	

void FreeIntVec( int *vec )
{
	free( (char *)vec );
}

float **AllocateFloatTri( int ll1 )
{
	float **tri;
	int i;

	tri = (float **)calloc( (unsigned int)ll1+1, sizeof( float * ) );
	if( !tri )
	{
		fprintf( stderr, "Allocation error ( float tri )\n" );
		exit( 1 );
	}
	for( i=0; i<ll1; i++ ) 
	{
		tri[i] = AllocateFloatVec( i+3 );
	}
	tri[ll1] = NULL;
		
	return( tri );
}

void FreeFloatTri( float **tri )
{
/*
	float **x;
	x = tri;
	while( *tri != NULL ) free( *tri++ );
	free( x );
*/
	int i;
	for( i=0; tri[i]; i++ ) 
		free( tri[i] );
	free( tri );
}
		
int **AllocateIntMtx( int ll1, int ll2 )
{
	int i;
	int **mtx;

	mtx = (int **)calloc( ll1+1, sizeof( int * ) );
	if( !mtx )
	{
		fprintf( stderr, "Allocation error( %d x %d int mtx )\n", ll1, ll2 );
		exit( 1 );
	}
	if( ll2 )
	{
		for( i=0; i<ll1; i++ ) 
		{
			mtx[i] = AllocateIntVec( ll2 );
		}
	}
	mtx[ll1] = NULL;
	return( mtx );
}

/*
void FreeIntMtx( int **mtx )
{
*
	int **x;
	x = mtx;
	while( !*mtx ) free( *mtx++ );
	free( x );
*
	int i;
	for( i=0; mtx[i] != NULL; i++ ) 
		free( (char *)mtx[i] );
	free( (char *)mtx );
}
*/

char ***AllocateCharCub( int ll1, int ll2, int  ll3 )
{
	int i;
	char ***cub;

	cub = (char ***)calloc( ll1+1, sizeof( char ** ) );
	if( !cub ) 
	{
		fprintf( stderr, "Allocation error( %d x %d x %d char cube\n", ll1, ll2, ll3 );
		exit( 1 );
	}
	if( ll2 )
	{
		for( i=0; i<ll1; i++ ) 
		{
			cub[i] = AllocateCharMtx( ll2, ll3 );
		}
	}
	cub[ll1] = NULL;
	return( cub );
}

void FreeCharCub( char ***cub )
{
	int i;

	for( i=0; cub[i]; i++ ) 
	{
		FreeCharMtx( cub[i] );
	}
	free( cub );
}

void freeintmtx( int **mtx, int ll1, int ll2 )
{
    int i;

    for( i=0; i<ll1; i++ ) 
        free( (char *)mtx[i] );
    free( (char *)mtx );
}
      
void FreeIntMtx( int **mtx )
{
	int i;

	for( i=0; mtx[i]; i++ ) 
		free( (char *)mtx[i] );
	free( (char *)mtx );
}

char ****AllocateCharHcu( int ll1, int ll2, int ll3, int ll4 )
{
	int i;
	char ****hcu;

	hcu = (char ****)calloc( ll1+1, sizeof( char *** ) );
	if( hcu == NULL ) exit( 1 );
	for( i=0; i<ll1; i++ ) 
		hcu[i] = AllocateCharCub( ll2, ll3, ll4 );
	hcu[ll1] = NULL;
	return( hcu );
}

void FreeCharHcu( char ****hcu )
{
	int i;
	for( i=0; hcu[i]; i++ )
	{
		FreeCharCub( hcu[i] );
	}
	free ( (char *)hcu );
}

double *AllocateDoubleVec( int ll1 )
{
	double *vec;

	vec = (double *)calloc( ll1, sizeof( double ) );
	return( vec );
}

void FreeDoubleVec( double *vec )
{
	free( vec );
}

int ***AllocateIntCub( int ll1, int ll2, int ll3 )
{
	int i;
	int ***cub;

	cub = (int ***)calloc( ll1+1, sizeof( int ** ) );
	if( cub == NULL )
	{
		fprintf( stderr, "cannot allocate IntCub\n" );
		exit( 1 );
	}
	for( i=0; i<ll1; i++ ) 
		cub[i] = AllocateIntMtx( ll2, ll3 );
	cub[ll1] = NULL;

	return cub;
}

void FreeIntCub( int ***cub )
{
	int i;
	for( i=0; cub[i]; i++ ) 
	{
		FreeIntMtx( cub[i] );
	}
	free( cub );
}

double **AllocateDoubleMtx( int ll1, int ll2 )
{
	int i;
	double **mtx;
	mtx = (double **)calloc( ll1+1, sizeof( double * ) );
	if( !mtx )
	{
		fprintf( stderr, "cannot allocate DoubleMtx\n" );
		exit( 1 );
	}
	if( ll2 )
	{
		for( i=0; i<ll1; i++ ) 
			mtx[i] = AllocateDoubleVec( ll2 );
	}
	mtx[ll1] = NULL;

	return mtx;
}

void FreeDoubleMtx( double **mtx )
{
	int i;
	for( i=0; mtx[i]; i++ )
		FreeDoubleVec( mtx[i] );
	free( mtx );
}

float ***AllocateFloatCub( int ll1, int ll2, int  ll3 )
{
	int i;
	float ***cub;

	cub = (float ***)calloc( ll1+1, sizeof( float ** ) );
	if( !cub ) 
	{
		fprintf( stderr, "cannot allocate float cube.\n" );
		exit( 1 );
	}
	for( i=0; i<ll1; i++ ) 
	{
		cub[i] = AllocateFloatMtx( ll2, ll3 );
	}
	cub[ll1] = NULL;
	return( cub );
}

void FreeFloatCub( float ***cub )
{
	int i;

	for( i=0; cub[i]; i++ ) 
	{
		FreeFloatMtx( cub[i] );
	}
	free( cub );
}

double ***AllocateDoubleCub( int ll1, int ll2, int  ll3 )
{
	int i;
	double ***cub;

	cub = (double ***)calloc( ll1+1, sizeof( double ** ) );
	if( !cub ) 
	{
		fprintf( stderr, "cannot allocate double cube.\n" );
		exit( 1 );
	}
	for( i=0; i<ll1; i++ ) 
	{
		cub[i] = AllocateDoubleMtx( ll2, ll3 );
	}
	cub[ll1] = NULL;
	return( cub );
}

void FreeDoubleCub( double ***cub )
{
	int i;

	for( i=0; cub[i]; i++ ) 
	{
		FreeDoubleMtx( cub[i] );
	}
	free( cub );
}


short *AllocateShortVec( int ll1 )
{
	short *vec;

	vec = (short *)calloc( ll1, sizeof( short ) );
	if( vec == NULL )
	{	
		fprintf( stderr, "Allocation error( %d short vec )\n", ll1 );
		exit( 1 );
	}
	return( vec );
}	

void FreeShortVec( short *vec )
{
	free( (char *)vec );
}

short **AllocateShortMtx( int ll1, int ll2 )
{
	int i;
	short **mtx;


	mtx = (short **)calloc( ll1+1, sizeof( short * ) );
	if( !mtx )
	{
		fprintf( stderr, "Allocation error( %d x %d short mtx ) \n", ll1, ll2 );
		exit( 1 );
	}
	for( i=0; i<ll1; i++ ) 
	{
		mtx[i] = AllocateShortVec( ll2 );
	}
	mtx[ll1] = NULL;
	return( mtx );
}

void FreeShortMtx( short **mtx )
{
	int i;

	for( i=0; mtx[i]; i++ ) 
		free( (char *)mtx[i] );
	free( (char *)mtx );
}

