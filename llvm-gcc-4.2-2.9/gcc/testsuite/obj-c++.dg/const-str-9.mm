/* APPLE LOCAL file mainline */
/* Test if ObjC constant strings get placed in the correct section.  */
/* Contributed by Ziemowit Laski <zlaski@apple.com>  */

/* { dg-options "-fnext-runtime -fno-constant-cfstrings" } */
/* APPLE LOCAL ARM radar 5804096 */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-require-effective-target ilp32 } */

#include <objc/Object.h>

@interface NSConstantString: Object {
  char *cString;
  unsigned int len;
}
@end

#   if (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5 || __OBJC2__)
extern Class _NSConstantStringClassReference;
#else
extern struct objc_class _NSConstantStringClassReference;
#endif

const NSConstantString *appKey = @"MyApp";

/* LLVM LOCAL accept llvm syntax */
/* { dg-final { scan-assembler ".section.*__OBJC,__cstring_object" } } */
/* LLVM LOCAL accept llvm syntax */
/* { dg-final { scan-assembler ".long\t__NSConstantStringClassReference\n\t.long\t.*\n\t.long\t5.*\n" } } */
