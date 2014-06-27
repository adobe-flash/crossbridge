/* For copyright information, see olden_v1.0/COPYRIGHT */

/*
 * VECTMATH.H: include file for vector/matrix operations.
 * Copyright (c) 1991, Joshua E. Barnes, Honolulu, HI.
 * 	    It's free because it's yours.
 */

#ifndef THREEDIM
#  ifndef TWODIM
#    ifndef NDIM
#      define THREEDIM
#    endif
#  endif
#endif

#ifdef TWODIM
#  define NDIM 2
#endif

#ifdef THREEDIM
#  define NDIM 3
#endif

typedef real vector[NDIM], matrix[NDIM][NDIM];
#ifdef TORONTO
extern double sqrt(double x);
#endif
/*
 * Vector operations.
 */

#define CLRV(v);			/* CLeaR Vector */			\
{									\
    register int i;							\
    for (i = 0; i < NDIM; i++)					\
        (v)[i] = 0.0;							\
}

#define UNITV(v,j);		/* UNIT Vector */			\
{									\
    register int i;							\
    for (i = 0; i < NDIM; i++)					\
        (v)[i] = (i == (j) ? 1.0 : 0.0);				\
}

#define SETV(v,u);		/* SET Vector */			\
{ 									\
    register int i; 							\
    for (i = 0; i < NDIM; i++) 					\
        (v)[i] = (u)[i]; 						\
}


#define ADDV(v,u,w);		/* ADD Vector */			\
{									\
    register int i;							\
    for (i = 0; i < NDIM; i++)					\
        (v)[i] = (u)[i] + (w)[i];					\
}

#define SUBV(v,u,w);		/* SUBtract Vector */			\
{									\
    register int i;							\
    for (i = 0; i < NDIM; i++)					\
        (v)[i] = (u)[i] - (w)[i];					\
}

#define MULVS(v,u,s);		/* MULtiply Vector by Scalar */		\
{									\
    register int i;							\
    for (i = 0; i < NDIM; i++)					\
        (v)[i] = (u)[i] * (s);					\
}


#define DIVVS(v,u,s);		/* DIVide Vector by Scalar */		\
{									\
    register int i;							\
    for (i = 0; i < NDIM; i++)					\
        (v)[i] = (u)[i] / (s);					\
}


#define DOTVP(s,v,u);		/* DOT Vector Product */		\
{									\
    register int i;							\
    (s) = 0.0;								\
    for (i = 0; i < NDIM; i++)					\
        (s) += (v)[i] * (u)[i];					\
}


#define ABSV(s,v);		/* ABSolute value of a Vector */	\
{									\
    double tmp;                                                \
    register int i;							\
    tmp = 0.0;								\
    for (i = 0; i < NDIM; i++)					\
        tmp += (v)[i] * (v)[i];					\
    (s) = sqrt(tmp);                                                   \
}

#define DISTV(s,u,v);		/* DISTance between Vectors */        	\
{									\
    double tmp;                                                \
    register int i;							\
    tmp = 0.0;								\
    for (i = 0; i < NDIM; i++)					\
        tmp += ((u)[i]-(v)[i]) * ((u)[i]-(v)[i]);		        \
    (s) = sqrt(tmp);                                                   \
}

#ifdef TWODIM

#define CROSSVP(s,v,u);		/* CROSS Vector Product */		\
{									\
    (s) = (v)[0]*(u)[1] - (v)[1]*(u)[0];				\
}

#endif

#ifdef THREEDIM

#define CROSSVP(v,u,w);		/* CROSS Vector Product */		\
{									\
    (v)[0] = (u)[1]*(w)[2] - (u)[2]*(w)[1];				\
    (v)[1] = (u)[2]*(w)[0] - (u)[0]*(w)[2];				\
    (v)[2] = (u)[0]*(w)[1] - (u)[1]*(w)[0];				\
}

#endif

#define INCADDV(v,u);             /* INCrementally ADD Vector */         \
{									\
    register int i;                                                    \
    for (i = 0; i < NDIM; i++)                                       \
        (v)[i] += (u)[i];                                             \
}

#define INCSUBV(v,u);             /* INCrementally SUBtract Vector */    \
{									\
    register int i;                                                    \
    for (i = 0; i < NDIM; i++)                                       \
        (v)[i] -= (u)[i];                                             \
}

#define INCMULVS(v,s);	/* INCrementally MULtiply Vector by Scalar */	\
{									\
    register int i;                                                    \
    for (i = 0; i < NDIM; i++)                                       \
        (v)[i] *= (s);                                                 \
}

