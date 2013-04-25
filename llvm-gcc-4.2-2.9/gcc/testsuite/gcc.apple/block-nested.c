/* APPLE LOCAL file radar 5732232 - blocks */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-mmacosx-version-min=10.6 -fblocks" { target *-*-darwin*  } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#include <stdio.h>
void * _NSConcreteStackBlock[32];
#ifdef __cplusplus
extern "C" void exit(int);
#else
extern void exit(int);
#endif

enum numbers {
  zero, one, two, three, four
};

typedef enum numbers (^myblock)(enum numbers);


double test(myblock I) {
  return I(three);
}

int main() {
  __block enum numbers x = one;
  __block enum numbers y = two;

  myblock CL = ^(enum numbers z)
		{ enum numbers savex = x;
		  { __block enum numbers x = savex;
		  y = z;
		  if (y != three)
		    exit (6);
		  test (
			^ (enum numbers z) {
			    if (y != three) {
			      exit(1);
			    }
			    if (x != one)
			      exit(2);
			    x = z;
			    if (x != three)
			      exit(3);
			    if (y != three)
			      exit(4);
			    return (enum numbers) four;
			});}
		  return x;
		  };

  enum numbers res = (enum numbers)test(CL);

  if (res != one)
    exit (5);
  return 0;
}
