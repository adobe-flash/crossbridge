#import <Foundation/Foundation.h>

int main (int argc, const char * argv[]) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
  NSMutableArray *array =
    [NSMutableArray arrayWithObjects:@"One", @"Two", @"Three", nil];
    
  BOOL enumerated = NO;
  BOOL exceptionThrown = NO;
  @try {
    for (id obj in array) {
      enumerated = YES;
      [array removeLastObject];
    }
  } @catch (NSException *e) {
    exceptionThrown = YES;
  }

  assert(enumerated == YES && exceptionThrown == YES);
  [pool drain];
  return 0;
}
