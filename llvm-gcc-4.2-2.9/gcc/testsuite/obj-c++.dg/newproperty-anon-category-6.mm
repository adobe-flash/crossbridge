/* APPLE LOCAL file radar 4994854 */
// Added methods and properties not implemented:
/* { dg-options "-mmacosx-version-min=10.5 -fobjc-new-property" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property" { target arm*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

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
@end				/* { dg-warning "property 'x' requires method '-x' to be defined" } */
/* { dg-warning "property 'x' requires the method 'setX:' to be defined" "" { target *-*-* } 21 } */
/* { dg-warning "incomplete implementation of class 'TwoStep'" "" { target *-*-* } 21 } */
/* { dg-warning "method definition for '-two' not found" "" { target *-*-* } 21 } */

int main (void) {return 0;}
