/*
 * parse.c - Parse "treecc" input files.
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

#include "parse.h"
#include "errors.h"
#include "options.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Append two strings and return a malloc'ed result.
 */
static char *AppendStrings(TreeCCInput *input, char *str1,
						   char *str2, int sep, int free2)
{
	int len1 = strlen(str1);
	int len2 = strlen(str2);
	int len = len1 + len2 + sep + 1;
	char *value = (char *)realloc(str1, len);
	if(!value)
	{
		TreeCCOutOfMemory(input);
	}
	if(sep)
	{
		value[len1] = ' ';
		strcpy(value + len1 + 1, str2);
		value[len1 + len2 + 1] = '\0';
	}
	else
	{
		strcpy(value + len1, str2);
		value[len1 + len2] = '\0';
	}
	if(free2)
	{
		free(str2);
	}
	return value;
}

/*
 * Parse a type specification and variable name using a
 * simplified subset of C type syntax.
 *
 * TypeAndName ::= Type [IDENTIFIER]
 *
 * Type ::= TypeName
 *        | Type '*'
 *        | Type '&'
 *        | Type '[' ']'
 * TypeName ::= IDENTIFIER { IDENTIFIER }
 *
 * Examples are "expression", "const char *", and "Item[]".
 * Types that don't fit this pattern should use typedef'ed
 * names from the underlying source language.
 */
static void ParseTypeAndName(TreeCCInput *input, char **type, char **name)
{
	char *tempType;
	char *last;

	/* Type names must begin with an identifier */
	if(input->token != TREECC_TOKEN_IDENTIFIER)
	{
		TreeCCError(input, "type name expected");
		*type = 0;
		*name = 0;
		return;
	}

	/* Collect up the identifiers within the type name.
	   We don't add the last to "tempType" until we've
	   seen what comes after it.  This resolves the
	   ambiguity in the grammar */
	tempType = 0;
	last = 0;
	while(input->token == TREECC_TOKEN_IDENTIFIER)
	{
		if(!tempType)
		{
			/* This is the first identifier, which is
			   always part of the type */
			tempType = TreeCCValue(input);
		}
		else if(!last)
		{
			/* No last identifier yet, so just save this one */
			last = TreeCCValue(input);
		}
		else
		{
			/* Append "last" to "tempType" */
			tempType = AppendStrings(input, tempType, last, 1, 1);

			/* Save the new identifier as "last" */
			last = TreeCCValue(input);
		}
		TreeCCNextToken(input);
	}

	/* If the next token is '*', '&', or '[', then "last" is
	   part of the type name, and not the variable name */
	if(last && (input->token == TREECC_TOKEN_STAR ||
				input->token == TREECC_TOKEN_REF ||
	            input->token == TREECC_TOKEN_LSQUARE))
	{
		tempType = AppendStrings(input, tempType, last, 1, 1);
		last = 0;
	}

	/* Parse the type suffixes */
	while(input->token == TREECC_TOKEN_STAR ||
		  input->token == TREECC_TOKEN_REF ||
		  input->token == TREECC_TOKEN_LSQUARE)
	{
		if(input->token == TREECC_TOKEN_STAR)
		{
			tempType = AppendStrings(input, tempType, "*", 1, 0);
		}
		else if(input->token == TREECC_TOKEN_REF)
		{
			tempType = AppendStrings(input, tempType, "&", 1, 0);
		}
		else
		{
			tempType = AppendStrings(input, tempType, "[]", 0, 0);
			TreeCCNextToken(input);
			if(input->token != TREECC_TOKEN_RSQUARE)
			{
				TreeCCError(input, "`]' expected");
				continue;
			}
		}
		TreeCCNextToken(input);
	}

	/* Parse the variable name, if necessary */
	if(!last && input->token == TREECC_TOKEN_IDENTIFIER)
	{
		last = TreeCCValue(input);
		TreeCCNextToken(input);
	}

	/* Return the values to the caller */
	*type = tempType;
	*name = last;
}

/*
 * Parse a node definition.
 *
 * Node ::= %node IDENTIFIER [ IDENTIFIER ] { NodeFlag } [ "=" Fields ]
 * NodeFlag ::= %abstract | %typedef
 * Fields ::= "{" { Field } "}"
 * Field ::= [ %nocreate ] TypeAndName [ "=" LITERAL_CODE ] ";"
 */
