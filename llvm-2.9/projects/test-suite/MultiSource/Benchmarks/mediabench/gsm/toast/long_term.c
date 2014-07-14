/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */

/* $Header$ */

#include <stdio.h>
#include <assert.h>

#include "private.h"

#include "gsm.h"
#include "proto.h"


#ifdef	USE_TABLE_MUL

unsigned int umul_table[ 513 ][ 256 ];

init_umul_table()
{
	int	i, j;
	int	n;
	unsigned int	 * p = &umul_table[0][0];

	for (i = 0; i < 513; i++) {
		n = 0;
		for (j = 0; j < 256; j++) {
			*p++ = n;
			n += i;
		}
	}
}

# define umul(x9, x15)	\
	((int)(umul_table[x9][x15 & 0x0FF] + (umul_table[x9][ x15 >> 8 ] << 8)))

# define table_mul(a, b)	\
	( (a < 0)  ? ((b < 0) ? umul(-a, -b) : -umul(-a, b))	\
	  	   : ((b < 0) ? -umul(a, -b) :  umul(a, b)))

#endif /* USE_TABLE_MUL */



/*
 *  4.2.11 .. 4.2.12 LONG TERM PREDICTOR (LTP) SECTION
 */


/*
 * This procedure computes the LTP gain (bc) and the LTP lag (Nc)
 * for the long term analysis filter.   This is done by calculating a
 * maximum of the cross-correlation function between the current
 * sub-segment short term residual signal d[0..39] (output of
 * the short term analysis filter; for simplification the index
 * of this array begins at 0 and ends at 39 for each sub-segment of the
 * RPE-LTP analysis) and the previous reconstructed short term
 * residual signal dp[ -120 .. -1 ].  A dynamic scaling must be
 * performed to avoid overflow.
 */

 /* This procedure exists in four versions.  First, the two integer
  * versions with or without table-multiplication (as one function);
  * then, the two floating point versions (as another function), with
  * or without scaling.
  */

#ifndef  USE_FLOAT_MUL

static void Calculation_of_the_LTP_parameters P4((d,dp,bc_out,Nc_out),
	register word	* d,		/* [0..39]	IN	*/
	register word	* dp,		/* [-120..-1]	IN	*/
	word		* bc_out,	/* 		OUT	*/
	word		* Nc_out	/* 		OUT	*/
)
{
	register int  	k, lambda;
	word		Nc, bc;
	word		wt[40];

	longword	L_max, L_power;
	word		R, S, dmax, scal;
	register word	temp;

	/*  Search of the optimum scaling of d[0..39].
	 */
	dmax = 0;

	for (k = 0; k <= 39; k++) {
		temp = d[k];
		temp = GSM_ABS( temp );
		if (temp > dmax) dmax = temp;
	}

	temp = 0;
	if (dmax == 0) scal = 0;
	else {
		assert(dmax > 0);
		temp = gsm_norm( (longword)dmax << 16 );
	}

	if (temp > 6) scal = 0;
	else scal = 6 - temp;

	assert(scal >= 0);

	/*  Initialization of a working array wt
	 */

	for (k = 0; k <= 39; k++) wt[k] = SASR( d[k], scal );

	/* Search for the maximum cross-correlation and coding of the LTP lag
	 */
	L_max = 0;
	Nc    = 40;	/* index for the maximum cross-correlation */

	for (lambda = 40; lambda <= 120; lambda++) {

# undef STEP
# ifdef USE_TABLE_MUL
#		define STEP(k) (table_mul(wt[k], dp[k - lambda]))
# else
#		define STEP(k) (wt[k] * dp[k - lambda])
# endif

		register longword L_result;

		L_result  = STEP(0)  ; L_result += STEP(1) ;
		L_result += STEP(2)  ; L_result += STEP(3) ;
		L_result += STEP(4)  ; L_result += STEP(5)  ;
		L_result += STEP(6)  ; L_result += STEP(7)  ;
		L_result += STEP(8)  ; L_result += STEP(9)  ;
		L_result += STEP(10) ; L_result += STEP(11) ;
		L_result += STEP(12) ; L_result += STEP(13) ;
		L_result += STEP(14) ; L_result += STEP(15) ;
		L_result += STEP(16) ; L_result += STEP(17) ;
		L_result += STEP(18) ; L_result += STEP(19) ;
		L_result += STEP(20) ; L_result += STEP(21) ;
		L_result += STEP(22) ; L_result += STEP(23) ;
		L_result += STEP(24) ; L_result += STEP(25) ;
		L_result += STEP(26) ; L_result += STEP(27) ;
		L_result += STEP(28) ; L_result += STEP(29) ;
		L_result += STEP(30) ; L_result += STEP(31) ;
		L_result += STEP(32) ; L_result += STEP(33) ;
		L_result += STEP(34) ; L_result += STEP(35) ;
		L_result += STEP(36) ; L_result += STEP(37) ;
		L_result += STEP(38) ; L_result += STEP(39) ;

		if (L_result > L_max) {

			Nc    = lambda;
			L_max = L_result;
		}
	}

	*Nc_out = Nc;

	L_max <<= 1;

	/*  Rescaling of L_max
	 */
	assert(scal <= 100 && scal >=  -100);
	L_max = L_max >> (6 - scal);	/* sub(6, scal) */

	assert( Nc <= 120 && Nc >= 40);

	/*   Compute the power of the reconstructed short term residual
	 *   signal dp[..]
	 */
	L_power = 0;
	for (k = 0; k <= 39; k++) {

		register longword L_temp;

		L_temp   = SASR( dp[k - Nc], 3 );
		L_power += L_temp * L_temp;
	}
	L_power <<= 1;	/* from L_MULT */

	/*  Normalization of L_max and L_power
	 */

	if (L_max <= 0)  {
		*bc_out = 0;
		return;
	}
	if (L_max >= L_power) {
		*bc_out = 3;
		return;
	}

	temp = gsm_norm( L_power );

	R = SASR( L_max   << temp, 16 );
	S = SASR( L_power << temp, 16 );

	/*  Coding of the LTP gain
	 */

	/*  Table 4.3a must be used to obtain the level DLB[i] for the
	 *  quantization of the LTP gain b to get the coded version bc.
	 */
	for (bc = 0; bc <= 2; bc++) if (R <= gsm_mult(S, gsm_DLB[bc])) break;
	*bc_out = bc;
}

