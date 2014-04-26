/* APPLE LOCAL file constant CFStrings */
/* Test whether the __builtin__CFStringMakeConstantString
   "function" generates compile-time objects with the correct layout. */
/* Developed by Ziemowit Laski <zlaski@apple.com>.  */

/* { dg-do compile { target *-*-darwin* } } */
/* { dg-require-effective-target ilp32 } */
/* { dg-options "-fconstant-cfstrings" } */

typedef const struct __CFString *CFStringRef;

#ifdef __CONSTANT_CFSTRINGS__
#define CFSTR(STR)  ((CFStringRef) __builtin___CFStringMakeConstantString (STR))
#else
#error __CONSTANT_CFSTRINGS__ not defined
#endif

extern int cond;
extern const char *func(void);

CFStringRef s0 = CFSTR("Hello" "there");

void foo(void) {
  const CFStringRef s1 = CFSTR("Str1");

  s0 = s1;
}

/* LLVM LOCAL begin allow for comment after numbers */
/* { dg-final { scan-assembler "\\.long\[ \\t\]+___CFConstantStringClassReference\n\[ \\t\]*\\.long\[ \\t\]+1992.*\n\[ \\t\]*\\.long\[ \\t\]+LC.*\n\[ \\t\]*\\.long\[ \\t\]+4.*\n" } } */
/* { dg-final { scan-assembler "\\.long\[ \\t\]+___CFConstantStringClassReference\n\[ \\t\]*\\.long\[ \\t\]+1992.*\n\[ \\t\]*\\.long\[ \\t\]+LC.*\n\[ \\t\]*\\.long\[ \\t\]+10.*\n" } } */
/* LLVM LOCAL end */
