/* APPLE LOCAL file radar 4491211 */
/* Test that in the presence of instantiated template as return method parameter
   that is memory bound, we still generate objc_msgSend_stret for all platforms. */
/* { dg-do compile { target *-*-darwin* } } */
/* APPLE LOCAL radar 4492976 */
/* { dg-require-effective-target ilp32 } */

@interface Joiner {
}

-(void) joinWithAttributes;
@end

template<class A, class B, class C> struct mytuple {
 A a_;
 B b_;
 C c_;
};


@interface Joiner (PRIVATE)
- (mytuple<int*, int*, int*>) addTrack;
@end

@implementation Joiner
-(void) joinWithAttributes {
 [self addTrack];
}

@end
/* { dg-final { scan-assembler "objc_msgSend_stret" } } */
