#include "mltaln.h"
#include "miyata.h"
#include "miyata5.h"
#include "DNA.h"

#include "JTT.c"
#include "blosum.c"

#define DEBUG 0
#define TEST 0

#define NORMALIZE1 1

static int shishagonyuu( double in )
{
	int out;
	if     ( in >  0.0 ) out = ( (int)( in + 0.5 ) );
	else if( in == 0.0 ) out = ( 0 );
	else if( in <  0.0 ) out = ( (int)( in - 0.5 ) );
	else                 out = 0;
	return( out );
}

static void calcfreq_nuc( int nseq, char **seq, double *datafreq )
{
	int i, j, l;
	int aan;
	double total;
	for( i=0; i<4; i++ )
		datafreq[i] = 0.0;
	total = 0.0;
	for( i=0; i<nseq; i++ )
	{
		l = strlen( seq[i] );
		for( j=0; j<l; j++ )
		{
			aan = amino_n[(int)seq[i][j]];
			if( aan == 4 ) aan = 3;
			if( aan >= 0 && aan < 4 )
			{
				datafreq[aan] += 1.0;
				total += 1.0;
			}
		}
	}
	for( i=0; i<4; i++ )
		if( datafreq[i] < 0.0001 ) datafreq[i] = 0.0001;

#if 0
	fprintf( stderr, "\ndatafreq = " );
	for( i=0; i<4; i++ )
		fprintf( stderr, "%10.0f ", datafreq[i] );
	fprintf( stderr, "\n" );
#endif

	total = 0.0; for( i=0; i<4; i++ ) total += datafreq[i];
//	fprintf( stderr, "total = %f\n", total );
	for( i=0; i<4; i++ ) datafreq[i] /= (double)total;
}

static void calcfreq( int nseq, char **seq, double *datafreq )
{
	int i, j, l;
	int aan;
	double total;
	for( i=0; i<20; i++ )
		datafreq[i] = 0.0;
	total = 0.0;
	for( i=0; i<nseq; i++ )
	{
		l = strlen( seq[i] );
		for( j=0; j<l; j++ )
		{
			aan = amino_n[(int)seq[i][j]];
			if( aan >= 0 && aan < 20 )
			{
				datafreq[aan] += 1.0;
				total += 1.0;
			}
		}
	}
	for( i=0; i<20; i++ )
		if( datafreq[i] < 0.0001 ) datafreq[i] = 0.0001;

	fprintf( stderr, "datafreq = \n" );
	for( i=0; i<20; i++ )
		fprintf( stderr, "%f\n", datafreq[i] );

	total = 0.0; for( i=0; i<20; i++ ) total += datafreq[i];
	fprintf( stderr, "total = %f\n", total );
	for( i=0; i<20; i++ ) datafreq[i] /= (double)total;
}

