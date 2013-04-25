#import "ObjectTesting.h"
#import <Foundation/NSArray.h>
#import <Foundation/NSAutoreleasePool.h>
#import <AppKit/NSApplication.h>
#import <AppKit/NSCell.h>

int main()
{
  NSAutoreleasePool   *arp = [NSAutoreleasePool new];
  id testObject;

  [NSApplication sharedApplication];  
  testObject = [NSCell new];
  test_alloc(@"NSCell");
  test_NSObject(@"NSCell",[NSArray arrayWithObject:testObject]);
  test_NSCoding([NSArray arrayWithObject:testObject]);
  test_keyed_NSCoding([NSArray arrayWithObject:testObject]);
  test_NSCopying(@"NSCell",
                 @"NSCell",
		 [NSArray arrayWithObject:testObject], NO, NO);

  [arp release]; arp = nil;
  return 0;
}

@implementation NSCell (Testing)

- (BOOL) isEqual: (id)anObject
{
  if (self == anObject)
    return YES;
  if (![anObject isKindOfClass: [NSCell class]])
    return NO;
  if (![[anObject stringValue] isEqual: [self stringValue]])
    return NO;
  if (![[anObject title] isEqual: [self title]])
    return NO;
  if ([anObject type] != [self type])
    return NO;
  if ([anObject tag] != [self tag])
    return NO;
  return YES;
}

@end
