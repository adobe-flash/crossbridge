/*
 * info.h - Store information about parsed "treecc" input files.
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

#ifndef	_TREECC_INFO_H
#define	_TREECC_INFO_H

#include "stream.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Forward declarations.
 */
typedef struct _tagTreeCCField			TreeCCField;
typedef struct _tagTreeCCNode			TreeCCNode;
typedef struct _tagTreeCCOperation		TreeCCOperation;
typedef struct _tagTreeCCParam			TreeCCParam;
typedef struct _tagTreeCCVirtual		TreeCCVirtual;
typedef struct _tagTreeCCTrigger		TreeCCTrigger;
typedef struct _tagTreeCCOperationCase	TreeCCOperationCase;

/*
 * Field definition flags.
 */
#define	TREECC_FIELD_NOCREATE		1

/*
 * Node definition flags.
 */
#define	TREECC_NODE_UNDEFINED		1
#define	TREECC_NODE_ABSTRACT		2
#define	TREECC_NODE_TYPEDEF			4
#define	TREECC_NODE_ENUM			8
#define	TREECC_NODE_ENUM_VALUE		16
#define	TREECC_NODE_MARK(n)			(0x100 << (n))
#define	TREECC_NODE_MARK_BITS		0xFF00

/*
 * Operation flags.
 */
#define	TREECC_OPER_VIRTUAL			1
#define	TREECC_OPER_INLINE			2
#define	TREECC_OPER_SPLIT			4

/*
 * Parameter flags.
 */
#define	TREECC_PARAM_TRIGGER		1

/*
 * Language values.
 */
#define	TREECC_LANG_C				0
#define	TREECC_LANG_CPP				1
#define	TREECC_LANG_JAVA			2
#define	TREECC_LANG_CSHARP			3
#define	TREECC_LANG_RUBY			4
#define	TREECC_LANG_PHP				5

/*
 * Information that is stored about a field.
 */
struct _tagTreeCCField
{
	char		   *name;		/* Name of the field */
	char		   *type;		/* Type associated with the field */
	char		   *value;		/* Default value for the field */
	int				flags;		/* Field flags */
	char		   *filename;	/* File that defines the field */
	long			linenum;	/* Line where the field is defined */
	TreeCCField	   *next;		/* Next field for the node type */

};

/*
 * Information that is stored about a node type.
 */
struct _tagTreeCCNode
{
	TreeCCNode	   *parent;		/* Parent node type */
	TreeCCNode	   *firstChild;	/* First child node type */
	TreeCCNode	   *lastChild;	/* Last child node type */
	TreeCCNode	   *nextSibling;/* Next sibling node type under parent */
	char		   *name;		/* Name of this node type */
	int				flags;		/* Node flags */
	int				number;		/* Number associated with this node type */
	int				position;	/* Position within the tree for operations */
	char		   *filename;	/* File that defines the node type */
	long			linenum;	/* Line where the node type is defined */
	TreeCCField    *fields;		/* List of fields for this node type */
	TreeCCVirtual  *virtuals;	/* List of virtual methods for this node */
	TreeCCNode	   *nextHash;	/* Next in the name hash table */
	TreeCCStream   *header;		/* Stream to write header to */
	TreeCCStream   *source;		/* Stream to write source to */

};

/*
 * Information that is stored about an operation.
 */
struct _tagTreeCCOperation
{
	char		   *name;		/* Name of the operation */
	char		   *className;	/* Name of the enclosing class */
	char		   *returnType;	/* Return type for the operation */
	char		   *defValue;	/* Default value for the operation */
	TreeCCParam    *params;		/* Parameters for the operation */
	int				flags;		/* Flags associated with the operation */
	int				numTriggers;/* Number of trigger parameters */
	char		   *filename;	/* File where the operation is declared */
	long			linenum;	/* Line where the operation is declared */
	TreeCCOperation *nextHash;	/* Next in the operation hash table */
	TreeCCOperationCase *firstCase; /* First case associated with operation */
	TreeCCOperationCase *lastCase;  /* Last case associated with operation */
	TreeCCOperationCase **sortedCases; /* Cases sorted for non-virtual ops */
	int				numCases;	/* Number of operation cases */
	TreeCCStream   *header;		/* Stream to write header to */
	TreeCCStream   *source;		/* Stream to write source to */

};

