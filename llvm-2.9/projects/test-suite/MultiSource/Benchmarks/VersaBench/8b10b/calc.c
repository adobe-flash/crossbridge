/*
 * Copyright (c) 2002 David Wentzlaff
 *
 * Permission  is hereby  granted,  free  of  charge, to  any  person
 * obtaining a  copy of  this software  and  associated documentation
 * files   (the  "Software"),  to   deal  in  the   Software  without
 * restriction, including without  limitation the rights to use, copy,
 * modify, merge, publish,  distribute, sublicense, and/or sell copies
 * of  the Software,  and to  permit persons to  whom the  Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above  copyright notice  and this  permission notice  shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE  SOFTWARE IS  PROVIDED "AS IS",  WITHOUT WARRANTY OF  ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING  BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY,   FITNESS   FOR   A   PARTICULAR    PURPOSE    AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM,  DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,  OUT OF OR IN
 * CONNECTION  WITH THE SOFTWARE OR  THE USE OR OTHER  DEALINGS IN THE
 * SOFTWARE.  
 */

//This is part of David Wentzlaff's Masters Thesis
//This file is Copyright David Wentzlaff 2002 All Rights Reserved.
//
// Filename : calc.c
// Date : 07/31/2002

/* Modified by: Rodric M. Rabbah 06-03-04 */

#include <stdio.h>

#include "calc.h"

//note in all of these things, we assume that the K bit is the
//high order bit

//also note that every thing here is little endian 
//and the IBM systems journal paper is big endian.
//so that means that D.1 is 10000 which lives in
//locattion 16 of the table not 1!

#define ENCODE5B(DATA, FLIP, PLUS, MINUS, X)	(((DATA)<<4)&0x3e0)|(((DATA)<<1)&2)|((((FLIP)<<3)|((PLUS)<<2)|((MINUS)<<1)|((X)))<<16)
#define ENCODE3B(DATA, FLIP, PLUS, MINUS, X)	((DATA)&1)|(((DATA)<<1)&0x1c)|((((FLIP)<<3)|((PLUS)<<2)|((MINUS)<<1)|((X)))<<16)

#define GET_FLIP_5(DATA)	(((DATA)>>(16+3))&1)
#define GET_PLUS_5(DATA)	(((DATA)>>(16+2))&1)
#define GET_MINUS_5(DATA)	(((DATA)>>(16+1))&1)
#define GET_X_5(DATA)	(((DATA)>>(16))&1)

#define GET_FLIP_3(DATA)	(((DATA)>>(16+3))&1)
#define GET_PLUS_3(DATA)	(((DATA)>>(16+2))&1)
#define GET_MINUS_3(DATA)	(((DATA)>>(16+1))&1)
#define GET_X_3(DATA)	(((DATA)>>(16))&1)

#define B5_MASK	0x3e2
#define B3_MASK	0x1d

