/* APPLE LOCAL file 4149909 */
/* Test if ObjC constant string layout is checked properly, regardless of how
   constant string classes get derived.  */
/* Contributed by Ziemowit Laski <zlaski@apple.com>  */

/* { dg-options "-fnext-runtime -fno-constant-cfstrings -fconstant-string-class=XStr" } */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-require-effective-target ilp32 } */

#include <objc/Object.h>

@interface XString: Object {
@protected
    char *bytes;
}
@end

@interface XStr : XString {
@public
    unsigned int len;
}
@end

#if OBJC_API_VERSION >= 2
extern Class _XStrClassReference;
#else
extern struct objc_class _XStrClassReference;
#endif

const XStr *appKey = @"MyApp";

/* LLVM LOCAL accept llvm syntax */
/* { dg-final { scan-assembler ".section.*__OBJC,__cstring_object" } } */
/* LLVM LOCAL accept llvm syntax */
/* { dg-final { scan-assembler ".long\t__XStrClassReference\n\t.long\t.*\n\t.long\t5.*\n" } } */
