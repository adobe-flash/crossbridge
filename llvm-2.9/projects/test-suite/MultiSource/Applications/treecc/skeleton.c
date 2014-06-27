/*
 * skeleton.c - Include skeleton code in an output stream.
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

/*
 * Find a particular skeleton string within "skels.c".
 */
extern const char * const TreeCCSkelFiles[];
static char *FindSkeletonString(const char *skeleton)
{
	char **search = (char **)TreeCCSkelFiles;
	while(*search != 0)
	{
		if(!strcmp(*search, skeleton))
		{
			return search[1];
		}
		search += 2;
	}
	return 0;
}

/*
 * Read a line from a skeleton buffer..
 */
static int ReadSkeletonLine(char *buffer, int size, char **skel)
{
	char *ptr = *skel;
	if(*ptr == '\0')
	{
		return 0;
	}
	while(*ptr != '\0' && *ptr != '\n')
	{
		if(size > 2)
		{
			*buffer++ = *ptr;
			--size;
		}
		++ptr;
	}
	if(*ptr == '\n')
	{
		*buffer++ = *ptr++;
	}
	*buffer = '\0';
	*skel = ptr;
	return 1;
}

void TreeCCIncludeSkeleton(TreeCCContext *context, TreeCCStream *stream,
						   const char *skeleton)
{
	char *skelstr = FindSkeletonString(skeleton);
	char buffer[BUFSIZ];
	int posn, start;
	if(!skelstr)
	{
		fprintf(stderr,
				"treecc: internal error - could not find skeleton \"%s\"\n",
				skeleton);
		exit(1);
	}
	TreeCCStreamLine(stream, 1, skeleton);
	while(ReadSkeletonLine(buffer, BUFSIZ, &skelstr))
	{
	#if HAVE_STRCHR
		if(strchr(buffer, 'Y') != 0 || strchr(buffer, 'y') != 0)
	#endif
		{
			/* The line probably contains "YYNODESTATE" or "yy" */
			posn = 0;
			start = 0;
			while(buffer[posn] != '\0')
			{
				if(buffer[posn] == 'Y' &&
				   !strncmp(buffer + posn, "YYNODESTATE", 11))
				{
					buffer[posn] = '\0';
					if(start < posn)
					{
						TreeCCStreamCode(stream, buffer + start);
					}
					TreeCCStreamCode(stream, context->state_type);
					posn += 11;
					start = posn;
				}
				else if(buffer[posn] == 'y' && buffer[posn + 1] == 'y')
				{
					buffer[posn] = '\0';
					if(start < posn)
					{
						TreeCCStreamCode(stream, buffer + start);
					}
					TreeCCStreamCode(stream, context->yy_replacement);
					posn += 2;
					start = posn;
				}
				else
				{
					++posn;
				}
			}
			if(start < posn)
			{
				TreeCCStreamCode(stream, buffer + start);
			}
		}
	#if HAVE_STRCHR
		else
		{
			/* Ordinary line */
			TreeCCStreamCode(stream, buffer);
		}
	#endif
	}
	TreeCCStreamFixLine(stream);
}

#ifdef	__cplusplus
};
#endif
