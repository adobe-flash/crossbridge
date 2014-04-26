/* APPLE LOCAL file radar 4829803 */
/* Check that this objective-c++ file does not cause gcc to loop with -fobjc-gc 
   and in presense of syntax error. */
/* { dg-options "-fobjc-gc" } */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-require-effective-target objc_gc } */

class TokenStream
{
  public:
   virtual const char *name() = 0;
};

extern "C" void *NSMapGet(const void *key);

void endHeader(TokenStream *stream)
{
        if (NSMapGet(stream->name) != __null) { 
        } 
        /* { dg-error "argument of type" "" { target *-*-* } 18 } */
}
