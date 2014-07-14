#include "mltaln.h"

#define DEBUG 0
#define IODEBUG 0
#define SCOREOUT 0

#define NODIST -9999

static char *whereispairalign;
static char *laraparams;
static char foldalignopt[1000];
static int out_align_instead_of_hat3;

static void t2u( char *seq )
{
	while( *seq )
	{
		if     ( *seq == 'A' ) *seq = 'a';
		else if( *seq == 'a' ) *seq = 'a';
		else if( *seq == 'T' ) *seq = 'u';
		else if( *seq == 't' ) *seq = 'u';
		else if( *seq == 'U' ) *seq = 'u';
		else if( *seq == 'u' ) *seq = 'u';
		else if( *seq == 'G' ) *seq = 'g';
		else if( *seq == 'g' ) *seq = 'g';
		else if( *seq == 'C' ) *seq = 'c';
		else if( *seq == 'c' ) *seq = 'c';
		else *seq = 'n';
		seq++;
	}
}

static float recallpairfoldalign( char **mseq1, char **mseq2, int m1, int m2, int *of1pt, int *of2pt, int alloclen )
{
	static FILE *fp = NULL;
	float value;
	char *aln1;
	char *aln2;
	int of1tmp, of2tmp;

	if( fp == NULL )
	{
		fp = fopen( "_foldalignout", "r" );
		if( fp == NULL )
		{
			fprintf( stderr, "Cannot open _foldalignout\n" );
			exit( 1 );
		}
	}

	aln1 = calloc( alloclen, sizeof( char ) );
	aln2 = calloc( alloclen, sizeof( char ) );

	readpairfoldalign( fp, *mseq1, *mseq2, aln1, aln2, m1, m2, &of1tmp, &of2tmp, alloclen );

	if( strstr( foldalignopt, "-global") )
	{
		fprintf( stderr, "Calling G__align11\n" );
		value = G__align11( mseq1, mseq2, alloclen );
		*of1pt = 0;
		*of2pt = 0;
	}
	else
	{
		fprintf( stderr, "Calling L__align11\n" );
		value = L__align11( mseq1, mseq2, alloclen, of1pt, of2pt );
	}

//	value = (float)naivepairscore11( *mseq1, *mseq2, penalty ); // nennnotame

	if( aln1[0] == 0 )
	{
		fprintf( stderr, "FOLDALIGN returned no alignment between %d and %d.  Sequence alignment is used instead.\n", m1+1, m2+1 );
	}
	else
	{
		strcpy( *mseq1, aln1 );
		strcpy( *mseq2, aln2 );
		*of1pt = of1tmp;
		*of2pt = of2tmp;
	}

//	value = naivepairscore11( *mseq1, *mseq2, penalty ); // v6.511 ha kore wo tsukau, global nomi dakara.

//	fclose( fp ); // saigo dake yatta houga yoi.

//	fprintf( stderr, "*mseq1 = %s\n", *mseq1 );
//	fprintf( stderr, "*mseq2 = %s\n", *mseq2 );


	free( aln1 );
	free( aln2 );

	return( value );
}

static void callfoldalign( int nseq, char **mseq )
{
	FILE *fp;
	int i;
	int res;
	static char com[10000];

	for( i=0; i<nseq; i++ )
		t2u( mseq[i] );

	fp = fopen( "_foldalignin", "w" );
	if( !fp )
	{
		fprintf( stderr, "Cannot open _foldalignin\n" );
		exit( 1 );
	}
	for( i=0; i<nseq; i++ )
	{
		fprintf( fp, ">%d\n", i+1 );
		fprintf( fp, "%s\n", mseq[i] );
	}
	fclose( fp );

	sprintf( com, "env PATH=%s  foldalign210 %s _foldalignin > _foldalignout ", whereispairalign, foldalignopt );
	res = system( com );
	if( res )
	{
		fprintf( stderr, "Error in foldalign\n" );
		exit( 1 );
	}

}

static void calllara( int nseq, char **mseq, char *laraarg )
{
	FILE *fp;
	int i;
	int res;
	static char com[10000];

	for( i=0; i<nseq; i++ )

	fp = fopen( "_larain", "w" );
	if( !fp )
	{
		fprintf( stderr, "Cannot open _larain\n" );
		exit( 1 );
	}
	for( i=0; i<nseq; i++ )
	{
		fprintf( fp, ">%d\n", i+1 );
		fprintf( fp, "%s\n", mseq[i] );
	}
	fclose( fp );


//	fprintf( stderr, "calling LaRA\n" );
	sprintf( com, "env PATH=%s:/bin:/usr/bin mafft_lara -i _larain -w _laraout -o _lara.params %s", whereispairalign, laraarg );
	res = system( com );
	if( res )
	{
		fprintf( stderr, "Error in lara\n" );
		exit( 1 );
	}
}

