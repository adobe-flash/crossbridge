/* APPLE LOCAL file radar 4805321 */
/* Test a variety of error reporting on mis-use of 'weak' attribute */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property -mmacosx-version-min=10.5 -fobjc-gc" } */
/* { dg-require-effective-target objc_gc } */

@interface INTF
{
  id IVAR;
}
@property (assign) __weak id pweak;
@end	

@implementation INTF
@synthesize pweak=IVAR; /* { dg-error "existing ivar 'IVAR' for the '__weak' property 'pweak' must be __weak" } */
@end
