/* APPLE LOCAL file radar 5435299 */
/* Test for synthesizing of one ivar for a synthesized property. */
/* { dg-options "-m64 -std=c99 -lobjc" { target i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property -std=c99 -lobjc" { target arm*-*-darwin* } } */
/* { dg-do run { target i?86*-*-darwin* arm*-*-darwin* } } */

#ifdef __OBJC2__
#import <objc/Object.h>
#import <objc/runtime.h>
#include <assert.h>

@interface Test1 : Object
@property int prop;
@end
@implementation Test1
@synthesize prop;
@end

int main() {
   unsigned int count;
   Ivar *list;
   Ivar ivar;
   list = class_copyIvarList([Test1 class], &count);
   assert(count == 1);
   ivar = class_getInstanceVariable([Test1 class], "prop");
   assert(ivar);
   return 0;
}

#endif
