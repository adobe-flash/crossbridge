/* APPLE LOCAL file radar 5338634 */
/* Check that no error is issued when setter name is the prefix for property name. */
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do run { target *-*-darwin* } } */

@interface Foo
{
    int intSetter; 
}
@property(setter=intSet:) int intSetter; 
@end

@implementation Foo
@synthesize intSetter; 
@end

int main()
{
    return 0;
}

