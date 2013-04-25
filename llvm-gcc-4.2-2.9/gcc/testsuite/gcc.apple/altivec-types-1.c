/* APPLE LOCAL file -Wno-altivec-long-deprecated */
/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-require-effective-target ilp32 } */
/* { dg-options "-maltivec -Wno-altivec-long-deprecated" } */

/* These should not get warnings for 32-bit code when the warning is
   disabled.  */

__vector long vl;
__vector unsigned long vul;
__vector signed long vsl;
__vector __bool long int vbli;
__vector long int vli;
__vector unsigned long int vuli;
__vector signed long int vsli;