//setup the table for the 5b/6b part
unsigned int lookupTable5B[64] = 
{
   /* 0 */ ENCODE5B(0x18,1,1,0,0),
   /* 1 */ ENCODE5B(0x1b,1,0,1,0),
   /* 2 */ ENCODE5B(0x06,1,1,0,0),
   /* 3 */ ENCODE5B(0x0c,1,1,0,0),
   /* 4 */ ENCODE5B(0x0a,1,1,0,0),
   /* 5 */ ENCODE5B(0x0b,0,0,0,1),
   /* 6 */ ENCODE5B(0x0d,0,0,0,1),
   /* 7 */ ENCODE5B(0x0e,0,0,0,1),
   /* 8 */ ENCODE5B(0x12,1,1,0,0),
   /* 9 */ ENCODE5B(0x13,0,0,0,1),
   /* 10 */ ENCODE5B(0x15,0,0,0,1),
   /* 11 */ ENCODE5B(0x16,0,0,0,1),
   /* 12 */ ENCODE5B(0x19,0,0,0,1),
   /* 13 */ ENCODE5B(0x1a,0,0,0,1),
   /* 14 */ ENCODE5B(0x1c,0,0,0,1),
   /* 15 */ ENCODE5B(0x1e,1,0,1,0),
   /* 16 */ ENCODE5B(0x22,1,1,0,0),
   /* 17 */ ENCODE5B(0x23,0,0,0,1),
   /* 18 */ ENCODE5B(0x25,0,0,0,1),
   /* 19 */ ENCODE5B(0x26,0,0,0,1),
   /* 20 */ ENCODE5B(0x29,0,0,0,1),
   /* 21 */ ENCODE5B(0x2a,0,0,0,1),
   /* 22 */ ENCODE5B(0x2c,0,0,0,1),
   /* 23 */ ENCODE5B(0x2e,1,0,1,0),
   /* 24 */ ENCODE5B(0x31,0,0,0,1),
   /* 25 */ ENCODE5B(0x32,0,0,0,1),
   /* 26 */ ENCODE5B(0x34,0,0,0,1),
   /* 27 */ ENCODE5B(0x36,1,0,1,0),
   /* 28 */ ENCODE5B(0x38,0,0,1,0),
   /* 29 */ ENCODE5B(0x3a,1,0,1,0),
   /* 30 */ ENCODE5B(0x28,1,1,0,0),
   /* 31 */ ENCODE5B(0x2b,1,0,1,0),

   /* 0 */ ENCODE5B(0x18,1,1,0,0),
   /* 1 */ ENCODE5B(0x1b,1,0,1,0),
   /* 2 */ ENCODE5B(0x06,1,1,0,0),
   /* 3 */ ENCODE5B(0x0c,1,1,0,0),
   /* 4 */ ENCODE5B(0x0a,1,1,0,0),
   /* 5 */ ENCODE5B(0x0b,0,0,0,1),
   /* 6 */ ENCODE5B(0x0d,0,0,0,1),
   /* 7 */ ENCODE5B(0x0f,1,0,1,0),
   /* 8 */ ENCODE5B(0x12,1,1,0,0),
   /* 9 */ ENCODE5B(0x13,0,0,0,1),
   /* 10 */ ENCODE5B(0x15,0,0,0,1),
   /* 11 */ ENCODE5B(0x16,0,0,0,1),
   /* 12 */ ENCODE5B(0x19,0,0,0,1),
   /* 13 */ ENCODE5B(0x1a,0,0,0,1),
   /* 14 */ ENCODE5B(0x1c,0,0,0,1),
   /* 15 */ ENCODE5B(0x1e,1,0,1,0),
   /* 16 */ ENCODE5B(0x22,1,1,0,0),
   /* 17 */ ENCODE5B(0x23,0,0,0,1),
   /* 18 */ ENCODE5B(0x25,0,0,0,1),
   /* 19 */ ENCODE5B(0x26,0,0,0,1),
   /* 20 */ ENCODE5B(0x29,0,0,0,1),
   /* 21 */ ENCODE5B(0x2a,0,0,0,1),
   /* 22 */ ENCODE5B(0x2c,0,0,0,1),
   /* 23 */ ENCODE5B(0x2e,1,0,1,0),
   /* 24 */ ENCODE5B(0x31,0,0,0,1),
   /* 25 */ ENCODE5B(0x32,0,0,0,1),
   /* 26 */ ENCODE5B(0x34,0,0,0,1),
   /* 27 */ ENCODE5B(0x36,1,0,1,0),
   /* 28 */ ENCODE5B(0x38,0,0,1,0),
   /* 29 */ ENCODE5B(0x3a,1,0,1,0),
   /* 30 */ ENCODE5B(0x28,1,1,0,0),
   /* 31 */ ENCODE5B(0x2b,1,0,1,0)
};

