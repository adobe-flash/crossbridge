/* APPLE LOCAL file radar 6209554 */
/* Better messages for bad property declarations. */
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property" { target arm*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

@interface I
{
	int _p;
	int _p3;
	int _p4;
}
@property int p;
@property int p1;
@property int p3;
@property int p4;
@end

@implementation I

@dynamic p3; /* { dg-error "previous property declaration of \\'p3\\' was here" } */
@dynamic p4;	/* { dg-error "previous property declaration of \\'p4\\' was here" } */
@synthesize p = _p; /* { dg-error "previous property declaration of \\'p\\' was here" } */


@synthesize p1 = _p;  /* { dg-error "synthesized properties \\'p1\\' and \\'p\\' both claim ivar \\'_p\\'" } */

@dynamic p3;  /* { dg-error "property \\'p3\\' is already implemented" } */

@synthesize p4=_p4;	/* { dg-error "property \\'p4\\' is already implemented" } */


@end


