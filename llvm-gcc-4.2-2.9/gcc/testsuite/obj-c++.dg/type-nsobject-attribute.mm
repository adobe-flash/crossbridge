/* APPLE LOCAL file radar 5595325 */
/* Test for implementation of __attribute__ ((NSObject)) to make a struct pointer
   a first class objective-c pointer to object. */
/* { dg-options "-mmacosx-version-min=10.6 -framework Foundation" } */
/* { dg-do run { target *-*-darwin* } } */

#import <Foundation/Foundation.h>

typedef struct CGColor * __attribute__ ((NSObject)) CGColorRef;
static int count;
static CGColorRef tmp = 0;

@interface HandTested : NSObject {
@public
    CGColorRef x;
}
@property(copy) CGColorRef x;
@end

void objc_setProperty(id self, SEL _sel, ptrdiff_t offset, id value, BOOL atomic, BOOL shouldCopy)  {
  if (count != 0 || !shouldCopy)
    abort ();
  ++ count;
  ((HandTested *)self)->x = value;
}

id objc_getProperty(id self, SEL _sel, ptrdiff_t offset, BOOL atomic) {
     if (count != 1)
       abort();
     return (id)((HandTested *)self)->x;
}

@implementation HandTested
@synthesize x=x;
@end

int main(char *argc, char *argv[]) {
    HandTested *to = [HandTested new];
    to.x = tmp;  // setter
    if (tmp != to.x)
      abort ();
    return 0;
}

