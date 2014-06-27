/*
 * options.h - Process options from "treecc" input files.
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

#ifndef	_TREECC_OPTIONS_H
#define	_TREECC_OPTIONS_H

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Return values from "TreeCCOptionProcess".
 */
#define	TREECC_OPT_OK				0
#define	TREECC_OPT_KEEP_VALUE		1
#define	TREECC_OPT_UNKNOWN			2
#define	TREECC_OPT_INVALID_VALUE	3
#define	TREECC_OPT_NEED_VALUE		4
#define	TREECC_OPT_NO_VALUE			5

/*
 * Process an option declaration.
 */
int TreeCCOptionProcess(TreeCCContext *context, char *name, char *value);

#ifdef	__cplusplus
};
#endif

#endif	/* _TREECC_OPTIONS_H */
