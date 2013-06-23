#include "mltaln.h"
#include "dp.h"

#define DEBUG 0

void tracking( char **nseq, char **aseq, char *seq, int **ijp, int icyc )
{
	int i, k, l;
	int iin, ifi, jin, jfi;
	int lgth = strlen( aseq[0] );
	int lgth1 = strlen( seq );
	char gap[] = "-";
	
	for( i=0; i<=icyc+1; i++ )
	{
		nseq[i] += lgth+lgth1;
		*nseq[i] = 0;
	}

	iin = lgth; jin = lgth1;
	for( k=0; k<=lgth+lgth1; )
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
			/*
			if( ifi < 0 ) jfi = -1;  
			if( jfi < 0 ) ifi = -1;
			*/
		}
		for( l=1; l<iin-ifi; l++ )
		{
			for( i=0; i<icyc+1; i++ ) 
				*--nseq[i] = aseq[i][iin-l];
			*--nseq[icyc+1] = *gap;
			k++;
		}
		for( l=1; l<jin-jfi; l++ )
		{
			for( i=0; i<icyc+1; i++ ) 
				*--nseq[i] = *gap;
			nseq[icyc+1]--;
			*nseq[icyc+1] = seq[jin-l];
			k++;
		}
		if( iin <= 0 || jin <= 0 ) break;
		for( i=0; i<icyc+1; i++ ) 
			*--nseq[i] = aseq[i][ifi];
		*--nseq[icyc+1] = seq[jfi];
		k++;
		iin = ifi; jin = jfi;
	}
}


