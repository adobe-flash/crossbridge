/* APPLE LOCAL file 5782740 - blocks */
/* Test generation of copy/destroy helper function. */
/* { dg-do compile } */
/* { dg-options "-mmacosx-version-min=10.5 -ObjC++ -fblocks" { target *-*-darwin* } } */

#import <Cocoa/Cocoa.h>

@interface Root @end

extern void bar(double (^cp)(int));

@implementation Root

- (void)example {
 __block int y, x;
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