void constants( int nseq, char **seq )
{
	int i, j, x;
//	double tmp;

	if( dorp == 'd' )  /* DNA */
	{
		int k, m;
		double average;
		double **pamx = AllocateDoubleMtx( 11,11 );
		double **pam1 = AllocateDoubleMtx( 4, 4 );
		double *freq = AllocateDoubleVec( 4 );


		scoremtx = -1;
		if( RNAppenalty == NOTSPECIFIED ) RNAppenalty = DEFAULTRNAGOP_N;
		if( RNAppenalty_ex == NOTSPECIFIED ) RNAppenalty_ex = DEFAULTRNAGEP_N;
		if( ppenalty == NOTSPECIFIED ) ppenalty = DEFAULTGOP_N;
		if( ppenalty_OP == NOTSPECIFIED ) ppenalty_OP = DEFAULTGOP_N;
		if( ppenalty_ex == NOTSPECIFIED ) ppenalty_ex = DEFAULTGEP_N;
		if( ppenalty_EX == NOTSPECIFIED ) ppenalty_EX = DEFAULTGEP_N;
		if( poffset == NOTSPECIFIED ) poffset = DEFAULTOFS_N;
		if( RNApthr == NOTSPECIFIED ) RNApthr = DEFAULTRNATHR_N;
		if( pamN == NOTSPECIFIED ) pamN = DEFAULTPAMN;
		if( kimuraR == NOTSPECIFIED ) kimuraR = 2;

		RNApenalty = (int)( 3 * 600.0 / 1000.0 * RNAppenalty + 0.5 );
		RNApenalty_ex = (int)( 3 * 600.0 / 1000.0 * RNAppenalty_ex + 0.5 );
//		fprintf( stderr, "DEFAULTRNAGOP_N = %d\n", DEFAULTRNAGOP_N );
//		fprintf( stderr, "RNAppenalty = %d\n", RNAppenalty );
//		fprintf( stderr, "RNApenalty = %d\n", RNApenalty );


		RNAthr = (int)( 3 * 600.0 / 1000.0 * RNApthr + 0.5 );
		penalty = (int)( 3 * 600.0 / 1000.0 * ppenalty + 0.5);
		penalty_OP = (int)( 3 * 600.0 / 1000.0 * ppenalty_OP + 0.5);
		penalty_ex = (int)( 3 * 600.0 / 1000.0 * ppenalty_ex + 0.5);
		penalty_EX = (int)( 3 * 600.0 / 1000.0 * ppenalty_EX + 0.5);
		offset = (int)( 3 * 600.0 / 1000.0 * poffset + 0.5);
		offsetFFT = (int)( 3 * 600.0 / 1000.0 * (-0) + 0.5);
		offsetLN = (int)( 3 * 600.0 / 1000.0 * 100 + 0.5);
		penaltyLN = (int)( 3 * 600.0 / 1000.0 * -2000 + 0.5);
		penalty_exLN = (int)( 3 * 600.0 / 1000.0 * -100 + 0.5);
		sprintf( modelname, "%s%d (%d), %6.3f (%6.3f), %6.3f (%6.3f)", rnakozo?"RNA":"DNA", pamN, kimuraR,
        -(double)ppenalty*0.001, -(double)ppenalty*0.003, -(double)poffset*0.001, -(double)poffset*0.003 );

		if( kimuraR == 9999 ) 
		{
			for( i=0; i<4; i++ ) for( j=0; j<4; j++ ) 
				pamx[i][j] = (double)locn_disn[i][j];
#if NORMALIZE1
			average = 0.0;
			for( i=0; i<4; i++ ) for( j=0; j<4; j++ ) 
				average += pamx[i][j];
			average /= 16.0;
	
   	     if( disp )
				fprintf( stderr, "average = %f\n", average );
	
			for( i=0; i<4; i++ ) for( j=0; j<4; j++ ) 
				pamx[i][j] -= average;
	
			for( i=0; i<4; i++ ) for( j=0; j<4; j++ )
				pamx[i][j] *= 600.0 / average;
			
			for( i=0; i<4; i++ ) for( j=0; j<4; j++ )
				pamx[i][j] -= offset; 
#endif
		}
		else
		{
				double f = 0.99;
				double s = (double)kimuraR / ( 2 + kimuraR ) * 0.01;
				double v = (double)1       / ( 2 + kimuraR ) * 0.01;
				pam1[0][0] = f; pam1[0][1] = s; pam1[0][2] = v; pam1[0][3] = v;
				pam1[1][0] = s; pam1[1][1] = f; pam1[1][2] = v; pam1[1][3] = v;
				pam1[2][0] = v; pam1[2][1] = v; pam1[2][2] = f; pam1[2][3] = s;
				pam1[3][0] = v; pam1[3][1] = v; pam1[3][2] = s; pam1[3][3] = f;
	
				fprintf( stderr, "generating %dPAM scoring matrix for nucleotides ... ", pamN );
	
		       	if( disp )
   		    	{
   		     		fprintf( stderr, " TPM \n" );
   		        	for( i=0; i<4; i++ )
   			       	{
   		            	for( j=0; j<4; j++ )
   		                	fprintf( stderr, "%+#6.10f", pam1[i][j] );
   		            	fprintf( stderr, "\n" );
   		        	}
   		        	fprintf( stderr, "\n" );
   		     	}
	
	
				MtxuntDouble( pamx, 4 );
				for( x=0; x < pamN; x++ ) MtxmltDouble( pamx, pam1, 4 );
				for( i=0; i<4; i++ ) for( j=0; j<4; j++ )
					pamx[i][j] /= 1.0 / 4.0;
	
				for( i=0; i<4; i++ ) for( j=0; j<4; j++ )
				{
					if( pamx[i][j] == 0.0 ) 
					{
						fprintf( stderr, "WARNING: pamx[i][j] = 0.0 ?\n" );
						pamx[i][j] = 0.00001; /* by J. Thompson */
					}
					pamx[i][j] = log10( pamx[i][j] ) * 1000.0;
				}
	
   	    		if( disp )
   	    		{
   		     		fprintf( stderr, " after log\n" );
   	        		for( i=0; i<4; i++ )
   		       		{
   	        	    	for( j=0; j<4; j++ )
   	        	        	fprintf( stderr, "%+#6.10f", pamx[i][j] );
   	        	    	fprintf( stderr, "\n" );
   	        		}
   	        		fprintf( stderr, "\n" );
   		     	}


// ?????
			for( i=0; i<26; i++ ) amino[i] = locaminon[i];
			for( i=0; i<0x80; i++ ) amino_n[i] = -1;
			for( i=0; i<26; i++ ) amino_n[(int)amino[i]] = i;
			if( fmodel == 1 )
				calcfreq_nuc( nseq, seq, freq );
			else
			{
				freq[0] = 0.25;
				freq[1] = 0.25;
				freq[2] = 0.25;
				freq[3] = 0.25;
			}
//			fprintf( stderr, "a, freq[0] = %f\n", freq[0] );
//			fprintf( stderr, "g, freq[1] = %f\n", freq[1] );
//			fprintf( stderr, "c, freq[2] = %f\n", freq[2] );
//			fprintf( stderr, "t, freq[3] = %f\n", freq[3] );

			
			average = 0.0;
			for( i=0; i<4; i++ ) for( j=0; j<4; j++ )
				average += pamx[i][j] * freq[i] * freq[j];
			for( i=0; i<4; i++ ) for( j=0; j<4; j++ )
				pamx[i][j] -= average;

			average = 0.0;
			for( i=0; i<4; i++ )
				average += pamx[i][i] * 1.0 / 4.0;

			for( i=0; i<4; i++ ) for( j=0; j<4; j++ )
				pamx[i][j] *= 600.0 / average;


			for( i=0; i<4; i++ ) for( j=0; j<4; j++ )
				pamx[i][j] -= offset;        /* extending gap cost */

			for( i=0; i<4; i++ ) for( j=0; j<4; j++ )
				pamx[i][j] = shishagonyuu( pamx[i][j] );

       		if( disp )
       		{
        		fprintf( stderr, " after shishagonyuu\n" );
           		for( i=0; i<4; i++ )
   	       		{
           	    	for( j=0; j<4; j++ )
           	        	fprintf( stderr, "%+#6.10f", pamx[i][j] );
           	    	fprintf( stderr, "\n" );
           		}
           		fprintf( stderr, "\n" );
        	}
			fprintf( stderr, "done\n" );
		}
	
		for( i=0; i<5; i++ ) 
		{
			pamx[4][i] = pamx[3][i];
			pamx[i][4] = pamx[i][3];
		}	

		for( i=5; i<10; i++ ) for( j=5; j<10; j++ )
		{
			pamx[i][j] = pamx[i-5][j-5];
		}
	
       	if( disp )
       	{
       		fprintf( stderr, " before dis\n" );
          	for( i=0; i<4; i++ )
   	       	{
           	   	for( j=0; j<4; j++ )
           	       	fprintf( stderr, "%+#6.10f", pamx[i][j] );
           	   	fprintf( stderr, "\n" );
           	}
           	fprintf( stderr, "\n" );
        }

       	if( disp )
       	{
        	fprintf( stderr, " score matrix  \n" );
           	for( i=0; i<4; i++ )
   	       	{
               	for( j=0; j<4; j++ )
                   	fprintf( stderr, "%+#6.10f", pamx[i][j] );
               	fprintf( stderr, "\n" );
           	}
           	fprintf( stderr, "\n" );
        }

		for( i=0; i<26; i++ ) amino[i] = locaminon[i];
		for( i=0; i<26; i++ ) amino_grp[(int)amino[i]] = locgrpn[i];
		for( i=0; i<26; i++ ) for( j=0; j<26; j++ ) n_dis[i][j] = 0;
		for( i=0; i<10; i++ ) for( j=0; j<10; j++ ) n_dis[i][j] = shishagonyuu( pamx[i][j] );
        if( disp )
        {
            fprintf( stderr, " score matrix  \n" );
            for( i=0; i<26; i++ )
            {
                for( j=0; j<26; j++ )
                    fprintf( stderr, "%+6d", n_dis[i][j] );
                fprintf( stderr, "\n" );
            }
            fprintf( stderr, "\n" );
        }

// RIBOSUM
#if 1
		average = 0.0;
		for( i=0; i<4; i++ ) for( j=0; j<4; j++ )
			average += ribosum4[i][j] * freq[i] * freq[j];
		for( i=0; i<4; i++ ) for( j=0; j<4; j++ )
			ribosum4[i][j] -= average;

		average = 0.0;
		for( i=0; i<4; i++ ) for( j=0; j<4; j++ ) for( k=0; k<4; k++ ) for( m=0; m<4; m++ )
		{
//			if( i%4==0&&j%4==3 || i%4==3&&j%4==0 || i%4==1&&j%4==2 || i%4==2&&j%4==1 || i%4==1&&j%4==3 || i%4==3&&j%4==1 )
//			if( k%4==0&&m%4==3 || k%4==3&&m%4==0 || k%4==1&&m%4==2 || k%4==2&&m%4==1 || k%4==1&&m%4==3 || k%4==3&&m%4==1 )
				average += ribosum16[i*4+j][k*4+m] * freq[i] * freq[j] * freq[k] * freq[m];
		}
		for( i=0; i<16; i++ ) for( j=0; j<16; j++ )
			ribosum16[i][j] -= average;

		average = 0.0;
		for( i=0; i<4; i++ )
			average += ribosum4[i][i] * freq[i];
		for( i=0; i<4; i++ ) for( j=0; j<4; j++ )
			ribosum4[i][j] *= 600.0 / average;

		average = 0.0;
		average += ribosum16[0*4+3][0*4+3] * freq[0] * freq[3]; // AU
		average += ribosum16[3*4+0][3*4+0] * freq[3] * freq[0]; // UA
		average += ribosum16[1*4+2][1*4+2] * freq[1] * freq[2]; // CG
		average += ribosum16[2*4+1][2*4+1] * freq[2] * freq[1]; // GC
		average += ribosum16[1*4+3][1*4+3] * freq[1] * freq[3]; // GU
		average += ribosum16[3*4+1][3*4+1] * freq[3] * freq[1]; // UG
		for( i=0; i<16; i++ ) for( j=0; j<16; j++ )
			ribosum16[i][j] *= 600.0 / average;


#if 1
		for( i=0; i<4; i++ ) for( j=0; j<4; j++ )
			ribosum4[i][j] -= offset;        /* extending gap cost ?????*/
		for( i=0; i<16; i++ ) for( j=0; j<16; j++ )
			ribosum16[i][j] -= offset;        /* extending gap cost ?????*/
#endif

		for( i=0; i<4; i++ ) for( j=0; j<4; j++ )
			ribosum4[i][j] = shishagonyuu( ribosum4[i][j] );
		for( i=0; i<16; i++ ) for( j=0; j<16; j++ )
			ribosum16[i][j] = shishagonyuu( ribosum16[i][j] );

  		if( disp )
   		{
     		fprintf( stderr, "ribosum after shishagonyuu\n" );
       		for( i=0; i<4; i++ )
       		{
       	    	for( j=0; j<4; j++ )
       	        	fprintf( stderr, "%+#6.10f", ribosum4[i][j] );
       	    	fprintf( stderr, "\n" );
       		}
       		fprintf( stderr, "\n" );
     		fprintf( stderr, "ribosum16 after shishagonyuu\n" );
       		for( i=0; i<16; i++ )
       		{
       	    	for( j=0; j<16; j++ )
       	        	fprintf( stderr, "%+#7.0f", ribosum16[i][j] );
       	    	fprintf( stderr, "\n" );
       		}
       		fprintf( stderr, "\n" );
      	}
		fprintf( stderr, "done\n" );

#if 1
		for( i=0; i<37; i++ ) for( j=0; j<37; j++ ) ribosumdis[i][j] = 0.0; //iru
		for( m=0; m<9; m++ ) for( i=0; i<4; i++ ) // loop
			for( k=0; k<9; k++ ) for( j=0; j<4; j++ ) ribosumdis[m*4+i][k*4+j] = ribosum4[i][j]; // loop-loop
//			for( k=0; k<9; k++ ) for( j=0; j<4; j++ ) ribosumdis[m*4+i][k*4+j] = n_dis[i][j]; // loop-loop

		for( i=0; i<16; i++ ) for( j=0; j<16; j++ ) ribosumdis[i+4][j+4] = ribosum16[i][j]; // stem5-stem5
		for( i=0; i<16; i++ ) for( j=0; j<16; j++ ) ribosumdis[i+20][j+20] = ribosum16[i][j]; // stem5-stem5
#else // do not use ribosum
		for( i=0; i<37; i++ ) for( j=0; j<37; j++ ) ribosumdis[i][j] = 0.0; //iru
		for( m=0; m<9; m++ ) for( i=0; i<4; i++ ) // loop
			for( k=0; k<9; k++ ) for( j=0; j<4; j++ ) ribosumdis[m*4+i][k*4+j] = n_dis[i][j]; // loop-loop
#endif

  		if( disp )
   		{
     		fprintf( stderr, "ribosumdis\n" );
       		for( i=0; i<37; i++ )
       		{
       	    	for( j=0; j<37; j++ )
       	        	fprintf( stderr, "%+5d", ribosumdis[i][j] );
       	    	fprintf( stderr, "\n" );
       		}
       		fprintf( stderr, "\n" );
      	}
		fprintf( stderr, "done\n" );
#endif

		FreeDoubleMtx( pam1 );
		FreeDoubleMtx( pamx );
		free( freq );

	}
	else if( dorp == 'p' && scoremtx == 1 )  /* Blosum */
	{
		double *freq;
		double *freq1;
		double *datafreq;
		double average;
//		double tmp;
		double **n_distmp;

		n_distmp = AllocateDoubleMtx( 20, 20 );
		datafreq = AllocateDoubleVec( 20 );
		freq = AllocateDoubleVec( 20 );

		if( ppenalty == NOTSPECIFIED ) ppenalty = DEFAULTGOP_B;
		if( ppenalty_OP == NOTSPECIFIED ) ppenalty_OP = DEFAULTGOP_B;
		if( ppenalty_ex == NOTSPECIFIED ) ppenalty_ex = DEFAULTGEP_B;
		if( ppenalty_EX == NOTSPECIFIED ) ppenalty_EX = DEFAULTGEP_B;
		if( poffset == NOTSPECIFIED ) poffset = DEFAULTOFS_B;
		if( pamN == NOTSPECIFIED ) pamN = 0;
		if( kimuraR == NOTSPECIFIED ) kimuraR = 1;
		penalty = (int)( 600.0 / 1000.0 * ppenalty + 0.5 );
		penalty_OP = (int)( 600.0 / 1000.0 * ppenalty_OP + 0.5 );
		penalty_ex = (int)( 600.0 / 1000.0 * ppenalty_ex + 0.5 );
		penalty_EX = (int)( 600.0 / 1000.0 * ppenalty_EX + 0.5 );
		offset = (int)( 600.0 / 1000.0 * poffset + 0.5 );
		offsetFFT = (int)( 600.0 / 1000.0 * (-0) + 0.5);
		offsetLN = (int)( 600.0 / 1000.0 * 100 + 0.5);
		penaltyLN = (int)( 600.0 / 1000.0 * -2000 + 0.5);
		penalty_exLN = (int)( 600.0 / 1000.0 * -100 + 0.5);

		BLOSUMmtx( nblosum, n_distmp, freq, amino, amino_grp );
		if( nblosum == -1 )
			sprintf( modelname, "User-defined, %6.3f, %+6.3f, %+6.3f", -(double)ppenalty/1000, -(double)poffset/1000, -(double)ppenalty_ex/1000 );
		else
			sprintf( modelname, "BLOSUM%d, %6.3f, %+6.3f, %+6.3f", nblosum, -(double)ppenalty/1000, -(double)poffset/1000, -(double)ppenalty_ex/1000 );
#if 0
		for( i=0; i<26; i++ ) amino[i] = locaminod[i];
		for( i=0; i<26; i++ ) amino_grp[(int)amino[i]] = locgrpd[i];
		for( i=0; i<0x80; i++ ) amino_n[i] = 0;
		for( i=0; i<26; i++ ) amino_n[(int)amino[i]] = i;
#endif
		for( i=0; i<0x80; i++ )amino_n[i] = -1;
		for( i=0; i<26; i++) amino_n[(int)amino[i]] = i;
		if( fmodel == 1 )
		{
			calcfreq( nseq, seq, datafreq );
			freq1 = datafreq;
		}
		else
			freq1 = freq;
#if TEST
		fprintf( stderr, "raw scoreing matrix : \n" );
		for( i=0; i<20; i++ )
		{
			for( j=0; j<20; j++ ) 
			{
				fprintf( stdout, "%6.2f", n_distmp[i][j] );
			}
			fprintf( stdout, "\n" );
		}
#endif
		if( fmodel == -1 )
			average = 0.0;
		else
		{
			for( i=0; i<20; i++ )
#if TEST 
				fprintf( stdout, "freq[%c] = %f, datafreq[%c] = %f, freq1[] = %f\n", amino[i], freq[i], amino[i], datafreq[i], freq1[i] );
#endif
			average = 0.0;
			for( i=0; i<20; i++ ) for( j=0; j<20; j++ )
				average += n_distmp[i][j] * freq1[i] * freq1[j];
		}
#if TEST
		fprintf( stdout, "####### average2  = %f\n", average );
#endif

		for( i=0; i<20; i++ ) for( j=0; j<20; j++ ) 
			n_distmp[i][j] -= average;
#if TEST
		fprintf( stdout, "average2 = %f\n", average );
		fprintf( stdout, "after average substruction : \n" );
		for( i=0; i<20; i++ )
		{
			for( j=0; j<20; j++ ) 
			{
				fprintf( stdout, "%6.2f", n_distmp[i][j] );
			}
			fprintf( stdout, "\n" );
		}
#endif
		
		average = 0.0;
		for( i=0; i<20; i++ ) 
			average += n_distmp[i][i] * freq1[i];
#if TEST
		fprintf( stdout, "####### average1  = %f\n", average );
#endif

		for( i=0; i<20; i++ ) for( j=0; j<20; j++ ) 
			n_distmp[i][j] *= 600.0 / average;
#if TEST
        fprintf( stdout, "after average division : \n" );
        for( i=0; i<20; i++ )
        {
            for( j=0; j<=i; j++ )
            {
                fprintf( stdout, "%7.1f", n_distmp[i][j] );
            }
            fprintf( stdout, "\n" );
        }
#endif

		for( i=0; i<20; i++ ) for( j=0; j<20; j++ ) 
			n_distmp[i][j] -= offset;  
#if TEST
		fprintf( stdout, "after offset substruction (offset = %d): \n", offset );
		for( i=0; i<20; i++ )
		{
			for( j=0; j<=i; j++ ) 
			{
				fprintf( stdout, "%7.1f", n_distmp[i][j] );
			}
			fprintf( stdout, "\n" );
		}
#endif
#if 0
/* 注意 ！！！！！！！！！！ */
			penalty -= offset;
#endif


		for( i=0; i<20; i++ ) for( j=0; j<20; j++ ) 
			n_distmp[i][j] = shishagonyuu( n_distmp[i][j] );

        if( disp )
        {
            fprintf( stdout, " scoring matrix  \n" );
            for( i=0; i<20; i++ )
            {
				fprintf( stdout, "%c    ", amino[i] );
                for( j=0; j<20; j++ )
                    fprintf( stdout, "%5.0f", n_distmp[i][j] );
                fprintf( stdout, "\n" );
            }
			fprintf( stdout, "     " );
            for( i=0; i<20; i++ )
				fprintf( stdout, "    %c", amino[i] );

			average = 0.0;
        	for( i=0; i<20; i++ ) for( j=0; j<20; j++ )
				average += n_distmp[i][j] * freq1[i] * freq1[j];
			fprintf( stdout, "average = %f\n", average );

			average = 0.0;
        	for( i=0; i<20; i++ )
				average += n_distmp[i][i] * freq1[i];
			fprintf( stdout, "itch average = %f\n", average );
			fprintf( stderr, "parameters: %d, %d, %d\n", penalty, penalty_ex, offset );

			
  			exit( 1 );
        }

		for( i=0; i<26; i++ ) for( j=0; j<26; j++ ) n_dis[i][j] = 0;
		for( i=0; i<20; i++ ) for( j=0; j<20; j++ ) n_dis[i][j] = (int)n_distmp[i][j];

		FreeDoubleMtx( n_distmp );
		FreeDoubleVec( datafreq );
		FreeDoubleVec( freq );

		fprintf( stderr, "done.\n" );

	}
	else if( dorp == 'p' && scoremtx == 2 ) /* Miyata-Yasunaga */
	{
		fprintf( stderr, "Not supported\n" );
		exit( 1 );
		for( i=0; i<26; i++ ) for( j=0; j<26; j++ ) n_dis[i][j] = locn_dism[i][j];
		for( i=0; i<26; i++ ) if( i != 24 ) n_dis[i][24] = n_dis[24][i] = exgpm;
		n_dis[24][24] = 0;
		if( ppenalty == NOTSPECIFIED ) ppenalty = locpenaltym;
		if( poffset == NOTSPECIFIED ) poffset = -20;
		if( pamN == NOTSPECIFIED ) pamN = 0;
		if( kimuraR == NOTSPECIFIED ) kimuraR = 1;

		penalty = ppenalty;
		offset = poffset;

		sprintf( modelname, "Miyata-Yasunaga, %6.3f, %6.3f", -(double)ppenalty/1000, -(double)poffset/1000 );
		for( i=0; i<26; i++ ) amino[i] = locaminom[i];
		for( i=0; i<26; i++ ) amino_grp[(int)amino[i]] = locgrpm[i];
#if DEBUG
		fprintf( stdout, "scoreing matrix : \n" );
		for( i=0; i<26; i++ )
		{
			for( j=0; j<26; j++ ) 
			{
				fprintf( stdout, "%#5d", n_dis[i][j] );
			}
			fprintf( stdout, "\n" );
		}
#endif
	}
	else         /* JTT */
	{
		double **rsr;
		double **pam1;
		double **pamx;
		double *freq;
		double *freq1;
		double *mutab;
		double *datafreq;
		double average;
		double tmp;
		double delta;

		rsr = AllocateDoubleMtx( 20, 20 );
		pam1 = AllocateDoubleMtx( 20, 20 );
		pamx = AllocateDoubleMtx( 20, 20 );
		freq = AllocateDoubleVec( 20 );
		mutab = AllocateDoubleVec( 20 );
		datafreq = AllocateDoubleVec( 20 );

		if( ppenalty == NOTSPECIFIED ) ppenalty = DEFAULTGOP_J;
		if( ppenalty_OP == NOTSPECIFIED ) ppenalty_OP = DEFAULTGOP_J;
		if( ppenalty_ex == NOTSPECIFIED ) ppenalty_ex = DEFAULTGEP_J;
		if( ppenalty_EX == NOTSPECIFIED ) ppenalty_EX = DEFAULTGEP_J;
		if( poffset == NOTSPECIFIED ) poffset = DEFAULTOFS_J;
		if( pamN == NOTSPECIFIED )    pamN    = DEFAULTPAMN;
		if( kimuraR == NOTSPECIFIED ) kimuraR = 1;

		penalty = (int)( 600.0 / 1000.0 * ppenalty + 0.5 );
		penalty_OP = (int)( 600.0 / 1000.0 * ppenalty_OP + 0.5 );
		penalty_ex = (int)( 600.0 / 1000.0 * ppenalty_ex + 0.5 );
		penalty_EX = (int)( 600.0 / 1000.0 * ppenalty_EX + 0.5 );
		offset = (int)( 600.0 / 1000.0 * poffset + 0.5 );
		offsetFFT = (int)( 600.0 / 1000.0 * (-0) + 0.5 );
		offsetLN = (int)( 600.0 / 1000.0 * 100 + 0.5);
		penaltyLN = (int)( 600.0 / 1000.0 * -2000 + 0.5);
		penalty_exLN = (int)( 600.0 / 1000.0 * -100 + 0.5);

		sprintf( modelname, "%s %dPAM, %6.3f, %6.3f", (TMorJTT==TM)?"Transmembrane":"JTT", pamN, -(double)ppenalty/1000, -(double)poffset/1000 );

		JTTmtx( rsr, freq, amino, amino_grp, (int)(TMorJTT==TM) );

#if TEST
		fprintf( stdout, "rsr = \n" );
		for( i=0; i<20; i++ )
		{
			for( j=0; j<20; j++ )
			{
				fprintf( stdout, "%9.2f ", rsr[i][j] );
			}
			fprintf( stdout, "\n" );
		}
#endif

		for( i=0; i<0x80; i++ ) amino_n[i] = -1;
		for( i=0; i<26; i++ ) amino_n[(int)amino[i]] = i;

		if( fmodel == 1 )
		{
			calcfreq( nseq, seq, datafreq );
			freq1 = datafreq;
		}
		else
			freq1 = freq;

		fprintf( stderr, "generating %dPAM %s scoring matrix for amino acids ... ", pamN, (TMorJTT==TM)?"Transmembrane":"JTT" );

		tmp = 0.0;
		for( i=0; i<20; i++ )
		{
			mutab[i] = 0.0;
			for( j=0; j<20; j++ )
				mutab[i] += rsr[i][j] * freq[j];
			tmp += mutab[i] * freq[i];
		}
#if TEST
		fprintf( stdout, "mutability = \n" );
		for( i=0; i<20; i++ )
			fprintf( stdout, "%5.3f\n", mutab[i] );

		fprintf( stdout, "tmp = %f\n", tmp );
#endif
		delta = 0.01 / tmp;
		for( i=0; i<20; i++ )
		{
			for( j=0; j<20; j++ )
			{
				if( i != j )
					pam1[i][j] = delta * rsr[i][j] * freq[i];
				else
					pam1[i][j] = 1.0 - delta * mutab[i];
			}
		}

		if( disp )
		{
			fprintf( stdout, "pam1 = \n" );
			for( i=0; i<20; i++ )
			{
				for( j=0; j<20; j++ )
				{
					fprintf( stdout, "%9.6f ", pam1[i][j] );
				}
				fprintf( stdout, "\n" );
			}
		}

		MtxuntDouble( pamx, 20 );
		for( x=0; x < pamN; x++ ) MtxmltDouble( pamx, pam1, 20 );

		for( i=0; i<20; i++ ) for( j=0; j<20; j++ ) 
			pamx[i][j] /= freq[j];

        for( i=0; i<20; i++ ) for( j=0; j<20; j++ )
		{
			if( pamx[i][j] == 0.0 ) 
			{
				fprintf( stderr, "WARNING: pamx[%d][%d] = 0.0?\n", i, j );
				pamx[i][j] = 0.00001; /* by J. Thompson */
			}
            pamx[i][j] = log10( pamx[i][j] ) * 1000.0;
		}
 
#if TEST
		fprintf( stdout, "raw scoring matrix : \n" );
		for( i=0; i<20; i++ )
		{
			for( j=0; j<20; j++ ) 
			{
				fprintf( stdout, "%5.0f", pamx[i][j] );
			}
			fprintf( stdout, "\n" );
		}
        average = tmp = 0.0;
        for( i=0; i<20; i++ ) for( j=0; j<20; j++ )
		{
           average += pamx[i][j] * freq1[i] * freq1[j];
		   tmp += freq1[i] * freq1[j];
		}
		average /= tmp;
		fprintf( stdout, "Zenbu average = %f, tmp = %f \n", average, tmp );
        average = tmp = 0.0;
        for( i=0; i<20; i++ ) for( j=i; j<20; j++ )
		{
           average += pamx[i][j] * freq1[i] * freq1[j];
		   tmp += freq1[i] * freq1[j];
		}
		average /= tmp;
		fprintf( stdout, "Zenbu average2 = %f, tmp = %f \n", average, tmp );
		average = tmp = 0.0;
		for( i=0; i<20; i++ )
		{
			average += pamx[i][i] * freq1[i];
			tmp += freq1[i];
		}
		average /= tmp;
		fprintf( stdout, "Itch average = %f, tmp = %f \n", average, tmp );
#endif

#if NORMALIZE1
		if( fmodel == -1 )
			average = 0.0;
		else
		{
#if TEST
			for( i=0; i<20; i++ )
				fprintf( stdout, "freq[%c] = %f, datafreq[%c] = %f, freq1[] = %f\n", amino[i], freq[i], amino[i], datafreq[i], freq1[i] );
#endif
			average = 0.0;
			for( i=0; i<20; i++ ) for( j=0; j<20; j++ )
				average += pamx[i][j] * freq1[i] * freq1[j];
		}
#if TEST
		fprintf( stdout, "####### average2  = %f\n", average );
#endif

		for( i=0; i<20; i++ ) for( j=0; j<20; j++ ) 
			pamx[i][j] -= average;
#if TEST
		fprintf( stdout, "average2 = %f\n", average );
		fprintf( stdout, "after average substruction : \n" );
		for( i=0; i<20; i++ )
		{
			for( j=0; j<20; j++ ) 
			{
				fprintf( stdout, "%5.0f", pamx[i][j] );
			}
			fprintf( stdout, "\n" );
		}
#endif
		
		average = 0.0;
		for( i=0; i<20; i++ ) 
			average += pamx[i][i] * freq1[i];
#if TEST
		fprintf( stdout, "####### average1  = %f\n", average );
#endif

		for( i=0; i<20; i++ ) for( j=0; j<20; j++ ) 
			pamx[i][j] *= 600.0 / average;
#if TEST
        fprintf( stdout, "after average division : \n" );
        for( i=0; i<20; i++ )
        {
            for( j=0; j<=i; j++ )
            {
                fprintf( stdout, "%5.0f", pamx[i][j] );
            }
            fprintf( stdout, "\n" );
        }
#endif

		for( i=0; i<20; i++ ) for( j=0; j<20; j++ ) 
			pamx[i][j] -= offset;  
#if TEST
		fprintf( stdout, "after offset substruction (offset = %d): \n", offset );
		for( i=0; i<20; i++ )
		{
			for( j=0; j<=i; j++ ) 
			{
				fprintf( stdout, "%5.0f", pamx[i][j] );
			}
			fprintf( stdout, "\n" );
		}
#endif
#if 0
/* 注意 ！！！！！！！！！！ */
			penalty -= offset;
#endif


		for( i=0; i<20; i++ ) for( j=0; j<20; j++ ) 
			pamx[i][j] = shishagonyuu( pamx[i][j] );

#else

        average = 0.0;
        for( i=0; i<20; i++ ) for( j=0; j<20; j++ )
           average += pamx[i][j];
        average /= 400.0;

        for( i=0; i<20; i++ ) for( j=0; j<20; j++ )
        {
            pamx[i][j] -= average;
            pamx[i][j] = shishagonyuu( pamx[i][j] );
        }
#endif
        if( disp )
        {
            fprintf( stdout, " scoring matrix  \n" );
            for( i=0; i<20; i++ )
            {
				fprintf( stdout, "%c    ", amino[i] );
                for( j=0; j<20; j++ )
                    fprintf( stdout, "%5.0f", pamx[i][j] );
                fprintf( stdout, "\n" );
            }
			fprintf( stdout, "     " );
            for( i=0; i<20; i++ )
				fprintf( stdout, "    %c", amino[i] );

			average = 0.0;
        	for( i=0; i<20; i++ ) for( j=0; j<20; j++ )
				average += pamx[i][j] * freq1[i] * freq1[j];
			fprintf( stdout, "average = %f\n", average );

			average = 0.0;
        	for( i=0; i<20; i++ )
				average += pamx[i][i] * freq1[i];
			fprintf( stdout, "itch average = %f\n", average );
			fprintf( stderr, "parameters: %d, %d, %d\n", penalty, penalty_ex, offset );

			
  			exit( 1 );
        }

		for( i=0; i<26; i++ ) for( j=0; j<26; j++ ) n_dis[i][j] = 0;
		for( i=0; i<20; i++ ) for( j=0; j<20; j++ ) n_dis[i][j] = (int)pamx[i][j];

		fprintf( stderr, "done.\n" );
		FreeDoubleMtx( rsr );
		FreeDoubleMtx( pam1 );
		FreeDoubleMtx( pamx );
		FreeDoubleVec( freq );
		FreeDoubleVec( mutab );
		FreeDoubleVec( datafreq );
	}
	fprintf( stderr, "scoremtx = %d\n", scoremtx );

#if DEBUG
	fprintf( stderr, "scoremtx = %d\n", scoremtx );
	fprintf( stderr, "amino[] = %s\n", amino );
#endif

	for( i=0; i<0x80; i++ )amino_n[i] = -1;
	for( i=0; i<26; i++) amino_n[(int)amino[i]] = i;
    for( i=0; i<0x80; i++ ) for( j=0; j<0x80; j++ ) amino_dis[i][j] = 0;
    for( i=0; i<0x80; i++ ) for( j=0; j<0x80; j++ ) amino_disLN[i][j] = 0;
    for( i=0; i<0x80; i++ ) for( j=0; j<0x80; j++ ) amino_dis_consweight_multi[i][j] = 0.0;
    for( i=0; i<26; i++) for( j=0; j<26; j++ )
	{
        amino_dis[(int)amino[i]][(int)amino[j]] = n_dis[i][j];
		n_dis_consweight_multi[i][j] = (float)n_dis[i][j] * consweight_multi;
		amino_dis_consweight_multi[(int)amino[i]][(int)amino[j]] = (double)n_dis[i][j] * consweight_multi;
	}

	if( dorp == 'd' )  /* DNA */
	{
	    for( i=0; i<5; i++) for( j=0; j<5; j++ )
        	amino_disLN[(int)amino[i]][(int)amino[j]] = n_dis[i][j] + offset - offsetLN;
	    for( i=5; i<10; i++) for( j=5; j<10; j++ )
        	amino_disLN[(int)amino[i]][(int)amino[j]] = n_dis[i][j] + offset - offsetLN;
	    for( i=0; i<5; i++) for( j=0; j<5; j++ )
        	n_disFFT[i][j] = n_dis[i][j] + offset - offsetFFT;
	    for( i=5; i<10; i++) for( j=5; j<10; j++ )
        	n_disFFT[i][j] = n_dis[i][j] + offset - offsetFFT;
	}
	else // protein
	{
	    for( i=0; i<20; i++) for( j=0; j<20; j++ )
        	amino_disLN[(int)amino[i]][(int)amino[j]] = n_dis[i][j] + offset - offsetLN;
	    for( i=0; i<20; i++) for( j=0; j<20; j++ )
        	n_disFFT[i][j] = n_dis[i][j] + offset - offsetFFT;
	}

#if 0
		fprintf( stderr, "amino_dis (offset = %d): \n", offset );
		for( i=0; i<20; i++ )
		{
			for( j=0; j<20; j++ ) 
			{
				fprintf( stderr, "%5d", amino_dis[(int)amino[i]][(int)amino[j]] );
			}
			fprintf( stderr, "\n" );
		}

		fprintf( stderr, "amino_disLN (offsetLN = %d): \n", offsetLN );
		for( i=0; i<20; i++ )
		{
			for( j=0; j<20; j++ ) 
			{
				fprintf( stderr, "%5d", amino_disLN[(int)amino[i]][(int)amino[j]] );
			}
			fprintf( stderr, "\n" );
		}

		fprintf( stderr, "n_dis (offset = %d): \n", offset );
		for( i=0; i<26; i++ )
		{
			for( j=0; j<26; j++ ) 
			{
				fprintf( stderr, "%5d", n_dis[i][j] );
			}
			fprintf( stderr, "\n" );
		}

		fprintf( stderr, "n_disFFT (offsetFFT = %d): \n", offsetFFT );
		for( i=0; i<26; i++ )
		{
			for( j=0; j<26; j++ ) 
			{
				fprintf( stderr, "%5d", n_disFFT[i][j] );
			}
			fprintf( stderr, "\n" );
		}
exit( 1 );
#endif


	ppid = 0;


	if( fftThreshold == NOTSPECIFIED )
	{
		fftThreshold = FFT_THRESHOLD;
	}
	if( fftWinSize == NOTSPECIFIED )
	{
		if( dorp == 'd' ) 
			fftWinSize = FFT_WINSIZE_D;
		else    
			fftWinSize = FFT_WINSIZE_P;
	}


	if( fftscore )
	{
		double av, sd;

		for( i=0; i<20; i++ ) polarity[i] = polarity_[i];
		for( av=0.0, i=0; i<20; i++ ) av += polarity[i];
		av /= 20.0;
		for( sd=0.0, i=0; i<20; i++ ) sd += ( polarity[i]-av ) * ( polarity[i]-av );
		sd /= 20.0; sd = sqrt( sd );
		for( i=0; i<20; i++ ) polarity[i] -= av;
		for( i=0; i<20; i++ ) polarity[i] /= sd;
	
		for( i=0; i<20; i++ ) volume[i] = volume_[i];
		for( av=0.0, i=0; i<20; i++ ) av += volume[i];
		av /= 20.0;
		for( sd=0.0, i=0; i<20; i++ ) sd += ( volume[i]-av ) * ( volume[i]-av );
		sd /= 20.0; sd = sqrt( sd );
		for( i=0; i<20; i++ ) volume[i] -= av;
		for( i=0; i<20; i++ ) volume[i] /= sd;

#if 0
		for( i=0; i<20; i++ ) fprintf( stdout, "amino=%c, pol = %f<-%f, vol = %f<-%f\n", amino[i], polarity[i], polarity_[i], volume[i], volume_[i] );
		for( i=0; i<20; i++ ) fprintf( stdout, "%c %+5.3f %+5.3f\n", amino[i], volume[i], polarity[i] );
#endif
	}
}
