/* APPLE LOCAL file mainline 2007-02-20 5005743 */
/* Basic test for -mmacosx-version-min switch on Darwin.  */
/* APPLE LOCAL 64-bit default */
/* { dg-options "-mmacosx-version-min=10.1 -mmacosx-version-min=10.3 -m32" } */
/* { dg-do run { target powerpc*-*-darwin* i?86*-*-darwin* } } */

int main(void)
{
#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ != 1030
  fail me;
#endif
  return 0;
}
