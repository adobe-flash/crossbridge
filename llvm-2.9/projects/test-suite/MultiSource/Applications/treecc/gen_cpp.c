/*
 * gen_cpp.c - Generate C++ source code from "treecc" input files.
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
#include "gen.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Define the node numbers.
 */
static void DefineNodeNumbers(TreeCCContext *context,
							  TreeCCNode *node)
{
	TreeCCStream *stream = node->header;
	if((node->flags & (TREECC_NODE_ENUM | TREECC_NODE_ENUM_VALUE)) == 0)
	{
		TreeCCStreamPrint(stream, "const int %s_kind = %d;\n",
						  node->name, node->number);
	}
}

/*
 * Declare the fields for a node type.
 */
static void DeclareFields(TreeCCContext *context,
						  TreeCCStream *stream, TreeCCNode *node)
{
	TreeCCField *field;
	field = node->fields;
	while(field != 0)
	{
		TreeCCStreamPrint(stream, "\t%s %s;\n", field->type, field->name);
		field = field->next;
	}
}

/*
 * Declare the virtuals for a node type.
 */
static void DeclareVirtuals(TreeCCContext *context, TreeCCStream *stream,
							TreeCCNode *node, TreeCCNode *actualNode)
{
	TreeCCVirtual *virt;
	TreeCCParam *param;
	int num, needComma;
	TreeCCOperationCase *operCase;
	int declareCase, abstractCase;
	TreeCCNode *tempNode;
	if(node->parent)
	{
		DeclareVirtuals(context, stream, node->parent, actualNode);
	}
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
			declareCase = abstractCase;
		}
		else
		{
			declareCase = 1;
			abstractCase = 0;
		}
		if(declareCase)
		{
			TreeCCStreamPrint(stream, "\tvirtual %s %s(",
							  virt->returnType, virt->name);
			param = virt->params;
			num = 1;
			needComma = 0;
			while(param != 0)
			{
				if(needComma)
				{
					TreeCCStreamPrint(stream, ", ");
				}
				else
				{
					needComma = 1;
				}
				if(param->name)
				{
					TreeCCStreamPrint(stream, "%s %s",
									  param->type, param->name);
				}
				else
				{
					TreeCCStreamPrint(stream, "%s P%d__",
									  param->type, num);
					++num;
				}
				param = param->next;
			}
			if(abstractCase)
			{
				TreeCCStreamPrint(stream, ") = 0;\n");
			}
			else
			{
				TreeCCStreamPrint(stream, ");\n");
			}
		}
		virt = virt->next;
	}
}

/*
 * Declare the type definitions for a node type.
 */
static void DeclareTypeDefs(TreeCCContext *context,
						    TreeCCNode *node)
{
	TreeCCStream *stream = node->header;
	if((node->flags & TREECC_NODE_ENUM) != 0)
	{
		/* Define an enumerated type */
		TreeCCNode *child;
		TreeCCStreamPrint(stream, "typedef enum {\n");
		child = node->firstChild;
		while(child != 0)
		{
			if((child->flags & TREECC_NODE_ENUM_VALUE) != 0)
			{
				TreeCCStreamPrint(stream, "\t%s,\n", child->name);
			}
			child = child->nextSibling;
		}
		TreeCCStreamPrint(stream, "} %s;\n\n", node->name);
	}
	else if((node->flags & TREECC_NODE_ENUM_VALUE) == 0)
	{
		/* Define a regular node type */
		TreeCCStreamPrint(stream, "class %s;\n", node->name);
	}
}

/*
 * Output the parameters for a node creation function.
 */
static int CreateParams(TreeCCContext *context, TreeCCStream *stream,
						TreeCCNode *node, int needComma)
{
	TreeCCField *field;
	if(node->parent)
	{
		needComma = CreateParams(context, stream, node->parent, needComma);
	}
	field = node->fields;
	while(field != 0)
	{
		if((field->flags & TREECC_FIELD_NOCREATE) == 0)
		{
			if(needComma)
			{
				TreeCCStreamPrint(stream, ", ");
			}
			TreeCCStreamPrint(stream, "%s %s", field->type, field->name);
			needComma = 1;
		}
		field = field->next;
	}
	return needComma;
}

