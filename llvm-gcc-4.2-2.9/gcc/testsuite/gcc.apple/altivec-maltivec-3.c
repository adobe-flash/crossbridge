/* APPLE LOCAL file AltiVec */
/* { dg-do compile { target powerpc*-*-* } } */
/* { dg-options "-maltivec" } */

/* Check that -faltivec language changes don't leak into -maltivec.  */

static int v1 = (1, 2);  /* { dg-error "not constant" } */

typedef int vt __attribute__((vector_size (16)));

void f(void)
{
  vt v2 = (vt)(1,2,3,4); /* { dg-error "can't convert" } */
}
