/* APPLE LOCAL file constant CFStrings  - modified for radar 2996215 */
/* Test the -Wnonportable-cfstrings option, which should give
   warnings if non-ASCII characters are embedded in constant
   CFStrings.  This will only work on MacOS X 10.2 and later.  */
/* Developed by Ziemowit Laski <zlaski@apple.com>.  */

/* { dg-do compile { target *-*-darwin* } } */
/* { dg-options "-fconstant-cfstrings" } */

#import <Foundation/NSString.h>
#import <CoreFoundation/CFString.h>

#ifndef __CONSTANT_CFSTRINGS__
#error The -fconstant-cfstrings option is not functioning properly
#endif

void foo(void) {
  NSString *s1 = @"Compile-time string literal";
  CFStringRef s2 = CFSTR("Compile-time string literal");
  NSString *s3 = @"Non-ASCII literal - \222";         /* { dg-error "input conversion stopped due to an input byte that does not belong to the input codeset UTF-8" } */
  CFStringRef s4 = CFSTR("\222 - Non-ASCII literal"); /* { dg-error "input conversion stopped due to an input byte that does not belong to the input codeset UTF-8" } */
  CFStringRef s5 = CFSTR("Non-ASCII (\222) literal"); /* { dg-error "input conversion stopped due to an input byte that does not belong to the input codeset UTF-8" } */
  NSString *s6 = @"\0Embedded NUL";
  CFStringRef s7 = CFSTR("Embedded \0NUL"); 
  CFStringRef s8 = CFSTR("Embedded NUL\0"); 
}
