#include "mltaln.h"

#if 0
static FILE *fftfp;
#endif
static int n20or4or2;

#define KEIKA 0
#define RND   0
#define DEBUG 0

#if RND // by D.Mathog
static void generateRndSeq( char *seq, int len )
{
	while( len-- )
#if 1
		*seq++ = (int)( rnd() * n20or4or2 );
#else
		*seq++ = (int)1;
#endif
}
#endif

static void vec_init( Fukusosuu *result, int nlen )
{
	while( nlen-- )
	{
		result->R = result->I = 0.0;
		result++;
	}
}

#if 0 // by D.Mathog
static void vec_init2( Fukusosuu **result, char *seq, double eff, int st, int ed )
{
	int i;
	for( i=st; i<ed; i++ )
		result[(int)*seq++][i].R += eff;
}
#endif

static void seq_vec_2( Fukusosuu *result, double *score, double incr, char *seq )
{
	static int n;
	for( ; *seq; result++ )
	{
		n = amino_n[(int)*seq++];
		if( n < 20 && n >= 0 ) result->R += incr * score[n];
#if 0
		fprintf( stderr, "n=%d, score=%f, inc=%f R=%f\n",n,  score[n], incr * score[n], result->R );
#endif
	}
}

static void seq_vec_3( Fukusosuu **result, double incr, char *seq )
{
	int i;
	int n;
	for( i=0; *seq; i++ )
	{
		n = amino_n[(int)*seq++];
		if( n < n20or4or2 && n >= 0 ) result[n][i].R += incr;
	}
}

static void seq_vec_5( Fukusosuu *result, double *score1, double *score2, double incr, char *seq )
{
	int n;
	for( ; *seq; result++ )
	{
		n = amino_n[(int)*seq++];
		if( n > 20 ) continue;
		result->R += incr * score1[n];
		result->I += incr * score2[n];
#if 0
		fprintf( stderr, "n=%d, score=%f, inc=%f R=%f\n",n,  score[n], incr * score[n], result->R );
#endif
	}
}


static void seq_vec_4( Fukusosuu *result, double incr, char *seq )
{
	char s;
	for( ; *seq; result++ )
	{
		s = *seq++;
		if( s == 'a' )
			result->R += incr;
		else if( s == 't' )
			result->R -= incr;
		else if( s == 'g' )
			result->I += incr;
		else if( s == 'c' )
			result->I -= incr;
	}
}

#if 0 // by D.Mathog
static void seq_vec( Fukusosuu *result, char query, double incr, char *seq )
{
#if 0
	int bk = nlen;
#endif
	while( *seq )
	{
		if( *seq++ == query ) result->R += incr;
		result++;
#if 0
fprintf( stderr, "i = %d result->R = %f\n", bk-nlen, (result-1)->R );
#endif
	}
}

static int checkRepeat( int num, int *cutpos )
{
	int tmp, buf;

	buf = *cutpos;
	while( num-- )
	{
		if( ( tmp = *cutpos++ ) < buf ) return( 1 );
		buf = tmp;
	}
	return( 0 );
}

static int segcmp( void *ptr1, void *ptr2 )
{
	int diff;
	Segment **seg1 = (Segment **)ptr1;
	Segment **seg2 = (Segment **)ptr2;
#if 0
	return( (*seg1)->center - (*seg2)->center );
#else
	diff = (*seg1)->center - (*seg2)->center;
	if( diff ) return( diff );

	diff = (*seg1)->start - (*seg2)->start;
	if( diff ) return( diff );

	diff = (*seg1)->end - (*seg2)->end;
	if( diff ) return( diff );

	fprintf( stderr, "USE STABLE SORT !!\n" );
	exit( 1 );
	return( 0 );
#endif
}
#endif


static void mymergesort( int first, int last, Segment **seg )
{
	int middle;
	static int i, j, k, p;
	static int allo = 0;
	static Segment **work = NULL;
	if( last > allo )
	{
		allo = last;
		if( work ) free( work );
		work = (Segment **)calloc( allo / 2 + 1, sizeof( Segment *) );
	}

	if( first < last )
	{
		middle = ( first + last ) / 2;
		mymergesort( first, middle, seg );
		mymergesort( middle+1, last, seg );
		p = 0;
		for( i=first; i<=middle; i++ ) work[p++] = seg[i];
		i = middle + 1; j = 0; k = first;
		while( i <= last && j < p )
		{
			if( work[j]->center <= seg[i]->center ) 
				seg[k++] = work[j++];
			else
				seg[k++] = seg[i++];
		}
		while( j < p ) seg[k++] = work[j++];
	}
}


