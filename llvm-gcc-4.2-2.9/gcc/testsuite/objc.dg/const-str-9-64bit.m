/* APPLE LOCAL file 4492976 */
/* Test if ObjC constant strings get placed in the correct section.  */
/* { dg-options "-fnext-runtime -m64 -fobjc-abi-version=1 -fno-constant-cfstrings" } */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */

#include <objc/Object.h>

@interface NSConstantString: Object {
  char *cString;
  unsigned int len;
}
@end

/* APPLE LOCAL begin objc2 */
#if OBJC_API_VERSION >= 2
extern Class _NSConstantStringClassReference;
#else
extern struct objc_class _NSConstantStringClassReference;
#endif
/* APPLE LOCAL end objc2 */

static const NSConstantString *appKey = @"MyApp";

/* LLVM LOCAL accept llvm syntax */
/* { dg-final { scan-assembler ".section.*__OBJC,__cstring_object" } } */
/* LLVM LOCAL accept llvm syntax */
/* { dg-final { scan-assembler ".quad\t__NSConstantStringClassReference\n\t.quad\t.*\n\t.long\t5.*\n\t.space( |\t)4\n" } } */
