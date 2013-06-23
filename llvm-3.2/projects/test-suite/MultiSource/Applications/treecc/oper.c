/*
 * oper.c - Management for operations for "treecc".
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

void TreeCCOperationFree(TreeCCOperation *oper)
{
	TreeCCParam *param, *nextParam;
	TreeCCOperationCase *operCase, *nextCase;
	TreeCCTrigger *trigger, *nextTrigger;

	/* Free the name and return type */
	free(oper->name);
	if(oper->className)
	{
		free(oper->className);
	}
	free(oper->returnType);

	/* Free the parameters */
	param = oper->params;
	while(param != 0)
	{
		nextParam = param->next;
		if(param->name)
		{
			free(param->name);
		}
		free(param->type);
		param = nextParam;
	}

	/* Free the operation cases */
	operCase = oper->firstCase;
	while(operCase != 0)
	{
		nextCase = operCase->next;
		if(operCase->code && !(operCase->nextHeader))
		{
			free(operCase->code);
		}
		trigger = operCase->triggers;
		while(trigger != 0)
		{
			nextTrigger = trigger->next;
			free(trigger);
			trigger = nextTrigger;
		}
		free(operCase);
		operCase = nextCase;
	}

	/* Free the array of sorted operation cases */
	if(oper->sortedCases)
	{
		free(oper->sortedCases);
	}

	/* Free the operation block itself */
	free(oper);
}
 
TreeCCOperation *TreeCCOperationCreate(TreeCCContext *context,
									   char *returnType, char *name,
									   char *className, char *defValue,
									   TreeCCParam *params, int flags,
									   int numTriggers, char *filename,
									   long linenum)
{
	TreeCCOperation *oper;
	unsigned int hash;

	/* Print debugging information if required */
	if(context->debugMode)
	{
		TreeCCParam *param;
		TreeCCDebug(linenum, "%%operation %s %s%s%s %d",
					returnType,
					(className ? className : ""),
					(className ? "::" : ""),
					name, flags);
		param = params;
		while(param != 0)
		{
			TreeCCDebug(linenum, "%%param %s %s %d",
						param->type, (param->name ? param->name : "no_name"),
						param->flags);
			param = param->next;
		}
	}

	/* Allocate the operation block */
	if((oper = (TreeCCOperation *)malloc(sizeof(TreeCCOperation))) == 0)
	{
		TreeCCOutOfMemory(context->input);
	}

	/* Initialise the operation block */
	oper->name = name;
	oper->className = className;
	oper->returnType = returnType;
	oper->defValue = defValue;
	oper->params = params;
	oper->flags = flags;
	oper->numTriggers = numTriggers;
	oper->filename = filename;
	oper->linenum = linenum;
	oper->firstCase = 0;
	oper->lastCase = 0;
	oper->sortedCases = 0;
	oper->numCases = 0;
	oper->header = context->headerStream;
	oper->source = context->sourceStream;

	/* Add the operation to the hash table */
	hash = (TreeCCHashString(name) & (TREECC_HASH_SIZE - 1));
	oper->nextHash = context->operHash[hash];
	context->operHash[hash] = oper;

	/* Done */
	return oper;
}

TreeCCOperation *TreeCCOperationFind(TreeCCContext *context, char *name)
{
	unsigned int hash = (TreeCCHashString(name) & (TREECC_HASH_SIZE - 1));
	TreeCCOperation *oper = context->operHash[hash];
	while(oper != 0)
	{
		if(!strcmp(oper->name, name))
		{
			return oper;
		}
		oper = oper->nextHash;
	}
	return 0;
}

TreeCCOperationCase *TreeCCOperationAddCase
			(TreeCCContext *context, TreeCCOperation *oper,
			 TreeCCTrigger *triggers, char *filename, long linenum)
{
	TreeCCOperationCase *operCase;

	/* Print debugging information if required */
	if(context->debugMode)
	{
		if(!triggers)
		{
			TreeCCDebug(linenum, "%%case %s", oper->name);
		}
		else
		{
			TreeCCDebug(linenum, "%%case %s %s", triggers->node->name,
						oper->name);
		}
	}

	/* Allocate the operation case block */
	if((operCase = (TreeCCOperationCase *)malloc(sizeof(TreeCCOperationCase)))
						== 0)
	{
		TreeCCOutOfMemory(context->input);
	}

	/* Initialise the operation case block */
	operCase->triggers = triggers;
	operCase->code = 0;
	operCase->oper = oper;
	operCase->number = 0;
	operCase->filename = filename;
	operCase->linenum = linenum;
	operCase->codeFilename = 0;
	operCase->codeLinenum = 0;
	operCase->next = 0;
	operCase->nextHeader = 0;

	/* Add the case to the operation's case list */
	if(oper->lastCase)
	{
		oper->lastCase->next = operCase;
	}
	else
	{
		oper->firstCase = operCase;
	}
	oper->lastCase = operCase;
	++(oper->numCases);

	/* Return the case to the caller */
	return operCase;
}