double Fgetlag( char  **seq1, char  **seq2, 
			    double *eff1, double *eff2, 
			    int    clus1, int    clus2,
			    int alloclen )
{
	int i, j, k, l, m;
	int nlen, nlen2, nlen4;
	static int crossscoresize = 0;
	static char **tmpseq1 = NULL;
	static char **tmpseq2 = NULL;
	static char **tmpptr1 = NULL;
	static char **tmpptr2 = NULL;
	static char **tmpres1 = NULL;
	static char **tmpres2 = NULL;
	static char **result1 = NULL;
	static char **result2 = NULL;
#if RND
	static char **rndseq1 = NULL;
	static char **rndseq2 = NULL;
#endif
	static Fukusosuu **seqVector1 = NULL;
	static Fukusosuu **seqVector2 = NULL;
	static Fukusosuu **naiseki = NULL;   
	static Fukusosuu *naisekiNoWa = NULL; 
	static double *soukan = NULL;
	static double **crossscore = NULL;
	int nlentmp;
	static int *kouho = NULL;
	static Segment *segment = NULL;
	static Segment *segment1 = NULL;
	static Segment *segment2 = NULL;
	static Segment **sortedseg1 = NULL;
	static Segment **sortedseg2 = NULL;
	static int *cut1 = NULL;
	static int *cut2 = NULL;
	static int localalloclen = 0;
	int lag;
	int tmpint;
	int count, count0;
	int len1, len2;
	int totallen;
	float dumfl = 0.0;

	len1 = strlen( seq1[0] );
	len2 = strlen( seq2[0] );
	nlentmp = MAX( len1, len2 );

	nlen = 1;
	while( nlentmp >= nlen ) nlen <<= 1;
#if 0
	fprintf( stderr, "###   nlen    = %d\n", nlen );
#endif

	nlen2 = nlen/2; nlen4 = nlen2 / 2;

#if DEBUG
	fprintf( stderr, "len1 = %d, len2 = %d\n", len1, len2 );
	fprintf( stderr, "nlentmp = %d, nlen = %d\n", nlentmp, nlen );
#endif

	if( !localalloclen )
	{
		kouho = AllocateIntVec( NKOUHO );
		cut1 = AllocateIntVec( MAXSEG );
		cut2 = AllocateIntVec( MAXSEG );
		tmpptr1 = AllocateCharMtx( njob, 0 );
		tmpptr2 = AllocateCharMtx( njob, 0 );
		result1 = AllocateCharMtx( njob, alloclen );
		result2 = AllocateCharMtx( njob, alloclen );
		tmpres1 = AllocateCharMtx( njob, alloclen );
		tmpres2 = AllocateCharMtx( njob, alloclen );
//		crossscore = AllocateDoubleMtx( MAXSEG, MAXSEG );
		segment = (Segment *)calloc( MAXSEG, sizeof( Segment ) );
		segment1 = (Segment *)calloc( MAXSEG, sizeof( Segment ) );
		segment2 = (Segment *)calloc( MAXSEG, sizeof( Segment ) );
		sortedseg1 = (Segment **)calloc( MAXSEG, sizeof( Segment * ) );
		sortedseg2 = (Segment **)calloc( MAXSEG, sizeof( Segment * ) );
		if( !( segment && segment1 && segment2 && sortedseg1 && sortedseg2 ) )
			ErrorExit( "Allocation error\n" );

		if     ( scoremtx == -1 ) n20or4or2 = 4;
		else if( fftscore == 1  ) n20or4or2 = 2;
		else                      n20or4or2 = 20;
	}
	if( localalloclen < nlen )
	{
		if( localalloclen )
		{
#if 1
			FreeFukusosuuMtx ( seqVector1 );
			FreeFukusosuuMtx ( seqVector2 );
			FreeFukusosuuVec( naisekiNoWa );
			FreeFukusosuuMtx( naiseki );
			FreeDoubleVec( soukan );
			FreeCharMtx( tmpseq1 );
			FreeCharMtx( tmpseq2 );
#endif
#if RND
			FreeCharMtx( rndseq1 );
			FreeCharMtx( rndseq2 );
#endif
		}


		tmpseq1 = AllocateCharMtx( njob, nlen );
		tmpseq2 = AllocateCharMtx( njob, nlen );
		naisekiNoWa = AllocateFukusosuuVec( nlen );
		naiseki = AllocateFukusosuuMtx( n20or4or2, nlen );
		seqVector1 = AllocateFukusosuuMtx( n20or4or2+1, nlen+1 );
		seqVector2 = AllocateFukusosuuMtx( n20or4or2+1, nlen+1 );
		soukan = AllocateDoubleVec( nlen+1 );

#if RND
		rndseq1 = AllocateCharMtx( njob, nlen );
		rndseq2 = AllocateCharMtx( njob, nlen );
		for( i=0; i<njob; i++ )
		{
			generateRndSeq( rndseq1[i], nlen );
			generateRndSeq( rndseq2[i], nlen );
		}
#endif
		localalloclen = nlen;
	}
	
	for( j=0; j<clus1; j++ ) strcpy( tmpseq1[j], seq1[j] );
	for( j=0; j<clus2; j++ ) strcpy( tmpseq2[j], seq2[j] );

#if 0
fftfp = fopen( "input_of_Falign", "w" );
fprintf( fftfp, "nlen = %d\n", nlen );
fprintf( fftfp, "seq1: ( %d sequences ) \n", clus1 );
for( i=0; i<clus1; i++ )
	fprintf( fftfp, "%s\n", seq1[i] );
fprintf( fftfp, "seq2: ( %d sequences ) \n", clus2 );
for( i=0; i<clus2; i++ )
	fprintf( fftfp, "%s\n", seq2[i] );
fclose( fftfp );
system( "less input_of_Falign < /dev/tty > /dev/tty" );
#endif

	if( fftkeika ) fprintf( stderr,  " FFT ... " );

	for( j=0; j<n20or4or2; j++ ) vec_init( seqVector1[j], nlen );
	if( fftscore && scoremtx != -1 )
	{
		for( i=0; i<clus1; i++ )
		{
			seq_vec_2( seqVector1[0], polarity, eff1[i], tmpseq1[i] );
			seq_vec_2( seqVector1[1], volume,   eff1[i], tmpseq1[i] );
		}
	}
	else
	{
#if 0
		for( i=0; i<clus1; i++ ) for( j=0; j<n20or4or2; j++ ) 
			seq_vec( seqVector1[j], amino[j], eff1[i], tmpseq1[i] );
#else
		for( i=0; i<clus1; i++ )
			seq_vec_3( seqVector1, eff1[i], tmpseq1[i] );
#endif
	}
#if RND
	for( i=0; i<clus1; i++ )
	{
		vec_init2( seqVector1, rndseq1[i], eff1[i], len1, nlen );
	}
#endif
#if 0
fftfp = fopen( "seqVec", "w" );
fprintf( fftfp, "before transform\n" );
for( k=0; k<n20or4or2; k++ ) 
{
   fprintf( fftfp, "nlen=%d\n", nlen );
   fprintf( fftfp, "%c\n", amino[k] );
   for( l=0; l<nlen; l++ )
   fprintf( fftfp, "%f %f\n", seqVector1[k][l].R, seqVector1[k][l].I );
}
fclose( fftfp );
system( "less seqVec < /dev/tty > /dev/tty" );
#endif

	for( j=0; j<n20or4or2; j++ ) vec_init( seqVector2[j], nlen );
	if( fftscore && scoremtx != -1 )
	{
		for( i=0; i<clus2; i++ )
		{
			seq_vec_2( seqVector2[0], polarity, eff2[i], tmpseq2[i] );
			seq_vec_2( seqVector2[1], volume,   eff2[i], tmpseq2[i] );
		}
	}
	else
	{
#if 0
		for( i=0; i<clus2; i++ ) for( j=0; j<n20or4or2; j++ ) 
			seq_vec( seqVector2[j], amino[j], eff2[i], tmpseq2[i] );
#else
		for( i=0; i<clus2; i++ )
			seq_vec_3( seqVector2, eff2[i], tmpseq2[i] );
#endif
	}
#if RND
	for( i=0; i<clus2; i++ )
	{
		vec_init2( seqVector2, rndseq2[i], eff2[i], len2, nlen );
	}
#endif

#if 0
fftfp = fopen( "seqVec2", "w" );
fprintf( fftfp, "before fft\n" );
for( k=0; k<n20or4or2; k++ ) 
{
   fprintf( fftfp, "%c\n", amino[k] );
   for( l=0; l<nlen; l++ )
   fprintf( fftfp, "%f %f\n", seqVector2[k][l].R, seqVector2[k][l].I );
}
fclose( fftfp );
system( "less seqVec2 < /dev/tty > /dev/tty" );
#endif

	for( j=0; j<n20or4or2; j++ )
	{
		fft( nlen, seqVector2[j], (j==0) );
		fft( nlen, seqVector1[j], 0 );
	}
#if 0
fftfp = fopen( "seqVec2", "w" );
fprintf( fftfp, "#after fft\n" );
for( k=0; k<n20or4or2; k++ ) 
{
   fprintf( fftfp, "#%c\n", amino[k] );
   for( l=0; l<nlen; l++ )
   fprintf( fftfp, "%f %f\n", seqVector2[k][l].R, seqVector2[k][l].I );
}
fclose( fftfp );
system( "less seqVec2 < /dev/tty > /dev/tty" );
#endif

	for( k=0; k<n20or4or2; k++ ) 
	{
		for( l=0; l<nlen; l++ ) 
			calcNaiseki( naiseki[k]+l, seqVector1[k]+l, seqVector2[k]+l );
	}
	for( l=0; l<nlen; l++ ) 
	{
		naisekiNoWa[l].R = 0.0;
		naisekiNoWa[l].I = 0.0;
		for( k=0; k<n20or4or2; k++ ) 
		{
			naisekiNoWa[l].R += naiseki[k][l].R;
			naisekiNoWa[l].I += naiseki[k][l].I;
		}
	}

#if 0
fftfp = fopen( "naisekiNoWa", "w" );
fprintf( fftfp, "#Before fft\n" );
for( l=0; l<nlen; l++ )
	fprintf( fftfp, "%d  %f %f\n", l, naisekiNoWa[l].R, naisekiNoWa[l].I ); 
fclose( fftfp );
system( "less naisekiNoWa < /dev/tty > /dev/tty " );
#endif

	fft( -nlen, naisekiNoWa, 0 );

	for( m=0; m<=nlen2; m++ ) 
		soukan[m] = naisekiNoWa[nlen2-m].R;
	for( m=nlen2+1; m<nlen; m++ ) 
		soukan[m] = naisekiNoWa[nlen+nlen2-m].R;

#if 0
fftfp = fopen( "naisekiNoWa", "w" );
fprintf( fftfp, "#After fft\n" );
for( l=0; l<nlen; l++ )
	fprintf( fftfp, "%d  %f\n", l, naisekiNoWa[l].R ); 
fclose( fftfp );
fftfp = fopen( "list.plot", "w"  );
fprintf( fftfp, "plot 'naisekiNoWa'\npause -1" );
fclose( fftfp );
system( "/usr/bin/gnuplot list.plot &" );
#endif
#if 0
fprintf( stderr, "frt write start\n" );
fftfp = fopen( "frt", "w" );
for( l=0; l<nlen; l++ )
	fprintf( fftfp, "%d  %f\n", l-nlen2, soukan[l] ); 
fclose( fftfp );
system( "less frt < /dev/tty > /dev/tty" );
#if 0
fftfp = fopen( "list.plot", "w"  );
fprintf( fftfp, "plot 'frt'\n pause +1" );
fclose( fftfp );
system( "/usr/bin/gnuplot list.plot" );
#endif
#endif


	getKouho( kouho, NKOUHO, soukan, nlen );

#if 0
	for( i=0; i<NKOUHO; i++ )
	{
		fprintf( stdout, "kouho[%d] = %d\n", i, kouho[i] );
	}
#endif

#if KEIKA
	fprintf( stderr, "Searching anchors ... " );
#endif
	count = 0;



#define CAND 0
#if CAND
	fftfp = fopen( "cand", "w" );
	fclose( fftfp );
#endif

	for( k=0; k<NKOUHO; k++ ) 
	{

		lag = kouho[k];
		zurasu2( lag, clus1, clus2, seq1, seq2, tmpptr1, tmpptr2 );
#if CAND
		fftfp = fopen( "cand", "a" );
		fprintf( fftfp, ">Candidate No.%d lag = %d\n", k+1, lag );
		fprintf( fftfp, "%s\n", tmpptr1[0] );
		fprintf( fftfp, ">Candidate No.%d lag = %d\n", k+1, lag );
		fprintf( fftfp, "%s\n", tmpptr2[0] );
		fprintf( fftfp, ">\n", k+1, lag );
		fclose( fftfp );
#endif
		tmpint = alignableReagion( clus1, clus2, tmpptr1, tmpptr2, eff1, eff2, segment+count );
		
		if( count+tmpint > MAXSEG -3 ) ErrorExit( "TOO MANY SEGMENTS.\n" );


		if( tmpint == 0 ) break; // 060430 iinoka ?
		while( tmpint-- > 0 )
		{
			if( lag > 0 )
			{
				segment1[count].start  = segment[count].start ;
				segment1[count].end    = segment[count].end   ;
				segment1[count].center = segment[count].center;
				segment1[count].score  = segment[count].score;

				segment2[count].start  = segment[count].start  + lag;
				segment2[count].end    = segment[count].end    + lag;
				segment2[count].center = segment[count].center + lag;
				segment2[count].score  = segment[count].score       ;
			}
			else
			{
				segment1[count].start  = segment[count].start  - lag;
				segment1[count].end    = segment[count].end    - lag;
				segment1[count].center = segment[count].center - lag;
				segment1[count].score  = segment[count].score       ;

				segment2[count].start  = segment[count].start ;
				segment2[count].end    = segment[count].end   ;
				segment2[count].center = segment[count].center;
				segment2[count].score  = segment[count].score ;
			}
#if 0
			fprintf( stderr, "Goukaku=%dko\n", tmpint ); 
			fprintf( stderr, "in 1 %d\n", segment1[count].center );
			fprintf( stderr, "in 2 %d\n", segment2[count].center );
#endif
			segment1[count].pair = &segment2[count];
			segment2[count].pair = &segment1[count];
			count++;
#if 0
			fprintf( stderr, "count=%d\n", count );
#endif
		}
	}

#if 1
	fprintf( stderr, "done. (%d anchors)\r", count );
#endif
	if( !count && fftNoAnchStop )
		ErrorExit( "Cannot detect anchor!" );
#if 0
	fprintf( stdout, "RESULT before sort:\n" );
	for( l=0; l<count+1; l++ )
	{
		fprintf( stdout, "cut[%d]=%d, ", l, segment1[l].center );
		fprintf( stdout, "%d score = %f\n", segment2[l].center, segment1[l].score );
	}
	exit( 1 );
#endif

#if KEIKA
	fprintf( stderr, "Aligning anchors ... " );
#endif
	for( i=0; i<count; i++ )
	{
		sortedseg1[i] = &segment1[i];
		sortedseg2[i] = &segment2[i];
	}

	{
		mymergesort( 0, count-1, sortedseg1 ); 
		mymergesort( 0, count-1, sortedseg2 ); 
		for( i=0; i<count; i++ ) sortedseg1[i]->number = i;
		for( i=0; i<count; i++ ) sortedseg2[i]->number = i;

		if( crossscoresize < count+2 )
		{
			crossscoresize = count+2;
			fprintf( stderr, "####################################################################################################################################allocating crossscore, size = %d\n", crossscoresize );
			if( crossscore ) FreeDoubleMtx( crossscore );
			crossscore = AllocateDoubleMtx( crossscoresize, crossscoresize );
		}

		for( i=0; i<count+2; i++ ) for( j=0; j<count+2; j++ )
			crossscore[i][j] = 0.0;
		for( i=0; i<count; i++ )
		{
			crossscore[segment1[i].number+1][segment1[i].pair->number+1] = segment1[i].score;
			cut1[i+1] = sortedseg1[i]->center;
			cut2[i+1] = sortedseg2[i]->center;
		}

#if DEBUG
		fprintf( stderr, "AFTER SORT\n" );
		for( i=0; i<count; i++ ) fprintf( stderr, "%d, %d\n", segment1[i].start, segment2[i].start );
#endif

		crossscore[0][0] = 10000000.0;
		cut1[0] = 0; 
		cut2[0] = 0;
		crossscore[count+1][count+1] = 10000000.0;
		cut1[count+1] = len1;
		cut2[count+1] = len2;
		count += 2;
		count0 = count;

		blockAlign2( cut1, cut2, sortedseg1, sortedseg2, crossscore, &count );
	}
	if( fftkeika )
	{
		if( count0 > count )
		{
			fprintf( stderr, "REPEAT!? \n" ); 
			if( fftRepeatStop ) exit( 1 );
		}
#if KEIKA
		else 
			fprintf( stderr, "done\n" );
			fprintf( stderr, "done. (%d anchors)\n", count );
#endif
	}

#if 0
	fftfp = fopen( "fft", "a" );
	fprintf( fftfp, "RESULT after sort:\n" );
	for( l=0; l<count; l++ )
	{
		fprintf( fftfp, "cut[%d]=%d, ", l, segment1[l].center );
		fprintf( fftfp, "%d\n", segment2[l].center );
	}
	fclose( fftfp );
#endif

#if 0
	fftfp = fopen( "fft", "a" );
	fprintf( fftfp, "RESULT after sort:\n" );
	for( l=0; l<count; l++ )
	{
		fprintf( fftfp, "cut : %d %d\n", cut1[l], cut2[l] );
	}
	fclose( fftfp );
#endif

#if KEIKA
	fprintf( trap_g, "Devided to %d segments\n", count-1 );
	fprintf( trap_g, "%d  %d forg\n", MIN( clus1, clus2 ), count-1 );
#endif

	totallen = 0;
	for( j=0; j<clus1; j++ ) result1[j][0] = 0;
	for( j=0; j<clus2; j++ ) result2[j][0] = 0;
	for( i=0; i<count-1; i++ )
	{
#if DEBUG
		fprintf( stderr, "DP %03d / %03d %4d to ", i+1, count-1, totallen );
#else
#if KEIKA
		fprintf( stderr, "DP %03d / %03d\r", i+1, count-1 );
#endif
#endif
		for( j=0; j<clus1; j++ )
		{
			strncpy( tmpres1[j], seq1[j]+cut1[i], cut1[i+1]-cut1[i] );
			tmpres1[j][cut1[i+1]-cut1[i]] = 0;
		}
		for( j=0; j<clus2; j++ )
		{
			strncpy( tmpres2[j], seq2[j]+cut2[i], cut2[i+1]-cut2[i] );
			tmpres2[j][cut2[i+1]-cut2[i]] = 0;
		}
		switch( alg )
		{
			case( 'a' ):
				Aalign( tmpres1, tmpres2, eff1, eff2, clus1, clus2, alloclen );
				break;
			case( 'M' ):
					MSalignmm( tmpres1, tmpres2, eff1, eff2, clus1, clus2, alloclen, NULL, NULL, NULL, NULL );
				break;
			case( 'A' ):
				if( clus1 == 1 && clus2 == 1 )
					G__align11( tmpres1, tmpres2, alloclen );
				else
					A__align( tmpres1, tmpres2, eff1, eff2, clus1, clus2, alloclen, NULL, &dumfl, NULL, NULL, NULL, NULL );
				break;
			case( 'H' ):
				if( clus1 == 1 && clus2 == 1 )
					G__align11( tmpres1, tmpres2, alloclen );
				else
					H__align( tmpres1, tmpres2, eff1, eff2, clus1, clus2, alloclen, NULL, &dumfl, NULL, NULL, NULL, NULL );
				break;
			case( 'Q' ):
				if( clus1 == 1 && clus2 == 1 )
					G__align11( tmpres1, tmpres2, alloclen );
				else
					Q__align( tmpres1, tmpres2, eff1, eff2, clus1, clus2, alloclen, NULL, &dumfl, NULL, NULL, NULL, NULL );
				break;
			default:
				fprintf( stderr, "alg = %c\n", alg );
				ErrorExit( "ERROR IN SOURCE FILE Falign.c" );
				break;
		}

		nlen = strlen( tmpres1[0] );
		if( totallen + nlen > alloclen ) ErrorExit( "LENGTH OVER in Falign\n " );
		for( j=0; j<clus1; j++ ) strcat( result1[j], tmpres1[j] );
		for( j=0; j<clus2; j++ ) strcat( result2[j], tmpres2[j] );
		totallen += nlen;
#if 0
		fprintf( stderr, "%4d\r", totallen );
		fprintf( stderr, "\n\n" );
		for( j=0; j<clus1; j++ ) 
		{
			fprintf( stderr, "%s\n", tmpres1[j] );
		}
		fprintf( stderr, "-------\n" );
		for( j=0; j<clus2; j++ ) 
		{
			fprintf( stderr, "%s\n", tmpres2[j] );
		}
#endif
	}
#if KEIKA
	fprintf( stderr, "DP ... done   \n" );
#endif

	for( j=0; j<clus1; j++ ) strcpy( seq1[j], result1[j] );
	for( j=0; j<clus2; j++ ) strcpy( seq2[j], result2[j] );
#if 0
	for( j=0; j<clus1; j++ ) 
	{
		fprintf( stderr, "%s\n", result1[j] );
	}
	fprintf( stderr, "- - - - - - - - - - -\n" );
	for( j=0; j<clus2; j++ ) 
	{
		fprintf( stderr, "%s\n", result2[j] );
	}
#endif
	return( 0.0 );
}
float Falign( char  **seq1, char  **seq2, 
			  double *eff1, double *eff2, 
			  int    clus1, int    clus2,
			  int alloclen, int *fftlog )
{
	int i, j, k, l, m, maxk;
	int nlen, nlen2, nlen4;
	static int prevalloclen = 0;
	static int crossscoresize = 0;
	static char **tmpseq1 = NULL;
	static char **tmpseq2 = NULL;
	static char **tmpptr1 = NULL;
	static char **tmpptr2 = NULL;
	static char **tmpres1 = NULL;
	static char **tmpres2 = NULL;
	static char **result1 = NULL;
	static char **result2 = NULL;
#if RND
	static char **rndseq1 = NULL;
	static char **rndseq2 = NULL;
#endif
	static Fukusosuu **seqVector1 = NULL;
	static Fukusosuu **seqVector2 = NULL;
	static Fukusosuu **naiseki = NULL;   
	static Fukusosuu *naisekiNoWa = NULL; 
	static double *soukan = NULL;
	static double **crossscore = NULL;
	int nlentmp;
	static int *kouho = NULL;
	static Segment *segment = NULL;
	static Segment *segment1 = NULL;
	static Segment *segment2 = NULL;
	static Segment **sortedseg1 = NULL;
	static Segment **sortedseg2 = NULL;
	static int *cut1 = NULL;
	static int *cut2 = NULL;
	static char *sgap1, *egap1, *sgap2, *egap2;
	static int localalloclen = 0;
	int lag;
	int tmpint;
	int count, count0;
	int len1, len2;
	int totallen;
	float totalscore;
	float dumfl = 0.0;


	len1 = strlen( seq1[0] );
	len2 = strlen( seq2[0] );
	nlentmp = MAX( len1, len2 );

	nlen = 1;
	while( nlentmp >= nlen ) nlen <<= 1;
#if 0
	fprintf( stderr, "###   nlen    = %d\n", nlen );
#endif

	nlen2 = nlen/2; nlen4 = nlen2 / 2;

#if DEBUG
	fprintf( stderr, "len1 = %d, len2 = %d\n", len1, len2 );
	fprintf( stderr, "nlentmp = %d, nlen = %d\n", nlentmp, nlen );
#endif

	if( prevalloclen != alloclen ) // Falign_noudp mo kaeru
	{
		if( prevalloclen )
		{
			FreeCharMtx( result1 );
			FreeCharMtx( result2 );
			FreeCharMtx( tmpres1 );
			FreeCharMtx( tmpres2 );
		}
//		fprintf( stderr, "\n\n\nreallocating ...\n" ); 
		result1 = AllocateCharMtx( njob, alloclen );
		result2 = AllocateCharMtx( njob, alloclen );
		tmpres1 = AllocateCharMtx( njob, alloclen );
		tmpres2 = AllocateCharMtx( njob, alloclen );
		prevalloclen = alloclen;
	}
	if( !localalloclen )
	{
		sgap1 = AllocateCharVec( njob );
		egap1 = AllocateCharVec( njob );
		sgap2 = AllocateCharVec( njob );
		egap2 = AllocateCharVec( njob );
		kouho = AllocateIntVec( NKOUHO );
		cut1 = AllocateIntVec( MAXSEG );
		cut2 = AllocateIntVec( MAXSEG );
		tmpptr1 = AllocateCharMtx( njob, 0 );
		tmpptr2 = AllocateCharMtx( njob, 0 );
//		crossscore = AllocateDoubleMtx( MAXSEG, MAXSEG );
		segment = (Segment *)calloc( MAXSEG, sizeof( Segment ) );
		segment1 = (Segment *)calloc( MAXSEG, sizeof( Segment ) );
		segment2 = (Segment *)calloc( MAXSEG, sizeof( Segment ) );
		sortedseg1 = (Segment **)calloc( MAXSEG, sizeof( Segment * ) );
		sortedseg2 = (Segment **)calloc( MAXSEG, sizeof( Segment * ) );
		if( !( segment && segment1 && segment2 && sortedseg1 && sortedseg2 ) )
			ErrorExit( "Allocation error\n" );

		if     ( scoremtx == -1 ) n20or4or2 = 1;
		else if( fftscore )       n20or4or2 = 1;
		else                      n20or4or2 = 20;
	}
	if( localalloclen < nlen )
	{
		if( localalloclen )
		{
#if 1
			if( !kobetsubunkatsu )
			{
				FreeFukusosuuMtx ( seqVector1 );
				FreeFukusosuuMtx ( seqVector2 );
				FreeFukusosuuVec( naisekiNoWa );
				FreeFukusosuuMtx( naiseki );
				FreeDoubleVec( soukan );
			}
			FreeCharMtx( tmpseq1 );
			FreeCharMtx( tmpseq2 );
#endif
#if RND
			FreeCharMtx( rndseq1 );
			FreeCharMtx( rndseq2 );
#endif
		}

		tmpseq1 = AllocateCharMtx( njob, nlen );
		tmpseq2 = AllocateCharMtx( njob, nlen );
		if( !kobetsubunkatsu )
		{
			naisekiNoWa = AllocateFukusosuuVec( nlen );
			naiseki = AllocateFukusosuuMtx( n20or4or2, nlen );
			seqVector1 = AllocateFukusosuuMtx( n20or4or2+1, nlen+1 );
			seqVector2 = AllocateFukusosuuMtx( n20or4or2+1, nlen+1 );
			soukan = AllocateDoubleVec( nlen+1 );
		}
#if RND
		rndseq1 = AllocateCharMtx( njob, nlen );
		rndseq2 = AllocateCharMtx( njob, nlen );
		for( i=0; i<njob; i++ )
		{
			generateRndSeq( rndseq1[i], nlen );
			generateRndSeq( rndseq2[i], nlen );
		}
#endif
		localalloclen = nlen;
	}
	
	for( j=0; j<clus1; j++ ) strcpy( tmpseq1[j], seq1[j] );
	for( j=0; j<clus2; j++ ) strcpy( tmpseq2[j], seq2[j] );

#if 0
fftfp = fopen( "input_of_Falign", "w" );
fprintf( fftfp, "nlen = %d\n", nlen );
fprintf( fftfp, "seq1: ( %d sequences ) \n", clus1 );
for( i=0; i<clus1; i++ )
	fprintf( fftfp, "%s\n", seq1[i] );
fprintf( fftfp, "seq2: ( %d sequences ) \n", clus2 );
for( i=0; i<clus2; i++ )
	fprintf( fftfp, "%s\n", seq2[i] );
fclose( fftfp );
system( "less input_of_Falign < /dev/tty > /dev/tty" );
#endif
	if( !kobetsubunkatsu )
	{
		if( fftkeika ) fprintf( stderr,  " FFT ... " );

		for( j=0; j<n20or4or2; j++ ) vec_init( seqVector1[j], nlen );
		if( fftscore && scoremtx != -1 )
		{
			for( i=0; i<clus1; i++ )
			{
#if 1
				seq_vec_5( seqVector1[0], polarity, volume, eff1[i], tmpseq1[i] );
#else
				seq_vec_2( seqVector1[0], polarity, eff1[i], tmpseq1[i] );
				seq_vec_2( seqVector1[1], volume,   eff1[i], tmpseq1[i] );
#endif
			}
		}
		else
		{
#if 0
			for( i=0; i<clus1; i++ ) for( j=0; j<n20or4or2; j++ ) 
				seq_vec( seqVector1[j], amino[j], eff1[i], tmpseq1[i] );
#else
			for( i=0; i<clus1; i++ )
				seq_vec_3( seqVector1, eff1[i], tmpseq1[i] );
#endif
		}
#if RND
		for( i=0; i<clus1; i++ )
		{
			vec_init2( seqVector1, rndseq1[i], eff1[i], len1, nlen );
		}
#endif
#if 0
fftfp = fopen( "seqVec", "w" );
fprintf( fftfp, "before transform\n" );
for( k=0; k<n20or4or2; k++ ) 
{
   fprintf( fftfp, "nlen=%d\n", nlen );
   fprintf( fftfp, "%c\n", amino[k] );
   for( l=0; l<nlen; l++ )
   fprintf( fftfp, "%f %f\n", seqVector1[k][l].R, seqVector1[k][l].I );
}
fclose( fftfp );
system( "less seqVec < /dev/tty > /dev/tty" );
#endif

		for( j=0; j<n20or4or2; j++ ) vec_init( seqVector2[j], nlen );
		if( fftscore && scoremtx != -1 )
		{
			for( i=0; i<clus2; i++ )
			{
#if 1
				seq_vec_5( seqVector2[0], polarity, volume, eff2[i], tmpseq2[i] );
#else
				seq_vec_2( seqVector2[0], polarity, eff2[i], tmpseq2[i] );
				seq_vec_2( seqVector2[1], volume,   eff2[i], tmpseq2[i] );
#endif
			}
		}
		else
		{
#if 0
			for( i=0; i<clus2; i++ ) for( j=0; j<n20or4or2; j++ ) 
				seq_vec( seqVector2[j], amino[j], eff2[i], tmpseq2[i] );
#else
			for( i=0; i<clus2; i++ )
				seq_vec_3( seqVector2, eff2[i], tmpseq2[i] );
#endif
		}
#if RND
		for( i=0; i<clus2; i++ )
		{
			vec_init2( seqVector2, rndseq2[i], eff2[i], len2, nlen );
		}
#endif

#if 0
fftfp = fopen( "seqVec2", "w" );
fprintf( fftfp, "before fft\n" );
for( k=0; k<n20or4or2; k++ ) 
{
   fprintf( fftfp, "%c\n", amino[k] );
   for( l=0; l<nlen; l++ )
   fprintf( fftfp, "%f %f\n", seqVector2[k][l].R, seqVector2[k][l].I );
}
fclose( fftfp );
system( "less seqVec2 < /dev/tty > /dev/tty" );
#endif

		for( j=0; j<n20or4or2; j++ )
		{
			fft( nlen, seqVector2[j], (j==0) );
			fft( nlen, seqVector1[j], 0 );
		}
#if 0
fftfp = fopen( "seqVec2", "w" );
fprintf( fftfp, "#after fft\n" );
for( k=0; k<n20or4or2; k++ ) 
{
   fprintf( fftfp, "#%c\n", amino[k] );
   for( l=0; l<nlen; l++ )
	   fprintf( fftfp, "%f %f\n", seqVector2[k][l].R, seqVector2[k][l].I );
}
fclose( fftfp );
system( "less seqVec2 < /dev/tty > /dev/tty" );
#endif

		for( k=0; k<n20or4or2; k++ ) 
		{
			for( l=0; l<nlen; l++ ) 
				calcNaiseki( naiseki[k]+l, seqVector1[k]+l, seqVector2[k]+l );
		}
		for( l=0; l<nlen; l++ ) 
		{
			naisekiNoWa[l].R = 0.0;
			naisekiNoWa[l].I = 0.0;
			for( k=0; k<n20or4or2; k++ ) 
			{
				naisekiNoWa[l].R += naiseki[k][l].R;
				naisekiNoWa[l].I += naiseki[k][l].I;
			}
		}
	
#if 0
	fftfp = fopen( "naisekiNoWa", "w" );
	fprintf( fftfp, "#Before fft\n" );
	for( l=0; l<nlen; l++ )
		fprintf( fftfp, "%d  %f %f\n", l, naisekiNoWa[l].R, naisekiNoWa[l].I ); 
	fclose( fftfp );
	system( "less naisekiNoWa < /dev/tty > /dev/tty " );
#endif

		fft( -nlen, naisekiNoWa, 0 );
	
		for( m=0; m<=nlen2; m++ ) 
			soukan[m] = naisekiNoWa[nlen2-m].R;
		for( m=nlen2+1; m<nlen; m++ ) 
			soukan[m] = naisekiNoWa[nlen+nlen2-m].R;

#if 0
	fftfp = fopen( "naisekiNoWa", "w" );
	fprintf( fftfp, "#After fft\n" );
	for( l=0; l<nlen; l++ )
		fprintf( fftfp, "%d  %f\n", l, naisekiNoWa[l].R ); 
	fclose( fftfp );
	fftfp = fopen( "list.plot", "w"  );
	fprintf( fftfp, "plot 'naisekiNoWa'\npause -1" );
	fclose( fftfp );
	system( "/usr/bin/gnuplot list.plot &" );
#endif
#if 0
	fprintf( stderr, "soukan\n" );
	for( l=0; l<nlen; l++ )
		fprintf( stderr, "%d  %f\n", l-nlen2, soukan[l] ); 
#if 0
	fftfp = fopen( "list.plot", "w"  );
	fprintf( fftfp, "plot 'frt'\n pause +1" );
	fclose( fftfp );
	system( "/usr/bin/gnuplot list.plot" );
#endif
#endif


		getKouho( kouho, NKOUHO, soukan, nlen );

#if 0
		for( i=0; i<NKOUHO; i++ )
		{
			fprintf( stderr, "kouho[%d] = %d\n", i, kouho[i] );
		}
#endif
	}

#if KEIKA
	fprintf( stderr, "Searching anchors ... " );
#endif
	count = 0;



#define CAND 0
#if CAND
	fftfp = fopen( "cand", "w" );
	fclose( fftfp );
#endif
	if( kobetsubunkatsu )
	{
		maxk = 1;
		kouho[0] = 0;
	}
	else
	{
		maxk = NKOUHO;
	}

	for( k=0; k<maxk; k++ ) 
	{
		lag = kouho[k];
		if( lag <= -len1 || len2 <= lag ) continue;
		zurasu2( lag, clus1, clus2, seq1, seq2, tmpptr1, tmpptr2 );
#if CAND
		fftfp = fopen( "cand", "a" );
		fprintf( fftfp, ">Candidate No.%d lag = %d\n", k+1, lag );
		fprintf( fftfp, "%s\n", tmpptr1[0] );
		fprintf( fftfp, ">Candidate No.%d lag = %d\n", k+1, lag );
		fprintf( fftfp, "%s\n", tmpptr2[0] );
		fprintf( fftfp, ">\n", k+1, lag );
		fclose( fftfp );
#endif

//		fprintf( stderr, "lag = %d\n", lag );
		tmpint = alignableReagion( clus1, clus2, tmpptr1, tmpptr2, eff1, eff2, segment+count );

//		if( lag == -50 ) exit( 1 );
		
		if( count+tmpint > MAXSEG -3 ) ErrorExit( "TOO MANY SEGMENTS.\n" );


		if( tmpint == 0 ) break; // 060430 iinoka ?
		while( tmpint-- > 0 )
		{
#if 0
			if( segment[count].end - segment[count].start < fftWinSize )
			{
				count++;
				continue;
			}
#endif
			if( lag > 0 )
			{
				segment1[count].start  = segment[count].start ;
				segment1[count].end    = segment[count].end   ;
				segment1[count].center = segment[count].center;
				segment1[count].score  = segment[count].score;

				segment2[count].start  = segment[count].start  + lag;
				segment2[count].end    = segment[count].end    + lag;
				segment2[count].center = segment[count].center + lag;
				segment2[count].score  = segment[count].score       ;
			}
			else
			{
				segment1[count].start  = segment[count].start  - lag;
				segment1[count].end    = segment[count].end    - lag;
				segment1[count].center = segment[count].center - lag;
				segment1[count].score  = segment[count].score       ;

				segment2[count].start  = segment[count].start ;
				segment2[count].end    = segment[count].end   ;
				segment2[count].center = segment[count].center;
				segment2[count].score  = segment[count].score ;
			}
#if 0
			fprintf( stderr, "in 1 %d\n", segment1[count].center );
			fprintf( stderr, "in 2 %d\n", segment2[count].center );
#endif
			segment1[count].pair = &segment2[count];
			segment2[count].pair = &segment1[count];
			count++;
		}
	}
#if 0
	if( !kobetsubunkatsu && fftkeika )
		fprintf( stderr, "%d anchors found\r", count );
#endif
	if( !count && fftNoAnchStop )
		ErrorExit( "Cannot detect anchor!" );
#if 0
	fprintf( stderr, "RESULT before sort:\n" );
	for( l=0; l<count+1; l++ )
	{
		fprintf( stderr, "cut[%d]=%d, ", l, segment1[l].center );
		fprintf( stderr, "%d score = %f\n", segment2[l].center, segment1[l].score );
	}
#endif

#if KEIKA
	fprintf( stderr, "done. (%d anchors)\n", count );
	fprintf( stderr, "Aligning anchors ... " );
#endif
	for( i=0; i<count; i++ )
	{
		sortedseg1[i] = &segment1[i];
		sortedseg2[i] = &segment2[i];
	}
#if 0
	tmpsort( count, sortedseg1 ); 
	tmpsort( count, sortedseg2 ); 
	qsort( sortedseg1, count, sizeof( Segment * ), segcmp );
	qsort( sortedseg2, count, sizeof( Segment * ), segcmp );
#else
	mymergesort( 0, count-1, sortedseg1 ); 
	mymergesort( 0, count-1, sortedseg2 ); 
#endif
	for( i=0; i<count; i++ ) sortedseg1[i]->number = i;
	for( i=0; i<count; i++ ) sortedseg2[i]->number = i;


	if( kobetsubunkatsu )
	{
		for( i=0; i<count; i++ )
	    {
			cut1[i+1] = sortedseg1[i]->center;
			cut2[i+1] = sortedseg2[i]->center;
		}
		cut1[0] = 0;
		cut2[0] = 0;
		cut1[count+1] = len1;
		cut2[count+1] = len2;
		count += 2;
	}
	else
	{
		if( crossscoresize < count+2 )
		{
			crossscoresize = count+2;
#if 1
			if( fftkeika ) fprintf( stderr, "######allocating crossscore, size = %d\n", crossscoresize );
#endif
			if( crossscore ) FreeDoubleMtx( crossscore );
			crossscore = AllocateDoubleMtx( crossscoresize, crossscoresize );
		}
		for( i=0; i<count+2; i++ ) for( j=0; j<count+2; j++ )
			crossscore[i][j] = 0.0;
		for( i=0; i<count; i++ )
		{
			crossscore[segment1[i].number+1][segment1[i].pair->number+1] = segment1[i].score;
			cut1[i+1] = sortedseg1[i]->center;
			cut2[i+1] = sortedseg2[i]->center;
		}

#if 0
		fprintf( stderr, "AFTER SORT\n" );
		for( i=0; i<count+1; i++ ) fprintf( stderr, "%d, %d\n", cut1[i], cut2[i] );
		fprintf( stderr, "crossscore = \n" );
		for( i=0; i<count+1; i++ )
		{
			for( j=0; j<count+1; j++ )
				fprintf( stderr, "%.0f ", crossscore[i][j] );
			fprintf( stderr, "\n" );
		}
#endif

		crossscore[0][0] = 10000000.0;
		cut1[0] = 0; 
		cut2[0] = 0;
		crossscore[count+1][count+1] = 10000000.0;
		cut1[count+1] = len1;
		cut2[count+1] = len2;
		count += 2;
		count0 = count;
	
		blockAlign2( cut1, cut2, sortedseg1, sortedseg2, crossscore, &count );

//		if( count-count0 )
//			fprintf( stderr, "%d unused anchors\n", count0-count );

		if( !kobetsubunkatsu && fftkeika )
			fprintf( stderr, "%d anchors found\n", count );
		if( fftkeika )
		{
			if( count0 > count )
			{
#if 0
				fprintf( stderr, "\7 REPEAT!? \n" ); 
#else
				fprintf( stderr, "REPEAT!? \n" ); 
#endif
				if( fftRepeatStop ) exit( 1 );
			}
#if KEIKA
			else fprintf( stderr, "done\n" );
#endif
		}
	}

#if 0
	fftfp = fopen( "fft", "a" );
	fprintf( fftfp, "RESULT after sort:\n" );
	for( l=0; l<count; l++ )
	{
		fprintf( fftfp, "cut[%d]=%d, ", l, segment1[l].center );
		fprintf( fftfp, "%d\n", segment2[l].center );
	}
	fclose( fftfp );
#endif

#if 0
	fprintf( stderr, "RESULT after blckalign:\n" );
	for( l=0; l<count+1; l++ )
	{
		fprintf( stderr, "cut : %d %d\n", cut1[l], cut2[l] );
	}
#endif

#if 0
	fprintf( trap_g, "Devided to %d segments\n", count-1 );
	fprintf( trap_g, "%d  %d forg\n", MIN( clus1, clus2 ), count-1 );
#endif

	totallen = 0;
	for( j=0; j<clus1; j++ ) result1[j][0] = 0;
	for( j=0; j<clus2; j++ ) result2[j][0] = 0;
	totalscore = 0.0;
	*fftlog = -1;
	for( i=0; i<count-1; i++ )
	{
		*fftlog += 1;

		if( cut1[i] ) // chuui
		{
//			getkyokaigap( sgap1, tmpres1, nlen-1, clus1 );
//			getkyokaigap( sgap2, tmpres2, nlen-1, clus2 );
			getkyokaigap( sgap1, tmpres1, nlen-1, clus1 );
			getkyokaigap( sgap2, tmpres2, nlen-1, clus2 );
		}
		else
		{
			for( j=0; j<clus1; j++ ) sgap1[j] = 'o';
			for( j=0; j<clus2; j++ ) sgap2[j] = 'o';
		}
		if( cut1[i+1] != len1 ) // chuui
		{       
			getkyokaigap( egap1, seq1, cut1[i+1], clus1 );
			getkyokaigap( egap2, seq2, cut2[i+1], clus2 );
		}       
		else    
		{       
			for( j=0; j<clus1; j++ ) egap1[j] = 'o';
			for( j=0; j<clus2; j++ ) egap2[j] = 'o';
		}
#if 0
		{
			fprintf( stderr, "kyokkaigap1(%d)=", cut1[i]-1 );
			for( j=0; j<clus1; j++ )
				fprintf( stderr, "%c", sgap1[j] );
			fprintf( stderr, "=kyokkaigap1-start\n" );
		}
		{
			fprintf( stderr, "kyokkaigap2(%d)=", cut2[i]-1 );
			for( j=0; j<clus2; j++ )
				fprintf( stderr, "%c", sgap2[j] );
			fprintf( stderr, "=kyokkaigap2-start\n" );
		}
		{
			fprintf( stderr, "kyokkaigap1(%d)=", cut1[i]-1 );
			for( j=0; j<clus1; j++ )
				fprintf( stderr, "%c", egap1[j] );
			fprintf( stderr, "=kyokkaigap1-end\n" );
		}
		{
			fprintf( stderr, "kyokkaigap2(%d)=", cut2[i]-1 );
			for( j=0; j<clus2; j++ )
				fprintf( stderr, "%c", egap2[j] );
			fprintf( stderr, "=kyokkaigap2-end\n" );
		}
#endif

#if DEBUG
		fprintf( stderr, "DP %03d / %03d %4d to ", i+1, count-1, totallen );
#else
#if KEIKA
		fprintf( stderr, "DP %03d / %03d\r", i+1, count-1 );
#endif
#endif
		for( j=0; j<clus1; j++ )
		{
			strncpy( tmpres1[j], seq1[j]+cut1[i], cut1[i+1]-cut1[i] );
			tmpres1[j][cut1[i+1]-cut1[i]] = 0;
		}
		if( kobetsubunkatsu && fftkeika ) commongappick( clus1, tmpres1 ); //dvtditr に呼ばれたとき fftkeika=1
//		if( kobetsubunkatsu ) commongappick( clus1, tmpres1 );
		for( j=0; j<clus2; j++ )
		{
			strncpy( tmpres2[j], seq2[j]+cut2[i], cut2[i+1]-cut2[i] );
			tmpres2[j][cut2[i+1]-cut2[i]] = 0;
		}
		if( kobetsubunkatsu && fftkeika ) commongappick( clus2, tmpres2 ); //dvtditr に呼ばれたとき fftkeika=1
//		if( kobetsubunkatsu ) commongappick( clus2, tmpres2 );

		if( constraint )
		{
			fprintf( stderr, "Not supported\n" );
			exit( 1 );
		}
#if 0
		fprintf( stderr, "i=%d, before alignment", i );
		fprintf( stderr, "%4d\n", totallen );
		fprintf( stderr, "\n\n" );
		for( j=0; j<clus1; j++ ) 
		{
			fprintf( stderr, "%s\n", tmpres1[j] );
		}
		fprintf( stderr, "-------\n" );
		for( j=0; j<clus2; j++ ) 
		{
			fprintf( stderr, "%s\n", tmpres2[j] );
		}
#endif

#if 0
		fprintf( stdout, "writing input\n" );
		for( j=0; j<clus1; j++ )
		{
			fprintf( stdout, ">%d of GROUP1\n", j );
			fprintf( stdout, "%s\n", tmpres1[j] );
		}
		for( j=0; j<clus2; j++ )
		{
			fprintf( stdout, ">%d of GROUP2\n", j );
			fprintf( stdout, "%s\n", tmpres2[j] );
		}
		fflush( stdout );
#endif
		switch( alg )
		{
			case( 'a' ):
				totalscore += Aalign( tmpres1, tmpres2, eff1, eff2, clus1, clus2, alloclen );
				break;
			case( 'M' ):
					totalscore += MSalignmm( tmpres1, tmpres2, eff1, eff2, clus1, clus2, alloclen, sgap1, sgap2, egap1, egap2 );
				break;
			case( 'A' ):
				if( clus1 == 1 && clus2 == 1 )
				{
					totalscore += G__align11( tmpres1, tmpres2, alloclen );
				}
				else
					totalscore += A__align( tmpres1, tmpres2, eff1, eff2, clus1, clus2, alloclen, NULL, &dumfl, sgap1, sgap2, egap1, egap2 );
				break;
			case( 'H' ):
				if( clus1 == 1 && clus2 == 1 )
				{
					totalscore += G__align11( tmpres1, tmpres2, alloclen );
				}
				else
					totalscore += H__align( tmpres1, tmpres2, eff1, eff2, clus1, clus2, alloclen, NULL, &dumfl, sgap1, sgap2, egap1, egap2 );
				break;
			case( 'Q' ):
				if( clus1 == 1 && clus2 == 1 )
				{
					totalscore += G__align11( tmpres1, tmpres2, alloclen );
				}
				else
					totalscore += Q__align( tmpres1, tmpres2, eff1, eff2, clus1, clus2, alloclen, NULL, &dumfl, sgap1, sgap2, egap1, egap2 );
				break;
			default:
				fprintf( stderr, "alg = %c\n", alg );
				ErrorExit( "ERROR IN SOURCE FILE Falign.c" );
				break;
		}

		nlen = strlen( tmpres1[0] );
		if( totallen + nlen > alloclen )
		{
			fprintf( stderr, "totallen=%d +  nlen=%d > alloclen = %d\n", totallen, nlen, alloclen );
			ErrorExit( "LENGTH OVER in Falign\n " );
		}
		for( j=0; j<clus1; j++ ) strcat( result1[j], tmpres1[j] );
		for( j=0; j<clus2; j++ ) strcat( result2[j], tmpres2[j] );
		totallen += nlen;
#if 0
		fprintf( stderr, "i=%d", i );
		fprintf( stderr, "%4d\n", totallen );
		fprintf( stderr, "\n\n" );
		for( j=0; j<clus1; j++ ) 
		{
			fprintf( stderr, "%s\n", tmpres1[j] );
		}
		fprintf( stderr, "-------\n" );
		for( j=0; j<clus2; j++ ) 
		{
			fprintf( stderr, "%s\n", tmpres2[j] );
		}
#endif
	}
#if KEIKA
	fprintf( stderr, "DP ... done   \n" );
#endif

	for( j=0; j<clus1; j++ ) strcpy( seq1[j], result1[j] );
	for( j=0; j<clus2; j++ ) strcpy( seq2[j], result2[j] );
#if 0
	for( j=0; j<clus1; j++ ) 
	{
		fprintf( stderr, "%s\n", result1[j] );
	}
	fprintf( stderr, "- - - - - - - - - - -\n" );
	for( j=0; j<clus2; j++ ) 
	{
		fprintf( stderr, "%s\n", result2[j] );
	}
#endif
	return( totalscore );
}








