/* APPLE LOCAL file 5131847 */
/* { dg-do run { target i?86*-*-* } } */
/* { dg-options "" } */
extern void abort ();
union uu {
  struct {
    unsigned int index     : 4;
    unsigned int image     : 4;
    unsigned int unused    : 24;
    unsigned int unused2   : 32;
  } ss;
  unsigned long long token;
};

extern void myFunc (union uu *u, union uu *v, int n);
void myFunc (union uu *u, union uu *v, int n)
{
  int unit;

  for(unit = 0; unit < n; unit++)
  {
    unsigned int index;
    index = u[unit].ss.index;
    u[unit].token    = v->token;
    u[unit].ss.image = unit;
    u[unit].ss.index = index;
  }
}

#define n 4
int main(int argc, char *argv[])
{
  int i;
  union uu u[n] = {};
  union uu v;

  v.token = ~0ULL;
  myFunc (u, &v, n);

#define UPPER1BITS (0xffffffff00000000ULL)

  for(i = 0; i < n; i++)
  {
    if ((u[i].token & UPPER1BITS) != UPPER1BITS)
      abort ();
  }

  return 0;
}
