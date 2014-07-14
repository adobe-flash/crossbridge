#include <Foundation/Foundation.h>
#include <stdio.h>

@interface A : NSThread
-(void) main;
@end

#define kNumThreads 10
#define kLoopCount 10
volatile unsigned numThreads = kNumThreads;
unsigned sum = 0;

@implementation A
-(void) addToSum: (int) n {
  @synchronized([self class]) {
    int tmp = sum + n;
    [NSThread sleepForTimeInterval: .001];
    sum = tmp;
  }
}

-(void) main {
  unsigned i;
  for (i = 0; i < kLoopCount; ++i)
    [self addToSum: i];
  __sync_fetch_and_sub(&numThreads, 1);
}
@end

int main() {
  unsigned i;
  for (i = 0; i < kNumThreads; ++i) {
    [[[A alloc] init] start];
  }
  while (numThreads) ;
  printf("sum: %d\n", sum);
  assert(sum == 10 * (kLoopCount * (kLoopCount - 1)) / 2);
  return 0;
}
