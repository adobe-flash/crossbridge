/* APPLE LOCAL file radar 5159707 */
/* Check that appropriate warning on property type mismatch is issued. */
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do run { target *-*-darwin* } } */

@class Bar;
@class Baz;

@interface Foo
{
    Baz *typeMismatchError;
}

@property(readonly, nonatomic, assign) Baz *typeMismatchError;
@end

@interface Foo()
@property(readwrite, nonatomic, assign) Bar *typeMismatchError; /* { dg-warning "property 'typeMismatchError' type in 'Foo' class continuation does not match class 'Foo' property" } */
@end

@implementation Foo
@synthesize typeMismatchError;
@end

int main (int argc, const char * argv[]) {
    return 0;
}
