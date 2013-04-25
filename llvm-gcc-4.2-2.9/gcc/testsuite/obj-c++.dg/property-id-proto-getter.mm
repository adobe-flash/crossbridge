/* APPLE LOCAL file radar 5893391 */
/* Test that property-style getter call syntax can be used on objects
   of 'id' type qualified by protocols which implement the getter
   method declared in that protocol. */
/* { dg-options "-mmacosx-version-min=10.5 -framework Foundation -fobjc-gc" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do run { target powerpc*-*-darwin* i?86*-*-darwin* } } */

#include <Foundation/Foundation.h>

int main()
{
        id spam = [NSArray arrayWithObjects:@"Spam", @"spam", nil];
        NSString *spamText= [ spam description ];
        id<NSObject> eggs = [NSArray arrayWithObjects:@"Spam", @"spam", nil];
        NSString *eggsText= eggs.description;
        NSObject *bacon = [NSArray arrayWithObjects:@"Spam", @"spam", nil];
        NSString *baconText= bacon.description;
        NSLog(@"spam=%@, eggs=%@, bacon=%@", spamText, eggsText, baconText);
	return 0;
}