static float recalllara( char **mseq1, char **mseq2, int alloclen )
{
	static FILE *fp = NULL;
	static char *ungap1;
	static char *ungap2;
	static char *ori1;
	static char *ori2;
//	int res;
	static char com[10000];
	float value;


	if( fp == NULL )
	{
		fp = fopen( "_laraout", "r" );
		if( fp == NULL )
		{
			fprintf( stderr, "Cannot open _laraout\n" );
			exit( 1 );
		}
		ungap1 = AllocateCharVec( alloclen );
		ungap2 = AllocateCharVec( alloclen );
		ori1 = AllocateCharVec( alloclen );
		ori2 = AllocateCharVec( alloclen );
	}


	strcpy( ori1, *mseq1 );
	strcpy( ori2, *mseq2 );

	fgets( com, 999, fp );
	myfgets( com, 9999, fp );
	strcpy( *mseq1, com );
	myfgets( com, 9999, fp );
	strcpy( *mseq2, com );

	gappick0( ungap1, *mseq1 );
	gappick0( ungap2, *mseq2 );
	t2u( ungap1 );
	t2u( ungap2 );
	t2u( ori1 );
	t2u( ori2 );

	if( strcmp( ungap1, ori1 ) || strcmp( ungap2, ori2 ) )
	{
		fprintf( stderr, "SEQUENCE CHANGED!!\n" );
		fprintf( stderr, "*mseq1  = %s\n", *mseq1 );
		fprintf( stderr, "ungap1  = %s\n", ungap1 );
		fprintf( stderr, "ori1    = %s\n", ori1 );
		fprintf( stderr, "*mseq2  = %s\n", *mseq2 );
		fprintf( stderr, "ungap2  = %s\n", ungap2 );
		fprintf( stderr, "ori2    = %s\n", ori2 );
		exit( 1 );
	}

	value = (float)naivepairscore11( *mseq1, *mseq2, penalty );

//	fclose( fp ); // saigo dake yatta houga yoi.

	return( value );
}

static float recallmxscarna( char **mseq1, char **mseq2, int m1, int m2, int alloclen )
{
	FILE *fp = NULL;
	static char *ungap1 = NULL;
	static char *ungap2 = NULL;
	static char *ori1 = NULL;
	static char *ori2 = NULL;
//	int res;
	static char com[10000];
	float value;


	if( ungap1 == NULL )
	{
		ungap1 = AllocateCharVec( alloclen );
		ungap2 = AllocateCharVec( alloclen );
		ori1 = AllocateCharVec( alloclen );
		ori2 = AllocateCharVec( alloclen );
	}

	sprintf( com, "_%d-_%d.fasta", m1, m2 );
	fp = fopen( com, "r" );
	if( fp == NULL )
	{
		fprintf( stderr, "Cannot open %s.\n", com );
		exit( 1 );
	}

	strcpy( ori1, *mseq1 );
	strcpy( ori2, *mseq2 );

	fgets( com, 999, fp );
	load1SeqWithoutName_new( fp, *mseq1 );
	fgets( com, 999, fp );
	load1SeqWithoutName_new( fp, *mseq2 );

	gappick0( ungap1, *mseq1 );
	gappick0( ungap2, *mseq2 );
	t2u( ungap1 );
	t2u( ungap2 );
	t2u( ori1 );
	t2u( ori2 );

	if( strcmp( ungap1, ori1 ) || strcmp( ungap2, ori2 ) )
	{
		fprintf( stderr, "SEQUENCE CHANGED!!\n" );
		fprintf( stderr, "*mseq1  = %s\n", *mseq1 );
		fprintf( stderr, "ungap1  = %s\n", ungap1 );
		fprintf( stderr, "ori1    = %s\n", ori1 );
		fprintf( stderr, "*mseq2  = %s\n", *mseq2 );
		fprintf( stderr, "ungap2  = %s\n", ungap2 );
		fprintf( stderr, "ori2    = %s\n", ori2 );
		exit( 1 );
	}

	value = (float)naivepairscore11( *mseq1, *mseq2, penalty );

	fclose( fp );

	return( value );
}

static float callmxscarna_giving_bpp( char **mseq1, char **mseq2, char **bpp1, char **bpp2, int alloclen )
{
	FILE *fp;
	int res;
	static char com[10000];
	float value;

	fp = fopen( "_bpporg", "w" );
	if( !fp )
	{
		fprintf( stderr, "Cannot write to _bpporg\n" );
		exit( 1 );
	}
	fprintf( fp, ">a\n" );
	while( *bpp1 )
		fprintf( fp, *bpp1++ );

	fprintf( fp, ">b\n" );
	while( *bpp2 )
		fprintf( fp, *bpp2++ );
	fclose( fp );

	system( "tr -d '\\r' < _bpporg > _bpp" ); // for cygwin, wakaran

	t2u( *mseq1 );
	t2u( *mseq2 );
	fp = fopen( "_mxscarnainorg", "w" );
	if( !fp )
	{
		fprintf( stderr, "Cannot open _mxscarnainorg\n" );
		exit( 1 );
	}
	fprintf( fp, ">1\n" );
//	fprintf( fp, "%s\n", *mseq1 );
	write1seq( fp, *mseq1 );
	fprintf( fp, ">2\n" );
//	fprintf( fp, "%s\n", *mseq2 );
	write1seq( fp, *mseq2 );
	fclose( fp );

	system( "tr -d '\\r' < _mxscarnainorg > _mxscarnain" ); // for cygwin, wakaran

	sprintf( com, "env PATH=%s mxscarnamod -readbpp _mxscarnain > _mxscarnaout 2>_dum", whereispairalign );
	res = system( com );
	if( res )
	{
		fprintf( stderr, "Error in mxscarna\n" );
		exit( 1 );
	}

	fp = fopen( "_mxscarnaout", "r" );
	if( !fp )
	{
		fprintf( stderr, "Cannot open _mxscarnaout\n" );
		exit( 1 );
	}

	fgets( com, 999, fp );
	load1SeqWithoutName_new( fp, *mseq1 );
	fgets( com, 999, fp );
	load1SeqWithoutName_new( fp, *mseq2 );

	fclose( fp );

//	fprintf( stderr, "*mseq1 = %s\n", *mseq1 );
//	fprintf( stderr, "*mseq2 = %s\n", *mseq2 );

	value = (float)naivepairscore11( *mseq1, *mseq2, penalty );

	return( value );
}

