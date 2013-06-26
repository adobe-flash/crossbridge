/*
 * errors.h - Error and message reporting for "treecc".
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

#ifndef	_TREECC_ERRORS_H
#define	_TREECC_ERRORS_H

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * If we are using GCC, then make it perform some extra
 * error checking for printf-style formats.
 */
#ifdef __GNUC__
	#define	TREECC_ERRFMT(n,m)	\
     	__attribute__ ((__format__ (__printf__, n, m)))
#else
	#define	TREECC_ERRFMT(n,m)
#endif

/*
 * Report an error on the current line of input.
 */
void TreeCCError(TreeCCInput *input, const char *format, ...)
			TREECC_ERRFMT(2, 3);

/*
 * Report an error on a particular line of input.
 */
void TreeCCErrorOnLine(TreeCCInput *input, char *filename, long linenum,
					   const char *format, ...)
			TREECC_ERRFMT(4, 5);

/*
 * Report an error on the current line of input and abort.
 */
void TreeCCAbort(TreeCCInput *input, const char *format, ...)
			TREECC_ERRFMT(2, 3);

/*
 * Print a debugging message.
 */
void TreeCCDebug(long linenum, const char *format, ...)
			TREECC_ERRFMT(2, 3);

/*
 * Abort the program due to out of memory.
 */
void TreeCCOutOfMemory(TreeCCInput *input);

#ifdef	__cplusplus
};
#endif

#endif	/* _TREECC_ERRORS_H */
