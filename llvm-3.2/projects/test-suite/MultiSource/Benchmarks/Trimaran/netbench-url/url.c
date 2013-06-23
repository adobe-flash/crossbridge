/*
 * url.c - Routines for URL switching
 *
 * This file is a part of the NetBench
 *
 * This source file is distributed "as is" in the hope that it will be
 * useful. The suite comes with no warranty, and no author or
 * distributor accepts any responsibility for the consequences of its
 * use. 
 * 
 * Everyone is granted permission to copy, modify and redistribute
 * this tool set under the following conditions:
 * 
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
 */

#include <sys/param.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "packet.h"
#include "utils.h"
#include "search.h"
#include "url.h"

#define MAX_PACKET_SIZE 1500

StrTreeNode *tree_head;

long
internet_checksum (addr, count)
     unsigned short * addr;
     int count;
{
  /* 
   *  Compute Internet Checksum for "count" bytes
   *  beginning at location "addr".
   */
  register unsigned long sum = 0;
 
  while( count > 1 ) 
    { 
      sum+= ~((unsigned short)*addr++);
      count -= 2;
    }
  
  /*  Add left-over byte, if any */
  if( count > 0 )
    sum += * (unsigned char *) addr;
  
  /*  Fold 32-bit sum to 16 bits */
  while (sum>>16)
    sum = (sum & 0xffff) + (sum >> 16);
  
#ifdef LITTLE_ENDIAN
  sum = ((sum >> 8) & 0xFF) + ((sum << 8) & 0xFF00);
#endif

  return sum; 
} 

/*************************************************\
*                                                *
* main: Initializes the string table             *
*       Reads packets and compares them against  *
*       the table                                *
*                                                *
\************************************************/
int main (int argc, char **argv)
{
  struct rtentry *entry;
  unsigned int num_packets, i;
  char *packet;

  if (argc != 3)
    fatal ("Usage: url inputfilename #numberofpackets");

  num_packets = atoi (argv[2]);  

  /* Read the string information */
  db_init(argv[1]);

  fprintf (stdout, "URL table initialized, reading packets... \n");

  /* The table is ready, now we get the packets... */
  for (i = 0; i < num_packets; i++)
    {
      long original_checksum, checksum;
      
      packet = get_next_packet(i);
      
      /* We only have to check the checksum to make sure nothing is changed.. */
#ifdef LITTLE_ENDIAN
      original_checksum = ((unsigned char)packet[10] << 8) + (unsigned char)packet[11];
#else
      original_checksum = *(unsigned short *)&packet[10];
#endif
      packet[10] = 0;
      packet[11] = 0;      
      checksum = (internet_checksum ((unsigned short*)packet, 20)) & 0xFFFF;
      
      entry = find_destination (packet, packet_size (i));
      
      checksum = (internet_checksum ((unsigned short*)packet, 20)) & 0xFFFF;
      packet[10] = (checksum & 0xFF00) >> 8;
      packet[11] = checksum & 0xFF;
      
      /* We actually now have to check whether the entry is NULL and
      if so forward the packet to a default destination, but we don't
      have any interface anyway and find_destination always return
      NULL (from different locations indicating a match for the
      packet). So, we don't perform the checking and forwarding. These
      are done in route application. */

      /* Done for the packet... */
      free(packet);
    }
  
  fprintf (stderr, "URL finished for %d packets \n", num_packets);
  return 0;
}