static void ParseNode(TreeCCContext *context)
{
	TreeCCInput *input = context->input;
	char *name;
	char *parent;
	int flags;
	long linenum;
	TreeCCNode *node;
	char *fieldName;
	char *fieldType;
	char *fieldValue;

	/* Skip the "%node" keyword */
	TreeCCNextToken(input);
	linenum = input->linenum;

	/* We need an identifier for the node that is being declared */
	if(input->token != TREECC_TOKEN_IDENTIFIER)
	{
		/* Report an error for the identifier */
		TreeCCError(input, "identifier expected");
		return;
	}
	name = TreeCCValue(input);
	TreeCCNextToken(input);

	/* Get the name of the parent */
	if(input->token == TREECC_TOKEN_IDENTIFIER)
	{
		parent = TreeCCValue(input);
		TreeCCNextToken(input);
	}
	else
	{
		parent = 0;
	}

	/* Parse the node definition flags */
	flags = 0;
	for(;;)
	{
		if(input->token == TREECC_TOKEN_ABSTRACT)
		{
			flags |= TREECC_NODE_ABSTRACT;
			TreeCCNextToken(input);
		}
		else if(input->token == TREECC_TOKEN_TYPEDEF)
		{
			flags |= TREECC_NODE_TYPEDEF;
			TreeCCNextToken(input);
		}
		else
		{
			break;
		}
	}

	/* Create the node */
	node = TreeCCNodeCreate(context, linenum, name, parent, flags);

	/* If we have a field definition block, then read it */
	if(input->token == TREECC_TOKEN_EQUALS)
	{
		input->parseLiteral = 0;
		TreeCCNextToken(input);
		input->parseLiteral = 1;
		if(input->token == TREECC_TOKEN_LBRACE)
		{
			/* Skip the "{" token */
			TreeCCNextToken(input);

			/* Process the field definitions */
			while(input->token != TREECC_TOKEN_RBRACE &&
				  input->token != TREECC_TOKEN_EOF)
			{
				/* Skip spurious semi-colons in the field definition */
				if(input->token == TREECC_TOKEN_SEMI)
				{
					TreeCCNextToken(input);
					continue;
				}

				/* Process field definition flags */
				if(input->token == TREECC_TOKEN_NOCREATE)
				{
					flags = TREECC_FIELD_NOCREATE;
					TreeCCNextToken(input);
				}
				else
				{
					flags = 0;
				}

				/* Parse the field type and name */
				ParseTypeAndName(input, &fieldType, &fieldName);
				if(fieldType && fieldName)
				{
					if(input->token == TREECC_TOKEN_EQUALS)
					{
						TreeCCNextToken(input);
						if(input->token == TREECC_TOKEN_LITERAL_CODE)
						{
							fieldValue = TreeCCValue(input);
							TreeCCNextToken(input);
						}
						else
						{
							TreeCCError
								(input, "literal code constant expected");
							fieldValue = 0;
						}
					}
					else
					{
						fieldValue = 0;
					}
					if(fieldValue && (flags & TREECC_FIELD_NOCREATE) == 0)
					{
						TreeCCError(input,
							"default values can only be specified for "
							"`%%nocreate' fields");
					}
					TreeCCFieldCreate(context, node, fieldName, fieldType,
									  fieldValue, flags);
				}
				else
				{
					if(fieldType)
					{
						TreeCCError(input, "field name expected");
					}
					else
					{
						TreeCCError(input, "field declaration expected");
					}
					if(fieldType)
					{
						free(fieldType);
					}
					if(fieldName)
					{
						free(fieldName);
					}

					/* Attempt to recover from the error */
					while(input->token != TREECC_TOKEN_SEMI &&
						  input->token != TREECC_TOKEN_RBRACE &&
						  input->token != TREECC_TOKEN_IDENTIFIER &&
						  input->token != TREECC_TOKEN_EOF)
					{
						TreeCCNextToken(input);
					}
				}

				/* Recognize the ";" at the end of the field */
				if(input->token == TREECC_TOKEN_SEMI)
				{
					TreeCCNextToken(input);
				}
				else
				{
					/* Attempt to recover from the lack of a semi-colon */
					TreeCCError(input, "`;' expected");
					if(input->token != TREECC_TOKEN_IDENTIFIER &&
					   input->token != TREECC_TOKEN_RBRACE)
					{
						TreeCCNextToken(input);
					}
				}
			}

			/* Skip the "}" token */
			TreeCCNextToken(input);
		}
		else
		{
			TreeCCError(input, "field definition block expected");
		}
	}
}

/*
 * Validate trigger type suffixes against the language.
 */
static void ValidateSuffixes(TreeCCContext *context, const char *type,
						     TreeCCNode *node, char *filename, long linenum)
{
	int len = strlen(type);
	if(context->language == TREECC_LANG_C ||
	   context->language == TREECC_LANG_CPP)
	{
		if((node->flags & TREECC_NODE_ENUM) == 0 &&
		   (node->flags & TREECC_NODE_ENUM_VALUE) == 0)
		{
			if(len < 2 || type[len - 1] != '*' ||
			   type[len - 2] != ' ')
			{
				TreeCCErrorOnLine(context->input, filename, linenum,
								  "trigger types must end in `*'");
			}
		}
	}
}

/*
 * Parse an operation definition.
 *
 * Operation ::= %operation { OperFlag } Type [ClassName ] IDENTIFIER
 *                  '(' [ Params ] ')' [ '=' LITERAL_CODE ] [ ';' ]
 * OperFlag ::= %virtual | %inline
 * ClassName ::= IDENTIFIER "::"
 * Params ::= Param { ',' Param }
 * Param ::= TypeAndName | '[' TypeAndName ']'
 */
