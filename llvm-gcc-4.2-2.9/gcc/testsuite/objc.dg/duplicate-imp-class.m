/* APPLE LOCAL file 4689268 */
/* Check that compiler gracefully exits on duplicate implementaiton of
   same class. */
@interface Foo
@end

@implementation Foo
- (void) bar { }		/* { dg-error "previous definition of" } */
@end

@implementation Foo
- (void) bar { }		/* { dg-error "reimplementation of class \'Foo\'" } */
				/* { dg-error "redefinition of" "" { target *-*-* } 12 } */
@end