/*
 * Verify the coverage of an operation over a node hierarchy.
 */
static void VerifyHierarchy(TreeCCContext *context, TreeCCOperation *oper,
							TreeCCNode *node)
{
	if((node->flags & TREECC_NODE_MARK(0)) != 0)
	{
		return;
	}
	if((node->flags & TREECC_NODE_ABSTRACT) == 0)
	{
		TreeCCErrorOnLine(context->input, oper->filename, oper->linenum,
						  "node type `%s' is not handled in operation `%s'",
						  node->name, oper->name);
		return;
	}
	node = node->firstChild;
	while(node != 0)
	{
		VerifyHierarchy(context, oper, node);
		node = node->nextSibling;
	}
}

/*
 * Append a parameter type name to a comma-separated string.
 */
static char *AppendParam(TreeCCContext *context, char *str, const char *type)
{
	if(str)
	{
		int slen = strlen(str);
		int tlen = strlen(type);
		str = (char *)realloc(str, slen + tlen + 3);
		if(!str)
		{
			TreeCCOutOfMemory(context->input);
		}
		strcpy(str + slen, ", ");
		strcpy(str + slen + 2, type);
		return str;
	}
	else
	{
		return TreeCCDupString((char *)type);
	}
}

/*
 * Forward declaration.
 */
static void MarkMultiCase(TreeCCOperationCase **sortedCases, int base,
						  int multiplier, TreeCCParam *nextParam,
						  TreeCCOperationCase *operCase,
						  TreeCCTrigger *nextTrigger);

/*
 * Scan down a node type hierarchy to mark all covered children.
 */
static void MarkMultiScan(TreeCCOperationCase **sortedCases, int base,
						  int multiplier, TreeCCParam *nextParam,
						  TreeCCOperationCase *operCase,
						  TreeCCTrigger *nextTrigger, TreeCCNode *node)
{
	/* Go in one level for this node */
	MarkMultiCase(sortedCases, base + node->position * multiplier,
				  multiplier * nextParam->size, nextParam->next,
				  operCase, nextTrigger->next);

	/* Scan all of the children */
	node = node->firstChild;
	while(node != 0)
	{
		MarkMultiScan(sortedCases, base, multiplier, nextParam,
					  operCase, nextTrigger, node);
		node = node->nextSibling;
	}
}

/*
 * Mark all of the node types associated with an operation case.
 */
static void MarkMultiCase(TreeCCOperationCase **sortedCases, int base,
						  int multiplier, TreeCCParam *nextParam,
						  TreeCCOperationCase *operCase,
						  TreeCCTrigger *nextTrigger)
{
	/* Scan for the next trigger parameter */
	while(nextParam != 0 && (nextParam->flags & TREECC_PARAM_TRIGGER) == 0)
	{
		nextParam = nextParam->next;
	}

	/* If we are out of triggers, then we need to set the current value */
	if(!nextParam)
	{
		if(sortedCases[base] == 0)
		{
			sortedCases[base] = operCase;
		}
		return;
	}

	/* Scan down the node type hierarchy for this trigger level */
	MarkMultiScan(sortedCases, base, multiplier, nextParam,
				  operCase, nextTrigger, nextTrigger->node);
}

/*
 * Forward declaration.
 */
static void VerifyMultiCoverage(TreeCCContext *context, TreeCCOperation *oper,
								TreeCCOperationCase **sortedCases, int base,
								int multiplier, TreeCCParam *nextParam,
								TreeCCNode **nodeStack,
								TreeCCNode **nodeStackTop);

/*
 * Scan all children to verify a multi-coverage trigger.
 */
static void VerifyMultiScan(TreeCCContext *context, TreeCCOperation *oper,
							TreeCCOperationCase **sortedCases, int base,
							int multiplier, TreeCCParam *nextParam,
							TreeCCNode *node, TreeCCNode **nodeStack,
							TreeCCNode **nodeStackTop)
{
	/* Go in one level for this node */
	*nodeStackTop = node;
	VerifyMultiCoverage(context, oper, sortedCases,
						base + node->position * multiplier,
						multiplier * nextParam->size,
						nextParam->next, nodeStack, nodeStackTop + 1);

	/* Scan all of the children */
	node = node->firstChild;
	while(node != 0)
	{
		VerifyMultiScan(context, oper, sortedCases, base, multiplier,
						nextParam, node, nodeStack, nodeStackTop);
		node = node->nextSibling;
	}
}

