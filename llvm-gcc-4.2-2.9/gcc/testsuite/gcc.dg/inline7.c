/* APPLE LOCAL file mainline 4.3 2006-10-31 4134307 */
/* { dg-do compile } */
/* { dg-options "-std=gnu89" } */
extern inline void func1 (void) {
  static int i;  /* { dg-warning "static" } */
}
inline void func3 (void) 
{
  static int i;
}
