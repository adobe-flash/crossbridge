/* APPLE LOCAL file radar 4985544 */
/* Test for implementation of (__format__ (__NSString__, m, n)) attribute */
/* { dg-options "-Wformat -Wformat-security" } */
/* { dg-do compile { target *-*-darwin* } } */

@class NSString;
@interface NSString @end

#define SECURITY_ATTR	__attribute__ ((__format__ (__NSString__, 1, 2)))

extern void NSLog(NSString *format, ...) SECURITY_ATTR;

int d;
const char *string;
int main()
{
	NSString * foo;
        NSLog (foo);	/* { dg-warning "format not a string literal and no format arguments" } */	
	NSLog (foo, d);		// ok
	NSLog(@"foo is %@", @"foo is %@", foo);	// OK
	NSLog(@"foo is %@", @"foo is %@");	// OK
	NSLog(@"foo is %@", @"foo is %@", foo);	// OK
	NSLog(@"foo is %@");			// OK
}

