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
	lwzu    r1,(sizeof(tDummyStruct))(r2)           // works
	lwzu    r1,sizeof(tDummyStruct)(r2)
}

int main (int argc, char * const argv[]) 
{
	foo();
}
