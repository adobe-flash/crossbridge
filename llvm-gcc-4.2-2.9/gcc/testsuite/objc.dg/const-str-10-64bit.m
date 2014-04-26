/* APPLE LOCAL file 4492976 */
/* Test if ObjC constant string layout is checked properly, regardless of how
   constant string classes get derived.  */
/* { dg-options "-fnext-runtime -fno-constant-cfstrings -m64 -fobjc-abi-version=1" } */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */

#include <objc/Object.h>

@interface NSString: Object
@end

@interface NSSimpleCString : NSString {
@protected
    char *bytes;
    unsigned int numBytes;
}
@end
    
@interface NSConstantString : NSSimpleCString
@end

/* APPLE LOCAL begin objc2 */
#if OBJC_API_VERSION >= 2
extern Class _NSConstantStringClassReference;
#else
extern struct objc_class _NSConstantStringClassReference;
#endif
/* APPLE LOCAL end objc2 */

const NSConstantString *appKey = @"MyApp";

/* LLVM LOCAL accept llvm syntax */
/* { dg-final { scan-assembler ".section.*__OBJC,__cstring_object" } } */
/* LLVM LOCAL accept llvm syntax */
/* { dg-final { scan-assembler ".quad\t__NSConstantStringClassReference\n\t.quad\t.*\n\t.long\t5.*\n\t.space( |\t)4\n" } } */
