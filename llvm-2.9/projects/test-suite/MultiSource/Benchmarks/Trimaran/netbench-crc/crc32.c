/*
 * crc32.c - Routines for crc calculation
 *
 * This file is a part of the NetBench suite
 *
 * This source file is distributed "as is" in the hope that it will be
 * useful. The suite comes with no warranty, and no author or
 * distributor accepts any responsibility for the consequences of its
 * use.
 * 
 * Everyone is granted permission to copy, modify and redistribute
 * this tool set under the following conditions:
 * 
 *    Permission is granted to anyone to make or distribute copies
 *    of this source code, either as received or modified, in any
 *    medium, provided that all copyright notices, permission and
 *    nonwarranty notices are preserved, and that the distributor
 *    grants the recipient permission for further redistribution as
 *    permitted by this document.
 *
 *    Permission is granted to distribute this file in compiled
 *    or executable form under the same conditions that apply for
 *    source code, provied that either:
 *
 *    A. it is accompanied by the corresponding machine-readable
 *       source code,
 *    B. it is accompanied by a written offer, with no time limit,
 *       to give anyone a machine-readable copy of the corresponding
 *       source code in return for reimbursement of the cost of
 *       distribution.  This written offer must permit verbatim
 *       duplication by anyone, or
 *    C. it is distributed by someone who received only the
 *       executable form, and is accompanied by a copy of the
 *       written offer of source code that they received concurrently.
 *
 * In other words, you are welcome to use and share this source file.
 * You are forbidden to forbid anyone else to use, share and improve
 * what you give them. 
 *  
 */

/* crc32.c -- package to compute 32-bit CRC one byte at a time          */
/*                                                                      */
/* Synopsis:                                                            */
/*  gen_crc_table() -- generates a 256-word table containing all CRC    */
/*                     remainders for every possible 8-bit byte.  It    */
/*                     must be executed (once) before any CRC updates.  */
/*                                                                      */
/*  unsigned update_crc(crc_accum, data_blk_ptr, data_blk_size)         */
/*           unsigned crc_accum; char *data_blk_ptr; int data_blk_size; */
/*           Returns the updated value of the CRC accumulator after     */
/*           processing each byte in the addressed block of data.       */
/*                                                                      */
/*  It is assumed that an unsigned long is at least 32 bits wide and    */
/*  that the predefined type char occupies one 8-bit byte of storage.   */
/*                                                                      */
/*  The generator polynomial used for this version of the package is    */
/*  x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x^1+x^0 */
/*  as specified in the Autodin/Ethernet/ADCCP protocol standards.      */
/*  Other degree 32 polynomials may be substituted by re-defining the   */
/*  symbol POLYNOMIAL below.  Lower degree polynomials must first be    */
/*  multiplied by an appropriate power of x.  The representation used   */
/*  is that the coefficient of x^0 is stored in the LSB of the 32-bit   */
/*  word and the coefficient of x^31 is stored in the most significant  */
/*  bit.  The CRC is to be appended to the data most significant byte   */
/*  first.  For those protocols in which bytes are transmitted MSB      */
/*  first and in the same order as they are encountered in the block    */
/*  this convention results in the CRC remainder being transmitted with */
/*  the coefficient of x^31 first and with that of x^0 last (just as    */
/*  would be done by a hardware shift register mechanization).          */
/*                                                                      */
/*  The table lookup technique was adapted from the algorithm described */
/*  by Avram Perez, Byte-wise CRC Calculations, IEEE Micro 3, 40 (1983).*/
/*                                                                      */

#include <stdlib.h>
#include <stdio.h>
#include "packet.h"

#define POLYNOMIAL 0x04c11db7L

static unsigned long crc_table[256];

/* generate the table of CRC remainders for all possible bytes */
void 
gen_crc_table()
{ 
  register int i, j;  
  register unsigned long crc_accum;
  
  for (i = 0;  i < 256;  i++)
    { 
      crc_accum = ((unsigned long) i << 24);
      for (j = 0;  j < 8;  j++)
	{ 
	  if (crc_accum & 0x80000000L)
	    crc_accum = (crc_accum << 1) ^ POLYNOMIAL;
	  else
	    crc_accum = (crc_accum << 1); 
	}
      crc_table[i] = crc_accum; 
    }
  return; 
}

/* update the CRC on the data block one byte at a time */
unsigned long 
update_crc(unsigned long crc_accum, 
	   char *data_blk_ptr,
	   int data_blk_size)
{ 
  register int i, j;
  for (j = 0;  j < data_blk_size;  j++)
    { 
      i = ((int)(crc_accum >> 24) ^ *data_blk_ptr++) & 0xff;
      crc_accum = (crc_accum << 8) ^ crc_table[i]; 
    }
  return crc_accum; 
}

int main (int argc, char **argv)
{
  unsigned long crc_accum;
  int i = 0, numpackets;
  char *packet;
  
  if (argc != 2)
    {
      fprintf (stderr, "Usage: crc #numpackets");
      exit (0);
    }
  else 
    numpackets = atoi (argv[1]);
  
  gen_crc_table();
  
  while (i < numpackets)
    {
      packet = get_next_packet(i);
      crc_accum = update_crc (0, packet, packet_size(i));
      i++;
    }
  
  fprintf (stdout, "CRC completed for %d packets \n", numpackets);
  fprintf (stdout, "crc_accum is %u\n", (unsigned) crc_accum);
  
  return 0;
}
