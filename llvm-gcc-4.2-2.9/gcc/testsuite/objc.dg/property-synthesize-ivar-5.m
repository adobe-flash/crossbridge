/* APPLE LOCAL file radar 5435299 */
/* Test that one ivar cannot back multiple properties. */
/* { dg-options "-mmacosx-version-min=10.5 -m64" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property" { target arm*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

#import <objc/Object.h>

#ifdef __OBJC2__

@interface Test5_1 : Object
{
   int ivar;
}
@property int ivar;
@property int prop2;
@end
@implementation Test5_1
@synthesize ivar;	/* { dg-error "previous property declaration of \\'ivar\\' was here" } */
@synthesize prop2 = ivar; /* { dg-error "synthesized properties \\'prop2\\' and \\'ivar\\' both claim ivar \\'ivar\\'" } */
@end

@interface Test5_2 : Object
{
   int ivar;
}
@property int prop2;
@property int ivar;
@end
@implementation Test5_2
@synthesize prop2 = ivar;	/* { dg-error "previous property declaration of \\'prop2\\' was here" } */
@synthesize ivar; /* { dg-error "synthesized properties \\'ivar\\' and \\'prop2\\' both claim ivar \\'ivar\\'" } */
@end

@interface Test5_3 : Object
{
   int ivar;
}
@property int prop1;
@property int prop2;
@end
@implementation Test5_3
@synthesize prop1 = ivar; /* { dg-error "previous property declaration of \\'prop1\\' was here" } */
@synthesize prop2 = ivar; /* { dg-error "synthesized properties \\'prop2\\' and \\'prop1\\' both claim ivar \\'ivar\\'" } */
@end


@interface Test5_4 : Object
@property int prop1;
@property int prop2;
@end
@implementation Test5_4
@synthesize prop1; /* { dg-error "previous property declaration of \\'prop1\\' was here" } */
@synthesize prop2 = prop1; /* { dg-error "synthesized properties \\'prop2\\' and \\'prop1\\' both claim ivar \\'prop1\\'" } */
@end

@interface Test5_5 : Object
@property int prop1;
@property int prop2;
@end
@implementation Test5_5
@synthesize prop1 = prop2;  /* { dg-error "previous property declaration of \\'prop1\\' was here" } */
@synthesize prop2; /* { dg-error "synthesized properties \\'prop2\\' and \\'prop1\\' both claim ivar \\'prop2\\'" } */
@end

@interface Test5_6 : Object
@property int prop1;
@property int prop2;
@end
@implementation Test5_6
@synthesize prop1 = ivar; /* { dg-error "previous property declaration of \\'prop1\\' was here" } */
@synthesize prop2 = ivar; /* { dg-error "synthesized properties \\'prop2\\' and \\'prop1\\' both claim ivar \\'ivar\\'" } */
@end

#endif
