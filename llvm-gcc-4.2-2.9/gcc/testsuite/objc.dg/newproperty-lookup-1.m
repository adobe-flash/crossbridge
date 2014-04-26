/* APPLE LOCAL file radar 4805321 */
/* Test lookup of properties in super classes. */
/* Program should compile with no error or warning. */
/* { dg-do compile { target *-*-darwin* } } */
/* { dg-options "-fobjc-new-property -mmacosx-version-min=10.5 -fobjc-abi-version=2" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property" { target arm*-*-darwin* } } */

#import <Foundation/Foundation.h>

@interface test : NSObject
{
}
@property (assign) NSString *foo;  
@end

@interface test_sub : test
@end

int main (int argc, char **argv)
{
    test_sub *outer = nil;
    test *inner = outer;
    NSString *x;

    x = outer.foo;
    outer.foo = @"two";

    x = inner.foo;
    inner.foo = @"one";

    return 0;
}