/*
 * Build the type declarations for a node type.
 */
static void BuildTypeDecls(TreeCCContext *context,
						   TreeCCNode *node)
{
	TreeCCStream *stream = node->header;
	int needComma;

	/* Ignore if this is an enumerated type node */
	if((node->flags & (TREECC_NODE_ENUM | TREECC_NODE_ENUM_VALUE)) != 0)
	{
		return;
	}

	/* Output the class header */
	if(node->parent)
	{
		/* Inherit from a specified parent type */
		TreeCCStreamPrint(stream, "class %s : public %s\n{\n",
						  node->name, node->parent->name);
	}
	else
	{
		/* This type is the base of a class hierarchy */
		if(context->baseType)
		{
			TreeCCStreamPrint(stream, "class %s : public %s\n{\n", node->name,context->baseType);
		}
		else
		{
			TreeCCStreamPrint(stream, "class %s\n{\n", node->name);
		}

		/* The following fields have protected access */
		TreeCCStreamPrint(stream, "protected:\n\n");

		/* Declare the node kind member variable */
		TreeCCStreamPrint(stream, "\tint kind__;\n");

		/* Declare the filename and linenum fields if we are tracking lines */
		if(context->track_lines)
		{
			TreeCCStreamPrint(stream, "\tchar *filename__;\n");
			TreeCCStreamPrint(stream, "\tlong linenum__;\n");
		}

		/* Declare the public methods for access to the above fields */
		TreeCCStreamPrint(stream, "\npublic:\n\n");
		TreeCCStreamPrint(stream, "\tint getKind() const { return kind__; }\n");
		if(context->track_lines)
		{
			TreeCCStreamPrint(stream,
				"\tconst char *getFilename() const { return filename__; }\n");
			TreeCCStreamPrint(stream,
				"\tlong getLinenum() const { return linenum__; }\n");
			TreeCCStreamPrint(stream,
			 "\tvoid setFilename(char *filename) { filename__ = filename; }\n");
			TreeCCStreamPrint(stream,
				"\tvoid setLinenum(long linenum) { linenum__ = linenum; }\n");
		}
		TreeCCStreamPrint(stream, "\n");

		/* Declare the overloaded "new" and "delete" operators */
		if(!(context->reentrant))
		{
			TreeCCStreamPrint(stream,
					"\tvoid *operator new(size_t);\n");
			TreeCCStreamPrint(stream,
					"\tvoid operator delete(void *, size_t);\n\n");
		}
	}

	/* Declare the constructor for the node type */
	if(context->reentrant)
	{
		/* Re-entrant systems use a factory to create the nodes */
		if(context->virtual_factory || context->abstract_factory)
		{
			TreeCCStreamPrint(stream, "public: // for virtual factory\n\n");
		}
		else
		{
			TreeCCStreamPrint(stream, "protected:\n\n");
			TreeCCStreamPrint(stream, "\tfriend class %s;\n\n",
							  context->state_type);
		}
	}
	else
	{
		/* Non-reentrant systems can construct nodes directly,
		   unless the node happens to be abstract, in which
		   case we force the constructor to be protected */
		if((node->flags & TREECC_NODE_ABSTRACT) != 0)
		{
			TreeCCStreamPrint(stream, "protected:\n\n");
		}
		else
		{
			TreeCCStreamPrint(stream, "public:\n\n");
		}
	}
	TreeCCStreamPrint(stream, "\t%s(", node->name);
	if(context->reentrant)
	{
		TreeCCStreamPrint(stream, "%s *state__", context->state_type);
		needComma = 1;
	}
	else
	{
		needComma = 0;
	}
	CreateParams(context, stream, node, needComma);
	TreeCCStreamPrint(stream, ");\n\n");

	/* Declare the fields */
	TreeCCStreamPrint(stream, "public:\n\n");
	if(node->fields)
	{
		DeclareFields(context, stream, node);
		TreeCCStreamPrint(stream, "\n");
	}

	/* Declare the virtual function prototypes */
	DeclareVirtuals(context, stream, node, node);
	TreeCCStreamPrint(stream, "\n");

	/* Declare the "isA" and "getKindName" helper methods */
	TreeCCStreamPrint(stream, "\tvirtual int isA(int kind) const;\n");
	TreeCCStreamPrint(stream, "\tvirtual const char *getKindName() const;\n\n");

	/* Declare the protected destructor for the node type.
	   The destructor is never called, but we need it to
	   keep C++ compilers from whinging about classes with
	   virtuals, but no virtual destructor */
	TreeCCStreamPrint(stream, "protected:\n\n");
	TreeCCStreamPrint(stream, "\tvirtual ~%s();\n\n", node->name);

	/* Output the class footer */
	TreeCCStreamPrint(stream, "};\n\n");
}

