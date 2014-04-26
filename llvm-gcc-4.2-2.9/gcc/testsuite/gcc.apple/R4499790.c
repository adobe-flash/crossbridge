/* Test case for radar 4499790 */
/* { dg-do run } */
/* { dg-options "-fpascal-strings -fshort-wchar" } */

extern void abort (void);
typedef unsigned short UInt16;
typedef UInt16 UniChar;
int main(int argc, char* argv[])
{

        char st[] = "\pfoo";            // pascal string
        UniChar wt[] = L"\pbar";        // pascal Unicode string

        if (st[0] != 3)
	  abort ();
        if (wt[0] != 3)
	  abort ();
        
        return 0;
}


