/* APPLE LOCAL file 4548636, 5556192 */
/* Check for a variety of rules for objc's class attributes. */
/* APPLE LOCAL radar 4899595 */
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do compile } */

#include <objc/objc.h>
/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

__attribute ((deprecated))  
@interface DEPRECATED : Object
  { @public int ivar; } 
  - (int) instancemethod;
@property  int prop; 
@end

@implementation DEPRECATED
@dynamic prop;
  -(int) instancemethod {  return ivar; } 
@end

__attribute ((deprecated)) void DEP();

@interface DEPRECATED (Category) 
@end /* { dg-warning "deprecated" } */

@interface NS : DEPRECATED 
@end /* { dg-warning "deprecated" } */

__attribute ((unavailable)) __attribute ((XXXX)) 
@interface UNAVAILABLE 
  - (int *) AnaotherInst;	/* { dg-warning "unknown" } */
  + (DEPRECATED*) return_deprecated; /* { dg-warning "deprecated" } */
  - (UNAVAILABLE *) return_unavailable;  /* { dg-error "unavailable" } */
@end

DEPRECATED * deprecated_obj; /* { dg-warning "deprecated" } */

UNAVAILABLE *unavailable_obj;		/* { dg-error "unavailable" } */

@implementation UNAVAILABLE
  - (int *) AnaotherInst { return (int*)0; }
  + (DEPRECATED *) return_deprecated { return deprecated_obj; } /* { dg-warning "deprecated" } */
  - (UNAVAILABLE *) return_unavailable { return unavailable_obj; }	/* { dg-error "unavailable" } */
@end

int foo (DEPRECATED *unavailable_obj) /* { dg-warning "deprecated" } */
{
    DEPRECATED *p = [DEPRECATED new];	/* { dg-warning "deprecated" } */ 

    int ppp = p.prop;		
    p.prop = 1;
    p.prop != 3;		
    DEP();	/* { dg-warning "deprecated" } */
    int q = p->ivar;
    return [p instancemethod]; 
   
}