static void ParseOperation(TreeCCContext *context)
{
	TreeCCInput *input = context->input;
	int flags;
	char *returnType;
	char *name;
	char *className;
	char *defValue;
	TreeCCParam *params = 0;
	TreeCCParam *lastParam = 0;
	TreeCCParam *newParam;
	int numTriggers = 0;
	char *paramType;
	char *paramName;
	int paramFlags;
	TreeCCOperation *oper;
	char *filename;
	long linenum;
	TreeCCNode *typeNode;

	/* Skip the "%operation" keyword */
	TreeCCNextToken(input);

	/* Parse the operation flags */
	flags = 0;
	for(;;)
	{
		if(input->token == TREECC_TOKEN_VIRTUAL)
		{
			flags |= TREECC_OPER_VIRTUAL;
			TreeCCNextToken(input);
		}
		else if(input->token == TREECC_TOKEN_INLINE)
		{
			flags |= TREECC_OPER_INLINE;
			TreeCCNextToken(input);
		}
		else if(input->token == TREECC_TOKEN_SPLIT)
		{
			flags |= TREECC_OPER_SPLIT;
			TreeCCNextToken(input);
		}
		else
		{
			break;
		}
	}
	if((flags & TREECC_OPER_VIRTUAL) != 0 &&
	   (flags & TREECC_OPER_INLINE) != 0)
	{
		TreeCCError(input, "`virtual' and `inline' cannot be used together");
		flags &= ~TREECC_OPER_INLINE;
	}

	/* Parse the return type and name of the operation */
	ParseTypeAndName(input, &returnType, &name);
	filename = input->filename;
	linenum = input->linenum;
	if(!returnType || !name)
	{
		if(returnType)
		{
			TreeCCError(input, "operation name expected");
		}
		else
		{
			TreeCCError(input, "operation return type expected");
		}
		if(returnType)
		{
			free(returnType);
		}
		if(name)
		{
			free(name);
		}
		return;
	}

	/* If the next token is "::", then "name" is actually the
	   class name and we need to get the real operation name */
	if(input->token == TREECC_TOKEN_COLON_COLON)
	{
		className = name;
		TreeCCNextToken(input);
		if(input->token != TREECC_TOKEN_IDENTIFIER)
		{
			TreeCCError(input, "operation name expected");
			free(returnType);
			free(className);
			return;
		}
		name = TreeCCValue(input);
		TreeCCNextToken(input);
	}
	else
	{
		className = 0;
	}

	/* If the operation is non-virtual, and the language is C#,
	   then we must have a class name which is different from
	   the operation name.  This is necessary because of a
	   "feature" in the design of C# and Microsoft's compilers */
	if((flags & TREECC_OPER_VIRTUAL) == 0 &&
	   context->language == TREECC_LANG_CSHARP)
	{
		if(!className)
		{
			TreeCCErrorOnLine(input, filename, linenum,
							  "C# requires that a class name be specified");
		}
		else if(!strcmp(className, name))
		{
			TreeCCErrorOnLine(input, filename, linenum,
				  "C# requires different class and operation names");
		}
	}

	/* Parse the parameter list for the operation */
	if(input->token == TREECC_TOKEN_LPAREN)
	{
		TreeCCNextToken(input);
		while(input->token == TREECC_TOKEN_IDENTIFIER ||
		      input->token == TREECC_TOKEN_LSQUARE)
		{
			/* Parse the parameter information */
			if(input->token == TREECC_TOKEN_IDENTIFIER)
			{
				paramFlags = 0;
				ParseTypeAndName(input, &paramType, &paramName);
			}
			else
			{
				paramFlags = TREECC_PARAM_TRIGGER;
				TreeCCNextToken(input);
				ParseTypeAndName(input, &paramType, &paramName);
				if(input->token == TREECC_TOKEN_RSQUARE)
				{
					TreeCCNextToken(input);
				}
				else
				{
					TreeCCError(input, "`]' expected");
				}
				++numTriggers;
			}

			/* Create the parameter block */
			if(!paramType)
			{
				TreeCCError(input, "parameter declaration expected");
			}
			else if(!strcmp(paramType, "void"))
			{
				/* Ignore "void" parameters */
				free(paramType);
				if(paramName)
				{
					free(paramName);
				}
			}
			else
			{
				newParam = (TreeCCParam *)malloc(sizeof(TreeCCParam));
				if(!newParam)
				{
					TreeCCOutOfMemory(input);
				}
				newParam->name = paramName;
				newParam->type = paramType;
				newParam->flags = paramFlags;
				newParam->size = 0;
				newParam->next = 0;
				if(lastParam)
				{
					lastParam->next = newParam;
				}
				else
				{
					params = newParam;
				}
				lastParam = newParam;
				if((paramFlags & TREECC_PARAM_TRIGGER) != 0)
				{
					if((typeNode = TreeCCNodeFindByType(context, paramType))
								== 0)
					{
						TreeCCError(input, "`%s' is not a valid trigger type",
									paramType);
					}
					else
					{
						ValidateSuffixes(context, paramType, typeNode,
										 input->filename, input->linenum);
					}
				}
			}

			/* Skip the comma and go on to the next parameter */
			if(input->token == TREECC_TOKEN_COMMA)
			{
				TreeCCNextToken(input);
				if(input->token != TREECC_TOKEN_IDENTIFIER &&
				   input->token != TREECC_TOKEN_LSQUARE)
				{
					TreeCCError(input, "parameter declaration expected");
				}
			}
			else
			{
				break;
			}
		}
		if(input->token == TREECC_TOKEN_RPAREN)
		{
			TreeCCNextToken(input);
		}
		else
		{
			TreeCCError(input, "`)' expected");
		}
	}
	else
	{
		TreeCCError(input, "`(' expected");
	}

	/* Recognise the default value, if present */
	if(input->token == TREECC_TOKEN_EQUALS)
	{
		TreeCCNextToken(input);
		if(input->token == TREECC_TOKEN_LITERAL_CODE)
		{
			defValue = TreeCCValue(input);
			TreeCCNextToken(input);
		}
		else
		{
			TreeCCError(input, "default value expected");
			defValue = 0;
		}
	}
	else
	{
		defValue = 0;
		if((flags & TREECC_OPER_VIRTUAL) == 0 &&
		   strcmp(returnType, "void") != 0)
		{
			TreeCCError(input, "default value required");
		}
	}

	/* Recognise an optional semi-colon */
	if(input->token == TREECC_TOKEN_SEMI)
	{
		TreeCCNextToken(input);
	}

	/* Set a default trigger parameter if necessary */
	if(numTriggers == 0 && params != 0)
	{
		numTriggers = 1;
		params->flags |= TREECC_PARAM_TRIGGER;
		if((typeNode = TreeCCNodeFindByType(context, params->type)) == 0)
		{
			TreeCCErrorOnLine(input, filename, linenum,
							  "`%s' is not a valid trigger type",
							  params->type);
		}
		else
		{
			ValidateSuffixes(context, params->type, typeNode,
							 filename, linenum);
		}
	}

	/* If the operation is virtual, then check that the first
	   parameter is the sole trigger for the operation */
	if((flags & TREECC_OPER_VIRTUAL) != 0)
	{
		if(!params || numTriggers != 1 ||
		   (params->flags & TREECC_PARAM_TRIGGER) == 0)
		{
			if(!params)
			{
				TreeCCError(input,
					"virtual operations must have at least one parameter");
			}
			else
			{
				TreeCCError(input,
					"the first parameter of a virtual must be the trigger");
			}
			flags &= ~TREECC_OPER_VIRTUAL;
		}
		else if((typeNode = TreeCCNodeFindByType(context, params->type)) == 0)
		{
			flags &= ~TREECC_OPER_VIRTUAL;
		}
		else if((typeNode->flags & TREECC_NODE_ENUM) != 0)
		{
			TreeCCError(input,
			  "cannot use enumerated types as triggers for virtual operations");
			flags &= ~TREECC_OPER_VIRTUAL;
		}
	}

	/* See if we already have an operation with this name */
	oper = TreeCCOperationFind(context, name);
	if(oper != 0)
	{
		TreeCCErrorOnLine(input, filename, linenum,
						  "operation `%s' is already declared", name);
		TreeCCErrorOnLine(input, oper->filename, oper->linenum,
						  "previous declaration here");
		free(returnType);
		free(name);
		if(className)
		{
			free(className);
		}
		if(defValue)
		{
			free(defValue);
		}
		while(params != 0)
		{
			newParam = params->next;
			if(params->name)
			{
				free(params->name);
			}
			if(params->type)
			{
				free(params->type);
			}
			free(params);
			params = newParam;
		}
		return;
	}

	/* Create the operation */
	oper = TreeCCOperationCreate(context, returnType, name, className,
								 defValue, params, flags, numTriggers,
								 filename, linenum);

	/* If the operation is virtual, then attach it to the node type */
	if((flags & TREECC_OPER_VIRTUAL) != 0)
	{
		TreeCCNodeAddVirtual
			(context, TreeCCNodeFindByType(context, params->type), oper);
	}
}

