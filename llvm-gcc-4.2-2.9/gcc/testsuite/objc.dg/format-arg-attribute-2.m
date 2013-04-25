/* APPLE LOCAL file radar 5195402 */
/* Test for correct implementation of format_arg attribute on NSString * type of
   format strings. */

/* { dg-options "-Wformat  -mmacosx-version-min=10.5" } */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
#include <Cocoa/Cocoa.h>


extern NSString *fa2 (const NSString *) __attribute__((format_arg(1)));
extern NSString *fa3 (NSString *) __attribute__((format_arg(1)));

extern void fc1 (const NSString *) __attribute__((format_arg)); /* { dg-error "wrong number of arguments" "bad format_arg" } */
extern void fc2 (const NSString *) __attribute__((format_arg())); /* { dg-error "wrong number of arguments" "bad format_arg" } */
extern void fc3 (const NSString *) __attribute__((format_arg(1, 2))); /* { dg-error "wrong number of arguments" "bad format_arg" } */

struct s1 { int i; } __attribute__((format_arg(1))); /* { dg-error "does not apply|only applies" "format_arg on struct" } */
union u1 { int i; } __attribute__((format_arg(1))); /* { dg-error "does not apply|only applies" "format_arg on union" } */
enum e1 { E1V0 } __attribute__((format_arg(1))); /* { dg-error "does not apply|only applies" "format_arg on enum" } */

extern NSString *ff3 (const NSString *) __attribute__((format_arg(3-2)));
extern NSString *ff4 (const NSString *) __attribute__((format_arg(foo))); /* { dg-error "invalid operand" "bad format_arg number" } */

/* format_arg formats must take and return a string.  */
extern NSString *fi0 (int) __attribute__((format_arg(1))); /* { dg-error "not a string" "format_arg int string" } */
extern NSString *fi1 (NSString *) __attribute__((format_arg(1))); 

extern NSString *fi2 (NSString *) __attribute__((format_arg(1))); 

extern int fi3 (const NSString *) __attribute__((format_arg(1))); /* { dg-error "not return string" "format_arg ret int string" } */
extern NSString *fi4 (const NSString *) __attribute__((format_arg(1))); 
extern NSString *fi5 (const NSString *) __attribute__((format_arg(1))); 