/*
 * Information that is stored about an operation parameter.
 */
struct _tagTreeCCParam
{
	char		   *name;		/* Name of the parameter */
	char		   *type;		/* Type for the parameter */
	int				flags;		/* Flags associated with the parameter */
	int				size;		/* Dimension size for non-virtual operations */
	TreeCCParam	   *next;		/* Next parameter for the operation */

};

/*
 * Information that is stored about a virtual method on a node type.
 */
struct _tagTreeCCVirtual
{
	char		   *name;		/* Name of the virtual method */
	char		   *returnType;	/* Return type for the virtual method */
	TreeCCParam	   *params;		/* Non-instance parameters */
	TreeCCOperation *oper;		/* Operation block for the virtual */
	TreeCCVirtual  *next;		/* Next virtual method for the node type */

};

/*
 * Information that is stored about a trigger match on an operation case.
 */
struct _tagTreeCCTrigger
{
	TreeCCNode	   *node;		/* Node type for the trigger */
	TreeCCTrigger  *next;		/* Next trigger for this case */

};

/*
 * Information that is stored about an operation case.
 */
struct _tagTreeCCOperationCase
{
	TreeCCTrigger  *triggers;	/* Trigger list for this case */
	char		   *code;		/* Code associated with the case */
	TreeCCOperation *oper;		/* Operation this case is associated with */
	int				number;		/* Reference number for code generation */
	char		   *filename;	/* File that starts the case definition */
	long		   	linenum;	/* Line that starts the case definition */
	char		   *codeFilename;/* File that starts the code */
	long		   	codeLinenum;/* Line that starts the code */
	TreeCCOperationCase *next;	/* Next case for the operation */
	TreeCCOperationCase *nextHeader; /* Next header for same code block */

};

/*
 * Context object that stores all definitions parsed from the input.
 */
#define	TREECC_HASH_SIZE	512
typedef struct _tagTreeCCContext
{
	/* Hash table that allows quick lookup of node types */
	TreeCCNode	   *nodeHash[TREECC_HASH_SIZE];

	/* Hash table that allows quick lookup of operation names */
	TreeCCOperation *operHash[TREECC_HASH_SIZE];

	/* Current input stream */
	TreeCCInput	   *input;

	/* Output streams */
	TreeCCStream   *streamList;			/* List of all streams */
	TreeCCStream   *headerStream;		/* Current header stream */
	TreeCCStream   *sourceStream;		/* Current source stream */
	TreeCCStream   *commonHeader;		/* Stream for common definitions */
	TreeCCStream   *commonSource;		/* Stream for common source */

	/* Flags that control the behaviour of the program */
	int				debugMode : 1;		/* Enable debug output */
	int				track_lines : 1;	/* Track node creation lines */
	int				no_singletons : 1;	/* Don't handle singletons specially */
	int				reentrant : 1;		/* Build a re-entrant system */
	int				force : 1;			/* Force the creation of files */
	int				virtual_factory : 1;/* Allow overrides of factory methods */
	int				abstract_factory : 1;/* Declare factory methods abstract */
	int				kind_in_vtable : 1;	/* Put kind value in vtable only */
	int				strip_filenames : 1; /* Strip names in #line directives */
	int				print_lines : 1;	/* Dont emit #line directives */
	int				internal_access : 1; /* Use "internal" classes in C# */
	int				use_allocator : 1;	/* Use the skeleton allocator */
	int				use_gc_allocator : 1; /* Use the libgc allocator */

	/* String to use to replace "yy" in output files */
	char		   *yy_replacement;

	/* Name of the type to use for re-entrant state */
	char		   *state_type;

	/* Namespace to store declarations within */
	char		   *namespace;

	/* Current node type number */
	int				nodeNumber;

	/* Output source language to use */
	int				language;

	/* Size of blocks to use in C/C++ memory alloction */
	int				block_size;

	/* Name of the directory to output Java source files to */
	char		   *outputDirectory;

	/* name of the base type which is what %typedef expands to */
	char		   *baseType;

} TreeCCContext;

/*
 * Create a context.
 */
TreeCCContext *TreeCCContextCreate(TreeCCInput *input);

/*
 * Destroy a context.
 */
void TreeCCContextDestroy(TreeCCContext *context);

/*
 * Hash a string.
 */
unsigned int TreeCCHashString(const char *str);

