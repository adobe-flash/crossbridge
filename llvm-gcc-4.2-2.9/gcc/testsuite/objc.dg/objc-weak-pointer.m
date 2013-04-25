/* APPLE LOCAL file radar 4945770 */
/* Do not ICE on the trivial program. */
/* { dg-options "-fobjc-gc" } */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-require-effective-target objc_gc } */

#include <objc/Object.h>

@interface NSObject { Class isa; } @end

@interface SCMObject : NSObject  {
    SCMObject *__weak _parent; 
}
@end
