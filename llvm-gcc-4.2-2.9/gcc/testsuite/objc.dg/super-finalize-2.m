/* APPLE LOCAL file radar 4757423 */
/* Check for warnings about missing [super finalize] calls.  */

/* { dg-do compile } */

@interface Foo {
  void *isa;
}
- (void) finalize;
- (void) some_other;
@end

@interface Bar: Foo {
  void *casa;
}
- (void) finalize0;
@end

@interface Baz: Bar {
  void *usa;
}
- (void) finalize;
@end

@implementation Foo
- (void) finalize {
  isa = 0;   /* Should not warn here.  */
}
- (void) some_other {
  isa = (void *)-1;
}
@end

@implementation Bar
- (void) finalize0 {
  casa = 0;
  [super some_other];  /* Should not warn here.  */
} 
@end

@implementation Baz
- (void) finalize {
  usa = 0;
  [super finalize0]; 
} /* { dg-warning "method possibly missing a .super finalize. call" } */
@end
