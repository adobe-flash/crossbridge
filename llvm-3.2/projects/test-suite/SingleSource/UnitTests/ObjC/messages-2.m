#include <Foundation/NSObject.h>
#include <Foundation/NSGeometry.h>
#include <stdio.h>

typedef struct {
  int x, y, z[10];
} MyPoint;

@interface A : NSObject
+(void) printThisInt: (int) arg0 andThatFloat: (float) arg1 andADouble: (double) arg2 andAPoint: (MyPoint) arg3;
+(float) returnAFloat;
+(double) returnADouble;
+(MyPoint) returnAPoint;
+(void) printThisSize: (NSSize) arg0;
+(NSSize) returnASize;

-(void) printThisInt: (int) arg0 andThatFloat: (float) arg1 andADouble: (double) arg2 andAPoint: (MyPoint) arg3;
-(float) returnAFloat;
-(double) returnADouble;
-(MyPoint) returnAPoint;
-(void) printThisSize: (NSSize) arg0;
-(NSSize) returnASize;
@end
@interface B : A
@end

@implementation A
+(void) printThisInt: (int) arg0 andThatFloat: (float) arg1 andADouble: (double) arg2 andAPoint: (MyPoint) arg3 {
  printf("(CLASS) theInt: %d, theFloat: %f, theDouble: %f, thePoint: { %d, %d }\n",
         arg0, arg1, arg2, arg3.x, arg3.y);
}
+(float) returnAFloat {
  return 15.;
}
+(double) returnADouble {
  return 25.;
}
+(MyPoint) returnAPoint {
  MyPoint x = { 35, 45 };
  return x;
}
+(void) printThisSize: (NSSize) arg0 {
  printf("(CLASS) theSize: { %f, %f }\n",
         arg0.width, arg0.height);
}
+(NSSize) returnASize {
  NSSize x = { 32, 44 };
  return x;
}

-(void) printThisInt: (int) arg0 andThatFloat: (float) arg1 andADouble: (double) arg2 andAPoint: (MyPoint) arg3 {
  printf("theInt: %d, theFloat: %f, theDouble: %f, thePoint: { %d, %d }\n",
         arg0, arg1, arg2, arg3.x, arg3.y);
}
-(float) returnAFloat {
  return 10.;
}
-(double) returnADouble {
  return 20.;
}
-(MyPoint) returnAPoint {
  MyPoint x = { 30, 40 };
  return x;
}
-(void) printThisSize: (NSSize) arg0 {
  printf("theSize: { %f, %f }\n",
         arg0.width, arg0.height);
}
-(NSSize) returnASize {
  NSSize x = { 22, 34 };
  return x;
}
@end

@implementation B
+(void) printThisInt: (int) arg0 andThatFloat: (float) arg1 andADouble: (double) arg2 andAPoint: (MyPoint) arg3 {
  arg3.x *= 2;
  arg3.y *= 2;
  [ super printThisInt: arg0*2 andThatFloat: arg1*2 andADouble: arg2*2 andAPoint: arg3 ];
}
+(void) printThisSize: (NSSize) arg0 {
  arg0.width *= 2;
  arg0.height *= 2;
  [ super printThisSize: arg0 ];
}
+(float) returnAFloat {  
  return [ super returnAFloat ]*2;// FIXME: + super.returnAFloat;
}
+(double) returnADouble {
  return [ super returnADouble ]*2;
}
+(MyPoint) returnAPoint {
  MyPoint x = [ super returnAPoint ];
  x.x *= 2;
  x.y *= 2;
  return x;
}
+(NSSize) returnASize {
  NSSize x = [ super returnASize ];
  x.width *= 2;
  x.height *= 2;
  return x;
}

-(void) printThisInt: (int) arg0 andThatFloat: (float) arg1 andADouble: (double) arg2 andAPoint: (MyPoint) arg3 {
  arg3.x *= 2;
  arg3.y *= 2;
  [ super printThisInt: arg0*2 andThatFloat: arg1*2 andADouble: arg2*2 andAPoint: arg3 ];
}
-(void) printThisSize: (NSSize) arg0 {
  arg0.width *= 2;
  arg0.height *= 2;
  [ super printThisSize: arg0 ];
}
-(float) returnAFloat {
  return [ super returnAFloat ]*2;
}
-(double) returnADouble {
  return [ super returnADouble ]*2;
}
-(MyPoint) returnAPoint {
  MyPoint x = [ super returnAPoint ];
  x.x *= 2;
  x.y *= 2;
  return x;
}
-(NSSize) returnASize {
  NSSize x = [ super returnASize ];
  x.width *= 2;
  x.height *= 2;
  return x;
}
@end

void test(A *a) {
  MyPoint pt = { 33, 52 };
  NSSize size = { 44, 55 };

  [ a printThisInt: 1 andThatFloat: 2.0 andADouble: 3.0 andAPoint: pt ];
  [ a printThisSize: size ] ;

  printf("A returned float: %f\n", [ a returnAFloat ]);

  printf("A returned double: %f\n", [ a returnADouble ]);

  MyPoint x = [ a returnAPoint ];
  printf("A returned struct: { %d, %d }\n", x.x, x.y);  

  printf("A returned struct (%d)\n", [ a returnAPoint ].x);  

  NSSize y = [ a returnASize ];
  printf("A returned size: { %f, %f }\n", y.width, y.height);  
}

void test2() {
  MyPoint pt = { 33, 52 };
  NSSize size = { 44, 55 };
  
  [ A printThisInt: 1 andThatFloat: 2.0 andADouble: 3.0 andAPoint: pt ];
  [ A printThisSize: size ] ;
  
  printf("A returned float: %f\n", [ A returnAFloat ]);

  printf("A returned double: %f\n", [ A returnADouble ]);

  MyPoint x = [ A returnAPoint ];
  printf("A returned struct: { %d, %d }\n", x.x, x.y);  

  printf("A returned struct (%d)\n", [ A returnAPoint].x);  

  NSSize y = [ A returnASize ];
  printf("A returned size: { %f, %f }\n", y.width, y.height);  
}

void test3() {
  MyPoint pt = { 33, 52 };
  NSSize size = { 44, 55 };
  
  [ B printThisInt: 1 andThatFloat: 2.0 andADouble: 3.0 andAPoint: pt ];
  [ B printThisSize: size ] ;
  
  printf("B returned float: %f\n", [ B returnAFloat ]);

  printf("B returned double: %f\n", [ B returnADouble ]);

  MyPoint x = [ B returnAPoint ];
  printf("B returned struct: { %d, %d }\n", x.x, x.y);  

  printf("B returned struct (%d)\n", [ B returnAPoint ].x);  

  NSSize y = [ B returnASize ];
  printf("B returned size: { %f, %f }\n", y.width, y.height);  
}

int main() {
  A *a = [[A alloc] init];
  test(a);
  A *b = [[B alloc] init];
  test(b);

  test2();
  test3();

  return 0;
}
