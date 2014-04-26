/* APPLE LOCAL file radar 5607453 */
/* Test that test case does not crash when trying to issue a warning (radar 5096444) */
/* { dg-options "-mmacosx-version-min=10.5 -fobjc-gc" } */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-require-effective-target objc_gc } */
@protocol PROTO @end

@interface Foo 
@property id <PROTO> prop; /* { dg-warning "no 'assign', 'retain', or 'copy' attribute is specified" } */
@property Class <PROTO> prop1;
@end

