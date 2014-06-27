/*
 * Hexxagon board game.
 * Copyright (C) 2001 Erik Jonsson.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * Email erik@nesqi.homeip.net
 * 
 */

#include "bitboard64.h"
#include <stdio.h>
#include <iostream>
#include <netinet/in.h>

using namespace std;

BitBoard64 BitBoard64::operator~(void)
{
	BitBoard64 ret(~lowbits, ~highbits);
	return ret;
}

BitBoard64 BitBoard64::operator&(const BitBoard64 &right)
{
	BitBoard64 ret(lowbits & right.lowbits, highbits & right.highbits);
	return ret;
}

BitBoard64 BitBoard64::operator|(const BitBoard64 &right)
{
	BitBoard64 ret(lowbits | right.lowbits, highbits | right.highbits);
	return ret;
}

BitBoard64 BitBoard64::operator=(const BitBoard64 &right)
{
	lowbits = right.lowbits;
	highbits = right.highbits;
	return *this;
}

BitBoard64 BitBoard64::operator^(const BitBoard64 &right)
{
	BitBoard64 ret(lowbits ^ right.lowbits, highbits ^ right.highbits);
	return ret;
}

BitBoard64::operator bool()
{
	return (bool)(lowbits || highbits);
}

void BitBoard64::setBit(int bit)
{
	if((bit < 0) || (bit > 63))
		return;

	if(bit < 32)
		lowbits |= (1 << bit);
	else
		highbits |= (1 << (bit-32));
}

void BitBoard64::unSetBit(int bit)
{
	if((bit < 0) || (bit > 63))
		return;

	if(bit < 32)
		lowbits &= ~(1 << bit);
	else
		highbits &= ~(1 << (bit-32));
}

int BitBoard64::getBit(int bit)
{
	if((bit < 0) || (bit > 63))
		return -1;

	if(bit < 32)
		return lowbits & (1 << bit);
	else
		return highbits & (1 << (bit-32));
}

int BitBoard64::readFromFile(FILE *file)
{
	 lowbits = 0;
	 highbits = 0;

	 /* Just to get the same byte order. 
		Could not use htonl; long int could be different sizes. */
	 for(int i = 0; i < 4; i++)
	 {
		 uint8_t byte = 0;
		 
		 if(fread(&byte, 1, 1, file) != 1) 
			 return -1;

		 lowbits |=  byte << (i * 8);
	 }

	 for(int i = 0; i < 4; i++)
	 {
		 uint8_t byte = 0;
		 
		 if(fread(&byte, 1, 1, file) != 1) 
			 return -1;

		 highbits |=  byte << (i * 8);
	 }

	 return 0;
}

int BitBoard64::writeToFile(FILE *file)
{
	/* Just to get the same byte order. */
	for(int i = 0; i < 4; i++)
	{
		uint8_t byte = 0;
		
		byte = (lowbits >> (i * 8)) & 0xFF;
	
		if(fwrite(&byte, 1, 1, file) != 1) 
			return -1;

	}

	for(int i = 0; i < 4; i++)
	{
		uint8_t byte = 0;
		
		byte = (highbits >> (i * 8)) & 0xFF;
	
		if(fwrite(&byte, 1, 1, file) != 1) 
			return -1;

	}
	
	return 0;
}


/*********************/
/* Debug struff      */
int getBFP(int x, int y)
{
	int no;

	if(x > 0 && x < 10 && y > 0 && y < 10)
	{
		if(y < 5)
			if(x > (9 - (5 - y)))
				return -1;

		if(y > 5)
			if(x <= (y - 5))
				return -1;
	  
		no = x+y*9 - 10;
	  
		if(y > 1) no -= 4;
		if(y > 2) no -= 3;
		if(y > 3) no -= 2;
		if(y > 4) no -= 1;
		if(y > 5) no -= 1;
		if(y > 6) no -= 2;
		if(y > 7) no -= 3;
		if(y > 8) no -= 4;
	  
		return no;
	}

	return -1;
}

void BitBoard64::print()
{
	int x, y, no, off;

	for(y = 1; y < 10; y++)
	{
		if(y < 5)
		{
			for(off = 0; off < (5 - y); off++)
				cout << " ";
		}
		for(x = 1; x < 10; x++)
		{
			if((no = getBFP(x, y)) == -1)
				cout << " ";
			else
			{
				if(getBit(no))
					cout << "x ";
				else
					cout << ". ";
			}
		}
		cout << "\n";
	}
}



