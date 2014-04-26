/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target powerpc*-*-darwin* } } */
/* { dg-options "-fasm-blocks" } */

struct x  {
	int filler;
	float f[100];
	virtual void vf();
};

asm void func()
{
	lwz r4, x.f[0](r2)
	lwz r5, x.f[0]+4(r2)
}

int main (int argc, char * const argv[]) 
{
        func();
}
