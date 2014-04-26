/* APPLE LOCAL file radar 5277239 */
/* Test use of class method calls using property dot-syntax used for
   property 'getter' and 'setter' messaging. */
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

@interface INTF
- (int) METH;
- (void)setMETH:(int)value;
+ (void) c_method;
@end

@implementation INTF
+ (void) c_method
{
	int j = self.METH;	/* { dg-error "accessing unknown" } */
	self.METH = 2;	/* { dg-error "accessing unknown" } */
}

- (int) METH { return 1; }
- (void)setMETH:(int)value { }
@end

int main() {
    int i = INTF.METH ;	/* { dg-error "accessing unknown" } */
    INTF.METH = 1;	/* { dg-error "accessing unknown" } */
}
