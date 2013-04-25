/* APPLE LOCAL file radar 5724385 */
/* Test that alignment of a bitfield is alignment of its underlying type. */
/* { dg-options "-std=c99 -mmacosx-version-min=10.5 -m64" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-std=c99" { target arm*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

@interface Foo {
@private
    int first;
    int :1;
    int third :1;
    int :1;
    int fifth :1;
}
@end
@implementation Foo 
@end
/* { dg-final { scan-assembler "\t.quad\t_OBJC_IVAR_\\\$_Foo.third\n\t.quad\tL_OBJC_METH_VAR_NAME_1\n\t.quad\tL_OBJC_METH_VAR_TYPE_1\n\t.long\t2" } } */
