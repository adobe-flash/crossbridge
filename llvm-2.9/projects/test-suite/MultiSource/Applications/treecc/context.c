/*
 * context.c - Create or destroy "treecc" parsing contexts.
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

TreeCCContext *TreeCCContextCreate(TreeCCInput *input)
{
	TreeCCContext *context;

	/* Allocate space for the context */
	if((context = (TreeCCContext *)calloc(1, sizeof(TreeCCContext))) == 0)
	{
		TreeCCOutOfMemory(input);
	}

	/* Initialize and return the context */
	context->input = input;
	context->debugMode = 0;
	context->track_lines = 1;
	context->no_singletons = 0;
	context->reentrant = 0;
	context->force = 0;
	context->virtual_factory = 0;
	context->abstract_factory = 0;
	context->kind_in_vtable = 0;
	context->strip_filenames = 0;
	context->print_lines = 1;
	context->internal_access = 0;
	context->use_allocator = 1;
	context->use_gc_allocator = 0;
	context->yy_replacement = "yy";
	context->state_type = "YYNODESTATE";
	context->namespace = 0;
	context->language = TREECC_LANG_C;
	context->block_size = 0;
	context->nodeNumber = 1;
	context->baseType = 0;
	return context;
}

void TreeCCContextDestroy(TreeCCContext *context)
{
	unsigned int hash;
	TreeCCNode *node, *nextNode;
	TreeCCOperation *oper, *nextOper;
	TreeCCStream *stream, *nextStream;

	/* Close the source streams */
	stream = context->streamList;
	while(stream != 0)
	{
		nextStream = stream->nextStream;
		TreeCCStreamDestroy(stream);
		stream = nextStream;
	}

	/* Free the contents of the node hash */
	for(hash = 0; hash < TREECC_HASH_SIZE; ++hash)
	{
		node = context->nodeHash[hash];
		while(node != 0)
		{
			nextNode = node->nextHash;
			TreeCCNodeFree(node);
			node = nextNode;
		}
	}

	/* Free the contents of the operation hash */
	for(hash = 0; hash < TREECC_HASH_SIZE; ++hash)
	{
		oper = context->operHash[hash];
		while(oper != 0)
		{
			nextOper = oper->nextHash;
			TreeCCOperationFree(oper);
			oper = nextOper;
		}
	}

	/* Free the context block itself */
	free(context);
}

#ifdef	__cplusplus
};
#endif
