/* Test case for radar 4499790 */
/* { dg-do run } */
/* { dg-options "-fpascal-strings" } */

typedef __WCHAR_TYPE__ wchar_t;
extern void abort (void);
int main(int argc, char* argv[])
{

        char st[] = "\pfoo";            // pascal string
        wchar_t wt[] = L"\pbar";        // pascal Unicode string

        if (st[0] != 3)
	  abort ();
        if (wt[0] != 3)
	  abort ();
        
        return 0;
}


