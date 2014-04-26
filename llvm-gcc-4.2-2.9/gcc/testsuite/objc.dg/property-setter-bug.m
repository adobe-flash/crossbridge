/* APPLE LOCAL file radar 5852190 */
/* { dg-options "-mmacosx-version-min=10.5 -framework Cocoa" } */
/* { dg-do run { target powerpc*-*-darwin* i?86*-*-darwin* } } */

#import <Cocoa/Cocoa.h>

@interface Foo : NSObject
{
@public
	BOOL _gratuitousBool;
	NSString* _assignedString;
	NSString* _retainedString;
	NSString* _copiedString;
}

@property BOOL gratuitousBool;
@property (assign) NSString* assignedString;
@property (retain) NSString* retainedString;
@property (copy) NSString* copiedString;

- (BOOL)gratuitousBool;
- (NSString*)assignedString;
- (NSString*)retainedString;
- (NSString*)copiedString;

-(void)setGratuitousBool:(BOOL)gratuitousBool;
-(void)setAssignedString:(NSString*)assignedString;
-(void)setRetainedString:(NSString*)retainedString;
-(void)setCopiedString:(NSString*)copiedString;

@end

//---------------------------------------------------------------------------

@implementation Foo

@synthesize gratuitousBool = _gratuitousBool;
@synthesize assignedString = _assignedString;
@synthesize retainedString = _retainedString;
@synthesize copiedString = _copiedString;

- (id)init
{
	self = [super init];
	if(self == nil) return nil;
	
	self.gratuitousBool = YES;
	self.assignedString = @"AssignedString";
	self.retainedString = @"RetainedString";
	self.copiedString = @"CopiedString";
	
	return self;
}

@end

//***************************************************************************

static void TestGratuitousBool(Foo* foo)
{
	NSLog(@"gratuitousBool initial value: %d", foo.gratuitousBool);
        if (foo.gratuitousBool != YES)
	  abort();
	foo.gratuitousBool = NO;
	NSLog(@"gratuitousBool new value: %d", foo.gratuitousBool);
	if (foo.gratuitousBool != NO)
	  abort();

	const BOOL oldGratuitousBool = foo.gratuitousBool;
	foo->_gratuitousBool = YES;
	NSCAssert(foo.gratuitousBool == YES, @"-gratuitousBool is borked");
	foo->_gratuitousBool = oldGratuitousBool;
}

int main(const int argc, const char* const argv[])
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	
	Foo* foo = [[Foo alloc] init];
	
	TestGratuitousBool(foo);
	
	[pool release];
		  
	return 0;
}
