/* APPLE LOCAL file radar 5435299 */
/* Test that synthesize ivar visibility is @private */
/* { dg-options "-mmacosx-version-min=10.5 -m64" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property" { target arm*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

#ifdef __OBJC2__
#import <objc/Object.h>

@interface Test7 : Object
@property int prop;
@end

@implementation Test7
@synthesize prop;
@end

@interface Test7Sub : Test7 @end

@implementation Test7Sub
-(int)method { return prop; } /* { dg-error "instance variable \\'prop\\' is declared private" } */
@end

#endif