static float callmxscarna_slow( char **mseq1, char **mseq2, int alloclen )
{
	FILE *fp;
	int res;
	static char com[10000];
	float value;


	t2u( *mseq1 );
	t2u( *mseq2 );
	fp = fopen( "_mxscarnain", "w" );
	if( !fp )
	{
		fprintf( stderr, "Cannot open _mxscarnain\n" );
		exit( 1 );
	}
	fprintf( fp, ">1\n" );
	fprintf( fp, "%s\n", *mseq1 );
	fprintf( fp, ">2\n" );
	fprintf( fp, "%s\n", *mseq2 );
	fclose( fp );

	sprintf( com, "env PATH=%s mxscarnamod _mxscarnain > _mxscarnaout 2>_dum", whereispairalign );
	res = system( com );
	if( res )
	{
		fprintf( stderr, "Error in mxscarna\n" );
		exit( 1 );
	}

	fp = fopen( "_mxscarnaout", "r" );
	if( !fp )
	{
		fprintf( stderr, "Cannot open _mxscarnaout\n" );
		exit( 1 );
	}

	fgets( com, 999, fp );
	load1SeqWithoutName_new( fp, *mseq1 );
	fgets( com, 999, fp );
	load1SeqWithoutName_new( fp, *mseq2 );

	fclose( fp );

//	fprintf( stderr, "*mseq1 = %s\n", *mseq1 );
//	fprintf( stderr, "*mseq2 = %s\n", *mseq2 );

	value = (float)naivepairscore11( *mseq1, *mseq2, penalty );

	return( value );
}

static void readhat4( FILE *fp, char ***bpp )
{
	char oneline[1000];
	int bppsize;
	int onechar;
//	double prob;
//	int posi, posj;

	bppsize = 0;
//	fprintf( stderr, "reading hat4\n" );
	onechar = getc(fp);
//	fprintf( stderr, "onechar = %c\n", onechar );
	if( onechar != '>' )
	{
		fprintf( stderr, "Format error\n" );
		exit( 1 );
	}
	ungetc( onechar, fp );
	fgets( oneline, 999, fp );
	while( 1 )
	{
		onechar = getc(fp);
		ungetc( onechar, fp );
		if( onechar == '>' || onechar == EOF )
		{
//			fprintf( stderr, "Next\n" );
			*bpp = realloc( *bpp, (bppsize+2) * sizeof( char * ) );
			(*bpp)[bppsize] = NULL;
			break;
		}
		fgets( oneline, 999, fp );
//		fprintf( stderr, "oneline=%s\n", oneline );
//		sscanf( oneline, "%d %d %f", &posi, &posj, &prob );
//		fprintf( stderr, "%d %d -> %f\n", posi, posj, prob );
		*bpp = realloc( *bpp, (bppsize+2) * sizeof( char * ) );
		(*bpp)[bppsize] = calloc( 100, sizeof( char ) );
		strcpy( (*bpp)[bppsize], oneline );
		bppsize++;
	}
}

static void preparebpp( int nseq, char ***bpp )
{
	FILE *fp;
	int i;

	fp = fopen( "hat4", "r" );
	if( !fp )
	{
		fprintf( stderr, "Cannot open hat4\n" );
		exit( 1 );
	}
	for( i=0; i<nseq; i++ )
		readhat4( fp, bpp+i );
	fclose( fp );
}

