/* APPLE LOCAL file radar 4947014 */
/* Check that 'retain' property does not call objc_setProperty/objc_getProperty
   with gc-only mode. */
/* { dg-options "-fobjc-gc-only -fobjc-new-property -mmacosx-version-min=10.5 -framework Foundation" } */
/* { dg-do run { target *-*-darwin* } } */
/* { dg-require-effective-target objc_gc } */

#import <Foundation/Foundation.h>

void objc_setProperty(id self, SEL _sel, ptrdiff_t offset, id value, BOOL atomic, BOOL shouldCopy)  {
   abort ();
}

id objc_getProperty(id self, SEL _sel, ptrdiff_t offset, BOOL atomic) {
	  abort ();
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
    return 0;
}

