/* APPLE LOCAL file 4529766 */
/* Check for declaration and usage of 'format' with __NSString__
   selector attribute on method declarations. */
/* { dg-do compile } */
/* { dg-options "-Wformat -Wformat-security" } */

#include <Foundation/Foundation.h>

@interface MyClass
- (void) message_with_line : (int)anchor : (NSString *)format, ...
  __attribute__ ((__format__ (__NSString__, 2, 3)));
+ (void) class_message : (int)anchor : (NSString *)format, ... __attribute__ ((__format__ (__NSString__, 2, 3)));
- (void) another_message : (int)anchor : (NSString *)format, ... 
__attribute__ ((__format__ (__NSString__, 0, 3))); /* { dg-warning "format NSString argument not an 'NSString \\*' type" } */
- (void) another_with_line : (int)anchor : (NSString *)format: (int)a2, ... __attribute__ ((__format__ (__NSString__, 2, 4)));
- (void) bar_with_line : (int)anchor : (NSString *)format, ... 
__attribute__ ((__format__ (__NSString__, 2, 2))); /* { dg-warning "format string argument follows the args to be formatted" } */
@end

void
FOO (MyClass* p, NSString * form)
{
       [MyClass  class_message :13:form, 10, 'a', 20, 10];
       [p message_with_line :13:form]; /* { dg-warning "format not a string literal and no format arguments" } */
       [MyClass  class_message :13: @"foo%@", 10, 'a', p, 10]; 
       [MyClass  class_message :13: form]; /* { dg-warning "format not a string literal and no format arguments" } */
       [p another_with_line :13:form:10, p, 'a', 20, 10]; 
       [MyClass  another_with_line :13:@"foo%@":100, 10, 'a', 20, 10];
       [p  another_with_line :13:form:100]; /* { dg-warning "format not a string literal and no format arguments" } */
}
