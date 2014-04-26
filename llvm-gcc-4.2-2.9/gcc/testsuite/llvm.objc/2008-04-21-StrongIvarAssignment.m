/* { dg-do run { target *-*-darwin* } } */
/* { dg-options "-fobjc-gc -framework Foundation" } */
#import <Foundation/Foundation.h>
#import <Foundation/NSString.h>

uintptr_t OldPointerVal;
uintptr_t NewPointerVal;

@class Bork;

typedef struct {
  NSString *name;
} Info;

@interface Bork : NSObject {
@private
  CFMutableDictionaryRef _f1;
  __strong Info*         _f2;
}

- (void) foo:(NSString*)name;
@end

@implementation Bork
- (id)init {
  _f2 = malloc(sizeof(Info) * 3);

  unsigned i;
  for (i = 0; i < 3; ++i)
    _f2[i].name = 0;
}

- (void)foo:(NSString*)name {
  OldPointerVal = (uintptr_t)_f2;
  _f2[0].name = [name copy];
  NewPointerVal = (uintptr_t)_f2;
}
@end

int main() {
  Bork *NM = [Bork alloc];
  [NM init];
  NSString *Str = [NSString stringWithUTF8String: "Hello world"];
  [NM foo: Str];
  if (OldPointerVal != NewPointerVal)
    abort();
  [NM release];
}
