/* APPLE LOCAL file radar 5435299 */
/* The ivar's C scope extends from the @synthesize statement to the end of the file */
/* { dg-options "-mmacosx-version-min=10.5 -m64" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property" { target arm*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

#ifdef __OBJC2__
#import <objc/Object.h>

@interface Test9_1 : Object
@property int prop;
@end

@implementation Test9_1
-(int) method1 { return prop; }  /* { dg-error "\\'prop\\' undeclared" } */
				 /* { dg-error "\\(Each undeclared identifier is reported only once" "" { target *-*-* } 15 } */
				 /* { dg-error "for each function it appears in.\\)" "" { target *-*-* } 15 } */	
@synthesize prop;
-(int) method2 { return prop; }
@end

@implementation Test9_1 (Category)
-(int) method3 { return prop; }
@end

@interface Test9_2 : Object
@property int prop;
@end

@implementation Test9_2
-(int) method1 { return prop_ivar; }  /* { dg-error "\\'prop_ivar\\' undeclared" } */
-(int) method2 { return prop; }       /* { dg-error "\\'prop\\' undeclared" } */
@synthesize prop = prop_ivar;
-(int) method3 { return prop_ivar; }
-(int) method4 { return prop; }       /* { dg-error "\\'prop\\' undeclared" } */
@end

@implementation Test9_2 (Category)
-(int) method5 { return prop_ivar; }  
-(int) method6 { return prop; }       /* { dg-error "\\'prop\\' undeclared" } */
@end

#endif
