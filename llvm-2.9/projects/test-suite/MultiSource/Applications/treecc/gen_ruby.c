/*
 * gen_ruby.c - Generate Ruby source code from "treecc" input files.
 *
 * Copyright (C) 2001, 2002  Southern Storm Software, Pty Ltd.
 *
 * Hacked by Peter Minten <silvernerd@users.sf.net>
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
 * Declare the type definitions for a node type.
 */
static void DeclareTypeDefs(TreeCCContext *context,
						    TreeCCNode *node)
{
	if((node->flags & TREECC_NODE_ENUM) != 0)
	{
		int counter = 0;
		/* Define an enumerated type */
		TreeCCStream *stream = node->source;
		TreeCCNode *child;
		TreeCCStreamPrint(stream, "class %s \n", node->name);
		child = node->firstChild;
		while(child != 0)
		{
			if((child->flags & TREECC_NODE_ENUM_VALUE) != 0)
			{
				TreeCCStreamPrint(stream, "  %s = %i\n", child->name, counter++);
			}

			child = child->nextSibling;
		}
		TreeCCStreamPrint(stream, "end\n\n");
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
			/* Don't name the types, Ruby figures that out */
			TreeCCStreamPrint(stream, "%s", /*field->type,*/ field->name);
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
					TreeCCStreamPrint(stream, "  def %s(", virt->name);
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
					TreeCCStreamPrint(stream, "  def %s(", virt->name);
				}
				else
				{
					TreeCCStreamPrint(stream, "  def %s(", virt->name);
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
						TreeCCStreamPrint(stream, "%s", param->name);
					}
					else
					{
						TreeCCStreamPrint(stream, "P%d__", num);
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
				TreeCCStreamPrint(stream, "  ");
				if(!(virt->oper->params->name) ||
				   !strcmp(virt->oper->params->name, "self"))
				{
					/* The first parameter is called "this", so we don't
					   need to declare it at the head of the function */
					TreeCCStreamCodeIndent(stream, operCase->code, 1);
				}
				else
				{
					/* The first parameter is called something else,
					   so create a temporary variable to hold "this" */
				   	TreeCCStreamPrint(stream, "\n    %s %s = self\n  ",
									  actualNode->name,
									  virt->oper->params->name);
					TreeCCStreamCodeIndent(stream, operCase->code, 1);
				}
				TreeCCStreamPrint(stream, "  end\n");
				TreeCCStreamFixLine(stream);
				TreeCCStreamPrint(stream, "\n");
			}
			else
			{
				TreeCCStreamPrint(stream, ")\n  end\n");
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
	/*const char *constructorAccess; NOT USED*/
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
	stream = node->source;
	if(node->parent)
	{
		/* Inherit from a specified parent type */
		/* Ruby doesn't know abstract */	
		/*   
		if(isAbstract)
		{
			TreeCCStreamPrint(stream, "public abstract class %s : %s\n{\n",
							  node->name, node->parent->name);
		}
		else
		{
			TreeCCStreamPrint(stream, "public class %s : %s\n{\n",
							  node->name, node->parent->name);
		}
		*/
		TreeCCStreamPrint(stream, "class %s < %s\n",
						  node->name, node->parent->name);
	}
	else
	{
		/* This type is the base of a class hierarchy */
		/* Ruby doesn't know abstract */
/*		if(isAbstract)
		{
			TreeCCStreamPrint(stream, "public abstract class %s\n{\n",
							  node->name);
		}
		else
		{
			TreeCCStreamPrint(stream, "public class %s\n{\n", node->name);
		}*/

		TreeCCStreamPrint(stream, "class %s\n", node->name);

		/* Declare the node kind member variable */
		/* No declaration needed in Ruby */

		/* Declare the filename and linenum fields if we are tracking lines */
		/* Not needed */

		/* Declare the public methods for access to the above fields */
		/* Ruby has handy accessor creating stuff */
		/*TreeCCStreamPrint(stream,
				"  public int getKind() { return kind__; }\n");*/
		TreeCCStreamPrint(stream,
				"  protected\n  attr_reader :kind\n  public\n\n");
			
		if(context->track_lines)
		{
			/* A same kind of hack here*/
			/*TreeCCStreamPrint(stream,
				"  public String getFilename() { return filename__; }\n");
			TreeCCStreamPrint(stream,
				"  public long getLinenum() { return linenum__; }\n");
			TreeCCStreamPrint(stream,
			 	"  public void setFilename(String filename) "
					"{ filename__ = filename; }\n");
			TreeCCStreamPrint(stream,
				"  public void setLinenum(long linenum) "
					"{ linenum__ = linenum; }\n");*/
			TreeCCStreamPrint(stream,
				"  attr_accessor :Linenum, :Filename\n");
		}
		TreeCCStreamPrint(stream, "\n");
	}

	/* Add the attr_accessor stuff for nodes specific to this node type */
	field = node->fields;
	while(field != 0)
	{
		TreeCCStreamPrint(stream, "  attr_accessor :%s\n", field->name);
		field = field->next;
	}
	/* End this section with an extra newline */
	TreeCCStreamPrint(stream, "\n");

	/* Declare the kind value */
	/* Stick to the Ruby convention of constants, start with Uppercase,
	   continue with lowercase */
	/* The parent doesn't matter, so don't check it */
	TreeCCStreamPrint(stream, "  KIND = %d\n\n",
					  node->number);

	/* Declare the constructor for the node type */
	/* A constructor is always public (I hope) anyway I don't expect
	   Ruby to cause troubles here */

	/* The constructor is ALWAYS called initialize */
	TreeCCStreamPrint(stream, "  def initialize(");
	if(context->reentrant)
	{
		TreeCCStreamPrint(stream, "state__");
		needComma = 1;

	}
	else
	{
		needComma = 0;
	}
	
	CreateParams(context, stream, node, needComma);

	TreeCCStreamPrint(stream, ")\n");
	
	/* Enter the super call */
	/* Call the parent class constructor */
	if(node->parent)
	{
		/* Do not use base, Ruby uses super for that */
		/*TreeCCStreamPrint(stream, "    : base(");*/
		TreeCCStreamPrint(stream, "    super(");
		if(context->reentrant)
		{
			TreeCCStreamPrint(stream, "@state");
			needComma = 1;
		}
		else
		{
			needComma = 0;
		}
		CreateParams(context, stream, node, needComma);		
		InheritParamsSource(context, stream, node->parent, needComma);
		TreeCCStreamPrint(stream, ")\n");
	}
	
	/* Set the node kind */	
	TreeCCStreamPrint(stream, "    @kind = KIND\n");

	/* Track the filename and line number if necessary */
	if(context->track_lines && !(node->parent))
	{
		if(context->reentrant)
		{
			TreeCCStreamPrint(stream,
					"    @Filename = @state.currFilename\n");
			TreeCCStreamPrint(stream,
					"    @Finenum = @state.currLinenum\n");
		}
		else
		{
			TreeCCStreamPrint(stream,
					"    @Filename = %s.state.currFilename()\n",
					context->state_type);
			TreeCCStreamPrint(stream,
					"    @Linenum = %s.state.currLinenum()\n",
					context->state_type);
		}
	}

	/* Initialize the fields that are specific to this node type */
	field = node->fields;
	while(field != 0)
	{
		if((field->flags & TREECC_FIELD_NOCREATE) == 0)
		{
			TreeCCStreamPrint(stream, "    self.%s = %s\n",
							  field->name, field->name);
		}
		else if(field->value)
		{
			TreeCCStreamPrint(stream, "    self.%s = %s\n",
							  field->name, field->value);
		}
		field = field->next;
	}
	TreeCCStreamPrint(stream, "  end\n\n");

	/* Implement the virtual functions */
	ImplementVirtuals(context, stream, node, node);

	/* Declare the "isA" and "getKindName" helper methods */

	TreeCCStreamPrint(stream, "  def isA(kind)\n");

	TreeCCStreamPrint(stream, "    if(@kind == KIND) then\n");
	TreeCCStreamPrint(stream, "      return true\n");
	TreeCCStreamPrint(stream, "    else\n");
	if(node->parent)
	{
		TreeCCStreamPrint(stream, "      return super(kind)\n    end\n");
	}
	else
	{
		TreeCCStreamPrint(stream, "      return 0\n    end\n");
	}
	TreeCCStreamPrint(stream, "  end\n\n");

	TreeCCStreamPrint(stream, "  def KindName\n");

	TreeCCStreamPrint(stream, "    return \"%s\"\n", node->name);
	TreeCCStreamPrint(stream, "  end\n");

	/* Output the class footer */
	TreeCCStreamPrint(stream, "end\n\n");
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
			/* Delete the types */
			TreeCCStreamPrint(stream, "%s", field->name);
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
	TreeCCStreamPrint(stream, "  def %s %sCreate(",
					  node->name, node->name);

	/* Output the parameters for the create function */
	FactoryCreateParams(context, stream, node, 0);

	/* Output the body of the creation function */
	if(context->abstract_factory)
	{
		TreeCCStreamPrint(stream, ")\n");
		TreeCCStreamPrint(stream, "raise \"Abstract method called: %s\\n\"\n", node->name);		
	}
	else
	{
		TreeCCStreamPrint(stream, ")\n");
		TreeCCStreamPrint(stream, "    return %s.new(this", node->name);
		FactoryInvokeParams(context, stream, node, 1);
		TreeCCStreamPrint(stream, ")\n");
		TreeCCStreamPrint(stream, "  end\n\n");
	}
}

/*
 * Implement the state type in the source stream.
 */
static void ImplementStateType(TreeCCContext *context, TreeCCStream *stream)
{
	TreeCCStreamPrint(stream, "class %s\n",
					  context->state_type);

	TreeCCStreamPrint(stream, "  @@state = nil\n");

	/* Singleton handling for non-reentrant systems */
	if(!(context->reentrant))
	{
		TreeCCStreamPrint(stream, "  def %s.state\n", context->state_type);
		TreeCCStreamPrint(stream, "    return @@state unless @@state.nil?\n");
		TreeCCStreamPrint(stream, "    @@state = %s.new()\n",
						  context->state_type);
		TreeCCStreamPrint(stream, "    return @@state\n");
		TreeCCStreamPrint(stream, "  end\n\n");
	}

	/* Implement the constructor */
	if(context->reentrant)
	{
		/* No constructor */
	}
	else
	{
		TreeCCStreamPrint(stream, "  def intialize \n     @@state = self \n   end\n\n");
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
			"  def currFilename \n     return nil \n  end\n\n");
		TreeCCStreamPrint(stream,
			"  def currLinenum \n     return 0 \n  end\n\n");
	}

	/* Declare the end of the state type */
	TreeCCStreamPrint(stream, "end\n\n");
}

