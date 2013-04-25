/* APPLE LOCAL file radar 6068877 */
/* Test that utf16 characters in a cfstring literal is supported in c++, as well as
   objc, objc++. */
/* { dg-options "-Werror" } */
/* { dg-do compile { target *-*-darwin* } } */

#import <CoreFoundation/CFString.h>

int main()
{
	CFSTR("hé hé hé");
}
