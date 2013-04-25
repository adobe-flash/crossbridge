/* APPLE LOCAL file radar 5839812 - synthesized methods should be given
   the location of the '@synthesize' statement.  */
/*  AppDelegate.m, AppDelegate.h  */
/* { dg-do compile } */
/* { dg-options " -O0 -gdwarf-2 -c -dA -mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* LLVM LOCAL allow for extra space */
/* { dg-final { scan-assembler "AppDelegate.m ?:12" } } */

#import "AppDelegate.h"

@implementation AppDelegate

@synthesize name;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    self.name = @"Fred";
}
 
@end
