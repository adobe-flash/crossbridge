/* APPLE LOCAL file radar 5435299 */
/* Multiple @synthesize of a single property is error. */
/* { dg-options "-mmacosx-version-min=10.5 " { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property" { target arm*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

#import <objc/Object.h>

@interface Test3 : Object
{
   int prop;
}
@property int prop;
@end
@implementation Test3
@synthesize prop;	/* { dg-error "previous property declaration of \\'prop\\' was here" } */
@synthesize prop;  /* { dg-error "synthesized properties \\'prop\\' and \\'prop\\' both claim ivar \\'prop\\'" } */
		   /* { dg-error "property \\'prop\\' is already implemented" "" { target *-*-* } 17 } */
@end
