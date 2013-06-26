/*
 * gen.h - Generate code to "treecc" output files.
 *
 * Copyright (C) 2001, 2002  Southern Storm Software, Pty Ltd.
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

#ifndef	_TREECC_GEN_H
#define	_TREECC_GEN_H

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Generate the source code output.
 */
void TreeCCGenerate(TreeCCContext *context);

/*
 * Generate the source code output for specific languages.
 */
void TreeCCGenerateC(TreeCCContext *context);
void TreeCCGenerateCPP(TreeCCContext *context);
void TreeCCGenerateJava(TreeCCContext *context);
void TreeCCGenerateCSharp(TreeCCContext *context);
void TreeCCGenerateRuby(TreeCCContext *context);
void TreeCCGeneratePHP(TreeCCContext *context);

/*
 * Control structure for generating the code for
 * non-virtual operations.
 */
typedef struct
{
	/* Generate the start declarations for a non-virtual operation */
	void (*genStart)(TreeCCContext *context, TreeCCStream *stream,
					 TreeCCOperation *oper);

	/* Generate the entry point for a non-virtual operation */
	void (*genEntry)(TreeCCContext *context, TreeCCStream *stream,
					 TreeCCOperation *oper);

	/* Generate the entry point for a split-out function */
	void (*genSplitEntry)(TreeCCContext *context, TreeCCStream *stream,
					      TreeCCOperation *oper, int number);

	/* Generate the head of a "switch" statement */
	void (*genSwitchHead)(TreeCCContext *context, TreeCCStream *stream,
						  char *paramName, int level, int isEnum);

	/* Generate a selector for a "switch" case */
	void (*genSelector)(TreeCCContext *context, TreeCCStream *stream,
					    TreeCCNode *node, int level);

	/* Terminate the selectors and begin the body of a "switch" case */
	void (*genEndSelectors)(TreeCCContext *context, TreeCCStream *stream,
						    int level);

	/* Generate the code for a case within a function */
	void (*genCaseFunc)(TreeCCContext *context, TreeCCStream *stream,
						TreeCCOperationCase *operCase, int number);

	/* Generate a call to a case function from within the "switch" */
	void (*genCaseCall)(TreeCCContext *context, TreeCCStream *stream,
						TreeCCOperationCase *operCase, int number, int level);

	/* Generate the code for a case inline within the "switch" */
	void (*genCaseInline)(TreeCCContext *context, TreeCCStream *stream,
						  TreeCCOperationCase *operCase, int level);

	/* Generate the code for a call to a split function within the "switch" */
	void (*genCaseSplit)(TreeCCContext *context, TreeCCStream *stream,
						 TreeCCOperationCase *operCase, int number, int level);

	/* Terminate a "switch" case */
	void (*genEndCase)(TreeCCContext *context, TreeCCStream *stream,
					   int level);

	/* Terminate the "switch" statement */
	void (*genEndSwitch)(TreeCCContext *context, TreeCCStream *stream,
						 int level);

	/* Generate the exit point for a non-virtual operation */
	void (*genExit)(TreeCCContext *context, TreeCCStream *stream,
					TreeCCOperation *oper);

	/* Generate the end declarations for a non-virtual operation */
	void (*genEnd)(TreeCCContext *context, TreeCCStream *stream,
				   TreeCCOperation *oper);

} TreeCCNonVirtual;

/*
 * Generate the code for the non-virtual operations.
 */
void TreeCCGenerateNonVirtuals(TreeCCContext *context,
							   const TreeCCNonVirtual *nonVirt);

/*
 * Common non-virtual operations that are used for C and C++.
 */
extern TreeCCNonVirtual const TreeCCNonVirtualFuncsC;

/*
 * Common non-virtual operations that are used for Java and C#.
 */
extern TreeCCNonVirtual const TreeCCNonVirtualFuncsJava;

#ifdef	__cplusplus
};
#endif

#endif	/* _TREECC_GEN_H */
