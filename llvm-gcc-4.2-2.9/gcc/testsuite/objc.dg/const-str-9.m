/* Test if ObjC constant strings get placed in the correct section.  */
/* Contributed by Ziemowit Laski <zlaski@apple.com>  */

/* APPLE LOCAL constant cfstrings */
/* { dg-options "-fnext-runtime -fno-constant-cfstrings" } */
/* APPLE LOCAL ARM objc2 */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* APPLE LOCAL radar 4492976 */
/* { dg-require-effective-target ilp32 } */

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
/* { dg-final { scan-assembler ".long\t__NSConstantStringClassReference\n\t.long\t.*\n\t.long\t5.*\n" } } */