/*
 * Declare the prototypes for the non-virtual operations.
 */
static void DeclareNonVirtuals(TreeCCContext *context,
						  	   TreeCCOperation *oper)
{
	TreeCCStream *stream = oper->header;
	TreeCCParam *param;
	int num;
	int needComma;

	/* Bail out if the operation is virtual */
	if((oper->flags & TREECC_OPER_VIRTUAL) != 0)
	{
		return;
	}

	/* Output a class header if the operation is inside a class */
	if(oper->className)
	{
		TreeCCStreamPrint(stream, "class %s\n", oper->className);
		TreeCCStreamPrint(stream, "{\n");
		TreeCCStreamPrint(stream, "public:\n\n");
		TreeCCStreamPrint(stream, "\tstatic ");
	}

	/* Output the prototype */
	TreeCCStreamPrint(stream, "%s %s(", oper->returnType, oper->name);
	param = oper->params;
	num = 1;
	needComma = 0;
	while(param != 0)
	{
		if(needComma)
		{
			TreeCCStreamPrint(stream, ", ");
		}
		if(param->name)
		{
			TreeCCStreamPrint(stream, "%s %s", param->type, param->name);
		}
		else
		{
			TreeCCStreamPrint(stream, "%s P%d__", param->type, num);
			++num;
		}
		needComma = 1;
		param = param->next;
	}
	if(!needComma)
	{
		TreeCCStreamPrint(stream, "void");
	}
	TreeCCStreamPrint(stream, ");\n");

	/* Output a class footer if the operation is inside a class */
	if(oper->className)
	{
		TreeCCStreamPrint(stream, "};\n\n");
	}
}

/*
 * Output a constructor parameter list to the source stream.
 */
static int CreateParamsSource(TreeCCContext *context, TreeCCStream *stream,
							  TreeCCNode *node, int needComma)
{
	TreeCCField *field;
	if(node->parent)
	{
		needComma = CreateParamsSource(context, stream,
									   node->parent, needComma);
	}
	field = node->fields;
	while(field != 0)
	{
		if((field->flags & TREECC_FIELD_NOCREATE) == 0)
		{
			if(needComma)
			{
				TreeCCStreamPrint(stream, ", ");
			}
			TreeCCStreamPrint(stream, "%s %s", field->type, field->name);
			needComma = 1;
		}
		field = field->next;
	}
	return needComma;
}

/*
 * Output the parameters to call an inherited constructor.
 */
static int InheritParamsSource(TreeCCContext *context, TreeCCStream *stream,
							   TreeCCNode *node, int needComma)
{
	TreeCCField *field;
	if(node->parent)
	{
		needComma = InheritParamsSource(context, stream,
									    node->parent, needComma);
	}
	field = node->fields;
	while(field != 0)
	{
		if((field->flags & TREECC_FIELD_NOCREATE) == 0)
		{
			if(needComma)
			{
				TreeCCStreamPrint(stream, ", ");
			}
			TreeCCStreamPrint(stream, "%s", field->name);
			needComma = 1;
		}
		field = field->next;
	}
	return needComma;
}

