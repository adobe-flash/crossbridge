/* APPLE LOCAL file radar 5781140 */
/* Check for correct treatment of empty struct when computing ivar layout. */
/* { dg-do run { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-mmacosx-version-min=10.5 -framework Foundation -fobjc-gc" } */
/* { dg-require-effective-target objc_gc } */

#import <Foundation/Foundation.h>
#import <objc/runtime.h>


//  gcc-4.2 -c test2.m -fobjc-gc
@interface F  : NSObject { id id1; struct { } x; uintptr_t i; id id2; } @end
@implementation F @end

int main()
{
	
  const char *layout = class_getIvarLayout([F class]);

  /* Desired layout: skip 0, scan 2 (isa, ivar); skip 1 (uintptr_t i), scan 1 (id id2). */
  if (!layout  ||  0 != strcmp(layout, "\x02\x11"))
        abort ();
  return 0;
}

