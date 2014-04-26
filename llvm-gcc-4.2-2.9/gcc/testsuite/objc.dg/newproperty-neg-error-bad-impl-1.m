/* APPLE LOCAL file radar 5265608 */
/* Catch error when property synthesis is not in an @implementation context. */
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

@interface BugBugBug
{
  id key;
}
@property(copy) id key;
@end

@synthesize key=key; /* { dg-warning "@synthesize property must be in implementation context" } */