/*
 * Flags for "TreeCCAddLiteralDefn".
 */
#define	TREECC_LITERAL_CODE			1		/* In source */
#define	TREECC_LITERAL_DECLS		2		/* In header */
#define	TREECC_LITERAL_END			4		/* Place at end */

/*
 * Add a literal code definition block to the context.
 */
void TreeCCAddLiteralDefn(TreeCCContext *context, char *code, int flags);

/*
 * Free a node definition.
 */
void TreeCCNodeFree(TreeCCNode *node);

/*
 * Create a new node definition.
 */
TreeCCNode *TreeCCNodeCreate(TreeCCContext *context, long linenum,
					  		 char *name, char *parent, int flags);

/*
 * Find a node definition given its name.
 */
TreeCCNode *TreeCCNodeFind(TreeCCContext *context, const char *name);

/*
 * Find a node definition given a type name, which may be
 * either "identifier" or "identifier *".
 */
TreeCCNode *TreeCCNodeFindByType(TreeCCContext *context, const char *name);

/*
 * Validate the node type hierarchy to ensure that everything is defined.
 */
void TreeCCNodeValidate(TreeCCContext *context);

/*
 * Visit every node type in a context's hierarchy.
 */
typedef void (*TreeCCNodeVisitor)(TreeCCContext *context, TreeCCNode *node);
void TreeCCNodeVisitAll(TreeCCContext *context, TreeCCNodeVisitor visitor);

/*
 * Determine if a node type is a singleton.  i.e. no fields.
 */
int TreeCCNodeIsSingleton(TreeCCNode *node);

/*
 * Determine if a node type has abstract virtual operation cases.
 */
int TreeCCNodeHasAbstracts(TreeCCContext *context, TreeCCNode *node);

/*
 * Add a virtual operation to a node.
 */
void TreeCCNodeAddVirtual(TreeCCContext *context, TreeCCNode *node,
						  TreeCCOperation *oper);

/*
 * Determine if "nodea" inherits from "nodeb".
 */
int TreeCCNodeInheritsFrom(TreeCCNode *nodea, TreeCCNode *nodeb);

/*
 * Clear all marking bits that are used to track operation coverage.
 */
void TreeCCNodeClearMarking(TreeCCContext *context, int flags);

/*
 * Assign positions to all nodes started at a particular place
 * in the node type hierarchy.  Returns the number of positions.
 */
int TreeCCNodeAssignPositions(TreeCCNode *node);

/*
 * Create a new field definition and add it to a node.
 */
void TreeCCFieldCreate(TreeCCContext *context, TreeCCNode *node,
					   char *name, char *type, char *value, int flags);

/*
 * Free an operation.
 */
void TreeCCOperationFree(TreeCCOperation *oper);

/*
 * Create a new operation.
 */
TreeCCOperation *TreeCCOperationCreate(TreeCCContext *context,
									   char *returnType, char *name,
									   char *className, char *defValue,
									   TreeCCParam *params, int flags,
									   int numTriggers, char *filename,
									   long linenum);

/*
 * Find an operation with a specific name.
 */
TreeCCOperation *TreeCCOperationFind(TreeCCContext *context, char *name);

/*
 * Add a case definition to an operation.
 */
TreeCCOperationCase *TreeCCOperationAddCase
				(TreeCCContext *context, TreeCCOperation *oper,
				 TreeCCTrigger *triggers, char *filename, long linenum);

/*
 * Validate all operations.
 */
void TreeCCOperationValidate(TreeCCContext *context);

/*
 * Find the operation case that corresponds to a virtual method.
 * Returns NULL if the node does not have a virtual implementation.
 */
TreeCCOperationCase *TreeCCOperationFindCase
		(TreeCCContext *context, TreeCCNode *node, char *name);

/*
 * Visit all operations declared by a context.
 */
typedef void (*TreeCCOperationVisitor)(TreeCCContext *context,
									   TreeCCOperation *oper);
void TreeCCOperationVisitAll(TreeCCContext *context,
							 TreeCCOperationVisitor visitor);

/*
 * Include the contents of a skeleton file in an output stream.
 */
void TreeCCIncludeSkeleton(TreeCCContext *context, TreeCCStream *stream,
						   const char *skeleton);

#ifdef	__cplusplus
};
#endif

#endif	/* _TREECC_INFO_H */
