#import <Foundation/NSObject.h>
#import <Foundation/NSKeyedArchiver.h>

@interface IBUserDefinedRuntimeAttributesPlaceholder : NSObject <NSCoding>
{
  NSString *typeIdentifier;
  NSString *keyPath;
  id value;
}

- (void) setTypeIdentifier: (NSString *)type;
- (NSString *) typeIdentifier;

- (void) setKeyPath: (NSString *)keyPath;
- (NSString *) keyPath;

- (void) setValue: (id)value;
- (id) value;

@end

@interface IBAccessibilityAttribute : NSObject <NSCoding>
@end

@interface IBNSLayoutConstraint : NSObject <NSCoding>
@end

@interface IBLayoutConstant : NSObject <NSCoding>
@end

@implementation IBUserDefinedRuntimeAttributesPlaceholder

- (void) encodeWithCoder: (NSCoder *)coder
{
  if([coder allowsKeyedCoding])
    {
      [coder encodeObject: typeIdentifier
		   forKey: @"typeIdentifier"];
      [coder encodeObject: keyPath
		   forKey: @"keyPath"];
      [coder encodeObject: value 
		   forKey: @"value"];
    }
}

- (id) initWithCoder: (NSCoder *)coder
{
  if([coder allowsKeyedCoding])
    {
      [self setTypeIdentifier: [coder decodeObjectForKey: @"typeIdentifier"]];
      [self setKeyPath: [coder decodeObjectForKey: @"keyPath"]];
      [self setValue: [coder decodeObjectForKey: @"value"]];
    }
  return self;
}

- (void) setTypeIdentifier: (NSString *)type
{
  ASSIGN(typeIdentifier, type);
}

- (NSString *) typeIdentifier
{
  return typeIdentifier;
}

- (void) setKeyPath: (NSString *)kpath
{
  ASSIGN(keyPath, kpath);
}

- (NSString *) keyPath
{
  return keyPath;
}

- (void) setValue: (id)val
{
  ASSIGN(value, val);
}

- (id) value
{
  return value;
}

@end

@implementation IBAccessibilityAttribute

- (void) encodeWithCoder: (NSCoder *)coder
{
}

- (id) initWithCoder: (NSCoder *)coder
{
  return self;
}

@end

@implementation IBNSLayoutConstraint
- (void) encodeWithCoder: (NSCoder *)coder
{
  // Do nothing...
}

- (id) initWithCoder: (NSCoder *)coder
{
  return self;
}
@end

@implementation IBLayoutConstant
- (void) encodeWithCoder: (NSCoder *)coder
{
  // Do nothing...
}

- (id) initWithCoder: (NSCoder *)coder
{
  return self;
}
@end
