/* APPLE LOCAL file radar 4947014 */
/* Check for consistancy of 'atomic' vs 'nonatomic' attribute. */
/* { dg-options "-fobjc-new-property -mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property" { target arm*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

@interface BASE
@property (copy) id p1;
@property (nonatomic, copy) id p2;
@end

@interface DERIVED : BASE
@property (copy, nonatomic) id p1;
@property (copy) id p2;
@end /* { dg-warning "property 'p2' and its super class 'BASE' don't have matching 'atomic' attribute" } */
     /* { dg-warning "property 'p1' and its super class 'BASE' don't have matching 'atomic' attribute" "" { target *-*-* } 15 } */
