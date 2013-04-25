/* APPLE LOCAL file 6255801 */
/* { dg-do compile { target *-*-darwin* } } */
@interface Foo @end
@implementation Foo @end
/* Any alignment above ".align 3" (=64-bit) is too much.  */
/* { dg-final { scan-assembler-not "\.align\[\t \]+\[4-9\]" } } */
