/*
 * node.c - Management for node types for "treecc".
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

unsigned int TreeCCHashString(const char *str)
{
	unsigned int hash = 0;
	while(*str != '\0')
	{
		hash = (hash << 5) + hash + (unsigned int)(*str++);
	}
	return hash;
}

static unsigned int HashStringLen(const char *str, int len)
{
	unsigned int hash = 0;
	while(len > 0)
	{
		hash = (hash << 5) + hash + (unsigned int)(*str++);
		--len;
	}
	return hash;
}

void TreeCCNodeFree(TreeCCNode *node)
{
	TreeCCField *field, *nextField;
	TreeCCVirtual *virt, *nextVirt;

	/* Free the node's fields */
	field = node->fields;
	while(field != 0)
	{
		nextField = field->next;
		free(field->name);
		free(field->type);
		if(field->value)
		{
			free(field->value);
		}
		free(field);
		field = nextField;
	}

	/* Free the virtual member function references */
	virt = node->virtuals;
	while(virt != 0)
	{
		nextVirt = virt->next;
		free(virt);
		virt = nextVirt;
	}

	/* Free other members and the node itself */
	free(node->name);
	free(node);
}

/*
 * Add a node to the hash table.
 */
static void AddToHash(TreeCCContext *context, TreeCCNode *node)
{
	unsigned int hash = (TreeCCHashString(node->name) & (TREECC_HASH_SIZE - 1));
	node->nextHash = context->nodeHash[hash];
	context->nodeHash[hash] = node;
}

TreeCCNode *TreeCCNodeCreate(TreeCCContext *context, long linenum,
					  		 char *name, char *parent, int flags)
{
	TreeCCNode *parentNode;
	TreeCCNode *node;

	/* Print debugging information if required */
	if(context->debugMode)
	{
		TreeCCDebug(linenum, "%%node %s %s %d", name,
					(parent ? parent : "no_parent"), flags);
	}

	/* Find or create the parent node */
	if(parent)
	{
		parentNode = TreeCCNodeFind(context, parent);
		if(!parentNode)
		{
			/* Create an undefined placeholder for the parent */
			parentNode = (TreeCCNode *)malloc(sizeof(TreeCCNode));
			if(!parentNode)
			{
				TreeCCOutOfMemory(context->input);
			}
			parentNode->parent = 0;
			parentNode->firstChild = 0;
			parentNode->lastChild = 0;
			parentNode->nextSibling = 0;
			parentNode->name = parent;
			parentNode->flags = TREECC_NODE_UNDEFINED;
			parentNode->number = (context->nodeNumber)++;
			parentNode->filename = context->input->filename;
			parentNode->linenum = linenum;
			parentNode->fields = 0;
			parentNode->virtuals = 0;
			parentNode->header = context->headerStream;
			parentNode->source = context->sourceStream;
			AddToHash(context, parentNode);
		}
		else
		{
			free(parent);
		}
	}
	else
	{
		parentNode = 0;
	}

	/* Find or create the current node */
	node = TreeCCNodeFind(context, name);
	if(node)
	{
		if((node->flags & TREECC_NODE_UNDEFINED) == 0)
		{
			TreeCCErrorOnLine(context->input, context->input->filename, linenum,
							  "node type `%s' is already declared", name);
			TreeCCErrorOnLine(context->input, node->filename, node->linenum,
							  "previous declaration here");
			free(name);
		}
		else
		{
			node->flags = flags;
			node->parent = parentNode;
			node->filename = context->input->filename;
			node->linenum = linenum;
			node->header = context->headerStream;
			node->source = context->sourceStream;
			node->nextSibling = 0;
			if(parentNode)
			{
				if(parentNode->lastChild)
				{
					parentNode->lastChild->nextSibling = node;
				}
				else
				{
					parentNode->firstChild = node;
				}
				parentNode->lastChild = node;
			}
		}
	}
	else
	{
		node = (TreeCCNode *)malloc(sizeof(TreeCCNode));
		if(!node)
		{
			TreeCCOutOfMemory(context->input);
		}
		node->parent = parentNode;
		node->firstChild = 0;
		node->lastChild = 0;
		node->name = name;
		node->flags = flags;
		node->number = (context->nodeNumber)++;
		node->filename = context->input->filename;
		node->linenum = linenum;
		node->fields = 0;
		node->virtuals = 0;
		node->header = context->headerStream;
		node->source = context->sourceStream;
		node->nextSibling = 0;
		if(parentNode)
		{
			if(parentNode->lastChild)
			{
				parentNode->lastChild->nextSibling = node;
			}
			else
			{
				parentNode->firstChild = node;
			}
			parentNode->lastChild = node;
		}
		AddToHash(context, node);
	}
	return node;
}

