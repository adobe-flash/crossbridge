/* APPLE LOCAL file 4529765 */
/* Check for declaration and usage of 'format' attribute on method declarations. */
/* { dg-do compile } */
/* { dg-options "-Wformat" } */

@interface MyClass
- (void) message_with_line : (int)anchor : (const char *)format, ...
  __attribute__ ((__format__ (__printf__, 2, 3))) __attribute__ ((deprecated)) __attribute__ ((__sentinel__(1)));
+ (void) class_message : (int)anchor : (const char *)format, ... __attribute__ ((__format__ (__printf__, 2, 3)));
- (void) another_message : (int)anchor : (const char *)format, ... 
__attribute__ ((__format__ (__printf__, 0, 3))); /* { dg-warning "format string argument not a string type" } */
- (void) another_with_line : (int)anchor : (const char *)format, ... __attribute__ ((__format__ (__printf__, 2, 3)));
- (void) bar_with_line : (int)anchor : (const char *)format, ... 
__attribute__ ((__format__ (__printf__, 2, 2))); /* { dg-warning "format string argument follows the args to be formatted" } */
@end

void
FOO (MyClass* p)
{
       [MyClass  class_message :13:"invalid length %d for char '%c' in alternative %d of operand %d",
                                    10, 'a', 20, 10];
       [p message_with_line :13:"invalid length %d for char '%c' in alternative %d of operand %d",
                                    p, 'a', 20, 10]; /* { dg-warning "\\\'message_with_line::\\\' is deprecated" } */
       /* { dg-warning "format \\\'%d\\\' expects type \\\'int\\\', but argument 3" "" { target *-*-* } 23 } */
       /* { dg-warning "missing sentinel in function call" "" { target *-*-* } 23 } */
       [MyClass  class_message :13:"invalid length %d for char '%c' in alternative %d of operand %d",
                 10, 'a', p, 10]; /* { dg-warning "format \\\'%d\\\' expects type \\\'int\\\', but argument 5" } */
       [p another_with_line :13:"invalid length %d for char '%c' in alternative %d of operand %d",
                        p, 'a', 20, 10]; /* { dg-warning "format \\\'%d\\\' expects type \\\'int\\\', but argument 3" } */
       [MyClass  another_with_line :13:"invalid length %d for char '%c' in alternative %d of operand %d",
                                    10, 'a', 20, 10];
}
