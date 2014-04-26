/* APPLE LOCAL file radar 5195402 */
/* Test for correct implementation of format_arg attribute on NSString * type of
   format strings. */
/* { dg-options "-Wformat -Wformat-security -Wformat-nonliteral -mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-Wformat -Wformat-security -Wformat-nonliteral" { target arm*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */
/* { dg-skip-if "" { arm*-*-darwin* } { "*" } { "" } } */


#include <Cocoa/Cocoa.h>

APPKIT_EXTERN NSInteger My_NSRunAlertPanel(NSString *title, NSString *msgFormat, NSString *defaultButton, NSString *alternateButton, NSString *otherButton, ...)
        __attribute__ ((format (__NSString__, 2, 6)));

extern NSString* _My_LocalizedStringForKey( NSString *key,  NSString *value, NSString *tableName )
                                            __attribute__ ((format_arg (1)));     

#define My_NSLocalizedString(key, comment) \
	    _My_LocalizedStringForKey(key,@"",nil)


static void Buggy_WarnAboutResponse( NSString *serverResponse )
{
    /* This next line is a security problem: it passes an untrusted string, received from the network, 
       as the format string of NSRunAlertPanel. At least six such misuses of NSRunAlertPanel were 
       identified and exploited in both Apple and third-party apps, during the Month Of Apple Bugs.
       Fortunately, the format attribute added to My_NSRunAlertPanel causes the compiler to emit 
       a warning for this unsafe call. */
    My_NSRunAlertPanel(@"Bad Server Response:", serverResponse, @"OK",nil,nil);     /* { dg-warning "format not a string literal and no format arguments" } */
}



static void WarnAboutResponse( NSString *serverResponse )
{
    /* This function uses NSRunAlertPanel correctly, with a safe format string in the second parameter, 
       and the untrusted string passed as an argument to be formatted. Note that the format string is localized, 
       however, meaning that it's passed through NSLocalizedString. But with -Wformat-nonliteral nevertheless
       we issue the warning. */
    My_NSRunAlertPanel(NSLocalizedString(@"Bad Server Response:", @"Title of error panel"),   
                       NSLocalizedString(@"The server responded: %@", @"Message of error panel"),    
                       @"OK",nil,nil,
                       serverResponse);     /* { dg-warning "format not a string literal, argument types not checked" } */
}


static void SimpleAlert()
{
    /* My_NSRunAlertPanel this time uses My_NSLocalizedString callfor its formatting argument.
       because of format_arg (1) attribute on My_NSLocalizedString, no warning to be issued.
       Even when -Wformat-nonliteral is specified. */
    My_NSRunAlertPanel(My_NSLocalizedString(@"Oops", @"Title of oops panel"), 
                       My_NSLocalizedString(@"Something went wrong. Sorry.", @"Message of oops panel"),
                       @"OK",nil,nil);     
}


int main()
{
    // Simulate an attack by pretending we got a string from the server that contains printf format specifiers:
    NSString *serverResponse = @"%s%@%n";
    
    // This call will work correctly.
    WarnAboutResponse(serverResponse);
    
    // This call will crash.
    Buggy_WarnAboutResponse(serverResponse);
}
