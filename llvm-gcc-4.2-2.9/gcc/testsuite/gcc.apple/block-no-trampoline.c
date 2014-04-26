/* APPLE LOCAL file radar 6230701 */
/* Test that no trampoline is generated for this test case. */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-mmacosx-version-min=10.6 -ObjC -framework Foundation -O0" { target *-*-darwin* } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#import <Foundation/NSObject.h>

#import <stdio.h>

extern void abort(void);
static void __enable_execute_stack ()
{
	abort();
}

@interface A : NSObject
- (int)bork:(int)i;
@end

@implementation A
- (int)bork:(int)i {
  printf("i == %d\n", i);
  return i + 37;
}
@end

@interface B : A
- (id)foo:(id)i;
@end

@implementation B
- (id)foo:(id)i {
  printf("Hello world\n");
}

- (id)bar:(int)i {
  int  new;

  [self foo:
    ^() {
      __block new;
      new = [super bork:i];
    }];
}
@end

int main() {
  B *b = [[B alloc] init];

  [b foo:0];
  [b bar:0];
  [b release];
  return 0;
}

