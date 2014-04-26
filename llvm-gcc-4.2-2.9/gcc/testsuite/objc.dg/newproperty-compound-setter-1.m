/* APPLE LOCAL file radar 5140757 */
/* Check that compound assignment to properties when rhs is a gimplifiable 
   expression works.  Program should build with no ICE or error. */
/* { dg-options "-fobjc-new-property -mmacosx-version-min=10.5 -framework Foundation" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property -framework Foundation" { target arm*-*-darwin* } } */
/* { dg-do run } */

#include <Foundation/Foundation.h>
#include <stdio.h>

@interface NSString (FileSync_Additions)
@property(readonly)             NSString*                       _name;
@property(readonly)             NSString*                       _path;
@property(readonly)             BOOL                            isADName;
@property(readonly)             BOOL                            isADPath;

+ (NSString*)                           stringWithFileSystemRepresentation:(const char*)str;
+ (NSString*)                           stringWithFileSystemRepresentation:(const char*)str length:(NSUInteger)len;

- (BOOL)                                        isEqualToPath:(NSString*)path;
- (NSMutableString*)            stringByPrefixingLinesWithString:(NSString*)prefix;
- (const UniChar*)                      unicode;
@end

// --------------------------------------------------------------------------------------

@interface NSMutableString (FileSync_Additions)
- (NSMutableString*)            prefixLinesWithString:(NSString*)prefix;
@end

// --------------------------------------------------------------------------------------


@interface TestObj : NSObject
{
	TestObj*	_i1;
	TestObj*	_i2;
	TestObj*	_i3;
}
@property(readonly,retain)	TestObj*	i1;
@property(readonly,retain)	TestObj*	i2;
@property(readwrite,retain)	TestObj*	i3;
@end

@interface TestObj ()
@property(readwrite,retain)	TestObj*	i1;
@property(readwrite,retain)	TestObj*	i2;
@end


@implementation TestObj
@synthesize i1 = _i1;
@synthesize i2 = _i2;
@synthesize i3 = _i3;

- (id) init
{
	self = [super init];
	self.i1 = self.i2 = self.i3 = self;
	return self;
} 

@end


const char*	gProcessName;

const char* _last_path_comp(const char* fullPath)
{
	char*	result = strrchr(fullPath, '/');
	if (result != nil) return ++result;
	return fullPath;
} // _last_path_comp

// --------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	gProcessName = _last_path_comp(argv[0]);
	NSAutoreleasePool*	pool  = [NSAutoreleasePool new];

	TestObj*	obj = [[TestObj new] autorelease];

	[pool drain];
	return 0;
} // main
