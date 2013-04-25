/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target powerpc*-*-* } } */
/* { dg-options "-fasm-blocks" } */

typedef struct astruct {
  int fieldy;
  int fieldx;
} atypedef;

void fun2 ()
{
  register int R3 asm ("r3");
  register int R4 asm ("r4");

  asm {
    stw R4, atypedef.fieldx(R3)
  }
}

int main()
{
	return 0;
}
