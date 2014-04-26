/* APPLE LOCAL file AltiVec */
/* { dg-do compile { target powerpc*-*-* } } */
/* { dg-options "-faltivec" } */

void foo() {
  vector bool int boolVector = (vector bool int) vec_splat_u32(3);
  boolVector = vec_sld( boolVector, boolVector, 
    1 );  /* { dg-bogus "no instance of overloaded" } */
}
