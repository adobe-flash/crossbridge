/*
 * input.c - Process input files for "treecc".
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

void TreeCCOpen(TreeCCInput *input, char *progname,
				FILE *stream, char *filename)
{
	input->token = TREECC_TOKEN_IDENTIFIER;
	input->text = 0;
	input->progname = progname;
	input->stream = stream;
	input->filename = filename;
	input->linenum = 1;
	input->nextline = 1;
	input->errors = 0;
	input->sawEOF = 0;
	input->parseLiteral = 1;
	input->readOnly = 0;
}

void TreeCCClose(TreeCCInput *input, int closeRaw)
{
	if(input->text && input->text != input->buffer)
	{
		free(input->text);
	}
	if(closeRaw)
	{
		fclose(input->stream);
	}
}

/*
 * Determine if the next character looks like part of an identifier.
 */
#define	IS_START_IDENT(ch)	(((ch) >= 'A' && (ch) <= 'Z') || \
						 	 ((ch) >= 'a' && (ch) <= 'z') || \
						 	 (ch) == '_')
#define	IS_IDENT(ch)	(((ch) >= 'A' && (ch) <= 'Z') || \
						 ((ch) >= 'a' && (ch) <= 'z') || \
						 ((ch) >= '0' && (ch) <= '9') || \
						 (ch) == '_')

/*
 * Report an invalid character error.
 */
static void InvalidChar(TreeCCInput *input, int ch)
{
	if(ch >= ' ' && ch <= (char)0x7E)
	{
		TreeCCError(input, "invalid `%c' character in input", ch);
	}
	else
	{
		TreeCCError(input, "invalid `\\x%02X' character in input", ch);
	}
}

/*
 * Report EOF inside a literal code block.
 */
static void LiteralEOF(TreeCCInput *input)
{
	TreeCCError(input, "end of file inside literal code block");
}

/*
 * Recognise an identifier from an input stream.
 */
static void RecogIdentifier(TreeCCInput *input, int ch, const char *name)
{
	int len = 0;
	int overflow = 0;
	for(;;)
	{
		/* Add the character to the buffer */
		if(len < (TREECC_BUFSIZ - 1))
		{
			input->buffer[len++] = ch;
		}
		else
		{
			overflow = 1;
		}

		/* Get the next character */
		ch = getc(input->stream);
		if(ch == EOF)
		{
			input->sawEOF = 1;
			break;
		}
		else if(!IS_IDENT(ch))
		{
			ungetc(ch, input->stream);
			break;
		}
	}
	if(overflow)
	{
		TreeCCError(input, "%s is too long", name);
	}
	input->buffer[len] = '\0';
	input->text = input->buffer;
}

/*
 * Flush a buffer of data to "input->text".
 */
static int FlushBuffer(TreeCCInput *input, int len, int currlen)
{
	char *newText;

	/* Ignore the request if "len" is zero */
	if(!len)
	{
		return currlen;
	}

	/* Reallocate the text buffer to the new length */
	if((newText = (char *)realloc(input->text, currlen + len + 1)) == 0)
	{
		TreeCCOutOfMemory(input);
	}
	input->text = newText;

	/* Copy the data to the buffer */
	strncpy(newText + currlen, input->buffer, len);
	newText[currlen + len] = '\0';
	return currlen + len;
}

/*
 * Finalize the text buffer.
 */
static void FinalizeBuffer(TreeCCInput *input, int len, int currlen)
{
	if(currlen != 0)
	{
		FlushBuffer(input, len, currlen);
	}
	else
	{
		input->buffer[len] = '\0';
		input->text = input->buffer;
	}
}

/*
 * Recognise a literal code definition section.
 */