TreeCCNode *TreeCCNodeFind(TreeCCContext *context, const char *name)
{
	unsigned int hash = (TreeCCHashString(name) & (TREECC_HASH_SIZE - 1));
	TreeCCNode *node = context->nodeHash[hash];
	while(node != 0)
	{
		if(!strcmp(node->name, name))
		{
			return node;
		}
		node = node->nextHash;
	}
	return 0;
}

TreeCCNode *TreeCCNodeFindByType(TreeCCContext *context, const char *name)
{
	unsigned int hash;
	TreeCCNode *node;
	int len;
	int hasSuffix;
	
	/* Strip " *" from the end of the name, if present */
	len = strlen(name);
	if(len >= 2 && name[len - 1] == '*' && name[len - 2] == ' ')
	{
		len -= 2;
		hasSuffix = 1;
	}
	else
	{
		hasSuffix = 0;
	}

	/* Look for the name */
	hash = (HashStringLen(name, len) & (TREECC_HASH_SIZE - 1));
	node = context->nodeHash[hash];
	while(node != 0)
	{
		if(!strncmp(node->name, name, len) && node->name[len] == '\0')
		{
			if((node->flags & TREECC_NODE_ENUM) != 0)
			{
				/* We can only use enumerations as types if no suffix */
				if(hasSuffix)
				{
					return 0;
				}
				else
				{
					return node;
				}
			}
			else if((node->flags & TREECC_NODE_ENUM_VALUE) != 0)
			{
				/* Enumerated values cannot be used as types */
				return 0;
			}
			else
			{
				return node;
			}
		}
		node = node->nextHash;
	}
	return 0;
}

void TreeCCNodeValidate(TreeCCContext *context)
{
	unsigned int hash;
	TreeCCNode *node;
	TreeCCNode *type;
	TreeCCField *field;
	int len;
	int typeCheck = (context->language == TREECC_LANG_C ||
					 context->language == TREECC_LANG_CPP);
	for(hash = 0; hash < TREECC_HASH_SIZE; ++hash)
	{
		node = context->nodeHash[hash];
		while(node != 0)
		{
			if((node->flags & TREECC_NODE_UNDEFINED) != 0)
			{
				TreeCCErrorOnLine(context->input,
								  node->filename, node->linenum,
								  "node type `%s' is not declared",
								  node->name);
			}
			if(!(node->parent) &&
			   (node->flags & TREECC_NODE_UNDEFINED) == 0 &&
			   (node->flags & TREECC_NODE_TYPEDEF) == 0)
			{
				TreeCCErrorOnLine(context->input,
					  node->filename, node->linenum,
					  "base node type `%s' must be declared with %%typedef",
					  node->name);
			}
			if(typeCheck)
			{
				/* In C or C++, fields that are declared as node
				   types must end in "*" */
				field = node->fields;
				while(field != 0)
				{
					type = TreeCCNodeFindByType(context, field->type);
					if(type != 0 && (type->flags & TREECC_NODE_ENUM) == 0)
					{
						len = strlen(field->type);
						if(len < 2 || field->type[len - 1] != '*' ||
						   field->type[len - 2] != ' ')
						{
							TreeCCErrorOnLine(context->input,
									field->filename, field->linenum,
									"field type does not end in `*'");
						}
					}
					field = field->next;
				}
			}
			node = node->nextHash;
		}
	}
}

