/* APPLE LOCAL file, Radar 6275985  */
/* { dg-do compile { target *-*-darwin* } } */
/* LLVM LOCAL - -fverbose-asm */
/* { dg-options "-O1 -gdwarf-2 -dA -fverbose-asm" } */
/* { dg-final { scan-assembler "__debug_inlined" } } */
/* { dg-final { scan-assembler "\[#;@]+\[ \t]+MIPS linkage name: \"_ZNK1UcvPKcEv\"" } } */
/* { dg-final { scan-assembler "\[#;@]+\[ \t]+Function name: \"operator const char" } } */
/* { dg-final { scan-assembler "\[#;@]+\[ \t]+MIPS linkage name: \"operator" } } */

struct S
{
  virtual ~S();
  S (const char *str);
  S& operator= (const char *str);
  operator const char *() const;
  S& operator+= (const char *str);
};
inline S operator+ (const char *s1, const S &s2)
{
  S x (s1);
  x += s2;
  return x;
}
struct U
{
  U ();
  U& operator= (const char *);
  const char *foo() const;
  operator const char *() const { return foo(); }
};
template <class T> struct V
{
  T v;
};
template <class T> struct W
{
  V<T> *w;
  W() : w (0) {}
  const T& operator* () const { return w->v; }
  T& operator* () { return w->v; }
};
struct X {
  X();
};
struct Y {
  Y (const U &u);
};
X::X()
{
  W<U> a;
  U b;
  b = (*a) + "xx";
  Y c (b);
}
