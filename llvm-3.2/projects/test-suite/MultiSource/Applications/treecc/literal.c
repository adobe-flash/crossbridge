/*
 * literal.c - Management node types for literal code blocks.
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

void TreeCCAddLiteralDefn(TreeCCContext *context, char *code, int flags)
{
	if(context->debugMode)
	{
		TreeCCDebug(context->input->linenum, "%%literal %d %s",
					flags, code);
	}
	if((flags & TREECC_LITERAL_CODE) != 0 &&
	   (flags & TREECC_LITERAL_DECLS) != 0)
	{
		TreeCCStreamAddLiteral(context->headerStream, code,
							   context->input->filename,
							   context->input->linenum,
							   (flags & TREECC_LITERAL_END) != 0, 0);
		TreeCCStreamAddLiteral(context->sourceStream, code,
							   context->input->filename,
							   context->input->linenum,
							   (flags & TREECC_LITERAL_END) != 0, 1);
	}
	else if((flags & TREECC_LITERAL_CODE) != 0)
	{
		TreeCCStreamAddLiteral(context->sourceStream, code,
							   context->input->filename,
							   context->input->linenum,
							   (flags & TREECC_LITERAL_END) != 0, 0);
	}
	else
	{
		TreeCCStreamAddLiteral(context->headerStream, code,
							   context->input->filename,
							   context->input->linenum,
							   (flags & TREECC_LITERAL_END) != 0, 0);
	}
}

#ifdef	__cplusplus
};
#endif