/*
 * Parse an operation case header.
 *
 * OperationHead ::= IDENTIFIER '(' [ TypeList ] ')'
 * TypeList ::= IDENTIFIER { ',' IDENTIFIER }
 */
static TreeCCOperationCase *ParseOperationHeader(TreeCCContext *context)
{
	TreeCCInput *input = context->input;
	TreeCCTrigger *triggers = 0;
	TreeCCTrigger *lastTrigger = 0;
	TreeCCTrigger *newTrigger;
	TreeCCOperation *oper;
	TreeCCNode *node;
	int fatalError = 0;
	int numTriggers = 0;
	char *filename;
	long linenum;

	/* Look up the operation name */
	filename = input->filename;
	linenum = input->linenum;
	oper = TreeCCOperationFind(context, input->text);
	if(!oper)
	{
		TreeCCError(input, "operation `%s' is not declared", input->text);
		fatalError = 1;
	}
	TreeCCNextToken(input);

	/* Parse the type trigger list */
	if(input->token == TREECC_TOKEN_LPAREN)
	{
		TreeCCNextToken(input);
		while(input->token == TREECC_TOKEN_IDENTIFIER)
		{
			++numTriggers;
			node = TreeCCNodeFind(context, input->text);
			if(!node)
			{
				TreeCCError(input, "node type `%s' is not declared",
							input->text);
				fatalError = 1;
			}
			newTrigger = (TreeCCTrigger *)malloc(sizeof(TreeCCTrigger));
			if(!newTrigger)
			{
				TreeCCOutOfMemory(input);
			}
			newTrigger->node = node;
			newTrigger->next = 0;
			if(lastTrigger)
			{
				lastTrigger->next = newTrigger;
			}
			else
			{
				triggers = newTrigger;
			}
			lastTrigger = newTrigger;
			TreeCCNextToken(input);
			if(input->token == TREECC_TOKEN_COMMA)
			{
				TreeCCNextToken(input);
				if(input->token != TREECC_TOKEN_IDENTIFIER)
				{
					TreeCCError(input, "type name expected");
				}
			}
			else
			{
				if(input->token != TREECC_TOKEN_RPAREN)
				{
					TreeCCError(input, "`,' expected");
				}
				break;
			}
		}
		if(input->token == TREECC_TOKEN_RPAREN)
		{
			TreeCCNextToken(input);
		}
		else
		{
			TreeCCError(input, "`)' expected");
		}
	}
	else
	{
		TreeCCError(input, "`(' expected");
	}

	/* Check that we have a trigger match for the operation */
	if(oper)
	{
		if(oper->numTriggers != numTriggers)
		{
			TreeCCErrorOnLine(input, filename, linenum,
							  "incorrect number of triggers for operation");
			TreeCCErrorOnLine(input, oper->filename, oper->linenum,
							  "operation declared here");
			fatalError = 1;
		}
		else
		{
			TreeCCParam *param = oper->params;
			int reportedError = 0;
			newTrigger = triggers;
			while(param != 0)
			{
				if((param->flags & TREECC_PARAM_TRIGGER) != 0)
				{
					node = TreeCCNodeFindByType(context, param->type);
					if(node && !TreeCCNodeInheritsFrom(newTrigger->node, node))
					{
						if(newTrigger->node)
						{
							TreeCCErrorOnLine(input, filename, linenum,
								"node type `%s' does not inherit from `%s'",
								newTrigger->node->name, node->name);
							reportedError = 1;
							fatalError = 1;
						}
					}
					newTrigger = newTrigger->next;
				}
				param = param->next;
			}
			if(reportedError)
			{
				TreeCCErrorOnLine(input, oper->filename, oper->linenum,
								  "operation declared here");
			}
		}
	}

	/* Bail out if we've seen a fatal error */
	if(fatalError)
	{
		while(triggers != 0)
		{
			newTrigger = triggers->next;
			free(triggers);
			triggers = newTrigger;
		}
		return 0;
	}

	/* Add the operation case to the operation */
	return TreeCCOperationAddCase(context, oper, triggers, filename, linenum);
}

