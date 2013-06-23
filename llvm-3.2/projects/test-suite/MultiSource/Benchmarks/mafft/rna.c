#include "mltaln.h"
#include "dp.h"

#define MEMSAVE 1

#define DEBUG 1
#define USE_PENALTY_EX  1
#define STOREWM 1



static float singleribosumscore( int n1, int n2, char **s1, char **s2, double *eff1, double *eff2, int p1, int p2 )
{
	float val;
	int i, j;
	int code1, code2;

	val = 0.0;
	for( i=0; i<n1; i++ ) for( j=0; j<n2; j++ )
	{
		code1 = amino_n[(int)s1[i][p1]];
		if( code1 > 3 ) code1 = 36;
		code2 = amino_n[(int)s2[j][p2]];
		if( code2 > 3 ) code2 = 36;

//		fprintf( stderr, "'l'%c-%c: %f\n", s1[i][p1], s2[j][p2], (float)ribosumdis[code1][code2] );

		val += (float)ribosumdis[code1][code2] * eff1[i] * eff2[j];
	}
	return( val );
}
static float pairedribosumscore53( int n1, int n2, char **s1, char **s2, double *eff1, double *eff2, int p1, int p2, int c1, int c2 )
{
	float val;
	int i, j;
	int code1o, code1u, code2o, code2u, code1, code2;

	val = 0.0;
	for( i=0; i<n1; i++ ) for( j=0; j<n2; j++ )
	{
		code1o = amino_n[(int)s1[i][p1]];
		code1u = amino_n[(int)s1[i][c1]];
		if( code1o > 3 ) code1 = code1o = 36;
		else if( code1u > 3 ) code1 = 36;
		else code1 = 4 + code1o * 4 + code1u;

		code2o = amino_n[(int)s2[j][p2]];
		code2u = amino_n[(int)s2[j][c2]];
		if( code2o > 3 ) code2 = code1o = 36;
		else if( code2u > 3 ) code2 = 36;
		else code2 = 4 + code2o * 4 + code2u;


//		fprintf( stderr, "%c%c-%c%c: %f\n", s1[i][p1], s1[i][c1], s2[j][p2], s2[j][c2], (float)ribosumdis[code1][code2] );

		if( code1 == 36 || code2 == 36 )
			val += (float)n_dis[code1o][code2o] * eff1[i] * eff2[j];
		else
			val += (float)ribosumdis[code1][code2] * eff1[i] * eff2[j];
	}
	return( val );
}

static float pairedribosumscore35( int n1, int n2, char **s1, char **s2, double *eff1, double *eff2, int p1, int p2, int c1, int c2 )
{
	float val;
	int i, j;
	int code1o, code1u, code2o, code2u, code1, code2;

	val = 0.0;
	for( i=0; i<n1; i++ ) for( j=0; j<n2; j++ )
	{
		code1o = amino_n[(int)s1[i][p1]];
		code1u = amino_n[(int)s1[i][c1]];
		if( code1o > 3 ) code1 = code1o = 36;
		else if( code1u > 3 ) code1 = 36;
		else code1 = 4 + code1u * 4 + code1o;

		code2o = amino_n[(int)s2[j][p2]];
		code2u = amino_n[(int)s2[j][c2]];
		if( code2o > 3 ) code2 = code1o = 36;
		else if( code2u > 3 ) code2 = 36;
		else code2 = 4 + code2u * 4 + code2o;


//		fprintf( stderr, "%c%c-%c%c: %f\n", s1[i][p1], s1[i][c1], s2[j][p2], s2[j][c2], (float)ribosumdis[code1][code2] );

		if( code1 == 36 || code2 == 36 )
			val += (float)n_dis[code1o][code2o] * eff1[i] * eff2[j];
		else
			val += (float)ribosumdis[code1][code2] * eff1[i] * eff2[j];
	}
	return( val );
}


