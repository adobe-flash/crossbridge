/* APPLE LOCAL file radar 5096644 */
/* Test that use of new property of an object of a forward class declaration deoes not ice. */
/* { dg-options "-fnext-runtime -fobjc-new-property -fobjc-gc-only -mmacosx-version-min=10.5" } */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-require-effective-target objc_gc } */
@protocol NSCopying
@end

@class BLOCK;

@interface INTC
@property BLOCK* p;
@end
