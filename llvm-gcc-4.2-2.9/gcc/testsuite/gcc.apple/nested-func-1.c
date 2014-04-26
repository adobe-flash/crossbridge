/* APPLE LOCAL file nested functions 4258406 */
/* Nested functions are not supported by default on darwin.  */

/* { dg-compile } */
/* { dg-options "" } */

void foo(int i, int j)
{
	void bar (int k)
	  {	 /* { dg-error "nested functions" } */
		k = j;
	}
}