static void LiteralCodeDefn(TreeCCInput *input, int tillEnd)
{
	int len = 0;
	int currlen = 0;
	int ch;

	/* Read characters until EOF or "%}" */
	for(;;)
	{
		ch = getc(input->stream);
		if(ch == EOF)
		{
			input->sawEOF = 1;
			if(!tillEnd)
			{
				LiteralEOF(input);
			}
			break;
		}
		else if(ch == '%' && !tillEnd)
		{
			/* Check for the "%}" terminating sequence */
			ch = getc(input->stream);
			if(ch == '}')
			{
				break;
			}
			else if(ch == EOF)
			{
				input->buffer[len++] = '%';
				input->sawEOF = 1;
				LiteralEOF(input);
				break;
			}
			else
			{
				ungetc(ch, input->stream);
				input->buffer[len++] = '%';
				if(len >= (TREECC_BUFSIZ - 1))
				{
					currlen = FlushBuffer(input, len, currlen);
					len = 0;
				}
			}
		}
		else
		{
			if(ch == '\n')
			{
				/* Unix-style end of line sequence */
				++(input->nextline);
				input->buffer[len++] = '\n';
			}
			else if(ch == '\r')
			{
				/* MS-DOS or Mac-style end of line sequence */
				++(input->nextline);
				input->buffer[len++] = '\n';
				ch = getc(input->stream);
				if(ch == EOF)
				{
					input->sawEOF = 1;
					if(!tillEnd)
					{
						LiteralEOF(input);
					}
					break;
				}
				else if(ch != '\n')
				{
					ungetc(ch, input->stream);
				}
			}
			else if(ch == '\0')
			{
				/* Strip out embedded NUL's */
				InvalidChar(input, ch);
			}
			else
			{
				/* Ordinary character */
				input->buffer[len++] = (char)ch;
			}
			if(len >= (TREECC_BUFSIZ - 1))
			{
				currlen = FlushBuffer(input, len, currlen);
				len = 0;
			}
		}
	}

	/* Finalize the text return buffer */
	FinalizeBuffer(input, len, currlen);
}

/*
 * Recognise a literal code section.  This version is a little
 * harder because we need to find a matching '}', while handling
 * C-style and C++-style comments and strings.
 */
static void LiteralCode(TreeCCInput *input)
{
	unsigned long level = 0;
	int len = 0;
	int currlen = 0;
	int ch, quotech;

	/* Parse input until the next matching '}' */
	for(;;)
	{
		ch = getc(input->stream);
		if(ch == EOF)
		{
			/* Premate end of file within the code block */
			input->sawEOF = 1;
			LiteralEOF(input);
			break;
		}
		else if(ch == '}')
		{
			/* Go out one code level */
			if(level == 0)
			{
				break;
			}
			--level;
			input->buffer[len++] = '}';
		}
		else if(ch == '{')
		{
			/* Go in one code level */
			++level;
			input->buffer[len++] = '{';
		}
		else if(ch == '"' || ch == '\'')
		{
			/* Skip to the end of this string */
			quotech = ch;
			input->buffer[len++] = (char)ch;
			if(len >= (TREECC_BUFSIZ - 1))
			{
				currlen = FlushBuffer(input, len, currlen);
				len = 0;
			}
			while((ch = getc(input->stream)) != EOF && ch != quotech)
			{
				if(ch == '\\')
				{
					input->buffer[len++] = (char)ch;
					if(len >= (TREECC_BUFSIZ - 1))
					{
						currlen = FlushBuffer(input, len, currlen);
						len = 0;
					}
					ch = getc(input->stream);
					if(ch == EOF)
					{
						break;
					}
				}
				if(ch == '\n' || ch == '\r')
				{
					TreeCCError(input, "end of line inside string");
					ungetc(ch, input->stream);
					break;
				}
				input->buffer[len++] = (char)ch;
				if(len >= (TREECC_BUFSIZ - 1))
				{
					currlen = FlushBuffer(input, len, currlen);
					len = 0;
				}
			}
			if(ch == EOF)
			{
				input->sawEOF = 1;
				TreeCCError(input, "end of file inside string");
				break;
			}
			else
			{
				input->buffer[len++] = (char)quotech;
			}
		}
		else if(ch == '/')
		{
			/* May be the start of a comment */
			input->buffer[len++] = '/';
			if(len >= (TREECC_BUFSIZ - 1))
			{
				currlen = FlushBuffer(input, len, currlen);
				len = 0;
			}
			ch = getc(input->stream);
			if(ch == EOF)
			{
				/* EOF in the middle of a code block */
				input->sawEOF = 1;
				LiteralEOF(input);
				break;
			}
			else if(ch == '/')
			{
				/* Single-line comment */
				input->buffer[len++] = '/';
				if(len >= (TREECC_BUFSIZ - 1))
				{
					currlen = FlushBuffer(input, len, currlen);
					len = 0;
				}
				while((ch = getc(input->stream)) != '\n' &&
					  ch != '\r' && ch != EOF)
				{
					if(ch != '\0')
					{
						input->buffer[len++] = (char)ch;
						if(len >= (TREECC_BUFSIZ - 1))
						{
							currlen = FlushBuffer(input, len, currlen);
							len = 0;
						}
					}
					else
					{
						InvalidChar(input, ch);
					}
				}
				if(ch == EOF)
				{
					input->sawEOF = 1;
					LiteralEOF(input);
					break;
				}
				ungetc(ch, input->stream);
			}
			else if(ch == '*')
			{
				/* Multi-line comment */
				input->buffer[len++] = '*';
				if(len >= (TREECC_BUFSIZ - 1))
				{
					currlen = FlushBuffer(input, len, currlen);
					len = 0;
				}
				for(;;)
				{
					ch = getc(input->stream);
					if(ch == EOF)
					{
						/* EOF encountered in a comment */
						input->sawEOF = 1;
						LiteralEOF(input);
						goto finalize;
					}
					else if(ch == '*')
					{
						/* Check for the end of the comment */
						input->buffer[len++] = '*';
						if(len >= (TREECC_BUFSIZ - 1))
						{
							currlen = FlushBuffer(input, len, currlen);
							len = 0;
						}
						ch = getc(input->stream);
						if(ch == '/')
						{
							input->buffer[len++] = '/';
							break;
						}
						else if(ch == EOF)
						{
							input->sawEOF = 1;
							LiteralEOF(input);
							goto finalize;
						}
						else
						{
							ungetc(ch, input->stream);
							continue;
						}
					}
					else if(ch == '\n')
					{
						/* Unix-style end of line in a comment */
						++(input->nextline);
					}
					else if(ch == '\r')
					{
						/* MS-DOS or Mac-style end of line in a comment */
						++(input->nextline);
						ch = getc(input->stream);
						if(ch == EOF)
						{
							input->buffer[len++] = '\n';
							input->sawEOF = 1;
							LiteralEOF(input);
							goto finalize;
						}
						else if(ch != '\n')
						{
							ungetc(ch, input->stream);
						}
						ch = '\n';
					}
					input->buffer[len++] = (char)ch;
					if(len >= (TREECC_BUFSIZ - 1))
					{
						currlen = FlushBuffer(input, len, currlen);
						len = 0;
					}
				}
			}
			else
			{
				/* Simple '/' character */
				ungetc(ch, input->stream);
			}
		}
		else if(ch == '\n')
		{
			/* Unix-like end of line sequence */
			++(input->nextline);
			input->buffer[len++] = '\n';
		}
		else if(ch == '\r')
		{
			/* MS-DOS or Mac-like end of line sequence */
			++(input->nextline);
			input->buffer[len++] = '\n';
			ch = getc(input->stream);
			if(ch == EOF)
			{
				input->sawEOF = 1;
				LiteralEOF(input);
				break;
			}
			else if(ch != '\n')
			{
				ungetc(ch, input->stream);
			}
		}
		else if(ch == '\0')
		{
			/* NUL characters are invalid in literal code blocks */
			InvalidChar(input, ch);
		}
		else
		{
			/* Normal character */
			input->buffer[len++] = (char)ch;
		}
		if(len >= (TREECC_BUFSIZ - 1))
		{
			currlen = FlushBuffer(input, len, currlen);
			len = 0;
		}
	}

	/* Finalize the text return buffer */
finalize:
	FinalizeBuffer(input, len, currlen);
}

