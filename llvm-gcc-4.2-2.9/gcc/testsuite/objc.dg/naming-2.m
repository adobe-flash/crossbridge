/* Test for collision of @interfaces with global vars.  */
/* Author: Ziemowit Laski <zlaski@apple.com>.  */
/* { dg-do compile } */

@interface Foo
@end
/* APPLE LOCAL begin radar 4281748 */
float Foo;  /* { dg-error "parse error|syntax error|expected|redeclaration" } */
/* APPLE LOCAL end radar 4281748 */

double Bar;
@interface Bar
@end  /* { dg-error "redeclared as different kind of symbol" } */
/* APPLE LOCAL begin radar 4281748 */
/* { dg-error "previous declaration of" "" { target *-*-* } 11 } */
/* APPLE LOCAL end radar 4281748 */
