/* APPLE LOCAL begin radar 4235138 */
/* { dg-do run { target powerpc*-*-* } } */
/* { dg-options "-fasm-blocks" } */
extern void abort();

static void FUNC(register unsigned int x) {
	union {
		double d;
		struct {
			int x;
			int y;
			struct {
			  	 int w;
				 int z;
			} q;
		} s;
	} u;
	asm {
		stw		x, u.s.x
		stw		x, u.s.y
		stw		x, u.s.q.w
		stw		x, u.s.q.z
		lfd		fp0, u.d
		mtfsf	1, fp0
	}

	if (u.s.y != x)
	  abort();

	if (u.s.x != x)
	  abort();

	if (u.s.q.w != x)
	  abort();

	if (u.s.q.z != x)
	  abort();
}

int main()
{
	FUNC (123);
	return 0;
}
/* APPLE LOCAL end radar 4235138 */