/*
 * Output the constructor implementation for a node type.
 */
static void ImplementConstructor(TreeCCContext *context, TreeCCStream *stream,
								 TreeCCNode *node)
{
	int needComma;
	TreeCCField *field;

	/* Output the constructor function header */
	TreeCCStreamPrint(stream, "%s::%s(", node->name, node->name);
	if(context->reentrant)
	{
		TreeCCStreamPrint(stream, "%s *state__", context->state_type);
		needComma = 1;
	}
	else
	{
		needComma = 0;
	}
	CreateParamsSource(context, stream, node, needComma);
	TreeCCStreamPrint(stream, ")\n");

	/* Call the parent class constructor */
	if(node->parent)
	{
		TreeCCStreamPrint(stream, "\t: %s(", node->parent->name);
		if(context->reentrant)
		{
			TreeCCStreamPrint(stream, "state__");
			needComma = 1;
		}
		else
		{
			needComma = 0;
		}
		InheritParamsSource(context, stream, node->parent, needComma);
		TreeCCStreamPrint(stream, ")\n");
	}

	/* Output the beginning of the function body */
	TreeCCStreamPrint(stream, "{\n");

	/* Set the node kind */
	TreeCCStreamPrint(stream, "\tthis->kind__ = %s_kind;\n", node->name);

	/* Track the filename and line number if necessary */
	if(context->track_lines && !(node->parent))
	{
		if(context->reentrant)
		{
			TreeCCStreamPrint(stream,
					"\tthis->filename__ = state__->currFilename();\n");
			TreeCCStreamPrint(stream,
					"\tthis->linenum__ = state__->currLinenum();\n");
		}
		else
		{
			TreeCCStreamPrint(stream,
					"\tthis->filename__ = %s::getState()->currFilename();\n",
					context->state_type);
			TreeCCStreamPrint(stream,
					"\tthis->linenum__ = %s::getState()->currLinenum();\n",
					context->state_type);
		}
	}

	/* Initialize the fields that are specific to this node type */
	field = node->fields;
	while(field != 0)
	{
		if((field->flags & TREECC_FIELD_NOCREATE) == 0)
		{
			TreeCCStreamPrint(stream, "\tthis->%s = %s;\n",
							  field->name, field->name);
		}
		else if(field->value)
		{
			TreeCCStreamPrint(stream, "\tthis->%s = %s;\n",
							  field->name, field->value);
		}
		field = field->next;
	}

	/* Output the constructor function footer */
	TreeCCStreamPrint(stream, "}\n\n");
}

/*
 * Implement the virtual methods that have implementations in a node type.
 */
