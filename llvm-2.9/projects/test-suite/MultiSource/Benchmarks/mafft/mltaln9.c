#include "mltaln.h"

#define DEBUG 0

int seqlen( char *seq )
{
	int val = 0;
	while( *seq )
		if( *seq++ != '-' ) val++;
	return( val );
}

int intlen( int *num )
{
	int value = 0;
	while( *num++ != -1 ) value++;
	return( value );
}

char seqcheck( char **seq )
{
	int i, len;
	char **seqbk = seq;
	while( *seq )	
	{
		len = strlen( *seq );
		for( i=0; i<len; i++ ) 
		{
			if( amino_n[(int)(*seq)[i]] == -1 ) 
			{

				fprintf( stderr, "========================================================================= \n" );
				fprintf( stderr, "========================================================================= \n" );
				fprintf( stderr, "========================================================================= \n" );
				fprintf( stderr, "=== \n" );
				fprintf( stderr, "=== Alphabet '%c' is unknown.\n", (*seq)[i] );
				fprintf( stderr, "=== Please check site %d in sequence %d.\n", i+1, (int)(seq-seqbk+1) );
				fprintf( stderr, "=== \n" );
				fprintf( stderr, "========================================================================= \n" );
				fprintf( stderr, "========================================================================= \n" );
				fprintf( stderr, "========================================================================= \n" );
				return( (int)(*seq)[i] );
			}
		}
		seq++;
	}
	return( 0 );
}
	
void scmx_calc( int icyc, char **aseq, double *effarr, float **scmx )
{
	int  i, j, lgth;
	 
	lgth = strlen( aseq[0] );
	for( j=0; j<lgth; j++ )
	{
		for( i=0; i<26; i++ )
		{
			scmx[i][j] = 0;
		}
	}
	for( i=0; i<icyc+1; i++ )
	{
		int id;
		id = amino_n[(int)aseq[i][0]];
		scmx[id][0] += (float)effarr[i];
	}
	for( j=1; j<lgth-1; j++ )
	{
		for( i=0; i<icyc+1; i++ )
		{
			int id;
			id = amino_n[(int)aseq[i][j]];
			scmx[id][j] += (float)effarr[i];
		}
	}
	for( i=0; i<icyc+1; i++ )
	{
		int id;
		id = amino_n[(int)aseq[i][lgth-1]];
		scmx[id][lgth-1] += (float)effarr[i];
	}
}

void exitall( char arr[] )
{
	fprintf( stderr, "%s\n", arr );
	exit( 1 );
}

void display( char **seq, int nseq )
{
	int i, imax;
	char b[121];

	if( !disp ) return;
		if( nseq > DISPSEQF ) imax = DISPSEQF;
		else                  imax = nseq;
		fprintf( stderr, "    ....,....+....,....+....,....+....,....+....,....+....,....+....,....+....,....+....,....+....,....+....,....+....,....+\n" );
		for( i=0; i<+imax; i++ )
		{
			strncpy( b, seq[i]+DISPSITEI, 120 );
			b[120] = 0;
			fprintf( stderr, "%3d %s\n", i+1, b );
		}
}
#if 0
double intergroup_score( char **seq1, char **seq2, double *eff1, double *eff2, int clus1, int clus2, int len )
{
	int i, j, k;
	double score;
	double tmpscore;
	char *mseq1, *mseq2;
	double efficient;
	char xxx[100];

//	totaleff1 = 0.0; for( i=0; i<clus1; i++ ) totaleff1 += eff1[i];
//	totaleff2 = 0.0; for( i=0; i<clus2; i++ ) totaleff2 += eff2[i];

	score = 0.0;
	for( i=0; i<clus1; i++ ) for( j=0; j<clus2; j++ ) 
	{
		efficient = eff1[i] * eff2[j];
		mseq1 = seq1[i];
		mseq2 = seq2[j];
		tmpscore = 0.0;
		for( k=0; k<len; k++ ) 
		{
			if( mseq1[k] == '-' && mseq2[k] == '-' ) continue;
			tmpscore += amino_dis[(int)mseq1[k]][(int)mseq2[k]];

			if( mseq1[k] == '-' ) 
			{
				tmpscore += penalty;
				tmpscore += amino_dis[(int)mseq1[k]][(int)mseq2[k]];
				while( mseq1[++k] == '-' )
					tmpscore += amino_dis[(int)mseq1[k]][(int)mseq2[k]];
				k--;
				if( k >len-2 ) break;
				continue;
			}
			if( mseq2[k] == '-' )
			{
				tmpscore += penalty;
				tmpscore += amino_dis[(int)mseq1[k]][(int)mseq2[k]];
				while( mseq2[++k] == '-' )
					tmpscore += amino_dis[(int)mseq1[k]][(int)mseq2[k]];
				k--;
				if( k > len-2 ) break;
				continue;
			}
		}
		score += (double)tmpscore * efficient;
#if 1
		sprintf( xxx, "%f", score );
//		fprintf( stderr, "## score in intergroup_score = %f\n", score );
#endif
	}
#if 0
		fprintf( stderr, "###score = %f\n", score );
#endif
#if 0
	fprintf( stderr, "## score in intergroup_score = %f\n", score );
#endif
	return( score );
}
#endif

void intergroup_score_consweight( char **seq1, char **seq2, double *eff1, double *eff2, int clus1, int clus2, int len, double *value )
{
	int i, j, k;
	int len2 = len - 2;
	int ms1, ms2;
	double tmpscore;
	char *mseq1, *mseq2;
	double efficient;

//	totaleff1 = 0.0; for( i=0; i<clus1; i++ ) totaleff1 += eff1[i];
//	totaleff2 = 0.0; for( i=0; i<clus2; i++ ) totaleff2 += eff2[i];



	*value = 0.0;
	for( i=0; i<clus1; i++ ) 
	{
		for( j=0; j<clus2; j++ ) 
		{
			efficient = eff1[i] * eff2[j]; /* なぜか配列を使わないとおかしくなる, 多分バグ */
			mseq1 = seq1[i];
			mseq2 = seq2[j];
			tmpscore = 0.0;
			for( k=0; k<len; k++ ) 
			{
				ms1 = (int)mseq1[k];
				ms2 = (int)mseq2[k];
				if( ms1 == (int)'-' && ms2 == (int)'-' ) continue;
				tmpscore += (double)amino_dis_consweight_multi[ms1][ms2];
	
				if( ms1 == (int)'-' ) 
				{
					tmpscore += (double)penalty;
					tmpscore += (double)amino_dis_consweight_multi[ms1][ms2];
					while( (ms1=(int)mseq1[++k]) == (int)'-' )
						;
//						tmpscore += (double)amino_dis_consweight_multi[ms1][ms2];
					k--;
					if( k >len2 ) break;
					continue;
				}
				if( ms2 == (int)'-' )
				{
					tmpscore += (double)penalty;
					tmpscore += (double)amino_dis_consweight_multi[ms1][ms2];
					while( (ms2=(int)mseq2[++k]) == (int)'-' )
						;
//						tmpscore += (double)amino_dis_consweight_multi[ms1][ms2];
					k--;
					if( k > len2 ) break;
					continue;
				}
			}
			*value += (double)tmpscore * (double)efficient;
//			fprintf( stderr, "val in _gapnomi = %f\n", *value );
		}
	}
#if 0
	fprintf( stdout, "###score = %f\n", score );
#endif
#if DEBUG
	fprintf( stderr, "score in intergroup_score = %f\n", score );
#endif
//	return( score );
}
void intergroup_score_gapnomi( char **seq1, char **seq2, double *eff1, double *eff2, int clus1, int clus2, int len, double *value )
{
	int i, j, k;
	int len2 = len - 2;
	int ms1, ms2;
	double tmpscore;
	char *mseq1, *mseq2;
	double efficient;

//	totaleff1 = 0.0; for( i=0; i<clus1; i++ ) totaleff1 += eff1[i];
//	totaleff2 = 0.0; for( i=0; i<clus2; i++ ) totaleff2 += eff2[i];



	*value = 0.0;
	for( i=0; i<clus1; i++ ) 
	{
		for( j=0; j<clus2; j++ ) 
		{
			efficient = eff1[i] * eff2[j]; /* なぜか配列を使わないとおかしくなる, 多分バグ */
			mseq1 = seq1[i];
			mseq2 = seq2[j];
			tmpscore = 0.0;
			for( k=0; k<len; k++ ) 
			{
				ms1 = (int)mseq1[k];
				ms2 = (int)mseq2[k];
				if( ms1 == (int)'-' && ms2 == (int)'-' ) continue;
//				tmpscore += (double)amino_dis[ms1][ms2];
	
				if( ms1 == (int)'-' ) 
				{
					tmpscore += (double)penalty;
//					tmpscore += (double)amino_dis[ms1][ms2];
					while( (ms1=(int)mseq1[++k]) == (int)'-' )
						;
//						tmpscore += (double)amino_dis[ms1][ms2];
					k--;
					if( k >len2 ) break;
					continue;
				}
				if( ms2 == (int)'-' )
				{
					tmpscore += (double)penalty;
//					tmpscore += (double)amino_dis[ms1][ms2];
					while( (ms2=(int)mseq2[++k]) == (int)'-' )
						;
//						tmpscore += (double)amino_dis[ms1][ms2];
					k--;
					if( k > len2 ) break;
					continue;
				}
			}
			*value += (double)tmpscore * (double)efficient;
//			fprintf( stderr, "val in _gapnomi = %f\n", *value );
		}
	}
#if 0
	fprintf( stdout, "###score = %f\n", score );
#endif
#if DEBUG
	fprintf( stderr, "score in intergroup_score = %f\n", score );
#endif
//	return( score );
}

void intergroup_score( char **seq1, char **seq2, double *eff1, double *eff2, int clus1, int clus2, int len, double *value )
{
	int i, j, k;
	int len2 = len - 2;
	int ms1, ms2;
	double tmpscore;
	char *mseq1, *mseq2;
	double efficient;

	double gaptmpscore;
	double gapscore = 0.0;

//	fprintf( stderr, "#### in intergroup_score\n" );

//	totaleff1 = 0.0; for( i=0; i<clus1; i++ ) totaleff1 += eff1[i];
//	totaleff2 = 0.0; for( i=0; i<clus2; i++ ) totaleff2 += eff2[i];

	*value = 0.0;
	for( i=0; i<clus1; i++ ) 
	{
		for( j=0; j<clus2; j++ ) 
		{
			efficient = eff1[i] * eff2[j]; /* なぜか配列を使わないとおかしくなる, 多分バグ */
			mseq1 = seq1[i];
			mseq2 = seq2[j];
			tmpscore = 0.0;
			gaptmpscore = 0.0;
			for( k=0; k<len; k++ ) 
			{
				ms1 = (int)mseq1[k];
				ms2 = (int)mseq2[k];
				if( ms1 == (int)'-' && ms2 == (int)'-' ) continue;
//				tmpscore += (double)amino_dis[ms1][ms2];
				tmpscore += (double)amino_dis_consweight_multi[ms1][ms2];
	
				if( ms1 == (int)'-' ) 
				{
					tmpscore += (double)penalty;
					gaptmpscore += (double)penalty;
//					tmpscore += (double)amino_dis[ms1][ms2];
					tmpscore += (double)amino_dis_consweight_multi[ms1][ms2];
					while( (ms1=(int)mseq1[++k]) == (int)'-' )
//						tmpscore += (double)amino_dis[ms1][ms2];
						tmpscore += (double)amino_dis_consweight_multi[ms1][ms2];
					k--;
					if( k >len2 ) break;
					continue;
				}
				if( ms2 == (int)'-' )
				{
					tmpscore += (double)penalty;
					gaptmpscore += (double)penalty;
//					tmpscore += (double)amino_dis[ms1][ms2];
					tmpscore += (double)amino_dis_consweight_multi[ms1][ms2];
					while( (ms2=(int)mseq2[++k]) == (int)'-' )
//						tmpscore += (double)amino_dis[ms1][ms2];
						tmpscore += (double)amino_dis_consweight_multi[ms1][ms2];
					k--;
					if( k > len2 ) break;
					continue;
				}
			}
			*value += (double)tmpscore * (double)efficient;
			gapscore += (double)gaptmpscore * (double)efficient;
		}
	}
#if 0
	fprintf( stderr, "###gapscore = %f\n", gapscore );
#endif
#if DEBUG
	fprintf( stderr, "score in intergroup_score = %f\n", score );
#endif
//	return( score );
}
void intergroup_score_new( char **seq1, char **seq2, double *eff1, double *eff2, int clus1, int clus2, int len, double *value )
{
	int i, j, k;
	int len2 = len - 2;
	int ms1, ms2;
	double tmpscore;
	char *mseq1, *mseq2;
	static double efficient[1];

//	totaleff1 = 0.0; for( i=0; i<clus1; i++ ) totaleff1 += eff1[i];
//	totaleff2 = 0.0; for( i=0; i<clus2; i++ ) totaleff2 += eff2[i];

	*value = 0.0;
	for( i=0; i<clus1; i++ ) 
	{
		for( j=0; j<clus2; j++ ) 
		{
			*efficient = eff1[i] * eff2[j]; /* なぜか配列を使わないとおかしくなる, 多分バグ */
			mseq1 = seq1[i];
			mseq2 = seq2[j];
			tmpscore = 0.0;
			for( k=0; k<len; k++ ) 
			{
				ms1 = (int)mseq1[k];
				ms2 = (int)mseq2[k];
				if( ms1 == (int)'-' && ms2 == (int)'-' ) continue;
				tmpscore += (double)amino_dis[ms1][ms2];
	
				if( ms1 == (int)'-' ) 
				{
					tmpscore += (double)penalty;
					tmpscore += (double)amino_dis[ms1][ms2];
					while( (ms1=(int)mseq1[++k]) == (int)'-' )
						tmpscore += (double)amino_dis[ms1][ms2];
					k--;
					if( k >len2 ) break;
					continue;
				}
				if( ms2 == (int)'-' )
				{
					tmpscore += (double)penalty;
					tmpscore += (double)amino_dis[ms1][ms2];
					while( (ms2=(int)mseq2[++k]) == (int)'-' )
						tmpscore += (double)amino_dis[ms1][ms2];
					k--;
					if( k > len2 ) break;
					continue;
				}
			}
			*value += (double)tmpscore * (double)*efficient;
		}
	}
#if 0
	fprintf( stdout, "###score = %f\n", score );
#endif
#if DEBUG
	fprintf( stderr, "score in intergroup_score = %f\n", score );
#endif
//	return( score );
}


double score_calc3( char **seq, int s, double **eff, int ex )  /* method 3 */
{
    int i, j, k, c;
    int len = strlen( seq[0] );
    double score;
    long tmpscore;
    static char mseq1[N*2], mseq2[N*2];
	double totaleff;

	switch( weight )
	{
		case 0:
			totaleff = ( (double)s * ((double)s-1.0) ) / 2.0;
			break;
		case 2:
			totaleff = s / 2; 
			break;
		case 3:
			totaleff = 0.0; for( i=0; i<s-1; i++ ) for( j=i+1; j<s; j++ ) totaleff += eff[i][j];
			break;
		default:
			fprintf( stderr, "error\n" );
			exit( 1 );
	}

    score = 0.0;
    for( i=0; i<s-1; i++ )
    {
        for( j=i+1; j<s; j++ )
        {
            strcpy( mseq1, seq[i] );
            strcpy( mseq2, seq[j] );
            tmpscore = 0;
            c = 0;
            for( k=0; k<len; k++ )
            {
                if( mseq1[k] == '-' && mseq2[k] == '-' ) continue;
                tmpscore += amino_dis[(int)mseq1[k]][(int)mseq2[k]] + 400 * !scoremtx;
                c++;
                if( mseq1[k] == '-' )
                {
                    tmpscore += penalty - n_dis[0][24];
                    while( mseq1[++k] == '-' )
                        ;
                    k--;
                    if( k > len-2 ) break;
                    continue;
                }
                if( mseq2[k] == '-' )
                {
                    tmpscore += penalty - n_dis[0][24];
                    while( mseq2[++k] == '-' )
                        ;
                    k--;
                    if( k > len-2 ) break;
                    continue;
                }
            }
            /*
            if( mseq1[0] == '-' || mseq2[0] == '-' )
            {
                for( k=0; k<len; k++ )
                {
                    if( mseq1[k] == '-' && mseq2[k] == '-' ) continue;
                    if( !( mseq1[k] != '-' && mseq2[k] != '-' ) )
                    {
                        c--;
                        tmpscore -= SGAPP;
                        break;
                    }
                    else break;
                }
            }
            if( mseq1[len-1] == '-' || mseq2[len-1] == '-' )
            {
                for( k=len-1; k>=0; k-- )
                {
                    if( mseq1[k] == '-' && mseq2[k] == '-' ) continue;
                    if( !( mseq1[k] != '-' && mseq2[k] != '-' ) )
                    {
                        c--;
                        tmpscore -= SGAPP;
                        break;
                    }
                    else break;
                }
            }
            */
            /*
            if( x == 65 ) printf( "i=%d j=%d tmpscore=%d l=%d\n", i, j, tmpscore, len )
;
            */
            score += (double)tmpscore / (double)c * eff[i][j];
        }
    }
	if( weight == 0 )
    	score /= totaleff; 
    return( (double)score );
}
double score_calc5( char **seq, int s, double **eff, int ex )  /* method 3 deha nai */
{
    int i, j, k;
    double c;
    int len = strlen( seq[0] );
    double score;
    double tmpscore;
    char *mseq1, *mseq2;
    double efficient;
#if DEBUG
	FILE *fp;
#endif

    score = 0.0;
    c = 0.0;

	for( i=0; i<s; i++ ) 
	{
		
			if( i == ex ) continue;
            efficient = eff[i][ex];
            mseq1 = seq[i];
            mseq2 = seq[ex];
            tmpscore = 0.0;
            for( k=0; k<len; k++ )
            {
                if( mseq1[k] == '-' && mseq2[k] == '-' ) continue;
                tmpscore += amino_dis[(int)mseq1[k]][(int)mseq2[k]];

                if( mseq1[k] == '-' )
                {
                    tmpscore += penalty;
                    while( mseq1[++k] == '-' )
                        tmpscore += amino_dis[(int)mseq1[k]][(int)mseq2[k]];
                    k--;
                    if( k > len-2 ) break;
                    continue;
                }
                if( mseq2[k] == '-' )
                {
                    tmpscore += penalty;
                    while( mseq2[++k] == '-' )
                        tmpscore += amino_dis[(int)mseq1[k]][(int)mseq2[k]];
                    k--;
                    if( k > len-2 ) break;
                    continue;
                }
            }
            score += (double)tmpscore * efficient;
/*
			fprintf( stdout, "%d-%d tmpscore = %f, eff = %f, tmpscore*eff = %f\n", i, ex, tmpscore, efficient, tmpscore*efficient );
*/
	}
	/*
	fprintf( stdout, "total score = %f\n", score );
	*/

    for( i=0; i<s-1; i++ )
    {
        for( j=i+1; j<s; j++ )
        {
			if( i == ex || j == ex ) continue;

            efficient = eff[i][j];
            mseq1 = seq[i];
            mseq2 = seq[j];
            tmpscore = 0.0;
            for( k=0; k<len; k++ )
            {
                if( mseq1[k] == '-' && mseq2[k] == '-' ) continue;
                tmpscore += amino_dis[(int)mseq1[k]][(int)mseq2[k]];

                if( mseq1[k] == '-' )
                {
                    tmpscore += penalty;
                    while( mseq1[++k] == '-' )
                        tmpscore += amino_dis[(int)mseq1[k]][(int)mseq2[k]];
                    k--;
                    if( k > len-2 ) break;
                    continue;
                }
                if( mseq2[k] == '-' )
                {
                    tmpscore += penalty;
                    while( mseq2[++k] == '-' )
                        tmpscore += amino_dis[(int)mseq1[k]][(int)mseq2[k]];
                    k--;
                    if( k > len-2 ) break;
                    continue;
                }
            }
            score += (double)tmpscore * efficient;
        }
    }
/*
	fprintf( stderr, "score in score_calc5 = %f\n", score );
*/
    return( (double)score );
/*

fprintf( trap_g, "score by fast = %f\n", (float)score );

tmpscore = score = 0.0;
	for( i=0; i<s; i++ ) 
	{
		if( i == ex ) continue;
		tmpscore = Cscore_m_1( seq, i, eff );
		fprintf( stdout, "%d %f\n", i, tmpscore );

		score += tmpscore;
	}
	tmpscore = Cscore_m_1( seq, ex, eff );
	fprintf( stdout, "ex%d %f\n", i, tmpscore );
	score += tmpscore;

	return( score );
*/
}


	
double score_calc4( char **seq, int s, double **eff, int ex )  /* method 3 deha nai */
{
    int i, j, k;
	double c;
    int len = strlen( seq[0] );
    double score;
    long tmpscore;
	char *mseq1, *mseq2;
	double efficient;

    score = 0.0;
	c = 0.0;
/*
	printf( "in score_calc4\n" );
	for( i=0; i<s; i++ )
	{
		for( j=0; j<s; j++ ) 
		{
			printf( "% 5.3f", eff[i][j] ); 
		}
		printf( "\n" );
		
	}
*/
    for( i=0; i<s-1; i++ )
    {
        for( j=i+1; j<s; j++ )
        {
			efficient = eff[i][j];
			if( mix == 1 ) efficient = 1.0;
			/*
			printf( "weight for %d v.s. %d = %f\n", i, j, efficient );
			*/
            mseq1 = seq[i];
            mseq2 = seq[j];
            tmpscore = 0;
            for( k=0; k<len; k++ )
            {
                if( mseq1[k] == '-' && mseq2[k] == '-' ) continue;
                tmpscore += amino_dis[(int)mseq1[k]][(int)mseq2[k]] + 400 * !scoremtx ;

				c += efficient;

                if( mseq1[k] == '-' )
                {
                    tmpscore += penalty - n_dis[24][0];
                    while( mseq1[++k] == '-' )
						;
                    k--;
                    if( k > len-2 ) break;
                    continue;
                }
                if( mseq2[k] == '-' )
                {
                    tmpscore += penalty - n_dis[24][0];
                    while( mseq2[++k] == '-' )
						;
                    k--;
                    if( k > len-2 ) break;
                    continue;
                }
            }
			/*
			if( x == 65 ) printf( "i=%d j=%d tmpscore=%d l=%d\n", i, j, tmpscore, len );
			*/
            score += (double)tmpscore * efficient;
        }
    }
    score /= c;
    return( (double)score );
}



void upg2( int nseq, double **eff, int ***topol, double **len )
{
    int i, j, k;
	double tmplen[M];

    static char **pair = NULL;

	if( !pair )
	{
		pair = AllocateCharMtx( njob, njob );
	}

	for( i=0; i<nseq; i++ ) tmplen[i] = 0.0;
    for( i=0; i<nseq; i++ ) for( j=0; j<nseq; j++ ) pair[i][j] = 0;
    for( i=0; i<nseq; i++ ) pair[i][i] = 1;

    for( k=0; k<nseq-1; k++ )
    {
        float minscore = 9999.0;
        int im = -1, jm = -1;
        int count;

        for( i=0; i<nseq-1; i++ ) for( j=i+1; j<nseq; j++ )
        {
            if( eff[i][j] < minscore )
            {
                minscore = eff[i][j];
                im = i; jm = j;
            }
        }
        for( i=0, count=0; i<nseq; i++ )
            if( pair[im][i] > 0 )
            {
                topol[k][0][count] = i;
                count++;
            }
        topol[k][0][count] = -1;
        for( i=0, count=0; i<nseq; i++ )
            if( pair[jm][i] > 0 )
            {
                topol[k][1][count] = i;
                count++;
            }
        topol[k][1][count] = -1;

		len[k][0] = minscore / 2.0 - tmplen[im];
		len[k][1] = minscore / 2.0 - tmplen[jm];

		tmplen[im] = minscore / 2.0;

        for( i=0; i<nseq; i++ ) pair[im][i] += ( pair[jm][i] > 0 );
        for( i=0; i<nseq; i++ ) pair[jm][i] = 0;

        for( i=0; i<nseq; i++ )
        {
            if( i != im && i != jm )
            {
                eff[MIN(i,im)][MAX(i,im)] =
                ( eff[MIN(i,im)][MAX(i,im)] + eff[MIN(i,jm)][MAX(i,jm)] ) / 2.0;
                eff[MIN(i,jm)][MAX(i,jm)] = 9999.0;
            }
            eff[im][jm] = 9999.0;
        }
#if DEBUG
        printf( "STEP-%03d:\n", k+1 );
		printf( "len0 = %f\n", len[k][0] );
        for( i=0; topol[k][0][i]>-1; i++ ) printf( " %03d", topol[k][0][i] );
        printf( "\n" );
		printf( "len1 = %f\n", len[k][1] );
        for( i=0; topol[k][1][i]>-1; i++ ) printf( " %03d", topol[k][1][i] );
        printf( "\n" );
#endif
    }
}

static void setnearest( int nseq, Bchain *acpt, float **eff, float *mindisfrompt, int *nearestpt, int pos )
{
	int j;
	float tmpfloat;
	float **effptpt;
	Bchain *acptj;

	*mindisfrompt = 999.9;
	*nearestpt = -1;

//	if( (acpt+pos)->next ) effpt = eff[pos]+(acpt+pos)->next->pos-pos;

//	for( j=pos+1; j<nseq; j++ )
	for( acptj=(acpt+pos)->next; acptj!=NULL; acptj=acptj->next )
	{
		j = acptj->pos;
//		if( (tmpfloat=*effpt++) < *mindisfrompt )
		if( (tmpfloat=eff[pos][j-pos]) < *mindisfrompt )
		{
			*mindisfrompt = tmpfloat;
			*nearestpt = j;
		}
	}
	effptpt = eff;
//	for( j=0; j<pos; j++ )
	for( acptj=acpt; (acptj&&acptj->pos!=pos); acptj=acptj->next )
	{
		j = acptj->pos;
//		if( (tmpfloat=(*effptpt++)[pos-j]) < *mindisfrompt )
		if( (tmpfloat=eff[j][pos-j]) < *mindisfrompt )
		{
			*mindisfrompt = tmpfloat;
			*nearestpt = j;
		}
	}
}



