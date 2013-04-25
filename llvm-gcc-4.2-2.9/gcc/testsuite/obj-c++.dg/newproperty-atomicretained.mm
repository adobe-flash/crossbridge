/* APPLE LOCAL file radar 4947014 */
/* Check that 'retain' property call objc_setProperty/objc_getProperty as it should. */
/* { dg-options "-fobjc-gc -fobjc-new-property -mmacosx-version-min=10.5 -framework Foundation" } */
/* { dg-do run { target *-*-darwin* } } */
/* { dg-require-effective-target objc_gc } */

#import <Foundation/Foundation.h>

static int count;

void objc_setProperty(id self, SEL _sel, ptrdiff_t offset, id value, BOOL atomic, BOOL shouldCopy)  {
  if (count != 1)
   abort ();
  ++count;
}

id objc_getProperty(id self, SEL _sel, ptrdiff_t offset, BOOL atomic) {
	if (count != 0)
	  abort ();
	++count;
        return nil;
}
@interface Test : NSObject {
	id x;
}
@property(retain) id x;
@end

@implementation Test
@synthesize x;
@end



int main(int argc, char *argv[]) {
    id pool = [NSAutoreleasePool new];

    Test *test = [Test new];
    test.x = test.x;  // call getter, then setter on retained property
    if (count != 2)
      abort ();
    return 0;
}

