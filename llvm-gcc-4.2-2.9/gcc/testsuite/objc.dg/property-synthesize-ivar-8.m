/* APPLE LOCAL file radar 5435299 */
/* Enforce order of generated ivars */
/* { dg-options "-m64 -std=c99 -lobjc" { target i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property -std=c99 -lobjc" { target arm*-*-darwin* } } */
/* { dg-do run { target i?86*-*-darwin* arm*-*-darwin* } } */


#ifdef __OBJC2__
#import <objc/Object.h>
#import <objc/runtime.h>
#include <assert.h>

@interface Test8 : Object
{
   int ivar1;
   int ivar2;
}
// order of @property declarations has no effect on ivar order
@property int prop5;
@property int prop4;
@property int prop6;
@property int prop1;
@property int prop3;
@property int prop2;
@end

@implementation Test8
// resulting ivar order should be: { ivar1; ivar2; prop3; prop4; prop5; prop6; }
@synthesize prop1 = ivar1;
@synthesize prop5;	// prop5 ivar is generated 1st after ivar2
@synthesize prop2 = ivar2;
@synthesize prop3, prop4, prop6;
@end

int main() {
   Ivar ivar, ivar2, ivar3, ivar4, ivar5, ivar6;
   ivar = class_getInstanceVariable([Test8 class], "ivar1");
   assert(ivar);

   ivar2 = class_getInstanceVariable([Test8 class], "ivar2");
   assert(ivar2);

   assert(ivar_getOffset(ivar) < ivar_getOffset(ivar2));

   ivar3 = class_getInstanceVariable([Test8 class], "prop5");
   assert(ivar3);
   assert(ivar_getOffset(ivar2) < ivar_getOffset(ivar3));

   ivar4 = class_getInstanceVariable([Test8 class], "prop3");
   assert(ivar4);
   assert(ivar_getOffset(ivar3) < ivar_getOffset(ivar4));

   ivar5 = class_getInstanceVariable([Test8 class], "prop4");
   assert(ivar5);
   assert(ivar_getOffset(ivar4) < ivar_getOffset(ivar5));

   ivar6 = class_getInstanceVariable([Test8 class], "prop6");
   assert(ivar6);
   assert(ivar_getOffset(ivar5) < ivar_getOffset(ivar6));

   return 0;
}

#endif

