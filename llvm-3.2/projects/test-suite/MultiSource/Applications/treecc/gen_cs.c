/*
 * gen_cs.c - Generate C# source code from "treecc" input files.
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
 * Declare the fields for a node type.
 */
static void DeclareFields(TreeCCContext *context,
						  TreeCCStream *stream, TreeCCNode *node)
{
	TreeCCField *field;
	field = node->fields;
	while(field != 0)
	{
		TreeCCStreamPrint(stream, "\tpublic %s %s;\n",
						  field->type, field->name);
		field = field->next;
	}
}

/*
 * Declare the type definitions for a node type.
 */
static void DeclareTypeDefs(TreeCCContext *context,
						    TreeCCNode *node)
{
	if((node->flags & TREECC_NODE_ENUM) != 0)
	{
		/* Define an enumerated type */
		TreeCCStream *stream = node->source;
		TreeCCNode *child;
		if(context->internal_access)
		{
			TreeCCStreamPrint(stream, "internal enum %s\n", node->name);
		}
		else
		{
			TreeCCStreamPrint(stream, "public enum %s\n", node->name);
		}
		TreeCCStreamPrint(stream, "{\n");
		child = node->firstChild;
		while(child != 0)
		{
			if((child->flags & TREECC_NODE_ENUM_VALUE) != 0)
			{
				TreeCCStreamPrint(stream, "\t%s,\n", child->name);
			}
			child = child->nextSibling;
		}
		TreeCCStreamPrint(stream, "}\n\n");
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
 * Implement the virtual methods that have implementations in a node type.
 */
static void ImplementVirtuals(TreeCCContext *context, TreeCCStream *stream,
							  TreeCCNode *node, TreeCCNode *actualNode)
{
	TreeCCVirtual *virt;
	TreeCCParam *param;
	TreeCCOperationCase *operCase;
	int declareCase, abstractCase;
	TreeCCNode *tempNode;
	int num, first;
	int needComma;
	if(node->parent)
	{
		ImplementVirtuals(context, stream, node->parent, actualNode);
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
			if(abstractCase)
			{
				if(node == actualNode)
				{
					TreeCCStreamPrint(stream,
									  "\tpublic abstract %s %s(",
									  virt->returnType, virt->name);
				}
				else
				{
					/* Inherit the "abstract" definition from the parent */
					virt = virt->next;
					continue;
				}
			}
			else
			{
				if(node == actualNode)
				{
					TreeCCStreamPrint(stream, "\tpublic virtual %s %s(",
									  virt->returnType, virt->name);
				}
				else
				{
					TreeCCStreamPrint(stream, "\tpublic override %s %s(",
									  virt->returnType, virt->name);
				}
			}
			param = virt->oper->params;
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
			if(!abstractCase)
			{
				TreeCCStreamPrint(stream, ")\n");
				TreeCCStreamLine(stream, operCase->codeLinenum,
								 operCase->codeFilename);
				TreeCCStreamPrint(stream, "\t{");
				if(!(virt->oper->params->name) ||
				   !strcmp(virt->oper->params->name, "this"))
				{
					/* The first parameter is called "this", so we don't
					   need to declare it at the head of the function */
					TreeCCStreamCodeIndent(stream, operCase->code, 1);
				}
				else
				{
					/* The first parameter is called something else,
					   so create a temporary variable to hold "this" */
				   	TreeCCStreamPrint(stream, "\n\t\t%s %s = this;\n\t",
									  actualNode->name,
									  virt->oper->params->name);
					TreeCCStreamCodeIndent(stream, operCase->code, 1);
				}
				TreeCCStreamPrint(stream, "}\n");
				TreeCCStreamFixLine(stream);
				TreeCCStreamPrint(stream, "\n");
			}
			else
			{
				TreeCCStreamPrint(stream, ");\n\n");
			}
		}
		virt = virt->next;
	}
}

/*
 * Build the type declarations for a node type.
 */
static void BuildTypeDecls(TreeCCContext *context,
						   TreeCCNode *node)
{
	TreeCCStream *stream;
	int needComma;
	const char *constructorAccess;
	TreeCCField *field;
	int isAbstract;
	const char *accessMode;

	/* Ignore if this is an enumerated type node */
	if((node->flags & (TREECC_NODE_ENUM | TREECC_NODE_ENUM_VALUE)) != 0)
	{
		return;
	}

	/* Determine if this class has abstract virtuals */
	isAbstract = TreeCCNodeHasAbstracts(context, node);

	/* Determine the access mode for the class */
	if(context->internal_access)
	{
		accessMode = "internal";
	}
	else
	{
		accessMode = "public";
	}

	/* Output the class header */
	stream = node->source;
	if(node->parent)
	{
		/* Inherit from a specified parent type */
		if(isAbstract)
		{
			TreeCCStreamPrint(stream, "%s abstract class %s : %s\n{\n",
							  accessMode, node->name, node->parent->name);
		}
		else
		{
			TreeCCStreamPrint(stream, "%s class %s : %s\n{\n",
							  accessMode, node->name, node->parent->name);
		}
	}
	else
	{
		/* This type is the base of a class hierarchy */
		if(isAbstract)
		{
			if(context->baseType)
			{
				TreeCCStreamPrint(stream, "%s abstract class %s : %s\n{\n",
							  accessMode, node->name, context->baseType);
			}
			else
			{
				TreeCCStreamPrint(stream, "%s abstract class %s\n{\n",
							  accessMode, node->name);
			}
		}
		else
		{
			if(context->baseType)
			{
				TreeCCStreamPrint(stream, "%s class %s : %s\n{\n",
							  accessMode, node->name, context->baseType);
			}
			else
			{
				TreeCCStreamPrint(stream, "%s class %s\n{\n",
							  accessMode, node->name);
			}
		}

		/* Declare the node kind member variable */
		TreeCCStreamPrint(stream, "\tprotected int kind__;\n");

		/* Declare the filename and linenum fields if we are tracking lines */
		if(context->track_lines)
		{
			TreeCCStreamPrint(stream, "\tprotected String filename__;\n");
			TreeCCStreamPrint(stream, "\tprotected long linenum__;\n");
		}
		TreeCCStreamPrint(stream, "\n");

		/* Declare the public methods for access to the above fields */
		TreeCCStreamPrint(stream,
				"\tpublic int getKind() { return kind__; }\n");
		if(context->track_lines)
		{
			TreeCCStreamPrint(stream,
				"\tpublic String getFilename() { return filename__; }\n");
			TreeCCStreamPrint(stream,
				"\tpublic long getLinenum() { return linenum__; }\n");
			TreeCCStreamPrint(stream,
			 	"\tpublic void setFilename(String filename) "
					"{ filename__ = filename; }\n");
			TreeCCStreamPrint(stream,
				"\tpublic void setLinenum(long linenum) "
					"{ linenum__ = linenum; }\n");
		}
		TreeCCStreamPrint(stream, "\n");
	}

	/* Declare the kind value */
	if(node->parent)
	{
		TreeCCStreamPrint(stream, "\tpublic new const int KIND = %d;\n\n",
						  node->number);
	}
	else
	{
		TreeCCStreamPrint(stream, "\tpublic const int KIND = %d;\n\n",
						  node->number);
	}

	/* Declare the fields */
	if(node->fields)
	{
		DeclareFields(context, stream, node);
		TreeCCStreamPrint(stream, "\n");
	}

	/* Declare the constructor for the node type */
	if(context->reentrant)
	{
		/* Re-entrant systems use a factory to create the nodes.
		   C# doesn't have a notion of "access by members of
		   the namespace only".  The closest is "internal", but
		   even that isn't quite right, so we always use "public" */
		constructorAccess = "public ";
	}
	else
	{
		/* Non-reentrant systems can construct nodes directly,
		   unless the node happens to be abstract, in which
		   case we force the constructor to be protected */
		if((node->flags & TREECC_NODE_ABSTRACT) != 0)
		{
			constructorAccess = "protected ";
		}
		else
		{
			constructorAccess = "public ";
		}
	}
	TreeCCStreamPrint(stream, "\t%s%s(", constructorAccess, node->name);
	if(context->reentrant)
	{
		TreeCCStreamPrint(stream, "%s state__", context->state_type);
		needComma = 1;
	}
	else
	{
		needComma = 0;
	}
	CreateParams(context, stream, node, needComma);
	TreeCCStreamPrint(stream, ")\n");

	/* Call the parent class constructor */
	if(node->parent)
	{
		TreeCCStreamPrint(stream, "\t\t: base(");
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

	/* Set the node kind */
	TreeCCStreamPrint(stream, "\t{\n");
	TreeCCStreamPrint(stream, "\t\tthis.kind__ = KIND;\n");

	/* Track the filename and line number if necessary */
	if(context->track_lines && !(node->parent))
	{
		if(context->reentrant)
		{
			TreeCCStreamPrint(stream,
					"\t\tthis.filename__ = state__.currFilename();\n");
			TreeCCStreamPrint(stream,
					"\t\tthis.linenum__ = state__.currLinenum();\n");
		}
		else
		{
			TreeCCStreamPrint(stream,
					"\t\tthis.filename__ = %s.getState().currFilename();\n",
					context->state_type);
			TreeCCStreamPrint(stream,
					"\t\tthis.linenum__ = %s.getState().currLinenum();\n",
					context->state_type);
		}
	}

	/* Initialize the fields that are specific to this node type */
	field = node->fields;
	while(field != 0)
	{
		if((field->flags & TREECC_FIELD_NOCREATE) == 0)
		{
			TreeCCStreamPrint(stream, "\t\tthis.%s = %s;\n",
							  field->name, field->name);
		}
		else if(field->value)
		{
			TreeCCStreamPrint(stream, "\t\tthis.%s = %s;\n",
							  field->name, field->value);
		}
		field = field->next;
	}
	TreeCCStreamPrint(stream, "\t}\n\n");

	/* Implement the virtual functions */
	ImplementVirtuals(context, stream, node, node);

	/* Declare the "isA" and "getKindName" helper methods */
	if(node->parent)
	{
		TreeCCStreamPrint(stream, "\tpublic override int isA(int kind)\n");
	}
	else
	{
		TreeCCStreamPrint(stream, "\tpublic virtual int isA(int kind)\n");
	}
	TreeCCStreamPrint(stream, "\t{\n");
	TreeCCStreamPrint(stream, "\t\tif(kind == KIND)\n");
	TreeCCStreamPrint(stream, "\t\t\treturn 1;\n");
	TreeCCStreamPrint(stream, "\t\telse\n");
	if(node->parent)
	{
		TreeCCStreamPrint(stream, "\t\t\treturn base.isA(kind);\n");
	}
	else
	{
		TreeCCStreamPrint(stream, "\t\t\treturn 0;\n");
	}
	TreeCCStreamPrint(stream, "\t}\n\n");
	if(node->parent)
	{
		TreeCCStreamPrint(stream, "\tpublic override String getKindName()\n");
	}
	else
	{
		TreeCCStreamPrint(stream, "\tpublic virtual String getKindName()\n");
	}
	TreeCCStreamPrint(stream, "\t{\n");
	TreeCCStreamPrint(stream, "\t\treturn \"%s\";\n", node->name);
	TreeCCStreamPrint(stream, "\t}\n");

	/* Output the class footer */
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
			TreeCCStreamPrint(stream, "%s %s",
							  field->type, field->name);
			needComma = 1;
		}
		field = field->next;
	}
	return needComma;
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
	if(context->abstract_factory)
	{
		TreeCCStreamPrint(stream, "\tpublic virtual abstract %s %sCreate(",
						  node->name, node->name);
	}
	else if(context->virtual_factory)
	{
		TreeCCStreamPrint(stream, "\tpublic virtual %s %sCreate(",
						  node->name, node->name);
	}
	else
	{
		TreeCCStreamPrint(stream, "\tpublic %s %sCreate(",
						  node->name, node->name);
	}

	/* Output the parameters for the create function */
	FactoryCreateParams(context, stream, node, 0);

	/* Output the body of the creation function */
	if(context->abstract_factory)
	{
		TreeCCStreamPrint(stream, ");\n");
	}
	else
	{
		TreeCCStreamPrint(stream, ")\n");
		TreeCCStreamPrint(stream, "\t{\n");
		TreeCCStreamPrint(stream, "\t\treturn new %s(this", node->name);
		FactoryInvokeParams(context, stream, node, 1);
		TreeCCStreamPrint(stream, ");\n");
		TreeCCStreamPrint(stream, "\t}\n\n");
	}
}

