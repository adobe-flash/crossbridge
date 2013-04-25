/* APPLE LOCAL file radar 5741070  */
/*  Classes used to dispatch methods should have debug information about
    them written out, even if they are not assigned to any variable.  */
/* { dg-do compile } */
/* { dg-options "-gdwarf-2 -O0  -dA -c" } */
/* { dg-final { scan-assembler "\"NSBundle\\\\0\".*DW_AT_name" } } */

#import <Foundation/Foundation.h>

int main (int argc, const char * argv[]) {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

    // insert code here...
    NSLog(@"Hello, World - I am %@!", [[NSBundle mainBundle] bundlePath]);
    [pool drain];
    return 0;
}
