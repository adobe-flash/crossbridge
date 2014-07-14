#include "mltaln.h"
#include "dp.h"

#define DEBUG 0
#define XXXXXXX    0
#define USE_PENALTY_EX  0

static void extendmseq( char **mseq1, char **mseq2, char **seq1, char **seq2, int i, int j, int prevhiti, int prevhitj )
{
	char gap[] = "-";
	int l;

	fprintf( stderr, "i=%d, prevhiti=%d\n", i, prevhiti );
	fprintf( stderr, "j=%d, prevhitj=%d\n", j, prevhitj );
	l = prevhiti - i - 1;
	fprintf( stderr, "l=%d\n", l );
	while( l>0 ) 
	{
		*--mseq1[0] = seq1[0][i+l--];
		*--mseq2[0] = *gap;
	}
	l= prevhitj - j - 1;
	fprintf( stderr, "l=%d\n", l );
	while( l>0 )
	{
		*--mseq1[0] = *gap;
		*--mseq2[0] = seq2[0][j+l--];
	}
	if( i < 0 || j < 0 ) return;
	*--mseq1[0] = seq1[0][i];
	*--mseq2[0] = seq2[0][j];
	fprintf( stderr, "added %c to mseq1, mseq1 = %s \n", seq1[0][i], mseq1[0] );
	fprintf( stderr, "added %c to mseq2, mseq2 = %s \n", seq2[0][j], mseq2[0] );
}

static void match_calc( float *match, char **s1, char **s2, int i1, int lgth2 )
{
	char tmpc = s1[0][i1];
	char *seq2 = s2[0];

	while( lgth2-- )
		*match++ = amino_dis[(int)tmpc][(int)*seq2++];
}

static float Atracking( float *lasthorizontalw, float *lastverticalw, 
						char **seq1, char **seq2, 
                        char **mseq1, char **mseq2, 
                        float **cpmx1, float **cpmx2, 
                        int **ijp )
{
	int i, j, l, iin, jin, ifi, jfi, lgth1, lgth2, k, limk;
	char gap[] = "-";
	lgth1 = strlen( seq1[0] );
	lgth2 = strlen( seq2[0] );


#if 0
	for( i=0; i<lgth1; i++ ) 
	{
		fprintf( stderr, "lastverticalw[%d] = %f\n", i, lastverticalw[i] );
	}
#endif
 
    for( i=0; i<lgth1+1; i++ ) 
    {
        ijp[i][0] = i + 1;
    }
    for( j=0; j<lgth2+1; j++ ) 
    {
        ijp[0][j] = -( j + 1 );
    }


	mseq1[0] += lgth1+lgth2;
	*mseq1[0] = 0;
	mseq2[0] += lgth1+lgth2;
	*mseq2[0] = 0;
	iin = lgth1; jin = lgth2;
	limk = lgth1+lgth2 + 1;
	for( k=0; k<limk; k++ ) 
	{
		if( ijp[iin][jin] < 0 ) 
		{
			ifi = iin-1; jfi = jin+ijp[iin][jin];
		}
		else if( ijp[iin][jin] > 0 )
		{
			ifi = iin-ijp[iin][jin]; jfi = jin-1;
		}
		else
		{
			ifi = iin-1; jfi = jin-1;
		}
		l = iin - ifi;
		while( --l ) 
		{
			*--mseq1[0] = seq1[0][ifi+l];
			*--mseq2[0] = *gap;
			k++;
		}
		l= jin - jfi;
		while( --l )
		{
			*--mseq1[0] = *gap;
			*--mseq2[0] = seq2[0][jfi+l];
			k++;
		}
		if( iin <= 0 || jin <= 0 ) break;
		*--mseq1[0] = seq1[0][ifi];
		*--mseq2[0] = seq2[0][jfi];
		k++;
		iin = ifi; jin = jfi;
	}
	return( 0.0 );
}