/*
 * Verify coverage of a multi-trigger operation.
 */
static void VerifyMultiCoverage(TreeCCContext *context, TreeCCOperation *oper,
								TreeCCOperationCase **sortedCases, int base,
								int multiplier, TreeCCParam *nextParam,
								TreeCCNode **nodeStack,
								TreeCCNode **nodeStackTop)
{
	TreeCCNode *type;

	/* Scan for the next trigger */
	while(nextParam != 0 && (nextParam->flags & TREECC_PARAM_TRIGGER) == 0)
	{
		nextParam = nextParam->next;
	}

	/* If we are out of triggers, then we need to do a test */
	if(!nextParam)
	{
		if(sortedCases[base] == 0)
		{
			/* No case: report an error if none of the node types
			   that led us here are abstract */
			int posn;
			int isAbstract = 0;
			for(posn = 0; posn < oper->numTriggers; ++posn)
			{
				if((nodeStack[posn]->flags & TREECC_NODE_ABSTRACT) != 0)
				{
					isAbstract = 1;
					break;
				}
			}
			if(!isAbstract)
			{
				char *str = 0;
				for(posn = 0; posn < oper->numTriggers; ++posn)
				{
					str = AppendParam(context, str, nodeStack[posn]->name);
				}
				if(!str)
				{
					str = TreeCCDupString("?");
				}
				TreeCCErrorOnLine(context->input, oper->filename, oper->linenum,
								  "case `%s' is missing from operation `%s'",
								  str, oper->name);
				free(str);
			}
		}
		return;
	}

	/* Scan down the node type hierarchy for this parameter level */
	type = TreeCCNodeFindByType(context, nextParam->type);
	if(!type)
	{
		return;
	}
	VerifyMultiScan(context, oper, sortedCases, base,
				    multiplier, nextParam, type,
					nodeStack, nodeStackTop);
}

/*
 * Test the coverage of a multi-trigger operation.
 */
static void MultiCoverageTest(TreeCCContext *context, TreeCCOperation *oper)
{
	TreeCCParam *param;
	TreeCCParam *param2;
	TreeCCNode *type;
	TreeCCNode *type2;
	int size;
	TreeCCNode **nodeStack;
	TreeCCOperationCase *operCase;

	/* Determine the size of the "sortedCases" array, and also
	   check to make sure there are no overlaps between triggers
	   that will cause position information to be inconsistent */
	size = 1;
	param = oper->params;
	while(param != 0)
	{
		if((param->flags & TREECC_PARAM_TRIGGER) != 0)
		{
			type = TreeCCNodeFindByType(context, param->type);
			if(type)
			{
				param->size = TreeCCNodeAssignPositions(type);
				size *= param->size;
				param2 = oper->params;
				while(param2 != 0)
				{
					if((param2->flags & TREECC_PARAM_TRIGGER) != 0 &&
					   param2 != param)
					{
						type2 = TreeCCNodeFindByType(context, param2->type);
						if(type2 && type2 != type)
						{
							if(TreeCCNodeInheritsFrom(type, type2) ||
							   TreeCCNodeInheritsFrom(type2, type))
							{
								TreeCCErrorOnLine(context->input,
										  oper->filename, oper->linenum,
										  "overlap between trigger types");
								return;
							}
						}
					}
					param2 = param2->next;
				}
			}
		}
		param = param->next;
	}

	/* Create the "sortedCases" array */
	if((oper->sortedCases = (TreeCCOperationCase **)calloc
				(size, sizeof(TreeCCOperationCase *))) == 0)
	{
		TreeCCOutOfMemory(context->input);
	}

	/* Find the operation case to use for each combination of values */
	operCase = oper->firstCase;
	while(operCase != 0)
	{
		MarkMultiCase(oper->sortedCases, 0, 1, oper->params,
					  operCase, operCase->triggers);
		operCase = operCase->next;
	}

	/* Verify that every combination of values has been handled */
	nodeStack = (TreeCCNode **)calloc(oper->numTriggers, sizeof(TreeCCNode *));
	if(!nodeStack)
	{
		TreeCCOutOfMemory(context->input);
	}
	VerifyMultiCoverage(context, oper, oper->sortedCases, 0, 1,
						oper->params, nodeStack, nodeStack);
	free(nodeStack);
}

/*
 * Test that an operation covers all relevant node type cases.
 */
