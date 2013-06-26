#import <Foundation/Foundation.h>
#import <assert.h>

int main() {
  int state = 0;
  @try {
    state++;
    @try {
      state++;
      @throw [NSObject new];
      assert(0);
    } @catch(NSArray *e) {
      assert(0);
    }
    assert(0);
  } @catch (id e) {
    state++;
  }
  assert(state == 3);

  printf("OK\n");

  return 0;
}
