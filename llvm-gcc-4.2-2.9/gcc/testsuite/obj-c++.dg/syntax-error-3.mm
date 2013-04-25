/* APPLE LOCAL file 4093475 */
/* Yet another stray infinite loop... */
/* { dg-do compile } */

@interface Foo
{
  int x;
  int y;
}
- (int) foo ; { /* { dg-error "stray .\{. between Objective\\-C\\+\\+ methods" } */
@end
