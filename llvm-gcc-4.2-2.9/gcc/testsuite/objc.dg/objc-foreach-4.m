/* APPLE LOCAL file radar 4507230 */
/* Test for valid objc objects used in a for-each statement. */
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do compile } */
#include <Foundation/Foundation.h>

// gcc -o foo foo.m -framework Foundation

int main (int argc, char const* argv[]) {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    NSArray * arr = [NSArray arrayWithObjects:@"A", @"B", @"C", nil];
    for (NSString * foo in arr) { 
      NSLog(@"foo is %@", foo);
    }
    [pool release];
    return 0;
}
