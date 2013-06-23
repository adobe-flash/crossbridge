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

/* stream.c */
/* Stream package for GhostScript interpreter */
#include <stdio.h>
#include "memory_.h"
#include "std.h"
#include "stream.h"
#include "scanchar.h"
#include "gxfixed.h"			/* for gstype1.h */
#include "gstype1.h"

/* Forward declarations */
	/* Generic */
private int
  snull(P1(stream *)),
  snoavailable(P2(stream *, long *));
	/* Strings */
private int
  ssread(P1(stream *)),
  sswrite(P2(stream *, byte)),
  ssavailable(P2(stream *, long *)),
  ssseek(P2(stream *, long));
private void
  ssinit(P4(stream *, byte *, uint, stream_procs *));
	/* Files */
private int
  sfread(P1(stream *)),
  sfavailable(P2(stream *, long *)),
  srseek(P2(stream *, long)),
  srclose(P1(stream *));
private int
  sfwrite(P2(stream *, byte)),
  swseek(P2(stream *, long)),
  swflush(P1(stream *)),
  swclose(P1(stream *));
private void
  sfinit(P5(stream *, FILE *, byte *, uint, stream_procs *));
	/* decrypting */
private int
  sxread(P1(stream *)),
  sxavailable(P2(stream *, long *));

/* ------ String streams ------ */

/* Initialize a stream for reading a string. */
void
sread_string(register stream *s, byte *ptr, uint len)
{	static stream_procs p =
		{ ssread, NULL, ssavailable, ssseek, snull, snull };
	ssinit(s, ptr, len, &p);
	s->writing = 0;
}
/* Handle end-of-buffer when reading from a string. */
private int
ssread(stream *s)
{	s->cptr = s->endptr;
	return EOFC;
}
/* Return the number of available bytes when reading from a string. */
private int
ssavailable(stream *s, long *pl)
{	*pl = sbufavailable(s);
	if ( *pl == 0 ) *pl = -1;	/* EOF */
	return 0;
}

/* Initialize a stream for writing a string. */
void
swrite_string(register stream *s, byte *ptr, uint len)
{	static stream_procs p =
		{ NULL, sswrite, snoavailable, ssseek, snull, snull };
	ssinit(s, ptr, len, &p);
	s->writing = 1;
}
/* Handle end-of-buffer when writing a string. */
private int
sswrite(stream *s, byte c)
{	s->cptr = s->endptr;
	return EOFC;
}

/* Seek in a string.  Return 0 if OK, -1 if not. */
private int
ssseek(register stream *s, long pos)
{	if ( pos < 0 || pos > s->bsize ) return -1;
	s->cptr = s->cbuf + pos - 1;
	return 0;
}

/* Private initialization */
private void
ssinit(register stream *s, byte *ptr, uint len, stream_procs *p)
{	s->cbuf = ptr;
	s->cptr = ptr - 1;
	s->endptr = s->cptr + len;
	s->bsize = len;
	s->eof = 1;			/* this is all there is */
	s->position = 0;
	s->procs = *p;
}

/* ------ File streams ------ */

/* Initialize a stream for reading an OS file. */
void
sread_file(register stream *s, FILE *file, byte *buf, uint len)
{	static stream_procs p =
		{ sfread, NULL, sfavailable, srseek, snull, srclose };
	sfinit(s, file, buf, len, &p);
	s->writing = 0;
	s->position = (file == stdin ? -1 : 0);
}
/* Procedures for reading from a file */
private int
sfread(register stream *s)
{	int nread;
	if ( s->eof )
	   {	s->cptr = s->endptr;
		return EOFC;
	   }
	if ( s->position >= 0 )		/* file is positionable */
		s->position = ftell(s->file);
	nread = fread(s->cbuf, 1, s->bsize, s->file);
	s->cptr = s->cbuf - 1;
	s->eof = feof(s->file);
	if ( nread > 0 )
	   {	s->endptr = s->cptr + nread;
		return (int)*++(s->cptr);	/* don't understand why the cast is needed.... */
	   }
	else if ( nread == 0 )
	   {	s->endptr = s->cptr;
		s->eof = 1;
		return EOFC;
	   }
	else				/* error, now what?? */
	   {	s->endptr = s->cptr;
		return 0;
	   }
   }