static void ImplementVirtuals(TreeCCContext *context, TreeCCStream *stream,
							  TreeCCNode *node, TreeCCNode *actualNode)
{
	TreeCCVirtual *virt;
	TreeCCParam *param;
	TreeCCOperationCase *operCase;
	TreeCCOperation *oper;
	int num, first;
	int needComma;
	if(node->parent)
	{
		ImplementVirtuals(context, stream, node->parent, actualNode);
	}
	virt = node->virtuals;
	while(virt != 0)
	{
		operCase = TreeCCOperationFindCase(context, actualNode, virt->name);
		if(!operCase)
		{
			/* We don't have a direct implementation in this class */
			virt = virt->next;
			continue;
		}
		TreeCCStreamPrint(stream, "%s %s::%s(", virt->returnType,
						  actualNode->name, virt->name);
		oper = operCase->oper;
		param = oper->params;
		needComma = 0;
		num = 1;
		first = 1;
		while(param != 0)
		{
			if(needComma)
			{
				TreeCCStreamPrint(stream, ", ");
			}
			if(first)
			{
				/* Skip the first argument, which corresponds to "this" */
				if(!(param->name))
				{
					++num;
				}
				first = 0;
			}
			else
			{
				if(param->name)
				{
					TreeCCStreamPrint(stream, "%s %s",
									  param->type, param->name);
				}
				else
				{
					TreeCCStreamPrint(stream, "%s P%d__",
									  param->type, num);
					++num;
				}
				needComma = 1;
			}
			param = param->next;
		}
		TreeCCStreamPrint(stream, ")\n");
		if(!(oper->params->name) || !strcmp(oper->params->name, "this"))
		{
			/* The first parameter is called "this", so we don't
			   need to declare it at the head of the function */
			TreeCCStreamLine(stream, operCase->codeLinenum,
							 operCase->codeFilename);
			TreeCCStreamPrint(stream, "{");
			TreeCCStreamCode(stream, operCase->code);
			TreeCCStreamPrint(stream, "}\n");
			TreeCCStreamFixLine(stream);
		}
		else
		{
			/* The first parameter is called something else,
			   so create a temporary variable to hold "this" */
		   	TreeCCStreamPrint(stream, "{\n\t%s *%s = this;\n",
							  actualNode->name, oper->params->name);
			TreeCCStreamLine(stream, operCase->codeLinenum,
							 operCase->codeFilename);
			TreeCCStreamPrint(stream, "\t{");
			TreeCCStreamCodeIndent(stream, operCase->code, 1);
			TreeCCStreamPrint(stream, "}\n");
			TreeCCStreamFixLine(stream);
			TreeCCStreamPrint(stream, "}\n");
		}
		TreeCCStreamPrint(stream, "\n");
		virt = virt->next;
	}
}

/*
 * Implement a node type.
 */
static void ImplementNodeTypes(TreeCCContext *context, TreeCCNode *node)
{
	TreeCCStream *stream = node->source;

	/* Ignore if this is an enumerated type node */
	if((node->flags & (TREECC_NODE_ENUM | TREECC_NODE_ENUM_VALUE)) != 0)
	{
		return;
	}

	/* Implement the "new" and "delete" operators if this is a top-level node */
	if(!(node->parent) && !(context->reentrant))
	{
		TreeCCStreamPrint(stream, "void *%s::operator new(size_t size__)\n",
						  node->name);
		TreeCCStreamPrint(stream, "{\n");
		TreeCCStreamPrint(stream, "\treturn %s::getState()->alloc(size__);\n",
						  context->state_type);
		TreeCCStreamPrint(stream, "}\n\n");
		TreeCCStreamPrint(stream,
				"void %s::operator delete(void *ptr__, size_t size__)\n",
						  node->name);
		TreeCCStreamPrint(stream, "{\n");
		TreeCCStreamPrint(stream, "\t%s::getState()->dealloc(ptr__, size__);\n",
						  context->state_type);
		TreeCCStreamPrint(stream, "}\n\n");
	}

	/* Implement the constructor */
	ImplementConstructor(context, stream, node);

	/* Implement the destructor */
	TreeCCStreamPrint(stream, "%s::~%s()\n", node->name, node->name);
	TreeCCStreamPrint(stream, "{\n");
	TreeCCStreamPrint(stream, "\t// not used\n");
	TreeCCStreamPrint(stream, "}\n\n");

	/* Implement the virtual methods that reside in this node type */
	ImplementVirtuals(context, stream, node, node);

	/* Implement the "isA" helper method */
	TreeCCStreamPrint(stream, "int %s::isA(int kind) const\n", node->name);
	TreeCCStreamPrint(stream, "{\n");
	TreeCCStreamPrint(stream, "\tif(kind == %s_kind)\n", node->name);
	TreeCCStreamPrint(stream, "\t\treturn 1;\n");
	TreeCCStreamPrint(stream, "\telse\n");
	if(node->parent)
	{
		TreeCCStreamPrint(stream, "\t\treturn %s::isA(kind);\n",
						  node->parent->name);
	}
	else
	{
		TreeCCStreamPrint(stream, "\t\treturn 0;\n");
	}
	TreeCCStreamPrint(stream, "}\n\n");

	/* Implement the "getKindName" helper method */
	TreeCCStreamPrint(stream, "const char *%s::getKindName() const\n",
					  node->name);
	TreeCCStreamPrint(stream, "{\n");
	TreeCCStreamPrint(stream, "\treturn \"%s\";\n", node->name);
	TreeCCStreamPrint(stream, "}\n\n");
}

