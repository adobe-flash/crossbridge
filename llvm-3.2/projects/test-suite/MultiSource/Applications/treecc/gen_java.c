/*
 * gen_java.c - Generate Java source code from "treecc" input files.
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
 * Convert enumerated types into "int", because Java
 * doesn't support enumerations.
 */
static const char *ConvertType(TreeCCContext *context, char *type)
{
	TreeCCNode *node = TreeCCNodeFind(context, type);
	if(node && (node->flags & TREECC_NODE_ENUM) != 0)
	{
		return "int";
	}
	else
	{
		return type;
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
		TreeCCStreamPrint(stream, "\tpublic %s %s;\n",
						  ConvertType(context, field->type), field->name);
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
		TreeCCStream *stream = TreeCCStreamGetJava(context, node->name);
		TreeCCNode *child;
		int num;
		TreeCCStreamPrint(stream, "public class %s\n", node->name);
		TreeCCStreamPrint(stream, "{\n");
		child = node->firstChild;
		num = 0;
		while(child != 0)
		{
			if((child->flags & TREECC_NODE_ENUM_VALUE) != 0)
			{
				TreeCCStreamPrint(stream,
						"\tpublic static final int %s = %d;\n",
						child->name, num);
				++num;
			}
			child = child->nextSibling;
		}
		TreeCCStreamPrint(stream, "}\n");
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
							  ConvertType(context, field->type),
							  field->name);
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
				TreeCCStreamPrint(stream, "\tpublic abstract %s %s(",
								  ConvertType(context, virt->returnType),
								  virt->name);
			}
			else
			{
				TreeCCStreamPrint(stream, "\tpublic %s %s(",
								  ConvertType(context, virt->returnType),
								  virt->name);
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
										  ConvertType(context, param->type),
										  param->name);
					}
					else
					{
						TreeCCStreamPrint(stream, "%s P%d__",
										  ConvertType(context, param->type),
										  num);
						++num;
					}
					needComma = 1;
				}
				param = param->next;
			}
			if(!abstractCase)
			{
				TreeCCStreamPrint(stream, ")\n");
				TreeCCStreamPrint(stream, "\t{\n");
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
				   	TreeCCStreamPrint(stream, "\t\t%s %s = this;\n",
									  actualNode->name,
									  virt->oper->params->name);
					TreeCCStreamCodeIndent(stream, operCase->code, 1);
				}
				TreeCCStreamPrint(stream, "}\n\n");
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

	/* Ignore if this is an enumerated type node */
	if((node->flags & (TREECC_NODE_ENUM | TREECC_NODE_ENUM_VALUE)) != 0)
	{
		return;
	}

	/* Determine if this class has abstract virtuals */
	isAbstract = TreeCCNodeHasAbstracts(context, node);

	/* Output the class header */
	stream = TreeCCStreamGetJava(context, node->name);
	if(node->parent)
	{
		/* Inherit from a specified parent type */
		if(isAbstract)
		{
			TreeCCStreamPrint(stream,
							  "public abstract class %s extends %s\n{\n",
							  node->name, node->parent->name);
		}
		else
		{
			TreeCCStreamPrint(stream, "public class %s extends %s\n{\n",
							  node->name, node->parent->name);
		}
	}
	else
	{
		/* This type is the base of a class hierarchy */
		if(isAbstract)
		{
			if(context->baseType)
			{
				TreeCCStreamPrint(stream, "public abstract class %s extends %s\n{\n",
								  node->name,context->baseType);
			}
			else
			{
				TreeCCStreamPrint(stream, "public abstract class %s\n{\n",
								  node->name);
			}
		}
		else
		{
			if(context->baseType)
			{
				TreeCCStreamPrint(stream, "public class %s extends %s\n{\n",
								  node->name,context->baseType);
			}
			else
			{
				TreeCCStreamPrint(stream, "public class %s\n{\n", node->name);
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
	TreeCCStreamPrint(stream, "\tpublic static final int KIND = %d;\n\n",
					  node->number);

	/* Declare the fields */
	if(node->fields)
	{
		DeclareFields(context, stream, node);
		TreeCCStreamPrint(stream, "\n");
	}

	/* Declare the constructor for the node type */
	if(context->reentrant)
	{
		/* Re-entrant systems use a factory to create the nodes */
		if(context->virtual_factory || context->abstract_factory)
		{
			/* Subclasses need to be able to construct the node */
			constructorAccess = "public ";
		}
		else
		{
			/* Only the state type needs to be able to construct the node */
			constructorAccess = "";
		}
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
	TreeCCStreamPrint(stream, "\t{\n");

	/* Call the parent class constructor */
	if(node->parent)
	{
		TreeCCStreamPrint(stream, "\t\tsuper(");
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
		TreeCCStreamPrint(stream, ");\n");
	}

	/* Set the node kind */
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
	TreeCCStreamPrint(stream, "\tpublic int isA(int kind)\n");
	TreeCCStreamPrint(stream, "\t{\n");
	TreeCCStreamPrint(stream, "\t\tif(kind == KIND)\n");
	TreeCCStreamPrint(stream, "\t\t\treturn 1;\n");
	TreeCCStreamPrint(stream, "\t\telse\n");
	if(node->parent)
	{
		TreeCCStreamPrint(stream, "\t\t\treturn super.isA(kind);\n");
	}
	else
	{
		TreeCCStreamPrint(stream, "\t\t\treturn 0;\n");
	}
	TreeCCStreamPrint(stream, "\t}\n\n");
	TreeCCStreamPrint(stream, "\tpublic String getKindName()\n");
	TreeCCStreamPrint(stream, "\t{\n");
	TreeCCStreamPrint(stream, "\t\treturn \"%s\";\n", node->name);
	TreeCCStreamPrint(stream, "\t}\n");

	/* Output the class footer */
	TreeCCStreamPrint(stream, "}\n");
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
							  ConvertType(context, field->type),
							  field->name);
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
	stream = TreeCCStreamGetJava(context, context->state_type);

	/* Output the start of the function definition */
	if(context->abstract_factory)
	{
		TreeCCStreamPrint(stream, "\tpublic abstract %s %sCreate(",
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
		TreeCCStreamPrint(stream, "public abstract class %s\n{\n\n",
						  context->state_type);
	}
	else
	{
		TreeCCStreamPrint(stream, "public class %s\n{\n\n",
						  context->state_type);
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
			"\tpublic String currFilename() { return null; }\n");
		TreeCCStreamPrint(stream,
			"\tpublic long currLinenum() { return 0; }\n\n");
	}

	/* Declare the end of the state type */
	TreeCCStreamPrint(stream, "}\n");
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
static void Java_GenStart(TreeCCContext *context, TreeCCStream *stream,
					      TreeCCOperation *oper)
{
	const char *accessMode;
	if(context->internal_access && context->language == TREECC_LANG_CSHARP)
	{
		accessMode = "internal";
	}
	else
	{
		accessMode = "public";
	}
	if(oper->className)
	{
		TreeCCStreamPrint(stream, "%s class %s\n{\n",
						  accessMode, oper->className);
	}
	else
	{
		TreeCCStreamPrint(stream, "%s class %s\n{\n",
						  accessMode, oper->name);
	}
}

/*
 * Generate the entry point for a non-virtual operation.
 */
static void JavaGenEntry(TreeCCContext *context, TreeCCStream *stream,
					     TreeCCOperation *oper, int number)
{
	TreeCCParam *param;
	int num;
	int needComma;
	if(number != -1)
	{
		TreeCCStreamPrint(stream, "\tprivate static %s %s_split_%d__(",
						  oper->returnType, oper->name, number);
	}
	else
	{
		TreeCCStreamPrint(stream, "\tpublic static %s %s(",
						  oper->returnType, oper->name);
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
		if(context->language == TREECC_LANG_CSHARP)
		{
			TreeCCStreamPrint(stream, "%s ", param->type);
		}
		else
		{
			TreeCCStreamPrint(stream, "%s ", ConvertType(context, param->type));
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
			if(!IsEnumType(context, param->type))
			{
				TreeCCStreamPrint(stream, "__");
			}
		}
		needComma = 1;
		param = param->next;
	}
	TreeCCStreamPrint(stream, ")\n");
	TreeCCStreamPrint(stream, "\t{\n");
}

/*
 * Generate the entry point for a non-virtual operation.
 */
static void Java_GenEntry(TreeCCContext *context, TreeCCStream *stream,
					      TreeCCOperation *oper)
{
	JavaGenEntry(context, stream, oper, -1);
}

/*
 * Generate the entry point for a split-out function.
 */
static void Java_GenSplitEntry(TreeCCContext *context, TreeCCStream *stream,
					           TreeCCOperation *oper, int number)
{
	JavaGenEntry(context, stream, oper, number);
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
static void Java_GenSwitchHead(TreeCCContext *context, TreeCCStream *stream,
							   char *paramName, int level, int isEnum)
{
	Indent(stream, level * 2 + 2);
	if(isEnum)
	{
		TreeCCStreamPrint(stream, "switch(%s)\n", paramName);
	}
	else
	{
		TreeCCStreamPrint(stream, "switch(%s__.getKind())\n", paramName);
	}
	Indent(stream, level * 2 + 2);
	TreeCCStreamPrint(stream, "{\n");
}

/*
 * Generate a selector for a "switch" case.
 */
static void Java_GenSelector(TreeCCContext *context, TreeCCStream *stream,
						     TreeCCNode *node, int level)
{
	if((node->flags & TREECC_NODE_ENUM_VALUE) != 0)
	{
		Indent(stream, level * 2 + 3);
		TreeCCStreamPrint(stream, "case %s.%s:\n",
						  node->parent->name, node->name);
	}
	else if((node->flags & TREECC_NODE_ENUM) == 0)
	{
		Indent(stream, level * 2 + 3);
		TreeCCStreamPrint(stream, "case %s.KIND:\n", node->name);
	}
}

/*
 * Terminate the selectors and begin the body of a "switch" case.
 */
static void Java_GenEndSelectors(TreeCCContext *context, TreeCCStream *stream,
							     int level)
{
	Indent(stream, level * 2 + 3);
	TreeCCStreamPrint(stream, "{\n");
}

/*
 * Generate the code for a case within a function.
 */
static void Java_GenCaseFunc(TreeCCContext *context, TreeCCStream *stream,
						     TreeCCOperationCase *operCase, int number)
{
	TreeCCParam *param;
	TreeCCTrigger *trigger;
	int num;
	int needComma;
	char *type;

	/* Output the header for the function */
	TreeCCStreamPrint(stream, "\tprivate static %s %s_%d__(",
					  ConvertType(context, operCase->oper->returnType),
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
			}
			else
			{
				type = param->type;
			}
			trigger = trigger->next;
		}
		else
		{
			type = param->type;
		}
		if(param->name)
		{
			if(context->language == TREECC_LANG_CSHARP)
			{
				TreeCCStreamPrint(stream, "%s %s", type, param->name);
			}
			else
			{
				TreeCCStreamPrint(stream, "%s %s",
								  ConvertType(context, type), param->name);
			}
		}
		else
		{
			if(context->language == TREECC_LANG_CSHARP)
			{
				TreeCCStreamPrint(stream, "%s P%d__", type, num);
			}
			else
			{
				TreeCCStreamPrint(stream, "%s P%d__",
								  ConvertType(context, type), num);
			}
			++num;
		}
		needComma = 1;
		param = param->next;
	}
	TreeCCStreamPrint(stream, ")\n");

	/* Output the code for the operation case */
	if(context->language == TREECC_LANG_CSHARP)
	{
		TreeCCStreamLine(stream, operCase->codeLinenum, operCase->codeFilename);
	}
	TreeCCStreamPrint(stream, "\t{");
	if(operCase->code)
	{
		TreeCCStreamCodeIndent(stream, operCase->code, 1);
	}
	TreeCCStreamPrint(stream, "}\n");
	if(context->language == TREECC_LANG_CSHARP)
	{
		TreeCCStreamFixLine(stream);
	}
	TreeCCStreamPrint(stream, "\n");
}

/*
 * Generate a call to a case function from within the "switch".
 */
static void Java_GenCaseCall(TreeCCContext *context, TreeCCStream *stream,
						     TreeCCOperationCase *operCase, int number,
						     int level)
{
	TreeCCParam *param;
	TreeCCTrigger *trigger;
	int num;
	int needComma;

	/* Indent to the correct level */
	Indent(stream, level * 2 + 4);

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
				TreeCCStreamPrint(stream, "(%s)", trigger->node->name);
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
static void Java_GenCaseInline(TreeCCContext *context, TreeCCStream *stream,
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
					Indent(stream, level * 2 + 4);
					TreeCCStreamPrint(stream, "%s %s = (%s)%s__;\n",
									  trigger->node->name, param->name,
									  trigger->node->name, param->name);
				}
			}
			trigger = trigger->next;
		}
		param = param->next;
	}

	/* Output the inline code for the case */
	if(context->language == TREECC_LANG_CSHARP)
	{
		TreeCCStreamLine(stream, operCase->codeLinenum, operCase->codeFilename);
	}
	Indent(stream, level * 2 + 4);
	TreeCCStreamPrint(stream, "{");
	if(operCase->code)
	{
		TreeCCStreamCodeIndent(stream, operCase->code, level * 2 + 4);
	}
	TreeCCStreamPrint(stream, "}\n");
	if(context->language == TREECC_LANG_CSHARP)
	{
		TreeCCStreamFixLine(stream);
	}
}

