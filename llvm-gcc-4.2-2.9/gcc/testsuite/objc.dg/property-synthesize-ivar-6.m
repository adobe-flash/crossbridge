/* APPLE LOCAL file radar 5435299 */
/* A synthesize property cannot use an ivar in its super class. */
/* { dg-options "-mmacosx-version-min=10.5 -m64" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property" { target arm*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

#ifdef __OBJC2__
#import <objc/Object.h>

@interface Test6Super : Object
{
   int prop;
}
@end

@implementation Test6Super @end

@interface Test6 : Test6Super
@property int prop;
@end

@implementation Test6
@synthesize prop;  /* { dg-error "property \\'prop\\' attempting to use ivar \\'prop\\' in super class \\'Test6\\'" } */
@end

#endif