void arguments( int argc, char *argv[] )
{
    int c;

	foldalignopt[0] = 0;
	laraparams = NULL;
	inputfile = NULL;
	fftkeika = 0;
	pslocal = -1000.0;
	constraint = 0;
	nblosum = 62;
	fmodel = 0;
	calledByXced = 0;
	devide = 0;
	use_fft = 0;
	fftscore = 1;
	fftRepeatStop = 0;
	fftNoAnchStop = 0;
    weight = 3;
    utree = 1;
	tbutree = 1;
    refine = 0;
    check = 1;
    cut = 0.0;
    disp = 0;
    outgap = 1;
    alg = 'A';
    mix = 0;
	tbitr = 0;
	scmtd = 5;
	tbweight = 0;
	tbrweight = 3;
	checkC = 0;
	treemethod = 'x';
	contin = 0;
	scoremtx = 1;
	kobetsubunkatsu = 0;
	divpairscore = 0;
	out_align_instead_of_hat3 = 0;
	dorp = NOTSPECIFIED;
	ppenalty = NOTSPECIFIED;
	ppenalty_OP = NOTSPECIFIED;
	ppenalty_ex = NOTSPECIFIED;
	ppenalty_EX = NOTSPECIFIED;
	poffset = NOTSPECIFIED;
	kimuraR = NOTSPECIFIED;
	pamN = NOTSPECIFIED;
	geta2 = GETA2;
	fftWinSize = NOTSPECIFIED;
	fftThreshold = NOTSPECIFIED;
	RNAppenalty = NOTSPECIFIED;
	RNApthr = NOTSPECIFIED;

    while( --argc > 0 && (*++argv)[0] == '-' )
	{
        while ( ( c = *++argv[0] ) )
		{
            switch( c )
            {
				case 'i':
					inputfile = *++argv;
					fprintf( stderr, "inputfile = %s\n", inputfile );
					--argc;
					goto nextoption;
				case 'f':
					ppenalty = (int)( atof( *++argv ) * 1000 - 0.5 );
					--argc;
					goto nextoption;
				case 'g':
					ppenalty_ex = (int)( atof( *++argv ) * 1000 - 0.5 );
					--argc;
					goto nextoption;
				case 'O':
					ppenalty_OP = (int)( atof( *++argv ) * 1000 - 0.5 );
					--argc;
					goto nextoption;
				case 'E':
					ppenalty_EX = (int)( atof( *++argv ) * 1000 - 0.5 );
					--argc;
					goto nextoption;
				case 'h':
					poffset = (int)( atof( *++argv ) * 1000 - 0.5 );
					--argc;
					goto nextoption;
				case 'k':
					kimuraR = atoi( *++argv );
//					fprintf( stderr, "kimuraR = %d\n", kimuraR );
					--argc;
					goto nextoption;
				case 'b':
					nblosum = atoi( *++argv );
					scoremtx = 1;
//					fprintf( stderr, "blosum %d\n", nblosum );
					--argc;
					goto nextoption;
				case 'j':
					pamN = atoi( *++argv );
					scoremtx = 0;
					TMorJTT = JTT;
					fprintf( stderr, "jtt %d\n", pamN );
					--argc;
					goto nextoption;
				case 'm':
					pamN = atoi( *++argv );
					scoremtx = 0;
					TMorJTT = TM;
					fprintf( stderr, "TM %d\n", pamN );
					--argc;
					goto nextoption;
				case 'l':
					ppslocal = (int)( atof( *++argv ) * 1000 + 0.5 );
					pslocal = (int)( 600.0 / 1000.0 * ppslocal + 0.5);
//					fprintf( stderr, "ppslocal = %d\n", ppslocal );
//					fprintf( stderr, "pslocal = %d\n", pslocal );
					--argc;
					goto nextoption;
				case 'd':
					whereispairalign = *++argv;
					fprintf( stderr, "whereispairalign = %s\n", whereispairalign );
					--argc; 
					goto nextoption;
				case 'p':
					laraparams = *++argv;
					fprintf( stderr, "laraparams = %s\n", laraparams );
					--argc; 
					goto nextoption;
				case 'n':
					out_align_instead_of_hat3 = 1;
					break;
#if 1
				case 'a':
					fmodel = 1;
					break;
#endif
				case 'r':
					fmodel = -1;
					break;
				case 'D':
					dorp = 'd';
					break;
				case 'P':
					dorp = 'p';
					break;
				case 'e':
					fftscore = 0;
					break;
#if 0
				case 'O':
					fftNoAnchStop = 1;
					break;
#endif
				case 'Q':
					calledByXced = 1;
					break;
				case 'x':
					disp = 1;
					break;
#if 0
				case 'a':
					alg = 'a';
					break;
#endif
				case 'S':
					alg = 'S';
					break;
				case 't':
					alg = 't';
					break;
				case 'L':
					alg = 'L';
					break;
				case 's':
					alg = 's';
					break;
				case 'B':
					alg = 'B';
					break;
				case 'T':
					alg = 'T';
					break;
				case 'H':
					alg = 'H';
					break;
				case 'M':
					alg = 'M';
					break;
				case 'R':
					alg = 'R';
					break;
				case 'N':
					alg = 'N';
					break;
				case 'K':
					alg = 'K';
					break;
				case 'A':
					alg = 'A';
					break;
				case 'V':
					alg = 'V';
					break;
				case 'C':
					alg = 'C';
					break;
				case 'F':
					use_fft = 1;
					break;
				case 'v':
					tbrweight = 3;
					break;
				case 'y':
					divpairscore = 1;
					break;
/* Modified 01/08/27, default: user tree */
				case 'J':
					tbutree = 0;
					break;
/* modification end. */
				case 'o':
//					foldalignopt = *++argv;
					strcat( foldalignopt, " " );
					strcat( foldalignopt, *++argv );
					fprintf( stderr, "foldalignopt = %s\n", foldalignopt );
					--argc; 
					goto nextoption;
				case 'z':
					fftThreshold = atoi( *++argv );
					--argc; 
					goto nextoption;
				case 'w':
					fftWinSize = atoi( *++argv );
					--argc;
					goto nextoption;
				case 'Z':
					checkC = 1;
					break;
                default:
                    fprintf( stderr, "illegal option %c\n", c );
                    argc = 0;
                    break;
            }
		}
		nextoption:
			;
	}
    if( argc == 1 )
    {
        cut = atof( (*argv) );
        argc--;
    }
    if( argc != 0 ) 
    {
        fprintf( stderr, "options: Check source file !\n" );
        exit( 1 );
    }
	if( tbitr == 1 && outgap == 0 )
	{
		fprintf( stderr, "conflicting options : o, m or u\n" );
		exit( 1 );
	}
	if( alg == 'C' && outgap == 0 )
	{
		fprintf( stderr, "conflicting options : C, o\n" );
		exit( 1 );
	}
}

