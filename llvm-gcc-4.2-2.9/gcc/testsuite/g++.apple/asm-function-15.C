/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target powerpc*-*-* } } */
/* { dg-options "-fasm-blocks" } */

signed short foo(signed int a, signed short b)
{
	b = a * -1;

	asm {
	  bl      *+24
	  bl      *-32
	  b       .+8
	}
 	return b;
}

int main (int argc, char * const argv[])
{
        return foo(1,2);
}
