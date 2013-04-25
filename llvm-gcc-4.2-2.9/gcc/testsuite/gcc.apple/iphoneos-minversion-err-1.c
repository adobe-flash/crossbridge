/* APPLE LOCAL file ARM 5683689 */

/* Verify that -miphoneos-version-min is not allowed in conjunction with
   -mmacosx-version-min.  */

/* { dg-do compile { target *-*-darwin* } } */
/* { dg-options "-miphoneos-version-min=1.2 -mmacosx-version-min=10.4" } */

int main(void)
{
  return 0;
}

/* { dg-error "-mmacosx-version-min not allowed with -miphoneos-version-min" "" {target *-*-*} 1 } */
