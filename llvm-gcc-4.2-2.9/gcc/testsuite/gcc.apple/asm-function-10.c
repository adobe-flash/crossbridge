/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target powerpc*-*-* } } */
/* { dg-options "-fasm-blocks" } */

asm void foo() 
{
  lis	   r1,(kUndefC<<10)|(3<<5)|(7)	/* { dg-error "block assembly operand not recognized" } */
  rlwinm   r0,r2,16+kUndefC,31		/* { dg-error "block assembly operand not recognized" } */
  cmpwi	   cr7,r1,kUndefC-1		/* { dg-error "block assembly operand not recognized" } */
}

int main (int argc, char * const argv[]) 
{
  foo();
}
