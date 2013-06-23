/*
 * stream.c - Stream handling for writing source code.
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

#include "system.h"
#include "input.h"
#include "info.h"
#include "errors.h"

#ifdef	__cplusplus
extern	"C" {
#endif

char *TreeCCDupString(char *str)
{
	char *newstr = (char *)malloc(strlen(str) + 1);
	if(!newstr)
	{
		TreeCCOutOfMemory(0);
	}
	strcpy(newstr, str);
	return newstr;
}

char *TreeCCResolvePathname(char *absolute, char *relative)
{
	int len;
	char *path;
	if(!absolute)
	{
		return TreeCCDupString(relative);
	}
	len = strlen(absolute);
	while(len > 0 && absolute[len - 1] != '/' && absolute[len - 1] != '\\')
	{
		--len;
	}
	if(len <= 0)
	{
		return TreeCCDupString(relative);
	}
	path = (char *)malloc(len + strlen(relative) + 1);
	if(!path)
	{
		TreeCCOutOfMemory(0);
	}
	strncpy(path, absolute, len);
	strcpy(path + len, relative);
	return path;
}

TreeCCStream *TreeCCStreamCreate(PTreeCCContext context,
								 char *filename, char *embedName,
								 int isHeader)
{
	TreeCCStream *stream;
	char *path;

	/* Resolve the filename into a path */
	if(isHeader < 0)
	{
		/* Already resolved by TreeCCStreamGetJava */
		isHeader = 0;
		path = TreeCCDupString(filename);
	}
	else
	{
		path = TreeCCResolvePathname(context->input->filename, filename);
	}

	/* Search for an existing stream for the file */
	stream = context->streamList;
	while(stream != 0)
	{
		if(!strcmp(stream->filename, path))
		{
			free(path);
			return stream;
		}
		stream = stream->nextStream;
	}

	/* Create a new stream */
	stream = (TreeCCStream *)malloc(sizeof(TreeCCStream));
	if(!stream)
	{
		TreeCCOutOfMemory(0);
	}
	stream->context = context;
	stream->filename = path;
	stream->embedName = (embedName ? TreeCCDupString(embedName) :
									 TreeCCDupString(filename));
	stream->linenum = 1;
	stream->firstBuf = 0;
	stream->lastBuf = 0;
	stream->posn = TREECC_STREAM_BUFSIZ;
	stream->forceCreate = context->force;
	stream->readOnly = 0;
	stream->isHeader = isHeader;
	stream->defaultFile = 0;
	stream->dirty = 0;
	stream->firstDefn = 0;
	stream->lastDefn = 0;
	stream->nextStream = context->streamList;
	context->streamList = stream;
	return stream;
}

TreeCCStream *TreeCCStreamGetJava(PTreeCCContext context, char *className)
{
	char *filename;
	int len;
	TreeCCStream *stream;
	if(context->outputDirectory)
	{
		len = strlen(context->outputDirectory) + strlen(className) + 7;
		if((filename = (char *)malloc(len)) == 0)
		{
			TreeCCOutOfMemory(context->input);
		}
		strcpy(filename, context->outputDirectory);
		len = strlen(context->outputDirectory);
		filename[len] = '/';
		strcpy(filename + len + 1, className);
		strcat(filename, ".java");
		++len;
	}
	else
	{
		len = strlen(className) + 6;
		if((filename = (char *)malloc(len)) == 0)
		{
			TreeCCOutOfMemory(context->input);
		}
		strcpy(filename, className);
		strcat(filename, ".java");
		len = 0;
	}
	stream = TreeCCStreamCreate(context, filename, filename + len, -1);
	free(filename);
	return stream;
}

void TreeCCStreamDestroy(TreeCCStream *stream)
{
	TreeCCStreamDefn *defn, *nextDefn;

	/* Free the data buffers for the stream */
	TreeCCStreamClear(stream);

	/* Free the definition list for the stream */
	defn = stream->firstDefn;
	while(defn != 0)
	{
		nextDefn = defn->next;
		if(!(defn->refOnly))
		{
			free(defn->code);
		}
		free(defn);
		defn = nextDefn;
	}

	/* Free the filenames */
	free(stream->filename);
	free(stream->embedName);

	/* Free the stream itself */
	free(stream);
}

