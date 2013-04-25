/* APPLE LOCAL file radar 4994854 */
/* Check for continuation anonymous category and new property decl in it as well. */
/* { dg-options "-mmacosx-version-min=10.5 -fobjc-new-property" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property" { target arm*-*-darwin* } } */
/* { dg-do run { target *-*-darwin* } } */

// Should compile cleanly

// Baseline "nil continuation":
@interface ToBeContinued
@end

@interface ToBeContinued()
@end

@implementation ToBeContinued
@end


// Add methods and properties:
@interface TwoStep
{
    int x;
}
- (int)one;
@end

@interface TwoStep()
- (int)two;
@property int x;
@end

@implementation TwoStep
- (int)one { return 1; }
- (int)two { return 2; }
@synthesize x;
@end


// Multiple continuations:
@interface Trilogy
- (int)one;
@end

@interface Trilogy()
- (int)two;
@end

@interface Trilogy()
- (int)three;
@end

@implementation Trilogy
- (int)one { return 1; }
- (int)two { return 2; }
- (int)three { return 3; }
@end


// "Property extension" continuation:
@interface Immutable
{
    int x;
}
@property(readonly) int x;
@end

@interface Immutable()
@property(readwrite) int x;  // privately re-declare to have a public getter and a private setter
@end

@implementation Immutable
@synthesize x;
@end


int main (void) {return 0;}
