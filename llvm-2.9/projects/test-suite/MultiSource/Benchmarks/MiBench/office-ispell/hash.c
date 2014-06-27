#ifndef lint
static char Rcs_Id[] =
    "$Id: hash.c 33046 2007-01-09 23:57:19Z lattner $";
#endif

/*
 * hash.c - a simple hash function for ispell
 *
 * Pace Willisson, 1983
 *
 * Copyright 1992, 1993, Geoff Kuenning, Granada Hills, CA
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
 * Revision 1.1.1.1  2007/01/09 02:58:51  evancheng
 * Add selected tests from MiBench 1.0 to LLVM test suite.
 *
 * Revision 1.20  1994/01/25  07:11:34  geoff
 * Get rid of all old RCS log lines in preparation for the 3.1 release.
 *
 */

#include "config.h"
#include "ispell.h"
#include "proto.h"

int		hash P ((ichar_t * word, int hashtblsize));

/*
 * The following hash algorithm is due to Ian Dall, with slight modifications
 * by Geoff Kuenning to reflect the results of testing with the English
 * dictionaries actually distributed with ispell.
 */
#define HASHSHIFT   5

#ifdef NO_CAPITALIZATION_SUPPORT
#define HASHUPPER(c)	c
#else /* NO_CAPITALIZATION_SUPPORT */
#define HASHUPPER(c)	mytoupper(c)
#endif /* NO_CAPITALIZATION_SUPPORT */

int hash (s, hashtblsize)
    register ichar_t *	s;
    register int	hashtblsize;
    {
    register long	h = 0;
    register int	i;

#ifdef ICHAR_IS_CHAR
    for (i = 4;  i--  &&  *s != 0;  )
	h = (h << 8) | HASHUPPER (*s++);
#else /* ICHAR_IS_CHAR */
    for (i = 2;  i--  &&  *s != 0;  )
	h = (h << 16) | HASHUPPER (*s++);
#endif /* ICHAR_IS_CHAR */
    while (*s != 0)
	{
	/*
	 * We have to do circular shifts the hard way, since C doesn't
	 * have them even though the hardware probably does.  Oh, well.
	 */
	h = (h << HASHSHIFT)
	  | ((h >> (32 - HASHSHIFT)) & ((1 << HASHSHIFT) - 1));
	h ^= HASHUPPER (*s++);
	}
    return (unsigned long) h % hashtblsize;
    }
