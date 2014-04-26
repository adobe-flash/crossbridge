/* APPLE LOCAL file deprecated use in deprecated 6425499 */

__attribute__((deprecated))
void depFunc1() {}

__attribute__((deprecated))
void depFunc2() { depFunc1(); }

@interface Foo
-(void)deprecatedMethod1 __attribute__((deprecated));
-(void)deprecatedMethod2 __attribute__((deprecated));
@end
@implementation Foo

-(void)deprecatedMethod1 {
}

-(void)deprecatedMethod2 {
  depFunc1();
  [self deprecatedMethod1];
}

@end