/*
 * Recognise a string from an input file.  Escape sequences
 * are not supported in this type of string.
 */
static void RecogString(TreeCCInput *input, int quotech)
{
	int len = 0;
	int currlen = 0;
	int ch;

	/* Parse the contents of the string */
	for(;;)
	{
		ch = getc(input->stream);
		if(ch == quotech)
		{
			/* Terminating quote for the string */
			break;
		}
		else if(ch == EOF)
		{
			/* EOF embedded in the string */
			input->sawEOF = 1;
			TreeCCError(input, "end of file inside string");
			break;
		}
		else if(ch == '\n' || ch == '\r')
		{
			/* End of line embedding in the string */
			ungetc(ch, input->stream);
			TreeCCError(input, "end of line inside string");
			break;
		}
		else if(ch == '\0')
		{
			/* NUL characters are invalid inside a string */
			InvalidChar(input, ch);
		}
		else
		{
			/* Ordinary character */
			input->buffer[len++] = (char)ch;
			if(len >= (TREECC_BUFSIZ - 1))
			{
				currlen = FlushBuffer(input, len, currlen);
				len = 0;
			}
		}
	}

	/* Finalize the text return buffer */
	FinalizeBuffer(input, len, currlen);
}

/*
 * Keyword table.  Must be sorted into ascending order.
 */
