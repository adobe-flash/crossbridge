/*
 * gen_c.c - Generate C source code from "treecc" input files.
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
		TreeCCStreamPrint(stream, "#define %s_kind %d\n",
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
	if(node->parent)
	{
		DeclareFields(context, stream, node->parent);
	}
	field = node->fields;
	while(field != 0)
	{
		TreeCCStreamPrint(stream, "\t%s %s;\n",
						  field->type, field->name);
		field = field->next;
	}
}

/*
 * Declare the virtuals for a node type.
 */
static void DeclareVirtuals(TreeCCContext *context,
							TreeCCStream *stream, TreeCCNode *node)
{
	TreeCCVirtual *virt;
	TreeCCParam *param;
	int num;
	if(node->parent)
	{
		DeclareVirtuals(context, stream, node->parent);
	}
	virt = node->virtuals;
	while(virt != 0)
	{
		TreeCCStreamPrint(stream, "\t%s (*%s_v__)(%s *this__",
						  virt->returnType, virt->name, node->name);
		param = virt->params;
		num = 1;
		while(param != 0)
		{
			if(param->name)
			{
				TreeCCStreamPrint(stream, ", %s %s",
								  param->type, param->name);
			}
			else
			{
				TreeCCStreamPrint(stream, ", %s P%d__",
								  param->type, num);
				++num;
			}
			param = param->next;
		}
		TreeCCStreamPrint(stream, ");\n");
		virt = virt->next;
	}
}

/*
 * Declare the macros for calling the virtuals for a node type.
 */
static void DeclareVirtualMacros(TreeCCContext *context,
								 TreeCCStream *stream, TreeCCNode *node)
{
	TreeCCVirtual *virt;
	TreeCCParam *param;
	int num;
	virt = node->virtuals;
	while(virt != 0)
	{
		TreeCCStreamPrint(stream, "#define %s(this__", virt->name);
		param = virt->params;
		num = 1;
		while(param != 0)
		{
			if(param->name)
			{
				TreeCCStreamPrint(stream, ",%s", param->name);
			}
			else
			{
				TreeCCStreamPrint(stream, ",P%d__", num);
				++num;
			}
			param = param->next;
		}
		TreeCCStreamPrint(stream, ") \\\n");
		TreeCCStreamPrint(stream,
			"\t((*(((struct %s_vtable__ *)((this__)->vtable__))->%s_v__)) \\\n",
			node->name, virt->name);
		TreeCCStreamPrint(stream, "\t\t((%s *)(this__)", node->name);
		param = virt->params;
		num = 1;
		while(param != 0)
		{
			if(param->name)
			{
				TreeCCStreamPrint(stream, ", (%s)", param->name);
			}
			else
			{
				TreeCCStreamPrint(stream, ", (P%d__)", num);
				++num;
			}
			param = param->next;
		}
		TreeCCStreamPrint(stream, "))\n\n");
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
		char const *sep = "\n";
		TreeCCNode *child;
		TreeCCStreamPrint(stream, "typedef enum {");
		child = node->firstChild;
		while(child != 0)
		{
			if((child->flags & TREECC_NODE_ENUM_VALUE) != 0)
			{
				TreeCCStreamPrint(stream, "%s\t%s", sep, child->name);
				sep = ",\n";
			}
			child = child->nextSibling;
		}
		TreeCCStreamPrint(stream, "\n} %s;\n\n", node->name);
	}
	else if((node->flags & TREECC_NODE_ENUM_VALUE) == 0)
	{
		/* Define a regular node type */
		TreeCCStreamPrint(stream, "typedef struct %s__ %s;\n",
						  node->name, node->name);
	}
}

/*
 * Declare the virtual method implementation functions for a node type.
 */
static int DeclareVirtualImpls(TreeCCContext *context, TreeCCStream *stream,
							   TreeCCNode *node, TreeCCNode *actualNode)
{
	TreeCCVirtual *virt;
	TreeCCParam *param;
	TreeCCOperationCase *operCase;
	TreeCCOperation *oper;
	int num;
	int needComma;
	int haveVirts = 0;
	if(node->parent)
	{
		haveVirts = DeclareVirtualImpls(context, stream,
										node->parent, actualNode);
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
		TreeCCStreamPrint(stream, "extern %s %s_%s__(",
						  virt->returnType, actualNode->name, virt->name);
		oper = operCase->oper;
		param = oper->params;
		needComma = 0;
		num = 1;
		while(param != 0)
		{
			if(needComma)
			{
				TreeCCStreamPrint(stream, ", ");
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
			}
			else
			{
				/* First argument must be declared with the actual type */
				if(param->name)
				{
					TreeCCStreamPrint(stream, "%s *%s",
									  actualNode->name, param->name);
				}
				else
				{
					TreeCCStreamPrint(stream, "%s *P%d__",
									  actualNode->name, num);
					++num;
				}
			}
			needComma = 1;
			param = param->next;
		}
		TreeCCStreamPrint(stream, ");\n");
		haveVirts = 1;
		virt = virt->next;
	}
	return haveVirts;
}

/*
 * Build the type declarations for a node type.
 */
