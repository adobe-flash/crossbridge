/*
 * input.h - Process input files for "treecc".
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

#ifndef	_TREECC_INPUT_H
#define	_TREECC_INPUT_H

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Token types.
 */
typedef enum
{
	TREECC_TOKEN_EOF,
	TREECC_TOKEN_IDENTIFIER,
	TREECC_TOKEN_LITERAL_DEFNS,
	TREECC_TOKEN_LITERAL_CODE,
	TREECC_TOKEN_LITERAL_END,
	TREECC_TOKEN_LPAREN,
	TREECC_TOKEN_RPAREN,
	TREECC_TOKEN_LBRACE,
	TREECC_TOKEN_RBRACE,
	TREECC_TOKEN_LSQUARE,
	TREECC_TOKEN_RSQUARE,
	TREECC_TOKEN_COMMA,
	TREECC_TOKEN_EQUALS,
	TREECC_TOKEN_STAR,
	TREECC_TOKEN_REF,
	TREECC_TOKEN_SEMI,
	TREECC_TOKEN_COLON_COLON,
	TREECC_TOKEN_STRING,
	TREECC_TOKEN_UNKNOWN,
	TREECC_TOKEN_NODE,
	TREECC_TOKEN_ABSTRACT,
	TREECC_TOKEN_TYPEDEF,
	TREECC_TOKEN_OPERATION,
	TREECC_TOKEN_NOCREATE,
	TREECC_TOKEN_VIRTUAL,
	TREECC_TOKEN_INLINE,
	TREECC_TOKEN_SPLIT,
	TREECC_TOKEN_OPTION,
	TREECC_TOKEN_HEADER,
	TREECC_TOKEN_OUTPUT,
	TREECC_TOKEN_OUTDIR,
	TREECC_TOKEN_BOTH,
	TREECC_TOKEN_DECLS,
	TREECC_TOKEN_END,
	TREECC_TOKEN_ENUM,
	TREECC_TOKEN_COMMON,
	TREECC_TOKEN_INCLUDE,
	TREECC_TOKEN_READONLY,

} TreeCCToken;

/*
 * Information structure for an input stream.
 */
#define	TREECC_BUFSIZ		1024
typedef struct
{
	TreeCCToken		token;
	char		   *text;
	char		   *progname;
	FILE		   *stream;
	char		   *filename;
	long			linenum;
	long			nextline;
	int				errors;
	int				sawEOF;
	int				parseLiteral;
	int				readOnly;
	char			buffer[TREECC_BUFSIZ];

} TreeCCInput;

/*
 * Open an input stream.
 */
void TreeCCOpen(TreeCCInput *input, char *progname,
				FILE *stream, char *filename);

/*
 * Close an input stream.
 */
void TreeCCClose(TreeCCInput *input, int closeRaw);

/*
 * Get the next token from an input stream.
 * Returns zero if at EOF, or non-zero if OK.
 */
int TreeCCNextToken(TreeCCInput *input);

/*
 * Copy the value of the current text token into a malloc'ed string.
 */
char *TreeCCValue(TreeCCInput *input);

#ifdef	__cplusplus
};
#endif

#endif	/* _TREECC_INPUT_H */
