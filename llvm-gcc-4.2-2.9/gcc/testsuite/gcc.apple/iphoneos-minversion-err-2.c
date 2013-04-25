/* APPLE LOCAL file ARM 5683689 */

/* Verify that an error message is given for an invalid
   -miphoneos-version-min argument.  */

/* { dg-do compile { target arm*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-miphoneos-version-min=1" } */
/* { dg-skip-if "Not valid with -mmacosx-version-min" { *-*-darwin* } { "-mmacosx-version-min=*" } { "" } } */

int main(void)
{
  return 0;
}

/* { dg-error "Unknown value '1' of -miphoneos-version-min" "" {target *-*-* } 0 } */
