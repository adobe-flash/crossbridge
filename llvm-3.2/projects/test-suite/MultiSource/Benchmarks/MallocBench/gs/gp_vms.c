/* Copyright (C) 1989, 1990 Aladdin Enterprises.  All rights reserved.
   Distributed by Free Software Foundation, Inc.

This file is part of Ghostscript.

Ghostscript is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY.  No author or distributor accepts responsibility
to anyone for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.  Refer
to the Ghostscript General Public License for full details.

Everyone is granted permission to copy, modify and redistribute
Ghostscript, but only under the conditions described in the Ghostscript
General Public License.  A copy of this license is supposed to have been
given to you along with Ghostscript so you can know your rights and
responsibilities.  It should be in a file named COPYING.  Among other
things, the copyright notice and this notice must be preserved on all
copies.  */

/* gp_vms.c */
/* VAX/VMS specific routines for Ghostscript */
#include "gx.h"

/* Do platform-dependent interpreter initialization */
void
gp_init()
{
}

/* Read the current date (in days since Jan. 1, 1980) */
/* and time (in milliseconds since midnight). */
void
gs_get_clock(long *pdt)
{	struct {unsigned long int _l0, _l1;} binary_date;
        long lib$day(), sys$bintim();
	long days, days0, seconds;
	char *jan_1_1980 = "1-JAN-1980";
	char *midnight   = "00:00:00.00";
	struct { unsigned short  dsc$w_length;
		 unsigned char   dsc$b_dtype;
		 unsigned char   dsc$b_class;
		 char           *dsc$a_pointer;} str_desc;

	/* Get days from system zero date (November 17, 1858) to present. */
	(void) lib$day (&days0);

	/* For those interested, Wednesday, November 17, 1858 is the base
	   of the Modified Julian Day system adopted by the Smithsonian
	   Astrophysical Observatory in 1957 for satellite tracking.  (The
	   year 1858 preceded the oldest star catalog in use at the
	   observatory.)  VMS uses quadword time stamps which are offsets
	   in 100 nanosecond units from November 17, 1858.  With a 63-bit
	   absolute time representation (sign bit must be clear), VMS will
	   have no trouble with time until 31-JUL-31086 02:48:05.47. */

	/* Convert January 1, 1980 into a binary absolute time */
	str_desc.dsc$w_length  = strlen(jan_1_1980);
	str_desc.dsc$a_pointer = jan_1_1980;
	(void) sys$bintim (&str_desc, &binary_date);

	/* Now get days from system zero date to January 1, 1980 */
	(void) lib$day (&days, &binary_date);

	/* Now compute number of days since January 1, 1980 */
	pdt[0] = 1 + days0 - days;

	/* Convert midnight into a binary delta time */
	str_desc.dsc$w_length  = strlen(midnight);
	str_desc.dsc$a_pointer = midnight;
	(void)  sys$bintim (&str_desc, &binary_date);

	/* Now get number 10 millisecond time units since midnight */
	(void) lib$day (&days, &binary_date, &seconds);
	pdt[1] = 10 * seconds;
}

/* ------ File name syntax ------ */

/* Define the character used for separating file names in a list. */
char gp_file_name_list_separator = ';';

/* Answer whether a file name contains a directory/device specification, */
/* i.e. is absolute (not directory- or device-relative). */
int
gp_file_name_is_absolute(char *fname, uint len)
{	/****** THIS NEEDS TO BE WRITTEN ******/
	return 0;
}

/* Answer the string to be used for combining a directory/device prefix */
/* with a base file name.  The file name is known to not be absolute. */
char *
gp_file_name_concat_string(char *prefix, uint plen, char *fname, uint len)
{	/****** THIS NEEDS TO BE CHECKED ******/
	if ( plen > 0 )
	  switch ( prefix[plen - 1] )
	   {	case ':': case ']': return "";
	   };
	return ":";
}
