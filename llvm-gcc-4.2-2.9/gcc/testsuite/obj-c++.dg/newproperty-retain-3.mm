/* APPLE LOCAL file radar 5610134 */
/* Test that non-fragile 'ivar' offset is generated for a 'retain'
   setter/getter API. */
/* { dg-options "-m64 -mmacosx-version-min=10.5 -framework Cocoa" } */
/* { dg-do run { target powerpc*-*-darwin* i?86*-*-darwin* } } */

#import <Cocoa/Cocoa.h>

@interface Base:NSObject
//{ int whatever; } // uncomment this, stops crashing
@property int whatever;
@end

@interface Oops:Base
@property (retain) id oops;
@end

@implementation Base
@synthesize whatever;
@end

@implementation Oops
@synthesize oops;

-(void)whatthe {
 NSLog(@"1. %x",self.oops);
 self.whatever=1;
 NSLog(@"2. %x",self.oops);
}

@end

int main(int ac,char **av) {
  [NSAutoreleasePool new];
  [[Oops new] whatthe];
  return 0;
}