/*
 * Parse an operation case definition.
 *
 * OperationCase ::= OperationHead { ',' OperationHead } LiteralCode
 */
static void ParseOperationCase(TreeCCContext *context)
{
	TreeCCInput *input = context->input;
	TreeCCOperationCase *operCase;
	TreeCCOperationCase *caseList;
	char *code;
	char *codeFilename;
	long codeLinenum;

	/* Parse the operation case headers */
	caseList = 0;
	while(input->token == TREECC_TOKEN_IDENTIFIER)
	{
		operCase = ParseOperationHeader(context);
		if(operCase)
		{
			operCase->nextHeader = caseList;
			caseList = operCase;
		}
		if(input->token == TREECC_TOKEN_IDENTIFIER)
		{
			TreeCCError(input, "`,' expected");
			continue;
		}
		if(input->token != TREECC_TOKEN_COMMA)
		{
			break;
		}
		TreeCCNextToken(input);
		if(input->token != TREECC_TOKEN_IDENTIFIER)
		{
			TreeCCError(input, "identifier expected");
		}
	}

	/* Recognise the code for the operation case */
	codeFilename = input->filename;
	codeLinenum = input->linenum;
	if(input->token == TREECC_TOKEN_LITERAL_CODE)
	{
		code = TreeCCValue(input);
		TreeCCNextToken(input);
	}
	else
	{
		code = 0;
		TreeCCError(input, "code block expected");
	}

	/* Add the code to all of the operation cases */
	if(code)
	{
		operCase = caseList;
		while(operCase != 0)
		{
			operCase->code = code;
			operCase->codeFilename = codeFilename;
			operCase->codeLinenum = codeLinenum;
			operCase = operCase->nextHeader;
		}
	}
}

