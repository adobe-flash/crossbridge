/* APPLE LOCAL file radar 6131435 */
/* { dg-do compile { target { { i?86-*-* x86_64-*-* } && lp64 } } } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler "movb" } } */
/* { dg-final { scan-assembler "movw" } } */
/* { dg-final { scan-assembler "andw" } } */
#define  ARRAY_LENGTH  16
union bitfield {
  struct {
    unsigned int field0 : 6;
    unsigned int field1 : 6;
    unsigned int field2 : 6;
    unsigned int field3 : 6;
    unsigned int field4 : 3;
    unsigned int field5 : 4;
    unsigned int field6 : 1;
  } bitfields, bits;
  unsigned int	 u32All;
  signed int	 i32All;
  float	         f32All;
};

typedef struct program_t {
  union bitfield array[ARRAY_LENGTH];
} program;

void foo(program* prog, unsigned int fmt1)
{
  unsigned int shift = 0;
  unsigned int texCount = 0;
  unsigned int i;
	
  for (i = 0; i < 8; i++)
  {
    prog->array[i].bitfields.field0 = texCount;
    prog->array[i].bitfields.field1 = texCount + 1;
    prog->array[i].bitfields.field2 = texCount + 2;
    prog->array[i].bitfields.field3 = texCount + 3;
    texCount += (fmt1 >> shift) & 0x7;
    shift    += 3;
  }
}
/* APPLE LOCAL file radar 4287182 */
