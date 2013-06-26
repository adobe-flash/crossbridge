#include "mltaln.h"
#include "dp.h"

#define DEBUG 0
#define DEBUG2 0
#define XXXXXXX    0
#define USE_PENALTY_EX  1

typedef struct _shuryoten
{
	int i;
	int j;
	float wm;
	struct _shuryoten *next;
	struct _shuryoten *prev;
} Shuryoten;


static int localstop;

static int compshuryo( Shuryoten *s1_arg, Shuryoten *s2_arg )
{
	Shuryoten *s1 = (Shuryoten *)s1_arg;
	Shuryoten *s2 = (Shuryoten *)s2_arg;
	if      ( s1->wm > s2->wm ) return( -1 );
	else if ( s1->wm < s2->wm ) return( 1 );
	else                        return( 0 );
}

static void match_calc( float *match, char **s1, char **s2, int i1, int lgth2 )
{
	int j;

	for( j=0; j<lgth2; j++ )
		match[j] = amino_dis[(int)(*s1)[i1]][(int)(*s2)[j]];
}

static float gentracking( int **used,
						char **seq1, char **seq2, 
                        char **mseq1, char **mseq2, 
                        float **cpmx1, float **cpmx2, 
                        int **ijpi, int **ijpj, int *off1pt, int *off2pt, int endi, int endj )
{
	int l, iin, jin, lgth1, lgth2, k, limk;
	int ifi=0, jfi=0;
	char gap[] = "-";
	static char *res1 = NULL, *res2 = NULL;
	char *mspt1, *mspt2;
	if( res1 == NULL )
	{
		res1 = (char *)calloc( N, sizeof( char ) );
		res2 = (char *)calloc( N, sizeof( char ) );
	}

	lgth1 = strlen( seq1[0] );
	lgth2 = strlen( seq2[0] );

	mspt1 = res1 + lgth1+lgth2;
	*mspt1 = 0;
	mspt2 = res2 + lgth1+lgth2;
	*mspt2 = 0;
	iin = endi; jin = endj;

	limk = lgth1+lgth2;
	if( used[iin][jin] ) return( -1.0 );
	for( k=0; k<=limk; k++ ) 
	{
		ifi = ( ijpi[iin][jin] );
		jfi = ( ijpj[iin][jin] );

		if( used[ifi][jfi] ) return( -1.0 );

		l = iin - ifi;
		while( --l ) 
		{
			*--mspt1 = seq1[0][ifi+l];
			*--mspt2 = *gap;
			k++;
		}
		l= jin - jfi;
		while( --l )
		{
			*--mspt1 = *gap;
			*--mspt2 = seq2[0][jfi+l];
			k++;
		}

		if( iin <= 0 || jin <= 0 ) break;
		*--mspt1 = seq1[0][ifi];
		*--mspt2 = seq2[0][jfi];
		if( ijpi[ifi][jfi] == localstop ) break;
		if( ijpj[ifi][jfi] == localstop ) break;
		k++;
		iin = ifi; jin = jfi;
	}
	if( ifi == -1 ) *off1pt = 0; else *off1pt = ifi;
	if( jfi == -1 ) *off2pt = 0; else *off2pt = jfi;

//	fprintf( stderr, "ifn = %d, jfn = %d\n", ifi, jfi );

	iin = endi; jin = endj;
	limk = lgth1+lgth2;
	for( k=0; k<=limk; k++ ) 
	{
		ifi = ( ijpi[iin][jin] );
		jfi = ( ijpj[iin][jin] );

		used[ifi][jfi] = 1;
		if( iin <= 0 || jin <= 0 ) break;
		if( ijpi[ifi][jfi] == localstop ) break;
		if( ijpj[ifi][jfi] == localstop ) break;

		k++;
		iin = ifi; jin = jfi;
	}


	strcpy( mseq1[0], mspt1 );
	strcpy( mseq2[0], mspt2 );

	fprintf( stderr, "mseq1=%s\nmseq2=%s\n", mspt1, mspt2 );

	return( 0.0 );
}


