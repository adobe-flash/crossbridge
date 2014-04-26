/* APPLE LOCAL file radar 5822637 */
/* Check that receiver of a getter call which is a messaging expression
   does not call the message more than once. */
/* { dg-options "-mmacosx-version-min=10.5 -framework Foundation" } */
/* { dg-do run } */

#include <Foundation/Foundation.h>

static int c;

@interface I : NSObject
{
	int count;
}
- (NSArray *)processResults;
- (int) Another;
@end

@implementation I

- (NSArray *)processResults 
{
    ++c;
    return nil;
}

- (int) Another
{
int totalResults = [self processResults].count;
return 0;
}
@end

int main()
{
	I* p = [[I alloc] init];
	[p Another];

	return c-1;
}

