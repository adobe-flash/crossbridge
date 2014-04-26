/* APPLE LOCAL file radar 5096648 */
#import <CoreFoundation/CFString.h>
/* Test for implementation of (__format__ (__CFString__, m, n)) attribute */
/* { dg-options "-Wformat -Wformat-security" } */
/* { dg-do compile { target *-*-darwin* } } */

#define SECURITY_ATTR   __attribute__ ((__format__ (__CFString__, 2, 6)))

typedef long CFInteger;

void CFGetAlertPanel(CFStringRef title, CFStringRef msgFormat, 
		   CFStringRef defaultButton, CFStringRef alternateButton, CFStringRef otherButton, ...) SECURITY_ATTR;

CFInteger CFRunAlertPanel(CFStringRef title, CFStringRef msgFormat, CFStringRef defaultButton, CFStringRef alternateButton, CFStringRef otherButton, ...) SECURITY_ATTR;

int d;
const char *string;
int main()
{
        CFStringRef  foo;
        CFGetAlertPanel (CFSTR ("foo%@"),foo, CFSTR ("foo%@"),CFSTR ("foo%@"),CFSTR ("foo%@"));    /* { dg-warning "format not a string literal and no format arguments" } */
        CFGetAlertPanel (CFSTR ("foo is %@"), foo, CFSTR ("foo%@"),CFSTR ("foo%@"),CFSTR ("foo%@"),CFSTR ("foo%@"),CFSTR ("foo%@"),CFSTR ("foo%@"));         // ok


	 CFRunAlertPanel(CFSTR ("title%@"), foo, CFSTR ("defaultButton%@"), CFSTR ("alternateButton%@"), 
				  CFSTR ("otherButton%@")); /* { dg-warning "format not a string literal and no format arguments" } */
	 CFRunAlertPanel(CFSTR ("title%@"), foo, CFSTR ("defaultButton%@"), CFSTR ("alternateButton%@"), 
				  CFSTR ("otherButton%@"), 100, 200);


}