static struct
{
	const char *keyword;
	TreeCCToken token;

} const KeywordTable[] = {
	{"abstract",		TREECC_TOKEN_ABSTRACT},
	{"both",			TREECC_TOKEN_BOTH},
	{"common",			TREECC_TOKEN_COMMON},
	{"decls",			TREECC_TOKEN_DECLS},
	{"end",				TREECC_TOKEN_END},
	{"enum",			TREECC_TOKEN_ENUM},
	{"header",			TREECC_TOKEN_HEADER},
	{"include",			TREECC_TOKEN_INCLUDE},
	{"inline",			TREECC_TOKEN_INLINE},
	{"nocreate",		TREECC_TOKEN_NOCREATE},
	{"node",			TREECC_TOKEN_NODE},
	{"operation",		TREECC_TOKEN_OPERATION},
	{"option",			TREECC_TOKEN_OPTION},
	{"outdir",			TREECC_TOKEN_OUTDIR},
	{"output",			TREECC_TOKEN_OUTPUT},
	{"readonly",		TREECC_TOKEN_READONLY},
	{"split",			TREECC_TOKEN_SPLIT},
	{"typedef",			TREECC_TOKEN_TYPEDEF},
	{"virtual",			TREECC_TOKEN_VIRTUAL},
};
#define	KeywordTableSize	(sizeof(KeywordTable) / sizeof(KeywordTable[0]))

