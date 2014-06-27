/* uudecode.c -- uudecode utility.
 * Copyright (C) 1994, 1995 Free Software Foundation, Inc.
 *
 * This product is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This product is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this product; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

/* Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *       This product includes software developed by the University of
 *       California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/* Reworked to GNU style by Ian Lance Taylor, ian@airs.com, August 93.  */
/* Altered to fit my benchmarking rules in april 2006 by Per Gustafsson  */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#define MAX_SIZE 10000000

int skip_to_newline(char* input, int offset) {					/*1*/
  while(input[offset] != 10)							/*2*/
    {offset++;}									/*3*/
  return ++offset;  								/*4*/
}

decode_char(char in) {								/*5*/
  return ((in) - ' ') & 63;							/*6*/
}

int decode(char* input, int offset, char* output) {				/*7*/
  int ooffset = 0;								/*8*/
  while (input[offset] != 32) {							/*9*/
    int encodedoctets;								/*10*/
    encodedoctets = decode_char(input[offset]);					/*11*/
    for (++offset; encodedoctets > 0; offset += 4, encodedoctets -= 3) {	/*12*/ 
      char ch;									/*13*/
      if (encodedoctets >= 3) {							/*14*/
        ch = decode_char (input[offset]) << 2 | 				/*15*/
	  decode_char (input[offset+1]) >> 4;					/*16*/
        output[ooffset++] = ch;							/*17*/
        ch = decode_char (input[offset+1]) << 4 | 				/*18*/
	  decode_char (input[offset+2]) >> 2;					/*19*/
        output[ooffset++] = ch;							/*20*/
        ch = decode_char (input[offset+2]) << 6 | 				/*21*/
	  decode_char (input[offset+3]);					/*22*/
        output[ooffset++] = ch;							/*23*/
      } else {
        if (encodedoctets >= 1) {						/*24*/
          ch = decode_char (input[offset]) << 2 | 				/*25*/
	    decode_char (input[offset+1]) >> 4;					/*26*/
          output[ooffset++] = ch;						/*27*/
        }
        if (encodedoctets >= 2) {						/*28*/
          ch = decode_char (input[offset+1]) << 4 | 				/*29*/
	    decode_char (input[offset+2]) >> 2;					/*30*/
          output[ooffset++] = ch;						/*31*/
        }
      }
    }
    offset = skip_to_newline(input, offset);					/*32*/
  }
  offset = skip_to_newline(input, offset);					/*33*/
  if (input[offset]=='e' && input[offset+1]=='n'				/*34*/
      && input[offset+2]=='d') {						/*35*/
    return ooffset;								/*36*/
  }
   exit(1);									/*37*/
 } 



int do_decode(char* input, char* output, char* outfilename) {			/*38*/
  int mode, namelen;								/*39*/
  if (sscanf (input, "begin %o %s \n", &mode, outfilename)==2){			/*40*/
    namelen = strlen(outfilename);						/*41*/
    return decode(input, namelen+12, output);					/*42*/
  }
  exit(1);									/*43*/
}
/*==========================================================================*/

static size_t read_data(FILE *in, void *buffer)
{ 
  return fread(buffer, 1, MAX_SIZE, in);
}

static size_t write_data(FILE *out, int size, void *buffer)
{ 
  return fwrite(buffer, 1, size, out);
}


int main(int argc, char *argv[])
{
  FILE *in,*out;
  int i;
  size_t size;
  int outsize,time;
  unsigned char *inbuf, *outbuf, *temp;
  char outfilename[100];
  char postfix[] = ".c";
  struct timeval pre,post;
  
  /* optional input arg */
  inbuf = malloc(MAX_SIZE);
  outbuf = malloc(MAX_SIZE);
  
  if (argc > 1) {
    //create_test_data(argv[1]); // for testing purposes
    if ((in = fopen(argv[1], "r")) == NULL) {
      perror(argv[1]);
      exit(1);
    }
    argv++; argc--;
  }
  
  else{
    in = stdin;
    out = stdout;
  }
  if (argc != 1) {
    printf("Usage: uudecode [infile]\n");
    exit(2);
  }
  size = read_data(in, inbuf);
  gettimeofday(&pre,0);
  for(i=0;i<100;i++){
    outsize = do_decode(inbuf,outbuf,outfilename);
  }

  printf("%d\n", outsize);
  exit(0); 
}
