/* APPLE LOCAL file radar 5128402 */
/* Check that elem collection can be used as a collection in the foreach-statement. */
/* { dg-options "-mmacosx-version-min=10.5 -framework Foundation" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-framework Foundation" { target arm*-*-darwin* } } */
/* { dg-do run } */

#import <Foundation/Foundation.h>

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

const char*	gProcessName;

// --------------------------------------------------------------------------------------

static void usage(int exitcode)
{
	printf("Usage: %s\n", gProcessName);
	exit(exitcode);
} // usage

// --------------------------------------------------------------------------------------

const char* _last_path_comp(const char* fullPath)
{
	char*	result = strrchr(fullPath, '/');
	if (result != nil) return ++result;
	return fullPath;
} // _last_path_comp

// --------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	int count;
	id elem;
	gProcessName = _last_path_comp(argv[0]);
	NSAutoreleasePool*	pool  = [NSAutoreleasePool new];

	NSArray*	a1   = [NSArray array];
	NSArray*	a2   = [NSArray array];
	NSArray*	col  = [NSArray arrayWithObjects:a1, a2, nil];
	NSArray*	save = col;

	NSEnumerator*	en = [col objectEnumerator];
        count = 0;
	while ((col = [en nextObject]) != nil) ++count;
	if (count != 2)
	  abort ();

	count = 0;
	col = save;
	for (col in col) ++count;
	if (count != 2)
	  abort ();
        elem = @"hi"; 
        for (elem in [NSArray array]) { }
        if (elem != nil)
	  abort ();

        elem = @"hi"; 
        for (elem in (id)nil) { }
        if (elem != nil)
	  abort ();

	[pool drain];
	return 0;
} // main
