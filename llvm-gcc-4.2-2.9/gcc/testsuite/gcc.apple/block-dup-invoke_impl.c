/* APPLE LOCAL file radar 5939894 */
/* Check that if a definition of struct invoke_impl already exist, then use it. */
/* { dg-options "-fblocks" } */
/* { dg-do compile } */

struct invoke_impl {
 void   *isa;
 int Flags;
 int Size;
 void *FuncPtr;
};

enum numbers
{
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
		{y = z; 
		 test (
		 ^ (enum numbers z) { 
		   x = z;
		   return (enum numbers) four;
		  }
		  );
		  return x;
		};

  enum numbers res = test(CL);

  return 0;
}
