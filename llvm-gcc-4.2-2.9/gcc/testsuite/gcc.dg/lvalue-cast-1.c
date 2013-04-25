/* APPLE LOCAL file non lvalue assign */
/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-options "-fnon-lvalue-assign -faltivec" } */
/* { dg-require-effective-target ilp32 } */

int foo(void) {

  char *p;
  long l;
  short s;
  vector unsigned int vui;
  volatile int *pvi;

  (long *)p = &l; /* { dg-warning "target of assignment not really an lvalue" } */
  ((long *)p)++;  /* { dg-warning "target of assignment not really an lvalue" } */
  (short)l = 2;   /* { dg-error "lvalue required as left operand of assignment" } */
  (long)s = 3;    /* { dg-error "lvalue required as left operand of assignment" } */
  (int)pvi = 4;   /* { dg-warning "target of assignment not really an lvalue" } */
  (int)pvi &= 5;  /* { dg-warning "target of assignment not really an lvalue" } */

  (vector float)vui = (vector float)(1.0, 2.0, 3.0, 4.0); /* { dg-warning "target of assignment not really an lvalue" } */

  return 0;
}