float suboptalign11( char **seq1, char **seq2, int alloclen, int *off1pt, int *off2pt, LocalHom *lhmpt )
/* score no keisan no sai motokaraaru gap no atukai ni mondai ga aru */
{
	int k;
	static int **used;
	register int i, j;
	int lasti, lastj;                      /* outgap == 0 -> lgth1, outgap == 1 -> lgth1+1 */
	int lgth1, lgth2;
	int resultlen;
	float wm = 0.0;   // by D.Mathog, 
	float g;
	float *currentw, *previousw;
#if 1
	float *wtmp;
	int *ijpipt;
	int *ijpjpt;
	float *mjpt, *Mjpt, *prept, *curpt;
	int *mpjpt, *Mpjpt;
#endif
	static float mi, *m;
	static float Mi, *largeM;
	static int **ijpi;
	static int **ijpj;
	static int mpi, *mp;
	static int Mpi, *Mp;
	static float *w1, *w2;
//	static float *match;
	static float *initverticalw;    /* kufuu sureba iranai */
	static float *lastverticalw;    /* kufuu sureba iranai */
	static char **mseq1;
	static char **mseq2;
	static float **cpmx1;
	static float **cpmx2;
	static int **intwork;
	static float **floatwork;
	static int orlgth1 = 0, orlgth2 = 0;
	float maxwm;
	float tbk;
	int tbki, tbkj;
	int endali, endalj;
//	float localthr = 0.0;
//	float localthr2 = 0.0;
	float fpenalty = (float)penalty;
	float fpenalty_OP = (float)penalty_OP;
	float fpenalty_ex = (float)penalty_ex;
//	float fpenalty_EX = (float)penalty_EX;
	float foffset = (float)offset;
	float localthr = -foffset;
	float localthr2 = -foffset;
	static Shuryoten *shuryo = NULL;
	int numshuryo;
	float minshuryowm = 0.0; // by D.Mathog
	int minshuryopos = 0; // by D.Mathog
	float resf;


//	fprintf( stderr, "@@@@@@@@@@@@@ penalty_OP = %f, penalty_EX = %f, pelanty = %f\n", fpenalty_OP, fpenalty_EX, fpenalty );

	fprintf( stderr, "in suboptalign11\n" );
	if( !shuryo )
	{
		shuryo = (Shuryoten *)calloc( 100, sizeof( Shuryoten ) );
	}
	for( i=0; i<100; i++ )
	{
		shuryo[i].i = -1;
		shuryo[i].j = -1;
		shuryo[i].wm = 0.0;
	}
	numshuryo = 0;

	if( orlgth1 == 0 )
	{
	}


	lgth1 = strlen( seq1[0] );
	lgth2 = strlen( seq2[0] );

	fprintf( stderr, "in suboptalign11 step 1\n" );

	if( lgth1 > orlgth1 || lgth2 > orlgth2 )
	{
		int ll1, ll2;

	fprintf( stderr, "in suboptalign11 step 1.3\n" );
		if( orlgth1 > 0 && orlgth2 > 0 )
		{
	fprintf( stderr, "in suboptalign11 step 1.4\n" );
			FreeFloatVec( w1 );
			FreeFloatVec( w2 );
//			FreeFloatVec( match );
			FreeFloatVec( initverticalw );
			FreeFloatVec( lastverticalw );
	fprintf( stderr, "in suboptalign11 step 1.5\n" );

			FreeFloatVec( m );
			FreeIntVec( mp );
			FreeFloatVec( largeM );
			FreeIntVec( Mp );
	fprintf( stderr, "in suboptalign11 step 1.6\n" );


			FreeFloatMtx( cpmx1 );
			FreeFloatMtx( cpmx2 );

	fprintf( stderr, "in suboptalign11 step 1.7\n" );
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
//		match = AllocateFloatVec( ll2+2 );

		initverticalw = AllocateFloatVec( ll1+2 );
		lastverticalw = AllocateFloatVec( ll1+2 );

		m = AllocateFloatVec( ll2+2 );
		mp = AllocateIntVec( ll2+2 );
		largeM = AllocateFloatVec( ll2+2 );
		Mp = AllocateIntVec( ll2+2 );

		cpmx1 = AllocateFloatMtx( 26, ll1+2 );
		cpmx2 = AllocateFloatMtx( 26, ll2+2 );

		floatwork = AllocateFloatMtx( 26, MAX( ll1, ll2 )+2 ); 
		intwork = AllocateIntMtx( 26, MAX( ll1, ll2 )+2 ); 

		mseq1 = AllocateCharMtx( njob, ll1+ll2 );
		mseq2 = AllocateCharMtx( njob, ll1+ll2 );

#if DEBUG
		fprintf( stderr, "succeeded\n" );
#endif

		orlgth1 = ll1 - 100;
		orlgth2 = ll2 - 100;
	}
	fprintf( stderr, "in suboptalign11 step 1.6\n" );



	fprintf( stderr, "in suboptalign11 step 2\n" );

	if( orlgth1 > commonAlloc1 || orlgth2 > commonAlloc2 )
	{
		int ll1, ll2;

		if( commonAlloc1 && commonAlloc2 )
		{
			FreeIntMtx( commonIP );
			FreeIntMtx( commonJP );
			FreeIntMtx( used );
		}

		ll1 = MAX( orlgth1, commonAlloc1 );
		ll2 = MAX( orlgth2, commonAlloc2 );

#if DEBUG
		fprintf( stderr, "\n\ntrying to allocate %dx%d matrices ... ", ll1+1, ll2+1 );
#endif

		used = AllocateIntMtx( ll1+10, ll2+10 );
		commonIP = AllocateIntMtx( ll1+10, ll2+10 );
		commonJP = AllocateIntMtx( ll1+10, ll2+10 );

#if DEBUG
		fprintf( stderr, "succeeded\n\n" );
#endif

		commonAlloc1 = ll1;
		commonAlloc2 = ll2;
	}
	ijpi = commonIP;
	ijpj = commonJP;


#if 0
	for( i=0; i<lgth1; i++ ) 
		fprintf( stderr, "ogcp1[%d]=%f\n", i, ogcp1[i] );
#endif

	fprintf( stderr, "in suboptalign11 step 3\n" );
	currentw = w1;
	previousw = w2;

	match_calc( initverticalw, seq2, seq1, 0, lgth1 );

	match_calc( currentw, seq1, seq2, 0, lgth2 );


	lasti = lgth2+1;
	for( j=1; j<lasti; ++j ) 
	{
		m[j] = currentw[j-1]; mp[j] = 0;
		largeM[j] = currentw[j-1]; Mp[j] = 0;
	}

	lastverticalw[0] = currentw[lgth2-1];


#if 0
fprintf( stderr, "currentw = \n" );
for( i=0; i<lgth1+1; i++ )
{
	fprintf( stderr, "%5.2f ", currentw[i] );
}
fprintf( stderr, "\n" );
fprintf( stderr, "initverticalw = \n" );
for( i=0; i<lgth2+1; i++ )
{
	fprintf( stderr, "%5.2f ", initverticalw[i] );
}
fprintf( stderr, "\n" );
#endif
#if DEBUG2
	fprintf( stderr, "\n" );
	fprintf( stderr, "       " );
	for( j=0; j<lgth2+1; j++ )
		fprintf( stderr, "%c     ", seq2[0][j] );
	fprintf( stderr, "\n" );
#endif

	localstop = lgth1+lgth2+1;
	maxwm = -999.9;
	endali = endalj = 0;
#if DEBUG2
	fprintf( stderr, "\n" );
	fprintf( stderr, "%c   ", seq1[0][0] );

	for( j=0; j<lgth2+1; j++ )
		fprintf( stderr, "%5.0f ", currentw[j] );
	fprintf( stderr, "\n" );
#endif

	lasti = lgth1+1;
	for( i=1; i<lasti; i++ )
	{
		wtmp = previousw; 
		previousw = currentw;
		currentw = wtmp;

		previousw[0] = initverticalw[i-1];

		match_calc( currentw, seq1, seq2, i, lgth2 );
#if DEBUG2
		fprintf( stderr, "%c   ", seq1[0][i] );
		fprintf( stderr, "%5.0f ", currentw[0] );
#endif

#if XXXXXXX
fprintf( stderr, "\n" );
fprintf( stderr, "i=%d\n", i );
fprintf( stderr, "currentw = \n" );
for( j=0; j<lgth2; j++ )
{
	fprintf( stderr, "%5.2f ", currentw[j] );
}
fprintf( stderr, "\n" );
#endif
#if XXXXXXX
fprintf( stderr, "\n" );
fprintf( stderr, "i=%d\n", i );
fprintf( stderr, "currentw = \n" );
for( j=0; j<lgth2; j++ )
{
	fprintf( stderr, "%5.2f ", currentw[j] );
}
fprintf( stderr, "\n" );
#endif
		currentw[0] = initverticalw[i];

		mi = previousw[0]; mpi = 0;
		Mi = previousw[0]; Mpi = 0;

#if 0
		if( mi < localthr ) mi = localthr2;
#endif

		ijpipt = ijpi[i] + 1;
		ijpjpt = ijpj[i] + 1;
		mjpt = m + 1;
		Mjpt = largeM + 1;
		prept = previousw;
		curpt = currentw + 1;
		mpjpt = mp + 1;
		Mpjpt = Mp + 1;
		tbk = -999999.9;
		tbki = 0;
		tbkj = 0;
		lastj = lgth2+1;
		for( j=1; j<lastj; j++ )
		{
			wm = *prept;
			*ijpipt = i-1;
			*ijpjpt = j-1;


//			fprintf( stderr, "i,j=%d,%d %c-%c\n", i, j, seq1[0][i], seq2[0][j] );
//			fprintf( stderr, "wm=%f\n", wm );
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
//				*ijpipt = i - 1;
				*ijpjpt = mpi;
			}
			g = *prept;
			if( g > mi )
			{
				mi = g;
				mpi = j-1;
			}

#if USE_PENALTY_EX
			mi += fpenalty_ex;
#endif

#if 0
			fprintf( stderr, "%5.0f->", wm );
#endif
			g = *mjpt + fpenalty;
#if 0
			fprintf( stderr, "m%5.0f?", g );
#endif
			if( g > wm )
			{
				wm = g;
				*ijpipt = *mpjpt;
//				*ijpjpt = j - 1;
			}
			g = *prept;
			if( g > *mjpt )
			{
				*mjpt = g;
				*mpjpt = i-1;
			}
#if USE_PENALTY_EX
			*mjpt += fpenalty_ex;
#endif


			g =  tbk + fpenalty_OP;
//			g =  tbk;
			if( g > wm )
			{
				wm = g;
				*ijpipt = tbki;
				*ijpjpt = tbkj;
//				fprintf( stderr, "hit! i%d, j%d, ijpi = %d, ijpj = %d\n", i, j, *ijpipt, *ijpjpt );
			}
			g = Mi;
			if( g > tbk )
			{
				tbk = g;
				tbki = i-1;
				tbkj = Mpi;
			}
			g = *Mjpt;
			if( g > tbk )
			{
				tbk = g;
				tbki = *Mpjpt;
				tbkj = j-1;
			}
//			tbk += fpenalty_EX;// + foffset;

			g = *prept;
			if( g > *Mjpt )
			{
				*Mjpt = g;
				*Mpjpt = i-1;
			}
//			*Mjpt += fpenalty_EX;// + foffset;

			g = *prept;
			if( g > Mi )
			{
				Mi = g;
				Mpi = j-1;
			}
//			Mi += fpenalty_EX;// + foffset;


//			fprintf( stderr, "wm=%f, tbk=%f(%c-%c), mi=%f, *mjpt=%f\n", wm, tbk, seq1[0][tbki], seq2[0][tbkj], mi, *mjpt );
//			fprintf( stderr, "ijp = %c,%c\n", seq1[0][abs(*ijpipt)], seq2[0][abs(*ijpjpt)] );


			if( maxwm < wm )
			{
				maxwm = wm;
				endali = i;
				endalj = j;
			}

#if 1
			if( numshuryo < 100 )
			{
				shuryo[numshuryo].i = i;
				shuryo[numshuryo].j = j;
				shuryo[numshuryo].wm = wm;

				if( minshuryowm > wm )
				{
					 minshuryowm = wm;
					 minshuryopos = numshuryo;
				}
				numshuryo++;
			}
			else
			{
				if( wm > minshuryowm )
				{
					shuryo[minshuryopos].i = i;
					shuryo[minshuryopos].j = j;
					shuryo[minshuryopos].wm = wm;
					minshuryowm = wm;
					for( k=0; k<100; k++ )    // muda
					{
						if( shuryo[k].wm < minshuryowm )
						{
							minshuryowm = shuryo[k].wm;
							minshuryopos = k;
							break;
						}
					}
				}
			}
#endif
#if 1
			if( wm < localthr )
			{
//				fprintf( stderr, "stop i=%d, j=%d, curpt=%f\n", i, j, *curpt );
				*ijpipt = localstop;
//				*ijpjpt = localstop;
				wm = localthr2;
			}
#endif
#if 0
			fprintf( stderr, "%5.0f ", *curpt );
#endif
#if DEBUG2
			fprintf( stderr, "%5.0f ", wm );
//			fprintf( stderr, "%c-%c *ijppt = %d, localstop = %d\n", seq1[0][i], seq2[0][j], *ijppt, localstop );
#endif

			*curpt += wm;
			ijpipt++;
			ijpjpt++;
			mjpt++;
			Mjpt++;
			prept++;
			mpjpt++;
			Mpjpt++;
			curpt++;
		}
#if DEBUG2
		fprintf( stderr, "\n" );
#endif

		lastverticalw[i] = currentw[lgth2-1];
	}

	for( k=0; k<100; k++ )
	{
		fprintf( stderr, "shuryo[%d].i,j,wm = %d,%d,%f\n", k, shuryo[k].i, shuryo[k].j, shuryo[k].wm );
	}


#if 1
	fprintf( stderr, "maxwm = %f\n", maxwm );
	fprintf( stderr, "endali = %d\n", endali );
	fprintf( stderr, "endalj = %d\n", endalj );
#endif

	qsort( shuryo, 100, sizeof( Shuryoten ), (int (*)())compshuryo );
	for( k=0; k<100; k++ )
	{
		fprintf( stderr, "shuryo[%d].i,j,wm = %d,%d,%f\n", k, shuryo[k].i, shuryo[k].j, shuryo[k].wm );
	}

		
	lasti = lgth1+1;
    for( i=0; i<lasti; i++ ) 
    {
        ijpi[i][0] = localstop;
        ijpj[i][0] = localstop;
    }
	lastj = lgth2+1;
    for( j=0; j<lastj; j++ ) 
    {
        ijpi[0][j] = localstop;
        ijpj[0][j] = localstop;
    }

	for( i=0; i<lasti; i++ ) for( j=0; j<lastj; j++ ) used[i][j] = 0;

	for( k=0; k<numshuryo; k++ )
	{
		if( shuryo[k].wm < shuryo[0].wm * 0.3 ) break;
		fprintf( stderr, "k=%d, shuryo[k].i,j,wm=%d,%d,%f go\n", k, shuryo[k].i, shuryo[k].j, shuryo[k].wm );
		resf = gentracking( used, seq1, seq2, mseq1, mseq2, cpmx1, cpmx2, ijpi, ijpj, off1pt, off2pt, shuryo[k].i, shuryo[k].j );
		if( resf == -1.0 ) continue;
		putlocalhom3( mseq1[0], mseq2[0], lhmpt, *off1pt, *off2pt, (int)shuryo[k].wm, strlen( mseq1[0] ) );
#if 0
		fprintf( stderr, "\n" );
		fprintf( stderr, ">\n%s\n", mseq1[0] );
		fprintf( stderr, ">\n%s\n", mseq2[0] );
#endif
	}
	for( i=0; i<20; i++ )
	{
		for( j=0; j<20; j++ )
		{
			fprintf( stderr, "%2d ", used[i][j] );
		}
		fprintf( stderr, "\n" );
	}


//	fprintf( stderr, "### impmatch = %f\n", *impmatch );

	resultlen = strlen( mseq1[0] );
	if( alloclen < resultlen || resultlen > N )
	{
		fprintf( stderr, "alloclen=%d, resultlen=%d, N=%d\n", alloclen, resultlen, N );
		ErrorExit( "LENGTH OVER!\n" );
	}





	return( wm );
}

