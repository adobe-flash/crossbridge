/* uuencode.c -- uuencode utility.
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
/* Hacked to work with BusyBox by Alfred M. Szmidt */
/* Rewrritten to fit my benchmark rules by Per Gustafsson */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#define MAX_SIZE 1000000

static const char *mybasename(const char *str) {
  const char *base = strrchr(str, '/');
  return base ? base+1 : str;
}

encode_char(char c) {								/*1*/
  return (32+(c & 63));								/*2*/
}

void encode_line(char* input, int offset, int octets, char* line){		/*3*/
  int loffs=0;									/*4*/
  line[loffs]=encode_char(octets);						/*5*/
  for (++loffs; octets > 0; offset += 3, octets -= 3) {				/*6*/
    char ch;									/*7*/
    if (octets >= 3) {								/*8*/
      ch = encode_char (input[offset] >> 2);  					/*9*/
      line[loffs++] = ch;							/*10*/
      ch = encode_char ((input[offset] << 4) | (input[offset+1] >> 4));		/*11*/
      line[loffs++] = ch;							/*12*/
      ch = encode_char ((input[offset+1] << 2) | (input[offset+2] >> 6));	/*13*/
      line[loffs++] = ch;							/*14*/
      ch = encode_char (input[offset+2]);					/*15*/
      line[loffs++] = ch;							/*16*/
    } else {
      if (octets == 1) {							/*17*/
	ch = encode_char (input[offset] >> 2);					/*18*/
	line[loffs++] = ch;							/*19*/
	ch = encode_char (input[offset] << 4);					/*20*/
	line[loffs++] = ch;							/*21*/
	line[loffs++] = '=';							/*22*/
	line[loffs++] = '=';							/*23*/
      } else {
	if (octets == 2) {							/*24*/
	  ch = encode_char ((input[offset]) >> 2); 				/*25*/ 
	  line[loffs++] = ch;							/*26*/
	  ch = encode_char ((input[offset] << 4) | (input[offset+1] >> 4));	/*27*/
	  line[loffs++] = ch;							/*28*/
	  ch = encode_char (input[offset+1] << 2);				/*29*/
	  line[loffs++] = ch;							/*30*/
	  line[loffs++] = '=';							/*31*/
	}
      }
    }
  }
  line[loffs++] = '\n';								/*32*/
  line[loffs] = 0;								/*33*/
  return ;
}
  
void encode(char* input, int limit, char* output){				/*34*/
  char line[63];								/*35*/
  int offset=0;									/*36*/
  while(offset<limit) {								/*37*/
    if ((limit - offset) >= 45) {						/*38*/
      encode_line(input, offset, 45, line);					/*39*/
      offset += 45;								/*40*/
    }
    else {
      encode_line(input, offset, limit-offset, line);				/*41*/
      offset = limit;								/*42*/
    }
    strcat(output,line);							/*43*/
    output = output + strlen(output);						/*44*/
  }
  strcat(output," \nend");							/*45*/
  return;
} 
 
int do_encode(char* input, char* output, int len, char* infilename) {		/*46*/
  int mode, namelen;								/*47*/
  strcpy(output, "begin 640 ");							/*48*/
  strcat(output,infilename);							/*49*/
  strcat(output, " \n");{							/*50*/
    namelen = strlen(infilename);						/*51*/
    encode(input, len, output+strlen(output));					/*52*/
    return strlen(output);							/*53*/
  }
  exit(1);									/*54*/
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
  struct timeval pre,post;
  
  /* optional input arg */
  inbuf = malloc(MAX_SIZE);
  outbuf = malloc(MAX_SIZE*2);
  
  if (argc > 1) {
    //create_test_data(argv[1]); // for testing purposes
    if ((in = fopen(argv[1], "r")) == NULL) {
      perror(argv[1]);
      exit(1);
    }
    argc--;
  }
  
  else{
    in = stdin;
    out = stdout;
  }
  if (argc != 1) {
    printf("Usage: uuencode [infile]\n");
    exit(2);
  }
  size = read_data(in, inbuf);
  gettimeofday(&pre,0);
  for(i=0;i<1000;i++){
    outsize = do_encode(inbuf,outbuf, size, mybasename(argv[1]));
   
  }

  gettimeofday(&post,0);
  time = ((post.tv_sec*1000000+post.tv_usec)-(pre.tv_sec*1000000+pre.tv_usec));
  
  printf("%d\n", outsize);
  
  exit(0); 
}