/*
 * Parse an option declaration.
 *
 * Option ::= %option IDENTIFIER [ '=' Value ]
 * Value ::= IDENTIFIER | STRING
 */
static void ParseOption(TreeCCContext *context)
{
	TreeCCInput *input = context->input;
	char *name;
	char *value;
	char *filename;
	long linenum;
	int optValue;

	/* Skip the "%option" keyword */
	TreeCCNextToken(input);

	/* Recognise the option name */
	if(input->token != TREECC_TOKEN_IDENTIFIER)
	{
		TreeCCError(input, "option name expected");
		return;
	}
	name = TreeCCValue(input);
	filename = input->filename;
	linenum = input->linenum;
	TreeCCNextToken(input);

	/* Recognise the option value, if present */
	if(input->token == TREECC_TOKEN_EQUALS)
	{
		TreeCCNextToken(input);
		if(input->token == TREECC_TOKEN_IDENTIFIER ||
		   input->token == TREECC_TOKEN_STRING)
		{
			value = TreeCCValue(input);
			TreeCCNextToken(input);
		}
		else
		{
			TreeCCError(input, "identifier or string expected");
			value = 0;
		}
	}
	else
	{
		value = 0;
	}

	/* Dump the option to the debug stream if necessary */
	if(context->debugMode)
	{
		if(value)
		{
			TreeCCDebug(linenum, "%%option %s %s", name, value);
		}
		else
		{
			TreeCCDebug(linenum, "%%option %s", name);
		}
	}

	/* Process the option */
	optValue = TreeCCOptionProcess(context, name, value);
	if(optValue == TREECC_OPT_UNKNOWN)
	{
		TreeCCErrorOnLine(input, filename, linenum,
						  "unknown option `%s'", name);
	}
	else if(optValue == TREECC_OPT_INVALID_VALUE)
	{
		TreeCCErrorOnLine(input, filename, linenum,
						  "invalid value for option `%s'", name);
	}
	else if(optValue == TREECC_OPT_NEED_VALUE)
	{
		TreeCCErrorOnLine(input, filename, linenum,
						  "option `%s' requires a value", name);
	}
	else if(optValue == TREECC_OPT_NO_VALUE)
	{
		TreeCCErrorOnLine(input, filename, linenum,
						  "option `%s' does not take a value", name);
	}

	/* Clean up the memory that we used */
	free(name);
	if(optValue != TREECC_OPT_KEEP_VALUE && value)
	{
		free(value);
	}
}

/*
 * Parse an enumerated type declaration.
 *
 * Enum ::= %enum IDENTIFIER '=' EnumBody
 * EnumBody ::= '{' IDENTIFIER { ',' IDENTIFIER } [ ',' ] '}'
 */
static void ParseEnum(TreeCCContext *context)
{
	TreeCCInput *input = context->input;
	char *name;
	TreeCCNode *node;
	int sawValue;

	/* Skip the "%enum" keyword */
	TreeCCNextToken(input);

	/* Get the name of the enumerated type */
	if(input->token != TREECC_TOKEN_IDENTIFIER)
	{
		TreeCCError(input, "enumerated type name expected");
		return;
	}
	name = TreeCCValue(input);

	/* Create a node for the enumerated type */
	node = TreeCCNodeCreate(context, input->linenum, name, 0,
					 		TREECC_NODE_ABSTRACT |
					 		TREECC_NODE_TYPEDEF |
					 		TREECC_NODE_ENUM);

	/* Parse the start of the enumeration body */
	TreeCCNextToken(input);
	if(input->token != TREECC_TOKEN_EQUALS)
	{
		TreeCCError(input, "`=' expected");
		return;
	}
	input->parseLiteral = 0;
	TreeCCNextToken(input);
	input->parseLiteral = 1;
	if(input->token != TREECC_TOKEN_LBRACE)
	{
		TreeCCError(input, "`{' expected");
		return;
	}
	TreeCCNextToken(input);

	/* Parse the enumeration body */
	sawValue = 0;
	while(input->token == TREECC_TOKEN_IDENTIFIER)
	{
		TreeCCNodeCreate(context, input->linenum,
						 TreeCCValue(input),
						 TreeCCDupString(node->name),
						 TREECC_NODE_ENUM_VALUE);
		sawValue = 1;
		TreeCCNextToken(input);
		if(input->token == TREECC_TOKEN_COMMA)
		{
			TreeCCNextToken(input);
		}
		else if(input->token == TREECC_TOKEN_IDENTIFIER)
		{
			TreeCCError(input, "`,' expected");
		}
		else
		{
			break;
		}
	}
	if(input->token == TREECC_TOKEN_COMMA)
	{
		/* Permit an extra trailing comma */
		TreeCCNextToken(input);
	}
	if(!sawValue)
	{
		TreeCCError(input, "no values were specified for the enumeration");
	}

	/* Recognise the end of the enumeration body */
	if(input->token == TREECC_TOKEN_RBRACE)
	{
		TreeCCNextToken(input);
	}
	else
	{
		TreeCCError(input, "`}' expected");
	}
}

