/*
 * packet.h - Interface for reading packet information
 *
 * This file is a part of the NetBench suite by Gokhan Memik
 *
 * This source file is distributed "as is" in the hope that it will be
 * useful.  The suite comes with no warranty, and no author or
 * distributor accepts any responsibility for the consequences of its
 * use. 
 * 
 * You are welcome to use, share and improve this
 * source file.  You are forbidden to forbid anyone else to use, share
 * and improve what you give them.
 *
 */
#define MAX_PACKETS 100

char *get_next_packet ();

/* For testing...*/
unsigned int packet_size (unsigned int);