void backdp( float **WMMTX, float wmmax, float *maxinw, float *maxinh, int lgth1, int lgth2, int alloclen, float *w1, float *w2, float *initverticalw, float *m, int *mp, int iin, int jin, char **seq1, char **seq2, char **mseq1, char **mseq2 )
{
	register int i, j;
	int prevhiti, prevhitj;
//	int lasti, lastj; 
	float g;
	float fpenalty = (float)penalty;
#if USE_PENALTY_EX
	float fpenalty_ex = (float)penalty_ex;
#endif
	float *currentw, *previousw, *wtmp;
	float mi;
	int mpi;
	int *mpjpt;
	float *mjpt, *prept, *curpt;
	float wm = 0.0;
	float forwwm;

	currentw = w1;
	previousw = w2;

	match_calc( initverticalw, seq2, seq1, lgth2-1, lgth1 );
	match_calc( currentw, seq1, seq2, lgth1-1, lgth2 );


	prevhiti = iin;
	prevhitj = jin;
	fprintf( stderr, "prevhiti = %d, lgth1 = %d\n", prevhiti, lgth1 );
	fprintf( stderr, "prevhitj = %d, lgth2 = %d\n", prevhitj, lgth2 );
	extendmseq( mseq1, mseq2, seq1, seq2, prevhiti, prevhitj, lgth1, lgth2 );

	for( i=0; i<lgth1-1; i++ )
	{
		initverticalw[i] += fpenalty;
		WMMTX[i][lgth2-1] += fpenalty;
	}
	for( j=0; j<lgth2-1; j++ )
	{
		currentw[j] += fpenalty;
		WMMTX[lgth1-1][j] += fpenalty;
	}


#if 0
	fprintf( stderr, "initverticalw = \n" );
	for( i=0; i<lgth1; i++ )
		fprintf( stderr, "% 8.2f", initverticalw[i] );
	fprintf( stderr, "\n" );
	fprintf( stderr, "currentw = \n" );
	for( i=0; i<lgth2; i++ )
		fprintf( stderr, "% 8.2f", currentw[i] );
	fprintf( stderr, "\n" );
#endif

	for( j=lgth2-1; j>0; --j ) 
	{
		m[j-1] = currentw[j]; 
		mp[j] = 0; // iranai
	}

	for( j=0; j<lgth2; j++ ) m[j] = 0.0;
	//m[lgth2-1] ga, irunoka iranainoka irahai.

	for( i=lgth1-2; i>-1; i-- )
	{
		wtmp = previousw; 
		previousw = currentw;
		currentw = wtmp;

		previousw[lgth2-1] = initverticalw[i+1];

		match_calc( currentw, seq1, seq2, i, lgth2 );

#if 0
		fprintf( stderr, "i=%d, currentw = \n", i );
		for( j=0; j<lgth2; j++ ) fprintf( stderr, "% 8.2f", currentw[j] );
		fprintf( stderr, "\n" );
#endif

		currentw[lgth2-1] = initverticalw[i];

		mi = previousw[lgth2-1]; 
		mpi = lgth2-1; //iranai


		mjpt = m + lgth2 - 2;
		prept = previousw + lgth2 - 1;
		curpt = currentw + lgth2 - 2;
		mpjpt = mp + lgth2 - 2;

		for( j=lgth2-2; j>-1; j-- )
		{

//			fprintf( stderr, "i,j=%d,%d %c-%c ", i, j, seq1[0][i], seq2[0][j] );
			wm = *prept;
			g = mi + fpenalty;
#if 0
			fprintf( stderr, "%5.0f?", g );
#endif
			if( g > wm )
			{
				wm = g;
			}
			g = *prept;
			if( g >= mi )
			{
				mi = g;
				mpi = j+1; //iranai
			}
#if USE_PENALTY_EX
			mi += fpenalty_ex;
#endif

			g = *mjpt + fpenalty;
#if 0 
			fprintf( stderr, "%5.0f?", g );
#endif
			if( g > wm )
			{
				wm = g;
			}
			g = *prept;
			if( g >= *mjpt )
			{
				*mjpt = g;
				*mpjpt = i-1; //iranai
			}
#if USE_PENALTY_EX
			m[j] += fpenalty_ex;
#endif

#if 0
			fprintf( stderr, "*curpt = %5.0f \n", *curpt );
#endif

//			forwwm = wm + MIN( maxinw[i], maxinh[j] );
			forwwm = wm + MIN( maxinw[i], maxinh[j] );
			WMMTX[i][j] = forwwm;
			if( forwwm == wmmax && i<prevhiti && j<prevhitj ) 
			{
				fprintf( stderr, "hit!\n" );
				extendmseq( mseq1, mseq2, seq1, seq2, i, j, prevhiti, prevhitj );
				if( forwwm == wmmax )
				{
					*--mseq1[0] = 'u';
					*--mseq2[0] = 'u';
				}
				prevhiti = i;
				prevhitj = j;
			}
			*curpt += wm;

			mjpt--;
			prept--;
			mpjpt--;
			curpt--;
		}
	}
	extendmseq( mseq1, mseq2, seq1, seq2, -1, -1, prevhiti, prevhitj );
}