/*
 * Implement the state type in the source stream.
 */
static void ImplementStateType(TreeCCContext *context, TreeCCStream *stream)
{
	/* Declare the class header */
	if(context->reentrant && context->abstract_factory)
	{
		if(context->internal_access)
		{
			TreeCCStreamPrint(stream, "internal abstract class %s\n{\n\n",
							  context->state_type);
		}
		else
		{
			TreeCCStreamPrint(stream, "public abstract class %s\n{\n\n",
							  context->state_type);
		}
	}
	else
	{
		if(context->internal_access)
		{
			TreeCCStreamPrint(stream, "internal class %s\n{\n\n",
							  context->state_type);
		}
		else
		{
			TreeCCStreamPrint(stream, "public class %s\n{\n\n",
							  context->state_type);
		}
	}

	/* Singleton handling for non-reentrant systems */
	if(!(context->reentrant))
	{
		TreeCCStreamPrint(stream, "\tprivate static %s state__;\n\n",
						  context->state_type);
		TreeCCStreamPrint(stream, "\tpublic static %s getState()\n",
						  context->state_type);
		TreeCCStreamPrint(stream, "\t{\n");
		TreeCCStreamPrint(stream, "\t\tif(state__ != null) return state__;\n");
		TreeCCStreamPrint(stream, "\t\tstate__ = new %s();\n",
						  context->state_type);
		TreeCCStreamPrint(stream, "\t\treturn state__;\n");
		TreeCCStreamPrint(stream, "\t}\n\n");
	}

	/* Implement the constructor */
	if(context->reentrant)
	{
		TreeCCStreamPrint(stream, "\tpublic %s() {}\n\n", context->state_type);
	}
	else
	{
		TreeCCStreamPrint(stream, "\tpublic %s() { state__ = this; }\n\n",
						  context->state_type);
	}

	/* Implement the create functions for all of the node types */
	if(context->reentrant)
	{
		TreeCCNodeVisitAll(context, ImplementCreateFuncs);
	}

	/* Implement the line number tracking methods */
	if(context->track_lines)
	{
		TreeCCStreamPrint(stream,
			"\tpublic virtual String currFilename() { return null; }\n");
		TreeCCStreamPrint(stream,
			"\tpublic virtual long currLinenum() { return 0; }\n\n");
	}

	/* Declare the end of the state type */
	TreeCCStreamPrint(stream, "}\n\n");
}

