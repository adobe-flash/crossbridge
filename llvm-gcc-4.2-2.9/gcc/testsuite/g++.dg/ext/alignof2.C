// PRs 16387 and 16389
// We were treating alignof (sa.a) as alignof (typeof (sa.a)), which is
// wrong for some fields.

// { dg-do run }
// APPLE LOCAL begin radar 5457234
// { dg-skip-if "" { "powerpc-apple-darwin*" } { "*" } { "" } }
// APPLE LOCAL end radar 5457234

extern "C" void abort();

struct A
{
  double a; 
} sa;

struct B
{
  char c;
  double b;
} sb;

int main()
{
  if (__alignof (sa) != __alignof (sa.a)
      || __alignof (sb) != __alignof (sb.b))
    abort();
}
