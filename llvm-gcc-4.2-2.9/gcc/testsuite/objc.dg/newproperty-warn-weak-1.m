/* APPLE LOCAL file radar 4899564 */
/* 'retain' or 'copy' are nonsensical in our system when used with __weak, 
    and should provoke an error. */
/* { dg-options "-fobjc-new-property -mmacosx-version-min=10.5 -fobjc-gc" } */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-require-effective-target objc_gc } */

@interface DooFus  {
   __weak id y;
   __weak id x;
   __weak id z;
}
@property (assign) __weak id y;
@property (copy) __weak id x;
@property (retain) __weak id z;

@end /* { dg-error "property \'z\' with \'retain\' attribute cannot be used with __weak storage specifier" } */
/* { dg-error "property \'x\' with \'copy\' attribute cannot be used with __weak storage specifier" "" { target *-*-* } 17 } */
