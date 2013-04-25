/* APPLE LOCAL begin radar 4291099 */
/* Test for generation of write-barriers for data members initialized in ctor's
   initializer list. */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fnext-runtime -fobjc-gc -Wassign-intercept" } */
/* { dg-require-effective-target objc_gc } */

/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

@interface Bar : Object
@end

Bar *NP = 0;

struct MyClass {
        MyClass () : path(0), 
		     new_path(NP) {}	/* { dg-warning "strong-cast assignment has been intercepted" 2 } */
Bar *path;
Bar *new_path;
};

int main()
{
	MyClass mc;
	return 0;
}
/* { dg-final { scan-assembler "objc_assign_strongCast" } } */
/* APPLE LOCAL end radar 4291099 */
