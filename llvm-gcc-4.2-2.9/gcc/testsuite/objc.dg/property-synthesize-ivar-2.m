/* APPLE LOCAL file radar 5435299 */
/* Test for property backed by generated ivar with specified name. */
/* { dg-options "-m64 -std=c99 -lobjc" { target i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property -std=c99 -lobjc" { target arm*-*-darwin* } } */
/* { dg-do run { target i?86*-*-darwin* arm*-*-darwin* } } */


#ifdef __OBJC2__
#import <objc/Object.h>
#import <objc/runtime.h>
#include <assert.h>

@interface Test2 : Object
@property int prop;
@end
@implementation Test2
@synthesize prop = prop_ivar;
@end

int main() {
   unsigned int count;
   Ivar *list;
   Ivar ivar;
   list = class_copyIvarList([Test2 class], &count);
   assert(count == 1);
   ivar = class_getInstanceVariable([Test2 class], "prop");
   assert(!ivar);
   ivar = class_getInstanceVariable([Test2 class], "prop_ivar");
   assert(ivar);
   return 0;
}

#endif