void TreeCCStreamClear(TreeCCStream *stream)
{
	TreeCCStreamBuf *buffer, *nextBuffer;
	buffer = stream->firstBuf;
	while(buffer != 0)
	{
		nextBuffer = buffer->next;
		free(buffer);
		buffer = nextBuffer;
	}
	stream->firstBuf = 0;
	stream->lastBuf = 0;
	stream->dirty = 0;
	stream->posn = TREECC_STREAM_BUFSIZ;
	stream->linenum = 1;
}

/*
 * Define the "MemCmp" function.
 */
#if HAVE_MEMCMP
#define	MemCmp(s1,s2,size)	(memcmp((s1),(s2),(size)))
#else
static int MemCmp(const char *s1, const char *s2, int size)
{
	while(size > 0)
	{
		if(*s1 < *s2)
		{
			return -1;
		}
		else if(*s1 > *s2)
		{
			return 1;
		}
		++s1;
		++s2;
		--size;
	}
	return 0;
}
#endif

int TreeCCStreamFlush(TreeCCStream *stream)
{
	int result;
	FILE *file;
	TreeCCStreamBuf *buffer;
	char tempbuf[TREECC_STREAM_BUFSIZ];
	int size, changed;

	/* Ignore default streams with no contents */
	if(stream->defaultFile && !(stream->firstBuf))
	{
		return 1;
	}

	/* Validate that the contents have changed non-trivially */
	if(!(stream->forceCreate) || stream->readOnly)
	{
		if((file = fopen(stream->filename, "r")) != NULL)
		{
			buffer = stream->firstBuf;
			changed = 0;
			while((size = fread(tempbuf, 1, TREECC_STREAM_BUFSIZ, file)) != 0)
			{
				if(!buffer)
				{
					changed = 1;
					break;
				}
				if(buffer == stream->lastBuf)
				{
					if(stream->posn != size)
					{
						changed = 1;
						break;
					}
					if(MemCmp(buffer->data, tempbuf, size) != 0)
					{
						changed = 1;
						break;
					}
				}
				else
				{
					if(size != TREECC_STREAM_BUFSIZ)
					{
						changed = 1;
						break;
					}
					if(MemCmp(buffer->data, tempbuf, TREECC_STREAM_BUFSIZ) != 0)
					{
						changed = 1;
						break;
					}
				}
				buffer = buffer->next;
				if(size < TREECC_STREAM_BUFSIZ)
				{
					size = 0;
					break;
				}
			}
			if(size == 0 && buffer != 0)
			{
				changed = 1;
			}
			fclose(file);
			if(!changed)
			{
				return 1;
			}
		}
		if(stream->readOnly)
		{
			fprintf(stderr, "%s: read-only file has different contents\n",
					stream->filename);
			return 0;
		}
	}

	/* Open the output file */
	if((file = fopen(stream->filename, "w")) == NULL)
	{
		perror(stream->filename);
		return 0;
	}

	/* Flush the data to the output file */
	result = TreeCCStreamFlushStdio(stream, file);

	/* Close the output file */
	fclose(file);
	return result;
}

int TreeCCStreamFlushStdio(TreeCCStream *stream, FILE *file)
{
	TreeCCStreamBuf *buffer = stream->firstBuf;
	while(buffer != 0)
	{
		if(buffer == stream->lastBuf)
		{
			if(fwrite(buffer->data, 1, stream->posn, file) != stream->posn)
			{
				return 0;
			}
		}
		else
		{
			if(fwrite(buffer->data, 1, TREECC_STREAM_BUFSIZ, file) !=
					TREECC_STREAM_BUFSIZ)
			{
				return 0;
			}
		}
		buffer = buffer->next;
	}
	return (fflush(file) == 0);
}

/*
 * Write the contents of a buffer to a stream.
 */
