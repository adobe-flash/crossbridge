#include <Foundation/NSObject.h>
#ifndef __arm__
#include <objc/Object.h>
#endif
#include <objc/runtime.h>
#include <stdio.h>

@interface X
-(unsigned char) conformsTo;
@end

@protocol P0;

@protocol P1
+(void) classMethodReq0;
-(void) methodReq0;
@optional
+(void) classMethodOpt1;
-(void) methodOpt1;
@required
+(void) classMethodReq2;
-(void) methodReq2;
@end

@protocol P2
//@property(readwrite) int x;
@end

@protocol P3<P1, P2>
-(id <P1>) print0;
-(void) print1;
@end

@interface A : NSObject <P0>
+(Class) getClass;
@end

@implementation A
+(Class) getClass { return self; }
@end

void foo(const id a) {
  void *p = @protocol(P3);
}

int main() {
  Protocol *P0 = @protocol(P0);
  Protocol *P1 = @protocol(P1);
  Protocol *P2 = @protocol(P2);
  Protocol *P3 = @protocol(P3);

#define Pbool(X) printf(#X ": %s\n", X ? "yes" : "no");
  Pbool([P0 conformsTo: P1]);
  Pbool([P1 conformsTo: P0]);
  Pbool([P1 conformsTo: P2]);
  Pbool([P2 conformsTo: P1]);
  Pbool([P3 conformsTo: P1]);
  Pbool([P1 conformsTo: P3]);
  Pbool([A getClass] != 0);
  Pbool(class_conformsToProtocol([A getClass], @protocol(P0)));
  Pbool(class_conformsToProtocol([A getClass], @protocol(P1)));

  return 0;
}
