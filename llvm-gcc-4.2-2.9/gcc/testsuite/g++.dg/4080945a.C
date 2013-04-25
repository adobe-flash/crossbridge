/* APPLE LOCAL file 4080945 / PR 20742 */
/* { dg-do compile } */
/* { dg-options "-O2" } */
/**********************************************************************/

extern int Loop1Max,Loop2Max;
void bar(int a, int b, int c);
void foo(int parm);

/**********************************************************************/

#define x7(x) x;x;x;x;x;x;x
#define REPL(x) x7(x);x7(x);x7(x);x7(x);x7(x);x7(x)	
	// 6 * 7 = 42 (0.06 seconds compile-time for GCC 3.3, 1 year for GCC 4.0)
	// Changing REPL from 42 to 21 will reduce GCC 4.0 compile-time to 20 seconds.

/**********************************************************************/
void foo(int parm)
{
	register int x,y;
	register int a,b,c;

	a = b = c = parm;

	for (y=0; y<Loop2Max; ++y)
	{
		for (x=0; x<Loop1Max; ++x)
		{
			REPL
			(
				a = c * a;
				b = a * b;
				c = parm - a;
			);
		}
	}

	bar(a,b,c);

	return;
}

/**********************************************************************/
