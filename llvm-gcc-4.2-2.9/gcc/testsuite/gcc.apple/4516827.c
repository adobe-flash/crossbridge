/* APPLE LOCAL file 4516827 PR 26643 */
/* { dg-options "-std=c99 -O1" } */
/* { dg-do run } */
#include <string.h>
extern void abort(void);

typedef union
{
	long rgb;
	struct
	{
		unsigned char unused;
		unsigned char ciBlue;
		unsigned char ciGreen;
		unsigned char ciRed;
	} cis;
} CVTest;

int validate (long) __attribute__((noinline));
int validate (long a) {
  if (a != 0)
    abort();
}

int placeholder (unsigned char, unsigned char, unsigned char) __attribute((noinline));
int placeholder (unsigned char a, unsigned char b, unsigned char c) {
  a; b; c; }

typedef unsigned char bool;
int main (int argc, char * const argv[]) {

	long index;	
	const long kNumCV = 3;
	CVTest theCV[kNumCV];
	
	memset(theCV, 0, sizeof(theCV));
	
	theCV[0].cis.ciRed = 10;
	theCV[0].cis.ciGreen = 10;
	theCV[0].cis.ciBlue = 10;
	
	theCV[1].cis.ciRed = 1;
	theCV[1].cis.ciGreen = 1;
	theCV[1].cis.ciBlue = 1;
	
	theCV[2].cis.ciRed = 0;
	theCV[2].cis.ciGreen = 0;
	theCV[2].cis.ciBlue = 0;
	
	CVTest *hpcvT = &theCV[0];
	for (index = 0; index < kNumCV; ++index)
	{
		bool a, b, c;
		placeholder(hpcvT->cis.ciRed, hpcvT->cis.ciGreen, hpcvT->cis.ciBlue);

		a = (hpcvT->cis.ciRed != hpcvT->cis.ciGreen);
		b = (hpcvT->cis.ciGreen != hpcvT->cis.ciBlue);
		c = ((hpcvT->cis.ciRed != hpcvT->cis.ciGreen) || (hpcvT->cis.ciGreen != hpcvT->cis.ciBlue));
		validate ((long)a);
		validate ((long)b);
		validate ((long)c);
		hpcvT++;
	}
	
    return 0;
}

