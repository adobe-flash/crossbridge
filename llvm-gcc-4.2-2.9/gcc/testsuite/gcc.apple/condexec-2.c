/* APPLE LOCAL file 6280380 */
/* The ARM backend was not recognizing that some operations cannot be
   predicated, and was blindly generating un-predicated operations,
   even though the branches were removed.  */
/* { dg-do run } */
/* { dg-options "-Oz -marm" { target arm*-*-darwin* } } */

extern void abort (void);

int x = 1;
float one = 1.0;

float foobar (float a, float b, float c, float d)
{
  if (x)
    return a + b;
  else
    return c - d;
}

int main (void)
{
  float result;

  result = foobar (one, one, one, one);
  if ((result) != 2.0f)
    abort ();
  return 0;
}