static void WriteBuffer(TreeCCStream *stream, const char *buf)
{
	int len = strlen(buf);
	int templen;
	TreeCCStreamBuf *buffer;
	stream->dirty = 1;
	while(len > 0)
	{
		/* Add another buffer to the stream if necessary */
		if(stream->posn >= TREECC_STREAM_BUFSIZ)
		{
			buffer = (TreeCCStreamBuf *)malloc(sizeof(TreeCCStreamBuf));
			if(!buffer)
			{
				TreeCCOutOfMemory(0);
			}
			buffer->next = 0;
			if(stream->lastBuf)
			{
				stream->lastBuf->next = buffer;
			}
			else
			{
				stream->firstBuf = buffer;
			}
			stream->lastBuf = buffer;
			stream->posn = 0;
			templen = TREECC_STREAM_BUFSIZ;
		}
		else
		{
			buffer = stream->lastBuf;
			templen = TREECC_STREAM_BUFSIZ - stream->posn;
		}

		/* Copy the data to the stream buffer */
		if(templen > len)
		{
			templen = len;
		}
	#if HAVE_MEMCPY
		memcpy(buffer->data + stream->posn, buf, templen);
	#else
		{
			char *s1 = buffer->data + stream->posn;
			const char *s2 = buf;
			int size = templen;
			while(size > 0)
			{
				*s1++ = *s2++;
				--size;
			}
		}
	#endif

		/* Advance to the next part of the buffer to be written */
		buf += templen;
		len -= templen;
		stream->posn += templen;
	}
}

/*
 * Update the line number position of a stream.
 */
static void UpdateLineNum(TreeCCStream *stream, const char *buf)
{
#if HAVE_STRCHR
	while((buf = strchr(buf, '\n')) != 0)
	{
		++buf;
		++(stream->linenum);
	}
#else
	while(*buf != '\0')
	{
		if(*buf == '\n')
		{
			++(stream->linenum);
		}
		++buf;
	}
#endif
}

void TreeCCStreamPrint(TreeCCStream *stream, const char *format, ...)
{
	char tempbuf[4096];

	/* Print the formatted data to tempbuf */
	VA_START;
#if HAVE_VSNPRINTF
	vsnprintf(tempbuf, sizeof(tempbuf), format, VA_GET_LIST);
#elif HAVE__VSNPRINTF
	_vsnprintf(tempbuf, sizeof(tempbuf), format, VA_GET_LIST);
#else
	vsprintf(tempbuf, format, VA_GET_LIST);
#endif
	VA_END;

	/* Write the contents of "tempbuf" to the stream */
	WriteBuffer(stream, tempbuf);

	/* Count newlines in the buffer to update the line number */
	UpdateLineNum(stream, tempbuf);
}

void TreeCCStreamCode(TreeCCStream *stream, char *code)
{
	WriteBuffer(stream, code);
	UpdateLineNum(stream, code);
}

/*
 * Put a single character to a stream buffer.
 */
static void _StreamPut(int ch, TreeCCStream *stream)
{
	char buf[2];
	buf[0] = (char)ch;
	buf[1] = '\0';
	WriteBuffer(stream, buf);
}
#define	StreamPut(ch,stream)	\
	do { \
		if((stream)->posn < TREECC_STREAM_BUFSIZ) \
		{ \
			(stream)->lastBuf->data[((stream)->posn)++] = (ch); \
			(stream)->dirty = 1; \
		} \
		else \
		{ \
			_StreamPut((ch), (stream)); \
		} \
	} while (0)

void TreeCCStreamCodeIndent(TreeCCStream *stream, char *code, int indent)
{
	int temp;
	while(*code != '\0')
	{
		StreamPut(*code, stream);
		if(*code == '\n')
		{
			++(stream->linenum);
			for(temp = 0; temp < indent; ++temp)
			{
				StreamPut('\t', stream);
			}
		}
		++code;
	}
}

void TreeCCStreamCodeIndentCustom(TreeCCStream *stream, char *code, 
								  char indentchar, int indent)
{
	int temp;
	while(*code != '\0')
	{
		StreamPut(*code, stream);
		if(*code == '\n')
		{
			++(stream->linenum);
			for(temp = 0; temp < indent; ++temp)
			{
				StreamPut(indentchar, stream);
			}
		}
		++code;
	}
}


void TreeCCStreamFixLine(TreeCCStream *stream)
{
	TreeCCStreamLine(stream, stream->linenum + 1, stream->embedName);
}

