/* APPLE LOCAL file CW asm blocks */
/* Test asm-syntax blocks within functions.  */

/* { dg-do run { target powerpc*-*-darwin* } } */
/* { dg-options "-fasm-blocks" } */

extern "C" void abort (void);

int
foo (int argx, int argy)
{
  register int locx = argx, locy = argy, rslt;
  asm {
      add rslt,locx,locy
    }
  return rslt;
}

/* Exercise corners of syntax.  */

int
bar ()
{
  register int rslt = 100;
  asm {
    // Combos of instructions on a line

    nop
     ;
    nop
    nop ;
    ; nop
    nop ; nop
    ; nop ;;; li r3,95 ; nop ;
    // '.' in an opcode
    add. r6,r6,r5
  }
  return rslt;
}

int baz ()
{
  register int rslt = 1;
  asm {
    li r8,29+(2 * 3);
    add rslt,rslt,r8;
    b @192
   @34
    nop
    ble+ @34
    b forward
   abc:
    b abc
   @abc:
    b @abc
   forward: ; @192:
    nop
    ;
  sameline: nop
  x1: @x2 @x3: nop ; @x4 x5:
  }
  return rslt;
}

int
main ()
{
  if (foo (22, 23) != 45)
    abort ();
  if (bar () != 100)
    abort ();
  if (baz () != 36)
    abort ();
  return 0;
}
