/* APPLE LOCAL file radar 5887355 */
/* Test correct generation of utf16-string in a function template. */
/* { dg-options "-framework Cocoa" } */
/* { dg-do run { target *-*-darwin* } } */

#import <Cocoa/Cocoa.h>

template <typename T> NSString* test (T i) { return @"æble"; }

main () { 
   NSLog (@"string: %@", test(42)); 
   return 0;
}