/*
 * Write out header information for all streams.
 */
static void WriteCSharpHeaders(TreeCCContext *context)
{
	TreeCCStream *stream = context->streamList;
	while(stream != 0)
	{
		if(!(stream->isHeader))
		{
			TreeCCStreamPrint(stream,
					"/* %s.  Generated automatically by treecc */\n\n",
					stream->embedName);
			if(context->namespace)
			{
				TreeCCStreamPrint(stream, "namespace %s\n{\n\n",
								  context->namespace);
			}
			TreeCCStreamPrint(stream, "using System;\n\n");
			TreeCCStreamSourceTopCS(stream);
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
static void WriteCSharpFooters(TreeCCContext *context)
{
	TreeCCStream *stream = context->streamList;
	while(stream != 0)
	{
		if(stream->defaultFile && !(stream->dirty))
		{
			/* Clear the default file's contents, which we don't need */
			TreeCCStreamClear(stream);
		}
		else if(!(stream->isHeader))
		{
			TreeCCStreamSourceBottom(stream);
			if(context->namespace)
			{
				TreeCCStreamPrint(stream, "}\n");
			}
		}
		stream = stream->nextStream;
	}
}

void TreeCCGenerateCSharp(TreeCCContext *context)
{
	/* Write all stream headers */
	WriteCSharpHeaders(context);

	/* Generate the contents of the source stream */
	TreeCCNodeVisitAll(context, DeclareTypeDefs);
	if(context->commonSource)
	{
		ImplementStateType(context, context->commonSource);
	}
	else
	{
		ImplementStateType(context, context->sourceStream);
	}
	TreeCCNodeVisitAll(context, BuildTypeDecls);
	TreeCCGenerateNonVirtuals(context, &TreeCCNonVirtualFuncsJava);

	/* Write all stream footers */
	WriteCSharpFooters(context);
}

#ifdef	__cplusplus
};
#endif