/*
 * Declare the parameters for a factory method in the state type.
 */
static int FactoryCreateParams(TreeCCContext *context, TreeCCStream *stream,
							   TreeCCNode *node, int needComma)
{
	TreeCCField *field;
	if(node->parent)
	{
		needComma = FactoryCreateParams(context, stream,
										node->parent, needComma);
	}
	field = node->fields;
	while(field != 0)
	{
		if((field->flags & TREECC_FIELD_NOCREATE) == 0)
		{
			if(needComma)
			{
				TreeCCStreamPrint(stream, ", ");
			}
			TreeCCStreamPrint(stream, "%s %s", field->type, field->name);
			needComma = 1;
		}
		field = field->next;
	}
	return needComma;
}

/*
 * Declare the create function for a node type.
 */
static void DeclareCreateFuncs(TreeCCContext *context, TreeCCNode *node)
{
	TreeCCStream *stream;

	/* Ignore if this is an enumerated type node */
	if((node->flags & (TREECC_NODE_ENUM | TREECC_NODE_ENUM_VALUE)) != 0)
	{
		return;
	}

	/* Ignore this if it is an abstract node */
	if((node->flags & TREECC_NODE_ABSTRACT) != 0)
	{
		return;
	}

	/* Determine which stream to write to */
	if(context->commonHeader)
	{
		stream = context->commonHeader;
	}
	else
	{
		stream = context->headerStream;
	}

	/* Output the start of the function definition */
	if(context->virtual_factory || context->abstract_factory)
	{
		TreeCCStreamPrint(stream, "\tvirtual %s *%sCreate(",
						  node->name, node->name);
	}
	else
	{
		TreeCCStreamPrint(stream, "\t%s *%sCreate(",
						  node->name, node->name);
	}

	/* Output the parameters for the create function */
	FactoryCreateParams(context, stream, node, 0);

	/* Terminate the function definition */
	if(context->abstract_factory)
	{
		TreeCCStreamPrint(stream, ") = 0;\n");
	}
	else
	{
		TreeCCStreamPrint(stream, ");\n");
	}
}

/*
 * Declare the state type in the header stream.
 */
static void DeclareStateType(TreeCCContext *context, TreeCCStream *stream)
{
	/* Declare the class header */
	TreeCCStreamPrint(stream, "class %s\n{\n", context->state_type);

	/* Declare the constructor and destructor */
	TreeCCStreamPrint(stream, "public:\n\n");
	TreeCCStreamPrint(stream, "\t%s();\n", context->state_type);
	TreeCCStreamPrint(stream, "\tvirtual ~%s();\n\n", context->state_type);

	/* Include the header skeleton */
	if(context->use_gc_allocator)
	{
		TreeCCIncludeSkeleton(context, stream, "cpp_gc_skel.h");
	}
	else
	{
		TreeCCIncludeSkeleton(context, stream, "cpp_skel.h");
	}

	/* Singleton handling for non-reentrant systems */
	if(!(context->reentrant))
	{
		TreeCCStreamPrint(stream, "private:\n\n");
		TreeCCStreamPrint(stream, "\tstatic %s *state__;\n\n",
						  context->state_type);
		TreeCCStreamPrint(stream, "public:\n\n");
		TreeCCStreamPrint(stream, "\tstatic %s *getState()\n",
						  context->state_type);
		TreeCCStreamPrint(stream, "\t\t{\n");
		TreeCCStreamPrint(stream, "\t\t\tif(state__) return state__;\n");
		TreeCCStreamPrint(stream, "\t\t\tstate__ = new %s();\n",
						  context->state_type);
		TreeCCStreamPrint(stream, "\t\t\treturn state__;\n");
		TreeCCStreamPrint(stream, "\t\t}\n\n");
	}

	/* Declare the create functions for all of the node types */
	if(context->reentrant)
	{
		TreeCCStreamPrint(stream, "public:\n\n");
		TreeCCNodeVisitAll(context, DeclareCreateFuncs);
		TreeCCStreamPrint(stream, "\n");
	}

	/* Declare the node pool handling functions */
	TreeCCStreamPrint(stream, "public:\n\n");
	TreeCCStreamPrint(stream, "\tvoid *alloc(size_t);\n");
	TreeCCStreamPrint(stream, "\tvoid dealloc(void *, size_t);\n");
	TreeCCStreamPrint(stream, "\tint push();\n");
	TreeCCStreamPrint(stream, "\tvoid pop();\n");
	TreeCCStreamPrint(stream, "\tvoid clear();\n");

	/* Declare the "failed" method for out of memory error reporting */
	TreeCCStreamPrint(stream, "\tvirtual void failed();\n");

	/* Declare the line number tracking methods */
	if(context->track_lines)
	{
		TreeCCStreamPrint(stream, "\tvirtual char *currFilename();\n");
		TreeCCStreamPrint(stream, "\tvirtual long currLinenum();\n");
	}

	/* Declare the end of the state type */
	TreeCCStreamPrint(stream, "\n};\n\n");
}