static void BuildTypeDecls(TreeCCContext *context,
						   TreeCCNode *node)
{
	TreeCCStream *stream = node->header;

	/* Ignore if this is an enumerated type node */
	if((node->flags & (TREECC_NODE_ENUM | TREECC_NODE_ENUM_VALUE)) != 0)
	{
		return;
	}

	/* Output the structure header */
	TreeCCStreamPrint(stream, "struct %s__ {\n", node->name);

	/* Declare the vtable member variable */
	TreeCCStreamPrint(stream, "\tconst struct %s_vtable__ *vtable__;\n",
					  node->name);

	/* Declare the node kind member variable */
	if(!(context->kind_in_vtable))
	{
		TreeCCStreamPrint(stream, "\tint kind__;\n");
	}

	/* Declare the filename and linenum fields if we are tracking lines */
	if(context->track_lines)
	{
		TreeCCStreamPrint(stream, "\tchar *filename__;\n");
		TreeCCStreamPrint(stream, "\tlong linenum__;\n");
	}

	/* Declare the fields */
	DeclareFields(context, stream, node);

	/* Output the structure footer */
	TreeCCStreamPrint(stream, "};\n\n");

	/* Output the vtable type header */
	TreeCCStreamPrint(stream, "struct %s_vtable__ {\n", node->name);

	/* Declare standard vtable members */
	if(node->parent)
	{
		TreeCCStreamPrint(stream, "\tconst struct %s_vtable__ *parent__;\n",
						  node->parent->name);
	}
	else
	{
		TreeCCStreamPrint(stream, "\tconst void *parent__;\n");
	}
	TreeCCStreamPrint(stream, "\tint kind__;\n");
	TreeCCStreamPrint(stream, "\tconst char *name__;\n");

	/* Declare the vtable function prototypes */
	DeclareVirtuals(context, stream, node);

	/* Output the vtable footer */
	TreeCCStreamPrint(stream, "};\n\n");

	/* Declare the actual vtable for this node type */
	TreeCCStreamPrint(stream, "extern struct %s_vtable__ const %s_vt__;\n\n",
					  node->name, node->name);

	/* Declare the macros for calling the virtuals in this node type */
	DeclareVirtualMacros(context, stream, node);

	/* Declare the functions that implement all virtuals in this node type */
	if(DeclareVirtualImpls(context, stream, node, node))
	{
		TreeCCStreamPrint(stream, "\n");
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
			TreeCCStreamPrint(stream, "%s %s",
							  field->type, field->name);
			needComma = 1;
		}
		field = field->next;
	}
	return needComma;
}

/*
 * Get the name of the "%typedef" type for a node.
 */
static const char *TypedefName(TreeCCNode *node)
{
	while(node->parent != 0 && (node->flags & TREECC_NODE_TYPEDEF) == 0)
	{
		node = node->parent;
	}
	return node->name;
}

/*
 * Declare the prototypes for the node creation functions.
 */
static void DeclareCreateFuncs(TreeCCContext *context,
						  	   TreeCCNode *node)
{
	TreeCCStream *stream = node->header;
	int needComma;

	/* Don't need the create function if this node is abstract or enumerated */
	if((node->flags & TREECC_NODE_ABSTRACT) != 0 ||
	   (node->flags & (TREECC_NODE_ENUM | TREECC_NODE_ENUM_VALUE)) != 0)
	{
		return;
	}

	/* Output the return type and name */
	TreeCCStreamPrint(stream, "extern %s *%s_create(",
					  TypedefName(node), node->name);

	/* Add the state type if building a re-entrant compiler */
	if(context->reentrant)
	{
		TreeCCStreamPrint(stream, "%s *state__", context->state_type);
		needComma = 1;
	}
	else
	{
		needComma = 0;
	}

	/* Output the field types and names as the create parameters */
	needComma = CreateParams(context, stream, node, needComma);
	if(!needComma)
	{
		TreeCCStreamPrint(stream, "void");
	}

	/* Terminate the declaration */
	TreeCCStreamPrint(stream, ");\n");
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

	/* Output the prototype */
	TreeCCStreamPrint(stream, "extern %s %s(",
					  oper->returnType, oper->name);
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
		param = param->next;
	}
	if(!needComma)
	{
		TreeCCStreamPrint(stream, "void");
	}
	TreeCCStreamPrint(stream, ");\n");
}

/*
 * Output helper macros to the header stream.
 */
