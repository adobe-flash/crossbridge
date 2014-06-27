#include <Foundation/Foundation.h>

struct S {
  int f0, f1;
};

@interface A : NSObject <NSCopying> {
  int x, _y, z, ro;
  id ob0, ob1, ob2, ob3, ob4;
}
@property int x;
@property int y;
@property int z;
@property(readonly) int ro;
@property(assign) id ob0;
@property(retain) id ob1;
@property(copy) id ob2;
@property(retain, nonatomic) id ob3;
@property(copy, nonatomic) id ob4;

-(id) copyWithZone: (NSZone*) zone;
@end

@implementation A
@synthesize x;
@synthesize y = _y;
@synthesize z;
@synthesize ro;
@synthesize ob0;
@synthesize ob1;
@synthesize ob2;
@synthesize ob3;
@synthesize ob4;
-(int) y {
  return x + 1;
}
-(void) setZ: (int) arg {
  x = arg - 1;
}

-(id) copyWithZone: (NSZone*) zone {
  [self retain];
  return self;
}
@end

@interface A (Cat)
@property int moo;
@end

@implementation A (Cat)
-(int) moo {
  return 10;
}
-(void) setMoo: (int) arg {
  printf("You set moo to %d. How quaint.\n", arg);  
}
@end

@interface I0 : NSObject {
@public
  _Complex float iv0;
}

@property(assign) _Complex float p0;

-(_Complex float) im0;
-(void) setIm0: (_Complex float) a0;
@end

@implementation I0 
@dynamic p0;

-(id) init {
  self->iv0 = 5.0 + 2.0i;
  return self;
}

-(_Complex float) im0 {
  printf("im0: %.2f + %.2fi\n", __real iv0, __imag iv0);
  return iv0 + (.1 + .2i);
}
-(void) setIm0: (_Complex float) a0 {
  printf("setIm0: %.2f + %.2fi\n", __real a0, __imag a0);
  iv0 = a0 + (.3 + .4i);
}

-(_Complex float) p0 {
  printf("p0: %.2f + %.2fi\n", __real iv0, __imag iv0);
  return iv0 + (.5 + .6i);
}
-(void) setP0: (_Complex float) a0 {
  printf("setP0: %.2f + %.2fi\n", __real a0, __imag a0);
  iv0 = a0 + (.7 + .8i);
}
@end

void f0(I0 *a0) {
    float l0 = __real a0.im0;
    float l1 = __imag a0->iv0;
    _Complex float l2 = (a0.im0 = a0.im0);
    _Complex float l3 = a0->iv0;
    _Complex float l4 = (a0->iv0 = a0->iv0);
    _Complex float l5 = a0->iv0;
    _Complex float l6 = (a0.p0 = a0.p0);
    _Complex float l7 = a0->iv0;
    _Complex float l8 = [a0 im0];
    printf("l0: %.2f + %.2fi\n", __real l0, __imag l0);
    printf("l1: %.2f + %.2fi\n", __real l1, __imag l1);
    printf("l2: %.2f + %.2fi\n", __real l2, __imag l2);
    printf("l3: %.2f + %.2fi\n", __real l3, __imag l3);
    printf("l4: %.2f + %.2fi\n", __real l4, __imag l4);
    printf("l5: %.2f + %.2fi\n", __real l5, __imag l5);
    printf("l6: %.2f + %.2fi\n", __real l6, __imag l6);
    printf("l7: %.2f + %.2fi\n", __real l7, __imag l7);
    printf("l8: %.2f + %.2fi\n", __real l8, __imag l8);
}

#define PRINT1(e0,t0) printf(#e0 ": %" #t0 "\n", e0)
int main() {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
  A *a = [[A alloc] init];

  PRINT1([a x], d);
  PRINT1([a y], d);
  PRINT1([a z], d);
  PRINT1([a ro], d);
  PRINT1([a moo], d);

  PRINT1(([a setX: 12], [a x]), d);
  PRINT1(([a setY: 23], [a y]), d);
  PRINT1(([a setZ: 4623], [a z]), d);
    PRINT1(([a setMoo: 22.3], [a moo]), d);

  PRINT1([a ob1], p);
  PRINT1(([a setOb1: a], [a ob1] == a), d);

  PRINT1([a ob2], p);
  PRINT1(([a setOb2: a], [a ob2] == a), d);

  PRINT1([a ob3], p);
  PRINT1(([a setOb3: a], [a ob3] == a), d);

  PRINT1([a ob4], p);
  PRINT1(([a setOb4: a], [a ob4] == a), d);

  [a release];

  f0([[I0 alloc] init]);

  return 0;
}
