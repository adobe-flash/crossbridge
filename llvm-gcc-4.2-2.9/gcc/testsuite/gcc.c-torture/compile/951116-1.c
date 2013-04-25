/* APPLE LOCAL testsuite nested functions */
/* { dg-options "-fnested-functions" } */
f ()
{
  long long i;
  int j;
  long long k = i = j;

  int inner () {return j + i;}
  return k;
}
