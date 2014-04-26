/* APPLE LOCAL file radar 4985544 */
/* Test for implementation of (__format__ (__NSString__, m, n)) attribute */
/* { dg-options "-Wformat -Wformat-security" } */
/* APPLE LOCAL radar 5107971 */
/* { dg-options "-Wformat -Wformat-security -mmacosx-version-min=10.5" { target powerpc*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

typedef long NSInteger;

@class NSString;
@interface NSString @end

#define SECURITY_ATTR   __attribute__ ((__format__ (__NSString__, 2, 6)))

id NSGetAlertPanel(NSString *title, NSString *msgFormat, 
		   NSString *defaultButton, NSString *alternateButton, NSString *otherButton, ...) SECURITY_ATTR;

NSInteger NSRunAlertPanel(NSString *title, NSString *msgFormat, NSString *defaultButton, NSString *alternateButton, NSString *otherButton, ...) SECURITY_ATTR;

int d;
const char *string;
int main()
{
        NSString * foo;
        NSGetAlertPanel (@"foo%@",foo, @"foo%@",@"foo%@",@"foo%@");    /* { dg-warning "format not a string literal and no format arguments" } */
        NSGetAlertPanel (@"foo is %@", foo, @"foo%@",@"foo%@",@"foo%@",@"foo%@",@"foo%@",@"foo%@");         // ok


	 NSRunAlertPanel(@"title%@", foo, @"defaultButton%@", @"alternateButton%@", 
				  @"otherButton%@"); /* { dg-warning "format not a string literal and no format arguments" } */
	 NSRunAlertPanel(@"title%@", foo, @"defaultButton%@", @"alternateButton%@", 
				  @"otherButton%@", 100, 200);


}