static void OutputHelpers(TreeCCContext *context)
{
	TreeCCStream *stream;

	/* Determine which stream to write to */
	if(context->commonHeader)
	{
		stream = context->commonHeader;
	}
	else
	{
		stream = context->headerStream;
	}

	/* yykind macro */
	TreeCCStreamPrint(stream, "#ifndef %skind\n", context->yy_replacement);
	if(context->kind_in_vtable)
	{
		TreeCCStreamPrint(stream,
				"#define %skind(node__) ((node__)->vtable__->kind__)\n",
				context->yy_replacement);
	}
	else
	{
		TreeCCStreamPrint(stream,
				"#define %skind(node__) ((node__)->kind__)\n",
				context->yy_replacement);
	}
	TreeCCStreamPrint(stream, "#endif\n\n");

	/* yykindname macro */
	TreeCCStreamPrint(stream, "#ifndef %skindname\n", context->yy_replacement);
	TreeCCStreamPrint(stream,
			"#define %skindname(node__) ((node__)->vtable__->name__)\n",
			context->yy_replacement);
	TreeCCStreamPrint(stream, "#endif\n\n");

	/* yykindof macro */
	TreeCCStreamPrint(stream, "#ifndef %skindof\n", context->yy_replacement);
	TreeCCStreamPrint(stream,
			"#define %skindof(type__) (type__##_kind)\n",
			context->yy_replacement);
	TreeCCStreamPrint(stream, "#endif\n\n");

	/* yyisa macro */
	TreeCCStreamPrint(stream, "#ifndef %sisa\n", context->yy_replacement);
	TreeCCStreamPrint(stream,
			"extern int %sisa__(const void *vtable__, int kind__);\n",
			context->yy_replacement);
	TreeCCStreamPrint(stream,
			"#define %sisa(node__,type__) \\\n",
   			context->yy_replacement);
	TreeCCStreamPrint(stream,
		"\t(%sisa__((node__)->vtable__, (type__##_kind)))\n",
		context->yy_replacement);
	TreeCCStreamPrint(stream, "#endif\n\n");

	/* Are we tracking line numbers? */
	if(context->track_lines)
	{
		/* yygetfilename macro */
		TreeCCStreamPrint(stream,
				"#ifndef %sgetfilename\n", context->yy_replacement);
		TreeCCStreamPrint(stream,
				"#define %sgetfilename(node__) ((node__)->filename__)\n",
				context->yy_replacement);
		TreeCCStreamPrint(stream, "#endif\n\n");

		/* yygetlinenum macro */
		TreeCCStreamPrint(stream,
				"#ifndef %sgetlinenum\n", context->yy_replacement);
		TreeCCStreamPrint(stream,
				"#define %sgetlinenum(node__) ((node__)->linenum__)\n",
				context->yy_replacement);
		TreeCCStreamPrint(stream, "#endif\n\n");

		/* yysetfilename macro */
		TreeCCStreamPrint(stream,
				"#ifndef %ssetfilename\n", context->yy_replacement);
		TreeCCStreamPrint(stream,
				"#define %ssetfilename(node__, value__) \\\n",
				context->yy_replacement);
		TreeCCStreamPrint(stream,
				"\t((node__)->filename__ = (value__))\n");
		TreeCCStreamPrint(stream, "#endif\n\n");

		/* yysetlinenum macro */
		TreeCCStreamPrint(stream,
				"#ifndef %ssetlinenum\n", context->yy_replacement);
		TreeCCStreamPrint(stream,
				"#define %ssetlinenum(node__, value__) \\\n",
				context->yy_replacement);
		TreeCCStreamPrint(stream,
				"\t((node__)->linenum__ = (value__))\n");
		TreeCCStreamPrint(stream, "#endif\n\n");

		/* yycurrfilename and yycurrlinenum functions */
		TreeCCStreamPrint(stream, "#ifndef %stracklines_declared\n",
						  context->yy_replacement);
		TreeCCStreamPrint(stream, "extern char *%scurrfilename(",
						  context->yy_replacement);
		if(context->reentrant)
		{
			TreeCCStreamPrint(stream, "%s *state__", context->state_type);
		}
		else
		{
			TreeCCStreamPrint(stream, "void");
		}
		TreeCCStreamPrint(stream, ");\n");
		TreeCCStreamPrint(stream, "extern long %scurrlinenum(",
						  context->yy_replacement);
		if(context->reentrant)
		{
			TreeCCStreamPrint(stream, "%s *state__", context->state_type);
		}
		else
		{
			TreeCCStreamPrint(stream, "void");
		}
		TreeCCStreamPrint(stream, ");\n");
		TreeCCStreamPrint(stream, "#define %stracklines_declared 1\n",
						  context->yy_replacement);
		TreeCCStreamPrint(stream, "#endif\n\n");
	}

	/* Declare the "yynodealloc", "yynodepush", "yynodepop", and
	   "yynodeclear", and "yynodefailed" functions */
	TreeCCStreamPrint(stream, "#ifndef %snodeops_declared\n",
					  context->yy_replacement);
	TreeCCStreamPrint(stream, "extern void %snodeinit(",
					  context->yy_replacement);
	if(context->reentrant)
	{
		TreeCCStreamPrint(stream, "%s *state__", context->state_type);
	}
	else
	{
		TreeCCStreamPrint(stream, "void");
	}
	TreeCCStreamPrint(stream, ");\n");
	TreeCCStreamPrint(stream, "extern void *%snodealloc(",
					  context->yy_replacement);
	if(context->reentrant)
	{
		TreeCCStreamPrint(stream, "%s *state__, ", context->state_type);
	}
	TreeCCStreamPrint(stream, "unsigned int size__");
	TreeCCStreamPrint(stream, ");\n");
	TreeCCStreamPrint(stream, "extern int %snodepush(",
					  context->yy_replacement);
	if(context->reentrant)
	{
		TreeCCStreamPrint(stream, "%s *state__", context->state_type);
	}
	else
	{
		TreeCCStreamPrint(stream, "void");
	}
	TreeCCStreamPrint(stream, ");\n");
	TreeCCStreamPrint(stream, "extern void %snodepop(",
					  context->yy_replacement);
	if(context->reentrant)
	{
		TreeCCStreamPrint(stream, "%s *state__", context->state_type);
	}
	else
	{
		TreeCCStreamPrint(stream, "void");
	}
	TreeCCStreamPrint(stream, ");\n");
	TreeCCStreamPrint(stream, "extern void %snodeclear(",
					  context->yy_replacement);
	if(context->reentrant)
	{
		TreeCCStreamPrint(stream, "%s *state__", context->state_type);
	}
	else
	{
		TreeCCStreamPrint(stream, "void");
	}
	TreeCCStreamPrint(stream, ");\n");
	TreeCCStreamPrint(stream, "extern void %snodefailed(",
					  context->yy_replacement);
	if(context->reentrant)
	{
		TreeCCStreamPrint(stream, "%s *state__", context->state_type);
	}
	else
	{
		TreeCCStreamPrint(stream, "void");
	}
	TreeCCStreamPrint(stream, ");\n");
	TreeCCStreamPrint(stream, "#define %snodeops_declared 1\n",
					  context->yy_replacement);
	TreeCCStreamPrint(stream, "#endif\n\n");
}

/*
 * Output the vtable function implementations for a node type.
 */
