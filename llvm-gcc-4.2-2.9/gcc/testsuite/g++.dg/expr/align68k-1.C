// APPLE LOCAL file test of -mdynamic-no-pic combined with 68k alignment
// Radar 3242139: Positive C++ test case
// Origin: Matt Austern <austern@apple.com>
// { dg-do run }
// { dg-options "-mdynamic-no-pic" }

#ifdef __LP64__
/* mac68k align not going to be supported for 64-bit, so skip entirely.  */
int main () { return 0; }
#else

const long val1 = 0xa0b0;
const long val2 = 0x1234;

#pragma options align=mac68k
struct X {
  long x1;
  long x2;
};

#pragma options align=reset

void setX(X* x) {
  x->x1 = val1;
  x->x2 = val2;
}

struct Y
{
  X field;
  void set_vals();
};

void Y::set_vals()
{
  ::setX(&field);
}

int main()
{
  Y y;
  bool ok = true;

  y.field.x1 = y.field.x2 = 0;
  ok = ok && y.field.x1 == 0 && y.field.x2 == 0;

  y.set_vals();
  ok = ok && y.field.x1 == val1 && y.field.x2 == val2;

  return !ok;
}

#endif /* __LP64__ */
