/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target powerpc*-*-darwin* } } */
/* { dg-options "-fasm-blocks" } */

struct tDummyStruct
{
	unsigned long fDummy;
	unsigned long fSample;
};

asm void foo()
{
	lwz		r1,-8+tDummyStruct.fSample(r2)
	stw		r1,-8+tDummyStruct.fSample(r2)
	lwz		r1,-8-(tDummyStruct.fSample-tDummyStruct.fDummy)(r2)
	stw		r1,-8-(tDummyStruct.fSample-tDummyStruct.fDummy)(r2)
}

int main (int argc, char * const argv[]) 
{
	foo();
}