private int
sfavailable(register stream *s, long *pl)
{	*pl = sbufavailable(s);
	if ( sseekable(s) )
	   {	long pos, end;
		pos = ftell(s->file);
		if ( fseek(s->file, 0L, 2) ) return -1;
		end = ftell(s->file);
		if ( fseek(s->file, pos, 0) ) return -1;
		*pl += end - pos;
		if ( *pl == 0 ) *pl = -1;	/* EOF */
	   }
	else
	   {	if ( *pl == 0 && feof(s->file) ) *pl = -1;	/* EOF */
	   }
	return 0;
}
private int
srseek(register stream *s, long pos)
{	uint end = s->endptr - s->cbuf + 1;
	long offset = pos - s->position;
	if ( offset >= 0 && offset <= end )
	   {	/* Staying within the same buffer */
		s->cptr = s->cbuf + offset - 1;
		return 0;
	   }
	if ( fseek(s->file, pos, 0) != 0 )
		return -1;
	s->endptr = s->cptr = s->cbuf - 1;
	s->eof = 0;
	return 0;
}
private int
srclose(stream *s)
{	return fclose(s->file);
}

/* Initialize a stream for writing an OS file. */
void
swrite_file(register stream *s, FILE *file, byte *buf, uint len)
{	static stream_procs p =
		{ NULL, sfwrite, snoavailable, swseek, swflush, swclose };
	sfinit(s, file, buf, len, &p);
	s->writing = 1;
	s->position = (file == stdout || file == stderr ? -1 : 0);
   }
/* Procedures for writing on a file */
#define fwrite_buf(s)\
  fwrite(s->cbuf, 1, (uint)(s->cptr + 1 - s->cbuf), s->file)
private int
sfwrite(register stream *s, byte c)
{	fwrite_buf(s);
	if ( s->position >= 0 )		/* file is positionable */
		s->position = ftell(s->file);
	s->cptr = s->cbuf - 1;
	s->endptr = s->cptr + s->bsize;
	return sputc(s, c);
}
private int
swseek(stream *s, long pos)
{	/* Output files are not positionable */
	return -1;
}
private int
swflush(register stream *s)
{	int result = fwrite_buf(s);
	fflush(s->file);
	s->position = ftell(s->file);
	s->cptr = s->cbuf - 1;
	s->endptr = s->cptr + s->bsize;
	return result;
}
private int
swclose(register stream *s)
{	fwrite_buf(s);
	return fclose(s->file);
}

/* Private initialization */
private void
sfinit(register stream *s, FILE *file, byte *buf, uint len, stream_procs *p)
{	s->cbuf = buf;
	s->cptr = buf - 1;
	s->bsize = len;
	s->file = file;
	s->endptr = s->cptr;
	s->eof = 0;
	s->procs = *p;
}

/* ------ Encrypted streams ------ */

/* Initialize a stream for reading and decrypting another stream. */
/* Decrypting streams are not positionable. */
void
sread_decrypt(register stream *s, stream *xs, byte *buf, uint len,
  ushort /*crype_state*/ state)
{	static stream_procs p =
		{ sxread, NULL, sxavailable, NULL, snull, snull };
	sfinit(s, (FILE *)NULL, buf, len, &p);
	s->writing = 0;
	s->position = -1;		/* not positionable */
	s->strm = xs;
	s->cstate = state;
	s->odd = -1;
}
/* Refill the buffer of a decrypting stream. */
private int
sxread(register stream *s)
{	byte *buf = s->cbuf;
	uint nread;
	s->cptr = buf - 1;
top:	nread = sgets(s->strm, buf, s->bsize);
	if ( nread == 0 )		/* end of stream */
	   {	s->endptr = s->cptr;
		s->eof = 1;
		return EOFC;
	   }
	else
	   {	/* Decrypt the buffer.  The buffer consists of information */
		/* in the form suitable for readhexstring. */
		stream sst;
		sread_string(&sst, buf, nread);
		sreadhex(&sst, buf, nread, &nread, &s->odd);
		if ( nread == 0 ) goto top;	/* try again */
		gs_type1_decrypt(buf, buf, nread, (crypt_state *)&s->cstate);
		s->endptr = s->cptr + nread;
		return (int)*++(s->cptr);	/* don't understand why the cast is needed.... */
	   }
}
/* Estimate the number of remaining bytes in a decrypting stream. */
private int
sxavailable(stream *s, long *pl)
{	if ( savailable(s->strm, pl) < 0 ) return -1;
	if ( *pl >= 0 ) *pl /= 2;
	return 0;
}