static void OutputVtableImpls(TreeCCContext *context, TreeCCStream *stream,
							  TreeCCNode *node, TreeCCNode *actualNode)
{
	TreeCCVirtual *virt;
	TreeCCParam *param;
	TreeCCOperationCase *operCase;
	TreeCCOperation *oper;
	int num;
	int needComma;
	if(node->parent)
	{
		OutputVtableImpls(context, stream, node->parent, actualNode);
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
		TreeCCStreamPrint(stream, "%s %s_%s__(",
						  virt->returnType, actualNode->name, virt->name);
		oper = operCase->oper;
		param = oper->params;
		needComma = 0;
		num = 1;
		while(param != 0)
		{
			if(needComma)
			{
				TreeCCStreamPrint(stream, ", ");
			}
			if(!needComma)
			{
				/* First argument must be declared with the actual type */
				if(param->name)
				{
					TreeCCStreamPrint(stream, "%s *%s",
									  actualNode->name, param->name);
				}
				else
				{
					TreeCCStreamPrint(stream, "%s *P%d__",
									  actualNode->name, num);
					++num;
				}
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
			}
			needComma = 1;
			param = param->next;
		}
		TreeCCStreamPrint(stream, ")\n");
		TreeCCStreamLine(stream, operCase->codeLinenum, operCase->codeFilename);
		TreeCCStreamPrint(stream, "{");
		TreeCCStreamCode(stream, operCase->code);
		TreeCCStreamPrint(stream, "}\n");
		TreeCCStreamFixLine(stream);
		TreeCCStreamPrint(stream, "\n");
		virt = virt->next;
	}
}

/*
 * Output the vtable function references for a node type.
 */
static void OutputVtableFuncs(TreeCCContext *context, TreeCCStream *stream,
							  TreeCCNode *node, TreeCCNode *actualNode)
{
	TreeCCVirtual *virt;
	TreeCCParam *param;
	TreeCCOperationCase *operCase;
	TreeCCNode *parent;
	int num;
	if(node->parent)
	{
		OutputVtableFuncs(context, stream, node->parent, actualNode);
	}
	virt = node->virtuals;
	while(virt != 0)
	{
		TreeCCStreamPrint(stream, "\t(%s (*)(%s *this__",
						  virt->returnType, node->name);
		param = virt->params;
		num = 1;
		while(param != 0)
		{
			if(param->name)
			{
				TreeCCStreamPrint(stream, ", %s %s",
							      param->type, param->name);
			}
			else
			{
				TreeCCStreamPrint(stream, ", %s P%d__",
							 	  param->type, num);
				++num;
			}
			param = param->next;
		}
		operCase = TreeCCOperationFindCase(context, actualNode, virt->name);
		parent = actualNode;
		while(!operCase)
		{
			parent = parent->parent;
			if(!parent)
			{
				break;
			}
			operCase = TreeCCOperationFindCase(context, parent, virt->name);
		}
		if(operCase)
		{
			/* We are inheriting the implementation from "parent" */
			TreeCCStreamPrint(stream, "))%s_%s__,\n",
							  parent->name, virt->name);
		}
		else
		{
			/* No implementation for the method at all */
			TreeCCStreamPrint(stream, "))0,\n");
		}
		virt = virt->next;
	}
}

/*
 * Define a vtable for a node type.
 */
static void DefineVtables(TreeCCContext *context,
						  TreeCCNode *node)
{
	TreeCCStream *stream = node->source;

	/* Ignore this node if it is an enumerated type */
	if((node->flags & (TREECC_NODE_ENUM | TREECC_NODE_ENUM_VALUE)) != 0)
	{
		return;
	}

	/* Output the implementations of the node vtable functions */
	OutputVtableImpls(context, stream, node, node);

	/* Output the vtable header */
	TreeCCStreamPrint(stream, "struct %s_vtable__ const %s_vt__ = {\n",
					  node->name, node->name);
	if(node->parent)
	{
		TreeCCStreamPrint(stream, "\t&%s_vt__,\n", node->parent->name);
	}
	else
	{
		TreeCCStreamPrint(stream, "\t0,\n");
	}
	TreeCCStreamPrint(stream, "\t%s_kind,\n", node->name);
	TreeCCStreamPrint(stream, "\t\"%s\",\n", node->name);

	/* Output the function references */
	OutputVtableFuncs(context, stream, node, node);

	/* Output the vtable footer */
	TreeCCStreamPrint(stream, "};\n\n");
}

/*
 * Output a create function parameter list to the source stream.
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
 * Initialize a node's fields within a create function.
 */
static void InitFields(TreeCCContext *context, TreeCCStream *stream,
					   TreeCCNode *node)
{
	TreeCCField *field;
	if(node->parent)
	{
		InitFields(context, stream, node->parent);
	}
	field = node->fields;
	while(field != 0)
	{
		if((field->flags & TREECC_FIELD_NOCREATE) == 0)
		{
			TreeCCStreamPrint(stream, "\tnode__->%s = %s;\n",
							  field->name, field->name);
		}
		else if(field->value)
		{
			TreeCCStreamPrint(stream, "\tnode__->%s = %s;\n",
							  field->name, field->value);
		}
		field = field->next;
	}
}

/*
 * Implement a create function for a node type.
 */