float Falign_noudp( char  **seq1, char  **seq2, 
			  double *eff1, double *eff2, 
			  int    clus1, int    clus2,
			  int alloclen, int *fftlog )
{
	int i, j, k, l, m, maxk;
	int nlen, nlen2, nlen4;
	static int prevalloclen = 0;
//	static int crossscoresize = 0;
	static char **tmpseq1 = NULL;
	static char **tmpseq2 = NULL;
	static char **tmpptr1 = NULL;
	static char **tmpptr2 = NULL;
	static char **tmpres1 = NULL;
	static char **tmpres2 = NULL;
	static char **result1 = NULL;
	static char **result2 = NULL;
#if RND
	static char **rndseq1 = NULL;
	static char **rndseq2 = NULL;
#endif
	static Fukusosuu **seqVector1 = NULL;
	static Fukusosuu **seqVector2 = NULL;
	static Fukusosuu **naiseki = NULL;   
	static Fukusosuu *naisekiNoWa = NULL; 
	static double *soukan = NULL;
	int nlentmp;
	static int *kouho = NULL;
	static Segment *segment = NULL;
	static Segment *segment1 = NULL;
	static Segment *segment2 = NULL;
	static Segment **sortedseg1 = NULL;
	static Segment **sortedseg2 = NULL;
	static int *cut1 = NULL;
	static int *cut2 = NULL;
	static char *sgap1, *egap1, *sgap2, *egap2;
	static int localalloclen = 0;
	int lag;
	int tmpint;
	int count, count0;
	int len1, len2;
	int totallen;
	float totalscore;
	int nkouho;
//	float dumfl = 0.0;


	len1 = strlen( seq1[0] );
	len2 = strlen( seq2[0] );
	nlentmp = MAX( len1, len2 );

	nlen = 1;
	while( nlentmp >= nlen ) nlen <<= 1;
#if 0
	fprintf( stderr, "###   nlen    = %d\n", nlen );
#endif

	nlen2 = nlen/2; nlen4 = nlen2 / 2;

#if 0
	fprintf( stderr, "len1 = %d, len2 = %d\n", len1, len2 );
	fprintf( stderr, "nlentmp = %d, nlen = %d\n", nlentmp, nlen );
#endif

	if( prevalloclen != alloclen ) // Falign_noudp mo kaeru
	{
		if( prevalloclen )
		{
			FreeCharMtx( result1 );
			FreeCharMtx( result2 );
			FreeCharMtx( tmpres1 );
			FreeCharMtx( tmpres2 );
		}
//		fprintf( stderr, "\n\n\nreallocating ...\n" ); 
		result1 = AllocateCharMtx( njob, alloclen );
		result2 = AllocateCharMtx( njob, alloclen );
		tmpres1 = AllocateCharMtx( njob, alloclen );
		tmpres2 = AllocateCharMtx( njob, alloclen );
		prevalloclen = alloclen;
	}

	if( !localalloclen )
	{
		sgap1 = AllocateCharVec( njob );
		egap1 = AllocateCharVec( njob );
		sgap2 = AllocateCharVec( njob );
		egap2 = AllocateCharVec( njob );
		kouho = AllocateIntVec( NKOUHO_LONG );
		cut1 = AllocateIntVec( MAXSEG );
		cut2 = AllocateIntVec( MAXSEG );
		tmpptr1 = AllocateCharMtx( njob, 0 );
		tmpptr2 = AllocateCharMtx( njob, 0 );
		segment = (Segment *)calloc( MAXSEG, sizeof( Segment ) );
		segment1 = (Segment *)calloc( MAXSEG, sizeof( Segment ) );
		segment2 = (Segment *)calloc( MAXSEG, sizeof( Segment ) );
		sortedseg1 = (Segment **)calloc( MAXSEG, sizeof( Segment * ) );
		sortedseg2 = (Segment **)calloc( MAXSEG, sizeof( Segment * ) );
		if( !( segment && segment1 && segment2 && sortedseg1 && sortedseg2 ) )
			ErrorExit( "Allocation error\n" );

		if     ( scoremtx == -1 ) n20or4or2 = 1;
		else if( fftscore )       n20or4or2 = 1;
		else                      n20or4or2 = 20;
	}
	if( localalloclen < nlen )
	{
		if( localalloclen )
		{
#if 1
			if( !kobetsubunkatsu )
			{
				FreeFukusosuuMtx ( seqVector1 );
				FreeFukusosuuMtx ( seqVector2 );
				FreeFukusosuuVec( naisekiNoWa );
				FreeFukusosuuMtx( naiseki );
				FreeDoubleVec( soukan );
			}
			FreeCharMtx( tmpseq1 );
			FreeCharMtx( tmpseq2 );
#endif
#if RND
			FreeCharMtx( rndseq1 );
			FreeCharMtx( rndseq2 );
#endif
		}


		tmpseq1 = AllocateCharMtx( njob, nlen );
		tmpseq2 = AllocateCharMtx( njob, nlen );
		if( !kobetsubunkatsu )
		{
			naisekiNoWa = AllocateFukusosuuVec( nlen );
			naiseki = AllocateFukusosuuMtx( n20or4or2, nlen );
			seqVector1 = AllocateFukusosuuMtx( n20or4or2, nlen+1 );
			seqVector2 = AllocateFukusosuuMtx( n20or4or2, nlen+1 );
			soukan = AllocateDoubleVec( nlen+1 );
		}
#if RND
		rndseq1 = AllocateCharMtx( njob, nlen );
		rndseq2 = AllocateCharMtx( njob, nlen );
		for( i=0; i<njob; i++ )
		{
			generateRndSeq( rndseq1[i], nlen );
			generateRndSeq( rndseq2[i], nlen );
		}
#endif
		localalloclen = nlen;
	}
	
	for( j=0; j<clus1; j++ ) strcpy( tmpseq1[j], seq1[j] );
	for( j=0; j<clus2; j++ ) strcpy( tmpseq2[j], seq2[j] );

#if 0
fftfp = fopen( "input_of_Falign", "w" );
fprintf( fftfp, "nlen = %d\n", nlen );
fprintf( fftfp, "seq1: ( %d sequences ) \n", clus1 );
for( i=0; i<clus1; i++ )
	fprintf( fftfp, "%s\n", seq1[i] );
fprintf( fftfp, "seq2: ( %d sequences ) \n", clus2 );
for( i=0; i<clus2; i++ )
	fprintf( fftfp, "%s\n", seq2[i] );
fclose( fftfp );
system( "less input_of_Falign < /dev/tty > /dev/tty" );
#endif
	if( !kobetsubunkatsu )
	{
		fprintf( stderr,  " FFT ... " );

		for( j=0; j<n20or4or2; j++ ) vec_init( seqVector1[j], nlen );
		if( scoremtx == -1 )
		{
			for( i=0; i<clus1; i++ )
				seq_vec_4( seqVector1[0], eff1[i], tmpseq1[i] );
		}
		else if( fftscore )
		{
			for( i=0; i<clus1; i++ )
			{
#if 0
				seq_vec_2( seqVector1[0], polarity, eff1[i], tmpseq1[i] );
				seq_vec_2( seqVector1[1], volume,   eff1[i], tmpseq1[i] );
#else
				seq_vec_5( seqVector1[0], polarity, volume, eff1[i], tmpseq1[i] );
#endif
			}
		}
		else
		{
			for( i=0; i<clus1; i++ )
				seq_vec_3( seqVector1, eff1[i], tmpseq1[i] );
		}
#if RND
		for( i=0; i<clus1; i++ )
		{
			vec_init2( seqVector1, rndseq1[i], eff1[i], len1, nlen );
		}
#endif
#if 0
fftfp = fopen( "seqVec", "w" );
fprintf( fftfp, "before transform\n" );
for( k=0; k<n20or4or2; k++ ) 
{
   fprintf( fftfp, "nlen=%d\n", nlen );
   fprintf( fftfp, "%c\n", amino[k] );
   for( l=0; l<nlen; l++ )
   fprintf( fftfp, "%f %f\n", seqVector1[k][l].R, seqVector1[k][l].I );
}
fclose( fftfp );
system( "less seqVec < /dev/tty > /dev/tty" );
#endif

		for( j=0; j<n20or4or2; j++ ) vec_init( seqVector2[j], nlen );
		if( scoremtx == -1 )
		{
			for( i=0; i<clus2; i++ )
				seq_vec_4( seqVector2[0], eff2[i], tmpseq2[i] );
		}
		else if( fftscore )
		{
			for( i=0; i<clus2; i++ )
			{
#if 0
				seq_vec_2( seqVector2[0], polarity, eff2[i], tmpseq2[i] );
				seq_vec_2( seqVector2[1], volume,   eff2[i], tmpseq2[i] );
#else
				seq_vec_5( seqVector2[0], polarity, volume, eff2[i], tmpseq2[i] );
#endif
			}
		}
		else
		{
			for( i=0; i<clus2; i++ )
				seq_vec_3( seqVector2, eff2[i], tmpseq2[i] );
		}
#if RND
		for( i=0; i<clus2; i++ )
		{
			vec_init2( seqVector2, rndseq2[i], eff2[i], len2, nlen );
		}
#endif

#if 0
fftfp = fopen( "seqVec2", "w" );
fprintf( fftfp, "before fft\n" );
for( k=0; k<n20or4or2; k++ ) 
{
   fprintf( fftfp, "%c\n", amino[k] );
   for( l=0; l<nlen; l++ )
   fprintf( fftfp, "%f %f\n", seqVector2[k][l].R, seqVector2[k][l].I );
}
fclose( fftfp );
system( "less seqVec2 < /dev/tty > /dev/tty" );
#endif

		for( j=0; j<n20or4or2; j++ )
		{
			fft( nlen, seqVector2[j], (j==0) );
			fft( nlen, seqVector1[j], 0 );
		}
#if 0
fftfp = fopen( "seqVec2", "w" );
fprintf( fftfp, "#after fft\n" );
for( k=0; k<n20or4or2; k++ ) 
{
   fprintf( fftfp, "#%c\n", amino[k] );
   for( l=0; l<nlen; l++ )
	   fprintf( fftfp, "%f %f\n", seqVector2[k][l].R, seqVector2[k][l].I );
}
fclose( fftfp );
system( "less seqVec2 < /dev/tty > /dev/tty" );
#endif

		for( k=0; k<n20or4or2; k++ ) 
		{
			for( l=0; l<nlen; l++ ) 
				calcNaiseki( naiseki[k]+l, seqVector1[k]+l, seqVector2[k]+l );
		}
		for( l=0; l<nlen; l++ ) 
		{
			naisekiNoWa[l].R = 0.0;
			naisekiNoWa[l].I = 0.0;
			for( k=0; k<n20or4or2; k++ ) 
			{
				naisekiNoWa[l].R += naiseki[k][l].R;
				naisekiNoWa[l].I += naiseki[k][l].I;
			}
		}
	
#if 0
	fftfp = fopen( "naisekiNoWa", "w" );
	fprintf( fftfp, "#Before fft\n" );
	for( l=0; l<nlen; l++ )
		fprintf( fftfp, "%d  %f %f\n", l, naisekiNoWa[l].R, naisekiNoWa[l].I ); 
	fclose( fftfp );
	system( "less naisekiNoWa < /dev/tty > /dev/tty " );
#endif

		fft( -nlen, naisekiNoWa, 0 );
	
		for( m=0; m<=nlen2; m++ ) 
			soukan[m] = naisekiNoWa[nlen2-m].R;
		for( m=nlen2+1; m<nlen; m++ ) 
			soukan[m] = naisekiNoWa[nlen+nlen2-m].R;

#if 0
	fftfp = fopen( "naisekiNoWa", "w" );
	fprintf( fftfp, "#After fft\n" );
	for( l=0; l<nlen; l++ )
		fprintf( fftfp, "%d  %f\n", l, naisekiNoWa[l].R ); 
	fclose( fftfp );
	fftfp = fopen( "list.plot", "w"  );
	fprintf( fftfp, "plot 'naisekiNoWa'\npause -1" );
	fclose( fftfp );
	system( "/usr/bin/gnuplot list.plot &" );
#endif
#if 0
	fprintf( stderr, "soukan\n" );
	for( l=0; l<nlen; l++ )
		fprintf( stderr, "%d  %f\n", l-nlen2, soukan[l] ); 
#if 0
	fftfp = fopen( "list.plot", "w"  );
	fprintf( fftfp, "plot 'frt'\n pause +1" );
	fclose( fftfp );
	system( "/usr/bin/gnuplot list.plot" );
#endif
#endif


		nkouho = getKouho( kouho, NKOUHO_LONG, soukan, nlen );

#if 0
		for( i=0; i<nkouho; i++ )
		{
			fprintf( stderr, "kouho[%d] = %d\n", i, kouho[i] );
		}
#endif
	}

#if KEIKA
	fprintf( stderr, "Searching anchors ... " );
#endif
	count = 0;



#define CAND 0
#if CAND
	fftfp = fopen( "cand", "w" );
	fclose( fftfp );
#endif
	if( kobetsubunkatsu )
	{
		maxk = 1;
		kouho[0] = 0;
	}
	else
	{
		maxk = nkouho;
	}

	for( k=0; k<maxk; k++ ) 
	{
		lag = kouho[k];
		if( lag <= -len1 || len2 <= lag ) continue;
//		fprintf( stderr, "k=%d, lag=%d\n", k, lag );
		zurasu2( lag, clus1, clus2, seq1, seq2, tmpptr1, tmpptr2 );
#if CAND
		fftfp = fopen( "cand", "a" );
		fprintf( fftfp, ">Candidate No.%d lag = %d\n", k+1, lag );
		fprintf( fftfp, "%s\n", tmpptr1[0] );
		fprintf( fftfp, ">Candidate No.%d lag = %d\n", k+1, lag );
		fprintf( fftfp, "%s\n", tmpptr2[0] );
		fprintf( fftfp, ">\n", k+1, lag );
		fclose( fftfp );
#endif

//		fprintf( stderr, "lag = %d\n", lag );
		tmpint = alignableReagion( clus1, clus2, tmpptr1, tmpptr2, eff1, eff2, segment+count );
//		fprintf( stderr, "lag = %d, %d found\n", lag, tmpint );

//		if( lag == -50 ) exit( 1 );
		
		if( count+tmpint > MAXSEG -3 ) ErrorExit( "TOO MANY SEGMENTS.\n" );

//		fprintf( stderr, "##### k=%d / %d\n", k, maxk );
		if( tmpint == 0 ) break; // 060430 iinoka ?
		while( tmpint-- > 0 )
		{
#if 0
			if( segment[count].end - segment[count].start < fftWinSize )
			{
				count++;
				continue;
			}
#endif
			if( lag > 0 )
			{
				segment1[count].start  = segment[count].start ;
				segment1[count].end    = segment[count].end   ;
				segment1[count].center = segment[count].center;
				segment1[count].score  = segment[count].score;

				segment2[count].start  = segment[count].start  + lag;
				segment2[count].end    = segment[count].end    + lag;
				segment2[count].center = segment[count].center + lag;
				segment2[count].score  = segment[count].score       ;
			}
			else
			{
				segment1[count].start  = segment[count].start  - lag;
				segment1[count].end    = segment[count].end    - lag;
				segment1[count].center = segment[count].center - lag;
				segment1[count].score  = segment[count].score       ;

				segment2[count].start  = segment[count].start ;
				segment2[count].end    = segment[count].end   ;
				segment2[count].center = segment[count].center;
				segment2[count].score  = segment[count].score ;
			}
#if 0
			fprintf( stderr, "##### k=%d / %d\n", k, maxk );
			fprintf( stderr, "anchor %d, score = %f\n", count, segment1[count].score );
			fprintf( stderr, "in 1 %d\n", segment1[count].center );
			fprintf( stderr, "in 2 %d\n", segment2[count].center );
#endif
			segment1[count].pair = &segment2[count];
			segment2[count].pair = &segment1[count];
			count++;
#if 0
			fprintf( stderr, "count=%d\n", count );
#endif
		}
	}
#if 1
	if( !kobetsubunkatsu )
		fprintf( stderr, "done. (%d anchors) ", count );
#endif
	if( !count && fftNoAnchStop )
		ErrorExit( "Cannot detect anchor!" );
#if 0
	fprintf( stderr, "RESULT before sort:\n" );
	for( l=0; l<count+1; l++ )
	{
		fprintf( stderr, "cut[%d]=%d, ", l, segment1[l].center );
		fprintf( stderr, "%d score = %f\n", segment2[l].center, segment1[l].score );
	}
#endif

	for( i=0; i<count; i++ )
	{
		sortedseg1[i] = &segment1[i];
		sortedseg2[i] = &segment2[i];
	}
#if 0
	tmpsort( count, sortedseg1 ); 
	tmpsort( count, sortedseg2 ); 
	qsort( sortedseg1, count, sizeof( Segment * ), segcmp );
	qsort( sortedseg2, count, sizeof( Segment * ), segcmp );
#else
	mymergesort( 0, count-1, sortedseg1 ); 
	mymergesort( 0, count-1, sortedseg2 ); 
#endif
	for( i=0; i<count; i++ ) sortedseg1[i]->number = i;
	for( i=0; i<count; i++ ) sortedseg2[i]->number = i;



	if( kobetsubunkatsu )
	{
		for( i=0; i<count; i++ )
	    {
			cut1[i+1] = sortedseg1[i]->center;
			cut2[i+1] = sortedseg2[i]->center;
		}
		cut1[0] = 0;
		cut2[0] = 0;
		cut1[count+1] = len1;
		cut2[count+1] = len2;
		count += 2;
	}
	else
	{
		cut1[0] = 0; 
		cut2[0] = 0;
		count0 = 0;
		for( i=0; i<count; i++ )
		{
//			fprintf( stderr, "i=%d, %d-%d ?\n", i, sortedseg1[i]->center, sortedseg1[i]->pair->center );
			if( sortedseg1[i]->center > cut1[count0]
			 && sortedseg1[i]->pair->center > cut2[count0] )
			{
				count0++;
				cut1[count0] = sortedseg1[i]->center;
				cut2[count0] = sortedseg1[i]->pair->center;
			}
			else
			{
				if( i && sortedseg1[i]->score > sortedseg1[i-1]->score )
				{
					if( sortedseg1[i]->center > cut1[count0-1]
					 && sortedseg1[i]->pair->center > cut2[count0-1] )
					{
						cut1[count0] = sortedseg1[i]->center;
						cut2[count0] = sortedseg1[i]->pair->center;
					}
					else
					{
//						count0--;
					}
				}
			}
		}
//		if( count-count0 )
//			fprintf( stderr, "%d anchors unused\n", count-count0 );
		cut1[count0+1] = len1;
		cut2[count0+1] = len2;
		count = count0 + 2;
		count0 = count;

	}

//	exit( 0 );

#if 0
	fftfp = fopen( "fft", "a" );
	fprintf( fftfp, "RESULT after sort:\n" );
	for( l=0; l<count; l++ )
	{
		fprintf( fftfp, "cut[%d]=%d, ", l, segment1[l].center );
		fprintf( fftfp, "%d\n", segment2[l].center );
	}
	fclose( fftfp );
#endif

#if 0
	fprintf( stderr, "RESULT after blckalign:\n" );
	for( l=0; l<count+1; l++ )
	{
		fprintf( stderr, "cut : %d %d\n", cut1[l], cut2[l] );
	}
#endif

#if 0
	fprintf( trap_g, "Devided to %d segments\n", count-1 );
	fprintf( trap_g, "%d  %d forg\n", MIN( clus1, clus2 ), count-1 );
#endif

	totallen = 0;
	for( j=0; j<clus1; j++ ) result1[j][0] = 0;
	for( j=0; j<clus2; j++ ) result2[j][0] = 0;
	totalscore = 0.0;
	*fftlog = -1;
	for( i=0; i<count-1; i++ )
	{
		*fftlog += 1;

		if( cut1[i] )
		{
//			getkyokaigap( sgap1, seq1, cut1[i]-1, clus1 );
//			getkyokaigap( sgap2, seq2, cut2[i]-1, clus2 );
			getkyokaigap( sgap1, tmpres1, nlen-1, clus1 );
			getkyokaigap( sgap2, tmpres2, nlen-1, clus2 );
		}
		else
		{
			for( j=0; j<clus1; j++ ) sgap1[j] = 'o';
			for( j=0; j<clus2; j++ ) sgap2[j] = 'o';
		}
		if( cut1[i+1] != len1 )
		{       
			getkyokaigap( egap1, seq1, cut1[i+1], clus1 );
			getkyokaigap( egap2, seq2, cut2[i+1], clus2 );
		}       
		else    
		{       
			for( j=0; j<clus1; j++ ) egap1[j] = 'o';
			for( j=0; j<clus2; j++ ) egap2[j] = 'o';
		}
#if DEBUG
		fprintf( stderr, "DP %03d / %03d %4d to ", i+1, count-1, totallen );
#else
#if 1
		fprintf( stderr, "DP %05d / %05d \b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b", i+1, count-1 );
#endif
#endif
		for( j=0; j<clus1; j++ )
		{
			strncpy( tmpres1[j], seq1[j]+cut1[i], cut1[i+1]-cut1[i] );
			tmpres1[j][cut1[i+1]-cut1[i]] = 0;
		}
		if( kobetsubunkatsu && fftkeika ) commongappick( clus1, tmpres1 ); //dvtditr に呼ばれたとき fftkeika=1
//		if( kobetsubunkatsu ) commongappick( clus1, tmpres1 );
		for( j=0; j<clus2; j++ )
		{
//			fprintf( stderr, "### cut2[i+1]-cut2[i] = %d\n", cut2[i+1]-cut2[i] );
			if( cut2[i+1]-cut2[i] <= 0 )
				fprintf( stderr, "### cut2[i+1]=%d, cut2[i]=%d\n", cut2[i+1], cut2[i] );
			strncpy( tmpres2[j], seq2[j]+cut2[i], cut2[i+1]-cut2[i] );
			tmpres2[j][cut2[i+1]-cut2[i]] = 0;
		}
		if( kobetsubunkatsu && fftkeika ) commongappick( clus2, tmpres2 ); //dvtditr に呼ばれたとき fftkeika=1
//		if( kobetsubunkatsu ) commongappick( clus2, tmpres2 );

		if( constraint )
		{
			fprintf( stderr, "Not supported\n" );
			exit( 1 );
		}
#if 0
		fprintf( stderr, "i=%d, before alignment", i );
		fprintf( stderr, "%4d\n", totallen );
		fprintf( stderr, "\n\n" );
		for( j=0; j<clus1; j++ ) 
		{
			fprintf( stderr, "%s\n", tmpres1[j] );
		}
		fprintf( stderr, "-------\n" );
		for( j=0; j<clus2; j++ ) 
		{
			fprintf( stderr, "%s\n", tmpres2[j] );
		}
#endif

#if 0
		fprintf( stdout, "writing input\n" );
		for( j=0; j<clus1; j++ )
		{
			fprintf( stdout, ">%d of GROUP1\n", j );
			fprintf( stdout, "%s\n", tmpres1[j] );
		}
		for( j=0; j<clus2; j++ )
		{
			fprintf( stdout, ">%d of GROUP2\n", j );
			fprintf( stdout, "%s\n", tmpres2[j] );
		}
		fflush( stdout );
#endif
		switch( alg )
		{
			case( 'M' ):
					totalscore += MSalignmm( tmpres1, tmpres2, eff1, eff2, clus1, clus2, alloclen, sgap1, sgap2, egap1, egap2 );
				break;
			default:
				fprintf( stderr, "alg = %c\n", alg );
				ErrorExit( "ERROR IN SOURCE FILE Falign.c" );
				break;
		}

		nlen = strlen( tmpres1[0] );
		if( totallen + nlen > alloclen )
		{
			fprintf( stderr, "totallen=%d +  nlen=%d > alloclen = %d\n", totallen, nlen, alloclen );
			ErrorExit( "LENGTH OVER in Falign\n " );
		}
		for( j=0; j<clus1; j++ ) strcat( result1[j], tmpres1[j] );
		for( j=0; j<clus2; j++ ) strcat( result2[j], tmpres2[j] );
		totallen += nlen;
#if 0
		fprintf( stderr, "i=%d", i );
		fprintf( stderr, "%4d\n", totallen );
		fprintf( stderr, "\n\n" );
		for( j=0; j<clus1; j++ ) 
		{
			fprintf( stderr, "%s\n", tmpres1[j] );
		}
		fprintf( stderr, "-------\n" );
		for( j=0; j<clus2; j++ ) 
		{
			fprintf( stderr, "%s\n", tmpres2[j] );
		}
#endif
	}
#if KEIKA
	fprintf( stderr, "DP ... done   \n" );
#endif

	for( j=0; j<clus1; j++ ) strcpy( seq1[j], result1[j] );
	for( j=0; j<clus2; j++ ) strcpy( seq2[j], result2[j] );
#if 0
	for( j=0; j<clus1; j++ ) 
	{
		fprintf( stderr, "%s\n", result1[j] );
	}
	fprintf( stderr, "- - - - - - - - - - -\n" );
	for( j=0; j<clus2; j++ ) 
	{
		fprintf( stderr, "%s\n", result2[j] );
	}
#endif
	return( totalscore );
}

