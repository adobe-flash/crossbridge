/* APPLE LOCAL file 4492976 */
/* Test if constant CFStrings get placed in the correct section.  */

/* { dg-options "-mmacosx-version-min=10.5 -fconstant-cfstrings -m64" } */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */

typedef const struct __CFString * CFStringRef;
CFStringRef appKey = (CFStringRef) @"com.apple.soundpref";

/* LLVM LOCAL accept llvm syntax */
/* { dg-final { scan-assembler ".section.*__DATA,__cfstring" } } */
/* LLVM LOCAL accept llvm syntax */
/* { dg-final { scan-assembler ".quad\t___CFConstantStringClassReference\n\t.long\t1992.*\n\t.space( |\t)4\n\t.quad\t.*\n\t.quad\t19\n" } } */
