/* APPLE LOCAL file AltiVec */
/* Test for handling of volatile reference vector parameters.  */
/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-options "-faltivec" } */

int foo(volatile vector float &i, int &j)
{
    vector unsigned char zero = vec_splat_u8(0);
    vector unsigned char one = vec_splat_u8(1);
    i = vec_add( (vector float)zero, (vector float)one );
    j = 5;
    return 0;
}

/* { dg-final { scan-assembler "Z3fooRVU8__vectorfRi" } } */
