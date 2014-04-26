/* APPLE LOCAL file CW asm blocks */
/* Test structure refs in asm-syntax blocks within functions.  */

/* { dg-do run { target powerpc*-*-darwin* } } */
/* { dg-options "-fasm-blocks -O2" } */

extern "C" void abort(void);

typedef struct astruct {
  int arr[40];
  char charfield;
  int fieldx;
} atypedef;

union aunion {
  int field1;
  int field2;
};

class aclass {
 public:
  double headfield;
  int intfield;
};

int fun1 (struct astruct *x)
{
  int loc;
  asm {
    lwz loc, astruct.fieldx(x)
    addi loc, loc, 42
  }
  return loc;
}

int fun2 (atypedef *x)
{
  int loc;

  asm {
    lwz loc, atypedef.fieldx(r3)
    addi loc, loc, 43
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

int fun5 (aclass *x)
{
  int loc;
  asm {
    lwz loc, aclass.intfield(x)
    addi loc, loc, 95
  }
  return loc;
}

struct astruct glob;
union uglob;

int
main ()
{
  aclass *localobj = new aclass;

  glob.charfield = 'b';
  glob.fieldx = 22;
  if (fun1 (&glob) != 64)
    abort ();
  if (fun2 (&glob) != 65)
    abort ();
  if (fun3 (89) != 89)
    abort ();
  if (fun4 (&glob) != 'c')
    abort ();
  localobj->intfield = 55;
  if (fun5 (localobj) != 150)
    abort ();
  return 0;
}