//setup the table for the 3b/4b part
unsigned int lookupTable3B[16] = 
{
  /* 0 */ ENCODE3B(0x4,1,1,0,0),
  /* 1 */ ENCODE3B(0x2,1,1,0,0),
  /* 2 */ ENCODE3B(0x5,0,0,0,1),
  /* 3 */ ENCODE3B(0x6,0,0,0,1),
  /* 4 */ ENCODE3B(0x9,0,0,0,1),
  /* 5 */ ENCODE3B(0xa,0,0,0,1),
  /* 6 */ ENCODE3B(0xc,0,0,1,0),
  /* 7 */ ENCODE3B(0xe,1,0,1,0),

  /* 0 */ ENCODE3B(0x4,1,1,0,0),
  /* 1 */ ENCODE3B(0x2,1,1,0,0),
  /* 2 */ ENCODE3B(0x5,0,1,0,0),
  /* 3 */ ENCODE3B(0x6,0,1,0,0),
  /* 4 */ ENCODE3B(0x9,0,1,0,0),
  /* 5 */ ENCODE3B(0xa,0,1,0,0),
  /* 6 */ ENCODE3B(0xc,0,0,1,0),
  /* 7 */ ENCODE3B(0x7,1,0,1,0)
};

unsigned int bigTable[1024];

//we are assuming here that 1 equals minus and 0 is plus
unsigned int disparity0 = 0;
unsigned int disparity1 = 1;

void calcSetup()
{
}

unsigned int calc(unsigned int theWord, unsigned int k)
{
	unsigned int index5;
	unsigned int index3;
	unsigned int lookup5;
	unsigned int lookup3;
	unsigned int result;
	index5 = ((theWord>>3)&0x1f)|(k<<5);
	index3 = ((theWord)&0x7)|(k<<3);
	lookup5 = lookupTable5B[index5];
	lookup3 = lookupTable3B[index3];
	if(GET_X_5(lookup5))
	{
		//set the output and don't touch the parity
		result = lookup5 & B5_MASK;
		disparity1 = disparity0;
	}
	else
	{
		//ugg I need to think about this one
		if(disparity0 == GET_PLUS_5(lookup5))
		{
			//this means that we are comming in the correct way
			result = (lookup5 & B5_MASK);
		}
		else
		{
			//this means that we are comming in the wrong way
			//and that means that we need to use the alternative
			//word
			result = (lookup5 & B5_MASK)^B5_MASK;
		}
		if(GET_FLIP_5(lookup5))
		{
			disparity1 = disparity0 ^ 1;
		}
		else
		{
			disparity1 = disparity0;
		}
	}
	if(GET_X_3(lookup3))
	{
		//set the output and don't touch the parity
		result |= lookup3 & B3_MASK;
		disparity0 = disparity1;
	}
	else
	{
		//ugg I need to think about this one
		if(disparity1 == GET_PLUS_3(lookup3))
		{
			//this means that we are comming in the correct way
			result |= (lookup3 & B3_MASK);
		}
		else
		{
			//this means that we are comming in the wrong way
			//and that means that we need to use the alternative
			//word
			result |= (lookup3 & B3_MASK)^B3_MASK;
		}
		if(GET_FLIP_3(lookup3))
		{
			disparity0 = disparity1 ^ 1;
		}
		else
		{
			disparity0 = disparity1;
		}
	}
	return result;
}

unsigned int bigTableCalc(unsigned int theWord)
{
	unsigned int result;
	result = bigTable[(disparity0<<9)|(theWord)];
	disparity0 = result >> 16;
	return (result&0x3ff);
}

void resetDisparity()
{
        disparity0 = 0;
}

void bigTableSetup()
{
	int counter;
	unsigned int tempResult;
	for(counter = 0; counter < 1024; counter++)
	{
		disparity0 = counter >> 9;
		tempResult = calc(counter & 0xff, ((counter >>8) & 1));
		bigTable[counter] = tempResult | disparity0 << 16;
	}
	resetDisparity();
}
