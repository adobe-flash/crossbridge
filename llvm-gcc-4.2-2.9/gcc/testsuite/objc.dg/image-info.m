/* Check if the '-freplace-objc-classes' option causes the
   __OBJC,__image_info section to be emitted.  This is only
   usable on MacOS X 10.3 and later. */
/* Contributed by Ziemowit Laski <zlaski@apple.com>.  */
/* { dg-options "-freplace-objc-classes" } */
/* APPLE LOCAL ARM objc2 */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* APPLE LOCAL radar 4894756 */
/* { dg-require-effective-target ilp32 } */

#ifndef __NEXT_RUNTIME__
#error Feature not currently supported by the GNU runtime
#endif

#include <objc/objc.h>
/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

extern void abort(void);

#define CHECK_IF(expr) if(!(expr)) abort();

@interface Base: Object {
@public
  int a;
  float b;
  char c;
}
- init;
@end

@implementation Base
- init {
  [super init];
  a = 123;
  b = 1.23;
  c = 'c';
  return self;
}
@end

/* LLVM LOCAL accept llvm syntax */
/* { dg-final { scan-assembler ".section.*__OBJC,__image_info.*\n\t.align.*\nL_OBJC_IMAGE_INFO.*:.*\n\t(.long\t0|.space\t4)\n\t.long\t1" } } */
