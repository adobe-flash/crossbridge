#include "mltaln.h"
#include "dp.h"

#define DEBUG 0
#define DEBUG2 0
#define XXXXXXX    0
#define USE_PENALTY_EX  1

static int localstop;

#if 1
static void match_calc( float *match, char **s1, char **s2, int i1, int lgth2 ) 
{
	char tmpc = s1[0][i1];
	char *seq2 = s2[0];

	while( lgth2-- )
		*match++ = amino_dis[(int)tmpc][(int)*seq2++];
}
#else
static void match_calc( float *match, char **s1, char **s2, int i1, int lgth2 )
{
	int j;

	for( j=0; j<lgth2; j++ )
		match[j] = amino_dis[(*s1)[i1]][(*s2)[j]];
}
#endif

#if 0
static void match_calc_bk( float *match, float **cpmx1, float **cpmx2, int i1, int lgth2, float **floatwork, int **intwork, int initialize )
{
	int j, k, l;
	float scarr[26];
	float **cpmxpd = floatwork;
	int **cpmxpdn = intwork;
	int count = 0;

	if( initialize )
	{
		for( j=0; j<lgth2; j++ )
		{
			count = 0;
			for( l=0; l<26; l++ )
			{
				if( cpmx2[l][j] )
				{
					cpmxpd[count][j] = cpmx2[l][j];
					cpmxpdn[count][j] = l;
					count++;
				}
			}
			cpmxpdn[count][j] = -1;
		}
	}

	for( l=0; l<26; l++ )
	{
		scarr[l] = 0.0;
		for( k=0; k<26; k++ )
			scarr[l] += n_dis[k][l] * cpmx1[k][i1];
	}
#if 0 
	{
		float *fpt, **fptpt, *fpt2;
		int *ipt, **iptpt;
		fpt2 = match;
		iptpt = cpmxpdn;
		fptpt = cpmxpd;
		while( lgth2-- )
		{
			*fpt2 = 0.0;
			ipt=*iptpt,fpt=*fptpt;
			while( *ipt > -1 )
				*fpt2 += scarr[*ipt++] * *fpt++;
			fpt2++,iptpt++,fptpt++;
		} 
	}
#else
	for( j=0; j<lgth2; j++ )
	{
		match[j] = 0.0;
		for( k=0; cpmxpdn[k][j]>-1; k++ )
			match[j] += scarr[cpmxpdn[k][j]] * cpmxpd[k][j];
	} 
#endif
}
#endif

static float gentracking( float *lasthorizontalw, float *lastverticalw, 
						char **seq1, char **seq2, 
                        char **mseq1, char **mseq2, 
                        float **cpmx1, float **cpmx2, 
                        int **ijpi, int **ijpj, int *off1pt, int *off2pt, int endi, int endj )
{
	int i, j, l, iin, jin, lgth1, lgth2, k, limk;
	int ifi=0, jfi=0; // by D.Mathog
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
        ijpi[i][0] = localstop;
        ijpj[i][0] = localstop;
    }
    for( j=0; j<lgth2+1; j++ ) 
    {
        ijpi[0][j] = localstop;
        ijpj[0][j] = localstop;
    }

	mseq1[0] += lgth1+lgth2;
	*mseq1[0] = 0;
	mseq2[0] += lgth1+lgth2;
	*mseq2[0] = 0;
	iin = endi; jin = endj;
	limk = lgth1+lgth2;
	for( k=0; k<=limk; k++ ) 
	{

		ifi = ( ijpi[iin][jin] );
		jfi = ( ijpj[iin][jin] );
		l = iin - ifi;
//		if( ijpi[iin][jin] < 0 || ijpj[iin][jin] < 0 )
//		{
//			fprintf( stderr, "skip! %d-%d\n", ijpi[iin][jin], ijpj[iin][jin] );
//			fprintf( stderr, "1: %c-%c\n", seq1[0][iin], seq1[0][ifi] );
//			fprintf( stderr, "2: %c-%c\n", seq2[0][jin], seq2[0][jfi] );
//		}
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

		if( ijpi[ifi][jfi] == localstop ) break;
		if( ijpj[ifi][jfi] == localstop ) break; 
		k++;
		iin = ifi; jin = jfi;
	}
	if( ifi == -1 ) *off1pt = 0; else *off1pt = ifi;
	if( jfi == -1 ) *off2pt = 0; else *off2pt = jfi;