static void OperationTest(TreeCCContext *context, TreeCCOperation *oper)
{
	TreeCCOperationCase *operCase;
	TreeCCTrigger *trigger;
	TreeCCParam *param;
	TreeCCNode *node;

	/* Clear the marking bits */
	TreeCCNodeClearMarking(context, TREECC_NODE_MARK_BITS);

	/* Print an error if the operation has no cases at all */
	if(!(oper->firstCase))
	{
		TreeCCErrorOnLine(context->input, oper->filename, oper->linenum,
						  "operation `%s' has no cases", oper->name);
	}

	/* Single or multiple triggers? */
	if(oper->numTriggers == 1)
	{
		/* Mark the coverage of all operation cases */
		operCase = oper->firstCase;
		while(operCase != 0)
		{
			trigger = operCase->triggers;
			if(trigger != 0)
			{
				if((trigger->node->flags & TREECC_NODE_MARK(0)) != 0)
				{
					TreeCCErrorOnLine(context->input, operCase->filename,
									  operCase->linenum,
					  				  "node type `%s' is handled multiple "
									  		"times for operation `%s'",
									  trigger->node->name, oper->name);
				}
				else
				{
					trigger->node->flags |= TREECC_NODE_MARK(0);
				}
			}
			operCase = operCase->next;
		}

		/* Walk the node hierarchy to verify coverage */
		param = oper->params;
		while(param != 0)
		{
			if((param->flags & TREECC_PARAM_TRIGGER) != 0)
			{
				node = TreeCCNodeFindByType(context, param->type);
				if(node)
				{
					VerifyHierarchy(context, oper, node);
				}
			}
			param = param->next;
		}
	}
	else if(oper->numTriggers == 0)
	{
		/* Zero triggers: look for a single case definition */
		operCase = oper->firstCase;
		if(operCase && operCase->next)
		{
			do
			{
				operCase = operCase->next;
				TreeCCErrorOnLine(context->input, operCase->filename,
								  operCase->linenum,
								  "multiple definitions for operation `%s'",
								  oper->name);
			}
			while(operCase->next != 0);
			TreeCCErrorOnLine(context->input, oper->firstCase->filename,
							  oper->firstCase->linenum,
							  "first definition here");
		}
	}
	else
	{
		/* Multiple triggers need a more complex algorithm to verify */
		if(oper->firstCase != 0)
		{
			MultiCoverageTest(context, oper);
		}
	}
}

/*
 * Compare two operation cases for a sort operation.
 */
static int CaseCompare(const void *e1, const void *e2)
{
	TreeCCOperationCase *operCase1 = *((TreeCCOperationCase **)e1);
	TreeCCOperationCase *operCase2 = *((TreeCCOperationCase **)e2);
	TreeCCTrigger *trigger1 = operCase1->triggers;
	TreeCCTrigger *trigger2 = operCase2->triggers;
	TreeCCNode *ancestor1;
	TreeCCNode *ancestor2;
	while(trigger1 != 0 && trigger2 != 0)
	{
		if(trigger1->node != trigger2->node)
		{
			/* See if trigger1->node inherits from trigger2->node */
			ancestor1 = trigger1->node;
			while(ancestor1->parent != 0)
			{
				if(ancestor1->parent == trigger2->node)
				{
					return -1;
				}
				ancestor1 = ancestor1->parent;
			}

			/* See if trigger2->node inherits from trigger1->node */
			ancestor2 = trigger2->node;
			while(ancestor2->parent != 0)
			{
				if(ancestor2->parent == trigger1->node)
				{
					return 1;
				}
				ancestor2 = ancestor2->parent;
			}

			/* If the ancestors are separate, then use the node numbers
			   to order the separate trees.  Note: this is highly unlikely
			   because operation definition forces all cases to inherit
			   from a declared common type.  However, if we change this
			   requirement in the future, this code will be needed, so
			   we are being paranoid and including it now */
			if(ancestor1 != ancestor2)
			{
				if(ancestor1->number < ancestor2->number)
				{
					return -1;
				}
				else
				{
					return 1;
				}
			}

			/* This is the hard case: the two nodes are in separate
			   subtrees of the same ancestor, but we still need an
			   ordering between them.  We need to find the lowest
			   common ancestor, and the nodes just below it */
			ancestor1 = trigger1->node;
			while(ancestor1->parent != 0)
			{
				ancestor1 = ancestor1->parent;
				ancestor1->flags |= TREECC_NODE_MARK(0);
			}
			ancestor2 = trigger2->node;
			while(ancestor2->parent != 0)
			{
				if((ancestor2->parent->flags & TREECC_NODE_MARK(0)) != 0)
				{
					break;
				}
				ancestor2 = ancestor2->parent;
			}
			ancestor1 = trigger1->node;
			while(ancestor1->parent != 0)
			{
				ancestor1 = ancestor1->parent;
				ancestor1->flags &= ~(TREECC_NODE_MARK(0));
			}
			ancestor1 = trigger1->node;
			while(ancestor1->parent != ancestor2->parent)
			{
				ancestor1 = ancestor1->parent;
			}

			/* ancestor1 and ancestor2 are now one step down
			   from the common ancestor that we found.  Use
			   their node numbers to order the trees */
			if(ancestor1->number < ancestor2->number)
			{
				return -1;
			}
			else
			{
				return 1;
			}
		}
		trigger1 = trigger1->next;
		trigger2 = trigger2->next;
	}
	return 0;
}

