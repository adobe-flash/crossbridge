/*
 * parse.h - Parse "treecc" input files.
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

#ifndef	_TREECC_PARSE_H
#define	_TREECC_PARSE_H

#include "system.h"
#include "input.h"
#include "info.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Parse the contents of an input stream and populate "context"
 * with the definitions that are found within it.
 */
void TreeCCParse(TreeCCContext *context);

#ifdef	__cplusplus
};
#endif

#endif	/* _TREECC_PARSE_H */