/*
 * Visit the children of a node in breadth-first order.
 */
static void Visit(TreeCCContext *context, TreeCCNode *node,
				  TreeCCNodeVisitor visitor)
{
	TreeCCNode *child;

	/* Visit the children of the node */
	child = node->firstChild;
	while(child != 0)
	{
		(*visitor)(context, child);
		child = child->nextSibling;
	}

	/* Visit their children */
	child = node->firstChild;
	while(child != 0)
	{
		Visit(context, child, visitor);
		child = child->nextSibling;
	}
}

void TreeCCNodeVisitAll(TreeCCContext *context, TreeCCNodeVisitor visitor)
{
	unsigned int hash;
	TreeCCNode *node;

	/* Find and visit all top-level nodes */
	for(hash = 0; hash < TREECC_HASH_SIZE; ++hash)
	{
		node = context->nodeHash[hash];
		while(node != 0)
		{
			if(!(node->parent))
			{
				(*visitor)(context, node);
				Visit(context, node, visitor);
			}
			node = node->nextHash;
		}
	}
}

int TreeCCNodeIsSingleton(TreeCCNode *node)
{
	while(node != 0)
	{
		if(node->fields != 0)
		{
			return 0;
		}
		node = node->parent;
	}
	return 1;
}

static int HasAbstracts(TreeCCContext *context, TreeCCNode *node,
						TreeCCNode *actualNode)
{
	TreeCCVirtual *virt;
	TreeCCOperationCase *operCase;
	TreeCCNode *tempNode;
	int abstractCase;

	/* Check for abstracts in the parent node type */
	if(node->parent)
	{
		if(HasAbstracts(context, node->parent, actualNode))
		{
			return 1;
		}
	}

	/* Check for abstracts in this node type */
	virt = node->virtuals;
	while(virt != 0)
	{
		/* Determine if we need a definition for this virtual,
		   and whether the definition is real or abstract */
		operCase = TreeCCOperationFindCase(context, actualNode, virt->name);
		if(!operCase)
		{
			tempNode = actualNode->parent;
			abstractCase = 1;
			while(tempNode != 0)
			{
				operCase = TreeCCOperationFindCase
								(context, tempNode, virt->name);
				if(operCase != 0)
				{
					abstractCase = 0;
					break;
				}
				tempNode = tempNode->parent;
			}
			if(abstractCase)
			{
				return 1;
			}
		}
		virt = virt->next;
	}
	return 0;
}

int TreeCCNodeHasAbstracts(TreeCCContext *context, TreeCCNode *node)
{
	return HasAbstracts(context, node, node);
}

void TreeCCNodeAddVirtual(TreeCCContext *context, TreeCCNode *node,
						  TreeCCOperation *oper)
{
	TreeCCVirtual *virt;
	TreeCCVirtual *last;

	/* Print debugging information if required */
	if(context->debugMode)
	{
		TreeCCDebug(oper->linenum, "%%virtual %s %s",
					node->name, oper->name);
	}

	/* Allocate the virtual method block */
	if((virt = (TreeCCVirtual *)malloc(sizeof(TreeCCVirtual))) == 0)
	{
		TreeCCOutOfMemory(context->input);
	}

	/* Initialise the virtual method fields */
	virt->name = oper->name;
	virt->returnType = oper->returnType;
	virt->params = oper->params->next;
	virt->oper = oper;
	virt->next = 0;

	/* Add the virtual method to the end of the node's virtual list */
	if(node->virtuals)
	{
		last = node->virtuals;
		while(last->next != 0)
		{
			last = last->next;
		}
		last->next = virt;
	}
	else
	{
		node->virtuals = virt;
	}
}

int TreeCCNodeInheritsFrom(TreeCCNode *nodea, TreeCCNode *nodeb)
{
	while(nodea != 0)
	{
		if(nodea == nodeb)
		{
			return 1;
		}
		nodea = nodea->parent;
	}
	return 0;
}

