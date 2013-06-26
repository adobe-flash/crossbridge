#ifndef lint
static char Rcs_Id[] =
    "$Id: xgets.c 33046 2007-01-09 23:57:19Z lattner $";
#endif

/*
 * Copyright 1987, 1988, 1989, 1992, 1993, Geoff Kuenning, Granada Hills, CA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All modifications to the source code must be clearly marked as
 *    such.  Binary redistributions based on modified source code
 *    must be clearly marked as modified versions in the documentation
 *    and/or other materials provided with the distribution.
 * 4. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgment:
 *      This product includes software developed by Geoff Kuenning and
 *      other unpaid contributors.
 * 5. The name of Geoff Kuenning may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY GEOFF KUENNING AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL GEOFF KUENNING OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * $Log$
 * Revision 1.1  2007/01/09 23:57:18  lattner
 * initial recheckin of mibench
 *
 * Revision 1.1.1.1  2007/01/09 02:59:06  evancheng
 * Add selected tests from MiBench 1.0 to LLVM test suite.
 *
 * Revision 1.22  1994/09/16  04:48:34  geoff
 * Be sure to deliver newlines to the caller, so that it can tell whether
 * or not a complete line was read.
 *
 * Revision 1.21  1994/01/25  07:12:22  geoff
 * Get rid of all old RCS log lines in preparation for the 3.1 release.
 *
 */

#include "config.h"
#include "ispell.h"
#include "proto.h"

char *		xgets P ((char * string, int size, FILE * stream));

#ifndef MAXINCLUDEFILES
#define MAXINCLUDEFILES	1	/* maximum number of new files in stack */
#endif

/*
 * xgets () acts just like gets () except that if a line matches
 * "&Include_File&<something>" xgets () will start reading from the
 * file <something>.
 *
 *  Andrew Vignaux -- andrew@vuwcomp  Fri May  8 16:40:23 NZST 1987
 * modified
 *  Mark Davies -- mark@vuwcomp  Mon May 11 22:38:10 NZST 1987
 */

char * xgets (str, size, stream)
    char		str[];
    int			size;
    FILE *		stream;
    {
#if MAXINCLUDEFILES == 0
    return fgets (str, size, stream);
#else
    static char *	Include_File = DEFINCSTR;
    static int		Include_Len = 0;
    static FILE *	F[MAXINCLUDEFILES+1];
    static FILE **	current_F = F;
    char *		s = str;
    int			c;

    /* read the environment variable if we havent already */
    if (Include_Len == 0)
	{
	char *		env_variable;

	if ((env_variable = getenv (INCSTRVAR)) != NULL)
	    Include_File = env_variable;
	Include_Len = strlen (Include_File);

	/* initialise the file stack */
	*current_F = stream;
	}

    for (  ;  ;  )
	{
	c = '\0';
        if ((s - str) + 1 < size
          &&  (c = getc (*current_F)) != EOF)
	    {
	    *s++ = (char) c;
	    if (c != '\n')
		continue;
	    }
	*s = '\0';		/* end of line */
	if (c == EOF)
	    {
	    if (current_F == F) /* if end of standard input */
		{
		if (s == str)
		    return (NULL);
		}
	    else
		{
	        (void) fclose (*(current_F--));
	      	if (s == str) continue;
		}
	    }

	if (incfileflag != 0
	  &&  strncmp (str, Include_File, (unsigned int) Include_Len) == 0)
	    {
	    char *	file_name = str + Include_Len;

	    if (current_F - F < MAXINCLUDEFILES  &&  strlen (file_name) != 0)
		{
		FILE *	f;

		if (f = fopen (file_name, "r"))
		    *(++current_F) = f;
		}
	    s = str;
	    continue;
	    }
	break;
	}
    
    return (str);
#endif
    }
