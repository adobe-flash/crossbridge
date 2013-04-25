/* APPLE LOCAL file radar 6212507 */
/* This for incorrect specification of format number argument. */
/* { dg-options "-fconstant-cfstrings -Wformat -Wformat-security" } */
/* { dg-do compile { target *-*-darwin* } } */

#include <CoreFoundation/CoreFoundation.h>
void doSomething(CFStringRef format, CFStringRef bla) __attribute__((format(CFString, 0, 1))); /* { dg-error "argument number of CFString format cannot be less than one" } */