static void ImplementCreateFuncs(TreeCCContext *context,
						  		 TreeCCNode *node)
{
	TreeCCStream *stream = node->source;
	const char *typedefName;
	int needComma;

	/* Ignore this node if it is an enumerated type */
	if((node->flags & (TREECC_NODE_ENUM | TREECC_NODE_ENUM_VALUE)) != 0)
	{
		return;
	}

	/* Don't need a create function if this node type is abstract */
	if((node->flags & TREECC_NODE_ABSTRACT) != 0)
	{
		return;
	}

	/* Find the name of the "%typedef" node type for this node */
	typedefName = TypedefName(node);

	/* Output the create function header */
	TreeCCStreamPrint(stream, "%s *%s_create(", typedefName, node->name);
	if(context->reentrant)
	{
		TreeCCStreamPrint(stream, "%s *state__", context->state_type);
		needComma = 1;
	}
	else
	{
		needComma = 0;
	}
	needComma = CreateParamsSource(context, stream, node, needComma);
	if(!needComma)
	{
		TreeCCStreamPrint(stream, "void");
	}
	TreeCCStreamPrint(stream, ")\n");
	TreeCCStreamPrint(stream, "{\n");

	/* Is the node type a singleton? */
	if(!(context->track_lines) && !(context->no_singletons) &&
	   TreeCCNodeIsSingleton(node))
	{
		/* This node type is a singleton, so create
		   a static node instance and always return it */
		TreeCCStreamPrint(stream, "\tstatic struct %s__ instance__ = {\n",
						  node->name);
		TreeCCStreamPrint(stream, "\t\t&%s_vt__,\n", node->name);
		if(!(context->kind_in_vtable))
		{
			TreeCCStreamPrint(stream, "\t\t%s_kind\n", node->name);
		}
		TreeCCStreamPrint(stream, "\t};\n");
		TreeCCStreamPrint(stream, "\treturn (%s *)&instance__;\n",
						  typedefName);
	}
	else
	{
		/* Non-singleton node type */
		if(context->reentrant)
		{
			TreeCCStreamPrint(stream,
			  "\t%s *node__ = (%s *)%snodealloc(state__, "
			  			"sizeof(struct %s__));\n",
				node->name, node->name, context->yy_replacement, node->name);
		}
		else
		{
			TreeCCStreamPrint(stream,
				"\t%s *node__ = (%s *)%snodealloc(sizeof(struct %s__));\n",
				node->name, node->name, context->yy_replacement, node->name);
		}

		/* Bail out to the caller if "yynodealloc" returned NULL */
		TreeCCStreamPrint(stream, "\tif(node__ == 0) return 0;\n");

		/* Set the vtable and kind */
		TreeCCStreamPrint(stream, "\tnode__->vtable__ = &%s_vt__;\n",
						  node->name);
		if(!(context->kind_in_vtable))
		{
			TreeCCStreamPrint(stream, "\tnode__->kind__ = %s_kind;\n",
							  node->name);
		}

		/* Track the filename and line number if necessary */
		if(context->track_lines)
		{
			if(context->reentrant)
			{
				TreeCCStreamPrint(stream,
						"\tnode__->filename__ = %scurrfilename(state__);\n",
						context->yy_replacement);
				TreeCCStreamPrint(stream,
						"\tnode__->linenum__ = %scurrlinenum(state__);\n",
						context->yy_replacement);
			}
			else
			{
				TreeCCStreamPrint(stream,
						"\tnode__->filename__ = %scurrfilename();\n",
						context->yy_replacement);
				TreeCCStreamPrint(stream,
						"\tnode__->linenum__ = %scurrlinenum();\n",
						context->yy_replacement);
			}
		}

		/* Initialize the fields */
		InitFields(context, stream, node);

		/* Return the node to the caller */
		TreeCCStreamPrint(stream, "\treturn (%s *)node__;\n", typedefName);
	}

	/* Output the create function footer */
	TreeCCStreamPrint(stream, "}\n\n");
}

/*
 * Output the source code for the "yyisa__" helper function.
 */
static void OutputIsA(TreeCCContext *context)
{
	TreeCCStream *stream;

	/* Determine which stream to write to */
	if(context->commonSource)
	{
		stream = context->commonSource;
	}
	else
	{
		stream = context->sourceStream;
	}

	/* Declare a helper structure for walking vtable types */
	TreeCCStreamPrint(stream, "struct %s_vtable__ {\n",
					  context->yy_replacement);
	TreeCCStreamPrint(stream, "\tconst struct %s_vtable__ *parent__;\n",
					  context->yy_replacement);
	TreeCCStreamPrint(stream, "\tint kind__;\n");
	TreeCCStreamPrint(stream, "};\n\n");

	/* Output the function header */
	TreeCCStreamPrint(stream,
		"int %sisa__(const void *vtable__, int kind__)\n",
		context->yy_replacement);

	/* Output the body of the function */
	TreeCCStreamPrint(stream, "{\n");
	TreeCCStreamPrint(stream, "\tconst struct %s_vtable__ *vt;\n",
					  context->yy_replacement);
	TreeCCStreamPrint(stream, "\tvt = (const struct %s_vtable__ *)vtable__;\n",
					  context->yy_replacement);
	TreeCCStreamPrint(stream, "\twhile(vt != 0) {\n");
	TreeCCStreamPrint(stream, "\t\tif(vt->kind__ == kind__)\n");
	TreeCCStreamPrint(stream, "\t\t\treturn 1;\n");
	TreeCCStreamPrint(stream, "\t\tvt = vt->parent__;\n");
	TreeCCStreamPrint(stream, "\t}\n");
	TreeCCStreamPrint(stream, "\treturn 0;\n");
	TreeCCStreamPrint(stream, "}\n\n");
}

/*
 * Determine if a type name corresponds to an enumerated type.
 */
static int IsEnumType(TreeCCContext *context, const char *type)
{
	TreeCCNode *node = TreeCCNodeFindByType(context, type);
	if(node)
	{
		if((node->flags & TREECC_NODE_ENUM) != 0)
		{
			return 1;
		}
	}
	return 0;
}

/*
 * Generate the start declarations for a non-virtual operation.
 */
static void C_GenStart(TreeCCContext *context, TreeCCStream *stream,
					   TreeCCOperation *oper)
{
	/* Nothing to do here for C and C++ */
}

/*
 * Generate the entry point for a non-virtual operation.
 */