/*
 * Sort a set of operation cases so that A precedes B
 * if A inherits directly or indirectly from B.  Operations
 * with multiple triggers sort on the first, then the second,
 * then the third, etc.
 */
static void SortCases(TreeCCContext *context, TreeCCOperation *oper)
{
	TreeCCOperationCase **caseList;
	TreeCCOperationCase *operCase;
	int num;

	/* Bail out early if there are 0 or 1 cases, or no triggers */
	if(oper->numCases < 2 || !(oper->numTriggers))
	{
		return;
	}

	/* Clear the marking bits */
	TreeCCNodeClearMarking(context, TREECC_NODE_MARK_BITS);

	/* Build an array that contains all operation cases */
	if((caseList = (TreeCCOperationCase **)malloc
				(sizeof(TreeCCOperationCase *) * oper->numCases)) == 0)
	{
		TreeCCOutOfMemory(context->input);
	}
	num = 0;
	operCase = oper->firstCase;
	while(operCase != 0)
	{
		caseList[num++] = operCase;
		operCase = operCase->next;
	}

	/* Sort the case list.  There are probably more efficient
	   ways to do this (e.g. topological sorting), but this
	   version is perhaps easier to understand and maintain */
#if HAVE_QSORT
	qsort(caseList, oper->numCases, sizeof(TreeCCOperationCase *), CaseCompare);
#else
	{
		int i, j;
		num = oper->numCases;
		for(i = 0; i < (num - 1); ++i)
		{
			for(j = (i + 1); j < num; ++j)
			{
				if(CaseCompare(&(caseList[i]), &(caseList[j])) > 0)
				{
					operCase = caseList[i];
					caseList[i] = caseList[j];
					caseList[j] = operCase;
				}
			}
		}
	}
#endif

	/* Build a new case list from the sorted array */
	oper->firstCase = caseList[0];
	for(num = 1; num < oper->numCases; ++num)
	{
		caseList[num - 1]->next = caseList[num];
	}
	oper->lastCase = caseList[oper->numCases - 1];
	caseList[oper->numCases - 1]->next = 0;
	free(caseList);
}

void TreeCCOperationValidate(TreeCCContext *context)
{
	unsigned int hash;
	TreeCCOperation *oper;
	for(hash = 0; hash < TREECC_HASH_SIZE; ++hash)
	{
		oper = context->operHash[hash];
		while(oper != 0)
		{
			if((oper->flags & TREECC_OPER_VIRTUAL) == 0)
			{
				/* Non-virtuals must be sorted for code generation,
				   and we also rely upon the sorted order to help us
				   test for operation coverage when multiple triggers
				   are used by the operation */
				SortCases(context, oper);
			}
			OperationTest(context, oper);
			oper = oper->nextHash;
		}
	}
}

TreeCCOperationCase *TreeCCOperationFindCase
		(TreeCCContext *context, TreeCCNode *node, char *name)
{
	TreeCCOperation *oper;
	TreeCCOperationCase *operCase;
	oper = TreeCCOperationFind(context, name);
	if(!oper)
	{
		return 0;
	}
	operCase = oper->firstCase;
	while(operCase != 0)
	{
		if(operCase->triggers && operCase->triggers->node == node)
		{
			return operCase;
		}
		operCase = operCase->next;
	}
	return 0;
}

void TreeCCOperationVisitAll(TreeCCContext *context,
							 TreeCCOperationVisitor visitor)
{
	unsigned int hash;
	TreeCCOperation *oper;
	for(hash = 0; hash < TREECC_HASH_SIZE; ++hash)
	{
		oper = context->operHash[hash];
		while(oper != 0)
		{
			(*visitor)(context, oper);
			oper = oper->nextHash;
		}
	}
}

#ifdef	__cplusplus
};
#endif
