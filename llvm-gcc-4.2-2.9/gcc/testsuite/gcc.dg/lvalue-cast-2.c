/* APPLE LOCAL file non lvalue assign */
/* Allow lvalue casts in conjunction with '&'.  */
/* { dg-do compile } */
/* { dg-options "-fnon-lvalue-assign" } */

int foo (void **ptr) {
  return 1;
}

int bar (void) {
  char *string;

  return foo ((void **)&((char *)string));  /* { dg-warning "argument to .&. not really an lvalue" } */
}