/*
 * Write out header information for all streams.
 */
static void WriteRubyHeaders(TreeCCContext *context)
{
	TreeCCStream *stream = context->streamList;
	while(stream != 0)
	{
		if(!(stream->isHeader))
		{
			TreeCCStreamPrint(stream,
					"# %s.  Generated automatically by treecc \n\n",
					stream->embedName);
			if(context->namespace)
			{
				TreeCCStreamPrint(stream, "module %s\nbegin\n\n",
								  context->namespace);
			}
			/*Ruby doesn't require a System lib to be included*/
			/*TreeCCStreamPrint(stream, "using System;\n\n");*/
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
static void WriteRubyFooters(TreeCCContext *context)
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
				TreeCCStreamPrint(stream, "end\n");
			}
		}
		stream = stream->nextStream;
	}
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
 * Output spaces's for a specific level of indenting.
 */
static void Indent(TreeCCStream *stream, int indent)
{
	while(indent >= 4)
	{
		TreeCCStreamPrint(stream, "        ");
		indent -= 4;
	}
	if(indent == 1)
	{
		TreeCCStreamPrint(stream, "  ");
	}
	else if(indent == 2)
	{
		TreeCCStreamPrint(stream, "    ");
	}
	else if(indent == 3)
	{
		TreeCCStreamPrint(stream, "      ");
	}
}

