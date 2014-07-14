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

/* gp_unix.c */
/* Unix-specific routines for Ghostscript */
#include "string_.h"
#include "gx.h"
#include <time.h>
#include <sys/time.h>

/* Do platform-dependent initialization */
void
gp_init()
{
}

/* Read the current date (in days since Jan. 1, 1980) */
/* and time (in milliseconds since midnight). */
void
gs_get_clock(long *pdt)
{	long secs_since_1980;
	struct timeval tp;
	struct timezone tzp;
	struct tm *tm, *localtime();

	if ( gettimeofday(&tp, &tzp) == -1 )
	   {	perror("Ghostscript: gettimeofday failed:");
		exit(-1);
	   }

	/* tp.tv_sec is #secs since Jan 1, 1970 */

	/* subtract off number of seconds in 10 years */
	/* leap seconds are not accounted for */
	secs_since_1980 = tp.tv_sec - (long)(60 * 60 * 24 * 365.25 * 10);
 
	/* adjust for timezone */
	secs_since_1980 -= (tzp.tz_minuteswest * 60);

	/* adjust for daylight savings time - assume dst offset is 1 hour */
	tm = localtime(&(tp.tv_sec));
	if ( tm->tm_isdst )
		secs_since_1980 += (60 * 60);

	/* divide secs by #secs/day to get #days (integer division truncates) */
	pdt[0] = secs_since_1980 / (60 * 60 * 24);
	/* modulo + microsecs/1000 gives number of millisecs since midnight */
	pdt[1] = (secs_since_1980 % (60 * 60 * 24)) * 1000 + tp.tv_usec / 1000;
#ifdef DEBUG_CLOCK
	printf("tp.tv_sec = %d  tp.tv_usec = %d  pdt[0] = %ld  pdt[1] = %ld\n",
		tp.tv_sec, tp.tv_usec, pdt[0], pdt[1]);
#endif
}

/* ------ File name syntax ------ */

/* Define the character used for separating file names in a list. */
char gp_file_name_list_separator = ':';

/* Answer whether a file name contains a directory/device specification, */
/* i.e. is absolute (not directory- or device-relative). */
int
gp_file_name_is_absolute(char *fname, uint len)
{	/* A file name is absolute if it starts with a /. */
	return ( len >= 1 && *fname == '/' );
}

/* Answer the string to be used for combining a directory/device prefix */
/* with a base file name.  The file name is known to not be absolute. */
char *
gp_file_name_concat_string(char *prefix, uint plen, char *fname, uint len)
{	if ( plen > 0 && prefix[plen - 1] == '/' )
		return "";
	return "/";
}