char **Calignm1( float *wm, char **aseq, char *seq, double *effarr, int icyc, int ex )
{
	register int i, j, k, l;
	float tmpfloat;
	int inttmp;
	static float mi, *m;
	static int mpi, *mp;
	static float ***g;
	int gb1, gc1;
	static int **ijp;
	static float **v, **w;
	static float *gvsa;
	static char **mseq;
	static char **nseq;
	static float **scmx;
	float wmax;
	int lgth, lgth1;
	static int orlgth = 0, orlgth1 = 0;
	float x;
	float efficient;
	float *AllocateFloatVec( int );
	float totaleff;
	static float **gl;
	static float **gs;
	float **AllocateFloatTri( int );
	void FreeFloatTri( float ** );

#if DEBUG
	FILE *fp;
#endif


	totaleff = 0.0;
	for( i=0; i<icyc+1; i++ ) totaleff += effarr[i];
	lgth = strlen( aseq[0] );
	lgth1 = strlen( seq );


#if DEBUG
	fprintf( stdout, "effarr in Calignm1 s = \n", ex  );
	for( i=0; i<icyc+1; i++ ) 
		fprintf( stdout, " %f", effarr[i] );
	printf( "\n" );
#endif

	if( orlgth > 0 && orlgth1 > 0 ) for( i=0; i<njob+1; i++ ) nseq[i] = mseq[i];
/* 
 *     allocate
 */

	if( lgth > orlgth || lgth1 > orlgth1 )
	{
		int ll1, ll2;

		if( orlgth > 0 && orlgth1 > 0 )
		{	
			FreeFloatMtx( v );

			FreeFloatCub( g );
			FreeFloatTri( gl );
			FreeFloatTri( gs );

			FreeFloatVec( m );
			FreeIntVec( mp );

#if 0
			FreeCharMtx( nseq );
#endif
			FreeCharMtx( mseq );

			FreeFloatVec( gvsa );
			FreeFloatMtx( scmx );
		}

		ll1 = MAX( (int)(1.1*lgth), orlgth );
		ll2 = MAX( (int)(1.1*lgth1), orlgth1 );
		ll1 = MAX( ll1, ll2 );

#if DEBUG
		fprintf( stderr, "\n\ntrying to allocate %dx%d matrices ... ", ll1+1, ll2+1 );
#endif

		v = AllocateFloatMtx( ll1+2, ll2+2 );

		g = AllocateFloatCub( ll1+2, 3, 3 );

		gl = AllocateFloatTri( MAX( ll1, ll2 ) + 3 );
		gs = AllocateFloatTri( MAX( ll1, ll2 ) + 3 );

		m = AllocateFloatVec( ll1+2 );
		mp = AllocateIntVec( ll1+2 );

		mseq = AllocateCharMtx( njob+1, 1 );
		nseq = AllocateCharMtx( njob+1, ll1+ll2 );
		for( i=0; i<njob+1; i++ ) mseq[i] = nseq[i];

		gvsa = AllocateFloatVec( ll1+2 );

		scmx = AllocateFloatMtx( 26, ll1+2 );

#if DEBUG
		fprintf( stderr, "succeeded\n\n" );
#endif

		orlgth = ll1;
		orlgth1 = ll2;
#if DEBUG
		fprintf( stderr, "orlgth   ==  %d\n", orlgth );
#endif
	}

	if( orlgth > commonAlloc1 || orlgth1 > commonAlloc2 )
	{
		int ll1, ll2;

		if( commonAlloc1 && commonAlloc2 )
		{
			FreeIntMtx( commonIP );
		}

		ll1 = MAX( commonAlloc1, orlgth );
		ll2 = MAX( commonAlloc2, orlgth1 );

#if DEBUG
		fprintf( stderr, "\n\ntrying to allocate %dx%d matrices ... ", ll1+1, ll2+1 );
#endif

		commonIP = AllocateIntMtx( ll1+10, ll2+10 );

#if DEBUG
		fprintf( stderr, "succeeded\n\n" );
#endif

		commonAlloc1 = ll1;
		commonAlloc2 = ll2;
	}

	ijp = commonIP;

	scmx_calc( icyc, aseq, effarr, scmx );

	for( i=0; i<lgth; i++ )
	{
		float scarr[26];
		for( l=0; l<26; l++ )
		{
			scarr[l] = 0.0;
			for( k=0; k<26; k++ )
				scarr[l] += n_dis[k][l] * scmx[k][i];
		}
		for( j=0; j<lgth1; j++ )
		{
			v[i][j] = scarr[(int)amino_n[(int)seq[j]]];
		}
		gvsa[i] = scarr[24];
	}
	for( i=0; i<lgth+1; i++ ) v[i][lgth1] = 0.0;
	for( j=0; j<lgth1+1; j++ ) v[lgth][j] = 0.0;
	gvsa[lgth] = 0.0;

	for( j=0; j<lgth+1; j++ ) for( k=0; k<3; k++ ) for( l=0; l<3; l++ )
		g[j][k][l] = 0;

		
	for( i=0; i<icyc+1; i++ )
	{
		efficient = (float)effarr[i];

		gc1 = 0; /* 1? */
		for( j=0; j<lgth+1; j++ ) 
		{
			gb1 = gc1;

			gc1 = ( aseq[i][j] == '-' );
			if( j == lgth )  gc1 = 0; /*  0? */
							/* continue; */

			g[j][0][0] += ( !gb1 *  gc1 ) * efficient * penalty;
			g[j][1][0] += ( 0 ) * efficient * penalty;
			g[j][2][0] += ( 0 ) * efficient * penalty;
			g[j][0][1] += ( gb1 * !gc1 + !gb1 * !gc1 ) * efficient * penalty;
			g[j][1][1] += ( 0 ) * efficient * penalty;
			g[j][0][2] += ( !gb1 ) * efficient * penalty;  /* ??? */
			g[j][2][2] += ( 0 ) * efficient * penalty;
		}
	}

	for( i=0; i<lgth+2; i++ ) for( j=0; j<=i+1; j++ ) 
	{
		gs[i][j] = gl[i][j] = 0.0;
	}

	for( i=0; i<icyc+1; i++ )
	{
		efficient = (float)effarr[i];
		inttmp = 0;
		for( j=0; j<lgth+1; j++ ) 
		{
			if( aseq[i][j] == '-' )
			{
				inttmp++;
				gs[j][inttmp] += (float)efficient * penalty;

				if( aseq[i][j+1] != '-' )
					gl[j][inttmp] += (float)efficient * penalty;
			}
			else inttmp = 0;
		}

	}
	/*

	for( i=0; i<lgth+1; i++ ) 
	{	
		fprintf( stderr, "gl[%d][] = ", i );
		fprintf( stderr, "\n" );
		for( j=0; j<=i+1; j++ ) 
			fprintf( stderr, " %.0f", i, j, gl[i][j] );
		fprintf( stderr, "\n" );
	}
	*/

	for( i=0; i<lgth+1; i++ ) 
	{
		for( j=1; j<=i; j++ ) 
			gs[i][j+1] += gs[i][j];
		for( j=i; j>0; j-- )
			gl[i][j] += gl[i][j+1];
	}
		
/*
	for( i=0; i<lgth+1; i++ ) 
	{	
	j = 5;
		fprintf( stderr, "gs[%d][%d] = %f, gl[%d][%d] = %f\n", i, j, gs[i][j], i, j, gl[i][j] );
	}

		
*/

/*
	printf( "%d\n", lgth  );	
	for( i=0; i<lgth; i++ )
	{
		printf( "%f %f %f %f %f %f %f\n", g[i][0][0], g[i][1][0], g[i][2][0], g[i][0][1], g[i][1][1], g[i][0][2], g[i][2][2] );
	}
	printf( "\n\n\n\n\n" );	
*/
/*
	for( i=0; i<lgth; i++ ) for( j=0; j<3; j++ ) for( k=0; k<3; k++ )
	{
		g[i][j][k] /= ( (float)icyc + 1.0 );
	}
*/

/*
fp = fopen( "gapp", "a" );
fprintf( fp, "gapmatrix g[i][][] except for %d\n", ex+1 );
for( i=0; i<100; i++ ) 
{
	fprintf( fp, "site No.%#3d  ", i+1 );
	fprintf( fp, "00:%#5d ", -(int)g[i][0][0] );
	fprintf( fp, "10:%#5d ", -(int)g[i][1][0] );
	fprintf( fp, "20:%#5d ", -(int)g[i][2][0] );
	fprintf( fp, "01:%#5d ", -(int)g[i][0][1] );
	fprintf( fp, "11:%#5d ", -(int)g[i][1][1] );
	fprintf( fp, "02:%#5d ", -(int)g[i][0][2] );
	fprintf( fp, "22:%#5d ", -(int)g[i][2][2] );
	fprintf( fp, "\n" );
}
fprintf( fp, "\n" );
fclose ( fp );
*/
		
	w = v;

	w[0][0] += /* scmx[24][0] * penalty */ g[0][0][0];  /* [0][0] ? */
	w[1][0] += g[0][0][1] + g[1][1][0] + gs[1][2] + gvsa[0];  /*  ??? */
	tmpfloat = 0.0;
	for( i=2; i<lgth+2; i++ ) 
	{
		tmpfloat += g[i-1][1][1] + gl[i-2][i-1] + gvsa[i-1];
		w[i][0] += g[0][0][1] + gvsa[0] + tmpfloat + g[i][1][0] + gs[i][i+1];
	}

	w[0][1] += penalty * totaleff + n_dis[24][0] * totaleff;
	tmpfloat = 0.0;
	for( j=2; j<lgth1+1; j++ ) 
	{
		tmpfloat += n_dis[24][0] * totaleff;
		w[0][j] += penalty * totaleff + tmpfloat;
	}

	for( j=0; j<lgth1+1; ++j ) 
	{
		m[j] = 0; mp[j] = 0;
	}
	for( i=1; i<lgth+1; i++ )
	{
		mi = 0; mpi = 0;
		for( j=1; j<lgth1+1; j++ )
		{
			if( j > 1 )
			{
				x = w[i-1][j-2] + g[i-0][0][2] + n_dis[24][0] * totaleff;
				mi += g[i-1][2][2] + n_dis[24][0] * totaleff;
				if( mi < x )
				{
					mi = x;
					mpi = j-2;
				}
			}
			else 
			{
				mi = w[i-1][0] + g[i-0][0][2]/* + n_dis[24][0] * totaleff */;  /* 0.0? */
				/*
				fprintf( stderr, " i == %d j == 1, w[i][0] == %f, mi == %f, g[i][0][2] == %f\n", i, w[i][0], mi, g[i][0][2] );
				*/
			/*
				mi = 0.0 + g[i-0][0][2];  * 0.0? *
			*/
				mpi = 0;
			}

			if( i > 1 )
			{
				x = w[i-2][j-1] + g[i-1][0][1] + gvsa[i-1];
				m[j] += g[i-1][1][1] + gl[i-2][i-mp[j]-2] + gvsa[i-1];      /* ??? */
				if( m[j] < x )
				{
					m[j] = x;
					mp[j] = i-2;
				}

			}
			else
			{
				m[j] = w[0][j-1]+ g[i][0][1]/* + gvsa[1] */;  /* 0.0? */
				mp[j] = 0;
			}

			wmax = w[i-1][j-1] + g[i][0][0];
			/*
			ip[i][j]=i-1;
			jp[i][j]=j-1;
			*/
			ijp[i][j] = 0;

			x = mi + g[i][2][0];
			if( x > wmax )
			{
				wmax = x;
				/*
				ip[i][j] = i-1;
				jp[i][j] = mpi;
				*/
				ijp[i][j] = -( j - mpi );    /* ijp[][] < 0 -> j ni gap */
			}

			x = m[j] + g[i][1][0] + gs[i][i-mp[j]];
			if( x > wmax )
			{
				wmax = x;
				/*
				ip[i][j] = mp[j];
				jp[i][j] = j-1;
				*/
				ijp[i][j] = +( i - mp[j] );
			}
			w[i][j] += wmax;
		}
	}
	if( cnst )
	{
		w[lgth][lgth1] = w[lgth-1][lgth1-1] + g[lgth][0][0];
		/*
		ip[lgth][lgth1] = lgth-1;
		jp[lgth][lgth1] = lgth1-1;
		*/
		ijp[lgth][lgth1] = 0;
	}

	*wm = w[lgth][lgth1];

	for( i=0; i<lgth+1; i++ ) 
	{
	/*
		ip[i][0] = -1; jp[i][0] = -1;
	*/
		ijp[i][0] = i + 1;
	}
	for( j=0; j<lgth1+1; j++ ) 
	{
	/*
		ip[0][j] = -1; jp[0][j] = -1;
	*/
		ijp[0][j] = -( j + 1 );
	}
	/*
	ip[lgth][lgth1]=iin; jp[lgth][lgth1]=jin;
	ip[lgth+1][lgth1+1]=lgth; jp[lgth+1][lgth1+1]=lgth1;
	*/
	/*
	tracking( nseq, aseq, seq, ip, jp, icyc );
	*/
	tracking( nseq, aseq, seq, ijp, icyc );
/*
fp = fopen( "matrx", "a" );
fprintf( fp, "matrix v\n" );
fprintf( fp, "%#5d", 0 );
for( j=0; j<20; j++ ) fprintf( fp, "%#5d", j );
fprintf( fp, "\n" );
for( i=0; i<lgth+1; i++ ) 
{
	fprintf( fp, "%#5d", i );
	for( j=0; j<20; j++ ) 
	{
		fprintf( fp, "%#5d", (int)( w[i][j]/(icyc+1) ) );
	}
	fprintf( fp, "\n" );
}
fclose( fp );
*/
#if DEBUG
	printf( "seq1[0] = %s\n", nseq[0] );
	printf( "seq2[0] = %s\n", nseq[icyc+1] );
#endif
	*wm = w[lgth][lgth1];
	return( nseq );
}

