/* APPLE LOCAL file 4557598 */
/* Test that for -arch i386, new Objective-C ABI uses objc_msgSend_fpret_fixup() 
in the same places that the old ABI uses objc_msgSend_fpret().
*/
/* { dg-options "-mmacosx-version-min=10.5 -fobjc-abi-version=2" } */
/* { dg-do compile { target i?86-*-darwin* } } */
/* { dg-require-effective-target ilp32 } */

@interface Test @end
@implementation Test
+(double) method { return 0.0; }
@end
int main() {
    [Test method];
} /* { dg-final { scan-assembler "_objc_msgSend_fpret_fixup" } } */
