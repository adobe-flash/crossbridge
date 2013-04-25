/* APPLE LOCAL begin radar 4218231 */
/* { dg-do run { target powerpc*-*-darwin* } } */
/* { dg-options "-fasm-blocks" } */

extern void abort();

double _get_fp_control(register float f1) {
    double fill = 4.5;

    struct {
	double d1;
	int i1;
    } s = {1.2, 0};

    union {
        double d;
        struct {
            int x;
            int y;
        } s;
	double result;
    } u;
    asm {
        mffs    fp0
        stfd    fp0, u.d
	stfd    fp1, u.d
    }
    return u.result + fill + s.d1;
}

int main()
{
	return 0;
}
/* APPLE LOCAL end radar 4218231 */
