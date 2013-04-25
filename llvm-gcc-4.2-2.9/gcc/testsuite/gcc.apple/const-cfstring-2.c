/* APPLE LOCAL file constant CFStrings */
/* Test whether the __builtin__CFStringMakeConstantString 
   "function" fails gracefully when used without the
   -fconstant-cfstrings flag.  */
/* Developed by Ziemowit Laski <zlaski@apple.com>.  */

/* { dg-options "-fno-constant-cfstrings" } */
/* { dg-do compile { target *-*-darwin* } } */

typedef const struct __CFString *CFStringRef;

const CFStringRef S = ((CFStringRef)__builtin___CFStringMakeConstantString("Testing"));
/* { dg-error "built-in" "built-in function .* requires .* flag" { target *-*-* } 12 } */