double Cscore_m_1( char **seq, int locnjob, int ex, double **eff )
{
    int i, k;
    int len = strlen( seq[0] );
    int gb1, gb2, gc1, gc2;
    int cob;
    int nglen;
    double score;
	double pen;
	double tmpscore;
	int *glen1, *glen2;

	int tmp1, tmp2;


	glen1 = AllocateIntVec( locnjob );
	glen2 = AllocateIntVec( locnjob );
    score = 0.0;
    nglen = 0;
	/*
	printf( "in Cscore_m_1\n" );
	for( i=0; i<locnjob; i++ ) 
	{
		if( i == ex ) continue;
		fprintf( stdout, "%d-%d:%f\n", ex, i, eff[ex][i] );
	}
	*/
   	for( k=0; k<len; k++ )
    {

		tmpscore = pen = 0;
   		for( i=0; i<locnjob; i++ )
        {
 	 		double efficient = eff[ex][i];
        	if( i == ex ) continue;
			if( k > 0 )
			{
         		 gb1 = ( seq[i][k-1] == '-' );
           		 gb2 = ( seq[ex][k-1] == '-' );
			}
			else
			{
				gb1 = 0; 
				gb2 = 0;
			}

			if( gb1 ) glen1[i]++; else glen1[i] = 0;
			if( gb2 ) glen2[i]++; else glen2[i] = 0;

            gc1 = ( seq[i][k] == '-' );
            gc2 = ( seq[ex][k] == '-' );

			if( glen1[i] >= glen2[i] ) tmp1 = 1; else tmp1 = 0;
			if( glen1[i] <= glen2[i] ) tmp2 = 1; else tmp2 = 0;
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
                 *  gb2  *  gc2      * tmp1

                 +  gb1  *  gc1
                 *  gb2  * !gc2      * tmp2
                 ;

            pen += (double)cob * penalty * efficient;
            tmpscore += (double)amino_dis[(int)seq[i][k]][(int)seq[ex][k]] * efficient;
            /*
            nglen += ( !gc1 * !gc2 );
            */
			/*
			if( k == 0 )
			{
				printf( "%c<->%c * %f = %f\n", seq[ex][k], seq[i][k], efficient, amino_dis[seq[i][k]][seq[ex][k]] * efficient );
			}
			*/
        }
		score += tmpscore;
		score += pen;
		/*
        if( 1 ) fprintf( stdout, "%c   %f    ", seq[ex][k], score / (locnjob-1) );
        if( 1 ) fprintf( stdout, "penalty  %f\n", (double)pen / (locnjob - 1 ) );
		*/
    }
	/*
    fprintf( stdout, "in Cscore_m_1 %f\n", score );
	*/
	/*
	fprintf( stdout, "%s\n", seq[ex] );
	*/
	free( glen1 );
	free( glen2 );
    return( (double)score /*/ nglen + 400.0*/ );
}

