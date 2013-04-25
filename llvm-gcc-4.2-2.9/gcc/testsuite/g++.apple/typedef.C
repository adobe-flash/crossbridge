/* APPLE LOCAL file, C++ typedefs of anonymous structs.  */
/* Radar 6007135  */
/* { dg-do compile { target *-*-darwin* } } */
/* LLVM LOCAL - -fverbose-asm */
/* { dg-options "-O0 -gdwarf-2 -dA -fverbose-asm" } */
/* { dg-final { scan-assembler "DW_TAG_typedef" } } */


typedef struct
{
  int exit_code;
} type_t;

int main (int argc, char const *argv[])
{
  type_t t = { 0 };
  return t.exit_code;
}