static void CGenEntry(TreeCCContext *context, TreeCCStream *stream,
					  TreeCCOperation *oper, int number)
{
	TreeCCParam *param;
	int num;
	int needComma;
	if(number != -1)
	{
		TreeCCStreamPrint(stream, "%s %s_split_%d__(",
						  oper->returnType, oper->name, number);
	}
	else if(context->language == TREECC_LANG_C || !(oper->className))
	{
		TreeCCStreamPrint(stream, "%s %s(",
						  oper->returnType, oper->name);
	}
	else
	{
		TreeCCStreamPrint(stream, "%s %s::%s(",
						  oper->returnType, oper->className, oper->name);
	}
	param = oper->params;
	num = 1;
	needComma = 0;
	while(param != 0)
	{
		if(needComma)
		{
			TreeCCStreamPrint(stream, ", ");
		}
		TreeCCStreamPrint(stream, "%s ", param->type);
		if(param->name)
		{
			TreeCCStreamPrint(stream, "%s", param->name);
		}
		else
		{
			TreeCCStreamPrint(stream, "P%d__", num);
			++num;
		}
		if((param->flags & TREECC_PARAM_TRIGGER) != 0)
		{
			if(!IsEnumType(context, param->type))
			{
				TreeCCStreamPrint(stream, "__");
			}
		}
		needComma = 1;
		param = param->next;
	}
	if(!needComma)
	{
		TreeCCStreamPrint(stream, "void");
	}
	TreeCCStreamPrint(stream, ")\n");
	TreeCCStreamPrint(stream, "{\n");
}

/*
 * Generate the entry point for a split-out function.
 */
static void C_GenSplitEntry(TreeCCContext *context, TreeCCStream *stream,
					        TreeCCOperation *oper, int number)
{
	CGenEntry(context, stream, oper, number);
}

/*
 * Generate the entry point for a non-virtual operation.
 */
static void C_GenEntry(TreeCCContext *context, TreeCCStream *stream,
					   TreeCCOperation *oper)
{
	CGenEntry(context, stream, oper, -1);
}

/*
 * Output TAB's for a specific level of indenting.
 */
static void Indent(TreeCCStream *stream, int indent)
{
	while(indent >= 4)
	{
		TreeCCStreamPrint(stream, "\t\t\t\t");
		indent -= 4;
	}
	if(indent == 1)
	{
		TreeCCStreamPrint(stream, "\t");
	}
	else if(indent == 2)
	{
		TreeCCStreamPrint(stream, "\t\t");
	}
	else if(indent == 3)
	{
		TreeCCStreamPrint(stream, "\t\t\t");
	}
}

/*
 * Generate the head of a "switch" statement.
 */
static void C_GenSwitchHead(TreeCCContext *context, TreeCCStream *stream,
							char *paramName, int level, int isEnum)
{
	Indent(stream, level * 2 + 1);
	if(isEnum)
	{
		TreeCCStreamPrint(stream, "switch(%s)\n", paramName);
	}
	else if(context->language == TREECC_LANG_C)
	{
		if(context->kind_in_vtable)
		{
			TreeCCStreamPrint(stream, "switch(%s__->vtable__->kind__)\n",
							  paramName);
		}
		else
		{
			TreeCCStreamPrint(stream, "switch(%s__->kind__)\n", paramName);
		}
	}
	else
	{
		TreeCCStreamPrint(stream, "switch(%s__->getKind())\n", paramName);
	}
	Indent(stream, level * 2 + 1);
	TreeCCStreamPrint(stream, "{\n");
}

/*
 * Generate a selector for a "switch" case.
 */
static void C_GenSelector(TreeCCContext *context, TreeCCStream *stream,
						  TreeCCNode *node, int level)
{
	if((node->flags & TREECC_NODE_ENUM_VALUE) != 0)
	{
		Indent(stream, level * 2 + 2);
		TreeCCStreamPrint(stream, "case %s:\n", node->name);
	}
	else if((node->flags & TREECC_NODE_ENUM) == 0)
	{
		Indent(stream, level * 2 + 2);
		TreeCCStreamPrint(stream, "case %s_kind:\n", node->name);
	}
}

/*
 * Terminate the selectors and begin the body of a "switch" case.
 */
static void C_GenEndSelectors(TreeCCContext *context, TreeCCStream *stream,
							  int level)
{
	Indent(stream, level * 2 + 2);
	TreeCCStreamPrint(stream, "{\n");
}

/*
 * Generate the code for a case within a function.
 */
static void C_GenCaseFunc(TreeCCContext *context, TreeCCStream *stream,
						  TreeCCOperationCase *operCase, int number)
{
	TreeCCParam *param;
	TreeCCTrigger *trigger;
	int num;
	int needComma;
	char *type;
	char *suffix;

	/* Output the header for the function */
	TreeCCStreamPrint(stream, "static %s %s_%d__(",
					  operCase->oper->returnType,
					  operCase->oper->name, number);
	param = operCase->oper->params;
	trigger = operCase->triggers;
	num = 1;
	needComma = 0;
	while(param != 0)
	{
		if(needComma)
		{
			TreeCCStreamPrint(stream, ", ");
		}
		if((param->flags & TREECC_PARAM_TRIGGER) != 0)
		{
		   	if((trigger->node->flags & TREECC_NODE_ENUM) == 0 &&
		   	   (trigger->node->flags & TREECC_NODE_ENUM_VALUE) == 0)
			{
				type = trigger->node->name;
				suffix = "*";
			}
			else
			{
				type = param->type;
				suffix = "";
			}
			trigger = trigger->next;
		}
		else
		{
			type = param->type;
			suffix = "";
		}
		if(param->name)
		{
			TreeCCStreamPrint(stream, "%s %s%s",
							  type, suffix, param->name);
		}
		else
		{
			TreeCCStreamPrint(stream, "%s %sP%d__",
							  type, suffix, num);
			++num;
		}
		needComma = 1;
		param = param->next;
	}
	if(!needComma)
	{
		TreeCCStreamPrint(stream, "void");
	}
	TreeCCStreamPrint(stream, ")\n");

	/* Output the code for the operation case */
	TreeCCStreamLine(stream, operCase->codeLinenum, operCase->codeFilename);
	TreeCCStreamPrint(stream, "{");
	if(operCase->code)
	{
		TreeCCStreamCode(stream, operCase->code);
	}
	TreeCCStreamPrint(stream, "}\n");
	TreeCCStreamFixLine(stream);
	TreeCCStreamPrint(stream, "\n");
}

