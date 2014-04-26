/* APPLE LOCAL file radar 4582204 - radar 5575115 */
/* Test that message_ref_t meta-data is generated for for objc and obj-c++ */
/* { dg-options "-fobjc-abi-version=2 -mmacosx-version-min=10.6" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fno-objc-legacy-dispatch" { target arm*-*-darwin* } } */
/* { dg-do compile } */

@interface Foo 
+class; 
@end
int main() {
    [Foo class];
}
/* { dg-final { scan-assembler "l_objc_msgSend_fixup.*:" } } */
