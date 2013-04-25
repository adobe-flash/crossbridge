/* APPLE LOCAL file */
#include <stdio.h>
#include <string.h>

/* Tests turning pragma on and off, and that setting in effect at time
   of the struct definition, not the variable definition, is what matters.
.  Layout same as bitreverse-0.  */

/* { dg-do run { target powerpc*-*-darwin* } } */

#pragma reverse_bitfields on
#pragma reverse_bitfields off
typedef struct
{
	union
		{
		unsigned int i1;
		struct
			{
			unsigned int b1: 1;
			unsigned int b2: 2;
			unsigned int b3: 4;
			unsigned int b4: 8;
			unsigned int b5: 16;
			} bits;
		} u1;

	union
		{
		struct
			{
			unsigned int i2;
			unsigned int i3;
			} ints;

		struct
			{
			unsigned int b1: 16;
			unsigned int b2: 8;
			unsigned int b3: 4;
			unsigned int b4: 2;
			unsigned int b5: 1;
			unsigned int b6: 2;
			unsigned int b7: 4;
			unsigned int b8: 8;
			unsigned int b9: 16;
			} bits;
		} u2;
} Bitfields;

#pragma ms_struct on
#pragma reverse_bitfields on
int main()
{
	Bitfields bitfields;


	memset(&bitfields, 0, sizeof(bitfields));

	bitfields.u1.bits.b1 = 1;
	bitfields.u1.bits.b2 = 1;
	bitfields.u1.bits.b3 = 1;
	bitfields.u1.bits.b4 = 1;
	bitfields.u1.bits.b5 = 1;

	bitfields.u2.bits.b1 = 1;
	bitfields.u2.bits.b2 = 1;
	bitfields.u2.bits.b3 = 1;
	bitfields.u2.bits.b4 = 1;
	bitfields.u2.bits.b5 = 1;
	bitfields.u2.bits.b6 = 1;
	bitfields.u2.bits.b7 = 1;
	bitfields.u2.bits.b8 = 1;
	bitfields.u2.bits.b9 = 1;

	if (bitfields.u1.i1 != 0xa2020002
	    || bitfields.u2.ints.i2 != 0x00010116
	    || bitfields.u2.ints.i3 != 0x44040004)
	  return 42;
    return 0;
}