static void mccaskillextract( char **seq, char **nogap, int nseq, RNApair **pairprob, RNApair ***single, int **sgapmap, double *eff )
{
	int lgth;
	int nogaplgth;
	int i, j;
	int left, right, adpos;
	float prob;
	static int *pairnum;
	RNApair *pt, *pt2;

	lgth = strlen( seq[0] );
	pairnum = calloc( lgth, sizeof( int ) );
	for( i=0; i<lgth; i++ ) pairnum[i] = 0;

	for( i=0; i<nseq; i++ )
	{
		nogaplgth = strlen( nogap[i] );
		for( j=0; j<nogaplgth; j++ ) for( pt=single[i][j]; pt->bestpos!=-1; pt++ )
		{
			left = sgapmap[i][j];
			right = sgapmap[i][pt->bestpos];
			prob = pt->bestscore;


			for( pt2=pairprob[left]; pt2->bestpos!=-1; pt2++ )
				if( pt2->bestpos == right ) break;

//			fprintf( stderr, "i,j=%d,%d, left=%d, right=%d, pt=%d, pt2->bestpos = %d\n", i, j, left, right, pt-single[i][j], pt2->bestpos );
			if( pt2->bestpos == -1 )
			{
				pairprob[left] = (RNApair *)realloc( pairprob[left], (pairnum[left]+2) * sizeof( RNApair ) );
				adpos = pairnum[left];
				pairnum[left]++;
				pairprob[left][adpos].bestscore = 0.0;
				pairprob[left][adpos].bestpos = right;
				pairprob[left][adpos+1].bestscore = -1.0;
				pairprob[left][adpos+1].bestpos = -1;
				pt2 = pairprob[left]+adpos;
			}
			else
				adpos = pt2-pairprob[left];

			pt2->bestscore += prob * eff[i];

			if( pt2->bestpos != right )
			{
				fprintf( stderr, "okashii!\n" );
				exit( 1 );
			}
//			fprintf( stderr, "adding %d-%d, %f\n", left, right, prob );
//			fprintf( stderr, "pairprob[0][0].bestpos=%d\n", pairprob[0][0].bestpos );
//			fprintf( stderr, "pairprob[0][0].bestscore=%f\n", pairprob[0][0].bestscore );
		}
	}

//	fprintf( stderr, "before taikakuka\n" );
	for( i=0; i<lgth; i++ ) for( j=0; j<pairnum[i]; j++ )
	{
		if( pairprob[i][j].bestpos > -1 )
		{
//			pairprob[i][j].bestscore /= (float)nseq;
//			fprintf( stderr, "pair of %d = %d (%f) %c:%c\n", i, pairprob[i][j].bestpos, pairprob[i][j].bestscore, seq[0][i], seq[0][pairprob[i][j].bestpos] );
		}
	}

#if 0
	for( i=0; i<lgth; i++ ) for( j=0; j<pairnum[i]; j++ )
	{
		right=pairprob[i][j].bestpos;
		if( right < i ) continue;
		fprintf( stderr, "no%d-%d, adding %d -> %d\n", i, j, right, i );
		pairprob[right] = (RNApair *)realloc( pairprob[right], (pairnum[right]+2) * sizeof( RNApair ) );
		pairprob[right][pairnum[right]].bestscore = pairprob[i][j].bestscore;
		pairprob[right][pairnum[right]].bestpos = i;
		pairnum[right]++;
		pairprob[right][pairnum[right]].bestscore = -1.0;
		pairprob[right][pairnum[right]].bestpos = -1;
	}
#endif

	free( pairnum );

}