int countamino( char *s, int end )
{
	int val = 0;
	while( end-- )
		if( *s++ != '-' ) val++;
	return( val );
}

static void pairalign( char name[M][B], int nlen[M], char **seq, char **aseq, char **mseq1, char **mseq2, double *effarr, int alloclen )
{
	int i, j, ilim;
	int clus1, clus2;
	int off1, off2;
	float pscore = 0.0; // by D.Mathog
	static char *indication1, *indication2;
	FILE *hat2p, *hat3p;
	static double **distancemtx;
	static double *effarr1 = NULL;
	static double *effarr2 = NULL;
	char *pt;
	char *hat2file = "hat2";
	LocalHom **localhomtable, *tmpptr;
	static char **pair;
	int intdum;
	double bunbo;
	char ***bpp; // mxscarna no toki dake

	localhomtable = (LocalHom **)calloc( njob, sizeof( LocalHom *) );
	for( i=0; i<njob; i++)
	{
		localhomtable[i] = (LocalHom *)calloc( njob, sizeof( LocalHom ) );
		for( j=0; j<njob; j++)
		{
			localhomtable[i][j].start1 = -1;
			localhomtable[i][j].end1 = -1;
			localhomtable[i][j].start2 = -1; 
			localhomtable[i][j].end2 = -1; 
			localhomtable[i][j].opt = -1.0;
			localhomtable[i][j].next = NULL;
			localhomtable[i][j].nokori = 0;
		}
	}

	if( effarr1 == NULL ) 
	{
		distancemtx = AllocateDoubleMtx( njob, njob );
		effarr1 = AllocateDoubleVec( njob );
		effarr2 = AllocateDoubleVec( njob );
		indication1 = AllocateCharVec( 150 );
		indication2 = AllocateCharVec( 150 );
#if 0
#else
		pair = AllocateCharMtx( njob, njob );
#endif
	}

#if 0
	fprintf( stderr, "##### fftwinsize = %d, fftthreshold = %d\n", fftWinSize, fftThreshold );
#endif

#if 0
	for( i=0; i<njob; i++ )
		fprintf( stderr, "TBFAST effarr[%d] = %f\n", i, effarr[i] );
#endif


//	writePre( njob, name, nlen, aseq, 0 );

	for( i=0; i<njob; i++ ) for( j=0; j<njob; j++ ) pair[i][j] = 0;
	for( i=0; i<njob; i++ ) pair[i][i] = 1;

	if( alg == 'H' )
	{
		fprintf( stderr, "Calling FOLDALIGN with option '%s'\n", foldalignopt );
		callfoldalign( njob, seq );
		fprintf( stderr, "done.\n" );
	}
	if( alg == 'B' )
	{
		fprintf( stderr, "Running LARA (Bauer et al. http://www.planet-lisa.net/)\n" );
		calllara( njob, seq, "" );
		fprintf( stderr, "done.\n" );
	}
	if( alg == 'T' )
	{
		fprintf( stderr, "Running SLARA (Bauer et al. http://www.planet-lisa.net/)\n" );
		calllara( njob, seq, "-s" );
		fprintf( stderr, "done.\n" );
	}
	if( alg == 's' )
	{
		fprintf( stderr, "Preparing bpp\n" );
//		bpp = AllocateCharCub( njob, nlenmax, 0 );
		bpp = calloc( njob, sizeof( char ** ) );
		preparebpp( njob, bpp );
		fprintf( stderr, "done.\n" );
		fprintf( stderr, "Running MXSCARNA (Tabei et al. http://www.ncrna.org/software/mxscarna)\n" );
	}

	ilim = njob - 1;
	for( i=0; i<ilim; i++ ) 
	{
		fprintf( stderr, "% 5d / %d\r", i, njob );
		for( j=i+1; j<njob; j++ )
		{

			if( strlen( seq[i] ) == 0 || strlen( seq[j] ) == 0 )
			{
				distancemtx[i][j] = NODIST;
				continue;
			}

			strcpy( aseq[i], seq[i] );
			strcpy( aseq[j], seq[j] );
			clus1 = conjuctionfortbfast( pair, i, aseq, mseq1, effarr1, effarr, indication1 );
			clus2 = conjuctionfortbfast( pair, j, aseq, mseq2, effarr2, effarr, indication2 );
	//		fprintf( stderr, "mseq1 = %s\n", mseq1[0] );
	//		fprintf( stderr, "mseq2 = %s\n", mseq2[0] );
	
#if 0
			fprintf( stderr, "group1 = %.66s", indication1 );
			fprintf( stderr, "\n" );
			fprintf( stderr, "group2 = %.66s", indication2 );
			fprintf( stderr, "\n" );
#endif
	//		for( l=0; l<clus1; l++ ) fprintf( stderr, "## STEP-eff for mseq1-%d %f\n", l, effarr1[l] );
	
#if 1
			if( use_fft )
			{
				pscore = Falign( mseq1, mseq2, effarr1, effarr2, clus1, clus2, alloclen, &intdum );
//				fprintf( stderr, "pscore (fft) = %f\n", pscore );
				off1 = off2 = 0;
			}
			else
#endif
			{
				switch( alg )
				{
					case( 'a' ):
						pscore = Aalign( mseq1, mseq2, effarr1, effarr2, clus1, clus2, alloclen );
						off1 = off2 = 0;
						break;
					case( 't' ):
						pscore = G__align11_noalign( amino_dis, penalty, penalty_ex, mseq1, mseq2, alloclen );
						off1 = off2 = 0;
						break;
					case( 'A' ):
						pscore = G__align11( mseq1, mseq2, alloclen );
						off1 = off2 = 0;
						break;
#if 0
					case( 'V' ):
						pscore = VAalign11( mseq1, mseq2, alloclen, &off1, &off2, localhomtable[i]+j );
						fprintf( stderr, "i,j = %d,%d, score = %f\n", i,j, pscore );
						break;
					case( 'S' ):
						fprintf( stderr, "aligning %d-%d\n", i, j );
						pscore = suboptalign11( mseq1, mseq2, alloclen, &off1, &off2, localhomtable[i]+j );
						fprintf( stderr, "i,j = %d,%d, score = %f\n", i,j, pscore );
						break;
#endif
					case( 'N' ):
						pscore = G__align11_noalign( amino_dis, penalty, penalty_ex, mseq1, mseq2, alloclen );
						genL__align11( mseq1, mseq2, alloclen, &off1, &off2 );
//						fprintf( stderr, "pscore = %f\n", pscore );
						break;
					case( 'K' ):
						pscore = genG__align11( mseq1, mseq2, alloclen );
						off1 = off2 = 0;
						break;
					case( 'L' ):
						pscore = G__align11_noalign( amino_dis, penalty, penalty_ex, mseq1, mseq2, alloclen );
						L__align11( mseq1, mseq2, alloclen, &off1, &off2 );
//						fprintf( stderr, "pscore (1) = %f\n", pscore );
//						pscore = (float)naivepairscore11( *mseq1, *mseq2, penalty ); // nennnotame
//						fprintf( stderr, "pscore (2) = %f\n\n", pscore );
						break;
					case( 'H' ):
						pscore = recallpairfoldalign( mseq1, mseq2, i, j, &off1, &off2, alloclen );
						break;
					case( 'B' ):
					case( 'T' ):
						pscore = recalllara( mseq1, mseq2, alloclen );
						off1 = off2 = 0;
//						fprintf( stderr, "lara, pscore = %f\n", pscore );
						break;
					case( 's' ):
						pscore = callmxscarna_giving_bpp( mseq1, mseq2, bpp[i], bpp[j], alloclen );
//						pscore = G__align11_noalign( amino_disLN, -1200, -60, mseq1, mseq2, alloclen );
						off1 = off2 = 0;
//						fprintf( stderr, "scarna, pscore = %f\n", pscore );
						break;
					case( 'M' ):
//						pscore = MSalign11( mseq1, mseq2, effarr1, effarr2, clus1, clus2, alloclen, NULL, NULL, NULL, NULL );
						pscore = MSalign11( mseq1, mseq2, alloclen );
//						fprintf( stderr, "pscore (M)= %f\n", pscore );
						break;
						ErrorExit( "ERROR IN SOURCE FILE" );
				}
			}
			if( alg == 't' || ( mseq1[0][0] != 0 && mseq2[0][0] != 0  ) ) // 't' no jouken ha iranai to omou. if( ( mseq1[0][0] != 0 && mseq2[0][0] != 0  ) )
			{
				distancemtx[i][j] = pscore;
#if SCOREOUT
				fprintf( stderr, "score = %10.2f (%d,%d)\n", pscore, i, j );
#endif
//				fprintf( stderr, "pslocal = %d\n", pslocal );
//				offset = makelocal( *mseq1, *mseq2, pslocal );
#if 0
				fprintf( stderr, "off1 = %d, off2 = %d\n", off1, off2 );
#endif

				if( alg == 't' || out_align_instead_of_hat3 == 1 )
					;
				else if( alg == 'H' )
					putlocalhom_ext( mseq1[0], mseq2[0], localhomtable[i]+j, off1, off2, (int)pscore, strlen( mseq1[0] ) );
				else if( alg != 'S' && alg != 'V' )
					putlocalhom2( mseq1[0], mseq2[0], localhomtable[i]+j, off1, off2, (int)pscore, strlen( mseq1[0] ) );
			}
			else
			{
				distancemtx[i][j] = NODIST;
			}

			if( out_align_instead_of_hat3 && alg != 't' )
			{
				fprintf( stdout, "sequence %d - sequence %d, pairwise score = %.0f\n", i+1, j+1, pscore );
				fprintf( stdout, ">%s\n", name[i] );
				write1seq( stdout, mseq1[0] );
				fprintf( stdout, ">%s\n", name[j] );
				write1seq( stdout, mseq2[0] );
				fprintf( stdout, "\n" );
			}
		}
	}
	for( i=0; i<njob; i++ )
	{
		pscore = 0.0;
		for( pt=seq[i]; *pt; pt++ )
			pscore += amino_dis[(int)*pt][(int)*pt];
		distancemtx[i][i] = pscore;

	}

	ilim = njob-1;	
	for( i=0; i<ilim; i++ )
	{
		for( j=i+1; j<njob; j++ )
		{

#if 0
			fprintf( stdout, "distancemtx[i][j] = %f\n", distancemtx[i][j] );
			fprintf( stdout, "distancemtx[i][i] = %f\n", distancemtx[i][i] );
			fprintf( stdout, "distancemtx[j][j] = %f\n", distancemtx[j][j] );
			fprintf( stdout, "bunbo = %f\n", MIN( distancemtx[i][i], distancemtx[j][j] ) );
#endif
			if( distancemtx[i][j] == NODIST || (bunbo=MIN( distancemtx[i][i], distancemtx[j][j] )) == 0.0 || bunbo < distancemtx[i][j] )
				distancemtx[i][j] = 2.0;
			else
				distancemtx[i][j] = ( 1.0 - distancemtx[i][j] / bunbo ) * 2.0;
#if 0
			fprintf( stdout, "distancemtx[i][j] => %f\n", distancemtx[i][j] );
#endif
		}
	}

	hat2p = fopen( hat2file, "w" );
	if( !hat2p ) ErrorExit( "Cannot open hat2." );
	if( out_align_instead_of_hat3 == 0 )
		WriteHat2( hat2p, njob, name, distancemtx );
	else
		WriteHat2plain( hat2p, njob, distancemtx );
	fclose( hat2p );

	if( out_align_instead_of_hat3 == 0 )
	{
		fprintf( stderr, "##### writing hat3\n" );
#ifndef LLVM
		hat3p = fopen( "hat3", "w" );
		if( !hat3p ) ErrorExit( "Cannot open hat3." );
#endif
		ilim = njob-1;	
		for( i=0; i<ilim; i++ ) 
		{
			for( j=i+1; j<njob; j++ )
			{
				for( tmpptr=localhomtable[i]+j; tmpptr; tmpptr=tmpptr->next )
				{
					if( tmpptr->opt == -1.0 ) continue;
// tmptmptmptmptmp
//					if( alg == 'B' || alg == 'T' )
//						fprintf( hat3p, "%d %d %d %7.5f %d %d %d %d %p\n", i, j, tmpptr->overlapaa, 1.0, tmpptr->start1, tmpptr->end1, tmpptr->start2, tmpptr->end2, (void *)tmpptr->next ); 
//					else
#ifdef LLVM
						fprintf( stdout, "%d %d %d %7.5f %d %d %d %d\n", i, j, tmpptr->overlapaa, tmpptr->opt, tmpptr->start1, tmpptr->end1, tmpptr->start2, tmpptr->end2 ); 
#else
						fprintf( hat3p, "%d %d %d %7.5f %d %d %d %d %p\n", i, j, tmpptr->overlapaa, tmpptr->opt, tmpptr->start1, tmpptr->end1, tmpptr->start2, tmpptr->end2, (void *)tmpptr->next ); 
#endif
				}
			}
		}
#ifndef LLVM
		fclose( hat3p );
#endif
	}
#if DEBUG
	fprintf( stderr, "calling FreeLocalHomTable\n" );
#endif
	FreeLocalHomTable( localhomtable, njob );
#if DEBUG
	fprintf( stderr, "done. FreeLocalHomTable\n" );
#endif

	if( alg == 's' )
	{
		char **ptpt;
		for( i=0; i<njob; i++ )
		{
			ptpt = bpp[i];
			while( 1 )
			{
				if( *ptpt ) free( *ptpt );
				else break;
				ptpt++;
			}
			free( bpp[i] );
		}
	free( bpp );
	}
}