//	fprintf( stderr, "ifn = %d, jfn = %d\n", ifi, jfi );


	return( 0.0 );
}


float genL__align11( char **seq1, char **seq2, int alloclen, int *off1pt, int *off2pt )
/* score no keisan no sai motokaraaru gap no atukai ni mondai ga aru */
{
//	int k;
	register int i, j;
	int lasti, lastj; 
	int lgth1, lgth2;
	int resultlen;
	float wm = 0.0;   /* int ?????? */
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
	static float *match;
	static float *initverticalw;    /* kufuu sureba iranai */
	static float *lastverticalw;    /* kufuu sureba iranai */
	static char **mseq1;
	static char **mseq2;
	static char **mseq;
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


//	fprintf( stderr, "@@@@@@@@@@@@@ penalty_OP = %f, penalty_EX = %f, pelanty = %f\n", fpenalty_OP, fpenalty_EX, fpenalty );

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

			FreeFloatVec( m );
			FreeIntVec( mp );
			FreeFloatVec( largeM );
			FreeIntVec( Mp );

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

		m = AllocateFloatVec( ll2+2 );
		mp = AllocateIntVec( ll2+2 );
		largeM = AllocateFloatVec( ll2+2 );
		Mp = AllocateIntVec( ll2+2 );

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
			FreeIntMtx( commonJP );
		}

		ll1 = MAX( orlgth1, commonAlloc1 );
		ll2 = MAX( orlgth2, commonAlloc2 );

#if DEBUG
		fprintf( stderr, "\n\ntrying to allocate %dx%d matrices ... ", ll1+1, ll2+1 );
#endif

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
	maxwm = -999999999.9;
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
				*ijpjpt = j - 1; //IRU!
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
//			g = Mi;
			if( Mi > tbk )
			{
				tbk = Mi; //error desu.
				tbki = i-1;
				tbkj = Mpi;
			}
//			g = *Mjpt;
			if( *Mjpt > tbk )
			{
				tbk = *Mjpt;
				tbki = *Mpjpt;
				tbkj = j-1;
			}
//			tbk += fpenalty_EX;// + foffset;

//			g = *prept;
			if( *prept > *Mjpt )
			{
				*Mjpt = *prept;
				*Mpjpt = i-1;
			}
//			*Mjpt += fpenalty_EX;// + foffset;

//			g = *prept;
			if( *prept > Mi )
			{
				Mi = *prept;
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


#if DEBUG2
	fprintf( stderr, "maxwm = %f\n", maxwm );
	fprintf( stderr, "endali = %d\n", endali );
	fprintf( stderr, "endalj = %d\n", endalj );
#endif

	if( ijpi[endali][endalj] == localstop ) // && ijpj[endali][endalj] == localstop )
	{
		strcpy( seq1[0], "" );
		strcpy( seq2[0], "" );
		*off1pt = *off2pt = 0;
		return( 0.0 );
	}


	gentracking( currentw, lastverticalw, seq1, seq2, mseq1, mseq2, cpmx1, cpmx2, ijpi, ijpj, off1pt, off2pt, endali, endalj );

//	fprintf( stderr, "### impmatch = %f\n", *impmatch );

	resultlen = strlen( mseq1[0] );
	if( alloclen < resultlen || resultlen > N )
	{
		fprintf( stderr, "alloclen=%d, resultlen=%d, N=%d\n", alloclen, resultlen, N );
		ErrorExit( "LENGTH OVER!\n" );
	}


	strcpy( seq1[0], mseq1[0] );
	strcpy( seq2[0], mseq2[0] );

#if 0
	fprintf( stderr, "\n" );
	fprintf( stderr, ">\n%s\n", mseq1[0] );
	fprintf( stderr, ">\n%s\n", mseq2[0] );
#endif


	return( maxwm );
}

