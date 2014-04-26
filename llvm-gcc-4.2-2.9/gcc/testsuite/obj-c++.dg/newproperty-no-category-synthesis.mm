/* APPLE LOCAL file radar 5180172 */
/* Compiler must err on use of @syntesize in a category implementation */
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

@interface Foo
{ 
    int x;
}
@end

@interface Foo(Manual)
@property int x;
@end

@implementation Foo(Manual)
@dynamic x;
//- (int)x { return x; }
//- (void)setX:(int)value { x = value; }
@end

@implementation Foo
@end


@interface Bar
{ 
    int x;
}
@end

@interface Bar(Synth)
@property int x;
@end

@implementation Bar(Synth)
@synthesize x; /* { dg-error "synthesize not allowed in a category's implementation" } */
@end

@implementation Bar
@end


@interface Baz
{ 
    int x;
}
@end

@interface Baz(Dynamic)
@property int x;
@end

@implementation Baz(Dynamic)
@dynamic x;
@end

@implementation Baz
@end

int main (void) {return 0;}
