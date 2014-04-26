/* APPLE LOCAL file 4492976 */
/* Test if ObjC constant string layout is checked properly, regardless of how
   constant string classes get derived.  */
/* { dg-options "-fnext-runtime -fno-constant-cfstrings -fconstant-string-class=XStr -m64 -fobjc-abi-version=1" } */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */

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

/* APPLE LOCAL begin objc2 */
#if OBJC_API_VERSION >= 2
extern Class _XStrClassReference;
#else
extern struct objc_class _XStrClassReference;
#endif
/* APPLE LOCAL end objc2 */

const XStr *appKey = @"MyApp";

/* LLVM LOCAL accept llvm syntax */
/* { dg-final { scan-assembler ".section.*__OBJC,__cstring_object" } } */
/* LLVM LOCAL accept llvm syntax */
/* { dg-final { scan-assembler ".quad\t__XStrClassReference\n\t.quad\t.*\n\t.long\t5.*\n\t.space( |\t)4\n" } } */
