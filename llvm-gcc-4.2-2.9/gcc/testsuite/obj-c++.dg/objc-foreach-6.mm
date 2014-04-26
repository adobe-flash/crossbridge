/* APPLE LOCAL file radar 4529200 */
/* Test that when the object to be iterated is the return value of a method,
   it compiles without error. */
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do compile } */
#include <Foundation/Foundation.h>

int main (int argc, char const* argv[]) {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    NSDictionary *dict = [NSDictionary dictionaryWithObjectsAndKeys:@"o1", @"k1", @"o2", @"k2", nil];
    for (id key in [dict allKeys]) {
            NSLog(@"key is %@", key);
    }
    [pool release];
    return 0;
}