int TreeCCNextToken(TreeCCInput *input)
{
	int ch;
	int low, middle, high;

	/* If we have seen EOF already, then bail out now */
	if(input->sawEOF)
	{
		input->linenum = input->nextline;
		input->token = TREECC_TOKEN_EOF;
		return 0;
	}

	/* Free the text input buffer used by the previous token */
	if(input->text && input->text != input->buffer)
	{
		free(input->text);
	}
	input->text = 0;

	/* Determine what kind of token we have from the next character */
	for(;;)
	{
		input->linenum = input->nextline;
		ch = getc(input->stream);
		if(ch == EOF)
		{
			break;
		}
		else if(ch == '%')
		{
			ch = getc(input->stream);
			if(ch == '{')
			{
				/* Start of a literal code definition section that
				   extends until the next occurrence of "%}" */
				LiteralCodeDefn(input, 0);
				input->token = TREECC_TOKEN_LITERAL_DEFNS;
				return 1;
			}
			else if(ch == '%')
			{
				/* Start of a literal code definition section that
				   extends until the end of the file */
				LiteralCodeDefn(input, 1);
				input->token = TREECC_TOKEN_LITERAL_END;
				return 1;
			}
			else if(IS_START_IDENT(ch))
			{
				/* Start of a keyword */
				RecogIdentifier(input, ch, "keyword");
				low = 0;
				high = KeywordTableSize - 1;
				while(low <= high)
				{
					middle = ((low + high) / 2);
					ch = strcmp(input->text, KeywordTable[middle].keyword);
					if(!ch)
					{
						input->token = KeywordTable[middle].token;
						return 1;
					}
					else if(ch < 0)
					{
						high = middle - 1;
					}
					else
					{
						low = middle + 1;
					}
				}
				TreeCCError(input, "unknown keyword `%%%s'", input->text);
				input->token = TREECC_TOKEN_UNKNOWN;
				return 1;
			}
			else
			{
				TreeCCError(input,
							"`%%' must be followed by a keyword, `{', or `%%'");
				if(ch == EOF)
				{
					break;
				}
				else
				{
					ungetc(ch, input->stream);
				}
			}
		}
		else if(IS_START_IDENT(ch))
		{
			/* Start of an identifier */
			RecogIdentifier(input, ch, "identifier");
			input->token = TREECC_TOKEN_IDENTIFIER;
			return 1;
		}
		else if(ch == '(')
		{
			input->token = TREECC_TOKEN_LPAREN;
			return 1;
		}
		else if(ch == ')')
		{
			input->token = TREECC_TOKEN_RPAREN;
			return 1;
		}
		else if(ch == '}')
		{
			input->token = TREECC_TOKEN_RBRACE;
			return 1;
		}
		else if(ch == '[')
		{
			input->token = TREECC_TOKEN_LSQUARE;
			return 1;
		}
		else if(ch == ']')
		{
			input->token = TREECC_TOKEN_RSQUARE;
			return 1;
		}
		else if(ch == ',')
		{
			input->token = TREECC_TOKEN_COMMA;
			return 1;
		}
		else if(ch == '=')
		{
			input->token = TREECC_TOKEN_EQUALS;
			return 1;
		}
		else if(ch == '*')
		{
			input->token = TREECC_TOKEN_STAR;
			return 1;
		}
		else if(ch == '&')
		{
			input->token = TREECC_TOKEN_REF;
			return 1;
		}
		else if(ch == ';')
		{
			input->token = TREECC_TOKEN_SEMI;
			return 1;
		}
		else if(ch == ':')
		{
			ch = getc(input->stream);
			if(ch == ':')
			{
				input->token = TREECC_TOKEN_COLON_COLON;
				return 1;
			}
			else if(ch == EOF)
			{
				InvalidChar(input, ':');
				input->sawEOF = 1;
				input->linenum = input->nextline;
				input->token = TREECC_TOKEN_EOF;
				return 0;
			}
			else
			{
				ungetc(ch, input->stream);
				InvalidChar(input, ':');
			}
		}
		else if(ch == '"' || ch == '\'')
		{
			RecogString(input, ch);
			input->token = TREECC_TOKEN_STRING;
			return 1;
		}
		else if(ch == '\n')
		{
			/* Unix-like end of line sequence */
			++(input->nextline);
		}
		else if(ch == '\r')
		{
			/* MS-DOS or Mac-like end of line sequence */
			ch = getc(input->stream);
			if(ch == EOF)
			{
				input->sawEOF = 1;
			}
			else if(ch != '\n')
			{
				ungetc(ch, input->stream);
			}
			++(input->nextline);
		}
		else if(ch == '{')
		{
			if(input->parseLiteral)
			{
				/* Start of a literal code block */
				LiteralCode(input);
				input->token = TREECC_TOKEN_LITERAL_CODE;
				return 1;
			}
			else
			{
				/* Probably a field definition block */
				input->token = TREECC_TOKEN_LBRACE;
				return 1;
			}
		}
		else if(ch == ' ' || ch == '\t' || ch == '\f' || ch == '\v')
		{
			/* Ignore white space on the current line */
		}
		else if(ch == '/')
		{
			/* May be the start of a comment */
			ch = getc(input->stream);
			if(ch == '*')
			{
				/* Multi-line comment */
				for(;;)
				{
					ch = getc(input->stream);
					if(ch == EOF)
					{
					eofInComment:
						TreeCCError(input, "end of file inside comment");
						input->sawEOF = 1;
						input->linenum = input->nextline;
						input->token = TREECC_TOKEN_EOF;
						return 0;
					}
					else if(ch == '*')
					{
						for(;;)
						{
							ch = getc(input->stream);
							if(ch == EOF)
							{
								goto eofInComment;
							}
							else if(ch == '/')
							{
								goto endComment;
							}
							else if(ch != '*')
							{
								ungetc(ch, input->stream);
								break;
							}
						}
					}
					else if(ch == '\n')
					{
						++(input->nextline);
					}
					else if(ch == '\r')
					{
						++(input->nextline);
						ch = getc(input->stream);
						if(ch == EOF)
						{
							goto eofInComment;
						}
						else if(ch != '\n')
						{
							ungetc(ch, input->stream);
						}
					}
				}
			endComment: ;
			}
			else if(ch == '/')
			{
				/* Single-line comment */
				for(;;)
				{
					ch = getc(input->stream);
					if(ch == EOF)
					{
						input->sawEOF = 1;
						input->linenum = input->nextline;
						input->token = TREECC_TOKEN_EOF;
						return 0;
					}
					else if(ch == '\n')
					{
						break;
					}
					else if(ch == '\r')
					{
						ch = getc(input->stream);
						if(ch == EOF)
						{
							input->sawEOF = 1;
						}
						else if(ch != '\n')
						{
							ungetc(ch, input->stream);
						}
						break;
					}
				}
				++(input->nextline);
			}
			else
			{
				/* Invalid comment sequence */
				InvalidChar(input, '/');
				if(ch == EOF)
				{
					break;
				}
				else
				{
					ungetc(ch, input->stream);
				}
			}
		}
		else
		{
			/* Invalid character in the input stream */
			InvalidChar(input, ch);
		}
	}

	/* If we get here, then we have reached EOF */
	input->linenum = input->nextline;
	input->token = TREECC_TOKEN_EOF;
	input->sawEOF = 1;
	return 0;
}

char *TreeCCValue(TreeCCInput *input)
{
	char *result;
	if(!(input->text))
	{
		/* We don't have a text version of this token */
		TreeCCAbort(input, "no text for token type %d", input->token);
		return 0;
	}
	else if(input->text != input->buffer)
	{
		/* The text is already malloc'ed, so return that */
		result = input->text;
		input->text = 0;
		return result;
	}
	else
	{
		/* Copy the text into a malloc'ed buffer */
		result = (char *)malloc(strlen(input->buffer) + 1);
		if(!result)
		{
			TreeCCOutOfMemory(input);
		}
		strcpy(result, input->buffer);
		input->text = 0;
		return result;
	}
}

#ifdef	__cplusplus
};
#endif
