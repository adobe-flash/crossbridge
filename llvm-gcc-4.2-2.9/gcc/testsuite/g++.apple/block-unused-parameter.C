/* APPLE LOCAL file radar 5925784 */
/* Don't issue warning with -Wunused-parameter on '_self' parameter. */
/* { dg-do compile } */
/* { dg-options "-Wunused-parameter" } */

int main()
{
	__block int i = 1;
	^ { i = 1; return i; }; 
	return 0;
}