/*
 * Non-virtual code generation functions start here
 */

/*
 * Generate the start declarations for a non-virtual operation.
 */
static void Ruby_GenStart(TreeCCContext *context, TreeCCStream *stream,
					      TreeCCOperation *oper)
{
	if(oper->className)
	{
		TreeCCStreamPrint(stream, "class %s\n", oper->className);
	}
	else
	{
		TreeCCStreamPrint(stream, "class %s\n", oper->name);
	}
}

/*
 * Generate the entry point for a non-virtual operation.
 */
static void RubyGenEntry(TreeCCContext *context, TreeCCStream *stream,
					     TreeCCOperation *oper, int number)
{
	TreeCCParam *param;
	int num;
	int needComma;
	char *cname;

	if(oper->className)
	{
		cname = oper->className;
	}
	else
	{
		cname = oper->name;
	}	

	if(number != -1)
	{
		TreeCCStreamPrint(stream, "  private \n  def %s.%s_split_%d__(",
						  cname, oper->name, number);
	}
	else
	{
		TreeCCStreamPrint(stream, "  public \n  def %s.%s(",
						  cname, oper->name);
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
		if(param->name)
		{
			TreeCCStreamPrint(stream, "%s", param->name);
		}
		else
		{
			TreeCCStreamPrint(stream, "p%d__", num);
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
}

/*
 * Generate the entry point for a non-virtual operation.
 */
static void Ruby_GenEntry(TreeCCContext *context, TreeCCStream *stream,
					      TreeCCOperation *oper)
{
	RubyGenEntry(context, stream, oper, -1);
}

/*
 * Generate the entry point for a split-out function.
 */
static void Ruby_GenSplitEntry(TreeCCContext *context, TreeCCStream *stream,
					           TreeCCOperation *oper, int number)
{
	RubyGenEntry(context, stream, oper, number);
}

/*
 * Generate the head of a "switch" statement.
 */
static void Ruby_GenSwitchHead(TreeCCContext *context, TreeCCStream *stream,
							   char *paramName, int level, int isEnum)
{
	Indent(stream, level * 2 + 2);
	if(isEnum)
	{
		TreeCCStreamPrint(stream, "case %s\n", paramName);
	}
	else
	{
		TreeCCStreamPrint(stream, "case %s__.type::KIND\n", paramName);
	}
	Indent(stream, level * 2 + 2);
	TreeCCStreamPrint(stream, "\n");
}

/*
 * Generate a selector for a "switch" case.
 */
static void Ruby_GenSelector(TreeCCContext *context, TreeCCStream *stream,
						     TreeCCNode *node, int level)
{
	if((node->flags & TREECC_NODE_ENUM_VALUE) != 0)
	{
		Indent(stream, level * 2 + 3);
		TreeCCStreamPrint(stream, "when %s.%s\n",
						  node->parent->name, node->name);
	}
	else if((node->flags & TREECC_NODE_ENUM) == 0)
	{
		Indent(stream, level * 2 + 3);
		TreeCCStreamPrint(stream, "when %s::KIND\n", node->name);
	}
}

/*
 * Terminate the selectors and begin the body of a "switch" case.
 */
static void Ruby_GenEndSelectors(TreeCCContext *context, TreeCCStream *stream,
							     int level)
{
	/* No use for this in Ruby */
}

/*
 * Generate the code for a case within a function.
 */
static void Ruby_GenCaseFunc(TreeCCContext *context, TreeCCStream *stream,
						     TreeCCOperationCase *operCase, int number)
{
	TreeCCParam *param;
	TreeCCTrigger *trigger;
	int num;
	int needComma;
	char *type;
	char *cname;

	if(operCase->oper->className)
	{
		cname = operCase->oper->className;
	}
	else
	{
		cname = operCase->oper->name;
	}	

	/* Output the header for the function */
	TreeCCStreamPrint(stream, "  private \n  def %s.%s_%d__(",
					  cname, operCase->oper->name, number);
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
			TreeCCStreamPrint(stream, "%s", param->name);
		}
		else
		{
			TreeCCStreamPrint(stream, "p%d__", num);
			++num;
		}
		needComma = 1;
		param = param->next;
	}
	TreeCCStreamPrint(stream, ")\n");

	/* Output the code for the operation case */
	if(operCase->code)
	{
		TreeCCStreamCodeIndent(stream, operCase->code, 1);
	}
	TreeCCStreamPrint(stream, "end\n");
	TreeCCStreamPrint(stream, "\n");
}

