/* APPLE LOCAL file AltiVec */
/* { dg-do preprocess { target powerpc*-*-* } } */
/* { dg-options "-maltivec" } */

/* -maltivec should not define macros.  */
#ifdef vector
#error vector should not be defined
#endif
#ifdef bool
#error bool should not be defined
#endif


