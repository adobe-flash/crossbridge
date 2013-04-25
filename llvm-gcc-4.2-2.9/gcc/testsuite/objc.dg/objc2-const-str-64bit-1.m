/* APPLE LOCAL file 4719165 */
/* Test for new way of representing constant string structure */
/* { dg-options "-mmacosx-version-min=10.5 -fnext-runtime -m64 -fobjc-abi-version=2 -fno-constant-cfstrings" } */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */

@interface NSConstantString { id isa; const char *c; int l; } @end
@implementation NSConstantString @end

int _objc_empty_cache = 0;
int _objc_empty_vtable = 0;

int main() {
    return (int)(long)@"foo";
}
/* LLVM LOCAL accept llvm syntax */
/* { dg-final { scan-assembler "(LC1|__unnamed_1_0|___unnamed_1):.*\n\t.quad\t_OBJC_CLASS_\\\$_NSConstantString" } } */