static void WriteOptions( FILE *fp )
{

	if( dorp == 'd' ) fprintf( fp, "DNA\n" );
	else
	{
		if     ( scoremtx ==  0 ) fprintf( fp, "JTT %dPAM\n", pamN );
		else if( scoremtx ==  1 ) fprintf( fp, "BLOSUM %d\n", nblosum );
		else if( scoremtx ==  2 ) fprintf( fp, "M-Y\n" );
	}
    fprintf( stderr, "Gap Penalty = %+5.2f, %+5.2f, %+5.2f\n", (double)ppenalty/1000, (double)ppenalty_ex/1000, (double)poffset/1000 );
    if( use_fft ) fprintf( fp, "FFT on\n" );

	fprintf( fp, "tree-base method\n" );
	if( tbrweight == 0 ) fprintf( fp, "unweighted\n" );
	else if( tbrweight == 3 ) fprintf( fp, "clustalw-like weighting\n" );
	if( tbitr || tbweight ) 
	{
		fprintf( fp, "iterate at each step\n" );
		if( tbitr && tbrweight == 0 ) fprintf( fp, "  unweighted\n" ); 
		if( tbitr && tbrweight == 3 ) fprintf( fp, "  reversely weighted\n" ); 
		if( tbweight ) fprintf( fp, "  weighted\n" ); 
		fprintf( fp, "\n" );
	}

   	 fprintf( fp, "Gap Penalty = %+5.2f, %+5.2f, %+5.2f\n", (double)ppenalty/1000, (double)ppenalty_ex/1000, (double)poffset/1000 );

	if( alg == 'a' )
		fprintf( fp, "Algorithm A\n" );
	else if( alg == 'A' ) 
		fprintf( fp, "Algorithm A+\n" );
	else if( alg == 'S' ) 
		fprintf( fp, "Apgorithm S\n" );
	else if( alg == 'C' ) 
		fprintf( fp, "Apgorithm A+/C\n" );
	else
		fprintf( fp, "Unknown algorithm\n" );

    if( use_fft )
    {
        fprintf( fp, "FFT on\n" );
        if( dorp == 'd' )
            fprintf( fp, "Basis : 4 nucleotides\n" );
        else
        {
            if( fftscore )
                fprintf( fp, "Basis : Polarity and Volume\n" );
            else
                fprintf( fp, "Basis : 20 amino acids\n" );
        }
        fprintf( fp, "Threshold   of anchors = %d%%\n", fftThreshold );
        fprintf( fp, "window size of anchors = %dsites\n", fftWinSize );
    }
	else
        fprintf( fp, "FFT off\n" );
	fflush( fp );
}
	 