/*
 * Output invocation parameters for a call to a constructor
 * from within a factory method.
 */
static int FactoryInvokeParams(TreeCCContext *context, TreeCCStream *stream,
							   TreeCCNode *node, int needComma)
{
	TreeCCField *field;
	if(node->parent)
	{
		needComma = FactoryInvokeParams(context, stream,
										node->parent, needComma);
	}
	field = node->fields;
	while(field != 0)
	{
		if((field->flags & TREECC_FIELD_NOCREATE) == 0)
		{
			if(needComma)
			{
				TreeCCStreamPrint(stream, ", ");
			}
			TreeCCStreamPrint(stream, "%s", field->name);
			needComma = 1;
		}
		field = field->next;
	}
	return needComma;
}

/*
 * Implement the create function for a node type.
 */
static void ImplementCreateFuncs(TreeCCContext *context, TreeCCNode *node)
{
	TreeCCStream *stream;

	/* Ignore if this is an enumerated type node */
	if((node->flags & (TREECC_NODE_ENUM | TREECC_NODE_ENUM_VALUE)) != 0)
	{
		return;
	}

	/* Ignore this if it is an abstract node */
	if((node->flags & TREECC_NODE_ABSTRACT) != 0)
	{
		return;
	}

	/* Determine which stream to write to */
	if(context->commonSource)
	{
		stream = context->commonSource;
	}
	else
	{
		stream = context->sourceStream;
	}

	/* Output the start of the function definition */
	TreeCCStreamPrint(stream, "%s *%s::%sCreate(",
					  node->name, context->state_type, node->name);

	/* Output the parameters for the create function */
	FactoryCreateParams(context, stream, node, 0);
	TreeCCStreamPrint(stream, ")\n");

	/* Output the body of the creation function */
	TreeCCStreamPrint(stream, "{\n");
	TreeCCStreamPrint(stream, "\tvoid *buf__ = this->alloc(sizeof(%s));\n",
					  node->name);

	/* Bail out to the caller if "alloc" returned NULL */
	TreeCCStreamPrint(stream, "\tif(buf__ == 0) return 0;\n");

	/* Invoke the constructor and return the node to the caller */
	TreeCCStreamPrint(stream, "\treturn new (buf__) %s(this", node->name);
	FactoryInvokeParams(context, stream, node, 1);
	TreeCCStreamPrint(stream, ");\n");
	TreeCCStreamPrint(stream, "}\n\n");
}

/*
 * Implement the state type in the source stream.
 */
