// APPLE LOCAL file test  68k alignment
// Radar 3313261: Positive C++ test case
// Origin: Matt Austern <austern@apple.com>
// { dg-do run }

#ifdef __LP64__
/* mac68k align not going to be supported for 64-bit, so skip entirely.  */
int main () { return 0; }
#else

#pragma options align=mac68k

typedef struct PMR {
  double  x;
  double  y;
}PMR;

#pragma options align=reset

static void GetDouble(double *doubleP)
{
  *doubleP = 1.;
}

static void GetPMR(PMR *p)
{
  GetDouble(&p->x);
  GetDouble(&p->y);
}

int main(void)
{
  PMR    tmp;
  bool ok = true;

  tmp.x = tmp.y = 0;
  ok = ok && tmp.x == 0.0 && tmp.y == 0.0;

  GetPMR(&tmp);
  ok = ok && tmp.x == 1.0 && tmp.y == 1.0;

  return !ok;
}

#endif /* __LP64__ */
