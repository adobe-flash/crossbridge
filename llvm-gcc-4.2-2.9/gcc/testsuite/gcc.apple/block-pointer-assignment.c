/* APPLE LOCAL file radar 5878380 */
/* Issue diagnostics, instead of ICE, on invalid block pointer assignment. */
/* { dg-do compile } */
/* { dg-options "-mmacosx-version-min=10.5 -ObjC" { target *-*-darwin* } } */

static int sNSImageSnapshotInWindowsComputedValue = -2;

static void _NSImageSnapshotInWindows(void)
{
  static int nInvalid = ^{ return 1; };	/* { dg-error "incompatible types in initialization" } */
  sNSImageSnapshotInWindowsComputedValue = ^{ return 1; }; /* { dg-error "incompatible types in assignment" } */
}
