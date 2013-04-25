/* APPLE LOCAL file radar 4494634 */
/* Test that no strict aliasing warning is issued with CFStrings and 
   -fstrict-aliasing. */
/* { dg-do compile { target *-*-darwin* } } */
/* { dg-options "-Wall -fconstant-cfstrings -fstrict-aliasing -Wno-format" } */
#import <Foundation/Foundation.h>

int main (int argc, const char * argv[]) {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

    // insert code here...
    NSLog(@"Hello, World!");
    
    NSString* foo = @"foo";
    NSLog(foo);
    
    [pool release];
    return 0;
}