#else	/* USE_FLOAT_MUL */

static void Calculation_of_the_LTP_parameters P4((d,dp,bc_out,Nc_out),
	register word	* d,		/* [0..39]	IN	*/
	register word	* dp,		/* [-120..-1]	IN	*/
	word		* bc_out,	/* 		OUT	*/
	word		* Nc_out	/* 		OUT	*/
)
{
	register int  	k, lambda;
	word		Nc, bc;

	float		wt_float[40];
	float		dp_float_base[120], * dp_float = dp_float_base + 120;

	longword	L_max, L_power;
	word		R, S, dmax, scal;
	register word	temp;

	/*  Search of the optimum scaling of d[0..39].
	 */
	dmax = 0;

	for (k = 0; k <= 39; k++) {
		temp = d[k];
		temp = GSM_ABS( temp );
		if (temp > dmax) dmax = temp;
	}

	temp = 0;
	if (dmax == 0) scal = 0;
	else {
		assert(dmax > 0);
		temp = gsm_norm( (longword)dmax << 16 );
	}

	if (temp > 6) scal = 0;
	else scal = 6 - temp;

	assert(scal >= 0);

	/*  Initialization of a working array wt
	 */

	for (k =    0; k < 40; k++) wt_float[k] =  SASR( d[k], scal );
	for (k = -120; k <  0; k++) dp_float[k] =  dp[k];

	/* Search for the maximum cross-correlation and coding of the LTP lag
	 */
	L_max = 0;
	Nc    = 40;	/* index for the maximum cross-correlation */

	for (lambda = 40; lambda <= 120; lambda += 9) {

		/*  Calculate L_result for l = lambda .. lambda + 9.
		 */
		register float *lp = dp_float - lambda;

		register float	W;
		register float	a = lp[-8], b = lp[-7], c = lp[-6],
				d = lp[-5], e = lp[-4], f = lp[-3],
				g = lp[-2], h = lp[-1];
		register float  E; 
		register float  S0 = 0, S1 = 0, S2 = 0, S3 = 0, S4 = 0,
				S5 = 0, S6 = 0, S7 = 0, S8 = 0;

#		undef STEP
#		define	STEP(K, a, b, c, d, e, f, g, h) \
			W = wt_float[K];		\
			E = W * a; S8 += E;		\
			E = W * b; S7 += E;		\
			E = W * c; S6 += E;		\
			E = W * d; S5 += E;		\
			E = W * e; S4 += E;		\
			E = W * f; S3 += E;		\
			E = W * g; S2 += E;		\
			E = W * h; S1 += E;		\
			a  = lp[K];			\
			E = W * a; S0 += E

#		define	STEP_A(K)	STEP(K, a, b, c, d, e, f, g, h)
#		define	STEP_B(K)	STEP(K, b, c, d, e, f, g, h, a)
#		define	STEP_C(K)	STEP(K, c, d, e, f, g, h, a, b)
#		define	STEP_D(K)	STEP(K, d, e, f, g, h, a, b, c)
#		define	STEP_E(K)	STEP(K, e, f, g, h, a, b, c, d)
#		define	STEP_F(K)	STEP(K, f, g, h, a, b, c, d, e)
#		define	STEP_G(K)	STEP(K, g, h, a, b, c, d, e, f)
#		define	STEP_H(K)	STEP(K, h, a, b, c, d, e, f, g)

		STEP_A( 0); STEP_B( 1); STEP_C( 2); STEP_D( 3);
		STEP_E( 4); STEP_F( 5); STEP_G( 6); STEP_H( 7);

		STEP_A( 8); STEP_B( 9); STEP_C(10); STEP_D(11);
		STEP_E(12); STEP_F(13); STEP_G(14); STEP_H(15);

		STEP_A(16); STEP_B(17); STEP_C(18); STEP_D(19);
		STEP_E(20); STEP_F(21); STEP_G(22); STEP_H(23);

		STEP_A(24); STEP_B(25); STEP_C(26); STEP_D(27);
		STEP_E(28); STEP_F(29); STEP_G(30); STEP_H(31);

		STEP_A(32); STEP_B(33); STEP_C(34); STEP_D(35);
		STEP_E(36); STEP_F(37); STEP_G(38); STEP_H(39);

		if (S0 > L_max) { L_max = S0; Nc = lambda;     }
		if (S1 > L_max) { L_max = S1; Nc = lambda + 1; }
		if (S2 > L_max) { L_max = S2; Nc = lambda + 2; }
		if (S3 > L_max) { L_max = S3; Nc = lambda + 3; }
		if (S4 > L_max) { L_max = S4; Nc = lambda + 4; }
		if (S5 > L_max) { L_max = S5; Nc = lambda + 5; }
		if (S6 > L_max) { L_max = S6; Nc = lambda + 6; }
		if (S7 > L_max) { L_max = S7; Nc = lambda + 7; }
		if (S8 > L_max) { L_max = S8; Nc = lambda + 8; }
	}
	*Nc_out = Nc;

	L_max <<= 1;

	/*  Rescaling of L_max
	 */
	assert(scal <= 100 && scal >=  -100);
	L_max = L_max >> (6 - scal);	/* sub(6, scal) */

	assert( Nc <= 120 && Nc >= 40);

	/*   Compute the power of the reconstructed short term residual
	 *   signal dp[..]
	 */
	L_power = 0;
	for (k = 0; k <= 39; k++) {

		register longword L_temp;

		L_temp   = SASR( dp[k - Nc], 3 );
		L_power += L_temp * L_temp;
	}
	L_power <<= 1;	/* from L_MULT */

	/*  Normalization of L_max and L_power
	 */

	if (L_max <= 0)  {
		*bc_out = 0;
		return;
	}
	if (L_max >= L_power) {
		*bc_out = 3;
		return;
	}

	temp = gsm_norm( L_power );

	R = SASR( L_max   << temp, 16 );
	S = SASR( L_power << temp, 16 );

	/*  Coding of the LTP gain
	 */

	/*  Table 4.3a must be used to obtain the level DLB[i] for the
	 *  quantization of the LTP gain b to get the coded version bc.
	 */
	for (bc = 0; bc <= 2; bc++) if (R <= gsm_mult(S, gsm_DLB[bc])) break;
	*bc_out = bc;
}

