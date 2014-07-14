/*
 * options.c - Process options from "treecc" input files.
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
#include "options.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * "track_lines": track line numbers when creating new nodes.
 */
static int TrackLinesOption(TreeCCContext *context, char *value, int flag)
{
	if(value)
	{
		return TREECC_OPT_NO_VALUE;
	}
	else
	{
		context->track_lines = flag;
		return TREECC_OPT_OK;
	}
}

/*
 * "no_singletons": suppress special create code for singletons.
 */
static int NoSingletonsOption(TreeCCContext *context, char *value, int flag)
{
	if(value)
	{
		return TREECC_OPT_NO_VALUE;
	}
	else
	{
		context->no_singletons = flag;
		return TREECC_OPT_OK;
	}
}

/*
 * "reentrant": generate re-entrant tree handling code.
 */
static int ReentrantOption(TreeCCContext *context, char *value, int flag)
{
	if(value)
	{
		return TREECC_OPT_NO_VALUE;
	}
	else
	{
		context->reentrant = flag;
		return TREECC_OPT_OK;
	}
}

/*
 * "force": force source files to be created even if unchanged.
 */
static int ForceOption(TreeCCContext *context, char *value, int flag)
{
	if(value)
	{
		return TREECC_OPT_NO_VALUE;
	}
	else
	{
		context->force = flag;
		return TREECC_OPT_OK;
	}
}

/*
 * "virtual_factory": use virtual factory methods.
 */
static int VirtualFactoryOption(TreeCCContext *context, char *value, int flag)
{
	if(value)
	{
		return TREECC_OPT_NO_VALUE;
	}
	else
	{
		context->virtual_factory = flag;
		return TREECC_OPT_OK;
	}
}

/*
 * "abstract_factory": use abstract factory methods.
 */
static int AbstractFactoryOption(TreeCCContext *context, char *value, int flag)
{
	if(value)
	{
		return TREECC_OPT_NO_VALUE;
	}
	else
	{
		context->abstract_factory = flag;
		return TREECC_OPT_OK;
	}
}

/*
 * "kind_in_vtable": put the kind value in the vtable, and not the node.
 */
static int KindInVtableOption(TreeCCContext *context, char *value, int flag)
{
	if(value)
	{
		return TREECC_OPT_NO_VALUE;
	}
	else
	{
		context->kind_in_vtable = flag;
		return TREECC_OPT_OK;
	}
}

/*
 * "prefix": specify the prefix to use instead of "yy".
 */
static int PrefixOption(TreeCCContext *context, char *value, int flag)
{
	if(!value)
	{
		return TREECC_OPT_NEED_VALUE;
	}
	else
	{
		context->yy_replacement = value;
		return TREECC_OPT_KEEP_VALUE;
	}
}

/*
 * "state_type": specify the type name to use for re-entrant state.
 */
static int StateTypeOption(TreeCCContext *context, char *value, int flag)
{
	if(!value)
	{
		return TREECC_OPT_NEED_VALUE;
	}
	else
	{
		context->state_type = value;
		return TREECC_OPT_KEEP_VALUE;
	}
}

/*
 * "namespace": specify the namespace to use for subsequent definitions.
 */
static int NamespaceOption(TreeCCContext *context, char *value, int flag)
{
	if(!value)
	{
		return TREECC_OPT_NEED_VALUE;
	}
	else if(*value == '\0')
	{
		/* Put following declarations into the global namespace */
		context->namespace = 0;
		return TREECC_OPT_OK;
	}
	else
	{
		context->namespace = value;
		return TREECC_OPT_KEEP_VALUE;
	}
}

/*
 * "base": specify a new base to use for node type identifiers.
 */
static int BaseOption(TreeCCContext *context, char *value, int flag)
{
	if(!value)
	{
		return TREECC_OPT_NEED_VALUE;
	}
	else if(*value == '\0')
	{
		return TREECC_OPT_INVALID_VALUE;
	}
	else
	{
		int num = 0;
		while(*value >= '0' && *value <= '9')
		{
			num = num * 10 + (int)(*value - '0');
			++value;
		}
		if(*value != '\0')
		{
			return TREECC_OPT_INVALID_VALUE;
		}
		context->nodeNumber = num;
		return TREECC_OPT_OK;
	}
}

/*
 * "lang": specify the source language to use in the output files.
 */
static int LangOption(TreeCCContext *context, char *value, int flag)
{
	if(!value)
	{
		return TREECC_OPT_NEED_VALUE;
	}
	else
	{
		if(!strcmp(value, "c") || !strcmp(value, "C"))
		{
			context->language = TREECC_LANG_C;
		}
		else if(!strcmp(value, "c++") || !strcmp(value, "C++"))
		{
			context->language = TREECC_LANG_CPP;
		}
		else if(!strcmp(value, "java") || !strcmp(value, "Java"))
		{
			context->language = TREECC_LANG_JAVA;
		}
		else if(!strcmp(value, "c#") || !strcmp(value, "C#") ||
		        !strcmp(value, "csharp"))
		{
			context->language = TREECC_LANG_CSHARP;
		}
		else if(!strcmp(value, "ruby") || !strcmp(value, "Ruby"))
		{
			context->language = TREECC_LANG_RUBY;
		}
		else if(!strcmp(value, "php") || !strcmp(value, "PHP"))
		{
			context->language = TREECC_LANG_PHP;
		}
		else
		{
			return TREECC_OPT_INVALID_VALUE;
		}
		return TREECC_OPT_OK;
	}
}

