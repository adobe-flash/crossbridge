/* APPLE LOCAL file radar 4495910 */
/* { dg-do compile } */
/* { dg-options "-fast -Wuninitialized" } */
typedef __SIZE_TYPE__ size_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
uint32_t S[8];
void *memcpy (void *, const void *, size_t);
void bar (uint64_t *, unsigned char *);
static void foo (uint64_t *c, const unsigned char *d)
{
  uint32_t t = 0;
  uint64_t *sp2;
  int i,j;
  uint32_t state[4];

  sp2 = c;
  for (i = 0, j = 2; i < 4; i++)
  {
    for (j = 0; j < 4; j++)
    {
      t = state[j]^= S[t];
    }
    t = (t + i)&0xff;
  }
  memcpy (sp2, state, 4);
}

void bar (uint64_t *a, unsigned char *b)
{
   foo (a, b);
}
