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

#ifndef _BITBOARD64_H
#define _BITBOARD64_H

#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdint.h>

class BitBoard64
{
 public:
	BitBoard64(void) {};
	BitBoard64(uint32_t l, uint32_t h) {lowbits = l, highbits = h;};

	void setBit(int bit);
	void unSetBit(int bit);
	int  getBit(int bit);

	BitBoard64 operator&(const BitBoard64 &right);
	BitBoard64 operator|(const BitBoard64 &right);
	BitBoard64 operator^(const BitBoard64 &right);
	BitBoard64 operator=(const BitBoard64 &right);
	BitBoard64 operator~(void);

	operator bool();

	void print(void);

	int readFromFile(FILE *file);
	int writeToFile(FILE *file);

 private:
	
	uint32_t lowbits, highbits;
};

#endif
