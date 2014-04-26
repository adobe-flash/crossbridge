/* APPLE LOCAL file ARM 5814138 */
/* Sequential if statements were being translated into conditionally-
   executed instructions with an incorrect condition.  */
/* { dg-do run } */
/* { dg-options "-O2" } */
int bug (int n1, int n2, int *d1, int *d2)
{
    int flag = 0, test, x, x1, x2;

    while (n1 != 0 && n2 != 0)
    {
	x1 = *d1, x2 = *d2;
	test = x1 - x2;
	if (test >= 0)
	    x = x2, d2++, n2--, flag ^= 2;
	if (test <= 0)
	    x = x1, d1++, n1--, flag ^= 1;
	if (flag == 3)
	    return 1;
    }

    return 0;
}

int arr1[5] = {1, 2, 3, 4, 5};
int arr2[5] = {0, -1, -2, -3, -4};
int arr3[5] = {7, 8, 9, 10, 11};

int main (void)
{
  return bug (5, 5, arr1, arr2) || bug (5, 5, arr1, arr3);
}

