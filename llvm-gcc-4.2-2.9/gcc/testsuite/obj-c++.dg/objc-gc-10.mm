/* APPLE LOCAL file radar 4600999 */
/* Test that addition of __strong attribute to actual argument does not
   result in a warning in objc or ICE in objc++ mode. */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fnext-runtime -fobjc-gc" } */
/* { dg-require-effective-target objc_gc } */

typedef  const struct _CFURLRequest*     CONSTRequestRef;

int Policy (CONSTRequestRef);

@interface NSURL 
{
    @public
    __strong  const struct _CFURLRequest*  request;
}
@end

int FOO(NSURL *_internal)
{
    return Policy(_internal->request);
}
