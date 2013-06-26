/*
 * errors.c - Error and message reporting for "treecc".
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
#include "errors.h"

#ifdef	__cplusplus
extern	"C" {
#endif

FILE *TreeCCErrorFile = NULL;
int TreeCCErrorStripPath = 0;

static void ReportError(char *filename, long linenum,
						const char *format, VA_LIST va)
{
	if(!TreeCCErrorFile)
	{
		TreeCCErrorFile = stderr;
	}
	if(filename)
	{
		if(TreeCCErrorStripPath)
		{
			int len = strlen(filename);
			while(len > 0 && filename[len - 1] != '/' &&
				  filename[len - 1] != '\\')
			{
				--len;
			}
			filename += len;
		}
		fputs(filename, TreeCCErrorFile);
		putc(':', TreeCCErrorFile);
	}
	fprintf(TreeCCErrorFile, "%ld: ", linenum);
#ifdef HAVE_VFPRINTF
	vfprintf(TreeCCErrorFile, format, va);
#else
	fputs(format, TreeCCErrorFile);
#endif
	putc('\n', TreeCCErrorFile);
	fflush(TreeCCErrorFile);
}

void TreeCCError(TreeCCInput *input, const char *format, ...)
{
	VA_START;
	ReportError((input ? input->filename : 0),
				(input ? input->linenum : 0), format, VA_GET_LIST);
	VA_END;
	if(input)
	{
		input->errors = 1;
	}
}

void TreeCCErrorOnLine(TreeCCInput *input, char *filename, long linenum,
					   const char *format, ...)
{
	VA_START;
	ReportError(filename, linenum, format, VA_GET_LIST);
	VA_END;
	if(input)
	{
		input->errors = 1;
	}
}

void TreeCCAbort(TreeCCInput *input, const char *format, ...)
{
	VA_START;
	ReportError((input ? input->filename : 0),
				(input ? input->linenum : 0), format, VA_GET_LIST);
	VA_END;
	exit(1);
}

void TreeCCDebug(long linenum, const char *format, ...)
{
	VA_START;
	printf("line %ld: ", linenum);
#ifdef HAVE_VFPRINTF
	vfprintf(stdout, format, VA_GET_LIST);
#else
	fputs(format, stdout);
#endif
	putc('\n', stdout);
	VA_END;
	fflush(stdout);
}

void TreeCCOutOfMemory(TreeCCInput *input)
{
	if(input && input->progname)
	{
		fputs(input->progname, stderr);
		fputs(": ", stderr);
	}
	fputs("virtual memory exhausted\n", stderr);
	exit(1);
}

#ifdef	__cplusplus
};
#endif
