/* APPLE LOCAL file 4506977 */
/* { dg-do compile } */
/* { dg-options "-Os" } */
/* { dg-final { scan-assembler-not "li r" } } */
long double global_ld_var = 0.0;
long double foo(void) {
  if (global_ld_var == 0.0) {
     return 99.0;
  }
  return global_ld_var;
}
