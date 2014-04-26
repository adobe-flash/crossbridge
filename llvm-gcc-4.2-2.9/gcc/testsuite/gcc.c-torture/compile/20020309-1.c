/* APPLE LOCAL testsuite nested functions */
/* { dg-options "-fnested-functions" } */
int
sub1 (char *p, int i)
{
  char j = p[i];

  {
    void
    sub2 ()
      {
	i = 2;
	p = p + 2;
      }
  }
}