/*
 * Generate a call to a case function from within the "switch".
 */
static void C_GenCaseCall(TreeCCContext *context, TreeCCStream *stream,
						  TreeCCOperationCase *operCase, int number,
						  int level)
{
	TreeCCParam *param;
	TreeCCTrigger *trigger;
	int num;
	int needComma;

	/* Indent to the correct level */
	Indent(stream, level * 2 + 3);

	/* Add "return" to the front if the operation is non-void */
	if(strcmp(operCase->oper->returnType, "void") != 0)
	{
		TreeCCStreamPrint(stream, "return ");
	}

	/* Print out the call */
	TreeCCStreamPrint(stream, "%s_%d__(", operCase->oper->name, number);
	param = operCase->oper->params;
	trigger = operCase->triggers;
	num = 1;
	needComma = 0;
	while(param != 0)
	{
		if(needComma)
		{
			TreeCCStreamPrint(stream, ", ");
		}
		if((param->flags & TREECC_PARAM_TRIGGER) != 0)
		{
			if((trigger->node->flags & TREECC_NODE_ENUM) == 0 &&
		   	   (trigger->node->flags & TREECC_NODE_ENUM_VALUE) == 0)
			{
				TreeCCStreamPrint(stream, "(%s *)", trigger->node->name);
			}
		}
		if(param->name)
		{
			TreeCCStreamPrint(stream, "%s", param->name);
		}
		else
		{
			TreeCCStreamPrint(stream, "P%d__", num);
			++num;
		}
		if((param->flags & TREECC_PARAM_TRIGGER) != 0)
		{
			if((trigger->node->flags & TREECC_NODE_ENUM) == 0 &&
		   	   (trigger->node->flags & TREECC_NODE_ENUM_VALUE) == 0)
			{
				TreeCCStreamPrint(stream, "__");
			}
			trigger = trigger->next;
		}
		needComma = 1;
		param = param->next;
	}
	TreeCCStreamPrint(stream, ");\n");
}

/*
 * Generate the code for a case inline within the "switch".
 */
static void C_GenCaseInline(TreeCCContext *context, TreeCCStream *stream,
						    TreeCCOperationCase *operCase, int level)
{
	TreeCCParam *param;
	TreeCCTrigger *trigger;

	/* Copy the parameters to new variables of the correct types */
	param = operCase->oper->params;
	trigger = operCase->triggers;
	while(param != 0)
	{
		if((param->flags & TREECC_PARAM_TRIGGER) != 0)
		{
			if(param->name != 0)
			{
				if((trigger->node->flags & TREECC_NODE_ENUM) == 0 &&
				   (trigger->node->flags & TREECC_NODE_ENUM_VALUE) == 0)
				{
					Indent(stream, level * 2 + 3);
					TreeCCStreamPrint(stream, "%s *%s = (%s *)%s__;\n",
									  trigger->node->name, param->name,
									  trigger->node->name, param->name);
				}
			}
			trigger = trigger->next;
		}
		param = param->next;
	}

	/* Output the inline code for the case */
	TreeCCStreamLine(stream, operCase->codeLinenum, operCase->codeFilename);
	Indent(stream, level * 2 + 3);
	TreeCCStreamPrint(stream, "{");
	if(operCase->code)
	{
		TreeCCStreamCodeIndent(stream, operCase->code, level * 2 + 3);
	}
	TreeCCStreamPrint(stream, "}\n");
	TreeCCStreamFixLine(stream);
}

/*
 * Generate the code for a call to a split function within the "switch".
 */
static void C_GenCaseSplit(TreeCCContext *context, TreeCCStream *stream,
						   TreeCCOperationCase *operCase,
						   int number, int level)
{
	TreeCCParam *param;
	TreeCCTrigger *trigger;
	int num;
	int needComma;

	/* Indent to the correct level */
	Indent(stream, level * 2 + 3);

	/* Add "return" to the front if the operation is non-void */
	if(strcmp(operCase->oper->returnType, "void") != 0)
	{
		TreeCCStreamPrint(stream, "return ");
	}

	/* Print out the call */
	TreeCCStreamPrint(stream, "%s_split_%d__(", operCase->oper->name, number);
	param = operCase->oper->params;
	trigger = operCase->triggers;
	num = 1;
	needComma = 0;
	while(param != 0)
	{
		if(needComma)
		{
			TreeCCStreamPrint(stream, ", ");
		}
		if((param->flags & TREECC_PARAM_TRIGGER) != 0)
		{
			if((trigger->node->flags & TREECC_NODE_ENUM) == 0 &&
		   	   (trigger->node->flags & TREECC_NODE_ENUM_VALUE) == 0)
			{
				TreeCCStreamPrint(stream, "(%s *)", trigger->node->name);
			}
		}
		if(param->name)
		{
			TreeCCStreamPrint(stream, "%s", param->name);
		}
		else
		{
			TreeCCStreamPrint(stream, "P%d__", num);
			++num;
		}
		if((param->flags & TREECC_PARAM_TRIGGER) != 0)
		{
			if((trigger->node->flags & TREECC_NODE_ENUM) == 0 &&
		   	   (trigger->node->flags & TREECC_NODE_ENUM_VALUE) == 0)
			{
				TreeCCStreamPrint(stream, "__");
			}
			trigger = trigger->next;
		}
		needComma = 1;
		param = param->next;
	}
	TreeCCStreamPrint(stream, ");\n");
}

/*
 * Terminate a "switch" case.
 */
static void C_GenEndCase(TreeCCContext *context, TreeCCStream *stream,
						 int level)
{
	Indent(stream, level * 2 + 2);
	TreeCCStreamPrint(stream, "}\n");
	Indent(stream, level * 2 + 2);
	TreeCCStreamPrint(stream, "break;\n\n");
}