/*
 * Determine if a token is the start of a declaration.
 */
#define	IsStartDecl(token)	\
			((token) == TREECC_TOKEN_EOF || \
			 (token) == TREECC_TOKEN_IDENTIFIER || \
			 (token) == TREECC_TOKEN_LITERAL_DEFNS || \
			 (token) == TREECC_TOKEN_LITERAL_END || \
			 (token) == TREECC_TOKEN_NODE || \
			 (token) == TREECC_TOKEN_OPERATION || \
			 (token) == TREECC_TOKEN_OPTION || \
			 (token) == TREECC_TOKEN_HEADER || \
			 (token) == TREECC_TOKEN_OUTPUT || \
			 (token) == TREECC_TOKEN_BOTH || \
			 (token) == TREECC_TOKEN_DECLS || \
			 (token) == TREECC_TOKEN_END || \
			 (token) == TREECC_TOKEN_ENUM || \
			 (token) == TREECC_TOKEN_COMMON || \
			 (token) == TREECC_TOKEN_INCLUDE)

/*
 * File ::= { Declaration }
 * Declaration ::= Node
 *               | Operation
 *               | OperationCase
 *               | Option
 *               | Enum
 *               | Literal
 *               | Header
 *               | Output
 *               | Common
 *               | Include
 * Literal ::= { LiteralFlag } ( LITERAL_DEFNS | LITERAL_END )
 * LiteralFlag ::= %both | %decls | %end
 * Header ::= %header STRING
 * Output ::= %output STRING
 * Common ::= %common
 * Include ::= %include [ %readonly ] STRING
 */
