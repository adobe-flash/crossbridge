/* APPLE LOCAL file radar 5180172 */
/* Test that cannot synthesize a category */
/* { dg-options "-fobjc-new-property -mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property" { target arm*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */
@protocol NSCopying;

@interface NSWindow 
{
	NSWindow* IVAR;
}
@property(copy) NSWindow <NSCopying>* title;
@end

@implementation NSWindow 
@dynamic title;
@end

@interface NSWindow (CAT)
@property(copy) NSWindow <NSCopying>* title;
@end

@implementation NSWindow (CAT)
@synthesize title=IVAR; /* { dg-error "synthesize not allowed in a category's implementation" } */
@end
