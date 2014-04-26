/* APPLE LOCAL file radar 4891561 */
/* { dg-do compile { target i?86-*-darwin* } } */
/* { dg-options "-O1 -std=gnu99 -fno-inline -fschedule-insns -mtune=pentium-m" } */
/* { dg-require-effective-target ilp32 } */

static inline unsigned char ceq (unsigned char a, unsigned char b)
{
  return a;
}

static inline unsigned char substr (const unsigned char * big,
                                    const char * small,
                                    unsigned char len,
                                    unsigned char slen)
{
  unsigned char i=0, i2=0;
  if (slen == 0)
    return 1;
redo:
  if (ceq (big[i],small[0]))
    goto more;
  return 0;

more:
  i2 = 1;
  for (i++; i < len; i++,i2++) {
    if (slen == i2)
      return 1;
    else
      goto redo;
  }
  return 0;
}

static const unsigned char inputs[94] =
  " \"$%&'()*+,-.!/0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

static void foo (unsigned char len, const char * search, unsigned char searchlen)
{
  unsigned char count[8];
  unsigned char *buf;
  if (substr (buf, search, 11, searchlen)) {
     unsigned char in[9],j;
     for (j=0; j < len; j++)
       in[j] = inputs[count[j]];
  }
}

int main ()
{
  int i, searchlen;
  const char *arg;
  foo (i, arg, searchlen);
  return 0;
}