#ifdef	FAST

static void Fast_Calculation_of_the_LTP_parameters P4((d,dp,bc_out,Nc_out),
	register word	* d,		/* [0..39]	IN	*/
	register word	* dp,		/* [-120..-1]	IN	*/
	word		* bc_out,	/* 		OUT	*/
	word		* Nc_out	/* 		OUT	*/
)
{
	register int  	k, lambda;
	word		Nc, bc;

	float		wt_float[40];
	float		dp_float_base[120], * dp_float = dp_float_base + 120;

	register float	L_max, L_power;

	for (k = 0; k < 40; ++k) wt_float[k] = (float)d[k];
	for (k = -120; k <= 0; ++k) dp_float[k] = (float)dp[k];

	/* Search for the maximum cross-correlation and coding of the LTP lag
	 */
	L_max = 0;
	Nc    = 40;	/* index for the maximum cross-correlation */

	for (lambda = 40; lambda <= 120; lambda += 9) {

		/*  Calculate L_result for l = lambda .. lambda + 9.
		 */
		register float *lp = dp_float - lambda;

		register float	W;
		register float	a = lp[-8], b = lp[-7], c = lp[-6],
				d = lp[-5], e = lp[-4], f = lp[-3],
				g = lp[-2], h = lp[-1];
		register float  E; 
		register float  S0 = 0, S1 = 0, S2 = 0, S3 = 0, S4 = 0,
				S5 = 0, S6 = 0, S7 = 0, S8 = 0;

#		undef STEP
#		define	STEP(K, a, b, c, d, e, f, g, h) \
			W = wt_float[K];		\
			E = W * a; S8 += E;		\
			E = W * b; S7 += E;		\
			E = W * c; S6 += E;		\
			E = W * d; S5 += E;		\
			E = W * e; S4 += E;		\
			E = W * f; S3 += E;		\
			E = W * g; S2 += E;		\
			E = W * h; S1 += E;		\
			a  = lp[K];			\
			E = W * a; S0 += E

#		define	STEP_A(K)	STEP(K, a, b, c, d, e, f, g, h)
#		define	STEP_B(K)	STEP(K, b, c, d, e, f, g, h, a)
#		define	STEP_C(K)	STEP(K, c, d, e, f, g, h, a, b)
#		define	STEP_D(K)	STEP(K, d, e, f, g, h, a, b, c)
#		define	STEP_E(K)	STEP(K, e, f, g, h, a, b, c, d)
#		define	STEP_F(K)	STEP(K, f, g, h, a, b, c, d, e)
#		define	STEP_G(K)	STEP(K, g, h, a, b, c, d, e, f)
#		define	STEP_H(K)	STEP(K, h, a, b, c, d, e, f, g)

		STEP_A( 0); STEP_B( 1); STEP_C( 2); STEP_D( 3);
		STEP_E( 4); STEP_F( 5); STEP_G( 6); STEP_H( 7);

		STEP_A( 8); STEP_B( 9); STEP_C(10); STEP_D(11);
		STEP_E(12); STEP_F(13); STEP_G(14); STEP_H(15);

		STEP_A(16); STEP_B(17); STEP_C(18); STEP_D(19);
		STEP_E(20); STEP_F(21); STEP_G(22); STEP_H(23);

		STEP_A(24); STEP_B(25); STEP_C(26); STEP_D(27);
		STEP_E(28); STEP_F(29); STEP_G(30); STEP_H(31);

		STEP_A(32); STEP_B(33); STEP_C(34); STEP_D(35);
		STEP_E(36); STEP_F(37); STEP_G(38); STEP_H(39);

		if (S0 > L_max) { L_max = S0; Nc = lambda;     }
		if (S1 > L_max) { L_max = S1; Nc = lambda + 1; }
		if (S2 > L_max) { L_max = S2; Nc = lambda + 2; }
		if (S3 > L_max) { L_max = S3; Nc = lambda + 3; }
		if (S4 > L_max) { L_max = S4; Nc = lambda + 4; }
		if (S5 > L_max) { L_max = S5; Nc = lambda + 5; }
		if (S6 > L_max) { L_max = S6; Nc = lambda + 6; }
		if (S7 > L_max) { L_max = S7; Nc = lambda + 7; }
		if (S8 > L_max) { L_max = S8; Nc = lambda + 8; }
	}
	*Nc_out = Nc;

	if (L_max <= 0.)  {
		*bc_out = 0;
		return;
	}

	/*  Compute the power of the reconstructed short term residual
	 *  signal dp[..]
	 */
	dp_float -= Nc;
	L_power = 0;
	for (k = 0; k < 40; ++k) {
		register float f = dp_float[k];
		L_power += f * f;
	}

	if (L_max >= L_power) {
		*bc_out = 3;
		return;
	}

	/*  Coding of the LTP gain
	 *  Table 4.3a must be used to obtain the level DLB[i] for the
	 *  quantization of the LTP gain b to get the coded version bc.
	 */
	lambda = L_max / L_power * 32768.;
	for (bc = 0; bc <= 2; ++bc) if (lambda <= gsm_DLB[bc]) break;
	*bc_out = bc;
}

