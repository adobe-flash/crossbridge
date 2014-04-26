/* APPLE LOCAL file radar 6164211 */
/* Check that when we build a reference to array of 'id' qualified protocols,
   we preserve protoloc-ness of the array elements so method prototype will
   be correct. Program must build and run without abort. */
/* { dg-options "-mmacosx-version-min=10.5 -framework Cocoa" } */
/* { dg-do run } */

#import <Cocoa/Cocoa.h>

static void compare (int val)
{
	if (val != 500)
	  abort();
}

@protocol myProto <NSObject>
- (int) width;
@end;

@interface myObject : NSObject < myProto>

@end

@implementation myObject

- (int) width
{
	return 500;
}

@end


@interface myController	: NSObject
{
	id<myProto> manyProtos[10];
	id<myProto> singleProto;
}

@end


@implementation myController

- init
{
	if ([super init] != nil)
	{
		int x;
		for (x=0;x<10; x++)
			manyProtos[x] = [[myObject alloc] init];
		
		singleProto = [[myObject alloc] init];
		
		return self;
	}
	else return nil;
}

-(void)dealloc
{
	int x;
	for (x=0; x<10; x++)
		[manyProtos[x] release];
	
	[super dealloc];
}

- bugOut
{
	compare([manyProtos[0] width]);
	compare([(id<myProto>)manyProtos[0] width]);
	
	compare([manyProtos[1] width]);
	compare([(id<myProto>)manyProtos[1] width]);
	
	compare([manyProtos[2] width]);
	compare([(id<myProto>)manyProtos[2] width]);
	
	compare([singleProto width]);
	compare([(id<myProto>)singleProto width]);
	
	
	return self;
}

@end



int main (int argc, const char * argv[]) {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

	myController *test;
	test = [[myController alloc] init];
	[test bugOut];
	[test release];
	
    [pool drain];
    return 0;
}
