/* Verify that switch tables aren't fundamentally broken with
   -mlongcall.  */
/* { dg-options "-mlongcall" } */
/* { dg-do run } */

int val = 3;

int main (void)
{
  switch (val)
    {
      case 0:
	return 1;
      case 1:
	return 2;
      case 2:
	return 4;
      case 3:
	return 0;
      case 4:
	return 8;
      case 5:
	return 16;
    }
  return 32;
}
 
