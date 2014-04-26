/* Test for handling of struct-returning methods
   for the Mac OS X ("NeXT") runtime (which uses specialized entry
   points).  */
/* Contributed by Ziemowit Laski <zlaski@apple.com>.  */
/* { dg-do compile { target *-*-darwin* } } */
/* APPLE LOCAL radar 4492976 */
/* { dg-require-effective-target ilp32 } */

/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

struct astruct {
  float a, b;
  /* APPLE LOCAL begin testing */
  char c;
} glob = { 1.0, 2.0, 'a' };
/* APPLE LOCAL end testing */

struct bstruct {
  float a, b, c, d, e, f;
} globb = { 1, 2, 3, 4, 5, 6 };

@interface foo : Object
- (struct astruct) stret;
- (struct bstruct) stretb;
@end

@implementation foo : Object
- (struct astruct) stret { return glob; }
- (struct bstruct) stretb { return globb; }
@end

@interface bar: foo
- (struct astruct) stret;
- (struct bstruct) stretb;
@end

@implementation bar
- (struct astruct) stret { return [super stret]; }
- (struct bstruct) stretb { return [super stretb]; }
@end

struct astruct afunc(foo *foo_obj) {
  return [foo_obj stret];
}

/* APPLE LOCAL begin ARM hybrid ABI */
/* { dg-final { scan-assembler "objc_msgSend_stret" } } */
/* { dg-final { scan-assembler "objc_msgSendSuper2_stret" { target arm*-*-darwin* } } } */
/* { dg-final { scan-assembler "objc_msgSendSuper_stret" { target { ! arm*-*-darwin* } } } } */

/* { dg-final { scan-assembler-not "objc_msgSend\[^_S\]" { target { ! arm*-*-darwin* } } } } */
/* { dg-final { scan-assembler-not "objc_msgSendSuper\[^_\]" { target { ! arm*-*-darwin* } } } } */
/* APPLE LOCAL end ARM hybrid ABI */

