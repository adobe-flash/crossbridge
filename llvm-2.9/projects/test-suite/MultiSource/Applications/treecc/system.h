/*
 * system.h - Import useful functions from the system libraries.
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

#ifndef	_TREECC_SYSTEM_H
#define	_TREECC_SYSTEM_H

#include "config.h"

/*
 * Standard C headers.
 */
#include <stdio.h>
#if HAVE_STDLIB_H
	#include <stdlib.h>
#endif
#if HAVE_STRING_H
	#include <string.h>
#else
	#if HAVE_STRINGS_H
		#include <strings.h>
	#endif
#endif

/*
 * Handle varargs.
 */
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#define	VA_LIST				va_list
#define	VA_START			va_list va; va_start(va, format)
#define	VA_END				va_end(va)
#define	VA_GET_LIST			va
#else
#ifdef HAVE_VARARGS_H
#include <varargs.h>
#define	VA_LIST				va_list
#define	VA_START			va_list va; va_start(va)
#define	VA_END				va_end(va)
#define	VA_GET_LIST			va
#else
#define	VA_LIST				int
#define	VA_START
#define	VA_END
#define	VA_GET_LIST			0
#endif
#endif

#endif	/* _TREECC_SYSTEM_H */