#endif	/* FAST 	 */
#endif	/* USE_FLOAT_MUL */


/* 4.2.12 */

static void Long_term_analysis_filtering P6((bc,Nc,dp,d,dpp,e),
	word		bc,	/* 					IN  */
	word		Nc,	/* 					IN  */
	register word	* dp,	/* previous d	[-120..-1]		IN  */
	register word	* d,	/* d		[0..39]			IN  */
	register word	* dpp,	/* estimate	[0..39]			OUT */
	register word	* e	/* long term res. signal [0..39]	OUT */
)
/*
 *  In this part, we have to decode the bc parameter to compute
 *  the samples of the estimate dpp[0..39].  The decoding of bc needs the
 *  use of table 4.3b.  The long term residual signal e[0..39]
 *  is then calculated to be fed to the RPE encoding section.
 */
{
	register int      k;
	register longword ltmp;

#	undef STEP
#	define STEP(BP)					\
	for (k = 0; k <= 39; k++) {			\
		dpp[k]  = GSM_MULT_R( BP, dp[k - Nc]);	\
		e[k]	= GSM_SUB( d[k], dpp[k] );	\
	}

	switch (bc) {
	case 0:	STEP(  3277 ); break;
	case 1:	STEP( 11469 ); break;
	case 2: STEP( 21299 ); break;
	case 3: STEP( 32767 ); break; 
	}
}

