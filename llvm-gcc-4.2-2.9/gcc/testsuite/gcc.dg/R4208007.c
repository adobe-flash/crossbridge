/* APPLE LOCAL file Radar 4208007 */
/* { dg-compile } */
/* { dg-options "-O2 -ftree-loop-memset" } */

int A[16];
void foo()
{
	int i;
	for (i=0;i<16;i++)
		A[i] = 0;
}