/* ------ Generic procedures ------ */

/* Implement a stream procedure as a no-op. */
private int
snull(stream *s)
{	return 0;
}

/* Indicate an error when asked for available input bytes. */
private int
snoavailable(stream *s, long *pl)
{	return -1;
}

/* Push back a character onto a (read) stream. */
/* Return 0 on success, -1 on failure. */
int
sungetc(register stream *s, byte c)
{	if ( s->writing || s->cptr < s->cbuf ) return -1;
	*(s->cptr)-- = c;
	return 0;
}

/* Read a string from a stream. */
/* Return the number of bytes read. */
uint
sgets(register stream *s, byte *str, uint rlen)
{	uint len = rlen;
	while ( len > 0 )
	   {	uint count = sbufavailable(s);
		if ( count > 0 )
		   {	if ( count > len ) count = len;
			memcpy(str, s->cptr + 1, count);
			s->cptr += count;
			str += count;
			len -= count;
		   }
		else
		   {	int ch = sgetc(s);
			if ( s->eof ) return rlen - len;
			*str++ = ch;
			len--;
		   }
	   }
	return rlen;
}

/* Write a string on a stream. */
/* Return the number of bytes written. */
uint
sputs(register stream *s, byte *str, uint wlen)
{	uint len = wlen;
	while ( len > 0 )
	   {	uint count = sbufavailable(s);
		if ( count > 0 )
		   {	if ( count > len ) count = len;
			memcpy(s->cptr + 1, str, count);
			s->cptr += count;
			str += count;
			len -= count;
		   }
		else
		   {	byte ch = *str++;
			sputc(s, ch);
			if ( s->eof ) return wlen - len;
			len--;
		   }
	   }
	return wlen;
}

/* Read a hex string from a stream. */
/* Skip all characters other than hex digits. */
/* Answer 1 if we reached end-of-file before filling the string, */
/* 0 if we filled the string first, or <0 on error. */
/* *odd_digit should be -1 initially: */
/* if an odd number of hex digits was read, *odd_digit is set to */
/* the odd digit value, otherwise *odd_digit is set to -1. */
int
sreadhex(register stream *s, byte *str, uint rlen, uint *nread,
  int *odd_digit)
{	byte *ptr = str;
	byte *limit = ptr + rlen;
	byte val1 = (byte)*odd_digit;
	byte val2;
#ifdef __MSDOS__
/* MS-DOS can't put pointers in registers.... */
#	define decoder scan_char_decoder
#else
	register byte *decoder = scan_char_decoder;
#endif
	if ( rlen == 0 )
	   {	*nread = 0;
		return 0;
	   }
	if ( val1 <= 0xf ) goto d2;
d1:	while ( (val1 = decoder[sgetc(s)]) > 0xf )
	   {	if ( val1 == ctype_eof ) { *odd_digit = -1; goto ended; }
	   }
d2:	while ( (val2 = decoder[sgetc(s)]) > 0xf )
	   {	if ( val2 == ctype_eof ) { *odd_digit = val1; goto ended; }
	   }
	*ptr++ = (val1 << 4) + val2;
	if ( ptr < limit ) goto d1;
	*nread = rlen;
	return 0;
ended:	*nread = ptr - str;
	return 1;
}
