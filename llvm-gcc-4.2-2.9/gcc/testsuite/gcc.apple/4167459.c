/* APPLE LOCAL begin radar 4167459 */
/* { dg-do compile { target i?86-*-darwin* } } */
/* { dg-require-effective-target ilp32 } */
/* { dg-options "-O -march=pentium4" } */
extern double foo(double);

typedef union { 
  int int_val;
  double real_val;
} Val_type;

typedef enum { UNKNOWN, INT, REAL, BOOL, FCT, STRING } Data_type;

void ln(int res, int op1)
{
  Data_type res_type, op1_type;
  Val_type res_val, op1_val;

  res_val.real_val = foo((double)((op1_type == REAL) ? op1_val.real_val : op1_val.int_val));

  st_set_val_type(res, res_type, res_val);
}
/* APPLE LOCAL end radar 4167459 */
