/* APPLE LOCAL file Altivec */
/* { dg-do compile { target powerpc*-*-* } } */
/* { dg-xfail-if "" { "powerpc*-*-*" } { "-m64" } { "" } } */
/* { dg-options "-maltivec" } */

#include <altivec.h>

extern void preansi();

typedef void (*pvecfunc) ();

void foo(pvecfunc pvf) {
   vector int v = (vector int){1, 2, 3, 4};
   preansi (4, 4.0, v); /* { dg-error "AltiVec argument passed to unprototyped function" } */
}