/*
 * "block_size": specify a new block size for C and C++ allocators.
 */
static int BlockSizeOption(TreeCCContext *context, char *value, int flag)
{
	if(!value)
	{
		return TREECC_OPT_NEED_VALUE;
	}
	else if(*value == '\0')
	{
		return TREECC_OPT_INVALID_VALUE;
	}
	else
	{
		int num = 0;
		while(*value >= '0' && *value <= '9')
		{
			num = num * 10 + (int)(*value - '0');
			++value;
		}
		if(*value != '\0')
		{
			return TREECC_OPT_INVALID_VALUE;
		}
		context->block_size = num;
		return TREECC_OPT_OK;
	}
}



/*
 * "print_lines": print out line number directives.
 */
static int PrintLineNumberOption(TreeCCContext *context, char *value, int flag)
{
	if(value)
	{
		return TREECC_OPT_NO_VALUE;
	}
	else
	{
		context->print_lines = flag;
		return TREECC_OPT_OK;
	}
}

/*
 * "strip_filenames": strip filenames in #line directives down
 * to their base name, with no directory information.
 */
static int StripFilenamesOption(TreeCCContext *context, char *value, int flag)
{
	if(value)
	{
		return TREECC_OPT_NO_VALUE;
	}
	else
	{
		context->strip_filenames = flag;
		return TREECC_OPT_OK;
	}
}

/*
 * "internal_access": use "internal" access on classes in C#, instead
 * of using "public".  "public_access" is used to select "public".
 */
static int InternalAccessOption(TreeCCContext *context, char *value, int flag)
{
	if(value)
	{
		return TREECC_OPT_NO_VALUE;
	}
	else
	{
		context->internal_access = flag;
		return TREECC_OPT_OK;
	}
}

/*
 * "allocator": use (or don't use) the standard treecc allocator for C/C++.
 */
static int AllocatorOption(TreeCCContext *context, char *value, int flag)
{
	if(value)
	{
		return TREECC_OPT_NO_VALUE;
	}
	else
	{
		context->use_allocator = flag;
		return TREECC_OPT_OK;
	}
}

/*
 * "gc_allocator": use (or don't use) the libgc treecc allocator for C/C++.
 */
static int GCAllocatorOption(TreeCCContext *context, char *value, int flag)
{
	if(value)
	{
		return TREECC_OPT_NO_VALUE;
	}
	else
	{
		context->use_gc_allocator = flag;
		return TREECC_OPT_OK;
	}
}

/*
 * "base_type": use the type as the base type for the root treecc node 
 */
static int BaseTypeOption(TreeCCContext *context, char *value, int flag)
{
	if(!value)
	{
		return TREECC_OPT_NEED_VALUE;
	}
	else if(value == '\0')
	{
		context->baseType = 0;
		return TREECC_OPT_OK;
	}
	else
	{
		context->baseType = value;
		return TREECC_OPT_KEEP_VALUE;
	}
}

/*
 * Table of option handlers.
 */
static struct
{
	const char *name;
	int (*func)(TreeCCContext *context, char *value, int flag);
	int flag;

} const OptionHandlers[] = {
	{"track_lines",			TrackLinesOption,		1},
	{"no_track_lines",		TrackLinesOption,		0},
	{"no_singletons",		NoSingletonsOption,		1},
	{"singletons",			NoSingletonsOption,		0},
	{"reentrant",			ReentrantOption,		1},
	{"no_reentrant",		ReentrantOption,		0},
	{"force",				ForceOption,			1},
	{"no_force",			ForceOption,			0},
	{"virtual_factory",		VirtualFactoryOption,	1},
	{"no_virtual_factory",	VirtualFactoryOption,	0},
	{"abstract_factory",	AbstractFactoryOption,	1},
	{"no_abstract_factory",	AbstractFactoryOption,	0},
	{"kind_in_vtable",		KindInVtableOption,		1},
	{"kind_in_node",		KindInVtableOption,		0},
	{"prefix",				PrefixOption,			0},
	{"state_type",			StateTypeOption,		0},
	{"namespace",			NamespaceOption,		0},
	{"package",				NamespaceOption,		0},
	{"base",				BaseOption,				0},
	{"lang",				LangOption,				0},
	{"block_size",			BlockSizeOption,		0},
	{"strip_filenames",		StripFilenamesOption,	1},
	{"print_lines",			PrintLineNumberOption,	1},
	{"no_print_lines",		PrintLineNumberOption,	0},
	{"no_strip_filenames",	StripFilenamesOption,	0},
	{"internal_access",		InternalAccessOption,	1},
	{"public_access",		InternalAccessOption,	0},
	{"allocator",			AllocatorOption,		1},
	{"no_allocator",		AllocatorOption,		0},
	{"gc_allocator",		GCAllocatorOption,		1},
	{"no_gc_allocator",		GCAllocatorOption,		0},
	{"base_type",			BaseTypeOption,			0},
	{0,						0,						0},
};

int TreeCCOptionProcess(TreeCCContext *context, char *name, char *value)
{
	int opt = 0;
	while(OptionHandlers[opt].name != 0)
	{
		if(!strcmp(name, OptionHandlers[opt].name))
		{
			return (*(OptionHandlers[opt].func))
						(context, value, OptionHandlers[opt].flag);
		}
		++opt;
	}
	return TREECC_OPT_UNKNOWN;
}

#ifdef	__cplusplus
};
#endif
