#include <Foundation/NSObject.h>
#include <stdio.h>

@interface A : NSObject
@end
@implementation A
+(void) foo {
  printf("__func__: %s\n", __func__);
  printf("__FUNCTION__: %s\n", __FUNCTION__);
  printf("__PRETTY_FUNCTION__: %s\n", __PRETTY_FUNCTION__);
}
@end

int main() {
  [A foo];
  return 0;
}
