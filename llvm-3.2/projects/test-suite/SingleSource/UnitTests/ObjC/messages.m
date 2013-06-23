#import <Foundation/NSObject.h>
#include <stdio.h>

typedef struct {
  int x;
  int y;
  int z[10];
} MyPoint;

@interface A : NSObject {
}

-(id) print0;
-(id) print1: (int) a0;
-(id) print2: (int) a0 and: (char*) a1 and: (double) a2;
-(id) takeStruct: (MyPoint) p;
@end

void foo(id a) {
  int i;
  MyPoint pt = { 1, 2};

  [a print0];
  [a print1: 10];
  [a print2: 10 and: "hello" and: 2.2];
  [a takeStruct: pt ];
  
  void *s = @selector(print0);
  for (i=0; i<2; ++i)
    [a performSelector:s];
}

@implementation A
-(id) print0 {
  printf("I am A! Hear me roar!\n");
  return nil;
}

-(id) print1: (int) a0 {
  printf("I AM A! HEAR ME ROAR (%d TIMES LOUDER)!\n", a0);
  return nil;
}

-(id) print2: (int) a0  and: (char*) a1 and: (double) a2{
  printf("I AM A! I CAN ROAR WITH ARGUMENTS LIKE %d, %s, and %f!\n", a0, a1, a2);
  return nil;
}

-(id) print2: (int) arg {
  printf("By golly you set x to %d\n", arg);
  return nil;
}

-(id) takeStruct: (MyPoint) p {
  printf("Struct: { %d, %d }\n", p.x, p.y);
  return nil;
}

@end

@interface C : NSObject
@end
@interface D : C
@end

@implementation C
+(void) classMessage {
  printf("C classMessage\n");
}
-(void) instanceMessage {
  printf("C instanceMessage\n");
}
@end

@implementation D
+(void) classMessage {
  printf("D classMessage\n");
  [ super classMessage ];
}
-(void) instanceMessage {
  printf("D instanceMessage\n");
  [ super instanceMessage ];
}
@end

int main() {
  A *a = [[A alloc] init];
  foo(a);
  [a release];

  [ C classMessage ];
  [[[C alloc] init] instanceMessage];
  [ D classMessage ];
  [[[D alloc] init] instanceMessage];

  return 0;
}
