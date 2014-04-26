/* APPLE LOCAL file radar 4697411 */
/* Test if addition of 'volatile' to object causes bogus warning in presence of try-catch. */
/* { dg-do compile } */

struct CGRect {
    int size;
};

static void foo(const int * ciarg)
{ 
	struct CGRect r = {0};

        @try {
        } 
        @catch (id localException) {
        }
        foo(&r.size);

}