/*
 * Terminate the "switch" statement.
 */
static void C_GenEndSwitch(TreeCCContext *context, TreeCCStream *stream,
						   int level)
{
	Indent(stream, level * 2 + 2);
	TreeCCStreamPrint(stream, "default: break;\n");
	Indent(stream, level * 2 + 1);
	TreeCCStreamPrint(stream, "}\n");
}

/*
 * Generate the exit point for a non-virtual operation.
 */
static void C_GenExit(TreeCCContext *context, TreeCCStream *stream,
					  TreeCCOperation *oper)
{
	if(strcmp(oper->returnType, "void") != 0)
	{
		/* Generate a default return value for the function */
		if(oper->defValue)
		{
			TreeCCStreamPrint(stream, "\treturn (%s);\n", oper->defValue);
		}
		else
		{
			TreeCCStreamPrint(stream, "\treturn 0;\n");
		}
	}
	TreeCCStreamPrint(stream, "}\n\n");
}

/*
 * Generate the end declarations for a non-virtual operation.
 */
static void C_GenEnd(TreeCCContext *context, TreeCCStream *stream,
					 TreeCCOperation *oper)
{
	/* Nothing to do here for C and C++ */
}

/*
 * Table of non-virtual code generation functions.
 */
TreeCCNonVirtual const TreeCCNonVirtualFuncsC = {
	C_GenStart,
	C_GenEntry,
	C_GenSplitEntry,
	C_GenSwitchHead,
	C_GenSelector,
	C_GenEndSelectors,
	C_GenCaseFunc,
	C_GenCaseCall,
	C_GenCaseInline,
	C_GenCaseSplit,
	C_GenEndCase,
	C_GenEndSwitch,
	C_GenExit,
	C_GenEnd,
};

/*
 * Write out header information for all streams.
 */
static void WriteHeaders(TreeCCContext *context)
{
	TreeCCStream *stream = context->streamList;
	while(stream != 0)
	{
		if(stream->isHeader)
		{
			TreeCCStreamHeaderTop(stream);
			TreeCCStreamPrint(stream, "\n");
			TreeCCStreamPrint(stream, "#ifdef __cplusplus\n");
			TreeCCStreamPrint(stream, "extern \"C\" {\n");
			TreeCCStreamPrint(stream, "#endif\n");
			TreeCCStreamPrint(stream, "\n");
		}
		else
		{
			TreeCCStreamSourceTop(stream);
			TreeCCStreamPrint(stream, "\n");
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
static void WriteFooters(TreeCCContext *context)
{
	TreeCCStream *stream = context->streamList;
	while(stream != 0)
	{
		if(stream->defaultFile && !(stream->dirty))
		{
			/* Clear the default file's contents, which we don't need */
			TreeCCStreamClear(stream);
		}
		else if(stream->isHeader)
		{
			TreeCCStreamPrint(stream, "#ifdef __cplusplus\n");
			TreeCCStreamPrint(stream, "};\n");
			TreeCCStreamPrint(stream, "#endif\n");
			TreeCCStreamPrint(stream, "\n");
			TreeCCStreamHeaderBottom(stream);
		}
		else
		{
			TreeCCStreamSourceBottom(stream);
		}
		stream = stream->nextStream;
	}
}

/*
 * Write the skeleton for the node memory manager to a source stream.
 */
static void WriteSourceSkeleton(TreeCCContext *context, TreeCCStream *stream)
{
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
	if(context->use_gc_allocator)
	{
		TreeCCIncludeSkeleton(context, stream, "c_gc_skel.c");
	}
	else
	{
		TreeCCIncludeSkeleton(context, stream, "c_skel.c");
	}
}

void TreeCCGenerateC(TreeCCContext *context)
{
	/* Write all stream headers */
	WriteHeaders(context);

	/* Generate the contents of the header stream */
	TreeCCNodeVisitAll(context, DefineNodeNumbers);
	TreeCCStreamPrint(context->headerStream, "\n");
	TreeCCNodeVisitAll(context, DeclareTypeDefs);
	TreeCCStreamPrint(context->headerStream, "\n");
	if(context->use_gc_allocator)
	{
		if(context->commonHeader)
		{
			TreeCCIncludeSkeleton
				(context, context->commonHeader, "c_gc_skel.h");
		}
		else
		{
			TreeCCIncludeSkeleton
				(context, context->headerStream, "c_gc_skel.h");
		}
	}
	else
	{
		if(context->commonHeader)
		{
			TreeCCIncludeSkeleton(context, context->commonHeader, "c_skel.h");
		}
		else
		{
			TreeCCIncludeSkeleton(context, context->headerStream, "c_skel.h");
		}
	}
	TreeCCNodeVisitAll(context, BuildTypeDecls);
	TreeCCNodeVisitAll(context, DeclareCreateFuncs);
	TreeCCStreamPrint(context->headerStream, "\n");
	TreeCCOperationVisitAll(context, DeclareNonVirtuals);
	TreeCCStreamPrint(context->headerStream, "\n");
	OutputHelpers(context);

	/* Generate the contents of the source stream */
	if(context->use_allocator)
	{
		if(context->commonSource)
		{
			WriteSourceSkeleton(context, context->commonSource);
		}
		else
		{
			WriteSourceSkeleton(context, context->sourceStream);
		}
	}
	TreeCCNodeVisitAll(context, DefineVtables);
	TreeCCNodeVisitAll(context, ImplementCreateFuncs);
	TreeCCGenerateNonVirtuals(context, &TreeCCNonVirtualFuncsC);
	OutputIsA(context);

	/* Write all stream footers */
	WriteFooters(context);
}

#ifdef	__cplusplus
};
#endif
