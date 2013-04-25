/* { dg-do compile } */
/* { dg-options "-O3 -gdwarf-2 -fno-common" } */
union {
  int i;
}     a1[10];
int v1;
int v2;
int *p1;

int f1(void);
void f2(void);
static int
f3(int arg1, int arg2, int arg3)
{
  int retval = v2;
  a1[v2++].i = 2;
  a1[v2++].i = arg1;
  a1[v2++].i = arg2;
  a1[v2++].i = arg3;
  if (v2 >= 50000)
    f2();
  return retval;
}

void
f4()
{
  do {
    v1 = f3(v1, p1[1], p1[0]);
    p1 += 2;
  } while (f1());
}
