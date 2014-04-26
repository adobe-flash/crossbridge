/* APPLE LOCAL file radar 5142207 */
/* Test for visibility 'hidden' flag inserted in 32bit objc class's meta-data. */
/* { dg-options "-mmacosx-version-min=10.5" } */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-require-effective-target ilp32 } */

__attribute__((visibility("hidden"))) 
@interface Foo {
  @public
    id publicIvar;
  @protected
    id protectedIvar;
  @private
    id privateIvar;
}
@end
@implementation Foo @end
/* { dg-final { scan-assembler     "\\.long\t131074" } } */
/* { dg-final { scan-assembler     "\\.long\t131073" } } */
