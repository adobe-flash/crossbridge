/* APPLE LOCAL file radar 5435299 */
/* Type mismatch of a property and its ivar. */
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property" { target arm*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

#import <objc/Object.h>

@interface Test4 : Object
{
   char ivar;
}
@property int prop;
@end
@implementation Test4
@synthesize prop = ivar; /* { dg-error "type of property \\'prop\\' does not match type of ivar \\'ivar\\'" } */
@end

