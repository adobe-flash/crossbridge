/* APPLE LOCAL file radar 5159695 */
/* Test that proper warning is issued when class continuation is used with mismatched attributes. */
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

@interface Foo
{
    id assignVsRetainTest;
}

@property(readonly, nonatomic, assign) id assignVsRetainTest;
@end

@interface Foo()
@property(readwrite, nonatomic, retain) id assignVsRetainTest; /* { dg-warning "property 'assignVsRetainTest' attribute in 'Foo' class continuation does not match class 'Foo' property" } */
@end

@implementation Foo
@synthesize assignVsRetainTest;
@end

int main (int argc, const char * argv[]) {
    return 0;
}
