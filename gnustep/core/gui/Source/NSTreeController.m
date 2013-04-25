#import <AppKit/NSTreeController.h>

@implementation NSTreeController

- (id) initWithCoder: (NSCoder *)coder
{
  return self;
}

- (void) encodeWithCoder: (NSCoder *)coder
{
  // Do nothing...
}

- (id) copyWithZone: (NSZone *)zone
{
  return [self retain];
}

@end
