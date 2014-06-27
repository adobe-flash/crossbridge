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

/* stream.h */
/* Definitions for standard stream package for GhostScript */
/* Requires stdio.h */

/* Note that the stream package works with bytes, not chars. */
/* This is to ensure unsigned representation on all systems. */
/* A stream can only be read or written, not both. */
/* Note also that the read procedure returns an int, */
/* not a char or a byte, because EOFC is -1. */
typedef struct stream_s stream;
typedef struct {
	int (*read)(P1(stream *));
	int (*write)(P2(stream *, byte));
	int (*available)(P2(stream *, long *));
	int (*seek)(P2(stream *, long));
	int (*flush)(P1(stream *));
	int (*close)(P1(stream *));
} stream_procs;
struct stream_s {
	byte *cptr;			/* pointer to last byte */
					/* read or written */
	byte *endptr;			/* pointer to last byte */
					/* containing data for reading, */
					/* or to be filled for writing */
	byte *cbuf;			/* base of buffer */
	uint bsize;			/* size of buffer */
	char writing;			/* 0 if reading, 1 if writing */
	char eof;			/* non-zero if at EOF when buffer */
					/* becomes empty */
	long position;			/* file position of beginning of */
					/* buffer, -1 means not seekable */
	stream_procs procs;
	int num_format;			/* format for Level 2 */
					/* encoded number reader */
					/* (only used locally) */
	/*
	 * If were were able to program in a real object-oriented style, 
	 * the remaining data would be per-subclass.  It's just too much
	 * of a nuisance to do this in C, so we allocate space for the
	 * private data of ALL subclasses.
	 */
	/* The following is for file streams. */
	FILE *file;			/* file handle for C library */
	/* The following are for decrypting streams. */
	stream *strm;
	ushort cstate;			/* encryption state */
	int odd;			/* odd hex digit */
};

/* Stream functions.  Some of these are macros -- beware. */
/* Note that there is no eof test -- instead, do a sgetc, */
/* compare against EOFC, and then do sputback if not at eof. */
/* Also note that unlike the C stream library, */
/* ALL stream procedures take the stream as the first argument. */

/* Following are valid for all streams. */
/* flush is a no-op for read streams. */
/* close is a no-op for non-file streams. */
#define sseekable(s) ((s)->position >= 0)
#define savailable(s,pl) (*(s)->procs.available)(s,pl)
#define sflush(s) (*(s)->procs.flush)(s)
#define sclose(s) (*(s)->procs.close)(s)

/* Following are only valid for read streams. */
#define sgetc(s)\
  ((s)->cptr < (s)->endptr ? *++((s)->cptr) : (*(s)->procs.read)(s))
extern uint sgets(P3(stream *, byte *, uint));
extern int sreadhex(P5(stream *, byte *, uint, uint *, int *));
extern int sungetc(P2(stream *, byte));	/* -1 on error, 0 if OK */
#define sputback(s) ((s)->cptr--)

/* Following are only valid for write streams. */
#define sputc(s,c)\
  ((s)->cptr < (s)->endptr ? ((int)(*++((s)->cptr)=(c))) :\
   (*(s)->procs.write)((s),(c)))
extern uint sputs(P3(stream *, byte *, uint));

/* Following are only valid for positionable streams. */
#define stell(s) ((s)->cptr + 1 - (s)->cbuf + (s)->position)
#define sseek(s,pos) (*(s)->procs.seek)(s,(long)(pos))

/* Following are for high-performance clients. */
/* bufptr points to the next item, bufend points beyond the last item. */
#define sbufptr(s) ((s)->cptr + 1)
#define sbufend(s) ((s)->endptr + 1)
#define ssetbufptr(s,ptr) ((s)->cptr = (ptr) - 1)
#define sbufavailable(s) ((s)->endptr - (s)->cptr)

#define EOFC (-1)

/* Stream creation procedures */
extern	void	sread_string(P3(stream *, byte *, uint)),
		swrite_string(P3(stream *, byte *, uint));
extern	void	sread_file(P4(stream *, FILE *, byte *, uint)),
		swrite_file(P4(stream *, FILE *, byte *, uint));
extern	void	sread_decrypt(P5(stream *, stream *, byte *, uint, ushort));