void rnaalifoldcall( char **seq, int nseq, RNApair **pairprob )
{
	int lgth;
	int i;
	static int *order = NULL;
	static char **name = NULL;
	char gett[1000];
	FILE *fp;
	int left, right, dumm;
	float prob;
	static int pid;
	static char fnamein[100];
	static char cmd[1000];
	static int *pairnum;

	lgth = strlen( seq[0] );
	if( order == NULL )
	{
		pid = (int)getpid();
		sprintf( fnamein, "/tmp/_rnaalifoldin.%d", pid );
		order = AllocateIntVec( njob );
		name = AllocateCharMtx( njob, 10 );
		for( i=0; i<njob; i++ )
		{
			order[i] = i;
			sprintf( name[i], "seq%d", i );
		}
	}
	pairnum = calloc( lgth, sizeof( int ) );
	for( i=0; i<lgth; i++ ) pairnum[i] = 0;

	fp = fopen( fnamein, "w" );
	if( !fp )
	{
		fprintf( stderr, "Cannot open /tmp/_rnaalifoldin\n" );
		exit( 1 );
	}
	clustalout_pointer( fp, nseq, lgth, seq, name, NULL, NULL, order );
	fclose( fp );

	sprintf( cmd, "RNAalifold -p %s", fnamein );
	system( cmd );

	fp = fopen( "alifold.out", "r" );
	if( !fp )
	{
		fprintf( stderr, "Cannot open /tmp/_rnaalifoldin\n" );
		exit( 1 );
	}

#if 0
	for( i=0; i<lgth; i++ ) // atode kesu
	{
		pairprob[i] = (RNApair *)realloc( pairprob[i], (2) * sizeof( RNApair ) ); // atode kesu
		pairprob[i][1].bestscore = -1.0;
		pairprob[i][1].bestpos = -1;
	}
#endif

	while( 1 )
	{
		fgets( gett, 999, fp );
		if( gett[0] == '(' ) break;
		if( gett[0] == '{' ) break;
		if( gett[0] == '.' ) break;
		if( gett[0] == ',' ) break;
		if( gett[0] != ' ' ) continue;

		sscanf( gett, "%d %d %d %f", &left, &right, &dumm, &prob );
		left--;
		right--;


#if 0
		if( prob > 50.0 && prob > pairprob[left][0].bestscore )
		{
			pairprob[left][0].bestscore = prob;
			pairprob[left][0].bestpos = right;
#else
		if( prob > 0.0 )
		{
			pairprob[left] = (RNApair *)realloc( pairprob[left], (pairnum[left]+2) * sizeof( RNApair ) );
			pairprob[left][pairnum[left]].bestscore = prob / 100.0;
			pairprob[left][pairnum[left]].bestpos = right;
			pairnum[left]++;
			pairprob[left][pairnum[left]].bestscore = -1.0;
			pairprob[left][pairnum[left]].bestpos = -1;
			fprintf( stderr, "%d-%d, %f\n", left, right, prob );

			pairprob[right] = (RNApair *)realloc( pairprob[right], (pairnum[right]+2) * sizeof( RNApair ) );
			pairprob[right][pairnum[right]].bestscore = prob / 100.0;
			pairprob[right][pairnum[right]].bestpos = left;
			pairnum[right]++;
			pairprob[right][pairnum[right]].bestscore = -1.0;
			pairprob[right][pairnum[right]].bestpos = -1;
			fprintf( stderr, "%d-%d, %f\n", left, right, prob );
#endif
		}
	}
	fclose( fp );
	sprintf( cmd, "rm -f %s", fnamein );
	system( cmd ); 

	for( i=0; i<lgth; i++ )
	{
		if( (right=pairprob[i][0].bestpos) > -1 )
		{
			pairprob[right][0].bestpos = i;
			pairprob[right][0].bestscore = pairprob[i][0].bestscore;
		}
	}

#if 0
	for( i=0; i<lgth; i++ ) // atode kesu
		if( pairprob[i][0].bestscore > -1 ) pairprob[i][0].bestscore = 1.0; // atode kesu
#endif

//	fprintf( stderr, "after taikakuka in rnaalifoldcall\n" );
//	for( i=0; i<lgth; i++ )
//	{
//		fprintf( stderr, "pair of %d = %d (%f) %c:%c\n", i, pairprob[i][0].bestpos, pairprob[i][0].bestscore, seq[0][i], seq[0][pairprob[i][0].bestpos] );
//	}

	free( pairnum );
}


static void utot( int n, int l, char **s )
{
	int i, j;
	for( i=0; i<l; i++ )
	{
		for( j=0; j<n; j++ )
		{
			if     ( s[j][i] == 'a' ) s[j][i] = 'a';
			else if( s[j][i] == 't' ) s[j][i] = 't';
			else if( s[j][i] == 'u' ) s[j][i] = 't';
			else if( s[j][i] == 'g' ) s[j][i] = 'g';
			else if( s[j][i] == 'c' ) s[j][i] = 'c';
			else if( s[j][i] == '-' ) s[j][i] = '-';
			else					  s[j][i] = 'n';
		}
	}
}


void foldrna( int nseq1, int nseq2, char **seq1, char **seq2, double *eff1, double *eff2, RNApair ***grouprna1, RNApair ***grouprna2, float **impmtx, int *gapmap1, int *gapmap2, RNApair *additionalpair )
{
	int i, j;
//	int ui, uj;
//	int uiup, ujup;
	int uido, ujdo;
	static char **useq1, **useq2;
	static char **oseq1, **oseq2, **oseq1r, **oseq2r, *odir1, *odir2;
	static RNApair **pairprob1, **pairprob2;
	static RNApair *pairpt1, *pairpt2;
	int lgth1 = strlen( seq1[0] );
	int lgth2 = strlen( seq2[0] );
	static float **impmtx2;
	static float **map;
//	double lenfac;
	float prob;
	int **sgapmap1, **sgapmap2;
	char *nogapdum;
	float **tbppmtx;


//	fprintf( stderr, "nseq1=%d, lgth1=%d\n", nseq1, lgth1 );
	useq1 = AllocateCharMtx( nseq1, lgth1+10 );
	useq2 = AllocateCharMtx( nseq2, lgth2+10 );
	oseq1 = AllocateCharMtx( nseq1, lgth1+10 );
	oseq2 = AllocateCharMtx( nseq2, lgth2+10 );
	oseq1r = AllocateCharMtx( nseq1, lgth1+10 );
	oseq2r = AllocateCharMtx( nseq2, lgth2+10 );
	odir1 = AllocateCharVec( lgth1+10 );
	odir2 = AllocateCharVec( lgth2+10 );
	sgapmap1 = AllocateIntMtx( nseq1, lgth1+1 );
	sgapmap2 = AllocateIntMtx( nseq2, lgth2+1 );
	nogapdum = AllocateCharVec( MAX( lgth1, lgth2 ) );
	pairprob1 = (RNApair **)calloc( lgth1, sizeof( RNApair *) );
	pairprob2 = (RNApair **)calloc( lgth2, sizeof( RNApair *) );
	map = AllocateFloatMtx( lgth1, lgth2 );
	impmtx2 = AllocateFloatMtx( lgth1, lgth2 );
	tbppmtx = AllocateFloatMtx( lgth1, lgth2 );

	for( i=0; i<nseq1; i++ ) strcpy( useq1[i], seq1[i] );
	for( i=0; i<nseq2; i++ ) strcpy( useq2[i], seq2[i] );
	for( i=0; i<nseq1; i++ ) strcpy( oseq1[i], seq1[i] );
	for( i=0; i<nseq2; i++ ) strcpy( oseq2[i], seq2[i] );

	for( i=0; i<nseq1; i++ ) commongappick_record( 1, useq1+i, sgapmap1[i] );
	for( i=0; i<nseq2; i++ ) commongappick_record( 1, useq2+i, sgapmap2[i] );

	for( i=0; i<lgth1; i++ )
	{
		pairprob1[i] = (RNApair *)calloc( 1, sizeof( RNApair ) );
		pairprob1[i][0].bestpos = -1;
		pairprob1[i][0].bestscore = -1;
	}
	for( i=0; i<lgth2; i++ )
	{
		pairprob2[i] = (RNApair *)calloc( 1, sizeof( RNApair ) );
		pairprob2[i][0].bestpos = -1;
		pairprob2[i][0].bestscore = -1;
	}

	utot( nseq1, lgth1, oseq1 );
	utot( nseq2, lgth2, oseq2 );

//	fprintf( stderr, "folding group1\n" );
//	rnalocal( oseq1, useq1, eff1, eff1, nseq1, nseq1, lgth1+10, pair1 );

/* base-pairing probability of group 1 */
	if( rnaprediction == 'r' )
		rnaalifoldcall( oseq1, nseq1, pairprob1 );
	else
		mccaskillextract( oseq1, useq1, nseq1, pairprob1, grouprna1, sgapmap1, eff1 );


//	fprintf( stderr, "folding group2\n" );
//	rnalocal( oseq2, useq2, eff2, eff2, nseq2, nseq2, lgth2+10, pair2 );

/* base-pairing probability of group 2 */
	if( rnaprediction == 'r' )
		rnaalifoldcall( oseq2, nseq2, pairprob2 );
	else
		mccaskillextract( oseq2, useq2, nseq2, pairprob2, grouprna2, sgapmap2, eff2 );



#if 0
	makerseq( oseq1, oseq1r, odir1, pairprob1, nseq1, lgth1 );
	makerseq( oseq2, oseq2r, odir2, pairprob2, nseq2, lgth2 );

	fprintf( stderr, "%s\n", odir2 );

	for( i=0; i<nseq1; i++ )
	{
		fprintf( stdout, ">ori\n%s\n", oseq1[0] );
		fprintf( stdout, ">rev\n%s\n", oseq1r[0] );
	}
#endif

/* similarity score */
	Lalignmm_hmout( oseq1, oseq2, eff1, eff2, nseq1, nseq2, 10000, NULL, NULL, NULL, NULL, map );

	if( 1 )
	{
		if( RNAscoremtx == 'n' )
		{
			for( i=0; i<lgth1; i++ ) for( j=0; j<lgth2; j++ )
			{
//				impmtx2[i][j] = osoiaveragescore( nseq1, nseq2, oseq1, oseq2, eff1, eff2, i, j ) * consweight_multi;
				impmtx2[i][j] = 0.0;
			}
		}
		else if( RNAscoremtx == 'r' )
		{
			for( i=0; i<lgth1; i++ ) for( j=0; j<lgth2; j++ ) 
			{
				tbppmtx[i][j] = 1.0;
				impmtx2[i][j] = 0.0;
			}
			for( i=0; i<lgth1; i++ ) for( pairpt1=pairprob1[i]; pairpt1->bestpos!=-1; pairpt1++ )
			{
				for( j=0; j<lgth2; j++ ) for( pairpt2=pairprob2[j]; pairpt2->bestpos!=-1; pairpt2++ )
				{
					uido = pairpt1->bestpos;
					ujdo = pairpt2->bestpos;
					prob = pairpt1->bestscore * pairpt2->bestscore;
					if( uido > -1  && ujdo > -1 )
					{
						if( uido > i && j > ujdo )
						{
							impmtx2[i][j] += prob * pairedribosumscore53( nseq1, nseq2, oseq1, oseq2, eff1, eff2, i, j, uido, ujdo ) * consweight_multi;
							tbppmtx[i][j] -= prob;
						}
						else if( i < uido && j < ujdo )
						{
							impmtx2[i][j] += prob * pairedribosumscore35( nseq1, nseq2, oseq1, oseq2, eff1, eff2, i, j, uido, ujdo ) * consweight_multi;
							tbppmtx[i][j] -= prob;
						}
					}
				}
			}
	
	
			for( i=0; i<lgth1; i++ )
			{
				for( j=0; j<lgth2; j++ )
				{
					impmtx2[i][j] += tbppmtx[i][j] * singleribosumscore( nseq1, nseq2, oseq1, oseq2, eff1, eff2, i, j ) * consweight_multi;
				}
			}
		}


/* four-way consistency */

		for( i=0; i<lgth1; i++ ) for( pairpt1=pairprob1[i]; pairpt1->bestpos!=-1; pairpt1++ )
		{

//			if( pairprob1[i] == NULL ) continue;

			for( j=0; j<lgth2; j++ ) for( pairpt2=pairprob2[j]; pairpt2->bestpos!=-1; pairpt2++ )
			{
//				fprintf( stderr, "i=%d, j=%d, pn1=%d, pn2=%d\n", i, j, pairpt1-pairprob1[i], pairpt2-pairprob2[j] ); 
//				if( pairprob2[j] == NULL ) continue;

				uido = pairpt1->bestpos;
				ujdo = pairpt2->bestpos;
				prob = pairpt1->bestscore * pairpt2->bestscore;
//				prob = 1.0;
//				fprintf( stderr, "i=%d->uido=%d, j=%d->ujdo=%d\n", i, uido, j, ujdo );

//				fprintf( stderr, "impmtx2[%d][%d] = %f\n", i, j, impmtx2[i][j] );

//				if( i < uido && j > ujdo ) continue;
//				if( i > uido && j < ujdo ) continue;


//				posdistj = abs( ujdo-j );

//				if( uido > -1 && ujdo > -1 )  
				if( uido > -1 && ujdo > -1 && ( ( i > uido && j > ujdo ) || ( i < uido && j < ujdo ) ) )
				{
					{
						impmtx2[i][j] += MAX( 0, map[uido][ujdo] ) * consweight_rna * 600 * prob; // osoi
					}
				}

			}
		}
		for( i=0; i<lgth1; i++ ) for( j=0; j<lgth2; j++ )
		{
			impmtx[i][j] += impmtx2[i][j];
//			fprintf( stderr, "fastathreshold=%f, consweight_multi=%f, consweight_rna=%f\n", fastathreshold, consweight_multi, consweight_rna );
//			impmtx[i][j] *= 0.5;
		}

//		impmtx[0][0] += 10000.0;
//		impmtx[lgth1-1][lgth2-1] += 10000.0;



#if 0
		fprintf( stdout, "#impmtx2 = \n" );
		for( i=0; i<lgth1; i++ )
		{
			for( j=0; j<lgth2; j++ )
			{
				fprintf( stdout, "%d %d %f\n", i, j, impmtx2[i][j] );
			}
			fprintf( stdout, "\n" );
		}
		exit( 1 );
#endif
	}

	FreeCharMtx( useq1 );
	FreeCharMtx( useq2 );
	FreeCharMtx( oseq1 );
	FreeCharMtx( oseq2 );
	FreeCharMtx( oseq1r );
	FreeCharMtx( oseq2r );
	free( odir1 );
	free( odir2 );
	FreeFloatMtx( impmtx2 );
	FreeFloatMtx( map );
	FreeIntMtx( sgapmap1 );
	FreeIntMtx( sgapmap2 );
	FreeFloatMtx( tbppmtx );

	for( i=0; i<lgth1; i++ ) free( pairprob1[i] );
	for( i=0; i<lgth2; i++ ) free( pairprob2[i] );
	free( pairprob1 );
	free( pairprob2 );
}

