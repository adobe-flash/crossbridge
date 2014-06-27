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

/* memory_.h */
/* Generic substitute for Unix memory.h */

/****** Note: the System V bcmp routine only returns zero or non-zero, ******/
/****** unlike memcmp which returns -1, 0, or 1. ******/

#ifdef __MSDOS__
/* The Turbo C implementation of memset swaps the arguments and calls */
/* the non-standard routine setmem.  We may as well do it in advance. */
#  undef memset				/* just in case */
#  include <mem.h>
#  define memset(dest,chr,cnt) setmem(dest,cnt,chr)
#else
#  ifdef VMS
	extern char *memcpy(), *memset();
	extern int memcmp();
#  else
#    ifdef BSD4_2
	extern bcopy(), bcmp(), bzero();
#       define memcpy(dest,src,len) bcopy(src,dest,len)
#       define memcmp(b1,b2,len) bcmp(b1,b2,len)
	/* Define our own version of memset */
#	ifdef __STDC__
	static void memset(void *dest, register char ch, unsigned len)
#	else
	static void memset(dest, ch, len)
	  void *dest; register char ch; unsigned len;
#	endif				/* (!)__STDC__ */
	   {	if ( ch == 0 )
			bzero(dest, len);
		else if ( len > 0 )
		   {	register char *p = (char *)dest;
			register unsigned count = len;
			do { *p++ = ch; } while ( --count );
		   }
	   }
#    else				/* !BSD4_2 */
#      ifdef _POSIX_SOURCE
#        include <string.h>
#      else
#        include <memory.h>
#      endif				/* !_POSIX_SOURCE */
#    endif				/* !BSD4_2 */
#  endif				/* !VMS */
#endif					/* !__MSDOS__ */