/*
 * Generate a call to a split function from within the "switch".
 */
static void Java_GenCaseSplit(TreeCCContext *context, TreeCCStream *stream,
						      TreeCCOperationCase *operCase,
							  int number, int level)
{
	TreeCCParam *param;
	TreeCCTrigger *trigger;
	int num;
	int needComma;

	/* Indent to the correct level */
	Indent(stream, level * 2 + 4);

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
				TreeCCStreamPrint(stream, "(%s)", trigger->node->name);
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
static void Java_GenEndCase(TreeCCContext *context, TreeCCStream *stream,
						    int level)
{
	Indent(stream, level * 2 + 3);
	TreeCCStreamPrint(stream, "}\n");
	Indent(stream, level * 2 + 3);
	TreeCCStreamPrint(stream, "break;\n\n");
}

/*
 * Terminate the "switch" statement.
 */
static void Java_GenEndSwitch(TreeCCContext *context, TreeCCStream *stream,
						      int level)
{
	Indent(stream, level * 2 + 3);
	TreeCCStreamPrint(stream, "default: break;\n");
	Indent(stream, level * 2 + 2);
	TreeCCStreamPrint(stream, "}\n");
}

/*
 * Generate the exit point for a non-virtual operation.
 */
static void Java_GenExit(TreeCCContext *context, TreeCCStream *stream,
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
	TreeCCStreamPrint(stream, "\t}\n");
}

/*
 * Generate the end declarations for a non-virtual operation.
 */
static void Java_GenEnd(TreeCCContext *context, TreeCCStream *stream,
					    TreeCCOperation *oper)
{
	TreeCCStreamPrint(stream, "}\n");
	if(context->language == TREECC_LANG_CSHARP)
	{
		TreeCCStreamPrint(stream, "\n");
	}
}

/*
 * Table of non-virtual code generation functions.
 */
TreeCCNonVirtual const TreeCCNonVirtualFuncsJava = {
	Java_GenStart,
	Java_GenEntry,
	Java_GenSplitEntry,
	Java_GenSwitchHead,
	Java_GenSelector,
	Java_GenEndSelectors,
	Java_GenCaseFunc,
	Java_GenCaseCall,
	Java_GenCaseInline,
	Java_GenCaseSplit,
	Java_GenEndCase,
	Java_GenEndSwitch,
	Java_GenExit,
	Java_GenEnd,
};

/*
 * Write out header information for all streams.
 */
static void WriteJavaHeaders(TreeCCContext *context)
{
	TreeCCStream *stream = context->streamList;
	while(stream != 0)
	{
		if(!(stream->isHeader))
		{
			TreeCCStreamSourceTop(stream);
			TreeCCStreamPrint(stream, "\n");
			if(context->namespace)
			{
				TreeCCStreamPrint(stream, "package %s;\n\n",
								  context->namespace);
			}
			TreeCCStreamPrint(stream, "import java.lang.*;\n\n");
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
static void WriteJavaFooters(TreeCCContext *context)
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
		}
		stream = stream->nextStream;
	}
}

/*
 * Create streams for all of the node types.
 */
static void CreateNodeStreams(TreeCCContext *context, TreeCCNode *node)
{
	if((node->flags & TREECC_NODE_ENUM_VALUE) == 0)
	{
		TreeCCStreamGetJava(context, node->name);
	}
}

/*
 * Create streams for all of the non-virtual operations
 */
static void CreateNonVirtualStreams(TreeCCContext *context,
						  	   	    TreeCCOperation *oper)
{
	if((oper->flags & TREECC_OPER_VIRTUAL) == 0)
	{
		if(oper->className)
		{
			TreeCCStreamGetJava(context, oper->className);
		}
		else
		{
			TreeCCStreamGetJava(context, oper->name);
		}
	}
}

void TreeCCGenerateJava(TreeCCContext *context)
{
	/* Create all streams that we will require later */
	TreeCCStreamGetJava(context, context->state_type);
	TreeCCNodeVisitAll(context, CreateNodeStreams);
	TreeCCOperationVisitAll(context, CreateNonVirtualStreams);

	/* Write all stream headers */
	WriteJavaHeaders(context);

	/* Generate the contents of the source stream */
	TreeCCNodeVisitAll(context, DeclareTypeDefs);
	ImplementStateType(context,
					   TreeCCStreamGetJava(context, context->state_type));
	TreeCCNodeVisitAll(context, BuildTypeDecls);
	TreeCCGenerateNonVirtuals(context, &TreeCCNonVirtualFuncsJava);

	/* Write all stream footers */
	WriteJavaFooters(context);
}

#ifdef	__cplusplus
};
#endif