void TreeCCParse(TreeCCContext *context)
{
	/* Fetch the first token from the input stream */
	if(!TreeCCNextToken(context->input))
	{
		return;
	}

	/* Parse lines of input until the input is exhausted */
	do
	{
		switch(context->input->token)
		{
			case TREECC_TOKEN_EOF:
			{
				/* Shouldn't happen, but ignore it if it does */
			}
			break;

			case TREECC_TOKEN_IDENTIFIER:
			{
				/* Parse an operation case definition */
				ParseOperationCase(context);
			}
			continue;	/* Skip the call to TreeCCNextToken */

			case TREECC_TOKEN_LITERAL_DEFNS:
			case TREECC_TOKEN_LITERAL_END:
			case TREECC_TOKEN_BOTH:
			case TREECC_TOKEN_DECLS:
			case TREECC_TOKEN_END:
			{
				/* Parse a literal definition block for this file */
				int flags = 0;
				while(context->input->token == TREECC_TOKEN_BOTH ||
				      context->input->token == TREECC_TOKEN_DECLS ||
				      context->input->token == TREECC_TOKEN_END)
				{
					if(context->input->token == TREECC_TOKEN_BOTH)
					{
						flags |= TREECC_LITERAL_CODE |
								 TREECC_LITERAL_DECLS;
					}
					else if(context->input->token == TREECC_TOKEN_DECLS)
					{
						flags |= TREECC_LITERAL_DECLS;
					}
					else
					{
						flags |= TREECC_LITERAL_END;
					}
					TreeCCNextToken(context->input);
				}
				if((flags & TREECC_LITERAL_DECLS) == 0)
				{
					flags |= TREECC_LITERAL_CODE;
				}
				if(context->input->token == TREECC_TOKEN_LITERAL_DEFNS)
				{
					TreeCCAddLiteralDefn(context, TreeCCValue(context->input),
										 flags);
				}
				else if(context->input->token == TREECC_TOKEN_LITERAL_END)
				{
					TreeCCAddLiteralDefn(context, TreeCCValue(context->input),
										 flags | TREECC_LITERAL_END);
				}
				else
				{
					TreeCCError(context->input,
								"literal definition block expected");
					continue;
				}
			}
			break;

			case TREECC_TOKEN_NODE:
			{
				/* Parse a node definition */
				ParseNode(context);
			}
			continue;	/* Skip the call to TreeCCNextToken */

			case TREECC_TOKEN_OPERATION:
			{
				/* Parse an operation definition */
				ParseOperation(context);
			}
			continue;	/* Skip the call to TreeCCNextToken */

			case TREECC_TOKEN_OPTION:
			{
				/* Parse an option declaration */
				ParseOption(context);
			}
			continue;	/* Skip the call to TreeCCNextToken */

			case TREECC_TOKEN_HEADER:
			{
				/* Parse a header filename specification */
				TreeCCNextToken(context->input);
				if(context->input->token == TREECC_TOKEN_STRING)
				{
					TreeCCStream *stream =
						TreeCCStreamCreate(context, context->input->text,
										   context->input->text, 1);
					context->headerStream = stream;
					stream->readOnly |= context->input->readOnly;
					if(!(context->commonHeader))
					{
						context->commonHeader = stream;
					}
				}
				else
				{
					TreeCCError(context->input, "header filename expected");
					continue;
				}
			}
			break;

			case TREECC_TOKEN_OUTPUT:
			{
				/* Parse an output filename specification */
				TreeCCNextToken(context->input);
				if(context->input->token == TREECC_TOKEN_STRING)
				{
					TreeCCStream *stream =
						TreeCCStreamCreate(context, context->input->text,
										   context->input->text, 0);
					context->sourceStream = stream;
					stream->readOnly |= context->input->readOnly;
					if(!(context->commonSource))
					{
						context->commonSource = stream;
					}
				}
				else
				{
					TreeCCError(context->input, "output filename expected");
					continue;
				}
			}
			break;

			case TREECC_TOKEN_OUTDIR:
			{
				/* Parse an output directory specification */
				TreeCCNextToken(context->input);
				if(context->input->token == TREECC_TOKEN_STRING)
				{
					context->outputDirectory =
						TreeCCResolvePathname(context->input->filename,
											  context->input->text);
				}
				else
				{
					TreeCCError(context->input, "output filename expected");
					continue;
				}
			}
			break;

			case TREECC_TOKEN_ENUM:
			{
				/* Parse an enumerated type definition */
				ParseEnum(context);
			}
			continue;	/* Skip the call to TreeCCNextToken */

			case TREECC_TOKEN_COMMON:
			{
				/* Put the common housekeeping code in the
				   current header and source streams */
				context->commonHeader = context->headerStream;
				context->commonSource = context->sourceStream;
			}
			break;

			case TREECC_TOKEN_INCLUDE:
			{
				/* Include another file at this point */
				int readOnly = context->input->readOnly;
				TreeCCNextToken(context->input);
				if(context->input->token == TREECC_TOKEN_READONLY)
				{
					readOnly = 1;
					TreeCCNextToken(context->input);
				}
				if(context->input->token == TREECC_TOKEN_STRING)
				{
					char *includeFile =
						TreeCCResolvePathname(context->input->filename,
											  context->input->text);
					FILE *file = fopen(includeFile, "r");
					if(file != NULL)
					{
						/* Parse the contents of the included file */
						TreeCCInput *newInput =
							(TreeCCInput *)malloc(sizeof(TreeCCInput));
						TreeCCInput *origInput = context->input;
						if(!newInput)
						{
							TreeCCOutOfMemory(context->input);
						}
						TreeCCOpen(newInput, context->input->progname,
								   file, includeFile);
						context->input = newInput;
						TreeCCParse(context);
						context->input = origInput;
						TreeCCClose(newInput, 1);
						free(newInput);
					}
					else
					{
						TreeCCError(context->input, "cannot open \"%s\"",
									includeFile);
						free(includeFile);
					}
				}
				else
				{
					TreeCCError(context->input, "include filename expected");
				}
			}
			break;

			case TREECC_TOKEN_LITERAL_CODE:
			case TREECC_TOKEN_LPAREN:
			case TREECC_TOKEN_RPAREN:
			case TREECC_TOKEN_LBRACE:
			case TREECC_TOKEN_RBRACE:
			case TREECC_TOKEN_LSQUARE:
			case TREECC_TOKEN_RSQUARE:
			case TREECC_TOKEN_COMMA:
			case TREECC_TOKEN_EQUALS:
			case TREECC_TOKEN_STAR:
			case TREECC_TOKEN_REF:
			case TREECC_TOKEN_SEMI:
			case TREECC_TOKEN_COLON_COLON:
			case TREECC_TOKEN_STRING:
			case TREECC_TOKEN_UNKNOWN:
			case TREECC_TOKEN_ABSTRACT:
			case TREECC_TOKEN_TYPEDEF:
			case TREECC_TOKEN_NOCREATE:
			case TREECC_TOKEN_VIRTUAL:
			case TREECC_TOKEN_INLINE:
			case TREECC_TOKEN_SPLIT:
			case TREECC_TOKEN_READONLY:
			{
				/* This token is not valid here */
				TreeCCError(context->input, "declaration expected");
				do
				{
					/* Attempt to re-synchronise on the next declaration */
					TreeCCNextToken(context->input);
				}
				while(!IsStartDecl(context->input->token));
			}
			continue;	/* Skip the call to TreeCCNextToken */
		}

		/* Get the next token from the input stream */
		TreeCCNextToken(context->input);
	}
	while(context->input->token != TREECC_TOKEN_EOF);
}

#ifdef	__cplusplus
};
#endif