void Gsm_Long_Term_Predictor P7((S,d,dp,e,dpp,Nc,bc), 	/* 4x for 160 samples */

	struct gsm_state	* S,

	word	* d,	/* [0..39]   residual signal	IN	*/
	word	* dp,	/* [-120..-1] d'		IN	*/

	word	* e,	/* [0..39] 			OUT	*/
	word	* dpp,	/* [0..39] 			OUT	*/
	word	* Nc,	/* correlation lag		OUT	*/
	word	* bc	/* gain factor			OUT	*/
)
{
	assert( d  ); assert( dp ); assert( e  );
	assert( dpp); assert( Nc ); assert( bc );

#if defined(FAST) && defined(USE_FLOAT_MUL)
	if (S->fast) 
		Fast_Calculation_of_the_LTP_parameters( d, dp, bc, Nc );
	else 
#endif
		Calculation_of_the_LTP_parameters( d, dp, bc, Nc );

	Long_term_analysis_filtering( *bc, *Nc, dp, d, dpp, e );
}

/* 4.3.2 */
void Gsm_Long_Term_Synthesis_Filtering P5((S,Ncr,bcr,erp,drp),
	struct gsm_state	* S,

	word			Ncr,
	word			bcr,
	register word		* erp,		/* [0..39]		  IN */
	register word		* drp		/* [-120..-1] IN, [0..40] OUT */
)
/*
 *  This procedure uses the bcr and Ncr parameter to realize the
 *  long term synthesis filtering.  The decoding of bcr needs
 *  table 4.3b.
 */
{
	register longword	ltmp;	/* for ADD */
	register int 		k;
	word			brp, drpp, Nr;

	/*  Check the limits of Nr.
	 */
	Nr = Ncr < 40 || Ncr > 120 ? S->nrp : Ncr;
	S->nrp = Nr;
	assert(Nr >= 40 && Nr <= 120);

	/*  Decoding of the LTP gain bcr
	 */
	brp = gsm_QLB[ bcr ];

	/*  Computation of the reconstructed short term residual 
	 *  signal drp[0..39]
	 */
	assert(brp != MIN_WORD);

	for (k = 0; k <= 39; k++) {
		drpp   = GSM_MULT_R( brp, drp[ k - Nr ] );
		drp[k] = GSM_ADD( erp[k], drpp );
	}

	/*
	 *  Update of the reconstructed short term residual signal
	 *  drp[ -1..-120 ]
	 */

	for (k = 0; k <= 119; k++) drp[ -120 + k ] = drp[ -80 + k ];
}