#define INCDIVVS(v,s);	/* INCrementally DIVide Vector by Scalar */	\
{									\
    register int i;                                                    \
    for (i = 0; i < NDIM; i++)                                       \
        (v)[i] /= (s);                                                 \
}

/*
 * Matrix operations.
 */

#define CLRM(p);			/* CLeaR Matrix */			\
{									\
    register int i, j;						\
    for (i = 0; i < NDIM; i++)					\
        for (j = 0; j < NDIM; j++)					\
	    (p)[i][j] = 0.0;						\
}

#define SETMI(p);		/* SET Matrix to Identity */		\
{									\
    register int i, j;						\
    for (i = 0; i < NDIM; i++)					\
        for (j = 0; j < NDIM; j++)					\
	    (p)[i][j] = (i == j ? 1.0 : 0.0);			\
}

#define SETM(p,q);		/* SET Matrix */			\
{									\
    register int i, j;						\
    for (i = 0; i < NDIM; i++)					\
        for (j = 0; j < NDIM; j++)					\
	    (p)[i][j] = (q)[i][j];					\
}

#define TRANM(p,q);		/* TRANspose Matrix */			\
{									\
    register int i, j;						\
    for (i = 0; i < NDIM; i++)					\
        for (j = 0; j < NDIM; j++)					\
	    (p)[i][j] = (q)[j][i];					\
}

#define ADDM(p,q,r);		/* ADD Matrix */			\
{									\
    register int i, j;						\
    for (i = 0; i < NDIM; i++)					\
        for (j = 0; j < NDIM; j++)					\
	    (p)[i][j] = (q)[i][j] + (r)[i][j];			\
}

#define SUBM(p,q,r);		/* SUBtract Matrix */			\
{									\
    register int i, j;						\
    for (i = 0; i < NDIM; i++)					\
        for (j = 0; j < NDIM; j++)					\
	    (p)[i][j] = (q)[i][j] - (r)[i][j];			\
}

#define MULM(p,q,r);		/* Multiply Matrix */			\
{									\
    register int i, j, k;						\
    for (i = 0; i < NDIM; i++)					\
	for (j = 0; j < NDIM; j++) {					\
	    (p)[i][j] = 0.0;						\
            for (k = 0; k < NDIM; k++)				\
		(p)[i][j] += (q)[i][k] * (r)[k][j];		\
        }								\
}

#define MULMS(p,q,s);		/* MULtiply Matrix by Scalar */		\
{									\
    register int i, j;						\
    for (i = 0; i < NDIM; i++)				        \
        for (j = 0; j < NDIM; j++)					\
	    (p)[i][j] = (q)[i][j] * (s);				\
}

#define DIVMS(p,q,s);		/* DIVide Matrix by Scalar */		\
{									\
    register int i, j;						\
    for (i = 0; i < NDIM; i++)					\
        for (j = 0; j < NDIM; j++)					\
	    (p)[i][j] = (q)[i][j] / (s);				\
}

#define MULMV(v,p,u);		/* MULtiply Matrix by Vector */		\
{									\
    register int i, j;						\
    for (i = 0; i < NDIM; i++) {					\
	(v)[i] = 0.0;							\
	for (j = 0; j < NDIM; j++)					\
	    (v)[i] += (p)[i][j] * (u)[j];				\
    }									\
}

#define OUTVP(p,v,u);		/* OUTer Vector Product */		\
{									\
    register int i, j;						\
    for (i = 0; i < NDIM; i++)					\
        for (j = 0; j < NDIM; j++)					\
	    (p)[i][j] = (v)[i] * (u)[j];				\
}

#define TRACEM(s,p);		/* TRACE of Matrix */			\
{									\
    register int i;							\
    (s) = 0.0;								\
    for (i = 0.0; i < NDIM; i++)					\
	(s) += (p)[i][i];						\
}

/*
 * Misc. impure operations.
 */

#define SETVS(v,s);		/* SET Vector to Scalar */		\
{									\
    register int i;							\
    for (i = 0; i < NDIM; i++)					\
        (v)[i] = (s);							\
}

#define ADDVS(v,u,s);		/* ADD Vector and Scalar */		\
{									\
    register int i;							\
    for (i = 0; i < NDIM; i++)					\
        (v)[i] = (u)[i] + (s);					\
}

#define SETMS(p,s);		/* SET Matrix to Scalar */		\
{									\
    register int i, j;						\
    for (i = 0; i < NDIM; i++)					\
        for (j = 0; j < NDIM; j++)					\
	    (p)[i][j] = (s);						\
}












