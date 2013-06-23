/*
 * stream.h - Stream handling for writing source code.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef	_TREECC_STREAM_H
#define	_TREECC_STREAM_H

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Stream buffer.
 */
#define	TREECC_STREAM_BUFSIZ	2048
typedef struct _tagTreeCCStreamBuf
{
	char	data[TREECC_STREAM_BUFSIZ];
	struct _tagTreeCCStreamBuf *next;

} TreeCCStreamBuf;

/*
 * Stream literal code definition.
 */
typedef struct _tagTreeCCStreamDefn
{
	char *code;
	char *filename;
	long  linenum;
	int   atEnd;
	int   refOnly;
	struct _tagTreeCCStreamDefn *next;

} TreeCCStreamDefn;

/*
 * Structure of a stream.
 */
typedef struct _tagTreeCCStream TreeCCStream;
typedef struct _tagTreeCCContext *PTreeCCContext;
struct _tagTreeCCStream
{
	PTreeCCContext	  context;		/* Context that owns the stream */
	char		     *filename;		/* Name of the file we are writing */
	char		     *embedName;	/* Name of the file to embed in source */
	long			  linenum;		/* Line number we are currently on */
	TreeCCStreamBuf  *firstBuf;		/* First buffer attached to the stream */
	TreeCCStreamBuf  *lastBuf;		/* Last buffer attached to the stream */
	int				  posn;			/* Position within the last buffer */
	int				  forceCreate : 1; /* Force creation of the file */
	int				  readOnly : 1;	/* File is read-only */
	int				  isHeader : 1;	/* File is a header */
	int				  defaultFile : 1; /* File is a discardable default */
	int				  dirty : 1;	/* Something useful has been written */
	TreeCCStreamDefn *firstDefn;	/* First definition for the stream */
	TreeCCStreamDefn *lastDefn;		/* Last definition for the stream */
	TreeCCStream     *nextStream;	/* Next stream associated with context */

};

/*
 * Duplicate a string into the heap.
 */
char *TreeCCDupString(char *str);

/*
 * Resolve a pathname.  The return value is malloc'ed.
 */
char *TreeCCResolvePathname(char *absolute, char *relative);

/*
 * Create an output stream.
 */
TreeCCStream *TreeCCStreamCreate(PTreeCCContext context,
								 char *filename, char *embedName,
								 int isHeader);

/*
 * Get the stream that corresponds to a Java class.
 */
TreeCCStream *TreeCCStreamGetJava(PTreeCCContext context, char *className);

/*
 * Destroy an output stream.
 */
void TreeCCStreamDestroy(TreeCCStream *stream);

/*
 * Clear the contents of a stream.
 */
void TreeCCStreamClear(TreeCCStream *stream);

/*
 * Flush the contents of a stream to the underlying file.
 * Returns zero if an error occurred during writing.
 */
int TreeCCStreamFlush(TreeCCStream *stream);

/*
 * Flush the contents of a stream to a specific stdio file.
 */
int TreeCCStreamFlushStdio(TreeCCStream *stream, FILE *file);

/*
 * If we are using GCC, then make it perform some extra
 * error checking for printf-style formats.
 */
#ifdef __GNUC__
	#define	TREECC_PRNFMT(n,m)	\
     	__attribute__ ((__format__ (__printf__, n, m)))
#else
	#define	TREECC_PRNFMT(n,m)
#endif

/*
 * Print formatted data to a stream.
 */
void TreeCCStreamPrint(TreeCCStream *stream, const char *format, ...)
			TREECC_PRNFMT(2, 3);

/*
 * Output a block of literal code to a stream.
 */
void TreeCCStreamCode(TreeCCStream *stream, char *code);

/*
 * Output a block of literal code to a stream which is indented.
 * The first line is not indented.
 */
void TreeCCStreamCodeIndent(TreeCCStream *stream, char *code, int indent);

/*
 * Output a block of literal code to a stream which is indented.
 * The first line is not indented. This version supports custom indent chars.
 */
void TreeCCStreamCodeIndentCustom(TreeCCStream *stream, char *code, 
								 char indentchar, int indent);

/*
 * Fix the line number information in the output stream
 * after outputting a block of code.
 */
void TreeCCStreamFixLine(TreeCCStream *stream);

/*
 * Add a literal definition block to a stream.
 */
void TreeCCStreamAddLiteral(TreeCCStream *stream, char *code,
							char *filename, long linenum,
							int atEnd, int refOnly);

/*
 * Output extra information that is needed at the top of a header file.
 */
void TreeCCStreamHeaderTop(TreeCCStream *stream);

/*
 * Output extra information that is needed at the bottom of a header file.
 */
void TreeCCStreamHeaderBottom(TreeCCStream *stream);

/*
 * Output extra information that is needed at the top of a source file.
 */
void TreeCCStreamSourceTop(TreeCCStream *stream);

/*
 * Output extra information that is needed at the top of a source file for C#.
 */
void TreeCCStreamSourceTopCS(TreeCCStream *stream);

/*
 * Output extra information that is needed at the bottom of a source file.
 */
void TreeCCStreamSourceBottom(TreeCCStream *stream);

/*
 * Print a "#line" directive to an output stream.
 */
void TreeCCStreamLine(TreeCCStream *stream, long linenum,
					  const char *filename);

#ifdef	__cplusplus
};
#endif

#endif	/* _TREECC_STREAM_H */
