/* APPLE LOCAL file radar 4805321, 4947014 */
/* Test that setter/getter helpers are generated for 'retain' property. */
/* { dg-options "-fobjc-new-property -mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property" { target arm*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

@interface NSPerson
{
  id ivar;
}
@property(retain) id firstName;
@end

@implementation NSPerson
@synthesize firstName=ivar;
@end
/* { dg-final { scan-assembler "objc_setProperty" } } */
/* { dg-final { scan-assembler "objc_getProperty" } } */
