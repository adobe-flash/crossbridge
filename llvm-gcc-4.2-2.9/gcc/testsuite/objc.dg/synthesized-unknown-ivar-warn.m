/* APPLE LOCAL file radar 5847641 */
/* Warn if an unknown ivar has been specified in @synthesize. This is for default
   abi only. */
/* { dg-options "-fobjc-abi-version=1" } */

@interface MyStupidClass {
    id _bar;
}
@property(readwrite, retain) id foo;
@end

@implementation MyStupidClass
@synthesize foo = _foo;  /* { dg-warning "ivar name \'_foo\' specified on the synthesized property" } */
			 /* { dg-error "synthesized property 'foo' must either be named the same as a compatible" "" { target *-*-* } 13 } */

- (id)foo {
   return _bar;
}
- (void)setFoo:(id)newFoo {
}
@end	

