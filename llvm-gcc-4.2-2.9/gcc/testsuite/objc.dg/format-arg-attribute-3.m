/* APPLE LOCAL file radar 5195402 */
/* Test for correct implementation of format_arg attribute on CFStringRef type of
   format strings. */
/* { dg-options "-Wformat -Wformat-security -Wformat-nonliteral -mmacosx-version-min=10.5" } */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */

#include <Cocoa/Cocoa.h>

APPKIT_EXTERN NSInteger My_NSRunAlertPanel(NSString *title, CFStringRef msgFormat, NSString *defaultButton, NSString *alternateButton, NSString *otherButton, ...)
        __attribute__ ((format (__CFString__, 2, 6)));

extern CFStringRef _My_LocalizedStringForKey( CFStringRef key,  NSString *value, NSString *tableName )
                                            __attribute__ ((format_arg (1)));     

#define My_NSLocalizedString(key, comment) \
	    _My_LocalizedStringForKey(key,@"",nil)


static void Buggy_WarnAboutResponse( CFStringRef serverResponse )
{
    /* This next line is a security problem: it passes an untrusted string, received from the network, 
       as the format string of NSRunAlertPanel. At least six such misuses of NSRunAlertPanel were 
       identified and exploited in both Apple and third-party apps, during the Month Of Apple Bugs.
       Fortunately, the format attribute added to My_NSRunAlertPanel causes the compiler to emit 
       a warning for this unsafe call. */
    My_NSRunAlertPanel(@"Bad Server Response:", serverResponse, @"OK",nil,nil);     /* { dg-warning "format not a string literal and no format arguments" } */
}

static void SimpleAlert()
{
    /* My_NSRunAlertPanel this time uses My_NSLocalizedString callfor its formatting argument.
       because of format_arg (1) attribute on My_NSLocalizedString, no warning to be issued.
       Even when -Wformat-nonliteral is specified. */
    My_NSRunAlertPanel(@"Title of oops panel", 
                       My_NSLocalizedString(CFSTR("Something went wrong. Sorry."), @"Message of oops panel"),
                       @"OK",nil,nil);     
}
