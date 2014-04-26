/* APPLE LOCAL file 5782740 - blocks */
/* Test generation of copy/destroy helper function. */
/* { dg-do compile } */
/* { dg-options "-mmacosx-version-min=10.5 -ObjC -fblocks" { target powerpc*-*-darwin* i?86*-*-darwin* x86_64-*-darwin* } } */
/* { dg-options "-iphoneos-version-min=4.0 -ObjC -fblocks" { target arm*-*-darwin* } } */

#import <Cocoa/Cocoa.h>

@interface Root @end

extern bar(double (^cp)(int));

@implementation Root

- (void)example {
 int x;
 __block int y;
 NSAutoreleasePool *pool = [NSAutoreleasePool new];
 NSAutoreleasePool *relpool = [NSAutoreleasePool new];
 bar(^(int z){ y = x+z;  [pool drain]; if (y) y++; [relpool release]; return y+2.0; }); 
}
@end

int main()
{
	int x = 10;
	int (^P) (void) = ^ { return x; };

	int (^Q) (void) = ^ { return P(); };
}
/* { dg-final { scan-assembler "___copy_helper_block_" } } */
/* { dg-final { scan-assembler "___destroy_helper_block_" } } */
