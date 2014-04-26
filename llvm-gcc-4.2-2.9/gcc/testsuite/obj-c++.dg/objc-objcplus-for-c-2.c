/* APPLE LOCAL file radar 3904247 */
/* Test for option -x objective-c++ which must invoke cc1objplus. Compile with no error. */
/* Note! name of the file *must* end with '.c'. */
/* { dg-options "-x objective-c++" } */
/* { dg-do compile } */

@interface TEST
@end

@implementation TEST @end

class Foo {
        Foo();
};