static void ImplementStateType(TreeCCContext *context, TreeCCStream *stream)
{
	/* Include the common definitions from the skeleton */
	if(context->block_size)
	{
		TreeCCStreamPrint(stream, "#define %s_BLKSIZ %d\n",
						  context->state_type, context->block_size);
	}
	if(context->reentrant)
	{
		TreeCCStreamPrint(stream, "#define %s_REENTRANT 1\n",
						  context->state_type);
	}
	if(context->track_lines)
	{
		TreeCCStreamPrint(stream, "#define %s_TRACK_LINES 1\n",
						  context->state_type);
	}
	if(context->use_allocator)
	{
		TreeCCStreamPrint(stream, "#define %s_USE_ALLOCATOR 1\n",
						  context->state_type);
	}
	if(context->use_gc_allocator)
	{
		TreeCCIncludeSkeleton(context, stream, "cpp_gc_skel.cc");
	}
	else
	{
		TreeCCIncludeSkeleton(context, stream, "cpp_skel.cc");
	}

	/* Implement the create functions for all of the node types */
	if(context->reentrant && !(context->abstract_factory))
	{
		TreeCCNodeVisitAll(context, ImplementCreateFuncs);
	}
}

/*
 * Write out header information for all streams.
 */
static void WriteCPPHeaders(TreeCCContext *context)
{
	TreeCCStream *stream = context->streamList;
	while(stream != 0)
	{
		if(stream->isHeader)
		{
			TreeCCStreamHeaderTop(stream);
			TreeCCStreamPrint(stream, "\n");
			TreeCCStreamPrint(stream, "#include <new>\n");
			TreeCCStreamPrint(stream, "\n");
		}
		else
		{
			TreeCCStreamSourceTop(stream);
			TreeCCStreamPrint(stream, "\n");
		}
		if(context->namespace)
		{
			TreeCCStreamPrint(stream, "namespace %s\n{\n\n",
					context->namespace);
		}
		if(stream->defaultFile)
		{
			/* Reset the dirty flag if this is a default stream,
			   because we don't want to write out the final file
			   if it isn't actually written to in practice */
			stream->dirty = 0;
		}
		stream = stream->nextStream;
	}
}

/*
 * Write out footer information for all streams.
 */
static void WriteCPPFooters(TreeCCContext *context)
{
	TreeCCStream *stream = context->streamList;
	while(stream != 0)
	{
		if(stream->defaultFile && !(stream->dirty))
		{
			/* Clear the default file's contents, which we don't need */
			TreeCCStreamClear(stream);
		}
		else
		{
			if(context->namespace)
			{
				TreeCCStreamPrint(stream, "}\n");
			}
			if(stream->isHeader)
			{
				TreeCCStreamHeaderBottom(stream);
			}
			else
			{
				TreeCCStreamSourceBottom(stream);
			}
		}
		stream = stream->nextStream;
	}
}

void TreeCCGenerateCPP(TreeCCContext *context)
{
	/* Write all stream headers */
	WriteCPPHeaders(context);

	/* Generate the contents of the header stream */
	TreeCCNodeVisitAll(context, DefineNodeNumbers);
	TreeCCStreamPrint(context->headerStream, "\n");
	TreeCCNodeVisitAll(context, DeclareTypeDefs);
	TreeCCStreamPrint(context->headerStream, "\n");
	if(context->commonHeader)
	{
		DeclareStateType(context, context->commonHeader);
	}
	else
	{
		DeclareStateType(context, context->headerStream);
	}
	TreeCCNodeVisitAll(context, BuildTypeDecls);
	TreeCCStreamPrint(context->headerStream, "\n");
	TreeCCOperationVisitAll(context, DeclareNonVirtuals);
	TreeCCStreamPrint(context->headerStream, "\n");

	/* Implement the state type */
	if(context->commonSource)
	{
		ImplementStateType(context, context->commonSource);
	}
	else
	{
		ImplementStateType(context, context->sourceStream);
	}

	/* Generate the contents of the source stream */
	TreeCCNodeVisitAll(context, ImplementNodeTypes);
	TreeCCGenerateNonVirtuals(context, &TreeCCNonVirtualFuncsC);

	/* Write all stream footers */
	WriteCPPFooters(context);
}

#ifdef	__cplusplus
};
#endif
