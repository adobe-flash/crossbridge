/* APPLE LOCAL file AltiVec */
/* Comma expressions are not considered constant per C99, even if their
   constituent elements are.  */
/* { dg-do compile } */

int i = (1, 2);  /* { dg-error "initializer element is not constant" } */