/*
 * Generate a call to a case function from within the "switch".
 */
static void Ruby_GenCaseCall(TreeCCContext *context, TreeCCStream *stream,
						     TreeCCOperationCase *operCase, int number,
						     int level)
{
	TreeCCParam *param;
	TreeCCTrigger *trigger;
	int num;
	int needComma;

	/* Indent to the correct level */
	Indent(stream, level * 2 + 1);

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
/*		if((param->flags & TREECC_PARAM_TRIGGER) != 0)
		{
			if((trigger->node->flags & TREECC_NODE_ENUM) == 0 &&
		   	   (trigger->node->flags & TREECC_NODE_ENUM_VALUE) == 0)
			{
				TreeCCStreamPrint(stream, "(%s)", trigger->node->name);
			}
		}*/
		if(param->name)
		{
			TreeCCStreamPrint(stream, "%s", param->name);
		}
		else
		{
			TreeCCStreamPrint(stream, "p%d__", num);
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
	TreeCCStreamPrint(stream, ")\n");
}

/*
 * Generate the code for a case inline within the "switch".
 */
static void Ruby_GenCaseInline(TreeCCContext *context, TreeCCStream *stream,
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
					TreeCCStreamPrint(stream, "%s = %s__;\n",
									  param->name, param->name);
				}
			}
			trigger = trigger->next;
		}
		param = param->next;
	}

	/* Output the inline code for the case */
	Indent(stream, level * 2 + 4);
	if(operCase->code)
	{
		/* Multiply the indent level by two because every ident is one space */
		TreeCCStreamCodeIndentCustom
			(stream, operCase->code,' ',(level * 2 + 3) * 2);		
	}
	Indent(stream, level * 2 + 4);
	TreeCCStreamPrint(stream, "\n");
}

