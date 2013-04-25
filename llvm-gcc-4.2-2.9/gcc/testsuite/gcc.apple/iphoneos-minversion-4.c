/* APPLE LOCAL file ARM 5683689 */

/* Verify that the correct builtin definition is generated when
   -miphoneos-version-min is given.  */

/* { dg-do compile { target arm*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-miphoneos-version-min=98.76.54" } */
/* { dg-skip-if "Not valid with -mmacosx-version-min" { *-*-darwin* } { "-mmacosx-version-min=*" } { "" } } */

#if (__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ != 987654)
#error TEST FAILS
#endif

#ifdef __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
#error TEST FAILS
#endif

int main(void)
{
  return 0;
}
