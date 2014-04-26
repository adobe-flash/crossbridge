/* APPLE LOCAL begin radar 4614623 */
/* { dg-do run { target i?86-*-* } } */
/* { dg-require-effective-target ilp32 } */
/* { dg-options "-O -mmmx -fomit-frame-pointer -march=pentium-m" } */
#include <signal.h>
extern void abort (void);

void sig_ill_handler (int sig)
{
  abort ();
}

int foo (const int m, const int n, const float *a1, const float *a,
         const float *b, const float *b1, float *c)
{
  float t1_i, t1_r;
  int i, iaki, ibij, ibkj, icij, j, jai, jbj, jcj, k, lda2 = 1, ldb2 = 2, ldc2 = 1;
  for (j = 0, jbj = 0, jcj = 0; j < n; j++, jbj += ldb2, jcj += ldc2)
    {
      for (i = m-1, jai = (m-1)*lda2, ibij = ((m-1) << 1)+jbj,
           icij = ((m-1) << 1 )+jcj; i >= 0;
           i--, jai -= lda2, ibij -= 2, icij -= 2 )
        {
          for (k = i+1, iaki = ((i+1) << 1 )+jai, ibkj = ((i+1) << 1)+jbj;
               k < m;
               k++, iaki += 2, ibkj += 2)
            {
              t1_r += (b[ibkj]) * (a[iaki]) - (b[ibkj+1]) * (-a[iaki+1]);
              t1_i += (b[ibkj]) * (-a[iaki+1]) + (b[ibkj+1]) * (a[iaki]);
            }
          c[icij] += (a1[0]) * (t1_r) - (a1[1]) * (t1_i);
          c[icij+1] += (a1[0]) * (t1_i) + (a1[1]) * (t1_r);
        }
    }
}

int main ()
{
  float a[4], b[4], c[6], d[4], e[5];
  int i;
  for (i=0; i<4; i++)
    a[i] = b[i] = c[i] = d[i] = e[i] = i;
  c[4] = 4;
  c[5] = 5;
  e[4] = 4;
  signal (SIGILL, sig_ill_handler);
  foo (2, 2, a, b, c, d, e);
  return 0;
}
/* APPLE LOCAL end radar 4614623 */
