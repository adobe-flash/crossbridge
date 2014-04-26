/* APPLE LOCAL file constant cfstrings */
/* Test the -fconstant-cfstrings option for constructing
   compile-time immutable CFStrings, and their interoperation
   with both Cocoa and CoreFoundation.  This will only work
   on MacOS X 10.1.2 and later.  */
/* Developed by Ziemowit Laski <zlaski@apple.com>.  */

/* { dg-do run { target *-*-darwin* } } */
/* { dg-options "-fconstant-cfstrings -framework Cocoa" } */
/* APPLE LOCAL ARM Cocoa not available on arm-darwin targets */
/* { dg-skip-if "" { arm*-*-darwin* } { "*" } { "" } } */

#import <Foundation/NSString.h>
#import <CoreFoundation/CFString.h>
#include <stdlib.h>

void printOut(NSString *str) {
  NSLog(@"The value of str is: %@", str);
}

CFStringRef s0a = CFSTR("Compile-time string literal");
CFStringRef s0b = CFSTR("Compile-time string literal");

void checkNSRange(NSRange r) {
  if (r.location != 6 || r.length != 5) {
    printOut(@"Range check failed");
    abort();
  }
}

void checkCFRange(CFRange r) {
  if (r.location != 6 || r.length != 5) {
    printOut(@"Range check failed");
    abort();
  }
}

int main(void) {
  const NSString *s1 = @"Compile-time string literal";
  CFStringRef s2 = CFSTR("Compile-time string literal");

  checkNSRange([@"Hello World" rangeOfString:@"World"]);
  checkNSRange([(id)CFSTR("Hello World") rangeOfString:@"World"]);
  checkNSRange([@"Hello World" rangeOfString:(id)CFSTR("World")]);
  checkNSRange([(id)CFSTR("Hello World") rangeOfString:(id)CFSTR("World")]);

  checkCFRange(CFStringFind((CFStringRef)@"Hello World", (CFStringRef)@"World", 0));
  checkCFRange(CFStringFind(CFSTR("Hello World"), (CFStringRef)@"World", 0));
  checkCFRange(CFStringFind((CFStringRef)@"Hello World", CFSTR("World"), 0));
  checkCFRange(CFStringFind(CFSTR("Hello World"), CFSTR("World"), 0));

  /* Check for string uniquing.  */
  if (s0a != s0b || s0a != s2 || s1 != (id)s2) {
    NSLog(@"String uniquing failed");
    abort ();
  }

  return 0;
}