int main( int argc, char *argv[] )
{
	static int  nlen[M];	
	static char name[M][B], **seq;
	static char **mseq1, **mseq2;
	static char **aseq;
	static char **bseq;
	static double *eff;
	int i;
	FILE *infp;
	char c;
	int alloclen;

	arguments( argc, argv );

	if( inputfile )
	{
		infp = fopen( inputfile, "r" );
		if( !infp )
		{
			fprintf( stderr, "Cannot open %s\n", inputfile );
			exit( 1 );
		}
	}
	else
		infp = stdin;

	getnumlen( infp );
	rewind( infp );

	if( njob < 2 )
	{
		fprintf( stderr, "At least 2 sequences should be input!\n"
						 "Only %d sequence found.\n", njob ); 
		exit( 1 );
	}
	if( njob > M )
	{
		fprintf( stderr, "The number of sequences must be < %d\n", M );
		fprintf( stderr, "Please try the splittbfast program for such large data.\n" );
		exit( 1 );
	}

	seq = AllocateCharMtx( njob, nlenmax*9+1 );
	aseq = AllocateCharMtx( njob, nlenmax*9+1 );
	bseq = AllocateCharMtx( njob, nlenmax*9+1 );
	mseq1 = AllocateCharMtx( njob, 0 );
	mseq2 = AllocateCharMtx( njob, 0 );
	alloclen = nlenmax*9;

	eff = AllocateDoubleVec( njob );

#if 0
	Read( name, nlen, seq );
#else
	readData( infp, name, nlen, seq );
#endif
	fclose( infp );

	constants( njob, seq );

#if 0
	fprintf( stderr, "params = %d, %d, %d\n", penalty, penalty_ex, offset );
#endif

	initSignalSM();

	initFiles();

	WriteOptions( trap_g );

	c = seqcheck( seq );
	if( c )
	{
		fprintf( stderr, "Illegal character %c\n", c );
		exit( 1 );
	}

//	writePre( njob, name, nlen, seq, 0 );

	for( i=0; i<njob; i++ ) eff[i] = 1.0;


	for( i=0; i<njob; i++ ) gappick0( bseq[i], seq[i] );

	pairalign( name, nlen, bseq, aseq, mseq1, mseq2, eff, alloclen );

	fprintf( trap_g, "done.\n" );
#if DEBUG
	fprintf( stderr, "closing trap_g\n" );
#endif
	fclose( trap_g );

//	writePre( njob, name, nlen, aseq, !contin );
#if 0
	writeData( stdout, njob, name, nlen, aseq );
#endif
#if IODEBUG
	fprintf( stderr, "OSHIMAI\n" );
#endif
	SHOWVERSION;
	return( 0 );
}
