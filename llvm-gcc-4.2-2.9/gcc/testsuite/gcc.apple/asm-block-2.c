/* APPLE LOCAL file CW asm blocks */
/* Test structure refs in asm-syntax blocks within functions.  */

/* { dg-do run { target powerpc*-*-* } } */
/* { dg-options "-fasm-blocks -O2" } */

void abort(void);


typedef struct astruct {
  int arr[40];
  char charfield;
  int fieldx;
  int fieldy;
} atypedef;

union aunion {
  int field1;
  int field2;
};

int fun1 (struct astruct *x)
{
  int loc;
  asm {
    lwz loc, astruct.fieldx(x)
    addi loc, loc, 42
    stw loc, astruct.fieldx+4(x)
  }
  return loc;
}

int fun2 (atypedef *x)
{
  int loc;

  asm {
    lwz loc, atypedef.fieldx(r3)
    addi loc, loc, 43
    stw loc, 4 + astruct.fieldx(x)
  }
  return loc;
}

int fun3(int arg)
{
  int loc;

  asm {
    mr loc, r3
    addi loc, loc, aunion.field1
  }
  return loc;
}

int fun4 (struct astruct *arg)
{
  int loc;
  asm {
    lbz loc, arg->charfield
    addi loc, loc, 1
  }
  return loc;
}

struct astruct glob;
union uglob;

int
main ()
{
  glob.charfield = 'b';
  glob.fieldx = 22;
  if (fun1 (&glob) != 64)
    abort ();
  if (glob.fieldy != 64)
    abort ();
  if (fun2 (&glob) != 65)
    abort ();
  if (glob.fieldy != 65)
    abort ();
  if (fun3 (89) != 89)
    abort ();
  if (fun4 (&glob) != 'c')
    abort ();
  return 0;
}