float Falign_udpari_long( char  **seq1, char  **seq2, 
			  double *eff1, double *eff2, 
			  int    clus1, int    clus2,
			  int alloclen, int *fftlog )
{
	int i, j, k, l, m, maxk;
	int nlen, nlen2, nlen4;
	static int prevalloclen = 0;
	static int crossscoresize = 0;
	static char **tmpseq1 = NULL;
	static char **tmpseq2 = NULL;
	static char **tmpptr1 = NULL;
	static char **tmpptr2 = NULL;
	static char **tmpres1 = NULL;
	static char **tmpres2 = NULL;
	static char **result1 = NULL;
	static char **result2 = NULL;
#if RND
	static char **rndseq1 = NULL;
	static char **rndseq2 = NULL;
#endif
	static Fukusosuu **seqVector1 = NULL;
	static Fukusosuu **seqVector2 = NULL;
	static Fukusosuu **naiseki = NULL;   
	static Fukusosuu *naisekiNoWa = NULL; 
	static double *soukan = NULL;
	static double **crossscore = NULL;
	int nlentmp;
	static int *kouho = NULL;
	static Segment *segment = NULL;
	static Segment *segment1 = NULL;
	static Segment *segment2 = NULL;
	static Segment **sortedseg1 = NULL;
	static Segment **sortedseg2 = NULL;
	static int *cut1 = NULL;
	static int *cut2 = NULL;
	static char *sgap1, *egap1, *sgap2, *egap2;
	static int localalloclen = 0;
	int lag;
	int tmpint;
	int count, count0;
	int len1, len2;
	int totallen;
	float totalscore;
	int nkouho;
//	float dumfl = 0.0;


	len1 = strlen( seq1[0] );
	len2 = strlen( seq2[0] );
	nlentmp = MAX( len1, len2 );

	nlen = 1;
	while( nlentmp >= nlen ) nlen <<= 1;
#if 0
	fprintf( stderr, "###   nlen    = %d\n", nlen );
#endif

	nlen2 = nlen/2; nlen4 = nlen2 / 2;

#if 0
	fprintf( stderr, "len1 = %d, len2 = %d\n", len1, len2 );
	fprintf( stderr, "nlentmp = %d, nlen = %d\n", nlentmp, nlen );
#endif

	if( prevalloclen != alloclen ) // Falign_noudp mo kaeru
	{
		if( prevalloclen )
		{
			FreeCharMtx( result1 );
			FreeCharMtx( result2 );
			FreeCharMtx( tmpres1 );
			FreeCharMtx( tmpres2 );
		}
//		fprintf( stderr, "\n\n\nreallocating ...\n" ); 
		result1 = AllocateCharMtx( njob, alloclen );
		result2 = AllocateCharMtx( njob, alloclen );
		tmpres1 = AllocateCharMtx( njob, alloclen );
		tmpres2 = AllocateCharMtx( njob, alloclen );
		prevalloclen = alloclen;
	}

	if( !localalloclen )
	{
		sgap1 = AllocateCharVec( njob );
		egap1 = AllocateCharVec( njob );
		sgap2 = AllocateCharVec( njob );
		egap2 = AllocateCharVec( njob );
		kouho = AllocateIntVec( NKOUHO_LONG );
		cut1 = AllocateIntVec( MAXSEG );
		cut2 = AllocateIntVec( MAXSEG );
		tmpptr1 = AllocateCharMtx( njob, 0 );
		tmpptr2 = AllocateCharMtx( njob, 0 );
		segment = (Segment *)calloc( MAXSEG, sizeof( Segment ) );
		segment1 = (Segment *)calloc( MAXSEG, sizeof( Segment ) );
		segment2 = (Segment *)calloc( MAXSEG, sizeof( Segment ) );
		sortedseg1 = (Segment **)calloc( MAXSEG, sizeof( Segment * ) );
		sortedseg2 = (Segment **)calloc( MAXSEG, sizeof( Segment * ) );
		if( !( segment && segment1 && segment2 && sortedseg1 && sortedseg2 ) )
			ErrorExit( "Allocation error\n" );

		if     ( scoremtx == -1 ) n20or4or2 = 1;
		else if( fftscore )       n20or4or2 = 1;
		else                      n20or4or2 = 20;
	}
	if( localalloclen < nlen )
	{
		if( localalloclen )
		{
#if 1
			if( !kobetsubunkatsu )
			{
				FreeFukusosuuMtx ( seqVector1 );
				FreeFukusosuuMtx ( seqVector2 );
				FreeFukusosuuVec( naisekiNoWa );
				FreeFukusosuuMtx( naiseki );
				FreeDoubleVec( soukan );
			}
			FreeCharMtx( tmpseq1 );
			FreeCharMtx( tmpseq2 );
#endif
#if RND
			FreeCharMtx( rndseq1 );
			FreeCharMtx( rndseq2 );
#endif
		}


		tmpseq1 = AllocateCharMtx( njob, nlen );
		tmpseq2 = AllocateCharMtx( njob, nlen );
		if( !kobetsubunkatsu )
		{
			naisekiNoWa = AllocateFukusosuuVec( nlen );
			naiseki = AllocateFukusosuuMtx( n20or4or2, nlen );
			seqVector1 = AllocateFukusosuuMtx( n20or4or2, nlen+1 );
			seqVector2 = AllocateFukusosuuMtx( n20or4or2, nlen+1 );
			soukan = AllocateDoubleVec( nlen+1 );
		}
#if RND
		rndseq1 = AllocateCharMtx( njob, nlen );
		rndseq2 = AllocateCharMtx( njob, nlen );
		for( i=0; i<njob; i++ )
		{
			generateRndSeq( rndseq1[i], nlen );
			generateRndSeq( rndseq2[i], nlen );
		}
#endif
		localalloclen = nlen;
	}
	
	for( j=0; j<clus1; j++ ) strcpy( tmpseq1[j], seq1[j] );
	for( j=0; j<clus2; j++ ) strcpy( tmpseq2[j], seq2[j] );

#if 0
fftfp = fopen( "input_of_Falign", "w" );
fprintf( fftfp, "nlen = %d\n", nlen );
fprintf( fftfp, "seq1: ( %d sequences ) \n", clus1 );
for( i=0; i<clus1; i++ )
	fprintf( fftfp, "%s\n", seq1[i] );
fprintf( fftfp, "seq2: ( %d sequences ) \n", clus2 );
for( i=0; i<clus2; i++ )
	fprintf( fftfp, "%s\n", seq2[i] );
fclose( fftfp );
system( "less input_of_Falign < /dev/tty > /dev/tty" );
#endif
	if( !kobetsubunkatsu )
	{
		fprintf( stderr,  " FFT ... " );

		for( j=0; j<n20or4or2; j++ ) vec_init( seqVector1[j], nlen );
		if( scoremtx == -1 )
		{
			for( i=0; i<clus1; i++ )
				seq_vec_4( seqVector1[0], eff1[i], tmpseq1[i] );
		}
		else if( fftscore )
		{
			for( i=0; i<clus1; i++ )
			{
#if 0
				seq_vec_2( seqVector1[0], polarity, eff1[i], tmpseq1[i] );
				seq_vec_2( seqVector1[1], volume,   eff1[i], tmpseq1[i] );
#else
				seq_vec_5( seqVector1[0], polarity, volume, eff1[i], tmpseq1[i] );
#endif
			}
		}
		else
		{
			for( i=0; i<clus1; i++ )
				seq_vec_3( seqVector1, eff1[i], tmpseq1[i] );
		}
#if RND
		for( i=0; i<clus1; i++ )
		{
			vec_init2( seqVector1, rndseq1[i], eff1[i], len1, nlen );
		}
#endif
#if 0
fftfp = fopen( "seqVec", "w" );
fprintf( fftfp, "before transform\n" );
for( k=0; k<n20or4or2; k++ ) 
{
   fprintf( fftfp, "nlen=%d\n", nlen );
   fprintf( fftfp, "%c\n", amino[k] );
   for( l=0; l<nlen; l++ )
   fprintf( fftfp, "%f %f\n", seqVector1[k][l].R, seqVector1[k][l].I );
}
fclose( fftfp );
system( "less seqVec < /dev/tty > /dev/tty" );
#endif

		for( j=0; j<n20or4or2; j++ ) vec_init( seqVector2[j], nlen );
		if( scoremtx == -1 )
		{
			for( i=0; i<clus2; i++ )
				seq_vec_4( seqVector2[0], eff2[i], tmpseq2[i] );
		}
		else if( fftscore )
		{
			for( i=0; i<clus2; i++ )
			{
#if 0
				seq_vec_2( seqVector2[0], polarity, eff2[i], tmpseq2[i] );
				seq_vec_2( seqVector2[1], volume,   eff2[i], tmpseq2[i] );
#else
				seq_vec_5( seqVector2[0], polarity, volume, eff2[i], tmpseq2[i] );
#endif
			}
		}
		else
		{
			for( i=0; i<clus2; i++ )
				seq_vec_3( seqVector2, eff2[i], tmpseq2[i] );
		}
#if RND
		for( i=0; i<clus2; i++ )
		{
			vec_init2( seqVector2, rndseq2[i], eff2[i], len2, nlen );
		}
#endif

#if 0
fftfp = fopen( "seqVec2", "w" );
fprintf( fftfp, "before fft\n" );
for( k=0; k<n20or4or2; k++ ) 
{
   fprintf( fftfp, "%c\n", amino[k] );
   for( l=0; l<nlen; l++ )
   fprintf( fftfp, "%f %f\n", seqVector2[k][l].R, seqVector2[k][l].I );
}
fclose( fftfp );
system( "less seqVec2 < /dev/tty > /dev/tty" );
#endif

		for( j=0; j<n20or4or2; j++ )
		{
			fft( nlen, seqVector2[j], (j==0) );
			fft( nlen, seqVector1[j], 0 );
		}
#if 0
fftfp = fopen( "seqVec2", "w" );
fprintf( fftfp, "#after fft\n" );
for( k=0; k<n20or4or2; k++ ) 
{
   fprintf( fftfp, "#%c\n", amino[k] );
   for( l=0; l<nlen; l++ )
	   fprintf( fftfp, "%f %f\n", seqVector2[k][l].R, seqVector2[k][l].I );
}
fclose( fftfp );
system( "less seqVec2 < /dev/tty > /dev/tty" );
#endif

		for( k=0; k<n20or4or2; k++ ) 
		{
			for( l=0; l<nlen; l++ ) 
				calcNaiseki( naiseki[k]+l, seqVector1[k]+l, seqVector2[k]+l );
		}
		for( l=0; l<nlen; l++ ) 
		{
			naisekiNoWa[l].R = 0.0;
			naisekiNoWa[l].I = 0.0;
			for( k=0; k<n20or4or2; k++ ) 
			{
				naisekiNoWa[l].R += naiseki[k][l].R;
				naisekiNoWa[l].I += naiseki[k][l].I;
			}
		}
	
#if 0
	fftfp = fopen( "naisekiNoWa", "w" );
	fprintf( fftfp, "#Before fft\n" );
	for( l=0; l<nlen; l++ )
		fprintf( fftfp, "%d  %f %f\n", l, naisekiNoWa[l].R, naisekiNoWa[l].I ); 
	fclose( fftfp );
	system( "less naisekiNoWa < /dev/tty > /dev/tty " );
#endif

		fft( -nlen, naisekiNoWa, 0 );
	
		for( m=0; m<=nlen2; m++ ) 
			soukan[m] = naisekiNoWa[nlen2-m].R;
		for( m=nlen2+1; m<nlen; m++ ) 
			soukan[m] = naisekiNoWa[nlen+nlen2-m].R;

#if 0
	fftfp = fopen( "naisekiNoWa", "w" );
	fprintf( fftfp, "#After fft\n" );
	for( l=0; l<nlen; l++ )
		fprintf( fftfp, "%d  %f\n", l, naisekiNoWa[l].R ); 
	fclose( fftfp );
	fftfp = fopen( "list.plot", "w"  );
	fprintf( fftfp, "plot 'naisekiNoWa'\npause -1" );
	fclose( fftfp );
	system( "/usr/bin/gnuplot list.plot &" );
#endif
#if 0
	fprintf( stderr, "soukan\n" );
	for( l=0; l<nlen; l++ )
		fprintf( stderr, "%d  %f\n", l-nlen2, soukan[l] ); 
#if 0
	fftfp = fopen( "list.plot", "w"  );
	fprintf( fftfp, "plot 'frt'\n pause +1" );
	fclose( fftfp );
	system( "/usr/bin/gnuplot list.plot" );
#endif
#endif


		nkouho = getKouho( kouho, NKOUHO_LONG, soukan, nlen );

#if 0
		for( i=0; i<nkouho; i++ )
		{
			fprintf( stderr, "kouho[%d] = %d\n", i, kouho[i] );
		}
#endif
	}

#if KEIKA
	fprintf( stderr, "Searching anchors ... " );
#endif
	count = 0;



#define CAND 0
#if CAND
	fftfp = fopen( "cand", "w" );
	fclose( fftfp );
#endif
	if( kobetsubunkatsu )
	{
		maxk = 1;
		kouho[0] = 0;
	}
	else
	{
		maxk = nkouho;
	}

	for( k=0; k<maxk; k++ ) 
	{
		lag = kouho[k];
		if( lag <= -len1 || len2 <= lag ) continue;
//		fprintf( stderr, "k=%d, lag=%d\n", k, lag );
		zurasu2( lag, clus1, clus2, seq1, seq2, tmpptr1, tmpptr2 );
#if CAND
		fftfp = fopen( "cand", "a" );
		fprintf( fftfp, ">Candidate No.%d lag = %d\n", k+1, lag );
		fprintf( fftfp, "%s\n", tmpptr1[0] );
		fprintf( fftfp, ">Candidate No.%d lag = %d\n", k+1, lag );
		fprintf( fftfp, "%s\n", tmpptr2[0] );
		fprintf( fftfp, ">\n", k+1, lag );
		fclose( fftfp );
#endif

//		fprintf( stderr, "lag = %d\n", lag );
		tmpint = alignableReagion( clus1, clus2, tmpptr1, tmpptr2, eff1, eff2, segment+count );
//		fprintf( stderr, "lag = %d, %d found\n", lag, tmpint );

//		if( lag == -50 ) exit( 1 );
		
		if( count+tmpint > MAXSEG -3 ) ErrorExit( "TOO MANY SEGMENTS.\n" );

//		fprintf( stderr, "##### k=%d / %d\n", k, maxk );
		if( tmpint == 0 ) break; // 060430 iinoka ?
		while( tmpint-- > 0 )
		{
#if 0
			if( segment[count].end - segment[count].start < fftWinSize )
			{
				count++;
				continue;
			}
#endif
			if( lag > 0 )
			{
				segment1[count].start  = segment[count].start ;
				segment1[count].end    = segment[count].end   ;
				segment1[count].center = segment[count].center;
				segment1[count].score  = segment[count].score;

				segment2[count].start  = segment[count].start  + lag;
				segment2[count].end    = segment[count].end    + lag;
				segment2[count].center = segment[count].center + lag;
				segment2[count].score  = segment[count].score       ;
			}
			else
			{
				segment1[count].start  = segment[count].start  - lag;
				segment1[count].end    = segment[count].end    - lag;
				segment1[count].center = segment[count].center - lag;
				segment1[count].score  = segment[count].score       ;

				segment2[count].start  = segment[count].start ;
				segment2[count].end    = segment[count].end   ;
				segment2[count].center = segment[count].center;
				segment2[count].score  = segment[count].score ;
			}
#if 0
			fprintf( stderr, "##### k=%d / %d\n", k, maxk );
			fprintf( stderr, "anchor %d, score = %f\n", count, segment1[count].score );
			fprintf( stderr, "in 1 %d\n", segment1[count].center );
			fprintf( stderr, "in 2 %d\n", segment2[count].center );
#endif
			segment1[count].pair = &segment2[count];
			segment2[count].pair = &segment1[count];
			count++;
#if 0
			fprintf( stderr, "count=%d\n", count );
#endif
		}
	}
#if 1
	if( !kobetsubunkatsu )
		fprintf( stderr, "done. (%d anchors) ", count );
#endif
	if( !count && fftNoAnchStop )
		ErrorExit( "Cannot detect anchor!" );
#if 0
	fprintf( stderr, "RESULT before sort:\n" );
	for( l=0; l<count+1; l++ )
	{
		fprintf( stderr, "cut[%d]=%d, ", l, segment1[l].center );
		fprintf( stderr, "%d score = %f\n", segment2[l].center, segment1[l].score );
	}
#endif

	for( i=0; i<count; i++ )
	{
		sortedseg1[i] = &segment1[i];
		sortedseg2[i] = &segment2[i];
	}
#if 0
	tmpsort( count, sortedseg1 ); 
	tmpsort( count, sortedseg2 ); 
	qsort( sortedseg1, count, sizeof( Segment * ), segcmp );
	qsort( sortedseg2, count, sizeof( Segment * ), segcmp );
#else
	mymergesort( 0, count-1, sortedseg1 ); 
	mymergesort( 0, count-1, sortedseg2 ); 
#endif
	for( i=0; i<count; i++ ) sortedseg1[i]->number = i;
	for( i=0; i<count; i++ ) sortedseg2[i]->number = i;



	if( kobetsubunkatsu )
	{
		for( i=0; i<count; i++ )
	    {
			cut1[i+1] = sortedseg1[i]->center;
			cut2[i+1] = sortedseg2[i]->center;
		}
		cut1[0] = 0;
		cut2[0] = 0;
		cut1[count+1] = len1;
		cut2[count+1] = len2;
		count += 2;
	}
#if 1

	else
	{
		if( crossscoresize < count+2 )
		{
			crossscoresize = count+2;
#if 1
			if( fftkeika ) fprintf( stderr, "######allocating crossscore, size = %d\n", crossscoresize );
#endif
			if( crossscore ) FreeDoubleMtx( crossscore );
			crossscore = AllocateDoubleMtx( crossscoresize, crossscoresize );
		}
		for( i=0; i<count+2; i++ ) for( j=0; j<count+2; j++ )
			crossscore[i][j] = 0.0;
		for( i=0; i<count; i++ )
		{
			crossscore[segment1[i].number+1][segment1[i].pair->number+1] = segment1[i].score;
			cut1[i+1] = sortedseg1[i]->center;
			cut2[i+1] = sortedseg2[i]->center;
		}

#if 0
		fprintf( stderr, "AFTER SORT\n" );
		for( i=0; i<count+1; i++ ) fprintf( stderr, "%d, %d\n", cut1[i], cut2[i] );
		fprintf( stderr, "crossscore = \n" );
		for( i=0; i<count+1; i++ )
		{
			for( j=0; j<count+1; j++ )
				fprintf( stderr, "%.0f ", crossscore[i][j] );
			fprintf( stderr, "\n" );
		}
#endif

		crossscore[0][0] = 10000000.0;
		cut1[0] = 0; 
		cut2[0] = 0;
		crossscore[count+1][count+1] = 10000000.0;
		cut1[count+1] = len1;
		cut2[count+1] = len2;
		count += 2;
		count0 = count;
	
//		fprintf( stderr, "\n\n\ncalling blockAlign2\n\n\n\n" );
		blockAlign2( cut1, cut2, sortedseg1, sortedseg2, crossscore, &count );

//		if( count-count0 )
//			fprintf( stderr, "%d unused anchors\n", count0-count );

		if( !kobetsubunkatsu && fftkeika )
			fprintf( stderr, "%d anchors found\n", count );
		if( fftkeika )
		{
			if( count0 > count )
			{
#if 0
				fprintf( stderr, "\7 REPEAT!? \n" ); 
#else
				fprintf( stderr, "REPEAT!? \n" ); 
#endif
				if( fftRepeatStop ) exit( 1 );
			}
#if KEIKA
			else fprintf( stderr, "done\n" );
#endif
		}
	}


#else
	else
	{
		cut1[0] = 0; 
		cut2[0] = 0;
		count0 = 0;
		for( i=0; i<count; i++ )
		{
//			fprintf( stderr, "i=%d, %d-%d ?\n", i, sortedseg1[i]->center, sortedseg1[i]->pair->center );
			if( sortedseg1[i]->center > cut1[count0]
			 && sortedseg1[i]->pair->center > cut2[count0] )
			{
				count0++;
				cut1[count0] = sortedseg1[i]->center;
				cut2[count0] = sortedseg1[i]->pair->center;
			}
			else
			{
				if( i && sortedseg1[i]->score > sortedseg1[i-1]->score )
				{
					if( sortedseg1[i]->center > cut1[count0-1]
					 && sortedseg1[i]->pair->center > cut2[count0-1] )
					{
						cut1[count0] = sortedseg1[i]->center;
						cut2[count0] = sortedseg1[i]->pair->center;
					}
					else
					{
//						count0--;
					}
				}
			}
		}
//		if( count-count0 )
//			fprintf( stderr, "%d anchors unused\n", count-count0 );
		cut1[count0+1] = len1;
		cut2[count0+1] = len2;
		count = count0 + 2;
		count0 = count;

	}
#endif

//	exit( 0 );

#if 0
	fftfp = fopen( "fft", "a" );
	fprintf( fftfp, "RESULT after sort:\n" );
	for( l=0; l<count; l++ )
	{
		fprintf( fftfp, "cut[%d]=%d, ", l, segment1[l].center );
		fprintf( fftfp, "%d\n", segment2[l].center );
	}
	fclose( fftfp );
#endif

#if 0
	fprintf( stderr, "RESULT after blckalign:\n" );
	for( l=0; l<count+1; l++ )
	{
		fprintf( stderr, "cut : %d %d\n", cut1[l], cut2[l] );
	}
#endif

#if 0
	fprintf( trap_g, "Devided to %d segments\n", count-1 );
	fprintf( trap_g, "%d  %d forg\n", MIN( clus1, clus2 ), count-1 );
#endif

	totallen = 0;
	for( j=0; j<clus1; j++ ) result1[j][0] = 0;
	for( j=0; j<clus2; j++ ) result2[j][0] = 0;
	totalscore = 0.0;
	*fftlog = -1;
	for( i=0; i<count-1; i++ )
	{
		*fftlog += 1;

		if( cut1[i] )
		{
//			getkyokaigap( sgap1, seq1, cut1[i]-1, clus1 );
//			getkyokaigap( sgap2, seq2, cut2[i]-1, clus2 );
			getkyokaigap( sgap1, tmpres1, nlen-1, clus1 );
			getkyokaigap( sgap2, tmpres2, nlen-1, clus2 );
		}
		else
		{
			for( j=0; j<clus1; j++ ) sgap1[j] = 'o';
			for( j=0; j<clus2; j++ ) sgap2[j] = 'o';
		}
		if( cut1[i+1] != len1 )
		{       
			getkyokaigap( egap1, seq1, cut1[i+1], clus1 );
			getkyokaigap( egap2, seq2, cut2[i+1], clus2 );
		}       
		else    
		{       
			for( j=0; j<clus1; j++ ) egap1[j] = 'o';
			for( j=0; j<clus2; j++ ) egap2[j] = 'o';
		}
#if DEBUG
		fprintf( stderr, "DP %03d / %03d %4d to ", i+1, count-1, totallen );
#else
#if 1
		fprintf( stderr, "DP %05d / %05d \b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b", i+1, count-1 );
#endif
#endif
		for( j=0; j<clus1; j++ )
		{
			strncpy( tmpres1[j], seq1[j]+cut1[i], cut1[i+1]-cut1[i] );
			tmpres1[j][cut1[i+1]-cut1[i]] = 0;
		}
		if( kobetsubunkatsu && fftkeika ) commongappick( clus1, tmpres1 ); //dvtditr に呼ばれたとき fftkeika=1
//		if( kobetsubunkatsu ) commongappick( clus1, tmpres1 );
		for( j=0; j<clus2; j++ )
		{
//			fprintf( stderr, "### cut2[i+1]-cut2[i] = %d\n", cut2[i+1]-cut2[i] );
			if( cut2[i+1]-cut2[i] <= 0 )
				fprintf( stderr, "### cut2[i+1]=%d, cut2[i]=%d\n", cut2[i+1], cut2[i] );
			strncpy( tmpres2[j], seq2[j]+cut2[i], cut2[i+1]-cut2[i] );
			tmpres2[j][cut2[i+1]-cut2[i]] = 0;
		}
		if( kobetsubunkatsu && fftkeika ) commongappick( clus2, tmpres2 ); //dvtditr に呼ばれたとき fftkeika=1
//		if( kobetsubunkatsu ) commongappick( clus2, tmpres2 );

		if( constraint )
		{
			fprintf( stderr, "Not supported\n" );
			exit( 1 );
		}
#if 0
		fprintf( stderr, "i=%d, before alignment", i );
		fprintf( stderr, "%4d\n", totallen );
		fprintf( stderr, "\n\n" );
		for( j=0; j<clus1; j++ ) 
		{
			fprintf( stderr, "%s\n", tmpres1[j] );
		}
		fprintf( stderr, "-------\n" );
		for( j=0; j<clus2; j++ ) 
		{
			fprintf( stderr, "%s\n", tmpres2[j] );
		}
#endif

#if 0
		fprintf( stdout, "writing input\n" );
		for( j=0; j<clus1; j++ )
		{
			fprintf( stdout, ">%d of GROUP1\n", j );
			fprintf( stdout, "%s\n", tmpres1[j] );
		}
		for( j=0; j<clus2; j++ )
		{
			fprintf( stdout, ">%d of GROUP2\n", j );
			fprintf( stdout, "%s\n", tmpres2[j] );
		}
		fflush( stdout );
#endif
		switch( alg )
		{
			case( 'M' ):
					totalscore += MSalignmm( tmpres1, tmpres2, eff1, eff2, clus1, clus2, alloclen, sgap1, sgap2, egap1, egap2 );
				break;
			default:
				fprintf( stderr, "alg = %c\n", alg );
				ErrorExit( "ERROR IN SOURCE FILE Falign.c" );
				break;
		}

		nlen = strlen( tmpres1[0] );
		if( totallen + nlen > alloclen )
		{
			fprintf( stderr, "totallen=%d +  nlen=%d > alloclen = %d\n", totallen, nlen, alloclen );
			ErrorExit( "LENGTH OVER in Falign\n " );
		}
		for( j=0; j<clus1; j++ ) strcat( result1[j], tmpres1[j] );
		for( j=0; j<clus2; j++ ) strcat( result2[j], tmpres2[j] );
		totallen += nlen;
#if 0
		fprintf( stderr, "i=%d", i );
		fprintf( stderr, "%4d\n", totallen );
		fprintf( stderr, "\n\n" );
		for( j=0; j<clus1; j++ ) 
		{
			fprintf( stderr, "%s\n", tmpres1[j] );
		}
		fprintf( stderr, "-------\n" );
		for( j=0; j<clus2; j++ ) 
		{
			fprintf( stderr, "%s\n", tmpres2[j] );
		}
#endif
	}
#if KEIKA
	fprintf( stderr, "DP ... done   \n" );
#endif

	for( j=0; j<clus1; j++ ) strcpy( seq1[j], result1[j] );
	for( j=0; j<clus2; j++ ) strcpy( seq2[j], result2[j] );
#if 0
	for( j=0; j<clus1; j++ ) 
	{
		fprintf( stderr, "%s\n", result1[j] );
	}
	fprintf( stderr, "- - - - - - - - - - -\n" );
	for( j=0; j<clus2; j++ ) 
	{
		fprintf( stderr, "%s\n", result2[j] );
	}
#endif
	return( totalscore );
}
