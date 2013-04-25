/* { dg-do compile } */
/* { dg-options "-Winline -O2" } */
/* APPLE LOCAL wording 4598393 */
inline __attribute__ ((always_inline)) void t(void); /* { dg-error "the function body must appear before caller" "" } */
void
q(void)
{
  t(); 				/* { dg-error "called from here" "" } */
}
