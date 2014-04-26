/* Make sure we're emitting the __HAS_TLS__ symbol if we have tls.  */
/* { dg-require-effective-target tls } */

#ifdef __HAS_TLS__
extern __thread int a;
#else
#error "No TLS!"
#endif