static void loadtreeoneline( int *ar, float *len, FILE *fp )
{
	static char gett[1000];

	fgets( gett, 999, fp );

//	fprintf( stderr, "gett=%s\n", gett );


	sscanf( gett, "%d %d %a %a", ar, ar+1, len, len+1 );

	ar[0]--;
	ar[1]--;

	if( ar[0] >= ar[1] )
	{
		fprintf( stderr, "Incorrect guide tree\n" );
		exit( 1 );
	}


//	fprintf( stderr, "ar[0] = %d, ar[1] = %d\n", ar[0], ar[1] );
//	fprintf( stderr, "len[0] = %f, len[1] = %f\n", len[0], len[1] );
}

void loadtree( int nseq, int ***topol, float **len, char **name, int *nlen )
{
	int i, j, k, miniim, maxiim, minijm, maxijm;
	int *intpt, *intpt2;
	static int *hist = NULL;
	static Bchain *ac = NULL;
	int im = -1, jm = -1;
	Bchain *acjmnext, *acjmprev;
	int prevnode;
	Bchain *acpti;
	int *pt1, *pt2, *pt11, *pt22;
	static int *nmemar;
	int nmemim, nmemjm;
	float minscore;
	int *nearest = NULL; // by D.Mathog, a guess
	float *mindisfrom = NULL; // by D.Mathog, a guess
	static char **tree;
	static char *treetmp;
	static char *nametmp;
	FILE *fp;
	int node[2];

	fp = fopen( "_guidetree", "r" );
	if( !fp )
	{
		fprintf( stderr, "cannot open _guidetree\n" );
		exit( 1 );
	}

	if( !hist )
	{
		hist = AllocateIntVec( njob );
		ac = (Bchain *)malloc( njob * sizeof( Bchain ) );
		nmemar = AllocateIntVec( njob );
		mindisfrom = AllocateFloatVec( njob );
		nearest = AllocateIntVec( njob );
		treetmp = AllocateCharVec( njob*50 );
		nametmp = AllocateCharVec( 30 );
		tree = AllocateCharMtx( njob, njob*50 );
	}

	
    for( i=0; i<nseq; i++ )
	{
		for( j=0; j<30; j++ ) nametmp[j] = 0;
		for( j=0; j<30; j++ ) 
		{
			if( isalnum( name[i][j] ) )
				nametmp[j] = name[i][j];
			else
				nametmp[j] = '_';
		}
		nametmp[30] = 0;
//		sprintf( tree[i], "%d_l=%d_%.20s", i+1, nlen[i], nametmp+1 );
		sprintf( tree[i], "%d_%.20s", i+1, nametmp+1 );
	}
	for( i=0; i<nseq; i++ )
	{
		ac[i].next = ac+i+1;
		ac[i].prev = ac+i-1;
		ac[i].pos = i;
	}
	ac[nseq-1].next = NULL;


	for( i=0; i<nseq; i++ ) 
	{
		hist[i] = -1;
		nmemar[i] = 1;
	}

	fprintf( stderr, "\n" );
	for( k=0; k<nseq-1; k++ )
	{
		if( k % 10 == 0 ) fprintf( stderr, "\r% 5d / %d", k, nseq );
#if 0
		minscore = 999.9;
		for( acpti=ac; acpti->next!=NULL; acpti=acpti->next ) 
		{
			i = acpti->pos;
//			fprintf( stderr, "k=%d i=%d\n", k, i );
			if( mindisfrom[i] < minscore ) // muscle
			{
				im = i;
				minscore = mindisfrom[i];
			}
		}
		jm = nearest[im];
		if( jm < im ) 
		{
			j=jm; jm=im; im=j;
		}
#else
		minscore = 0.0;
		len[k][0] = len[k][1] = -1.0;
		loadtreeoneline( node, len[k], fp );
		im = node[0];
		jm = node[1];

		if( len[k][0] == -1.0 || len[k][1] == -1.0 )
		{
			fprintf( stderr, "\n\nERROR: Branch length is not given.\n" );
			exit( 1 );
		}

		if( len[k][0] < 0.0 ) len[k][0] = 0.0;
		if( len[k][1] < 0.0 ) len[k][1] = 0.0;

#endif

		prevnode = hist[im];
		nmemim = nmemar[im];

//		fprintf( stderr, "prevnode = %d, nmemim = %d\n", prevnode, nmemim );

		intpt = topol[k][0] = (int *)realloc( topol[k][0], ( nmemim + 1 ) * sizeof( int ) );
		if( prevnode == -1 )
		{
			*intpt++ = im;
			*intpt = -1;
		}
		else
		{
			pt1 = topol[prevnode][0];
			pt2 = topol[prevnode][1];
			if( *pt1 > *pt2 )
			{
				pt11 = pt2;
				pt22 = pt1;
			}
			else
			{
				pt11 = pt1;
				pt22 = pt2;
			}
			for( intpt2=pt11; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			for( intpt2=pt22; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			*intpt = -1;
		}


		nmemjm = nmemar[jm];
		prevnode = hist[jm];

//		fprintf( stderr, "prevnode = %d, nmemjm = %d\n", prevnode, nmemjm );

		intpt = topol[k][1] = (int *)realloc( topol[k][1], ( nmemjm + 1 ) * sizeof( int ) );
		if( !intpt )
		{
			fprintf( stderr, "Cannot reallocate topol\n" );
			exit( 1 );
		}
		if( prevnode == -1 )
		{
			*intpt++ = jm;
			*intpt = -1;
		}
		else
		{
			pt1 = topol[prevnode][0];
			pt2 = topol[prevnode][1];
			if( *pt1 > *pt2 )
			{
				pt11 = pt2;
				pt22 = pt1;
			}
			else
			{
				pt11 = pt1;
				pt22 = pt2;
			}
			for( intpt2=pt11; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			for( intpt2=pt22; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			*intpt = -1;
		}

		minscore *= 0.5;

//		len[k][0] = ( minscore - tmptmplen[im] );
//		len[k][1] = ( minscore - tmptmplen[jm] );
//		len[k][0] = -1;
//		len[k][1] = -1;


		hist[im] = k;
		nmemar[im] = nmemim + nmemjm;

		mindisfrom[im] = 999.9;
		for( acpti=ac; acpti!=NULL; acpti=acpti->next )
        {
			i = acpti->pos;
            if( i != im && i != jm )
            {
				if( i < im )
				{
					 miniim = i;
					 maxiim = im;
					 minijm = i;
					 maxijm = jm;
				}
				else if( i < jm )
				{
					 miniim = im;
					 maxiim = i;
					 minijm = i;
					 maxijm = jm;
				}
				else
				{
					 miniim = im;
					 maxiim = i;
					 minijm = jm;
					 maxijm = i;
				}
            }
        }

		sprintf( treetmp, "(%s:%7.5f,%s:%7.5f)", tree[im], len[k][0], tree[jm], len[k][1] );
		strcpy( tree[im], treetmp );

//		fprintf( stderr, "im,jm=%d,%d\n", im, jm );
		acjmprev = ac[jm].prev; 
		acjmnext = ac[jm].next; 
		acjmprev->next = acjmnext;
		if( acjmnext != NULL )
			acjmnext->prev = acjmprev;
//		free( (void *)eff[jm] ); eff[jm] = NULL;

#if 0 // muscle seems to miss this.
		for( acpti=ac; acpti!=NULL; acpti=acpti->next )
		{
			i = acpti->pos;
			if( nearest[i] == im ) 
			{
//				fprintf( stderr, "calling setnearest\n" );
//				setnearest( nseq, ac, eff, mindisfrom+i, nearest+i, i );
			}
		}
#endif


#if 0
        fprintf( stdout, "vSTEP-%03d:\n", k+1 );
		fprintf( stdout, "len0 = %f\n", len[k][0] );
        for( i=0; topol[k][0][i]>-1; i++ ) fprintf( stdout, " %03d", topol[k][0][i]+1 );
        fprintf( stdout, "\n" );
		fprintf( stdout, "len1 = %f\n", len[k][1] );
        for( i=0; topol[k][1][i]>-1; i++ ) fprintf( stdout, " %03d", topol[k][1][i]+1 );
        fprintf( stdout, "\n" );
#endif
    }
	fclose( fp );
	fp = fopen( "infile.tree", "w" );
		fprintf( fp, "%s\n", treetmp );
		fprintf( fp, "#by loadtree\n" );
	fclose( fp );

	FreeCharMtx( tree );
	free( treetmp );
	free( nametmp );
	free( hist ); hist = NULL;
	free( (char *)ac ); ac = NULL;
	free( (void *)nmemar ); nmemar = NULL;
	free( mindisfrom );
	free( nearest );


}

static float sueff1, sueff05;
static double sueff1_double, sueff05_double;

static float cluster_mix_float( float d1, float d2 )
{
	return( MIN( d1, d2 ) * sueff1 + ( d1 + d2 ) * sueff05 ); 
}
static float cluster_average_float( float d1, float d2 )
{
	return( ( d1 + d2 ) * 0.5 ); 
}
static float cluster_minimum_float( float d1, float d2 )
{
	return( MIN( d1, d2 ) ); 
}
static double cluster_mix_double( double d1, double d2 )
{
	return( MIN( d1, d2 ) * sueff1_double + ( d1 + d2 ) * sueff05_double ); 
}
static double cluster_average_double( double d1, double d2 )
{
	return( ( d1 + d2 ) * 0.5 ); 
}
static double cluster_minimum_double( double d1, double d2 )
{
	return( MIN( d1, d2 ) ); 
}

void loadtop( int nseq, float **eff, int ***topol, float **len ) // computes branch length BUG!!
{
	int i, k, miniim, maxiim, minijm, maxijm;
	int *intpt, *intpt2;
	static Bchain *ac = NULL;
	float eff1, eff0;
	static float *tmptmplen = NULL;
	static int *hist = NULL;
	int im = -1, jm = -1;
	Bchain *acjmnext, *acjmprev;
	int prevnode;
	Bchain *acpti;
	int *pt1, *pt2, *pt11, *pt22;
	static int *nmemar;
	int nmemim, nmemjm;
	float minscore;
	static char **tree;
	static char *treetmp;
	FILE *fp;
	int node[2];
	float dumfl[2];
	float (*clusterfuncpt[1])(float,float);


	sueff1 = 1 - SUEFF;
	sueff05 = SUEFF * 0.5;
	if ( treemethod == 'X' )
		clusterfuncpt[0] = cluster_mix_float;
	else if ( treemethod == 'E' )
		clusterfuncpt[0] = cluster_average_float;
	else if ( treemethod == 'q' )
		clusterfuncpt[0] = cluster_minimum_float;
	else
	{
		fprintf( stderr, "Unknown treemethod, %c\n", treemethod );
		exit( 1 );
	}

	fp = fopen( "_guidetree", "r" );
	if( !fp )
	{
		fprintf( stderr, "cannot open _guidetree\n" );
		exit( 1 );
	}

	if( !hist )
	{
		treetmp = AllocateCharVec( njob*50 );
		tree = AllocateCharMtx( njob, njob*50 );
		hist = AllocateIntVec( njob );
		tmptmplen = AllocateFloatVec( njob );
		ac = (Bchain *)malloc( njob * sizeof( Bchain ) );
		nmemar = AllocateIntVec( njob );
	}
	
    for( i=0; i<nseq; i++ ) sprintf( tree[i], "%d", i+1 );
	for( i=0; i<nseq; i++ )
	{
		ac[i].next = ac+i+1;
		ac[i].prev = ac+i-1;
		ac[i].pos = i;
	}
	ac[nseq-1].next = NULL;

	for( i=0; i<nseq; i++ ) tmptmplen[i] = 0.0;
	for( i=0; i<nseq; i++ ) 
	{
		hist[i] = -1;
		nmemar[i] = 1;
	}

	fprintf( stderr, "\n" );
	for( k=0; k<nseq-1; k++ )
	{
		if( k % 10 == 0 ) fprintf( stderr, "\r% 5d / %d", k, nseq );

#if 0
		minscore = 99999.9;
		for( acpti=ac; acpti->next!=NULL; acpti=acpti->next ) 
		{
			effpt = eff[i=acpti->pos];
//			i = acpti->pos;
			for( acptj=acpti->next; acptj!=NULL; acptj=acptj->next )
	        {
//				j=acptj->pos;
//				tmpfloat = eff[i][j-i];
//				if( tmpfloat < minscore )
				if( (tmpfloat= effpt[(j=acptj->pos)-i]) < minscore )
				{
					minscore = tmpfloat;
					im = i; jm = j;
				}
			}
		}

//		fprintf( stderr, "im=%d, jm=%d, minscore = %f\n", im, jm, minscore );
#else
		dumfl[0] = dumfl[1] = -1.0;
		loadtreeoneline( node, dumfl, fp );
		im = node[0];
		jm = node[1];
		minscore = eff[im][jm-im];

//		fprintf( stderr, "im=%d, jm=%d, minscore = %f\n", im, jm, minscore );


		if( dumfl[0] != -1.0 || dumfl[1] != -1.0 )
		{
			fprintf( stderr, "\n\nERROR: Branch length should not be given.\n" );
			exit( 1 );
		}

#endif


		prevnode = hist[im];
		nmemim = nmemar[im];
		intpt = topol[k][0] = (int *)realloc( topol[k][0], ( nmemim + 1 ) * sizeof( int ) );
		if( prevnode == -1 )
		{
			*intpt++ = im;
			*intpt = -1;
		}
		else
		{
			pt1 = topol[prevnode][0];
			pt2 = topol[prevnode][1];
			if( *pt1 > *pt2 )
			{
				pt11 = pt2;
				pt22 = pt1;
			}
			else
			{
				pt11 = pt1;
				pt22 = pt2;
			}
			for( intpt2=pt11; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			for( intpt2=pt22; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			*intpt = -1;
		}

		prevnode = hist[jm];
		nmemjm = nmemar[jm];
		intpt = topol[k][1] = (int *)realloc( topol[k][1], ( nmemjm + 1 ) * sizeof( int ) );
		if( !intpt )
		{
			fprintf( stderr, "Cannot reallocate topol\n" );
			exit( 1 );
		}
		if( prevnode == -1 )
		{
			*intpt++ = jm;
			*intpt = -1;
		}
		else
		{
			pt1 = topol[prevnode][0];
			pt2 = topol[prevnode][1];
			if( *pt1 > *pt2 )
			{
				pt11 = pt2;
				pt22 = pt1;
			}
			else
			{
				pt11 = pt1;
				pt22 = pt2;
			}
			for( intpt2=pt11; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			for( intpt2=pt22; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			*intpt = -1;
		}

		minscore *= 0.5;

		len[k][0] = ( minscore - tmptmplen[im] );
		len[k][1] = ( minscore - tmptmplen[jm] );

		if( len[k][0] < 0.0 ) len[k][0] = 0.0;
		if( len[k][1] < 0.0 ) len[k][1] = 0.0;

		tmptmplen[im] = minscore;

		hist[im] = k;
		nmemar[im] = nmemim + nmemjm;

		for( acpti=ac; acpti!=NULL; acpti=acpti->next )
        {
			i = acpti->pos;
            if( i != im && i != jm )
            {
				if( i < im )
				{
					 miniim = i;
					 maxiim = im;
					 minijm = i;
					 maxijm = jm;
				}
				else if( i < jm )
				{
					 miniim = im;
					 maxiim = i;
					 minijm = i;
					 maxijm = jm;
				}
				else
				{
					 miniim = im;
					 maxiim = i;
					 minijm = jm;
					 maxijm = i;
				}
				eff0 = eff[miniim][maxiim-miniim];
				eff1 = eff[minijm][maxijm-minijm];
#if 0
                eff[miniim][maxiim-miniim] =
				MIN( eff0, eff1 ) * sueff1 + ( eff0 + eff1 ) * sueff05; 
#else
                eff[miniim][maxiim-miniim] =
				(clusterfuncpt[0])( eff0, eff1 );
#endif
            }
        }
//		sprintf( treetmp, "(%s,%s)", tree[im], tree[jm] );
		sprintf( treetmp, "(%s:%7.5f,%s:%7.5f)", tree[im], len[k][0], tree[jm], len[k][1] );
		strcpy( tree[im], treetmp );

		acjmprev = ac[jm].prev; 
		acjmnext = ac[jm].next; 
		acjmprev->next = acjmnext;
		if( acjmnext != NULL )
			acjmnext->prev = acjmprev;
		free( (void *)eff[jm] ); eff[jm] = NULL;
#if 0
        fprintf( stdout, "vSTEP-%03d:\n", k+1 );
		fprintf( stdout, "len0 = %f\n", len[k][0] );
        for( i=0; topol[k][0][i]>-1; i++ ) fprintf( stdout, " %03d", topol[k][0][i]+1 );
        fprintf( stdout, "\n" );
		fprintf( stdout, "len1 = %f\n", len[k][1] );
        for( i=0; topol[k][1][i]>-1; i++ ) fprintf( stdout, " %03d", topol[k][1][i]+1 );
        fprintf( stdout, "\n" );
#endif
    }
#if 1
	fclose( fp );
	fp = fopen( "infile.tree", "w" );
		fprintf( fp, "%s\n", treetmp );
		fprintf( fp, "by loadtop\n" );
	fclose( fp );
#endif
	free( (void *)tmptmplen ); tmptmplen = NULL;
	free( hist ); hist = NULL;
	free( (char *)ac ); ac = NULL;
	free( (void *)nmemar ); nmemar = NULL;

}


void fixed_musclesupg_float_realloc_nobk_halfmtx_treeout( int nseq, float **eff, int ***topol, float **len, char **name, int *nlen )
{
	int i, j, k, miniim, maxiim, minijm, maxijm;
	int *intpt, *intpt2;
	float tmpfloat;
	float eff1, eff0;
	static float *tmptmplen = NULL;
	static int *hist = NULL;
	static Bchain *ac = NULL;
	int im = -1, jm = -1;
	Bchain *acjmnext, *acjmprev;
	int prevnode;
	Bchain *acpti;
	int *pt1, *pt2, *pt11, *pt22;
	static int *nmemar;
	int nmemim, nmemjm;
	float minscore;
	int *nearest = NULL; // by D.Mathog, a guess
	float *mindisfrom = NULL; // by D.Mathog, a guess
	static char **tree;
	static char *treetmp;
	static char *nametmp;
	FILE *fp;
	float (*clusterfuncpt[1])(float,float);


	sueff1 = 1 - SUEFF;
	sueff05 = SUEFF * 0.5;
	if ( treemethod == 'X' )
		clusterfuncpt[0] = cluster_mix_float;
	else if ( treemethod == 'E' )
		clusterfuncpt[0] = cluster_average_float;
	else if ( treemethod == 'q' )
		clusterfuncpt[0] = cluster_minimum_float;
	else
	{
		fprintf( stderr, "Unknown treemethod, %c\n", treemethod );
		exit( 1 );
	}

	if( !hist )
	{
		hist = AllocateIntVec( njob );
		tmptmplen = AllocateFloatVec( njob );
		ac = (Bchain *)malloc( njob * sizeof( Bchain ) );
		nmemar = AllocateIntVec( njob );
		mindisfrom = AllocateFloatVec( njob );
		nearest = AllocateIntVec( njob );
		treetmp = AllocateCharVec( njob*50 );
		nametmp = AllocateCharVec( 30 );
		tree = AllocateCharMtx( njob, njob*50 );
	}

	
    for( i=0; i<nseq; i++ )
	{
		for( j=0; j<30; j++ ) nametmp[j] = 0;
		for( j=0; j<30; j++ ) 
		{
			if( isalnum( name[i][j] ) )
				nametmp[j] = name[i][j];
			else
				nametmp[j] = '_';
		}
		nametmp[30] = 0;
//		sprintf( tree[i], "%d_l=%d_%.20s", i+1, nlen[i], nametmp+1 );
		sprintf( tree[i], "%d_%.20s", i+1, nametmp+1 );
	}
	for( i=0; i<nseq; i++ )
	{
		ac[i].next = ac+i+1;
		ac[i].prev = ac+i-1;
		ac[i].pos = i;
	}
	ac[nseq-1].next = NULL;

	for( i=0; i<nseq; i++ ) setnearest( nseq, ac, eff, mindisfrom+i, nearest+i, i ); // muscle

	for( i=0; i<nseq; i++ ) tmptmplen[i] = 0.0;
	for( i=0; i<nseq; i++ ) 
	{
		hist[i] = -1;
		nmemar[i] = 1;
	}

	fprintf( stderr, "\n" );
	for( k=0; k<nseq-1; k++ )
	{
		if( k % 10 == 0 ) fprintf( stderr, "\r% 5d / %d", k, nseq );

		minscore = 999.9;
		for( acpti=ac; acpti->next!=NULL; acpti=acpti->next ) 
		{
			i = acpti->pos;
//			fprintf( stderr, "k=%d i=%d\n", k, i );
			if( mindisfrom[i] < minscore ) // muscle
			{
				im = i;
				minscore = mindisfrom[i];
			}
		}
		jm = nearest[im];
		if( jm < im ) 
		{
			j=jm; jm=im; im=j;
		}


		prevnode = hist[im];
		nmemim = nmemar[im];
		intpt = topol[k][0] = (int *)realloc( topol[k][0], ( nmemim + 1 ) * sizeof( int ) );
		if( prevnode == -1 )
		{
			*intpt++ = im;
			*intpt = -1;
		}
		else
		{
			pt1 = topol[prevnode][0];
			pt2 = topol[prevnode][1];
			if( *pt1 > *pt2 )
			{
				pt11 = pt2;
				pt22 = pt1;
			}
			else
			{
				pt11 = pt1;
				pt22 = pt2;
			}
			for( intpt2=pt11; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			for( intpt2=pt22; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			*intpt = -1;
		}

		prevnode = hist[jm];
		nmemjm = nmemar[jm];
		intpt = topol[k][1] = (int *)realloc( topol[k][1], ( nmemjm + 1 ) * sizeof( int ) );
		if( !intpt )
		{
			fprintf( stderr, "Cannot reallocate topol\n" );
			exit( 1 );
		}
		if( prevnode == -1 )
		{
			*intpt++ = jm;
			*intpt = -1;
		}
		else
		{
			pt1 = topol[prevnode][0];
			pt2 = topol[prevnode][1];
			if( *pt1 > *pt2 )
			{
				pt11 = pt2;
				pt22 = pt1;
			}
			else
			{
				pt11 = pt1;
				pt22 = pt2;
			}
			for( intpt2=pt11; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			for( intpt2=pt22; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			*intpt = -1;
		}

		minscore *= 0.5;

		len[k][0] = ( minscore - tmptmplen[im] );
		len[k][1] = ( minscore - tmptmplen[jm] );

		tmptmplen[im] = minscore;

		hist[im] = k;
		nmemar[im] = nmemim + nmemjm;

		mindisfrom[im] = 999.9;
		for( acpti=ac; acpti!=NULL; acpti=acpti->next )
        {
			i = acpti->pos;
            if( i != im && i != jm )
            {
				if( i < im )
				{
					 miniim = i;
					 maxiim = im;
					 minijm = i;
					 maxijm = jm;
				}
				else if( i < jm )
				{
					 miniim = im;
					 maxiim = i;
					 minijm = i;
					 maxijm = jm;
				}
				else
				{
					 miniim = im;
					 maxiim = i;
					 minijm = jm;
					 maxijm = i;
				}
				eff0 = eff[miniim][maxiim-miniim];
				eff1 = eff[minijm][maxijm-minijm];
#if 0
                		tmpfloat = eff[miniim][maxiim-miniim] =
				MIN( eff0, eff1 ) * sueff1 + ( eff0 + eff1 ) * sueff05; 
#else
                tmpfloat = eff[miniim][maxiim-miniim] =
				(clusterfuncpt[0])( eff0, eff1 );
#endif
				if( tmpfloat < mindisfrom[i]  )
				{
					mindisfrom[i] = tmpfloat;
					nearest[i] = im;
				}
				if( tmpfloat < mindisfrom[im]  )
				{
					mindisfrom[im] = tmpfloat;
					nearest[im] = i;
				}
				if( nearest[i] == jm )
				{
					nearest[i] = im;
				}
            }
        }

		sprintf( treetmp, "(%s:%7.5f,%s:%7.5f)", tree[im], len[k][0], tree[jm], len[k][1] );
		strcpy( tree[im], treetmp );

//		fprintf( stderr, "im,jm=%d,%d\n", im, jm );
		acjmprev = ac[jm].prev; 
		acjmnext = ac[jm].next; 
		acjmprev->next = acjmnext;
		if( acjmnext != NULL )
			acjmnext->prev = acjmprev;
		free( (void *)eff[jm] ); eff[jm] = NULL;

#if 1 // muscle seems to miss this.
		for( acpti=ac; acpti!=NULL; acpti=acpti->next )
		{
			i = acpti->pos;
			if( nearest[i] == im ) 
			{
//				fprintf( stderr, "calling setnearest\n" );
				setnearest( nseq, ac, eff, mindisfrom+i, nearest+i, i );
			}
		}
#endif


#if 0
        fprintf( stdout, "vSTEP-%03d:\n", k+1 );
		fprintf( stdout, "len0 = %f\n", len[k][0] );
        for( i=0; topol[k][0][i]>-1; i++ ) fprintf( stdout, " %03d", topol[k][0][i]+1 );
        fprintf( stdout, "\n" );
		fprintf( stdout, "len1 = %f\n", len[k][1] );
        for( i=0; topol[k][1][i]>-1; i++ ) fprintf( stdout, " %03d", topol[k][1][i]+1 );
        fprintf( stdout, "\n" );
#endif
    }
	fp = fopen( "infile.tree", "w" );
		fprintf( fp, "%s\n", treetmp );
	fclose( fp );

	FreeCharMtx( tree );
	free( treetmp );
	free( nametmp );
	free( (void *)tmptmplen ); tmptmplen = NULL;
	free( hist ); hist = NULL;
	free( (char *)ac ); ac = NULL;
	free( (void *)nmemar ); nmemar = NULL;
	free( mindisfrom );
	free( nearest );

}


void fixed_musclesupg_float_realloc_nobk_halfmtx( int nseq, float **eff, int ***topol, float **len )
{
	int i, j, k, miniim, maxiim, minijm, maxijm;
	int *intpt, *intpt2;
	float tmpfloat;
	float eff1, eff0;
	static float *tmptmplen = NULL;
	static int *hist = NULL;
	static Bchain *ac = NULL;
	int im = -1, jm = -1;
	Bchain *acjmnext, *acjmprev;
	int prevnode;
	Bchain *acpti;
	int *pt1, *pt2, *pt11, *pt22;
	static int *nmemar;
	int nmemim, nmemjm;
	float minscore;
//	float sueff1 = 1 - SUEFF;
//	float sueff05 = SUEFF * 0.5;
	int *nearest = NULL; // by Mathog, a guess
	float *mindisfrom = NULL; // by Mathog, a guess
	float (*clusterfuncpt[1])(float,float);


	sueff1 = 1 - SUEFF;
	sueff05 = SUEFF * 0.5;
	if ( treemethod == 'X' )
		clusterfuncpt[0] = cluster_mix_float;
	else if ( treemethod == 'E' )
		clusterfuncpt[0] = cluster_average_float;
	else if ( treemethod == 'q' )
		clusterfuncpt[0] = cluster_minimum_float;
	else
	{
		fprintf( stderr, "Unknown treemethod, %c\n", treemethod );
		exit( 1 );
	}

	if( !hist )
	{
		hist = AllocateIntVec( njob );
		tmptmplen = AllocateFloatVec( njob );
		ac = (Bchain *)malloc( njob * sizeof( Bchain ) );
		nmemar = AllocateIntVec( njob );
		mindisfrom = AllocateFloatVec( njob );
		nearest = AllocateIntVec( njob );
	}

	
	for( i=0; i<nseq; i++ )
	{
		ac[i].next = ac+i+1;
		ac[i].prev = ac+i-1;
		ac[i].pos = i;
	}
	ac[nseq-1].next = NULL;

	for( i=0; i<nseq; i++ ) setnearest( nseq, ac, eff, mindisfrom+i, nearest+i, i ); // muscle

	for( i=0; i<nseq; i++ ) tmptmplen[i] = 0.0;
	for( i=0; i<nseq; i++ ) 
	{
		hist[i] = -1;
		nmemar[i] = 1;
	}

	fprintf( stderr, "\n" );
	for( k=0; k<nseq-1; k++ )
	{
		if( k % 10 == 0 ) fprintf( stderr, "\r% 5d / %d", k, nseq );

		minscore = 999.9;
		for( acpti=ac; acpti->next!=NULL; acpti=acpti->next ) 
		{
			i = acpti->pos;
//			fprintf( stderr, "k=%d i=%d\n", k, i );
			if( mindisfrom[i] < minscore ) // muscle
			{
				im = i;
				minscore = mindisfrom[i];
			}
		}
		jm = nearest[im];
		if( jm < im ) 
		{
			j=jm; jm=im; im=j;
		}


		prevnode = hist[im];
		nmemim = nmemar[im];
		intpt = topol[k][0] = (int *)realloc( topol[k][0], ( nmemim + 1 ) * sizeof( int ) );
		if( prevnode == -1 )
		{
			*intpt++ = im;
			*intpt = -1;
		}
		else
		{
			pt1 = topol[prevnode][0];
			pt2 = topol[prevnode][1];
			if( *pt1 > *pt2 )
			{
				pt11 = pt2;
				pt22 = pt1;
			}
			else
			{
				pt11 = pt1;
				pt22 = pt2;
			}
			for( intpt2=pt11; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			for( intpt2=pt22; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			*intpt = -1;
		}

		prevnode = hist[jm];
		nmemjm = nmemar[jm];
		intpt = topol[k][1] = (int *)realloc( topol[k][1], ( nmemjm + 1 ) * sizeof( int ) );
		if( !intpt )
		{
			fprintf( stderr, "Cannot reallocate topol\n" );
			exit( 1 );
		}
		if( prevnode == -1 )
		{
			*intpt++ = jm;
			*intpt = -1;
		}
		else
		{
			pt1 = topol[prevnode][0];
			pt2 = topol[prevnode][1];
			if( *pt1 > *pt2 )
			{
				pt11 = pt2;
				pt22 = pt1;
			}
			else
			{
				pt11 = pt1;
				pt22 = pt2;
			}
			for( intpt2=pt11; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			for( intpt2=pt22; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			*intpt = -1;
		}

		minscore *= 0.5;

		len[k][0] = ( minscore - tmptmplen[im] );
		len[k][1] = ( minscore - tmptmplen[jm] );

		tmptmplen[im] = minscore;

		hist[im] = k;
		nmemar[im] = nmemim + nmemjm;

		mindisfrom[im] = 999.9;
		for( acpti=ac; acpti!=NULL; acpti=acpti->next )
        {
			i = acpti->pos;
            if( i != im && i != jm )
            {
				if( i < im )
				{
					 miniim = i;
					 maxiim = im;
					 minijm = i;
					 maxijm = jm;
				}
				else if( i < jm )
				{
					 miniim = im;
					 maxiim = i;
					 minijm = i;
					 maxijm = jm;
				}
				else
				{
					 miniim = im;
					 maxiim = i;
					 minijm = jm;
					 maxijm = i;
				}
				eff0 = eff[miniim][maxiim-miniim];
				eff1 = eff[minijm][maxijm-minijm];
				tmpfloat = eff[miniim][maxiim-miniim] =
#if 0
				MIN( eff0, eff1 ) * sueff1 + ( eff0 + eff1 ) * sueff05; 
#else
				(clusterfuncpt[0])( eff0, eff1 );
#endif
				if( tmpfloat < mindisfrom[i]  )
				{
					mindisfrom[i] = tmpfloat;
					nearest[i] = im;
				}
				if( tmpfloat < mindisfrom[im]  )
				{
					mindisfrom[im] = tmpfloat;
					nearest[im] = i;
				}
				if( nearest[i] == jm )
				{
					nearest[i] = im;
				}
            }
        }

//		fprintf( stderr, "im,jm=%d,%d\n", im, jm );
		acjmprev = ac[jm].prev; 
		acjmnext = ac[jm].next; 
		acjmprev->next = acjmnext;
		if( acjmnext != NULL )
			acjmnext->prev = acjmprev;
		free( (void *)eff[jm] ); eff[jm] = NULL;

#if 1 // muscle seems to miss this.
		for( acpti=ac; acpti!=NULL; acpti=acpti->next )
		{
			i = acpti->pos;
			if( nearest[i] == im ) 
			{
//				fprintf( stderr, "calling setnearest\n" );
				setnearest( nseq, ac, eff, mindisfrom+i, nearest+i, i );
			}
		}
#endif


#if 0
        fprintf( stdout, "vSTEP-%03d:\n", k+1 );
		fprintf( stdout, "len0 = %f\n", len[k][0] );
        for( i=0; topol[k][0][i]>-1; i++ ) fprintf( stdout, " %03d", topol[k][0][i]+1 );
        fprintf( stdout, "\n" );
		fprintf( stdout, "len1 = %f\n", len[k][1] );
        for( i=0; topol[k][1][i]>-1; i++ ) fprintf( stdout, " %03d", topol[k][1][i]+1 );
        fprintf( stdout, "\n" );
#endif
    }
	free( (void *)tmptmplen ); tmptmplen = NULL;
	free( hist ); hist = NULL;
	free( (char *)ac ); ac = NULL;
	free( (void *)nmemar ); nmemar = NULL;
	free( mindisfrom );
	free( nearest );
}








void veryfastsupg_double_loadtop( int nseq, double **eff, int ***topol, double **len ) // BUG!!!
{
    int i, k, miniim, maxiim, minijm, maxijm;
	int *intpt, *intpt2;
	double eff1, eff0;
	static double *tmptmplen = NULL;
    static int *hist = NULL;
	static Achain *ac = NULL;
	double minscore;
	static char **tree;
	static char *treetmp;
	int im = -1, jm = -1;
	int prevnode, acjmnext, acjmprev;
	int *pt1, *pt2, *pt11, *pt22;
	FILE *fp;
	int node[2];
	float dumfl[2];

	fp = fopen( "_guidetree", "r" );
	if( !fp )
	{
		fprintf( stderr, "cannot open _guidetree\n" );
		exit( 1 );
	}

	if( !hist )
	{
		treetmp = AllocateCharVec( njob*50 );
		tree = AllocateCharMtx( njob, njob*50 );
		hist = AllocateIntVec( njob );
		tmptmplen = (double *)malloc( njob * sizeof( double ) );
		ac = (Achain *)malloc( njob * sizeof( Achain ) );
	}
	for( i=0; i<nseq; i++ ) sprintf( tree[i], "%d", i+1 );
	
	for( i=0; i<nseq; i++ )
	{
		ac[i].next = i+1;
		ac[i].prev = i-1;
//		ac[i].curr = i;
	}
	ac[nseq-1].next = -1;

	for( i=0; i<nseq; i++ ) tmptmplen[i] = 0.0;
    for( i=0; i<nseq; i++ ) hist[i] = -1;

	fprintf( stderr, "\n" );
    for( k=0; k<nseq-1; k++ )
    {
		if( k % 10 == 0 ) fprintf( stderr, "%d / %d\r", k, nseq );

#if 0
		minscore = 99999.9;
		for( i=0; ac[i].next!=-1; i=ac[i].next ) 
		{
			for( j=ac[i].next; j!=-1; j=ac[j].next )
	        {
				tmpdouble = eff[i][j];
				if( tmpdouble < minscore )
				{
					minscore = tmpdouble;
					im = i; jm = j;
				}
			}
		}
#else
		dumfl[0] = dumfl[1] = -1.0;
		loadtreeoneline( node, dumfl, fp );
		im = node[0];
		jm = node[1];
		minscore = eff[im][jm];

//		fprintf( stderr, "im=%d, jm=%d, minscore = %f\n", im, jm, minscore );


		if( dumfl[0] != -1.0 || dumfl[1] != -1.0 )
		{
			fprintf( stderr, "\n\nBranch length should not given.\n" );
			exit( 1 );
		}
#endif

//		fprintf( stderr, "im=%d, jm=%d\n", im, jm );

		intpt = topol[k][0];
		prevnode = hist[im];
		if( prevnode == -1 )
		{
			*intpt++ = im;
			*intpt = -1;
		}
		else
		{
			pt1 = topol[prevnode][0];
			pt2 = topol[prevnode][1];
			if( *pt1 > *pt2 )
			{
				pt11 = pt2;
				pt22 = pt1;
			}
			else
			{
				pt11 = pt1;
				pt22 = pt2;
			}
			for( intpt2=pt11; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			for( intpt2=pt22; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			*intpt = -1;
		}

		intpt = topol[k][1];
		prevnode = hist[jm];
		if( prevnode == -1 )
		{
			*intpt++ = jm;
			*intpt = -1;
		}
		else
		{
			pt1 = topol[prevnode][0];
			pt2 = topol[prevnode][1];
			if( *pt1 > *pt2 )
			{
				pt11 = pt2;
				pt22 = pt1;
			}
			else
			{
				pt11 = pt1;
				pt22 = pt2;
			}
			for( intpt2=pt11; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			for( intpt2=pt22; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			*intpt = -1;
		}

		minscore *= 0.5;

		len[k][0] = minscore - tmptmplen[im];
		len[k][1] = minscore - tmptmplen[jm];

		if( len[k][0] < 0.0 ) len[k][0] = 0.0;
		if( len[k][1] < 0.0 ) len[k][1] = 0.0;

		tmptmplen[im] = minscore;

		hist[im] = k;

		for( i=0; i!=-1; i=ac[i].next )
        {
            if( i != im && i != jm )
            {
				if( i < im )
				{
					 miniim = i;
					 maxiim = im;
					 minijm = i;
					 maxijm = jm;
				}
				else if( i < jm )
				{
					 miniim = im;
					 maxiim = i;
					 minijm = i;
					 maxijm = jm;
				}
				else
				{
					 miniim = im;
					 maxiim = i;
					 minijm = jm;
					 maxijm = i;
				}
				eff0 = eff[miniim][maxiim];
				eff1 = eff[minijm][maxijm];
                eff[miniim][maxiim] =
                MIN( eff0, eff1 ) * ( 1.0 - SUEFF ) +
				( eff0 + eff1 ) * 0.5 * SUEFF;
            }
        }
		acjmprev = ac[jm].prev; 
		acjmnext = ac[jm].next; 
		ac[acjmprev].next = acjmnext;
		if( acjmnext != -1 )
			ac[acjmnext].prev = acjmprev;

		sprintf( treetmp, "(%s:%7.5f,%s:%7.5f)", tree[im], len[k][0], tree[jm], len[k][1] );
		strcpy( tree[im], treetmp );
#if 0
        fprintf( stdout, "STEP-%03d:\n", k+1 );
		fprintf( stdout, "len0 = %f\n", len[k][0] );
        for( i=0; topol[k][0][i]>-1; i++ ) fprintf( stdout, " %03d", topol[k][0][i] );
        fprintf( stdout, "\n" );
		fprintf( stdout, "len1 = %f\n", len[k][1] );
        for( i=0; topol[k][1][i]>-1; i++ ) fprintf( stdout, " %03d", topol[k][1][i] );
        fprintf( stdout, "\n" );
#endif
    }
	fclose( fp );

	fp = fopen( "infile.tree", "w" );
	fprintf( fp, "%s\n", treetmp );
//	fprintf( fp, "by veryfastsupg_double_loadtop\n" );
	fclose( fp );

#if 1
	fprintf( stderr, "\n" );
	free( (void *)tmptmplen ); tmptmplen = NULL;
	free( hist ); hist = NULL;
	free( (char *)ac ); ac = NULL;
	FreeCharMtx( tree );
	free( treetmp );
#endif
}

void veryfastsupg_double_loadtree( int nseq, double **eff, int ***topol, double **len )
{
    int i, k, miniim, maxiim, minijm, maxijm;
	int *intpt, *intpt2;
	double eff1, eff0;
	static double *tmptmplen = NULL;
    static int *hist = NULL;
	static Achain *ac = NULL;
	double minscore;
	static char **tree;
	static char *treetmp;
	int im = -1, jm = -1;
	int prevnode, acjmnext, acjmprev;
	int *pt1, *pt2, *pt11, *pt22;
	FILE *fp;
	int node[2];
	float lenfl[2];

	fp = fopen( "_guidetree", "r" );
	if( !fp )
	{
		fprintf( stderr, "cannot open _guidetree\n" );
		exit( 1 );
	}

	if( !hist )
	{
		treetmp = AllocateCharVec( njob*50 );
		tree = AllocateCharMtx( njob, njob*50 );
		hist = AllocateIntVec( njob );
		tmptmplen = (double *)malloc( njob * sizeof( double ) );
		ac = (Achain *)malloc( njob * sizeof( Achain ) );
	}

	for( i=0; i<nseq; i++ ) sprintf( tree[i], "%d", i+1 );
	
	for( i=0; i<nseq; i++ )
	{
		ac[i].next = i+1;
		ac[i].prev = i-1;
//		ac[i].curr = i;
	}
	ac[nseq-1].next = -1;

	for( i=0; i<nseq; i++ ) tmptmplen[i] = 0.0;
    for( i=0; i<nseq; i++ ) hist[i] = -1;

	fprintf( stderr, "\n" );
    for( k=0; k<nseq-1; k++ )
    {
		if( k % 10 == 0 ) fprintf( stderr, "%d / %d\r", k, nseq );

#if 0
		minscore = 99999.9;
		for( i=0; ac[i].next!=-1; i=ac[i].next ) 
		{
			for( j=ac[i].next; j!=-1; j=ac[j].next )
	        {
				tmpdouble = eff[i][j];
				if( tmpdouble < minscore )
				{
					minscore = tmpdouble;
					im = i; jm = j;
				}
			}
		}
#else
		lenfl[0] = lenfl[1] = -1.0;
		loadtreeoneline( node, lenfl, fp );
		im = node[0];
		jm = node[1];
		minscore = eff[im][jm];

//		fprintf( stderr, "im=%d, jm=%d, minscore = %f\n", im, jm, minscore );


		if( lenfl[0] == -1.0 || lenfl[1] == -1.0 )
		{
			fprintf( stderr, "\n\nWARNING: Branch length is not given.\n" );
			exit( 1 );
		}

		if( lenfl[0] < 0.0 ) lenfl[0] = 0.0;
		if( lenfl[1] < 0.0 ) lenfl[1] = 0.0;
#endif

//		fprintf( stderr, "im=%d, jm=%d\n", im, jm );

		intpt = topol[k][0];
		prevnode = hist[im];
		if( prevnode == -1 )
		{
			*intpt++ = im;
			*intpt = -1;
		}
		else
		{
			pt1 = topol[prevnode][0];
			pt2 = topol[prevnode][1];
			if( *pt1 > *pt2 )
			{
				pt11 = pt2;
				pt22 = pt1;
			}
			else
			{
				pt11 = pt1;
				pt22 = pt2;
			}
			for( intpt2=pt11; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			for( intpt2=pt22; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			*intpt = -1;
		}

		intpt = topol[k][1];
		prevnode = hist[jm];
		if( prevnode == -1 )
		{
			*intpt++ = jm;
			*intpt = -1;
		}
		else
		{
			pt1 = topol[prevnode][0];
			pt2 = topol[prevnode][1];
			if( *pt1 > *pt2 )
			{
				pt11 = pt2;
				pt22 = pt1;
			}
			else
			{
				pt11 = pt1;
				pt22 = pt2;
			}
			for( intpt2=pt11; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			for( intpt2=pt22; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			*intpt = -1;
		}

		minscore *= 0.5;

#if 0
		len[k][0] = minscore - tmptmplen[im];
		len[k][1] = minscore - tmptmplen[jm];
#else
		len[k][0] = lenfl[0];
		len[k][1] = lenfl[1];
#endif

		tmptmplen[im] = minscore;

		hist[im] = k;

		for( i=0; i!=-1; i=ac[i].next )
        {
            if( i != im && i != jm )
            {
				if( i < im )
				{
					 miniim = i;
					 maxiim = im;
					 minijm = i;
					 maxijm = jm;
				}
				else if( i < jm )
				{
					 miniim = im;
					 maxiim = i;
					 minijm = i;
					 maxijm = jm;
				}
				else
				{
					 miniim = im;
					 maxiim = i;
					 minijm = jm;
					 maxijm = i;
				}
				eff0 = eff[miniim][maxiim];
				eff1 = eff[minijm][maxijm];
                eff[miniim][maxiim] =
                MIN( eff0, eff1 ) * ( 1.0 - SUEFF ) +
				( eff0 + eff1 ) * 0.5 * SUEFF;
            }
        }
		acjmprev = ac[jm].prev; 
		acjmnext = ac[jm].next; 
		ac[acjmprev].next = acjmnext;
		if( acjmnext != -1 )
			ac[acjmnext].prev = acjmprev;

		sprintf( treetmp, "(%s:%7.5f,%s:%7.5f)", tree[im], len[k][0], tree[jm], len[k][1] );
		strcpy( tree[im], treetmp );

#if 0
        fprintf( stdout, "STEP-%03d:\n", k+1 );
		fprintf( stdout, "len0 = %f\n", len[k][0] );
        for( i=0; topol[k][0][i]>-1; i++ ) fprintf( stdout, " %03d", topol[k][0][i] );
        fprintf( stdout, "\n" );
		fprintf( stdout, "len1 = %f\n", len[k][1] );
        for( i=0; topol[k][1][i]>-1; i++ ) fprintf( stdout, " %03d", topol[k][1][i] );
        fprintf( stdout, "\n" );
#endif
    }
	fclose( fp );


	fp = fopen( "infile.tree", "w" );
	fprintf( fp, "%s\n", treetmp );
//	fprintf( fp, "by veryfastsupg_double_loadtree\n" );
	fclose( fp );

#if 1
	fprintf( stderr, "\n" );
	free( (void *)tmptmplen ); tmptmplen = NULL;
	free( hist ); hist = NULL;
	free( (char *)ac ); ac = NULL;
	FreeCharMtx( tree );
	free( treetmp );
#endif


}

#if 0
void veryfastsupg_double( int nseq, double **eff, int ***topol, double **len )
{
    int i, j, k, miniim, maxiim, minijm, maxijm;
	int *intpt, *intpt2;
	double tmpdouble;
	double eff1, eff0;
	static double *tmptmplen = NULL;
    static int *hist = NULL;
	static Achain *ac = NULL;
	double minscore;
	int im = -1, jm = -1;
	int prevnode, acjmnext, acjmprev;
	int *pt1, *pt2, *pt11, *pt22;
	if( !hist )
	{
		hist = AllocateIntVec( njob );
		tmptmplen = (double *)malloc( njob * sizeof( double ) );
		ac = (Achain *)malloc( njob * sizeof( Achain ) );
	}
	
	for( i=0; i<nseq; i++ )
	{
		ac[i].next = i+1;
		ac[i].prev = i-1;
//		ac[i].curr = i;
	}
	ac[nseq-1].next = -1;

	for( i=0; i<nseq; i++ ) tmptmplen[i] = 0.0;
    for( i=0; i<nseq; i++ ) hist[i] = -1;

	fprintf( stderr, "\n" );
    for( k=0; k<nseq-1; k++ )
    {
		if( k % 10 == 0 ) fprintf( stderr, "%d / %d\r", k, nseq );

		minscore = 99999.9;
		for( i=0; ac[i].next!=-1; i=ac[i].next ) 
		{
			for( j=ac[i].next; j!=-1; j=ac[j].next )
	        {
				tmpdouble = eff[i][j];
				if( tmpdouble < minscore )
				{
					minscore = tmpdouble;
					im = i; jm = j;
				}
			}
		}

//		fprintf( stderr, "im=%d, jm=%d\n", im, jm );

		intpt = topol[k][0];
		prevnode = hist[im];
		if( prevnode == -1 )
		{
			*intpt++ = im;
			*intpt = -1;
		}
		else
		{
			pt1 = topol[prevnode][0];
			pt2 = topol[prevnode][1];
			if( *pt1 > *pt2 )
			{
				pt11 = pt2;
				pt22 = pt1;
			}
			else
			{
				pt11 = pt1;
				pt22 = pt2;
			}
			for( intpt2=pt11; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			for( intpt2=pt22; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			*intpt = -1;
		}

		intpt = topol[k][1];
		prevnode = hist[jm];
		if( prevnode == -1 )
		{
			*intpt++ = jm;
			*intpt = -1;
		}
		else
		{
			pt1 = topol[prevnode][0];
			pt2 = topol[prevnode][1];
			if( *pt1 > *pt2 )
			{
				pt11 = pt2;
				pt22 = pt1;
			}
			else
			{
				pt11 = pt1;
				pt22 = pt2;
			}
			for( intpt2=pt11; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			for( intpt2=pt22; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			*intpt = -1;
		}

		minscore *= 0.5;

		len[k][0] = minscore - tmptmplen[im];
		len[k][1] = minscore - tmptmplen[jm];

		tmptmplen[im] = minscore;

		hist[im] = k;

		for( i=0; i!=-1; i=ac[i].next )
        {
            if( i != im && i != jm )
            {
				if( i < im )
				{
					 miniim = i;
					 maxiim = im;
					 minijm = i;
					 maxijm = jm;
				}
				else if( i < jm )
				{
					 miniim = im;
					 maxiim = i;
					 minijm = i;
					 maxijm = jm;
				}
				else
				{
					 miniim = im;
					 maxiim = i;
					 minijm = jm;
					 maxijm = i;
				}
				eff0 = eff[miniim][maxiim];
				eff1 = eff[minijm][maxijm];
                eff[miniim][maxiim] =
                MIN( eff0, eff1 ) * ( 1.0 - SUEFF ) +
				( eff0 + eff1 ) * 0.5 * SUEFF;
            }
        }
		acjmprev = ac[jm].prev; 
		acjmnext = ac[jm].next; 
		ac[acjmprev].next = acjmnext;
		if( acjmnext != -1 )
			ac[acjmnext].prev = acjmprev;
#if 0
        fprintf( stdout, "STEP-%03d:\n", k+1 );
		fprintf( stdout, "len0 = %f\n", len[k][0] );
        for( i=0; topol[k][0][i]>-1; i++ ) fprintf( stdout, " %03d", topol[k][0][i] );
        fprintf( stdout, "\n" );
		fprintf( stdout, "len1 = %f\n", len[k][1] );
        for( i=0; topol[k][1][i]>-1; i++ ) fprintf( stdout, " %03d", topol[k][1][i] );
        fprintf( stdout, "\n" );
#endif
    }
#if 1
	fprintf( stderr, "\n" );
	free( (void *)tmptmplen ); tmptmplen = NULL;
	free( hist ); hist = NULL;
	free( (char *)ac ); ac = NULL;
#endif
}
#endif

void veryfastsupg_double_outtree( int nseq, double **eff, int ***topol, double **len, char name[M][B] )
{
    int i, j, k, miniim, maxiim, minijm, maxijm;
	int *intpt, *intpt2;
	double tmpdouble;
	double eff1, eff0;
	static double *tmptmplen = NULL;
    static int *hist = NULL;
	static Achain *ac = NULL;
	double minscore;
	static char **tree;
	static char *treetmp;
	static char *nametmp;
	FILE *fpout;
	int im = -1, jm = -1;
	int prevnode, acjmnext, acjmprev;
	int *pt1, *pt2, *pt11, *pt22;
	double (*clusterfuncpt[1])(double,double);


	sueff1_double = 1 - SUEFF;
	sueff05_double = SUEFF * 0.5;
	if ( treemethod == 'X' )
		clusterfuncpt[0] = cluster_mix_double;
	else if ( treemethod == 'E' )
		clusterfuncpt[0] = cluster_average_double;
	else if ( treemethod == 'q' )
		clusterfuncpt[0] = cluster_minimum_double;
	else
	{
		fprintf( stderr, "Unknown treemethod, %c\n", treemethod );
		exit( 1 );
	}

	if( !hist )
	{
		treetmp = AllocateCharVec( njob*50 );
		tree = AllocateCharMtx( njob, njob*50 );
		hist = AllocateIntVec( njob );
		tmptmplen = (double *)malloc( njob * sizeof( double ) );
		ac = (Achain *)malloc( njob * sizeof( Achain ) );
		nametmp = AllocateCharVec( 30 );
	}

//	for( i=0; i<nseq; i++ ) sprintf( tree[i], "%d", i+1 );
    for( i=0; i<nseq; i++ )
	{
		for( j=0; j<30; j++ ) nametmp[j] = 0;
		for( j=0; j<30; j++ ) 
		{
			if( isalnum( name[i][j] ) )
				nametmp[j] = name[i][j];
			else
				nametmp[j] = '_';
		}
		nametmp[30] = 0;
		sprintf( tree[i], "%d_%.20s", i+1, nametmp+1 );
	}
	
	for( i=0; i<nseq; i++ )
	{
		ac[i].next = i+1;
		ac[i].prev = i-1;
//		ac[i].curr = i;
	}
	ac[nseq-1].next = -1;

	for( i=0; i<nseq; i++ ) tmptmplen[i] = 0.0;
    for( i=0; i<nseq; i++ ) hist[i] = -1;

	fprintf( stderr, "\n" );
    for( k=0; k<nseq-1; k++ )
    {
		if( k % 10 == 0 ) fprintf( stderr, "%d / %d\r", k, nseq );

		minscore = 99999.9;
		for( i=0; ac[i].next!=-1; i=ac[i].next ) 
		{
			for( j=ac[i].next; j!=-1; j=ac[j].next )
	        {
				tmpdouble = eff[i][j];
				if( tmpdouble < minscore )
				{
					minscore = tmpdouble;
					im = i; jm = j;
				}
			}
		}

//		fprintf( stderr, "im=%d, jm=%d\n", im, jm );

		intpt = topol[k][0];
		prevnode = hist[im];
		if( prevnode == -1 )
		{
			*intpt++ = im;
			*intpt = -1;
		}
		else
		{
			pt1 = topol[prevnode][0];
			pt2 = topol[prevnode][1];
			if( *pt1 > *pt2 )
			{
				pt11 = pt2;
				pt22 = pt1;
			}
			else
			{
				pt11 = pt1;
				pt22 = pt2;
			}
			for( intpt2=pt11; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			for( intpt2=pt22; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			*intpt = -1;
		}

		intpt = topol[k][1];
		prevnode = hist[jm];
		if( prevnode == -1 )
		{
			*intpt++ = jm;
			*intpt = -1;
		}
		else
		{
			pt1 = topol[prevnode][0];
			pt2 = topol[prevnode][1];
			if( *pt1 > *pt2 )
			{
				pt11 = pt2;
				pt22 = pt1;
			}
			else
			{
				pt11 = pt1;
				pt22 = pt2;
			}
			for( intpt2=pt11; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			for( intpt2=pt22; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			*intpt = -1;
		}

		minscore *= 0.5;

		len[k][0] = minscore - tmptmplen[im];
		len[k][1] = minscore - tmptmplen[jm];

		tmptmplen[im] = minscore;

		hist[im] = k;

		for( i=0; i!=-1; i=ac[i].next )
        {
            if( i != im && i != jm )
            {
				if( i < im )
				{
					 miniim = i;
					 maxiim = im;
					 minijm = i;
					 maxijm = jm;
				}
				else if( i < jm )
				{
					 miniim = im;
					 maxiim = i;
					 minijm = i;
					 maxijm = jm;
				}
				else
				{
					 miniim = im;
					 maxiim = i;
					 minijm = jm;
					 maxijm = i;
				}
				eff0 = eff[miniim][maxiim];
				eff1 = eff[minijm][maxijm];
#if 0
                eff[miniim][maxiim] =
                MIN( eff0, eff1 ) * ( 1.0 - SUEFF ) +
				( eff0 + eff1 ) * 0.5 * SUEFF;
#else
                eff[miniim][maxiim] =
				(clusterfuncpt[0])( eff0, eff1 );
#endif
            }
        }
		acjmprev = ac[jm].prev; 
		acjmnext = ac[jm].next; 
		ac[acjmprev].next = acjmnext;
		if( acjmnext != -1 )
			ac[acjmnext].prev = acjmprev;

		sprintf( treetmp, "(%s:%7.5f,%s:%7.5f)", tree[im], len[k][0], tree[jm], len[k][1] );
		strcpy( tree[im], treetmp );
#if 0
        fprintf( stdout, "STEP-%03d:\n", k+1 );
		fprintf( stdout, "len0 = %f\n", len[k][0] );
        for( i=0; topol[k][0][i]>-1; i++ ) fprintf( stdout, " %03d", topol[k][0][i] );
        fprintf( stdout, "\n" );
		fprintf( stdout, "len1 = %f\n", len[k][1] );
        for( i=0; topol[k][1][i]>-1; i++ ) fprintf( stdout, " %03d", topol[k][1][i] );
        fprintf( stdout, "\n" );
#endif
    }
	fpout = fopen( "infile.tree", "w" );
	fprintf( fpout, "%s\n", treetmp );
//	fprintf( fpout, "by veryfastsupg_double_outtree\n" );
	fclose( fpout );
#if 1
	fprintf( stderr, "\n" );
	free( (void *)tmptmplen ); tmptmplen = NULL;
	free( hist ); hist = NULL;
	free( (char *)ac ); ac = NULL;
	FreeCharMtx( tree );
	free( treetmp );
	free( nametmp );
#endif
}

void veryfastsupg( int nseq, double **oeff, int ***topol, double **len )
{
    int i, j, k, miniim, maxiim, minijm, maxijm;
	int *intpt, *intpt2;
	int tmpint;
	int eff1, eff0;
	static double *tmptmplen = NULL;
	static int **eff = NULL;
    static int *hist = NULL;
	static Achain *ac = NULL;
	int minscore;
	double minscoref;
	int im = -1, jm = -1;
	int prevnode, acjmnext, acjmprev;
	int *pt1, *pt2, *pt11, *pt22;
	if( !eff )
	{
		eff = AllocateIntMtx( njob, njob );
		hist = AllocateIntVec( njob );
		tmptmplen = (double *)malloc( njob * sizeof( double ) );
		ac = (Achain *)malloc( njob * sizeof( Achain ) );
	}
	
	for( i=0; i<nseq; i++ ) 
	{
		for( j=0; j<nseq; j++ ) 
		{
			eff[i][j] = (int)( oeff[i][j] * INTMTXSCALE + 0.5 );
		}
	}

	for( i=0; i<nseq; i++ )
	{
		ac[i].next = i+1;
		ac[i].prev = i-1;
//		ac[i].curr = i;
	}
	ac[nseq-1].next = -1;

	for( i=0; i<nseq; i++ ) tmptmplen[i] = 0.0;
    for( i=0; i<nseq; i++ ) hist[i] = -1;

	fprintf( stderr, "\n" );
    for( k=0; k<nseq-1; k++ )
    {
		if( k % 10 == 0 ) fprintf( stderr, "%d / %d\r", k, nseq );

		minscore = INTMTXSCALE*4;
		for( i=0; ac[i].next!=-1; i=ac[i].next ) 
		{
			for( j=ac[i].next; j!=-1; j=ac[j].next )
	        {
				tmpint = eff[i][j];
				if( tmpint < minscore )
				{
					minscore = tmpint;
					im = i; jm = j;
				}
			}
		}
		minscoref = (double)minscore * 0.5 / ( INTMTXSCALE );

//		fprintf( stderr, "im=%d, jm=%d\n", im, jm );

#if 1
		intpt = topol[k][0];
		prevnode = hist[im];
		if( prevnode == -1 )
		{
			*intpt++ = im;
			*intpt = -1;
		}
		else
		{
			pt1 = topol[prevnode][0];
			pt2 = topol[prevnode][1];
			if( *pt1 > *pt2 )
			{
				pt11 = pt2;
				pt22 = pt1;
			}
			else
			{
				pt11 = pt1;
				pt22 = pt2;
			}
			for( intpt2=pt11; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			for( intpt2=pt22; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			*intpt = -1;
		}

		intpt = topol[k][1];
		prevnode = hist[jm];
		if( prevnode == -1 )
		{
			*intpt++ = jm;
			*intpt = -1;
		}
		else
		{
			pt1 = topol[prevnode][0];
			pt2 = topol[prevnode][1];
			if( *pt1 > *pt2 )
			{
				pt11 = pt2;
				pt22 = pt1;
			}
			else
			{
				pt11 = pt1;
				pt22 = pt2;
			}
			for( intpt2=pt11; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			for( intpt2=pt22; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			*intpt = -1;
		}
#else
		intpt = topol[k][0];
        for( i=0; i<nseq; i++ )
            if( pair[im][i] > -2 )
				*intpt++ = i;
		*intpt = -1;

		intpt = topol[k][1];
        for( i=0; i<nseq; i++ )
            if( pair[jm][i] > -2 )
				*intpt++ = i;
		*intpt = -1;
#endif

		len[k][0] = minscoref - tmptmplen[im];
		len[k][1] = minscoref - tmptmplen[jm];

		tmptmplen[im] = minscoref;

		hist[im] = k;

		for( i=0; i!=-1; i=ac[i].next )
        {
            if( i != im && i != jm )
            {
				if( i < im )
				{
					 miniim = i;
					 maxiim = im;
					 minijm = i;
					 maxijm = jm;
				}
				else if( i < jm )
				{
					 miniim = im;
					 maxiim = i;
					 minijm = i;
					 maxijm = jm;
				}
				else
				{
					 miniim = im;
					 maxiim = i;
					 minijm = jm;
					 maxijm = i;
				}
				eff0 = eff[miniim][maxiim];
				eff1 = eff[minijm][maxijm];
                eff[miniim][maxiim] =
                MIN( eff0, eff1 ) * ( 1.0 - SUEFF ) +
				( eff0 + eff1 ) * 0.5 * SUEFF;
            }
        }
		acjmprev = ac[jm].prev; 
		acjmnext = ac[jm].next; 
		ac[acjmprev].next = acjmnext;
		if( acjmnext != -1 )
			ac[acjmnext].prev = acjmprev;
#if 0
        fprintf( stdout, "STEP-%03d:\n", k+1 );
		fprintf( stdout, "len0 = %f\n", len[k][0] );
        for( i=0; topol[k][0][i]>-1; i++ ) fprintf( stdout, " %03d", topol[k][0][i] );
        fprintf( stdout, "\n" );
		fprintf( stdout, "len1 = %f\n", len[k][1] );
        for( i=0; topol[k][1][i]>-1; i++ ) fprintf( stdout, " %03d", topol[k][1][i] );
        fprintf( stdout, "\n" );
#endif
    }
#if 1
	FreeIntMtx( eff ); eff = NULL;
	free( (void *)tmptmplen ); tmptmplen = NULL;
	free( hist ); hist = NULL;
	free( (char *)ac ); ac = NULL;
#endif
}
void veryfastsupg_int( int nseq, int **oeff, int ***topol, double **len )
/* lenは、 oeffが整数。lenも実は整数。
   必要に応じて割って使う。 */
{
    int i, j, k, miniim, maxiim, minijm, maxijm;
	int *intpt, *intpt2;
	int tmpint;
	int eff1, eff0;
	static int *tmptmplen = NULL;
	static int **eff = NULL;
    static int *hist = NULL;
	static Achain *ac = NULL;
	int minscore;
	int im = -1, jm = -1;
	int prevnode, acjmnext, acjmprev;
	int *pt1, *pt2, *pt11, *pt22;


	if( !eff )
	{
		eff = AllocateIntMtx( njob, njob );
		hist = AllocateIntVec( njob );
		tmptmplen = AllocateIntVec( njob );
		ac = (Achain *)malloc( njob * sizeof( Achain ) );
	}
	
	for( i=0; i<nseq; i++ ) 
	{
		for( j=0; j<nseq; j++ ) 
		{
			eff[i][j] = ( oeff[i][j] );
		}
	}

	for( i=0; i<nseq; i++ )
	{
		ac[i].next = i+1;
		ac[i].prev = i-1;
//		ac[i].curr = i;
	}
	ac[nseq-1].next = -1;

	for( i=0; i<nseq; i++ ) tmptmplen[i] = 0;
    for( i=0; i<nseq; i++ ) hist[i] = -1;

	fprintf( stderr, "\n" );
    for( k=0; k<nseq-1; k++ )
    {
		if( k % 10 == 0 ) fprintf( stderr, "%d / %d\r", k, nseq );

		minscore = INTMTXSCALE*4;
		for( i=0; ac[i].next!=-1; i=ac[i].next ) 
		{
			for( j=ac[i].next; j!=-1; j=ac[j].next )
	        {
				tmpint = eff[i][j];
				if( tmpint < minscore )
				{
					minscore = tmpint;
					im = i; jm = j;
				}
			}
		}

//		fprintf( stderr, "im=%d, jm=%d\n", im, jm );

		intpt = topol[k][0];
		prevnode = hist[im];
		if( prevnode == -1 )
		{
			*intpt++ = im;
			*intpt = -1;
		}
		else
		{
			pt1 = topol[prevnode][0];
			pt2 = topol[prevnode][1];
			if( *pt1 > *pt2 )
			{
				pt11 = pt2;
				pt22 = pt1;
			}
			else
			{
				pt11 = pt1;
				pt22 = pt2;
			}
			for( intpt2=pt11; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			for( intpt2=pt22; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			*intpt = -1;
		}

		intpt = topol[k][1];
		prevnode = hist[jm];
		if( prevnode == -1 )
		{
			*intpt++ = jm;
			*intpt = -1;
		}
		else
		{
			pt1 = topol[prevnode][0];
			pt2 = topol[prevnode][1];
			if( *pt1 > *pt2 )
			{
				pt11 = pt2;
				pt22 = pt1;
			}
			else
			{
				pt11 = pt1;
				pt22 = pt2;
			}
			for( intpt2=pt11; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			for( intpt2=pt22; *intpt2!=-1; )
				*intpt++ = *intpt2++;
			*intpt = -1;
		}

		minscore *= 0.5;

		len[k][0] = (double)( minscore - tmptmplen[im] );
		len[k][1] = (double)( minscore - tmptmplen[jm] );

		tmptmplen[im] = minscore;

#if 0
		free( tmptmplen );
		tmptmplen = AllocateIntVec( nseq );
#endif


		hist[im] = k;

		for( i=0; i!=-1; i=ac[i].next )
        {
            if( i != im && i != jm )
            {
				if( i < im )
				{
					 miniim = i;
					 maxiim = im;
					 minijm = i;
					 maxijm = jm;
				}
				else if( i < jm )
				{
					 miniim = im;
					 maxiim = i;
					 minijm = i;
					 maxijm = jm;
				}
				else
				{
					 miniim = im;
					 maxiim = i;
					 minijm = jm;
					 maxijm = i;
				}
				eff0 = eff[miniim][maxiim];
				eff1 = eff[minijm][maxijm];
                eff[miniim][maxiim] =
				(int) ( (float)MIN( eff0, eff1 ) * ( 1.0 - SUEFF ) + (float)( eff0 + eff1 ) * 0.5 * SUEFF );
            }
        }
		acjmprev = ac[jm].prev; 
		acjmnext = ac[jm].next; 
		ac[acjmprev].next = acjmnext;
		if( acjmnext != -1 )
			ac[acjmnext].prev = acjmprev;
#if 0
        fprintf( stdout, "STEP-%03d:\n", k+1 );
		fprintf( stdout, "len0 = %f\n", len[k][0] );
        for( i=0; topol[k][0][i]>-1; i++ ) fprintf( stdout, " %03d", topol[k][0][i] );
        fprintf( stdout, "\n" );
		fprintf( stdout, "len1 = %f\n", len[k][1] );
        for( i=0; topol[k][1][i]>-1; i++ ) fprintf( stdout, " %03d", topol[k][1][i] );
        fprintf( stdout, "\n" );
#endif
    }
	FreeIntMtx( eff ); eff = NULL;
	free( (void *)tmptmplen ); tmptmplen = NULL;
	free( hist ); hist = NULL;
	free( (char *)ac ); ac = NULL;
}
void fastsupg( int nseq, double **oeff, int ***topol, double **len )
{
    int i, j, k, miniim, maxiim, minijm, maxijm;
#if 0
	double eff[nseq][nseq];
    char pair[njob][njob];
#else
	static float *tmplen;
	int *intpt;
	float tmpfloat;
	float eff1, eff0;
	static float **eff = NULL;
    static char **pair = NULL;
	static Achain *ac;
	float minscore;
	int im = -1, jm = -1;
	if( !eff )
	{
		eff = AllocateFloatMtx( njob, njob );
		pair = AllocateCharMtx( njob, njob );
		tmplen = AllocateFloatVec( njob );
		ac = (Achain *)calloc( njob, sizeof( Achain ) );
	}
#endif
	
	for( i=0; i<nseq; i++ ) 
	{
		for( j=0; j<nseq; j++ ) 
		{
			eff[i][j] = (float)oeff[i][j];
		}
	}

	for( i=0; i<nseq; i++ )
	{
		ac[i].next = i+1;
		ac[i].prev = i-1;
//		ac[i].curr = i;
	}
	ac[nseq-1].next = -1;

	for( i=0; i<nseq; i++ ) tmplen[i] = 0.0;
    for( i=0; i<nseq; i++ ) for( j=0; j<nseq; j++ ) pair[i][j] = 0;
    for( i=0; i<nseq; i++ ) pair[i][i] = 1;

	fprintf( stderr, "\n" );
    for( k=0; k<nseq-1; k++ )
    {
		if( k % 10 == 0 ) fprintf( stderr, "%d / %d\r", k, nseq );

		minscore = 9999.0;
		for( i=0; ac[i].next!=-1; i=ac[i].next ) 
//		for( i=0; i<nseq-1; i++ ) 
		{
			for( j=ac[i].next; j!=-1; j=ac[j].next )
//			for( j=i+1; j<nseq; j++ ) 
	        {
				tmpfloat = eff[i][j];
				if( tmpfloat < minscore )
				{
					minscore = tmpfloat;
					im = i; jm = j;
				}
			}
		}

//		fprintf( stderr, "im=%d, jm=%d\n", im, jm );

		intpt = topol[k][0];
        for( i=0; i<nseq; i++ )
            if( pair[im][i] > 0 )
				*intpt++ = i;
		*intpt = -1;

		intpt = topol[k][1];
        for( i=0; i<nseq; i++ )
            if( pair[jm][i] > 0 )
				*intpt++ = i;
		*intpt = -1;

		minscore /= 2.0;

		len[k][0] = (double)minscore - tmplen[im];
		len[k][1] = (double)minscore - tmplen[jm];

		tmplen[im] = (double)minscore;

        for( i=0; i<nseq; i++ ) pair[im][i] += ( pair[jm][i] > 0 );
        for( i=0; i<nseq; i++ ) pair[jm][i] = 0;

//		for( i=0; i<nseq; i++ )
		for( i=0; i!=-1; i=ac[i].next )
        {
            if( i != im && i != jm )
            {
				if( i < im )
				{
					 miniim = i;
					 maxiim = im;
					 minijm = i;
					 maxijm = jm;
				}
				else if( i < jm )
				{
					 miniim = im;
					 maxiim = i;
					 minijm = i;
					 maxijm = jm;
				}
				else
				{
					 miniim = im;
					 maxiim = i;
					 minijm = jm;
					 maxijm = i;
				}
				eff0 = eff[miniim][maxiim];
				eff1 = eff[minijm][maxijm];
                eff[miniim][maxiim] =
                MIN( eff0, eff1 ) * ( 1.0 - SUEFF ) +
				( eff0 + eff1 ) * 0.5 * SUEFF;
//        		eff[minijm][maxijm] = 9999.0;
            }
        }
		ac[ac[jm].prev].next = ac[jm].next;
		ac[ac[jm].next].prev = ac[jm].prev;
//		eff[im][jm] = 9999.0;
#if 0
        fprintf( stderr, "STEP-%03d:\n", k+1 );
		fprintf( stderr, "len0 = %f\n", len[k][0] );
        for( i=0; topol[k][0][i]>-1; i++ ) fprintf( stderr, " %03d", topol[k][0][i] );
        fprintf( stderr, "\n" );
		fprintf( stderr, "len1 = %f\n", len[k][1] );
        for( i=0; topol[k][1][i]>-1; i++ ) fprintf( stderr, " %03d", topol[k][1][i] );
        fprintf( stderr, "\n" );
#endif
    }
	fprintf( stderr, "\n" );

//	FreeFloatMtx( eff );
//	FreeCharMtx( pair );
//	FreeFloatVec( tmplen );
//	free( ac );
}
void supg( int nseq, double **oeff, int ***topol, double **len )
{
    int i, j, k, miniim, maxiim, minijm, maxijm;
#if 0
	double eff[nseq][nseq];
    char pair[njob][njob];
#else
	static float *tmplen;
	int *intpt;
	float **floatptpt;
	float *floatpt;
	float tmpfloat;
	float eff1, eff0;
	static float **eff = NULL;
    static char **pair = NULL;
	if( !eff )
	{
		eff = AllocateFloatMtx( njob, njob );
		pair = AllocateCharMtx( njob, njob );
		tmplen = AllocateFloatVec( njob );
	}
#endif

	
	for( i=0; i<nseq; i++ ) 
	{
		for( j=0; j<nseq; j++ ) 
		{
			eff[i][j] = (float)oeff[i][j];
		}
	}
	for( i=0; i<nseq; i++ ) tmplen[i] = 0.0;
    for( i=0; i<nseq; i++ ) for( j=0; j<nseq; j++ ) pair[i][j] = 0;
    for( i=0; i<nseq; i++ ) pair[i][i] = 1;

    for( k=0; k<nseq-1; k++ )
    {
        float minscore = 9999.0;
        int im = -1, jm = -1;


		floatptpt = eff;
        for( i=0; i<nseq-1; i++ ) 
		{
			floatpt = *floatptpt++ + i + 1;
			for( j=i+1; j<nseq; j++ )
	        {
				tmpfloat = *floatpt++;
				if( tmpfloat < minscore )
				{
					minscore = tmpfloat;
					im = i; jm = j;
				}
			}
		}
		intpt = topol[k][0];
        for( i=0; i<nseq; i++ )
            if( pair[im][i] > 0 )
				*intpt++ = i;
		*intpt = -1;

		intpt = topol[k][1];
        for( i=0; i<nseq; i++ )
            if( pair[jm][i] > 0 )
				*intpt++ = i;
		*intpt = -1;

		len[k][0] = (double)minscore / 2.0 - tmplen[im];
		len[k][1] = (double)minscore / 2.0 - tmplen[jm];

		tmplen[im] = (double)minscore / 2.0;

        for( i=0; i<nseq; i++ ) pair[im][i] += ( pair[jm][i] > 0 );
        for( i=0; i<nseq; i++ ) pair[jm][i] = 0;

        for( i=0; i<nseq; i++ )
        {
            if( i != im && i != jm )
            {
#if 1
				if( i < im )
				{
					 miniim = i;
					 maxiim = im;
					 minijm = i;
					 maxijm = jm;
				}
				else if( i < jm )
				{
					 miniim = im;
					 maxiim = i;
					 minijm = i;
					 maxijm = jm;
				}
				else
				{
					 miniim = im;
					 maxiim = i;
					 minijm = jm;
					 maxijm = i;
				}
#else
				miniim = MIN( i, im );
				maxiim = MAX( i, im );
				minijm = MIN( i, jm );
				maxijm = MAX( i, jm );
#endif
#if 1
				eff0 = eff[miniim][maxiim];
				eff1 = eff[minijm][maxijm];
                eff[miniim][maxiim] =
                MIN( eff0, eff1 ) * ( 1.0 - SUEFF ) +
				( eff0 + eff1 ) * 0.5 * SUEFF;
#else
                MIN( eff[miniim][maxiim], eff[minijm][maxijm] ) * ( 1.0 - SUEFF ) +
				( eff[miniim][maxiim] + eff[minijm][maxijm] ) * 0.5 * SUEFF;
#endif
                eff[minijm][maxijm] = 9999.0;
            	eff[im][jm] = 9999.0;
            }
        }
#if DEBUG
        printf( "STEP-%03d:\n", k+1 );
		printf( "len0 = %f\n", len[k][0] );
        for( i=0; topol[k][0][i]>-1; i++ ) printf( " %03d", topol[k][0][i] );
        printf( "\n" );
		printf( "len1 = %f\n", len[k][1] );
        for( i=0; topol[k][1][i]>-1; i++ ) printf( " %03d", topol[k][1][i] );
        printf( "\n" );
#endif
    }
}

void spg( int nseq, double **oeff, int ***topol, double **len )
{
    int i, j, k;
	double tmplen[M];
#if 0
	double eff[nseq][nseq];
    char pair[njob][njob];
#else
	double **eff = NULL;
    char **pair = NULL;
	if( !eff )
	{
		eff = AllocateDoubleMtx( njob, njob );
		pair = AllocateCharMtx( njob, njob );
	}
#endif
	
	for( i=0; i<nseq; i++ ) for( j=0; j<nseq; j++ ) eff[i][j] = oeff[i][j];
	for( i=0; i<nseq; i++ ) tmplen[i] = 0.0;
    for( i=0; i<nseq; i++ ) for( j=0; j<nseq; j++ ) pair[i][j] = 0;
    for( i=0; i<nseq; i++ ) pair[i][i] = 1;

    for( k=0; k<nseq-1; k++ )
    {
        float minscore = 9999.0;
        int im = -1, jm = -1;
        int count;

        for( i=0; i<nseq-1; i++ ) for( j=i+1; j<nseq; j++ )
        {
            if( eff[i][j] < minscore )
            {
                minscore = eff[i][j];
                im = i; jm = j;
            }
        }
        for( i=0, count=0; i<nseq; i++ )
            if( pair[im][i] > 0 )
            {
                topol[k][0][count] = i;
                count++;
            }
        topol[k][0][count] = -1;
        for( i=0, count=0; i<nseq; i++ )
            if( pair[jm][i] > 0 )
            {
                topol[k][1][count] = i;
                count++;
            }
        topol[k][1][count] = -1;

		len[k][0] = minscore / 2.0 - tmplen[im];
		len[k][1] = minscore / 2.0 - tmplen[jm];

		tmplen[im] = minscore / 2.0;

        for( i=0; i<nseq; i++ ) pair[im][i] += ( pair[jm][i] > 0 );
        for( i=0; i<nseq; i++ ) pair[jm][i] = 0;

        for( i=0; i<nseq; i++ )
        {
            if( i != im && i != jm )
            {
                eff[MIN(i,im)][MAX(i,im)] =
                MIN( eff[MIN(i,im)][MAX(i,im)], eff[MIN(i,jm)][MAX(i,jm)] );
                eff[MIN(i,jm)][MAX(i,jm)] = 9999.0;
            }
            eff[im][jm] = 9999.0;
        }
#if DEBUG
        printf( "STEP-%03d:\n", k+1 );
		printf( "len0 = %f\n", len[k][0] );
        for( i=0; topol[k][0][i]>-1; i++ ) printf( " %03d", topol[k][0][i] );
        printf( "\n" );
		printf( "len1 = %f\n", len[k][1] );
        for( i=0; topol[k][1][i]>-1; i++ ) printf( " %03d", topol[k][1][i] );
        printf( "\n" );
#endif
    }
}

double ipower( double x, int n )    /* n > 0  */
{
    double r;

    r = 1;
    while( n != 0 )
    {
        if( n & 1 ) r *= x;
        x *= x; n >>= 1;
    }
    return( r );
}

void countnode( int nseq, int ***topol, double **node ) /* node[j][i] != node[i][j] */
{
    int i, j, k, s1, s2;
    static double rootnode[M];

    if( nseq-2 < 0 )
	{
		fprintf( stderr, "Too few sequence for countnode: nseq = %d\n", nseq );
		exit( 1 );
    }

    for( i=0; i<nseq; i++ ) rootnode[i] = 0;
    for( i=0; i<nseq-2; i++ )
    {
        for( j=0; topol[i][0][j]>-1; j++ )
            rootnode[topol[i][0][j]]++;
        for( j=0; topol[i][1][j]>-1; j++ )
            rootnode[topol[i][1][j]]++;
        for( j=0; topol[i][0][j]>-1; j++ )
        {
            s1 = topol[i][0][j];
            for( k=0; topol[i][1][k]>-1; k++ )
            {
                s2 = topol[i][1][k];
                node[MIN(s1,s2)][MAX(s1,s2)] = rootnode[s1] + rootnode[s2] - 1;
            }
        }
    }
    for( j=0; topol[nseq-2][0][j]>-1; j++ )
    {
        s1 = topol[nseq-2][0][j];
        for( k=0; topol[nseq-2][1][k]>-1; k++ )
        {
            s2 = topol[nseq-2][1][k];
            node[MIN(s1,s2)][MAX(s1,s2)] = rootnode[s1] + rootnode[s2];
        }
    }
}

void countnode_int( int nseq, int ***topol, int **node )  /* node[i][j] == node[j][i] */
{
    int i, j, k, s1, s2;
    int rootnode[M];

    for( i=0; i<nseq; i++ ) rootnode[i] = 0;
    for( i=0; i<nseq-2; i++ )
    {
        for( j=0; topol[i][0][j]>-1; j++ )
            rootnode[topol[i][0][j]]++;
        for( j=0; topol[i][1][j]>-1; j++ )
            rootnode[topol[i][1][j]]++;
        for( j=0; topol[i][0][j]>-1; j++ )
        {
            s1 = topol[i][0][j];
            for( k=0; topol[i][1][k]>-1; k++ )
            {
                s2 = topol[i][1][k];
                node[MIN(s1,s2)][MAX(s1,s2)] = rootnode[s1] + rootnode[s2] - 1;
            }
        }
    }
    for( j=0; topol[nseq-2][0][j]>-1; j++ )
    {
        s1 = topol[nseq-2][0][j];
        for( k=0; topol[nseq-2][1][k]>-1; k++ )
        {
            s2 = topol[nseq-2][1][k];
            node[MIN(s1,s2)][MAX(s1,s2)] = rootnode[s1] + rootnode[s2];
        }
    }
	for( i=0; i<nseq-1; i++ ) for( j=i+1; j<nseq; j++ ) 
		node[j][i] = node[i][j];
#if DEBUG
	fprintf( stderr, "node[][] in countnode_int" );
	for( i=0; i<nseq; i++ ) 
	{
		for( j=0; j<nseq; j++ ) 
		{
			fprintf( stderr, "%#3d", node[i][j] );
		}
		fprintf( stderr, "\n" );
	}
#endif
}

void counteff_simple_float( int nseq, int ***topol, float **len, double *node )
{
    int i, j, s1, s2;
	double total;
	static double rootnode[M];
	static double eff[M];

#if DEBUG
	for( i=0; i<nseq; i++ ){
		fprintf( stderr, "len0 = %f\n", len[i][0] );
		fprintf( stderr, "len1 = %f\n", len[i][1] );
	}
#endif
    for( i=0; i<nseq; i++ )
	{
		rootnode[i] = 0.0;
		eff[i] = 1.0;
/*
		rootnode[i] = 1.0;
*/
	}
   	for( i=0; i<nseq-1; i++ )
   	{
       	for( j=0; (s1=topol[i][0][j]) > -1; j++ )
		{
           	rootnode[s1] += (double)len[i][0] * eff[s1];
			eff[s1] *= 0.5;
/*
           	rootnode[s1] *= 0.5;
*/
			
		}
       	for( j=0; (s2=topol[i][1][j]) > -1; j++ )
		{
           	rootnode[s2] +=  (double)len[i][1] * eff[s2];
			eff[s2] *= 0.5;
/*
           	rootnode[s2] *= 0.5;
*/
				
		}
	}
	for( i=0; i<nseq; i++ ) 
	{
#if 1 /* 97.9.29 */
		rootnode[i] += GETA3;
#endif
#if 0
		fprintf( stderr, "### rootnode for %d = %f\n", i, rootnode[i] );
#endif
	}
#if 1
	total = 0.0;
	for( i=0; i<nseq; i++ ) 
	{
		total += rootnode[i];
	}
#else
	total = 1.0;
#endif
		
	for( i=0; i<nseq; i++ ) 
	{
		node[i] = rootnode[i] / total;
	}

#if 0
	fprintf( stderr, "weight array in counteff_simple\n" );
	for( i=0; i<nseq; i++ )
		fprintf( stderr, "%f\n", node[i] );
	printf( "\n" );
	exit( 1 );
#endif
}

void counteff_simple( int nseq, int ***topol, double **len, double *node )
{
    int i, j, s1, s2;
	double total;
	static double rootnode[M];
	static double eff[M];

#if DEBUG
	for( i=0; i<nseq; i++ ){
		fprintf( stderr, "len0 = %f\n", len[i][0] );
		fprintf( stderr, "len1 = %f\n", len[i][1] );
	}
#endif
    for( i=0; i<nseq; i++ )
	{
		rootnode[i] = 0.0;
		eff[i] = 1.0;
/*
		rootnode[i] = 1.0;
*/
	}
   	for( i=0; i<nseq-1; i++ )
   	{
       	for( j=0; (s1=topol[i][0][j]) > -1; j++ )
		{
           	rootnode[s1] += len[i][0] * eff[s1];
			eff[s1] *= 0.5;
/*
           	rootnode[s1] *= 0.5;
*/
			
		}
       	for( j=0; (s2=topol[i][1][j]) > -1; j++ )
		{
           	rootnode[s2] +=  len[i][1] * eff[s2];
			eff[s2] *= 0.5;
/*
           	rootnode[s2] *= 0.5;
*/
				
		}
	}
	for( i=0; i<nseq; i++ ) 
	{
#if 1 /* 97.9.29 */
		rootnode[i] += GETA3;
#endif
#if 0
		fprintf( stderr, "### rootnode for %d = %f\n", i, rootnode[i] );
#endif
	}
#if 1
	total = 0.0;
	for( i=0; i<nseq; i++ ) 
	{
		total += rootnode[i];
	}
#else
	total = 1.0;
#endif
		
	for( i=0; i<nseq; i++ ) 
	{
		node[i] = rootnode[i] / total;
	}

#if 0
	fprintf( stderr, "weight array in counteff_simple\n" );
	for( i=0; i<nseq; i++ )
		fprintf( stderr, "%f\n", node[i] );
	printf( "\n" );
	exit( 1 );
#endif
}


void counteff( int nseq, int ***topol, double **len, double **node )
{
    int i, j, k, s1, s2;
	double rootnode[M];
	double eff[M];

	if( mix ) 
	{
		switch( weight )
		{
			case( 2 ): 
				weight = 3;
				break;
			case( 3 ): 
				weight = 2;
				break;
			default: 
				ErrorExit( "mix error" );
				break;
		}
	}

	if( weight == 2 )
	{
	    for( i=0; i<nseq; i++ ) rootnode[i] = 0;
    	for( i=0; i<nseq-2; i++ )
    	{
        	for( j=0; topol[i][0][j]>-1; j++ )
            	rootnode[topol[i][0][j]]++;
        	for( j=0; topol[i][1][j]>-1; j++ )
            	rootnode[topol[i][1][j]]++;
        	for( j=0; topol[i][0][j]>-1; j++ )
        	{
            	s1 = topol[i][0][j];
            	for( k=0; topol[i][1][k]>-1; k++ )
            	{
                	s2 = topol[i][1][k];
                	node[MIN(s1,s2)][MAX(s1,s2)] = rootnode[s1] + rootnode[s2] - 1;
            	}
        	}
    	}
    	for( j=0; topol[nseq-2][0][j]>-1; j++ )
    	{
        	s1 = topol[nseq-2][0][j];
        	for( k=0; topol[nseq-2][1][k]>-1; k++ )
        	{
            	s2 = topol[nseq-2][1][k];
            	node[MIN(s1,s2)][MAX(s1,s2)] = rootnode[s1] + rootnode[s2];
        	}
    	}
   		for( i=0; i<nseq-1; i++ ) for( j=i+1; j<nseq; j++ )
   	   		node[i][j] = ipower( 0.5, (int)node[i][j] ) + geta2;
		for( i=0; i<nseq-1; i++ ) for( j=i+1; j<nseq; j++ ) 
			node[j][i] = node[i][j];
	}

	if( weight == 3 )
	{
#if DEBUG
		for( i=0; i<nseq; i++ ){
			fprintf( stderr, "len0 = %f\n", len[i][0] );
			fprintf( stderr, "len1 = %f\n", len[i][1] );
		}
#endif
	    for( i=0; i<nseq; i++ )
		{
			rootnode[i] = 0.0;
			eff[i] = 1.0;
/*
			rootnode[i] = 1.0;
*/
		}
    	for( i=0; i<nseq-1; i++ )
    	{
        	for( j=0; (s1=topol[i][0][j]) > -1; j++ )
			{
   	        	rootnode[s1] += len[i][0] * eff[s1];
				eff[s1] *= 0.5;
/*
   	        	rootnode[s1] *= 0.5;
*/
				
			}
   	    	for( j=0; (s2=topol[i][1][j]) > -1; j++ )
			{
   	        	rootnode[s2] +=  len[i][1] * eff[s2];
				eff[s2] *= 0.5;
/*
   	        	rootnode[s2] *= 0.5;
*/
				
			}
		}
		for( i=0; i<nseq; i++ ) 
		{
#if 1 /* 97.9.29 */
			rootnode[i] += GETA3;
#endif
#if DEBUG
			fprintf( stderr, "rootnode for %d = %f\n", i, rootnode[i] );
#endif
		}
		for( i=0; i<nseq; i++ ) 
		{
			for( j=0; j<nseq; j++ ) 
				if( j != i )
					node[i][j] = (double)rootnode[i] * rootnode[j];
				else node[i][i] = rootnode[i];
		}
	}

#if 0
	printf( "weight matrix in counteff\n" );
	for( i=0; i<nseq; i++ )
	{
		for( j=0; j<nseq; j++ ) 
		{
			printf( "%f ", node[i][j] );
		}
		printf( "\n" );
	}
#endif
}

float score_calcp( char *seq1, char *seq2, int len )
{
	int k;
	int ms1, ms2;
	float tmpscore;
	int len2 = len - 2;

	tmpscore = 0.0;
	for( k=0; k<len; k++ )
	{
		ms1 = (int)seq1[k];
		ms2 = (int)seq2[k];
		if( ms1 == (int)'-' && ms2 == (int)'-' ) continue;
		tmpscore += (float)amino_dis[ms1][ms2];
	
		if( ms1 == (int)'-' ) 
		{
			tmpscore += (float)penalty;
			tmpscore += (float)amino_dis[ms1][ms2];
			while( (ms1=(int)seq1[++k]) == (int)'-' )
				tmpscore += (float)amino_dis[ms1][ms2];
			k--;
			if( k >len2 ) break;
			continue;
		}
		if( ms2 == (int)'-' )
		{
			tmpscore += (float)penalty;
			tmpscore += (float)amino_dis[ms1][ms2];
			while( (ms2=(int)seq2[++k]) == (int)'-' )
				tmpscore += (float)amino_dis[ms1][ms2];
			k--;
			if( k > len2 ) break;
			continue;
		}
	}
	return( tmpscore );
}

float score_calc1( char *seq1, char *seq2 )   /* method 1 */
{
	int k;
	float score = 0.0;
	int count = 0;
	int len = strlen( seq1 );

	for( k=0; k<len; k++ )
	{	
		if( seq1[k] != '-' && seq2[k] != '-' )
		{
			score += (float)amino_dis[(int)seq1[k]][(int)seq2[k]];
			count++;
		}
	}
	if( count ) score /= (float)count;
	else score = 1.0;
	return( score );
}

float substitution_nid( char *seq1, char *seq2 )
{
	int k;
	float s12;
	int len = strlen( seq1 );
	
	s12 = 0.0;
	for( k=0; k<len; k++ )
		if( seq1[k] != '-' && seq2[k] != '-' )
			s12 += ( seq1[k] == seq2[k] );

//	fprintf( stdout, "s12 = %f\n", s12 );
	return( s12 );
}

float substitution_score( char *seq1, char *seq2 )
{
	int k;
	float s12;
	int len = strlen( seq1 );
	
	s12 = 0.0;
	for( k=0; k<len; k++ )
		if( seq1[k] != '-' && seq2[k] != '-' )
			s12 += amino_dis[(int)seq1[k]][(int)seq2[k]];

//	fprintf( stdout, "s12 = %f\n", s12 );
	return( s12 );
}

float substitution_hosei( char *seq1, char *seq2 )   /* method 1 */
#if 0
{
	int k;
	float score = 0.0;
	int count = 0;
	int len = strlen( seq1 );

	for( k=0; k<len; k++ )
	{	
		if( seq1[k] != '-' && seq2[k] != '-' )
		{
			score += (float)( seq1[k] != seq2[k] );
			count++;
		}
	}
	if( count ) score /= (float)count;
	else score = 1.0;
	if( score < 0.95 ) score = - log( 1.0 - score );
	else score = 3.0;
	return( score );
}
#else
{
	int count = 0;
	float score;
	int iscore = 0;
	char s1, s2;

	while( (s1=*seq1++) )
	{
		s2 = *seq2++;
		if( s1 == '-' ) continue;
		if( s2 == '-' ) continue;
		iscore += ( s1 != s2 );
		count++;
	}
	if( count ) score = (float)iscore / count;
	else score = 1.0;
	if( score < 0.95 ) score = - log( 1.0 - score );
	else score = 3.0;
	return( score );
}
#endif

float substitution( char *seq1, char *seq2 )   /* method 1 */
{
	int k;
	float score = 0.0;
	int count = 0;
	int len = strlen( seq1 );

	for( k=0; k<len; k++ )
	{	
		if( seq1[k] != '-' && seq2[k] != '-' )
		{
			score += (float)( seq1[k] != seq2[k] );
			count++;
		}
	}
	if( count ) score /= (float)count;
	else score = 1.0;
	return( score );
}


void treeconstruction( char **seq, int nseq, int ***topol, double **len, double **eff )
{
    int i, j;

	if( weight > 1 )
	{
		if( utree == 0 )
		{
	    	for( i=0; i<nseq-1; i++ ) for( j=i+1; j<nseq; j++ )
   		 	{
/*
		       	 eff[i][j] = (double)score_calc1( seq[i], seq[j] );
*/
		       	 eff[i][j] = (double)substitution_hosei( seq[i], seq[j] );
 /*
				 fprintf( stderr, "%f\n", eff[i][j] );
 */
   		 	}
/*
			fprintf( stderr, "distance matrix\n" );
			for( i=0; i<nseq; i++ )
			{
				for( j=0; j<nseq; j++ ) 
				{
					fprintf( stderr, "%f ", eff[i][j] );
				}
				fprintf( stderr, "\n" );
			}
*/
/*
   			upg( nseq, eff, topol, len );
   			upg2( nseq, eff, topol, len );
*/
   			spg( nseq, eff, topol, len );
   			counteff( nseq, topol, len, eff );
		}
	}
	else
	{
		for( i=0; i<nseq; i++ ) for( j=0; j<nseq; j++ ) 
			eff[i][j] = 1.0;
	}
/*
fprintf( stderr, "weight matrix\n" );
for( i=0; i<nseq; i++ )
{
	for( j=0; j<nseq; j++ ) 
	{
		fprintf( stderr, "%f ", eff[i][j] );
	}
	fprintf( stderr, "\n" );
}
*/
}

float bscore_calc( char **seq, int s, double **eff )  /* algorithm B */
{
	int i, j, k;
	int gb1, gb2, gc1, gc2;
	int cob;
	int nglen;
    int len = strlen( seq[0] );
    long score;

	score = 0;
	nglen = 0;
	for( i=0; i<s-1; i++ ) for( j=i+1; j<s; j++ )
	{
		double efficient = eff[i][j];

		gc1 = 0;
		gc2 = 0;
		for( k=0; k<len; k++ )
		{
			gb1 = gc1;
			gb2 = gc2;

			gc1 = ( seq[i][k] == '-' );
			gc2 = ( seq[j][k] == '-' );
			
            cob = 
	               !gb1  *  gc1
 		         * !gb2  * !gc2

                 + !gb1  * !gc1 
                 * !gb2  *  gc2

                 + !gb1  *  gc1
                 *  gb2  * !gc2

                 +  gb1  * !gc1
                 * !gb2  *  gc2
      
				 + gb1   * !gc1
				 * gb2   *  gc2      *BEFF

				 + gb1   *  gc1
				 * gb2   * !gc2      *BEFF
                 ;
			score += (long)cob * penalty * efficient;
			score += (long)amino_dis[(int)seq[i][k]][(int)seq[j][k]] * efficient;
			nglen += ( !gc1 * !gc2 );
		}
	}
	return( (float)score / nglen + 400.0 * !scoremtx );
}

void AllocateTmpSeqs( char ***mseq2pt, char **mseq1pt, int locnlenmax )
{
	*mseq2pt = AllocateCharMtx( njob, locnlenmax+1 );
	*mseq1pt = AllocateCharVec( locnlenmax+1 );
}

void FreeTmpSeqs( char **mseq2, char *mseq1 )
{
	FreeCharMtx( mseq2 );
	free( (char *)mseq1 );
}

void gappick0( char *aseq, char *seq )
{
	for( ; *seq != 0; seq++ )
	{
		if( *seq != '-' )
			*aseq++ = *seq;
	}
	*aseq = 0;

}

void gappick( int nseq, int s, char **aseq, char **mseq2, 
			  double **eff, double *effarr )
{
	int i, j, count, countjob, len, allgap;
	len = strlen( aseq[0] );
	for( i=0, count=0; i<len; i++ ) 
	{
		allgap = 1;
		for( j=0; j<nseq; j++ ) if( j != s ) allgap *= ( aseq[j][i] == '-' );
        if( allgap == 0 )
		{
			for( j=0, countjob=0; j<nseq; j++ ) 
			{
				if( j != s )
				{
					mseq2[countjob][count] = aseq[j][i];
					countjob++;
				}
			}
			count++;
		}
	}
	for( i=0; i<nseq-1; i++ ) mseq2[i][count] = 0;

	for( i=0, countjob=0; i<nseq; i++ ) 
	{
		if( i != s )
		{
			effarr[countjob] = eff[s][i];
			countjob++;
		}
	}
/*
fprintf( stdout, "effarr in gappick s = %d\n", s+1 );
for( i=0; i<countjob; i++ ) 
	fprintf( stdout, " %f", effarr[i] );
printf( "\n" );
*/
}

void commongappick_record( int nseq, char **seq, int *map )
{
	int i, j, count;
	int len = strlen( seq[0] );


	for( i=0, count=0; i<=len; i++ ) 
	{
	/*
		allgap = 1;
		for( j=0; j<nseq; j++ ) 
			allgap *= ( seq[j][i] == '-' );
		if( !allgap )
	*/
		for( j=0; j<nseq; j++ )
			if( seq[j][i] != '-' ) break;
		if( j != nseq )
		{
			for( j=0; j<nseq; j++ )
			{
				seq[j][count] = seq[j][i];
			}
			map[count] = i;
			count++;
	 	}
	}
}

void commongappick( int nseq, char **seq )
{
	int i, j, count;
	int len = strlen( seq[0] );

	for( i=0, count=0; i<=len; i++ ) 
	{
	/*
		allgap = 1;
		for( j=0; j<nseq; j++ ) 
			allgap *= ( seq[j][i] == '-' );
		if( !allgap )
	*/
		for( j=0; j<nseq; j++ )
			if( seq[j][i] != '-' ) break;
		if( j != nseq )
		{
			for( j=0; j<nseq; j++ )
			{
				seq[j][count] = seq[j][i];
			}
			count++;
	 	}
	}
}
		
double score_calc0( char **seq, int s, double **eff, int ex )
{
	double tmp;

	if( scmtd == 3 ) tmp = score_calc3( seq, s, eff, ex );
	if( scmtd == 4 ) tmp = score_calc4( seq, s, eff, ex );
	if( scmtd == 5 ) tmp = score_calc5( seq, s, eff, ex );
	else             tmp = score_calc5( seq, s, eff, ex );

	return( tmp );

}

/*
float score_m_1( char **seq, int ex, double **eff )
{
	int i, j, k;
	int len = strlen( seq[0] );
	int gb1, gb2, gc1, gc2;
	int cob;
	int nglen;
	double score;

	score = 0.0;
	nglen = 0;
	for( i=0; i<njob; i++ ) 
	{
		double efficient = eff[MIN(i,ex)][MAX(i,ex)];
		if( i == ex ) continue;

		gc1 = 0; 
		gc2 = 0;
		for( k=0; k<len; k++ ) 
		{
			gb1 = gc1;
			gb2 = gc2;

			gc1 = ( seq[i][k] == '-' );
			gc2 = ( seq[ex][k] == '-' );
      
            cob = 
                   !gb1  *  gc1
                 * !gb2  * !gc2

                 + !gb1  * !gc1
                 * !gb2  *  gc2

                 + !gb1  *  gc1
                 *  gb2  * !gc2

                 +  gb1  * !gc1
                 * !gb2  *  gc2
      
                 +  gb1  * !gc1
                 *  gb2  *  gc2      *BEFF

                 +  gb1  *  gc1
                 *  gb2  * !gc2      *BEFF
                 ;
			score += (double)cob * penalty * efficient;
			score += (double)amino_dis[seq[i][k]][seq[ex][k]] * efficient;
			*
			nglen += ( !gc1 * !gc2 );
			*
			if( !gc1 && !gc2 ) fprintf( stdout, "%f\n", score );
		}
	}
	return( (float)score / nglen + 400.0 * !scoremtx );
}
*/

#if 0
void sitescore( char **seq, double **eff, char sco1[], char sco2[], char sco3[] )
{
	int i, j, k;
	int len = strlen( seq[0] );
	double tmp;
	double count;
	int ch;
	double sco[N];

	for( i=0; i<len; i++ ) 
	{
		tmp = 0.0; count = 0;
		for( j=0; j<njob-1; j++ ) for( k=j+1; k<njob; k++ ) 
		{
		/*
			if( seq[j][i] != '-' && seq[k][i] != '-' )
		*/
			{
				tmp += amino_dis[seq[j][i]][seq[k][i]] + 400 * !scoremtx;
				count++; 
			}
		}
		if( count > 0.0 ) tmp /= count;
		else( tmp = 0.0 );
		ch = (int)( tmp/100.0 - 0.000001 );
		sprintf( sco1+i, "%c", ch+0x61 );
	}
	sco1[len] = 0;

    for( i=0; i<len; i++ ) 
    {
        tmp = 0.0; count = 0;
        for( j=0; j<njob-1; j++ ) for( k=j+1; k<njob; k++ ) 
        {
		/*
            if( seq[j][i] != '-' && seq[k][i] != '-' )
		*/
            {
                tmp += eff[j][k] * ( amino_dis[seq[j][i]][seq[k][i]] + 400 * !scoremtx );
                count += eff[j][k]; 
            }
        }
		if( count > 0.0 ) tmp /= count;
		else( tmp = 0.0 );
		tmp = ( tmp - 400 * !scoremtx ) * 2;
		if( tmp < 0 ) tmp = 0;
        ch = (int)( tmp/100.0 - 0.000001 );
        sprintf( sco2+i, "%c", ch+0x61 );
		sco[i] = tmp;
    }
    sco2[len] = 0;

	for( i=WIN; i<len-WIN; i++ )
	{
		tmp = 0.0;
		for( j=i-WIN; j<=i+WIN; j++ )
		{
			tmp += sco[j];
		}
		for( j=0; j<njob; j++ ) 
		{
			if( seq[j][i] == '-' )
			{
				tmp = 0.0;
				break;
			}
		}
		tmp /= WIN * 2 + 1;
		ch = (int)( tmp/100.0 - 0.0000001 );
		sprintf( sco3+i, "%c", ch+0x61 );
	}
	for( i=0; i<WIN; i++ ) sco3[i] = '-';
	for( i=len-WIN; i<len; i++ ) sco3[i] = '-';
	sco3[len] = 0;
}
#endif

void strins( char *str1, char *str2 )
{
	char *bk;
	int len1 = strlen( str1 );
	int len2 = strlen( str2 );

	bk = str2;
	str2 += len1+len2;
	str1 += len1-1;

	while( str2 >= bk+len1 ) { *str2 = *(str2-len1); str2--;} // by D.Mathog
	while( str2 >= bk ) { *str2-- = *str1--; }
}

int isaligned( int nseq, char **seq )
{
	int i;
	int len = strlen( seq[0] );
	for( i=1; i<nseq; i++ ) 
	{
		if( strlen( seq[i] ) != len ) return( 0 );
	}
	return( 1 );
}

double score_calc_for_score( int nseq, char **seq )
{
    int i, j, k, c;
    int len = strlen( seq[0] );
    double score;
    double tmpscore;
    char *mseq1, *mseq2;

    score = 0.0;
    for( i=0; i<nseq-1; i++ )
    {
        for( j=i+1; j<nseq; j++ )
        {
            mseq1 = seq[i];
            mseq2 = seq[j];
            tmpscore = 0.0;
            c = 0;
            for( k=0; k<len; k++ )
            {
                if( mseq1[k] == '-' && mseq2[k] == '-' ) continue;
                tmpscore += amino_dis[(int)mseq1[k]][(int)mseq2[k]];
                c++;
                if( mseq1[k] == '-' )
                {
                    tmpscore += penalty - n_dis[0][24];
                    while( mseq1[++k] == '-' )
                        ;
                    k--;
                    if( k > len-2 ) break;
                    continue;
                }
                if( mseq2[k] == '-' )
                {
                    tmpscore += penalty - n_dis[0][24];
                    while( mseq2[++k] == '-' )
                        ;
                    k--;
                    if( k > len-2 ) break;
                    continue;
                }
            }
            score += (double)tmpscore / (double)c;
#if DEBUG
			printf( "tmpscore in mltaln9.c = %f\n", tmpscore );
			printf( "tmpscore / c          = %f\n", tmpscore/(double)c );
#endif
        }
    }
	fprintf( stderr, "raw score = %f\n", score );
	score /= (double)nseq * ( nseq-1.0 ) / 2.0;
	score += 400.0;
#if DEBUG
	printf( "score in mltaln9.c = %f\n", score );
#endif
    return( (double)score );
}

void floatncpy( float *vec1, float *vec2, int len )
{
	while( len-- )
		*vec1++ = *vec2++;
}

float score_calc_a( char **seq, int s, double **eff )  /* algorithm A+ */
{
	int i, j, k;
	int gb1, gb2, gc1, gc2;
	int cob;
	int nglen;
    int len = strlen( seq[0] );
    float score;

	score = 0;
	nglen = 0;
	for( i=0; i<s-1; i++ ) for( j=i+1; j<s; j++ )
	{
		double efficient = eff[i][j];

		gc1 = 0;
		gc2 = 0;
		for( k=0; k<len; k++ )
		{
			gb1 = gc1;
			gb2 = gc2;

			gc1 = ( seq[i][k] == '-' );
			gc2 = ( seq[j][k] == '-' );
			
            cob = 
	               !gb1  *  gc1
 		         * !gb2  * !gc2

                 +  gb1  * !gc1 
                 * !gb2  * !gc2

	             + !gb1  * !gc1
 		         * !gb2  *  gc2

                 + !gb1  * !gc1 
                 *  gb2  * !gc2

                 + !gb1  *  gc1
                 *  gb2  * !gc2

                 +  gb1  * !gc1
                 * !gb2  *  gc2
      
				 +  gb1  * !gc1
				 *  gb2  *  gc2

				 +  gb1  *  gc1
				 *  gb2  * !gc2
      
				 + !gb1  *  gc1
				 *  gb2  *  gc2

				 +  gb1  *  gc1
				 * !gb2  *  gc2
                 ;
			score += 0.5 * (float)cob * penalty * efficient;
			score += (float)amino_dis[(int)seq[i][k]][(int)seq[j][k]] * (float)efficient;
			nglen += ( !gc1 * !gc2 );
		}
	}
	return( (float)score / nglen + 400.0 * !scoremtx );
}


float score_calc_s( char **seq, int s, double **eff )  /* algorithm S, not used */
{
	int i, j, k;
	int gb1, gb2, gc1, gc2;
	int cob;
	int nglen;
    int len = strlen( seq[0] );
    float score;

	score = 0;
	nglen = 0;
	for( i=0; i<s-1; i++ ) for( j=i+1; j<s; j++ )
	{
		double efficient = eff[i][j];

		gc1 = 0;
		gc2 = 0;
		for( k=0; k<len; k++ )
		{
			gb1 = gc1;
			gb2 = gc2;

			gc1 = ( seq[i][k] == '-' );
			gc2 = ( seq[j][k] == '-' );
			
            cob = 
	               !gb1  *  gc1
 		         * !gb2  * !gc2

                 +  gb1  * !gc1 
                 * !gb2  * !gc2

	             + !gb1  * !gc1
 		         * !gb2  *  gc2

                 + !gb1  * !gc1 
                 *  gb2  * !gc2

                 + !gb1  *  gc1
                 *  gb2  * !gc2

                 +  gb1  * !gc1
                 * !gb2  *  gc2
      
#if 0
				 +  gb1  * !gc1
				 *  gb2  *  gc2

				 +  gb1  *  gc1
				 *  gb2  * !gc2
      
				 + !gb1  *  gc1
				 *  gb2  *  gc2

				 +  gb1  *  gc1
				 * !gb2  *  gc2
#endif
                 ;
			score += 0.5 * (float)cob * penalty * efficient;
			score += (float)amino_dis[(int)seq[i][k]][(int)seq[j][k]] * (float)efficient;
			nglen += ( !gc1 * !gc2 );
		}
	}
	return( (float)score / nglen + 400.0 );
}

double score_calc_for_score_s( int s, char **seq )  /* algorithm S */
{
	int i, j, k;
	int gb1, gb2, gc1, gc2;
	int cob;
	int nglen;
    int len = strlen( seq[0] );
    float score;

	score = 0;
	nglen = 0;
	for( i=0; i<s-1; i++ ) for( j=i+1; j<s; j++ )
	{

		gc1 = 0;
		gc2 = 0;
		for( k=0; k<len; k++ )
		{
			gb1 = gc1;
			gb2 = gc2;

			gc1 = ( seq[i][k] == '-' );
			gc2 = ( seq[j][k] == '-' );
			
            cob = 
	               !gb1  *  gc1
 		         * !gb2  * !gc2

                 +  gb1  * !gc1 
                 * !gb2  * !gc2

	             + !gb1  * !gc1
 		         * !gb2  *  gc2

                 + !gb1  * !gc1 
                 *  gb2  * !gc2

                 + !gb1  *  gc1
                 *  gb2  * !gc2

                 +  gb1  * !gc1
                 * !gb2  *  gc2
      
#if 0
				 +  gb1  * !gc1
				 *  gb2  *  gc2

				 +  gb1  *  gc1
				 *  gb2  * !gc2
      
				 + !gb1  *  gc1
				 *  gb2  *  gc2

				 +  gb1  *  gc1
				 * !gb2  *  gc2
#endif
                 ;
			score += 0.5 * (float)cob * penalty;
			score += (float)amino_dis[(int)seq[i][k]][(int)seq[j][k]];
			nglen += ( !gc1 * !gc2 );
		}
#if 0
		fprintf( stderr, "i = %d, j=%d\n", i+1, j+1 );
		fprintf( stderr, "score = %f\n", score );
#endif
	}
	return( (double)score / nglen + 400.0 );
}

double SSPscore___( int s, char **seq, int ex )  /* algorithm S */
{
	int i, j, k;
	int gb1, gb2, gc1, gc2;
	int cob;
	int nglen;
    int len = strlen( seq[0] );
    float score;

	score = 0;
	nglen = 0;
	i=ex; for( j=0; j<s; j++ )
	{

		if( j == ex ) continue;

		gc1 = 0;
		gc2 = 0;
		for( k=0; k<len; k++ )
		{
			gb1 = gc1;
			gb2 = gc2;

			gc1 = ( seq[i][k] == '-' );
			gc2 = ( seq[j][k] == '-' );
			
            cob = 
	               !gb1  *  gc1
 		         * !gb2  * !gc2

                 +  gb1  * !gc1 
                 * !gb2  * !gc2

	             + !gb1  * !gc1
 		         * !gb2  *  gc2

                 + !gb1  * !gc1 
                 *  gb2  * !gc2

                 + !gb1  *  gc1
                 *  gb2  * !gc2 * 2.0 

                 +  gb1  * !gc1
                 * !gb2  *  gc2 * 2.0 
      
#if 0
				 +  gb1  * !gc1
				 *  gb2  *  gc2

				 +  gb1  *  gc1
				 *  gb2  * !gc2
      
				 + !gb1  *  gc1
				 *  gb2  *  gc2

				 +  gb1  *  gc1
				 * !gb2  *  gc2
#endif
                 ;
			score += 0.5 * (float)cob * penalty;
			score += (float)amino_dis[(int)seq[i][k]][(int)seq[j][k]];
			nglen += ( !gc1 * !gc2 ); /* tsukawanai */
		}
#if 0
		fprintf( stderr, "i = %d, j=%d\n", i+1, j+1 );
		fprintf( stderr, "score = %f\n", score );
#endif
	}
	return( (double)score );
}

double SSPscore( int s, char **seq )  /* algorithm S */
{
	int i, j, k;
	int gb1, gb2, gc1, gc2;
	int cob;
	int nglen;
    int len = strlen( seq[0] );
    float score;

	score = 0;
	nglen = 0;
	for( i=0; i<s-1; i++ ) for( j=i+1; j<s; j++ )
	{

		gc1 = 0;
		gc2 = 0;
		for( k=0; k<len; k++ )
		{
			gb1 = gc1;
			gb2 = gc2;

			gc1 = ( seq[i][k] == '-' );
			gc2 = ( seq[j][k] == '-' );
			
            cob = 
	               !gb1  *  gc1
 		         * !gb2  * !gc2

                 +  gb1  * !gc1 
                 * !gb2  * !gc2

	             + !gb1  * !gc1
 		         * !gb2  *  gc2

                 + !gb1  * !gc1 
                 *  gb2  * !gc2

                 + !gb1  *  gc1
                 *  gb2  * !gc2

                 +  gb1  * !gc1
                 * !gb2  *  gc2
      
#if 0
				 +  gb1  * !gc1
				 *  gb2  *  gc2

				 +  gb1  *  gc1
				 *  gb2  * !gc2
      
				 + !gb1  *  gc1
				 *  gb2  *  gc2

				 +  gb1  *  gc1
				 * !gb2  *  gc2
#endif
                 ;
			score += 0.5 * (float)cob * penalty;
			score += (float)amino_dis[(int)seq[i][k]][(int)seq[j][k]];
			nglen += ( !gc1 * !gc2 ); /* tsukawanai */
		}
#if 0
		fprintf( stderr, "i = %d, j=%d\n", i+1, j+1 );
		fprintf( stderr, "score = %f\n", score );
#endif
	}
	return( (double)score );
}



double DSPscore( int s, char **seq )  /* method 3 deha nai */
{
    int i, j, k;
    double c;
    int len = strlen( seq[0] );
    double score;
    double tmpscore;
    char *mseq1, *mseq2;
#if DEBUG
	FILE *fp;
#endif

    score = 0.0;
    c = 0.0;

    for( i=0; i<s-1; i++ )
    {
        for( j=i+1; j<s; j++ )
        {
            mseq1 = seq[i];
            mseq2 = seq[j];
            tmpscore = 0.0;
            for( k=0; k<len; k++ )
            {
                if( mseq1[k] == '-' && mseq2[k] == '-' ) continue;
                tmpscore += amino_dis[(int)mseq1[k]][(int)mseq2[k]];

                if( mseq1[k] == '-' )
                {
                    tmpscore += penalty;
                    while( mseq1[++k] == '-' )
                        tmpscore += amino_dis[(int)mseq1[k]][(int)mseq2[k]];
                    k--;
                    if( k > len-2 ) break;
                    continue;
                }
                if( mseq2[k] == '-' )
                {
                    tmpscore += penalty;
                    while( mseq2[++k] == '-' )
                        tmpscore += amino_dis[(int)mseq1[k]][(int)mseq2[k]];
                    k--;
                    if( k > len-2 ) break;
                    continue;
                }
            }
            score += (double)tmpscore;
        }
    }

	return( score );
}


#define SEGMENTSIZE 150

int searchAnchors( int nseq, char **seq, Segment *seg )
{
	int i, j, k;
	int status;
	double score;
	int value = 0;
	int len;
	int length;
	static double *stra = NULL;
	static int alloclen = 0;
	double cumscore;
	static double threshold;

	len = strlen( seq[0] );
	if( alloclen < len )
	{
		if( alloclen )
		{
			FreeDoubleVec( stra );
		}
		else
		{
			threshold = (int)divThreshold / 100.0 * 600.0 * divWinSize;
		}
		stra = AllocateDoubleVec( len );
		alloclen = len;
	}

	for( i=0; i<len; i++ )
	{
#if 0
		/* make prf */
		for( j=0; j<26; j++ )
		{
			prf[j] = 0.0;
		}
		for( j=0; j<nseq; j++ ) prf[amino_n[seq[j][i]]] += 1.0;

		/* make hat */
		pre = 26;
		for( j=25; j>=0; j-- )
		{
			if( prf[j] )
			{
				hat[pre] = j;
				pre = j;
			}
		}
		hat[pre] = -1;

		/* make site score */
		stra[i] = 0.0;
		for( k=hat[26]; k!=-1; k=hat[k] ) 
			for( j=hat[26]; j!=-1; j=hat[j] ) 
				stra[i] += n_dis[k][j] * prf[k] * prf[j];
#else
		stra[i] = 0.0;
		for( k=0; k<nseq-1; k++ ) for( j=k+1; j<nseq; j++ )
			stra[i] += n_dis[(int)amino_n[(int)seq[k][i]]][(int)amino_n[(int)seq[j][i]]];
		stra[i] /= (double)nseq * ( nseq-1 ) / 2;
#endif
	}

	(seg+0)->skipForeward = 0;
	(seg+1)->skipBackward = 0;
	status = 0;
	cumscore = 0.0;
	score = 0.0;
	length = 0; /* modified at 01/09/11 */
	for( j=0; j<divWinSize; j++ ) score += stra[j];
	for( i=1; i<len-divWinSize; i++ )
	{
		score = score - stra[i-1] + stra[i+divWinSize-1];
#if DEBUG
		fprintf( stderr, "%d %f   ? %f", i, score, threshold );
		if( score > threshold ) fprintf( stderr, "YES\n" );
		else                    fprintf( stderr, "NO\n" );
#endif

		if( score > threshold )
		{
			if( !status )
			{
				status = 1;
				seg->start = i;
				length = 0;
				cumscore = 0.0;
			}
			length++;
			cumscore += score;
		}
		if( score <= threshold || length > SEGMENTSIZE )
		{
			if( status )
			{
				seg->end = i;
				seg->center = ( seg->start + seg->end + divWinSize ) / 2 ;
				seg->score = cumscore;
#if DEBUG
				fprintf( stderr, "%d-%d length = %d\n", seg->start, seg->end, length );
#endif
				if( length > SEGMENTSIZE )
				{
					(seg+0)->skipForeward = 1;
					(seg+1)->skipBackward = 1;
				}
				else
				{
					(seg+0)->skipForeward = 0;
					(seg+1)->skipBackward = 0;
				}
				length = 0;
				cumscore = 0.0;
				status = 0;
				value++;
				seg++;
				if( value > MAXSEG - 3 ) ErrorExit( "TOO MANY SEGMENTS!");
			}
		}
	}
	if( status )
	{
		seg->end = i;
		seg->center = ( seg->start + seg->end + divWinSize ) / 2 ;
		seg->score = cumscore;
#if DEBUG
fprintf( stderr, "%d-%d length = %d\n", seg->start, seg->end, length );
#endif
		value++;
	}
	return( value );
}

void dontcalcimportance( int nseq, double *eff, char **seq, LocalHom **localhom )
{
	int i, j;
	LocalHom *ptr;
	static int *nogaplen = NULL;

	if( nogaplen == NULL )
	{
		nogaplen = AllocateIntVec( nseq );
	}

	for( i=0; i<nseq; i++ )
	{
		nogaplen[i] = seqlen( seq[i] );
//		fprintf( stderr, "nogaplen[%d] = %d\n", i, nogaplen[i] );
	}

	for( i=0; i<nseq; i++ )
	{
		for( j=0; j<nseq; j++ )
		{
			for( ptr=localhom[i]+j; ptr; ptr=ptr->next )
			{
//				fprintf( stderr, "i,j=%d,%d,ptr=%p\n", i, j, ptr );
#if 1
				ptr->importance = ptr->opt / ptr->overlapaa;
				ptr->fimportance = (float)ptr->importance;
#else
				ptr->importance = ptr->opt / MIN( nogaplen[i], nogaplen[j] );
#endif
			}
		}
	}
}

void calcimportance( int nseq, double *eff, char **seq, LocalHom **localhom )
{
	int i, j, pos, len;
	static double *importance;
	double tmpdouble;
	static int *nogaplen = NULL;
	LocalHom *tmpptr;

	if( importance == NULL )
	{
		importance = AllocateDoubleVec( nlenmax );
		nogaplen = AllocateIntVec( nseq );
	}


	for( i=0; i<nseq; i++ )
	{
		nogaplen[i] = seqlen( seq[i] );
//		fprintf( stderr, "nogaplen[] = %d\n", nogaplen[i] );
	}

#if 0
	for( i=0; i<nseq; i++ ) for( j=0; j<nseq; j++ )
	{
		tmpptr = localhom[i]+j;
		fprintf( stderr, "%d-%d\n", i, j );
		do
		{
			fprintf( stderr, "reg1=%d-%d, reg2=%d-%d, opt=%f\n", tmpptr->start1, tmpptr->end1, tmpptr->start2, tmpptr->end2, tmpptr->opt );
		} while( tmpptr=tmpptr->next );
	}
#endif


	for( i=0; i<nseq; i++ )
	{
//		fprintf( stderr, "i = %d\n", i );
		for( pos=0; pos<nlenmax; pos++ )
			importance[pos] = 0.0;
		for( j=0; j<nseq; j++ )
		{
			if( i == j ) continue;
			tmpptr = localhom[i]+j;
			for( tmpptr = localhom[i]+j; tmpptr; tmpptr=tmpptr->next )
			{
				if( tmpptr->opt == -1 ) continue;
				for( pos=tmpptr->start1; pos<=tmpptr->end1; pos++ )
#if 1
					importance[pos] += eff[j];
#else
					importance[pos] += eff[j] * tmpptr->opt / MIN( nogaplen[i], nogaplen[j] );
					importance[pos] += eff[j] * tmpptr->opt / tmpptr->overlapaa;
#endif
			}
		}
#if 0
		fprintf( stderr, "position specific importance of seq %d:\n", i );
		for( pos=0; pos<nlenmax; pos++ )
			fprintf( stderr, "%d: %f\n", pos, importance[pos] );
		fprintf( stderr, "\n" );
#endif
		for( j=0; j<nseq; j++ )
		{
//			fprintf( stderr, "i=%d, j=%d\n", i, j );
			if( i == j ) continue;
			if( localhom[i][j].opt == -1.0 ) continue;
#if 1
			for( tmpptr = localhom[i]+j; tmpptr; tmpptr=tmpptr->next )
			{
				if( tmpptr->opt == -1.0 ) continue;
				tmpdouble = 0.0;
				len = 0;
				for( pos=tmpptr->start1; pos<=tmpptr->end1; pos++ )
				{
					tmpdouble += importance[pos];
					len++;
				}
				tmpdouble /= (double)len;

				tmpptr->importance = tmpdouble * tmpptr->opt;
				tmpptr->fimportance = (float)tmpptr->importance;
			}
#else
			tmpdouble = 0.0;
			len = 0;
			for( tmpptr = localhom[i]+j; tmpptr; tmpptr=tmpptr->next )
			{
				if( tmpptr->opt == -1.0 ) continue;
				for( pos=tmpptr->start1; pos<=tmpptr->end1; pos++ )
				{
					tmpdouble += importance[pos];
					len++;
				}
			}

			tmpdouble /= (double)len;

			for( tmpptr = localhom[i]+j; tmpptr; tmpptr=tmpptr->next )
			{
				if( tmpptr->opt == -1.0 ) continue;
				tmpptr->importance = tmpdouble * tmpptr->opt;
//				tmpptr->importance = tmpptr->opt / tmpptr->overlapaa; //なかったことにする
			}
#endif

//			fprintf( stderr, "importance of match between %d - %d = %f\n", i, j, tmpdouble );
		}
	}

#if 0
	fprintf( stderr, "before averaging:\n" );

	for( i=0; i<nseq; i++ ) for( j=0; j<nseq; j++ )
	{
		fprintf( stderr, "%d-%d\n", i, j );
		for( tmpptr = localhom[i]+j; tmpptr; tmpptr=tmpptr->next )
		{
			fprintf( stderr, "reg1=%d-%d, reg2=%d-%d, imp=%f -> %f opt=%f\n", tmpptr->start1, tmpptr->end1, tmpptr->start2, tmpptr->end2, tmpptr->opt / tmpptr->overlapaa, eff[i] * tmpptr->importance, tmpptr->opt );
		}
	}
#endif

#if 1
//	fprintf( stderr, "average?\n" );
	for( i=0; i<nseq-1; i++ ) for( j=i+1; j<nseq; j++ )
	{
		double imp;
		LocalHom *tmpptr1, *tmpptr2;

//		fprintf( stderr, "i=%d, j=%d\n", i, j );

		tmpptr1 = localhom[i]+j; tmpptr2 = localhom[j]+i;
		for( ; tmpptr1 && tmpptr2; tmpptr1 = tmpptr1->next, tmpptr2 = tmpptr2->next)
		{
			if( tmpptr1->opt == -1.0 || tmpptr2->opt == -1.0 ) 
			{
//				fprintf( stderr, "WARNING: i=%d, j=%d, tmpptr1->opt=%f, tmpptr2->opt=%f\n", i, j, tmpptr1->opt, tmpptr2->opt );
				continue;
			}
//			fprintf( stderr, "## importances = %f, %f\n", tmpptr1->importance, tmpptr2->importance );
			imp = 0.5 * ( tmpptr1->importance + tmpptr2->importance );
			tmpptr1->importance = tmpptr2->importance = imp;
			tmpptr1->fimportance = tmpptr2->fimportance = (float)imp;

//			fprintf( stderr, "## importance = %f\n", tmpptr1->importance );

		}

#if 1
		if( ( tmpptr1 && !tmpptr2 ) || ( !tmpptr1 && tmpptr2 ) )
		{
			fprintf( stderr, "ERROR: i=%d, j=%d\n", i, j );
			exit( 1 );
		}
#endif
	}
#endif
#if 0
	fprintf( stderr, "after averaging:\n" );

	for( i=0; i<nseq; i++ ) for( j=0; j<nseq; j++ )
	{
		fprintf( stderr, "%d-%d\n", i, j );
		for( tmpptr = localhom[i]+j; tmpptr; tmpptr=tmpptr->next )
		{
			fprintf( stderr, "reg1=%d-%d, reg2=%d-%d, imp=%f -> %f opt=%f\n", tmpptr->start1, tmpptr->end1, tmpptr->start2, tmpptr->end2, tmpptr->opt / tmpptr->overlapaa, tmpptr->importance, tmpptr->opt );
		}
	}
#endif
}


#if 0
void weightimportance( int nseq, double **eff, LocalHom **localhom )
{
	int i, j, pos, len;
	static double *importance;
	double tmpdouble;
	LocalHom *tmpptr, *tmpptr1, *tmpptr2;
	if( importance == NULL )
		importance = AllocateDoubleVec( nlenmax );


	fprintf( stderr, "effmtx = :\n" );
	for( i=0; i<nseq; i++ )
	{
		for( j=0; j<nseq; j++ )
		{
			fprintf( stderr, "%6.3f ", eff[i][j] );
		}
		fprintf( stderr, "\n" );
	}
	for( i=0; i<nseq; i++ )
	{
		for( pos=0; pos<nlenmax; pos++ )
			importance[pos] = 0.0;
		for( j=0; j<nseq; j++ )
		{

			if( i == j ) continue;
			tmpptr = localhom[i]+j;
			while( 1 )
			{
				fprintf( stderr, "i=%d, j=%d\n", i, j );
				for( pos=tmpptr->start1; pos<=tmpptr->end1; pos++ )
//					importance[pos] += eff[i][j] * tmpptr->importance;
					importance[pos] += eff[i][j] / (double)nseq * tmpptr->importance / 1.0;
				fprintf( stderr, "eff[][] = %f, localhom[i][j].importance = %f \n", eff[i][j], tmpptr->importance );
				tmpptr = tmpptr->next;
				if( tmpptr == NULL ) break;
			} 

		}
#if 0
		fprintf( stderr, "position specific importance of seq %d:\n", i );
		for( pos=0; pos<nlenmax; pos++ )
			fprintf( stderr, "%d: %f\n", pos, importance[pos] );
		fprintf( stderr, "\n" );
#endif
		for( j=0; j<nseq; j++ )
		{
			fprintf( stderr, "i=%d, j=%d\n", i, j );
			if( i == j ) continue;
			tmpptr = localhom[i]+j;
			do
			{
				tmpdouble = 0.0;
				len = 0;
				for( pos=tmpptr->start1; pos<=tmpptr->end1; pos++ )
				{
					tmpdouble += importance[pos];
					len++;
				}
				tmpdouble /= (double)len;
				tmpptr->importance = tmpdouble;
				fprintf( stderr, "importance of match between %d - %d = %f\n", i, j, tmpdouble );
				tmpptr = tmpptr->next;
			} while( tmpptr );
		}
	}
#if 1
	for( i=0; i<nseq-1; i++ ) for( j=i+1; j<nseq; j++ )
	{
		fprintf( stderr, "i = %d, j=%d\n", i, j );
		tmpptr1 = localhom[i]+j;
		tmpptr2 = localhom[j]+i;
		while( tmpptr1 && tmpptr2 )
		{
			tmpptr1->importance += tmpptr2->importance;
			tmpptr1->importance *= 0.5;
			tmpptr2->importance *= tmpptr1->importance;
			fprintf( stderr, "%d-%d: s1=%d, e1=%d, s2=%d, e2=%d, importance=%f\n", i, j, tmpptr1->start1, tmpptr1->end1, tmpptr1->start2, tmpptr1->end2, tmpptr1->importance );
			tmpptr1 = tmpptr1->next;
			tmpptr2 = tmpptr2->next;
			fprintf( stderr, "tmpptr1 = %p, tmpptr2 = %p\n", tmpptr1, tmpptr2 );
		}
	}
#endif
}

void weightimportance2( int nseq, double *eff, LocalHom **localhom )
{
	int i, j, pos, len;
	static double *wimportance;
	double tmpdouble;
	if( wimportance == NULL )
		wimportance = AllocateDoubleVec( nlenmax );


	fprintf( stderr, "effmtx = :\n" );
	for( i=0; i<nseq; i++ )
	{
		for( j=0; j<nseq; j++ )
		{
			fprintf( stderr, "%6.3f ", eff[i] * eff[j] );
		}
		fprintf( stderr, "\n" );
	}
	for( i=0; i<nseq; i++ )
	{
		fprintf( stderr, "i = %d\n", i );
		for( pos=0; pos<nlenmax; pos++ )
			wimportance[pos] = 0.0;
		for( j=0; j<nseq; j++ )
		{
			if( i == j ) continue;
			for( pos=localhom[i][j].start1; pos<=localhom[i][j].end1; pos++ )
//				wimportance[pos] += eff[i][j];
				wimportance[pos] += eff[i] * eff[j] / (double)nseq * localhom[i][j].importance / 1.0;
		}
#if 0
		fprintf( stderr, "position specific wimportance of seq %d:\n", i );
		for( pos=0; pos<nlenmax; pos++ )
			fprintf( stderr, "%d: %f\n", pos, wimportance[pos] );
		fprintf( stderr, "\n" );
#endif
		for( j=0; j<nseq; j++ )
		{
			if( i == j ) continue;
			tmpdouble = 0.0;
			len = 0;
			for( pos=localhom[i][j].start1; pos<=localhom[i][j].end1; pos++ )
			{
				tmpdouble += wimportance[pos];
				len++;
			}
			tmpdouble /= (double)len;
			localhom[i][j].wimportance = tmpdouble;
			fprintf( stderr, "wimportance of match between %d - %d = %f\n", i, j, tmpdouble );
		}
	}
#if 1
	for( i=0; i<nseq-1; i++ ) for( j=i+1; j<nseq; j++ )
	{
		localhom[i][j].wimportance += localhom[j][i].wimportance;
		localhom[i][j].wimportance = 0.5 * ( localhom[i][j].wimportance );
	}
	for( i=0; i<nseq-1; i++ ) for( j=i+1; j<nseq; j++ )
	{
		localhom[j][i].wimportance = localhom[i][j].wimportance;
	}
#endif
}

void weightimportance4( int clus1, int clus2, double *eff1, double *eff2, LocalHom ***localhom )
{
	int i, j, pos, len;
	static double *wimportance;
	LocalHom *tmpptr, *tmpptr1, *tmpptr2;
	if( wimportance == NULL )
		wimportance = AllocateDoubleVec( nlenmax );


#if 0
	fprintf( stderr, "effarr1 = :\n" );
	for( i=0; i<clus1; i++ )
		fprintf( stderr, "%6.3f\n", eff1[i] );
	fprintf( stderr, "effarr2 = :\n" );
	for( i=0; i<clus2; i++ )
		fprintf( stderr, "%6.3f\n", eff2[i] );
#endif

	for( i=0; i<clus1; i++ )
	{
		for( j=0; j<clus2; j++ )
		{
//			fprintf( stderr, "i=%d, j=%d\n", i, j );
			tmpptr = localhom[i][j];
			do
			{
				tmpptr->wimportance = tmpptr->importance * eff1[i] * eff2[j];
				tmpptr = tmpptr->next;
			} while( tmpptr );
		}
	}
}

static void	addlocalhom_e( LocalHom *localhom, int start1, int start2, int end1, int end2, double opt )
{
	LocalHom *tmpptr;
	tmpptr = localhom;

	fprintf( stderr, "adding localhom\n" );
	while( tmpptr->next )
		tmpptr = tmpptr->next;
	fprintf( stderr, "allocating localhom\n" );
	tmpptr->next = (LocalHom *)calloc( 1, sizeof( LocalHom ) );
	fprintf( stderr, "done\n" );
	tmpptr = tmpptr->next;

	tmpptr->start1 = start1;
	tmpptr->start2 = start2;
	tmpptr->end1 = end1;
	tmpptr->end2 = end2;
	tmpptr->opt = opt;

	fprintf( stderr, "start1 = %d, end1 = %d, start2 = %d, end2 = %d\n", start1, end1, start2, end2 );
}

#if 0
#endif



void extendlocalhom( int nseq, LocalHom **localhom )
{
	int i, j, k, pos0, pos1, pos2, st;
	int start1, start2, end1, end2;
	static int *tmpint1 = NULL;
	static int *tmpint2 = NULL;
	static int *tmpdouble1 = NULL;
	static int *tmpdouble2 = NULL;
	double opt;
	LocalHom *tmpptr;
	if( tmpint1 == NULL )
	{
		tmpint1 = AllocateIntVec( nlenmax );
		tmpint2 = AllocateIntVec( nlenmax );
		tmpdouble1 = AllocateIntVec( nlenmax );
		tmpdouble2 = AllocateIntVec( nlenmax );
	}


	for( k=0; k<nseq; k++ )
	{
		for( i=0; i<nseq-1; i++ ) 
		{
			if( i == k ) continue;
			for( pos0=0; pos0<nlenmax; pos0++ ) 
				tmpint1[pos0] = -1;

			tmpptr=localhom[k]+i;
			do
			{
				pos0 = tmpptr->start1;
				pos1 = tmpptr->start2;
				while( pos0<=tmpptr->end1 )
				{
					tmpint1[pos0] = pos1++;
					tmpdouble1[pos0] = tmpptr->opt;
					pos0++;
				}
			} while( tmpptr = tmpptr->next );


			for( j=i+1; j<nseq; j++ )
			{
				if( j == k ) continue;
				for( pos1=0; pos1<nlenmax; pos1++ ) tmpint2[pos1] = -1;
				tmpptr=localhom[k]+j;
				do
				{
					pos0 = tmpptr->start1;
					pos2 = tmpptr->start2;
					while( pos0<=tmpptr->end1 )
					{
						tmpint2[pos0] = pos2++;
						tmpdouble2[pos0++] = tmpptr->opt;
					}
				} while( tmpptr = tmpptr->next );

#if 0

				fprintf( stderr, "i,j=%d,%d\n", i, j );

				for( pos0=0; pos0<nlenmax; pos0++ )
					fprintf( stderr, "%d ", tmpint1[pos0] );
				fprintf( stderr, "\n" );

				for( pos0=0; pos0<nlenmax; pos0++ )
					fprintf( stderr, "%d ", tmpint2[pos0] );
				fprintf( stderr, "\n" );
#endif


				st = 0;
				for( pos0=0; pos0<nlenmax; pos0++ )
				{
//					fprintf( stderr, "pos0 = %d/%d, st = %d, tmpint1[pos0] = %d, tmpint2[pos0] = %d\n", pos0, nlenmax, st, tmpint1[pos0], tmpint2[pos0] );
					if( tmpint1[pos0] >= 0 && tmpint2[pos0] >= 0 )
					{
						if( st == 0 )
						{
							st = 1;
							start1 = tmpint1[pos0];
							start2 = tmpint2[pos0];
							opt = MIN( tmpdouble1[pos0], tmpdouble2[pos0] );
						}
						else if( tmpint1[pos0-1] != tmpint1[pos0]-1 || tmpint2[pos0-1] != tmpint2[pos0]-1 )
						{
							addlocalhom_e( localhom[i]+j, start1, start2, tmpint1[pos0-1], tmpint2[pos0-1], opt );
							addlocalhom_e( localhom[j]+i, start2, start1, tmpint2[pos0-1], tmpint1[pos0-1], opt );
							start1 = tmpint1[pos0];
							start2 = tmpint2[pos0];
							opt = MIN( tmpdouble1[pos0], tmpdouble2[pos0] );
						}
					}
					if( tmpint1[pos0] == -1 || tmpint2[pos0] == -1 )
					{
						if( st == 1 )
						{
							st = 0;
							addlocalhom_e( localhom[i]+j, start1, start2, tmpint1[pos0-1], tmpint2[pos0-1], opt );
							addlocalhom_e( localhom[j]+i, start2, start1, tmpint2[pos0-1], tmpint1[pos0-1], opt );
						}
					}
				}
			}
		}
	}
}
#endif

static void addlocalhom2_e( LocalHom *pt, LocalHom *lh, int sti, int stj, int eni, int enj, double opt, int overlp, int interm )
{
// dokka machigatteru
	if( pt != lh ) // susumeru
	{
		pt->next = (LocalHom *)calloc( 1, sizeof( LocalHom ) );
		pt = pt->next;
		pt->next = NULL;
		lh->last = pt;
	}
	else // sonomamatsukau
	{
		lh->last = pt;
	}
	lh->nokori++;
//	fprintf( stderr, "in addlocalhom2_e, pt = %p, pt->next = %p, interm=%d, sti-eni-stj-enj=%d %d %d %d\n", pt, pt->next, interm, sti, eni, stj, enj );

	pt->start1 = sti;
	pt->start2 = stj;
	pt->end1 = eni;
	pt->end2 = enj;
	pt->opt = opt;
	pt->extended = interm;
	pt->overlapaa = overlp;
#if 0
	fprintf( stderr, "i: %d-%d\n", sti, eni );
	fprintf( stderr, "j: %d-%d\n", stj, enj );
	fprintf( stderr, "opt=%f\n", opt );
	fprintf( stderr, "overlp=%d\n", overlp );
#endif
}

void extendlocalhom2( int nseq, LocalHom **localhom, double **dist )
{
	int overlp, plim;
	int i, j, k;
	int pi, pj, pk, len;
	int status, sti, stj;
	int *ipt;
	int co;
	static int *ini = NULL;
	static int *inj = NULL;
	LocalHom *pt;

	sti = 0; // by D.Mathog, a guess
	stj = 0; // by D.Mathog, a guess

	if( ini == NULL )
	{
		ini = AllocateIntVec( nlenmax+1 );
		inj = AllocateIntVec( nlenmax+1 );
	}


	for( i=0; i<nseq-1; i++ )
	{
		for( j=i+1; j<nseq; j++ )
		{
#if 0
			for( k=0; k<nseq; k++ ) sai[k] = 0;
			numint = ncons;
			while( 1 )
			{
				k = (int)( rnd() * nseq );
				if( k == i || k == j ) continue; // mou yatta nomo habuita hoga ii 
				if( numint-- == 0 ) break;
				if( sai[k] ) continue;
				sai[k] = 1;
#else
			for( k=0; k<nseq; k++ )
			{
#endif
//				fprintf( stderr, "i=%d, j=%d, k=%d, dists = %f,%f,%f thrinter=%f\n", i, j, k, dist[i][j], dist[MIN(i,k)][MAX(i,k)], dist[MIN(j,k)][MAX(j,k)], thrinter );
				if( k == i || k == j ) continue; // mou yatta nomo habuita hoga ii 
				if( dist[MIN(i,k)][MAX(i,k)] > dist[i][j] * thrinter || dist[MIN(j,k)][MAX(j,k)] > dist[i][j] * thrinter ) continue;
				ipt = ini; co = nlenmax+1;
				while( co-- ) *ipt++ = -1;
				ipt = inj; co = nlenmax+1;
				while( co-- ) *ipt++ = -1;
				overlp = 0;

				{
					for( pt=localhom[i]+k; pt; pt=pt->next )
		        	{
//						fprintf( stderr, "i=%d,k=%d,st1:st2=%d:%d,pt=%p,extended=%p\n", i, k, pt->start1, pt->start2, pt, pt->extended );
						if( pt->opt == -1 )
						{
							fprintf( stderr, "opt kainaide tbfast.c = %f\n", pt->opt );
						}
						if( pt->extended > -1 ) break;
						pi = pt->start1;
						pk = pt->start2;
						len = pt->end1 - pt->start1 + 1;
						ipt = ini + pk;
						while( len-- ) *ipt++ = pi++;
					}
				}

				{
					for( pt=localhom[j]+k; pt; pt=pt->next )
		        	{
						if( pt->opt == -1 )
						{
							fprintf( stderr, "opt kainaide tbfast.c = %f\n", pt->opt );
						}
						if( pt->extended > -1 ) break;
						pj = pt->start1;
						pk = pt->start2;
						len = pt->end1 - pt->start1 + 1;
						ipt = inj + pk;
						while( len-- ) *ipt++ = pj++;
					}
				}
#if 0
				fprintf( stderr, "i=%d,j=%d,k=%d\n", i, j, k );
				overlp = 0;
				for( pk = 0; pk < nlenmax; pk++ )
				{
					if( ini[pk] != -1 && inj[pk] != -1 ) overlp++;
					fprintf( stderr, " %d", inj[pk] );
				}
				fprintf( stderr, "\n" );

				fprintf( stderr, "i=%d,j=%d,k=%d\n", i, j, k );
				overlp = 0;
				for( pk = 0; pk < nlenmax; pk++ )
				{
					if( ini[pk] != -1 && inj[pk] != -1 ) overlp++;
					fprintf( stderr, " %d", ini[pk] );
				}
				fprintf( stderr, "\n" );
#endif
				overlp = 0;
				plim = nlenmax+1;
				for( pk = 0; pk < plim; pk++ )
					if( ini[pk] != -1 && inj[pk] != -1 ) overlp++;


				status = 0;
				plim = nlenmax+1;
				for( pk=0; pk<plim; pk++ )
				{
//					fprintf( stderr, "%d %d: %d-%d\n", i, j, ini[pk], inj[pk] );
					if( status )
					{
						if( ini[pk] == -1 || inj[pk] == -1 || ini[pk-1] != ini[pk] - 1 || inj[pk-1] != inj[pk] - 1 ) // saigonoshori
						{
							status = 0;
//							fprintf( stderr, "end here!\n" );

							pt = localhom[i][j].last;
//							fprintf( stderr, "in ex (ba), pt = %p, nokori=%d, i,j,k=%d,%d,%d\n", pt, localhom[i][j].nokori, i, j, k );
							addlocalhom2_e( pt, localhom[i]+j, sti, stj, ini[pk-1], inj[pk-1], MIN( localhom[i][k].opt, localhom[j][k].opt ) * 1.0, overlp, k );
//							fprintf( stderr, "in ex, pt = %p, pt->next = %p, pt->next->next = %p\n", pt, pt->next, pt->next->next );

							pt = localhom[j][i].last;
//							fprintf( stderr, "in ex (ba), pt = %p, pt->next = %p\n", pt, pt->next );
//							fprintf( stderr, "in ex (ba), pt = %p, pt->next = %p, k=%d\n", pt, pt->next, k );
							addlocalhom2_e( pt, localhom[j]+i, stj, sti, inj[pk-1], ini[pk-1], MIN( localhom[i][k].opt, localhom[j][k].opt ) * 1.0, overlp, k );
//							fprintf( stderr, "in ex, pt = %p, pt->next = %p, pt->next->next = %p\n", pt, pt->next, pt->next->next );
						}
					}
					if( !status ) // else deha arimasenn.
					{
						if( ini[pk] == -1 || inj[pk] == -1 ) continue;
						sti = ini[pk];
						stj = inj[pk];
//						fprintf( stderr, "start here!\n" );
						status = 1;
					}
				}
//				if( status ) fprintf( stderr, "end here\n" );

//				exit( 1 );
//					fprintf( hat3p, "%d %d %d %6.3f %d %d %d %d %p\n", i, j, tmpptr->overlapaa, tmpptr->opt, tmpptr->start1, tmpptr->end1, tmpptr->start2, tmpptr->end2, tmpptr->next ); 
			}
#if 0
			for( pt=localhomtable[i]+j; pt; pt=pt->next )
        	{
	            if( tmpptr->opt == -1.0 ) continue;
				fprintf( hat3p, "%d %d %d %6.3f %d %d %d %d %p\n", i, j, tmpptr->overlapaa, tmpptr->opt, tmpptr->start1, tmpptr->end1, tmpptr->start2, tmpptr->end2, tmpptr->next ); 
        	}
#endif
		}
	}
}

int makelocal( char *s1, char *s2, int thr )
{
	int start, maxstart, maxend;
	char *pt1, *pt2;
	double score;
	double maxscore;

	pt1 = s1;
	pt2 = s2;

	maxend = 0; // by D.Mathog, a guess

//	fprintf( stderr, "thr = %d, \ns1 = %s\ns2 = %s\n", thr, s1, s2 );
	maxscore = 0.0;
	score = 0.0;
	start = 0;
	maxstart = 0;
	while( *pt1 )
	{
//		fprintf( stderr, "*pt1 = %c*pt2 = %c\n", *pt1, *pt2 );
		if( *pt1 == '-' || *pt2 == '-' )
		{
//			fprintf( stderr, "penalty = %d\n", penalty );
			score += penalty;
			while( *pt1 == '-' || *pt2 == '-' )
			{
				pt1++; pt2++;
			}
			continue;
		}

		score += ( amino_dis[(int)*pt1++][(int)*pt2++] - thr );
//		score += ( amino_dis[(int)*pt1++][(int)*pt2++] );
		if( score > maxscore ) 
		{
//			fprintf( stderr, "score = %f\n", score );
			maxscore = score;
			maxstart = start;
//			fprintf( stderr, "## max! maxstart = %d, start = %d\n", maxstart, start );
		}
		if( score < 0.0 )
		{
//			fprintf( stderr, "## resetting, start = %d, maxstart = %d\n", start, maxstart );
			if( start == maxstart )
			{
				maxend = pt1 - s1;
//				fprintf( stderr, "maxend = %d\n", maxend );
			}
			score = 0.0;
			start = pt1 - s1;
		}
	}
	if( start == maxstart )
		maxend = pt1 - s1 - 1;

//	fprintf( stderr, "maxstart = %d, maxend = %d, maxscore = %f\n", maxstart, maxend, maxscore );
	s1[maxend+1] = 0;
	s2[maxend+1] = 0;
	return( maxstart );
}

void resetlocalhom( int nseq, LocalHom **lh )
{
	int i, j;
	LocalHom *pt;

	for( i=0; i<nseq-1; i++ ) for( j=i+1; j<nseq; j++ )
	{
		for( pt=lh[i]+j; pt; pt=pt->next )
			pt->opt = 1.0;
	}

}

void gapireru( char *res, char *ori, char *gt )
{
	char g;
	while( (g = *gt++) )
	{
		if( g == '-' )
		{
			*res++ = '-';
		}
		else
		{
			*res++ = *ori++;
		}
	}
	*res = 0;
}

void getkyokaigap( char *g, char **s, int pos, int n )
{
//	char *bk = g;
//	while( n-- ) *g++ = '-';
	while( n-- ) *g++ = (*s++)[pos];

//	fprintf( stderr, "bk = %s\n", bk );
}

void new_OpeningGapCount( float *ogcp, int clus, char **seq, double *eff, int len, char *sgappat )
#if 0
{
	int i, j, gc, gb; 
	float feff;

	
	for( i=0; i<len+1; i++ ) ogcp[i] = 0.0;
	for( j=0; j<clus; j++ ) 
	{
		feff = (float)eff[j];
		gc = ( sgappat[j] == '-' );
		for( i=0; i<len; i++ ) 
		{
			gb = gc;
			gc = ( seq[j][i] == '-' );
			if( !gb *  gc ) ogcp[i] += feff;
		}
	}
}
#else
{
	int i, j, gc, gb; 
	float feff;
	float *fpt;
	char *spt;
	
	fpt = ogcp;
	i = len;
	while( i-- ) *fpt++ = 0.0;
	for( j=0; j<clus; j++ ) 
	{
		feff = (float)eff[j];
		spt = seq[j];
		fpt = ogcp;
		gc = ( sgappat[j] == '-' );
		i = len;
		while( i-- )
		{
			gb = gc;
			gc = ( *spt++ == '-' );
			{
				if( !gb *  gc ) *fpt += feff;
				fpt++;
			}
		}
	}
}
#endif
void new_OpeningGapCount_zure( float *ogcp, int clus, char **seq, double *eff, int len, char *sgappat, char *egappat )
#if 0
{
	int i, j, gc, gb; 
	float feff;

	
	for( i=0; i<len+1; i++ ) ogcp[i] = 0.0;
	for( j=0; j<clus; j++ ) 
	{
		feff = (float)eff[j];
		gc = ( sgappat[j] == '-' );
		for( i=0; i<len; i++ ) 
		{
			gb = gc;
			gc = ( seq[j][i] == '-' );
			if( !gb *  gc ) ogcp[i] += feff;
		}
		{
			gb = gc;
			gc = ( egappat[j] == '-' );
			if( !gb *  gc ) ogcp[i] += feff;
		}
	}
}
#else
{
	int i, j, gc, gb; 
	float feff;
	float *fpt;
	char *spt;
	
	fpt = ogcp;
	i = len+2;
	while( i-- ) *fpt++ = 0.0;
	for( j=0; j<clus; j++ ) 
	{
		feff = (float)eff[j];
		spt = seq[j];
		fpt = ogcp;
		gc = ( sgappat[j] == '-' );
		i = len;
		while( i-- )
		{
			gb = gc;
			gc = ( *spt++ == '-' );
			{
				if( !gb *  gc ) *fpt += feff;
				fpt++;
			}
		}
		{
			gb = gc;
			gc = ( egappat[j] == '-' );
			if( !gb *  gc ) *fpt += feff;
		}
	}
}
#endif

void new_FinalGapCount_zure( float *fgcp, int clus, char **seq, double *eff, int len, char *sgappat, char *egappat )
#if 0
{
	int i, j, gc, gb; 
	float feff;
	
	for( i=0; i<len+1; i++ ) fgcp[i] = 0.0;
	for( j=0; j<clus; j++ ) 
	{
		feff = (float)eff[j];
		gc = ( sgappat[j] == '-' );
		for( i=0; i<len; i++ ) 
		{
			gb = gc;
			gc = ( seq[j][i] == '-' );
			{
				if( gb * !gc ) fgcp[i] += feff;
			}
		}
		{
			gb = gc;
			gc = ( egappat[j] == '-' );
			{
				if( gb * !gc ) fgcp[len] += feff;
			}
		}
	}
}
#else
{
	int i, j, gc, gb; 
	float feff;
	float *fpt;
	char *spt;
	
	fpt = fgcp;
	i = len+2;
	while( i-- ) *fpt++ = 0.0;
	for( j=0; j<clus; j++ ) 
	{
		feff = (float)eff[j];
		fpt = fgcp;
		spt = seq[j];
		gc = ( sgappat[j] == '-' );
		i = len;
		while( i-- )
		{
			gb = gc;
			gc = ( *spt++ == '-' );
			{
				if( gb * !gc ) *fpt += feff;
				fpt++;
			}
		}
		{
			gb = gc;
			gc = ( egappat[j] == '-' );
			{
				if( gb * !gc ) *fpt += feff;
			}
		}
	}
}
#endif
void new_FinalGapCount( float *fgcp, int clus, char **seq, double *eff, int len, char *egappat )
#if 0
{
	int i, j, gc, gb; 
	float feff;
	
	for( i=0; i<len; i++ ) fgcp[i] = 0.0;
	for( j=0; j<clus; j++ ) 
	{
		feff = (float)eff[j];
		gc = ( seq[j][0] == '-' );
		for( i=1; i<len; i++ ) 
		{
			gb = gc;
			gc = ( seq[j][i] == '-' );
			{
				if( gb * !gc ) fgcp[i-1] += feff;
			}
		}
		{
			gb = gc;
			gc = ( egappat[j] == '-' );
			{
				if( gb * !gc ) fgcp[len-1] += feff;
			}
		}
	}
}
#else
{
	int i, j, gc, gb; 
	float feff;
	float *fpt;
	char *spt;
	
	fpt = fgcp;
	i = len;
	while( i-- ) *fpt++ = 0.0;
	for( j=0; j<clus; j++ ) 
	{
		feff = (float)eff[j];
		fpt = fgcp;
		spt = seq[j];
		gc = ( *spt == '-' );
		i = len;
		while( i-- )
		{
			gb = gc;
			gc = ( *++spt == '-' );
			{
				if( gb * !gc ) *fpt += feff;
				fpt++;
			}
		}
		{
			gb = gc;
			gc = ( egappat[j] == '-' );
			{
				if( gb * !gc ) *fpt += feff;
			}
		}
	}
}
#endif

void st_OpeningGapCount( float *ogcp, int clus, char **seq, double *eff, int len )
{
	int i, j, gc, gb; 
	float feff;
	float *fpt;
	char *spt;
	
	fpt = ogcp;
	i = len;
	while( i-- ) *fpt++ = 0.0;
	for( j=0; j<clus; j++ ) 
	{
		feff = (float)eff[j];
		spt = seq[j];
		fpt = ogcp;
		gc = 0;
//		gc = 1;
		i = len;
		while( i-- )
		{
			gb = gc;
			gc = ( *spt++ == '-' );
			{
				if( !gb *  gc ) *fpt += feff;
				fpt++;
			}
		}
	}
	ogcp[len] = 0.0;
}

void st_FinalGapCount_zure( float *fgcp, int clus, char **seq, double *eff, int len )
{
	int i, j, gc, gb; 
	float feff;
	float *fpt;
	char *spt;
	
	fpt = fgcp;
	i = len+1;
	while( i-- ) *fpt++ = 0.0;
	for( j=0; j<clus; j++ ) 
	{
		feff = (float)eff[j];
		fpt = fgcp+1;
		spt = seq[j];
		gc = ( *spt == '-' );
		i = len;
//		for( i=1; i<len; i++ ) 
		while( i-- )
		{
			gb = gc;
			gc = ( *++spt == '-' );
			{
				if( gb * !gc ) *fpt += feff;
				fpt++;
			}
		}
		{
			gb = gc;
			gc = 0;
//			gc = 1;
			{
				if( gb * !gc ) *fpt += feff;
			}
		}
	}
}

void st_FinalGapCount( float *fgcp, int clus, char **seq, double *eff, int len )
{
	int i, j, gc, gb; 
	float feff;
	float *fpt;
	char *spt;
	
	fpt = fgcp;
	i = len;
	while( i-- ) *fpt++ = 0.0;
	for( j=0; j<clus; j++ ) 
	{
		feff = (float)eff[j];
		fpt = fgcp;
		spt = seq[j];
		gc = ( *spt == '-' );
		i = len;
//		for( i=1; i<len; i++ ) 
		while( i-- )
		{
			gb = gc;
			gc = ( *++spt == '-' );
			{
				if( gb * !gc ) *fpt += feff;
				fpt++;
			}
		}
		{
			gb = gc;
			gc = 0;
//			gc = 1;
			{
				if( gb * !gc ) *fpt += feff;
			}
		}
	}
}

void getGapPattern( float *fgcp, int clus, char **seq, double *eff, int len, char *xxx )
{
	int i, j, gc, gb; 
	float feff;
	float *fpt;
	char *spt;
	
	fpt = fgcp;
	i = len+1;
	while( i-- ) *fpt++ = 0.0;
	for( j=0; j<clus; j++ ) 
	{
		feff = (float)eff[j];
		fpt = fgcp;
		spt = seq[j];
		gc = ( *spt == '-' );
		i = len+1;
		while( i-- )
		{
			gb = gc;
			gc = ( *++spt == '-' );
			{
				if( gb * !gc ) *fpt += feff;
				fpt++;
			}
		}
#if 0
		{
			gb = gc;
			gc = ( egappat[j] == '-' );
			{
				if( gb * !gc ) *fpt += feff;
			}
		}
#endif
	}
	for( j=0; j<len; j++ )
	{
		fprintf( stderr, "%d, %f\n", j, fgcp[j] );
	}
}

void getdigapfreq_st( float *freq, int clus, char **seq, double *eff, int len )
{
	int i, j;
	float feff;
	for( i=0; i<len+1; i++ ) freq[i] = 0.0;
	for( i=0; i<clus; i++ )
	{
		feff = eff[i];
		if( 0 && seq[i][0] == '-' ) // machigai kamo
			freq[0] += feff;
		for( j=1; j<len; j++ ) 
		{
			if( seq[i][j] == '-' && seq[i][j-1] == '-' )
				freq[j] += feff;
		}
		if( 0 && seq[i][len-1] == '-' )
			freq[len] += feff;
	}
//	fprintf( stderr, "\ndigapf = \n" );
//	for( i=0; i<len+1; i++ ) fprintf( stderr, "%5.3f ", freq[i] );
}

void getdiaminofreq_x( float *freq, int clus, char **seq, double *eff, int len )
{
	int i, j;
	float feff;
	for( i=0; i<len+2; i++ ) freq[i] = 0.0;
	for( i=0; i<clus; i++ )
	{
		feff = eff[i];
		if( seq[i][0] != '-' ) // tadashii
			freq[0] += feff;
		for( j=1; j<len; j++ ) 
		{
			if( seq[i][j] != '-' && seq[i][j-1] != '-' )
				freq[j] += feff;
		}
		if( 1 && seq[i][len-1] != '-' ) // xxx wo tsukawanaitoki [len-1] nomi
			freq[len] += feff;
	}
//	fprintf( stderr, "\ndiaaf = \n" );
//	for( i=0; i<len+1; i++ ) fprintf( stderr, "%5.3f ", freq[i] );
}

void getdiaminofreq_st( float *freq, int clus, char **seq, double *eff, int len )
{
	int i, j;
	float feff;
	for( i=0; i<len+1; i++ ) freq[i] = 0.0;
	for( i=0; i<clus; i++ )
	{
		feff = eff[i];
		if( seq[i][0] != '-' )
			freq[0] += feff;
		for( j=1; j<len; j++ ) 
		{
			if( seq[i][j] != '-' && seq[i][j-1] != '-' )
				freq[j] += feff;
		}
//		if( 1 && seq[i][len-1] != '-' ) // xxx wo tsukawanaitoki [len-1] nomi
			freq[len] += feff;
	}
//	fprintf( stderr, "\ndiaaf = \n" );
//	for( i=0; i<len+1; i++ ) fprintf( stderr, "%5.3f ", freq[i] );
}

void getdigapfreq_part( float *freq, int clus, char **seq, double *eff, int len, char *sgappat, char *egappat )
{
	int i, j;
	float feff;
	for( i=0; i<len+2; i++ ) freq[i] = 0.0;
	for( i=0; i<clus; i++ )
	{
		feff = eff[i];
//		if( seq[i][0] == '-' )
		if( seq[i][0] == '-' && sgappat[i] == '-' )
			freq[0] += feff;
		for( j=1; j<len; j++ ) 
		{
			if( seq[i][j] == '-' && seq[i][j-1] == '-' )
				freq[j] += feff;
		}
//		if( seq[i][len] == '-' && seq[i][len-1] == '-' ) // xxx wo tsukawanaitoki arienai
		if( egappat[i] == '-' && seq[i][len-1] == '-' )
			freq[len] += feff;
	}
//	fprintf( stderr, "\ndigapf = \n" );
//	for( i=0; i<len+1; i++ ) fprintf( stderr, "%5.3f ", freq[i] );
}

void getdiaminofreq_part( float *freq, int clus, char **seq, double *eff, int len, char *sgappat, char *egappat )
{
	int i, j;
	float feff;
	for( i=0; i<len+2; i++ ) freq[i] = 0.0;
	for( i=0; i<clus; i++ )
	{
		feff = eff[i];
		if( seq[i][0] != '-' && sgappat[i] != '-' )
			freq[0] += feff;
		for( j=1; j<len; j++ ) 
		{
			if( seq[i][j] != '-' && seq[i][j-1] != '-' )
				freq[j] += feff;
		}
//		if( 1 && seq[i][len-1] != '-' ) // xxx wo tsukawanaitoki [len-1] nomi
		if( egappat[i] != '-'  && seq[i][len-1] != '-' ) // xxx wo tsukawanaitoki [len-1] nomi
			freq[len] += feff;
	}
//	fprintf( stderr, "\ndiaaf = \n" );
//	for( i=0; i<len+1; i++ ) fprintf( stderr, "%5.3f ", freq[i] );
}

void getgapfreq_zure_part( float *freq, int clus, char **seq, double *eff, int len, char *sgap )
{
	int i, j;
	float feff;
	for( i=0; i<len+2; i++ ) freq[i] = 0.0;
	for( i=0; i<clus; i++ )
	{
		feff = eff[i];
		if( sgap[i] == '-' )
			freq[0] += feff;
		for( j=0; j<len; j++ ) 
		{
			if( seq[i][j] == '-' )
				freq[j+1] += feff;
		}
//		if( egap[i] == '-' )
//			freq[len+1] += feff;
	}
//	fprintf( stderr, "\ngapf = \n" );
//	for( i=0; i<len+1; i++ ) fprintf( stderr, "%5.3f ", freq[i] );
}

void getgapfreq_zure( float *freq, int clus, char **seq, double *eff, int len )
{
	int i, j;
	float feff;
	for( i=0; i<len+1; i++ ) freq[i] = 0.0;
	for( i=0; i<clus; i++ )
	{
		feff = eff[i];
		for( j=0; j<len; j++ ) 
		{
			if( seq[i][j] == '-' )
				freq[j+1] += feff;
		}
	}
	freq[len+1] = 0.0;
//	fprintf( stderr, "\ngapf = \n" );
//	for( i=0; i<len+1; i++ ) fprintf( stderr, "%5.3f ", freq[i] );
}

void getgapfreq( float *freq, int clus, char **seq, double *eff, int len )
{
	int i, j;
	float feff;
	for( i=0; i<len+1; i++ ) freq[i] = 0.0;
	for( i=0; i<clus; i++ )
	{
		feff = eff[i];
		for( j=0; j<len; j++ ) 
		{
			if( seq[i][j] == '-' )
				freq[j] += feff;
		}
	}
	freq[len] = 0.0;
//	fprintf( stderr, "\ngapf = \n" );
//	for( i=0; i<len+1; i++ ) fprintf( stderr, "%5.3f ", freq[i] );
}

void st_getGapPattern( Gappat **pat, int clus, char **seq, double *eff, int len )
{
	int i, j, k, gb, gc; 
	int known;
	float feff;
	Gappat **fpt;
	char *spt;
	int gaplen;

	fpt = pat;
	i = len+1;
	while( i-- ) 
	{
		if( *fpt ) free( *fpt );
		*fpt++ = NULL;
	}

	for( j=0; j<clus; j++ ) 
	{
//		fprintf( stderr, "seq[%d] = %s\n", j, seq[j] );
		feff = (float)eff[j];

		fpt = pat;
		*fpt = NULL; // Falign.c kara yobareru tokiha chigau.
		spt = seq[j];
		gc = 0;
		gaplen = 0;

		for( i=0; i<len+1; i++ ) 
//		while( i-- )
		{
//			fprintf( stderr, "i=%d, gaplen = %d\n", i, gaplen );
			gb = gc;
			gc = ( i != len && *spt++ == '-' );
			if( gc ) 
				gaplen++;
			else
			{
				if( gb && gaplen )
				{
					k = 1;
					known = 0;
					if( *fpt ) for( ; (*fpt)[k].len != -1; k++ )
					{
						if( (*fpt)[k].len == gaplen ) 
						{
//							fprintf( stderr, "known\n" );
							known = 1;
							break;
						}
					}

					if( known == 0 )
					{
						*fpt = (Gappat *)realloc( *fpt, (k+3) *  sizeof( Gappat ) );  // mae1 (total), ato2 (len0), term
						if( !*fpt )
						{
							fprintf( stderr, "Cannot allocate gappattern!'n" );
							fprintf( stderr, "Use an approximate method, with the --mafft5 option.\n" );
							exit( 1 );
						}
						(*fpt)[k].freq = 0.0;
						(*fpt)[k].len = gaplen;
						(*fpt)[k+1].len = -1;
						(*fpt)[k+1].freq = 0.0; // iranai
//						fprintf( stderr, "gaplen=%d, Unknown, %f\n", gaplen, (*fpt)[k].freq );
					}

//					fprintf( stderr, "adding pos %d, len=%d, k=%d, freq=%f->", i, gaplen, k, (*fpt)[k].freq );
					(*fpt)[k].freq += feff;
//					fprintf( stderr, "%f\n", (*fpt)[k].freq );
					gaplen = 0;
				}
			}
			fpt++;
		}
	}
#if 1
	for( j=0; j<len+1; j++ )
	{
		if( pat[j] )
		{
//			fprintf( stderr, "j=%d\n", j );
//			for( i=1; pat[j][i].len!=-1; i++ )
//				fprintf( stderr, "pos=%d, i=%d, len=%d, freq=%f\n", j, i, pat[j][i].len, pat[j][i].freq );

			pat[j][0].len = 0; // iminashi
			pat[j][0].freq = 0.0;
			for( i=1; pat[j][i].len!=-1;i++ )
			{
				pat[j][0].freq += pat[j][i].freq;
//				fprintf( stderr, "totaling, i=%d, result = %f\n", i, pat[j][0].freq );
			}
//			fprintf( stderr, "totaled, result = %f\n", pat[j][0].freq );

			pat[j][i].freq = 1.0 - pat[j][0].freq;
			pat[j][i].len = 0; // imiari
			pat[j][i+1].len = -1; 
		}
		else
		{
			pat[j] = (Gappat *)calloc( 3, sizeof( Gappat ) );
			pat[j][0].freq = 0.0;
			pat[j][0].len = 0; // iminashi

			pat[j][1].freq = 1.0 - pat[j][0].freq;
			pat[j][1].len = 0; // imiari
			pat[j][2].len = -1; 
		}
	}
#endif
}

static void commongappickpair( char *r1, char *r2, char *i1, char *i2 )
{
//	strcpy( r1, i1 );
//	strcpy( r2, i2 );
//	return; // not SP
	while( *i1 )
	{
		if( *i1 == '-' && *i2 == '-' ) 
		{
			i1++;
			i2++;
		}
		else
		{
			*r1++ = *i1++;
			*r2++ = *i2++;
		}
	}
	*r1 = 0;
	*r2 = 0;
}

float naiveRpairscore( int n1, int n2, char **seq1, char **seq2, double *eff1, double *eff2, int penal )
{
//	return( 0 );
	int i, j;
	float val;
	float  valf;
	int  pv;
	double deff;
	char *p1, *p2, *p1p, *p2p;
	val = 0.0;
	for( i=0; i<n1; i++ ) for( j=0; j<n2; j++ )
	{
		deff = eff1[i] * eff2[j];
//		fprintf( stderr, "feff %d-%d = %f\n", i, j, feff );
//		fprintf( stderr, "i1 = %s\n", seq1[i] );
//		fprintf( stderr, "i2 = %s\n", seq2[j] );
//		fprintf( stderr, "s1 = %s\n", s1 );
//		fprintf( stderr, "s2 = %s\n", s2 );
//		fprintf( stderr, "penal = %d\n", penal );

		valf = 0;
		p1 = seq1[i]; p2 = seq2[j];
		pv = 0;
		if( *p1 == '-' && *p2 != '-' )
			pv = penal;
		if( *p1 != '-' && *p2 == '-' )
			pv = penal;
//		if( pv ) fprintf( stderr, "Penal!, %f, %d-%d, pos1,pos2=%d,%d\n", pv * deff * 0.5,  i, j, p1-seq1[i], p2-seq2[j] );
		p1p = p1; p2p = p2;
		valf += (float)amino_dis[(int)*p1++][(int)*p2++] + 0.5 * pv;
		while( *p1p )
		{
			pv = 0;
			if( *p1p != '-' && *p2p != '-' )
			{
				if( *p1 == '-' && *p2 != '-' )
					pv = penal;
				if( *p1 != '-' && *p2 == '-' )
					pv = penal;
				if( *p1 != '-' && *p2 != '-' )
					;
				if( *p1 == '-' && *p2 == '-' )
					;
			}
			if( *p1p == '-' && *p2p == '-' )
			{
				if( *p1 == '-' && *p2 != '-' )
					pv = penal;
//					;
				if( *p1 != '-' && *p2 == '-' )
					pv = penal;
//					;
				if( *p1 != '-' && *p2 != '-' )
					;
				if( *p1 == '-' && *p2 == '-' )
					;
			}
			if( *p1p != '-' && *p2p == '-' )
			{
				if( *p1 == '-' && *p2 != '-' )
					pv = penal * 2; // ??
//					;
				if( *p1 != '-' && *p2 == '-' )
					;
				if( *p1 != '-' && *p2 != '-' )
					pv = penal;
//					;
				if( *p1 == '-' && *p2 == '-' )
					pv = penal;
//					;
			}
			if( *p1p == '-' && *p2p != '-' )
			{
				if( *p1 == '-' && *p2 != '-' )
					;
				if( *p1 != '-' && *p2 == '-' )
					pv = penal * 2; // ??
//					;
				if( *p1 != '-' && *p2 != '-' )
					pv = penal;
//					;
				if( *p1 == '-' && *p2 == '-' )
					pv = penal;
//					;
			}
//			fprintf( stderr, "adding %c-%c, %d\n", *p1, *p2, amino_dis[*p1][*p2] );
//			if( pv ) fprintf( stderr, "Penal!, %f, %d-%d, pos1,pos2=%d,%d\n", pv * deff * 0.5,  i, j, p1-seq1[i], p2-seq2[j] );
			valf += amino_dis[(int)*p1++][(int)*p2++] + 0.5 * pv;
			p1p++; p2p++;
		}
//		fprintf( stderr, "valf = %d\n", valf );
		val += deff * ( valf );
	}
	fprintf( stderr, "val = %f\n", val );
	return( val );
//	exit( 1 );
}
float naiveQpairscore( int n1, int n2, char **seq1, char **seq2, double *eff1, double *eff2, int penal )
{
	int i, j;
	float val;
	float  valf;
	int  pv;
	double deff;
	char *p1, *p2, *p1p, *p2p;
	return( 0 );
	val = 0.0;
	for( i=0; i<n1; i++ ) for( j=0; j<n2; j++ )
	{
		deff = eff1[i] * eff2[j];
//		fprintf( stderr, "feff %d-%d = %f\n", i, j, feff );
//		fprintf( stderr, "i1 = %s\n", seq1[i] );
//		fprintf( stderr, "i2 = %s\n", seq2[j] );
//		fprintf( stderr, "s1 = %s\n", s1 );
//		fprintf( stderr, "s2 = %s\n", s2 );
//		fprintf( stderr, "penal = %d\n", penal );

		valf = 0;
		p1 = seq1[i]; p2 = seq2[j];
		pv = 0;
		if( *p1 == '-' && *p2 != '-' )
			pv = penal;
		if( *p1 != '-' && *p2 == '-' )
			pv = penal;
//		if( pv ) fprintf( stderr, "Penal!, %f, %d-%d, pos1,pos2=%d,%d\n", pv * deff * 0.5,  i, j, p1-seq1[i], p2-seq2[j] );
		p1p = p1; p2p = p2;
		valf += (float)amino_dis[(int)*p1++][(int)*p2++] + 0.5 * pv;
		while( *p1p )
		{
			pv = 0;
			if( *p1p != '-' && *p2p != '-' )
			{
				if( *p1 == '-' && *p2 != '-' )
					pv = penal;
				if( *p1 != '-' && *p2 == '-' )
					pv = penal;
				if( *p1 != '-' && *p2 != '-' )
					;
				if( *p1 == '-' && *p2 == '-' )
					;
			}
			if( *p1p == '-' && *p2p == '-' )
			{
				if( *p1 == '-' && *p2 != '-' )
//					pv = penal;
					;
				if( *p1 != '-' && *p2 == '-' )
//					pv = penal;
					;
				if( *p1 != '-' && *p2 != '-' )
					;
				if( *p1 == '-' && *p2 == '-' )
					;
			}
			if( *p1p != '-' && *p2p == '-' )
			{
				if( *p1 == '-' && *p2 != '-' )
					pv = penal * 2; // ??
//					;
				if( *p1 != '-' && *p2 == '-' )
					;
				if( *p1 != '-' && *p2 != '-' )
					pv = penal;
//					;
				if( *p1 == '-' && *p2 == '-' )
//					pv = penal;
					;
			}
			if( *p1p == '-' && *p2p != '-' )
			{
				if( *p1 == '-' && *p2 != '-' )
					;
				if( *p1 != '-' && *p2 == '-' )
					pv = penal * 2; // ??
//					;
				if( *p1 != '-' && *p2 != '-' )
					pv = penal;
//					;
				if( *p1 == '-' && *p2 == '-' )
//					pv = penal;
					;
			}
//			fprintf( stderr, "adding %c-%c, %d\n", *p1, *p2, amino_dis[*p1][*p2] );
//			if( pv ) fprintf( stderr, "Penal!, %f, %d-%d, pos1,pos2=%d,%d\n", pv * deff * 0.5,  i, j, p1-seq1[i], p2-seq2[j] );
			valf += amino_dis[(int)*p1++][(int)*p2++] + 0.5 * pv;
			p1p++; p2p++;
		}
//		fprintf( stderr, "valf = %d\n", valf );
		val += deff * ( valf );
	}
	fprintf( stderr, "val = %f\n", val );
	return( val );
//	exit( 1 );
}
float naiveHpairscore( int n1, int n2, char **seq1, char **seq2, double *eff1, double *eff2, int penal )
{
	int i, j;
	float val;
	float  valf;
	int  pv;
//	float feff = 0.0; // by D.Mathog, a guess
	double deff;
	char *p1, *p2, *p1p, *p2p;
	val = 0.0;
	for( i=0; i<n1; i++ ) for( j=0; j<n2; j++ )
	{
		deff = eff1[i] * eff2[j];
//		fprintf( stderr, "i1 = %s\n", seq1[i] );
//		fprintf( stderr, "i2 = %s\n", seq2[j] );
//		fprintf( stderr, "s1 = %s\n", s1 );
//		fprintf( stderr, "s2 = %s\n", s2 );
//		fprintf( stderr, "penal = %d\n", penal );

		valf = 0;
		p1 = seq1[i]; p2 = seq2[j];
		pv = 0;
		if( *p1 == '-' && *p2 != '-' )
			pv = penal;
		if( *p1 != '-' && *p2 == '-' )
			pv = penal;
		if( pv ) fprintf( stderr, "Penal!, %f, %d-%d, pos1,pos2=%d,%d\n", pv * deff * 0.5,  i, j, (int)(p1-seq1[i]), (int)(p2-seq2[j]) );
		p1p = p1; p2p = p2;
		valf += (float)amino_dis[(int)*p1++][(int)*p2++] + 0.5 * pv;
		while( *p1p )
		{
			pv = 0;
			if( *p1p != '-' && *p2p != '-' )
			{
				if( *p1 == '-' && *p2 != '-' )
					pv = penal;
				if( *p1 != '-' && *p2 == '-' )
					pv = penal;
				if( *p1 != '-' && *p2 != '-' )
					;
				if( *p1 == '-' && *p2 == '-' )
					;
			}
			if( *p1p == '-' && *p2p == '-' )
			{
				if( *p1 == '-' && *p2 != '-' )
//					pv = penal;
					;
				if( *p1 != '-' && *p2 == '-' )
//					pv = penal;
					;
				if( *p1 != '-' && *p2 != '-' )
					;
				if( *p1 == '-' && *p2 == '-' )
					;
			}
			if( *p1p != '-' && *p2p == '-' )
			{
				if( *p1 == '-' && *p2 != '-' )
//					pv = penal;
					;
				if( *p1 != '-' && *p2 == '-' )
					;
				if( *p1 != '-' && *p2 != '-' )
					pv = penal;
				if( *p1 == '-' && *p2 == '-' )
//					pv = penal;
					;
			}
			if( *p1p == '-' && *p2p != '-' )
			{
				if( *p1 == '-' && *p2 != '-' )
					;
				if( *p1 != '-' && *p2 == '-' )
//					pv = penal;
					;
				if( *p1 != '-' && *p2 != '-' )
					pv = penal;
				if( *p1 == '-' && *p2 == '-' )
//					pv = penal;
					;
			}
//			fprintf( stderr, "adding %c-%c, %d\n", *p1, *p2, amino_dis[*p1][*p2] );
//			if( pv ) fprintf( stderr, "Penal!, %f, %d-%d, pos1,pos2=%d,%d\n", pv * deff * 0.5,  i, j, p1-seq1[i], p2-seq2[j] );
			valf += amino_dis[(int)*p1++][(int)*p2++] + 0.5 * pv;
			p1p++; p2p++;
		}
//		fprintf( stderr, "valf = %d\n", valf );
		val += deff * ( valf );
	}
	fprintf( stderr, "val = %f\n", val );
	return( val );
//	exit( 1 );
}

int naivepairscore11( char *seq1, char *seq2, int penal )
{
	int  vali;
	int len = strlen( seq1 );
	char *s1, *s2, *p1, *p2;
	s1 = calloc( len+1, sizeof( char ) );
	s2 = calloc( len+1, sizeof( char ) );
	{
		vali = 0;
		commongappickpair( s1, s2, seq1, seq2 );
//		fprintf( stderr, "i1 = %s\n", seq1[i] );
//		fprintf( stderr, "i2 = %s\n", seq2[j] );
//		fprintf( stderr, "s1 = %s\n", s1 );
//		fprintf( stderr, "s2 = %s\n", s2 );
//		fprintf( stderr, "penal = %d\n", penal );

		p1 = s1; p2 = s2;
		while( *p1 )
		{
			if( *p1 == '-' )
			{
//				fprintf( stderr, "Penal! %c-%c in %d-%d, %f\n", *(p1-1), *(p2-1), i, j, feff );
				vali += penal;
//				while( *p1 == '-' || *p2 == '-' ) 
				while( *p1 == '-' )  // SP
				{
					p1++;
					p2++;
				}
				continue;
			}
			if( *p2 == '-' )
			{
//				fprintf( stderr, "Penal! %c-%c in %d-%d, %f\n", *(p1-1), *(p2-1), i, j, feff );
				vali +=  penal;
//				while( *p2 == '-' || *p1 == '-' ) 
				while( *p2 == '-' )  // SP
				{
					p1++;
					p2++;
				}
				continue;
			}
//			fprintf( stderr, "adding %c-%c, %d\n", *p1, *p2, amino_dis[*p1][*p2] );
			vali += amino_dis[(int)*p1++][(int)*p2++];
		}
	}
	free( s1 );
	free( s2 );
//	fprintf( stderr, "vali = %d\n", vali );
	return( vali );
//	exit( 1 );
}
float naivepairscore( int n1, int n2, char **seq1, char **seq2, double *eff1, double *eff2, int penal )
{
//	return( 0.0 );
	int i, j;
	float val;
	int  vali;
	float feff;
	int len = strlen( seq1[0] );
	char *s1, *s2, *p1, *p2;
	s1 = calloc( len+1, sizeof( char ) );
	s2 = calloc( len+1, sizeof( char ) );
	val = 0.0;
	for( i=0; i<n1; i++ ) for( j=0; j<n2; j++ )
	{
		vali = 0;
		feff = eff1[i] * eff2[j];
//		fprintf( stderr, "feff %d-%d = %f\n", i, j, feff );
		commongappickpair( s1, s2, seq1[i], seq2[j] );
//		fprintf( stderr, "i1 = %s\n", seq1[i] );
//		fprintf( stderr, "i2 = %s\n", seq2[j] );
//		fprintf( stderr, "s1 = %s\n", s1 );
//		fprintf( stderr, "s2 = %s\n", s2 );
//		fprintf( stderr, "penal = %d\n", penal );

		p1 = s1; p2 = s2;
		while( *p1 )
		{
			if( *p1 == '-' )
			{
//				fprintf( stderr, "Penal! %c-%c in %d-%d, %f\n", *(p1-1), *(p2-1), i, j, feff );
				vali += penal;
//				while( *p1 == '-' || *p2 == '-' ) 
				while( *p1 == '-' )  // SP
				{
					p1++;
					p2++;
				}
				continue;
			}
			if( *p2 == '-' )
			{
//				fprintf( stderr, "Penal! %c-%c in %d-%d, %f\n", *(p1-1), *(p2-1), i, j, feff );
				vali +=  penal;
//				while( *p2 == '-' || *p1 == '-' ) 
				while( *p2 == '-' )  // SP
				{
					p1++;
					p2++;
				}
				continue;
			}
//			fprintf( stderr, "adding %c-%c, %d\n", *p1, *p2, amino_dis[*p1][*p2] );
			vali += amino_dis[(int)*p1++][(int)*p2++];
		}
//		fprintf( stderr, "vali = %d\n", vali );
		val += feff * vali;
	}
	free( s1 );
	free( s2 );
	fprintf( stderr, "val = %f\n", val );
	return( val );
//	exit( 1 );
}
