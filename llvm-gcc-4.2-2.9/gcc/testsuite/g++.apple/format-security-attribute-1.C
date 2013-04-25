/* APPLE LOCAL file radar 5096648 */
#import <CoreFoundation/CFString.h>
/* Test for implementation of (__format__ (__CFString__, m, n)) attribute */
/* { dg-options "-Wformat -Wformat-security" } */
/* { dg-do compile { target *-*-darwin* } } */

#define SECURITY_ATTR	__attribute__ ((__format__ (__CFString__, 1, 2)))

extern void CFLog(CFStringRef format, ...) SECURITY_ATTR;

int d;
const char *string;
int main()
{
	CFStringRef foo;
        CFLog (foo);	/* { dg-warning "format not a string literal and no format arguments" } */	
	CFLog (foo, d);		// ok
	CFLog(CFSTR("foo is %@"), CFSTR("foo is %@"), foo);	// OK
	CFLog(CFSTR("foo is %@"), CFSTR("foo is %@"));	// OK
	CFLog(CFSTR("foo is %@"), CFSTR("foo is %@"), foo);	// OK
	CFLog(CFSTR("foo is %@"));			// OK
}
