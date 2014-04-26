/* APPLE LOCAL file mainline 4.3 2006-10-31 4134307 */
/* { dg-do compile } */
/* APPLE LOCAL inline fixups 5580320 */
/* { dg-options "-std=gnu89 -pedantic" } */
static int i;
extern int j;
extern inline int func1 (void) {
  return i++;  /* { dg-warning "static" } */
}
extern inline int func2 (void) {
  return j++;
}
inline int func3 (void) 
{
  return i++;
}