float MSalign11( char **seq1, char **seq2, int alloclen )
/* score no keisan no sai motokaraaru gap no atukai ni mondai ga aru */
{
//	int k;
	register int i, j;
	int lasti, lastj;
	int iin = 0, jin = 0;  // by Mathog, a guess
	int lgth1, lgth2;
	int resultlen;
	float wm = 0.0;   /* int ?????? */
	float g;
	float *currentw, *previousw;
	float fpenalty = (float)penalty;
#if USE_PENALTY_EX
	float fpenalty_ex = (float)penalty_ex;
#endif
	float *maxinw = NULL, *maxinwpt = NULL; // by D.Mathog, guess
	float *maxinh = NULL; // by D.Mathog, guess
#if 1
	float wmmax;
	float *wtmp;
	int *ijppt;
	float *mjpt, *prept, *curpt;
	int *mpjpt;
#endif
	static float mi, *m;
	static int **ijp;
	static int mpi, *mp;
	static float *w1, *w2;
	static float *match;
	static float *initverticalw;    /* kufuu sureba iranai */
	static float *lastverticalw;    /* kufuu sureba iranai */
	static char **mseq1;
	static char **mseq2;
	static char **mseq;
	static float **cpmx1;
	static float **cpmx2;
	static int **intwork;
	static float **WMMTX;
	static float **floatwork;
	static int orlgth1 = 0, orlgth2 = 0;

	if( orlgth1 == 0 )
	{
		mseq1 = AllocateCharMtx( njob, 0 );
		mseq2 = AllocateCharMtx( njob, 0 );
	}


	lgth1 = strlen( seq1[0] );
	lgth2 = strlen( seq2[0] );

	if( lgth1 > orlgth1 || lgth2 > orlgth2 )
	{
		int ll1, ll2;

		if( orlgth1 > 0 && orlgth2 > 0 )
		{
			FreeFloatVec( w1 );
			FreeFloatVec( w2 );
			FreeFloatVec( match );
			FreeFloatVec( initverticalw );
			FreeFloatVec( lastverticalw );
			FreeFloatVec( maxinw );
			FreeFloatVec( maxinh );

			FreeFloatVec( m );
			FreeIntVec( mp );

			FreeCharMtx( mseq );


			FreeFloatMtx( cpmx1 );
			FreeFloatMtx( cpmx2 );

			FreeFloatMtx( floatwork );
			FreeIntMtx( intwork );
		}

		ll1 = MAX( (int)(1.3*lgth1), orlgth1 ) + 100;
		ll2 = MAX( (int)(1.3*lgth2), orlgth2 ) + 100;

#if DEBUG
		fprintf( stderr, "\ntrying to allocate (%d+%d)xn matrices ... ", ll1, ll2 );
#endif

		w1 = AllocateFloatVec( ll2+2 );
		w2 = AllocateFloatVec( ll2+2 );
		match = AllocateFloatVec( ll2+2 );

		initverticalw = AllocateFloatVec( ll1+2 );
		lastverticalw = AllocateFloatVec( ll1+2 );
		maxinw = AllocateFloatVec( ll1+2 );


		m = AllocateFloatVec( ll2+2 );
		mp = AllocateIntVec( ll2+2 );
		maxinh = AllocateFloatVec( ll2+2 );

		mseq = AllocateCharMtx( njob, ll1+ll2 );

		cpmx1 = AllocateFloatMtx( 26, ll1+2 );
		cpmx2 = AllocateFloatMtx( 26, ll2+2 );

		floatwork = AllocateFloatMtx( 26, MAX( ll1, ll2 )+2 ); 
		intwork = AllocateIntMtx( 26, MAX( ll1, ll2 )+2 ); 

#if DEBUG
		fprintf( stderr, "succeeded\n" );
#endif

		orlgth1 = ll1 - 100;
		orlgth2 = ll2 - 100;
	}


	mseq1[0] = mseq[0];
	mseq2[0] = mseq[1];


	if( orlgth1 > commonAlloc1 || orlgth2 > commonAlloc2 )
	{
		int ll1, ll2;

		if( commonAlloc1 && commonAlloc2 )
		{
			FreeIntMtx( commonIP );
			FreeFloatMtx( WMMTX );
		}

		ll1 = MAX( orlgth1, commonAlloc1 );
		ll2 = MAX( orlgth2, commonAlloc2 );

#if DEBUG
		fprintf( stderr, "\n\ntrying to allocate %dx%d matrices ... ", ll1+1, ll2+1 );
#endif

		commonIP = AllocateIntMtx( ll1+10, ll2+10 );
		WMMTX = AllocateFloatMtx( ll1+10, ll2+10 );

#if DEBUG
		fprintf( stderr, "succeeded\n\n" );
#endif

		commonAlloc1 = ll1;
		commonAlloc2 = ll2;
	}
	ijp = commonIP;


#if 0
	for( i=0; i<lgth1; i++ ) 
		fprintf( stderr, "ogcp1[%d]=%f\n", i, ogcp1[i] );
#endif

	currentw = w1;
	previousw = w2;

	match_calc( initverticalw, seq2, seq1, 0, lgth1 );


	match_calc( currentw, seq1, seq2, 0, lgth2 );

	WMMTX[0][0] = initverticalw[0];

	maxinh[0] = initverticalw[0];
	for( i=1; i<lgth1+1; i++ )
	{
		initverticalw[i] += fpenalty;
		WMMTX[i][0] = initverticalw[i];
		if( maxinh[0] < initverticalw[i] ) maxinh[0] = initverticalw[i];
	}
	maxinw[0] = currentw[0];
	for( j=1; j<lgth2+1; j++ )
	{
		currentw[j] += fpenalty;
		WMMTX[0][j] = currentw[j];
		if( maxinw[0] < currentw[j] ) maxinw[0] = currentw[j];
	}

	for( j=1; j<lgth2+1; ++j ) 
	{
		m[j] = currentw[j-1]; mp[j] = 0;
	}

	lastverticalw[0] = currentw[lgth2-1];

	lasti = lgth1+1;

	for( i=1; i<lasti; i++ )
	{
		wtmp = previousw; 
		previousw = currentw;
		currentw = wtmp;

		previousw[0] = initverticalw[i-1];

		match_calc( currentw, seq1, seq2, i, lgth2 );

		currentw[0] = initverticalw[i];

		mi = previousw[0]; mpi = 0;

		maxinwpt = maxinw + i;
		*maxinwpt = currentw[0];

		fprintf( stderr, "currentw[0]  = %f, *maxinwpt = %f\n", currentw[0], maxinw[i] );

		ijppt = ijp[i] + 1;
		mjpt = m + 1;
		prept = previousw;
		curpt = currentw + 1;
		mpjpt = mp + 1;
		lastj = lgth2+1;

		for( j=1; j<lastj; j++ )
		{
			wm = *prept;
			*ijppt = 0;

#if 0
			fprintf( stderr, "%5.0f->", wm );
#endif
			g = mi + fpenalty;
#if 0
			fprintf( stderr, "%5.0f?", g );
#endif
			if( g > wm )
			{
				wm = g;
				*ijppt = -( j - mpi );
			}
			g = *prept;
			if( g >= mi )
			{
				mi = g;
				mpi = j-1;
			}
#if USE_PENALTY_EX
			mi += fpenalty_ex;
#endif

			g = *mjpt + fpenalty;
#if 0 
			fprintf( stderr, "%5.0f?", g );
#endif
			if( g > wm )
			{
				wm = g;
				*ijppt = +( i - *mpjpt );
			}
			g = *prept;
			if( g >= *mjpt )
			{
				*mjpt = g;
				*mpjpt = i-1;
			}
#if USE_PENALTY_EX
			m[j] += fpenalty_ex;
#endif

#if 0
			fprintf( stderr, "%5.0f ", wm );
#endif
			*curpt += wm;

			WMMTX[i][j] = *curpt;


			if( j<lgth2 && *maxinwpt < *curpt ) *maxinwpt = *curpt;
			if( j<lgth2 && maxinh[j] < *curpt ) maxinh[j] = *curpt;
//			fprintf( stderr, "maxintwpt = %f\n", *maxinwpt );

			ijppt++;
			mjpt++;
			prept++;
			mpjpt++;
			curpt++;
		}
		lastverticalw[i] = currentw[lgth2-1];
	}

	wmmax = -999.9;
	for( i=0; i<lgth1; i++ )
	{
		g = lastverticalw[i];
		if( g > wmmax ) 
		{
			wmmax = g;
			iin = i;
			jin = lgth2-1;
		}
	}
	for( j=0; j<lgth2; j++ )
	{
		g = currentw[j];
		if( g > wmmax )
		{
			wmmax = g;
			iin = lgth1-1;
			jin = j;
		}
	}

	for( i=0; i<lgth1; i++ )
		fprintf( stderr, "maxinw[%d] = %f\n", i, maxinw[i] );
	for( j=0; j<lgth2; j++ )
		fprintf( stderr, "maxinh[%d] = %f\n", j, maxinh[j] );

	fprintf( stderr, "wmmax = %f (%d,%d)\n", wmmax, iin, jin );
	if( iin == lgth1 - 1 && jin == lgth2 - 1 )
		;
	else
		wmmax += fpenalty;

	fprintf( stderr, "wmmax = %f\n", wmmax );

#if 0
	for( i=0; i<lgth1; i++ )
	{
		for( j=0; j<lgth2; j++ )
		{
			fprintf( stderr, "% 10.2f ", WMMTX[i][j] );
		}
		fprintf( stderr, "\n" );
	}
#endif

	mseq1[0] += lgth1+lgth2;
	*mseq1[0] = 0;
	mseq2[0] += lgth1+lgth2;
	*mseq2[0] = 0;

	backdp( WMMTX, wmmax, maxinw, maxinh, lgth1, lgth2, alloclen, w1, w2, initverticalw, m, mp, iin, jin, seq1, seq2, mseq1, mseq2 );

	fprintf( stderr, "\n" );
#if 1
	fprintf( stderr, "\n" );
	fprintf( stderr, ">MSres\n%s\n", mseq1[0] );
	fprintf( stderr, ">MSres\n%s\n", mseq2[0] );
#endif

#if 0
	for( i=0; i<lgth1; i++ )
	{
		for( j=0; j<lgth2; j++ )
		{
			fprintf( stderr, "% 10.2f ", WMMTX[i][j] );
		}
		fprintf( stderr, "\n" );
	}
#endif

	mseq1[0] = mseq[0];
	mseq2[0] = mseq[1];
	mseq1[0] += lgth1+lgth2;
	*mseq1[0] = 0;
	mseq2[0] += lgth1+lgth2;
	*mseq2[0] = 0;

	Atracking( currentw, lastverticalw, seq1, seq2, mseq1, mseq2, cpmx1, cpmx2, ijp );


	resultlen = strlen( mseq1[0] );
	if( alloclen < resultlen || resultlen > N )
	{
		fprintf( stderr, "alloclen=%d, resultlen=%d, N=%d\n", alloclen, resultlen, N );
		ErrorExit( "LENGTH OVER!\n" );
	}


	strcpy( seq1[0], mseq1[0] );
	strcpy( seq2[0], mseq2[0] );
#if 1
	fprintf( stderr, "\n" );
	fprintf( stderr, ">\n%s\n", mseq1[0] );
	fprintf( stderr, ">\n%s\n", mseq2[0] );
#endif


	return( wm );
}

