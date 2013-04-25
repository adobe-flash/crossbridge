/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target powerpc*-*-* } } */
/* { dg-options "-fasm-blocks" } */
/* Test for use of '*' in a relative jump. This is to conform with CW compiler. */

asm void foo()
{
	b       *+8
	bl      *+24
	b       *-8
	bl      *-32
	andi. r1,r0,2
}

int main (int argc, char * const argv[]) 
{
        foo();
}
