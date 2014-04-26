/* APPLE LOCAL file radar 4441551 */
/* Test that with new abi __OBJC2__ is defined. */
/* { dg-options "-mmacosx-version-min=10.5 -fobjc-abi-version=2" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do compile } */

#ifdef __OBJC2__
int i = 3;
#else
error __OBJC2__ 
#endif
