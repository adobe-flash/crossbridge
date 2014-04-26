/* APPLE FILE local radar 5612279 */ 
/* { dg-do run } */
extern void abort (void);
int main ()
{
  int result, i = 0;
  result = -2*abs(i-2);
  if (result != -4)
    abort ();
  return 0;
}
