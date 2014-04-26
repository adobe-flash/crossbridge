/* APPLE LOCAL file Apple version */
/* { dg-do compile } */

#if __APPLE_CC__ < 1000
#error build number too small
#endif
#if __APPLE_CC__ > 32768
#error build number too big
#endif
#if ! ( __APPLE_CC__ > 1000)
#error build number not really a number
#endif

int x = __APPLE_CC__;
