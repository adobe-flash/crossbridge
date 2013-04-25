/* Radar 6476836 */
/* { dg-do compile { target *-*-darwin* } } */
/* { dg-options "-Os -gdwarf-2 -dA -feliminate-unused-debug-types" } */
/* { dg-final { scan-assembler-not "LASF2" } } */
/* { dg-final { scan-assembler-not "LASF3" } } */

bool f();

void g(int p) {
  class C {
  public:
    static bool m(int i) {
      switch (i) {
      case 1:
      if (h())
        return true;
	if (f())
	  return m(0);
	  break;
      };

      return false;
    }
    static bool h() {
      if (f())
      return true;
      return false;
    }
  };
  if (C::m(p))
    asm("nop");
}