void TreeCCStreamAddLiteral(TreeCCStream *stream, char *code,
							char *filename, long linenum,
							int atEnd, int refOnly)
{
	TreeCCStreamDefn *defn;

	/* Bail out if the stream is NULL (can happen in "test_parse" sometimes) */
	if(!stream)
	{
		return;
	}

	/* Allocate space for a definition block */
	defn = (TreeCCStreamDefn *)malloc(sizeof(TreeCCStreamDefn));
	if(!defn)
	{
		TreeCCOutOfMemory(0);
	}

	/* Initialize the definition block */
	defn->code = code;
	defn->filename = filename;
	defn->linenum = linenum;
	defn->atEnd = atEnd;
	defn->refOnly = refOnly;
	defn->next = 0;

	/* Add the definition block to the stream's definition list */
	if(stream->lastDefn)
	{
		stream->lastDefn->next = defn;
	}
	else
	{
		stream->firstDefn = defn;
	}
	stream->lastDefn = defn;
}

/*
 * Output a macro name that has been generated from a filename.
 */
static void OutputMacroName(TreeCCStream *stream, const char *filename)
{
	while(*filename != '\0')
	{
		if((*filename >= 'A' && *filename <= 'Z') ||
		   (*filename >= 'a' && *filename <= 'z') ||
		   (*filename >= '0' && *filename <= '9'))
		{
			StreamPut(*filename, stream);
		}
		else
		{
			StreamPut('_', stream);
		}
		++filename;
	}
	StreamPut('\n', stream);
	++(stream->linenum);
}

/*
 * Output a list of definitions to a header or source stream.
 */
static void OutputDefns(TreeCCStream *stream, int atEnd)
{
	TreeCCStreamDefn *defn = stream->firstDefn;
	int sawDefn = 0;
	while(defn != 0)
	{
		if(defn->atEnd == atEnd)
		{
			TreeCCStreamLine(stream, defn->linenum, defn->filename);
			WriteBuffer(stream, defn->code);
			UpdateLineNum(stream, defn->code);
			if(*(defn->code) != '\0' &&
			   defn->code[strlen(defn->code) - 1] != '\n')
			{
				/* Terminate the final line */
				StreamPut('\n', stream);
				++(stream->linenum);
			}
			sawDefn = 1;
		}
		defn = defn->next;
	}
	if(sawDefn)
	{
		TreeCCStreamFixLine(stream);
	}
}

void TreeCCStreamHeaderTop(TreeCCStream *stream)
{
	char *filename = stream->embedName;
	TreeCCStreamPrint(stream,
			"/* %s.  Generated automatically by treecc */\n", filename);
	TreeCCStreamPrint(stream, "#ifndef __%s_", stream->context->yy_replacement);
	OutputMacroName(stream, filename);
	TreeCCStreamPrint(stream, "#define __%s_", stream->context->yy_replacement);
	OutputMacroName(stream, filename);
	OutputDefns(stream, 0);
}

void TreeCCStreamHeaderBottom(TreeCCStream *stream)
{
	OutputDefns(stream, 1);
	TreeCCStreamPrint(stream, "#endif\n");
}

void TreeCCStreamSourceTop(TreeCCStream *stream)
{
	TreeCCStreamPrint(stream, "/* %s.  Generated automatically by treecc */\n",
					  stream->embedName);
	OutputDefns(stream, 0);
}

void TreeCCStreamSourceTopCS(TreeCCStream *stream)
{
	OutputDefns(stream, 0);
}

void TreeCCStreamSourceBottom(TreeCCStream *stream)
{
	OutputDefns(stream, 1);
}

void TreeCCStreamLine(TreeCCStream *stream, long linenum,
					  const char *filename)
{
	if(stream->context->print_lines)
	{
		int len;
		if(stream->context->strip_filenames)
		{
			len = strlen(filename);
			while(len > 0 && filename[len - 1] != '/' &&
				  filename[len - 1] != '\\')
			{
				--len;
			}
			filename += len;
		}
		TreeCCStreamPrint(stream, "#line %ld \"%s\"\n", linenum, filename);
	}
}

#ifdef	__cplusplus
};
#endif
