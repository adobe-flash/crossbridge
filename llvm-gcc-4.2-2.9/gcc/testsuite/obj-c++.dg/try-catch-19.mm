/* APPLE LOCAL file radar 4697411 */
/* Test if addition of 'volatile' to object causes bogus error in presence of try-catch. */
/* { dg-do compile } */

struct CGRect {
    int size;
};

static void foo(const int&)
{ 
	CGRect r = {0};

        @try {
        } 
        @catch (id localException) {
        }
        foo(r.size);

}
