/* APPLE LOCAL file radar 4805365 */
/* { dg-do compile { target "powerpc*-*-darwin*" } } */
/* { dg-options "-Os -m64" } */
/* { dg-final { scan-assembler "lwz" } } */
/* { dg-final { scan-assembler "extsw" } } */
/* { dg-final { scan-assembler-not "lwa" } } */
#pragma pack(push, 2)
struct FixedRectangle {
  int left;
  int top;
  int right;
  int bottom;
};
#pragma pack(pop)

static struct FixedRectangle mybox = { 1,2,3,4 };
struct FixedRectangle* bar() { return &mybox; }
long left;

long getLeft ()
{
  return mybox.left;
}

void foo ()
{
  left = getLeft ();
}
