/* { dg-do compile { target powerpc*-*-* } } */
/* { dg-options "-maltivec -faltivec" } */

#define vector __attribute__((vector_size(16)))

vector long long vbl;	/* { dg-error "use of 'long long' in AltiVec types is invalid" } */