void TreeCCNodeClearMarking(TreeCCContext *context, int flags)
{
	unsigned int hash;
	TreeCCNode *node;
	for(hash = 0; hash < TREECC_HASH_SIZE; ++hash)
	{
		node = context->nodeHash[hash];
		while(node != 0)
		{
			node->flags &= ~flags;
			node = node->nextHash;
		}
	}
}

/*
 * Assign positions starting at a particular value.
 */
static int AssignPositions(TreeCCNode *node, int posn)
{
	TreeCCNode *child;

	/* Assign positions to the children */
	child = node->firstChild;
	while(child != 0)
	{
		posn = AssignPositions(child, posn);
		child = child->nextSibling;
	}

	/* Assign a position to this node */
	node->position = posn;
	return posn + 1;
}

int TreeCCNodeAssignPositions(TreeCCNode *node)
{
	return AssignPositions(node, 0);
}

/*
 * Determine if a field name is already declared in a node type.
 */
static int IsDeclared(TreeCCContext *context, TreeCCNode *node,
					  char *name, int reverseError)
{
	TreeCCField *field = node->fields;
	while(field != 0)
	{
		if(!strcmp(field->name, name))
		{
			if(reverseError)
			{
				/* We are attempting to declare a field already in a child */
				TreeCCErrorOnLine(context->input,
						field->filename, field->linenum,
					  	"field `%s' is already declared", name);
				TreeCCError(context->input, "previous declaration here");
			}
			else
			{
				/* We are attempting to declare a field already in a parent */
				TreeCCError(context->input, "field `%s' is already declared",
							name);
				TreeCCErrorOnLine(context->input,
						field->filename, field->linenum,
					  	"previous declaration here");
			}
			return 1;
		}
		field = field->next;
	}
	return 0;
}

/*
 * Determine if a field name is already declared in child node types.
 */
static int IsDeclaredInChildren(TreeCCContext *context, TreeCCNode *node,
								char *name)
{
	TreeCCNode *child = node->firstChild;
	while(child != 0)
	{
		if(IsDeclared(context, child, name, 1))
		{
			return 1;
		}
		if(IsDeclaredInChildren(context, child, name))
		{
			return 1;
		}
		child = child->nextSibling;
	}
	return 0;
}

void TreeCCFieldCreate(TreeCCContext *context, TreeCCNode *node,
					   char *name, char *type, char *value, int flags)
{
	TreeCCNode *current;
	TreeCCField *field;
	TreeCCField *prev;

	/* Print debugging information if required */
	if(context->debugMode)
	{
		TreeCCDebug(context->input->linenum,
					"%%field %s %s %s %d", name,
					(type ? type : "no_type"),
					(value ? value : "no_value"), flags);
	}

	/* Check to make sure that the field does not already exist
	   in this node type or any of its ancestor node types */
	current = node;
	while(current != 0)
	{
		if(IsDeclared(context, current, name, 0))
		{
			free(name);
			free(type);
			if(value)
			{
				free(value);
			}
			return;
		}
		current = current->parent;
	}

	/* Check to make sure that the field does not already exist
	   in any of the child classes.  This may happen if a child
	   class is declared before one of its ancestors */
	IsDeclaredInChildren(context, node, name);

	/* Find the end of the node's field list */
	field = node->fields;
	prev = 0;
	while(field != 0)
	{
		prev = field;
		field = field->next;
	}

	/* Create a new field block and fill it in */
	field = (TreeCCField *)malloc(sizeof(TreeCCField));
	if(!field)
	{
		TreeCCOutOfMemory(context->input);
	}
	field->name = name;
	field->type = type;
	field->value = value;
	field->flags = flags;
	field->filename = context->input->filename;
	field->linenum = context->input->linenum;
	field->next = 0;

	/* Add the field to the list */
	if(prev)
	{
		prev->next = field;
	}
	else
	{
		node->fields = field;
	}
}

#ifdef	__cplusplus
};
#endif
