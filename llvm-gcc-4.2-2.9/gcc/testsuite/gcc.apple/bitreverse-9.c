/* APPLE LOCAL file */
#include <stdio.h>
#include <string.h>

/* Test reset. */

/* { dg-do run { target powerpc*-*-darwin* } } */
/* { dg-options "-std=gnu99" } */

#pragma reverse_bitfields on
#pragma reverse_bitfields reset
typedef struct
{
	union
		{
		unsigned int i1;
		struct
			{
			short b1: 1;
			unsigned int b2: 2;
			char b3: 4;
			unsigned int b4: 8;
			unsigned int b5: 16;
			} bits;
		} u1;

	short baz;
	union
		{
		struct
			{
			unsigned int i2;
			unsigned int i3;
			unsigned int i4;
			unsigned int i5;
			} ints;

		struct
			{
			unsigned int b1: 16;
			unsigned int b2: 8;
			unsigned int b3: 4;
			char quux;
			unsigned int b4: 2;
			unsigned int b5: 1;
			short baz;
			unsigned int b6: 2;
			unsigned int b7: 4;
			short baz2;
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
	bitfields.baz = 0x5555;
	bitfields.u2.bits.b1 = 1;
	bitfields.u2.bits.b2 = 1;
	bitfields.u2.bits.b3 = 1;
        bitfields.u2.bits.quux = 0xa55a;  /* { dg-warning "overflow in implicit constant conversion" } */
	bitfields.u2.bits.b4 = 1;
	bitfields.u2.bits.b5 = 1;
	bitfields.u2.bits.baz = 0xaaaa;
	bitfields.u2.bits.b6 = 1;
	bitfields.u2.bits.b7 = 1;
	bitfields.u2.bits.baz2 = 0x3333;
	bitfields.u2.bits.b8 = 1;
	bitfields.u2.bits.b9 = 1;

	if (bitfields.u1.i1 != 0xa2020002
	    || bitfields.baz != 0x5555
	    || bitfields.u2.ints.i2 != 0x00010110
	    || bitfields.u2.ints.i3 != 0x5a60aaaa
	    || bitfields.u2.ints.i4 != 0x44003333
	    || bitfields.u2.ints.i5 != 0x01000100
	    || sizeof(bitfields) != 24)
	  return 42;
    return 0;
}
