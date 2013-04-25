/* APPLE LOCAL file radar 4436866 */
/* Check for proper declaration of @property. */
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

@interface Bar
{
  int iVar;
}
@property int FooBar /* { dg-warning "expected \\`@end\\' at end of input" } */