/*
 * Generate a call to a split function from within the "switch".
 */
static void Ruby_GenCaseSplit(TreeCCContext *context, TreeCCStream *stream,
						      TreeCCOperationCase *operCase,
							  int number, int level)
{
	TreeCCParam *param;
	TreeCCTrigger *trigger;
	int num;
	int needComma;

	/* Indent to the correct level */
	Indent(stream, level * 2 + 2);

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
				TreeCCStreamPrint(stream, "%s", trigger->node->name);
			}
		}
		if(param->name)
		{
			TreeCCStreamPrint(stream, "%s", param->name);
		}
		else
		{
			TreeCCStreamPrint(stream, "p%d__", num);
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
	TreeCCStreamPrint(stream, ")\n");
}

/*
 * Terminate a "switch" case.
 */
static void Ruby_GenEndCase(TreeCCContext *context, TreeCCStream *stream,
						    int level)
{
	/*Indent(stream, level * 2 + 3);
	TreeCCStreamPrint(stream, "\n");*/
}

/*
 * Terminate the "switch" statement.
 */
static void Ruby_GenEndSwitch(TreeCCContext *context, TreeCCStream *stream,
						      int level)
{
	Indent(stream, level * 2 + 3);
	TreeCCStreamPrint(stream, "else\n");
	Indent(stream, level * 2 + 2);
	TreeCCStreamPrint(stream, "end\n");
}

/*
 * Generate the exit point for a non-virtual operation.
 */
static void Ruby_GenExit(TreeCCContext *context, TreeCCStream *stream,
					     TreeCCOperation *oper)
{
	if(strcmp(oper->returnType, "void") != 0)
	{
		/* Generate a default return value for the function */
		if(oper->defValue)
		{
			TreeCCStreamPrint(stream, "  return %s\n", oper->defValue);
		}
		else
		{
			TreeCCStreamPrint(stream, "  return 0\n");
		}
	}
	TreeCCStreamPrint(stream, "  end\n");
}

/*
 * Generate the end declarations for a non-virtual operation.
 */
static void Ruby_GenEnd(TreeCCContext *context, TreeCCStream *stream,
					    TreeCCOperation *oper)
{
	TreeCCStreamPrint(stream, "end\n");
}

/*
 * Table of non-virtual code generation functions.
 */
TreeCCNonVirtual const TreeCCNonVirtualFuncsRuby = {
	Ruby_GenStart,
	Ruby_GenEntry,
	Ruby_GenSplitEntry,
	Ruby_GenSwitchHead,
	Ruby_GenSelector,
	Ruby_GenEndSelectors,
	Ruby_GenCaseFunc,
	Ruby_GenCaseCall,
	Ruby_GenCaseInline,
	Ruby_GenCaseSplit,
	Ruby_GenEndCase,
	Ruby_GenEndSwitch,
	Ruby_GenExit,
	Ruby_GenEnd,
};

void TreeCCGenerateRuby(TreeCCContext *context)
{
	/* Write all stream headers */
	WriteRubyHeaders(context);

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
	/* NO JAVA PLEASE */
	TreeCCGenerateNonVirtuals(context, &TreeCCNonVirtualFuncsRuby);
	

	/* Write all stream footers */
	WriteRubyFooters(context);
}

#ifdef	__cplusplus
};
#endif
