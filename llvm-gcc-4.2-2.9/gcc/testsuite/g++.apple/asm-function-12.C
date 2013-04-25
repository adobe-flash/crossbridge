/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target powerpc*-*-darwin* } } */
/* { dg-options "-fasm-blocks" } */

asm void foo()
{
	b       .+8
	bl      .+24
	b       .-8
	bl      .-32
	andi.   r1, r0, 3
}

int main (int argc, char * const argv[]) 
{
        foo();
}
