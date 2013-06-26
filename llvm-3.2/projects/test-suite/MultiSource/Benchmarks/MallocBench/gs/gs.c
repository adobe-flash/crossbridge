/* Copyright (C) 1989, 1990 Aladdin Enterprises.  All rights reserved.
   Distributed by Free Software Foundation, Inc.

This file is part of Ghostscript.

Ghostscript is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY.  No author or distributor accepts responsibility
to anyone for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.  Refer
to the Ghostscript General Public License for full details.

Everyone is granted permission to copy, modify and redistribute
Ghostscript, but only under the conditions described in the Ghostscript
General Public License.  A copy of this license is supposed to have been
given to you along with Ghostscript so you can know your rights and
responsibilities.  It should be in a file named COPYING.  Among other
things, the copyright notice and this notice must be preserved on all
copies.  */

/* gs.c */
/* Driver program for GhostScript */
#include <stdio.h>
#include "string_.h"
#include "ghost.h"
#include "alloc.h"
#include "store.h"
#include "stream.h"

/* Library routines not declared in a standard header */
extern char *getenv(P1(const char *));

/* Exported data */
uint memory_chunk_size = 20000;
/* File name search paths */
char **gs_lib_paths;
private int gs_lib_count;
private char *gs_lib_env_path;
private char *gs_lib_default_path = GS_LIB_DEFAULT;	/* defined in the makefile */

/* Imported data */
extern ref *osbot, *osp, *esp;
extern ref estack[];

/* Forward references */
private void init1(), init2();
private void set_lib_paths();
private void run_file(P2(char *file_name, int user_errors));
private void debug_dump_stack(P1(int code));

/* Parameters set by swproc */
private int user_errors;
private int quiet;

main(int argc, char *argv[])
{	int num_files;
	int code;
	int swproc(P2(char, char *));
	void argproc(P2(char *, int));

#ifdef BWGC
	{
	    extern gc_init();
	    gc_init();
	}
#endif BWGC
	
	/* Initialize the file search paths */
	gs_lib_env_path = 0;
	   {	char *lib = getenv("GS_LIB");
		if ( lib != 0 ) 
		   {	int len = strlen(lib);
			gs_lib_env_path = gs_malloc(len + 1, 1, "GS_LIB");
			strcpy(gs_lib_env_path, lib);
		   }
	   }
	gs_lib_paths =
		(char **)gs_malloc(argc + 2, sizeof(char *), "-I array");
	gs_lib_count = 0;
	set_lib_paths();
	/* Execute files named in the command line, */
	/* processing options along the way. */
	/* Wait until the first file name (or the end */
	/* of the line) to finish initialization. */
	quiet = 0;
	user_errors = 1;
	num_files = gs_main(argc, argv, "GS.MAP", swproc, argproc);
	if ( num_files == 0 )
	   {	init2();
	   }
/*
	code = interpret_string("start");
*/
	code = 0;
	zflush((ref *)0);	/* flush stdout */
	zflushpage((ref *)0);	/* force display update */
	if ( code < 0 )
		debug_dump_stack(code);
	gs_exit((code == 0 ? 0 : 2));
}

/* Process switches */
int
swproc(char sw, char *arg)
{	switch ( sw )
	   {
	default:
		return -1;
	case 'I':			/* specify search path */
		gs_lib_paths[gs_lib_count] = arg;
		gs_lib_count++;
		set_lib_paths();
		break;
	case 'q':			/* quiet startup */
	   {	ref vnull;
		quiet = 1;
		init1();
		make_null(&vnull);
		initial_enter_name("QUIET", &vnull);
	   }	break;
	case 'D':			/* define name */
	case 'd':
	case 'S':			/* define name as string */
	case 's':
	   {	char *eqp = strchr(arg, '=');
		ref value;
		/* Initialize the object memory, scanner, and */
		/* name table now if needed. */
		init1();
		if ( eqp == NULL )
		   {	make_null(&value);
		   }
		else if ( eqp == arg )
		   {	printf("Usage: -dname, -dname=token, -sname=string");
			gs_exit(1);
		   }
		else
		   {	int code;
			*eqp++ = 0;	/* delimit name */
			if ( sw == 'D' || sw == 'd' )
			   {	stream astream;
				sread_string(&astream,
					     (byte *)eqp, strlen(eqp));
				code = scan_token(&astream, 0, &value);
				if ( code )
				   {	printf("-dname= must be followed by a valid token");
					gs_exit(1);
				   }
			   }
			else
			   {	int len = strlen(eqp);
				char *str = gs_malloc((uint)len, 1, "-s");
				if ( str == 0 )
				   {	dprintf("Out of memory!\n");
					gs_exit(1);
				   }
				strcpy(str, eqp);
				make_tasv(&value, t_string, a_read+a_execute,
					  len, bytes, (byte *)str);
			   }
		   }
		/* Enter the name in systemdict */
		initial_enter_name(arg, &value);
		break;
	   }
	case 'w':			/* define device width */
	case 'h':			/* define device height */
	   {	stream astream;
		ref value;
		int code;
		init1();
		sread_string(&astream, (byte *)arg, strlen(arg));
		code = scan_token(&astream, 0, &value);
		if ( code || (r_type(&value) != t_integer && r_type(&value) != t_real) )
		   {	printf("-w and -h must be followed by a number\n");
			gs_exit(1);
		   }
		initial_enter_name((sw == 'w' ? "DEVICEWIDTH" : "DEVICEHEIGHT"), &value);
		break;
	   }
	case 'E':			/* suppress normal error handling */
		user_errors = 0;
		break;
	case 'M':			/* set memory allocation increment */
	   {	unsigned msize = 0;
		sscanf(arg, "%d", &msize);
		if ( msize <= 0 || msize >= 64 )
		   {	printf("-M must be between 1 and 64");
			gs_exit(1);
		   }
		memory_chunk_size = msize << 10;
	   }
		break;
	   }
	return 0;
}

/* Process file names */
void
argproc(char *arg, int index)
{	char line[256];
	int code;
	/* Finish initialization */
	init2();
	strcpy(line, "{(");
	strcat(line, arg);
	strcat(line, ")run}execute");
	code = interpret_string(line);
	zflushpage((ref *)0); /* force display update */
	if ( code ) debug_dump_stack(code), gs_exit(1);
}

/* Interpret a (C) string. */
int
interpret_string(char *str)
{	ref stref;
	make_tasv(&stref, t_string, a_executable + a_read + a_execute,
		  strlen(str), bytes, (byte *)str);
	return interpret(&stref, user_errors);
}

private int init1_done = 0, init2_done = 0;
private void
init1()
{	if ( !init1_done )
	   {	alloc_init(gs_malloc, gs_free, memory_chunk_size);
		name_init();
		obj_init();		/* requires name_init */
		scan_init();		/* ditto */
		init1_done = 1;
	   }
}
private void
init2()
{	init1();
	if ( !init2_done )
	   {	gs_init();
		zfile_init();
		zfont_init();
		zmath_init();
		zmatrix_init();
		interp_init(1);		/* requires obj_init */
		op_init();		/* requires obj_init, scan_init */
		/* Execute the standard initialization file. */
		run_file("ghost.ps", user_errors);
		init2_done = 1;
	   }
   }

/* Complete the list of library search paths. */
private void
set_lib_paths()
{	char **ppath = &gs_lib_paths[gs_lib_count];
	if ( gs_lib_env_path != 0 ) *ppath++ = gs_lib_env_path;
	if ( gs_lib_default_path != 0 ) *ppath++ = gs_lib_default_path;
	*ppath = 0;
}

/* Open and execute a file */
private void
run_file(char *file_name, int user_errors)
{	ref initial_file;
	int code;
	if ( lib_file_open(file_name, strlen(file_name), &initial_file) < 0 )
	   {	eprintf1("Can't find file %s (from command line)\n", file_name);
		gs_exit(1);
	   }
	if ( !quiet )
	   {	printf("Reading %s... ", file_name);
		fflush(stdout);
	   }
	r_set_attrs(&initial_file, a_execute + a_executable);
	code = interpret(&initial_file, user_errors);
	if ( code < 0 )
		debug_dump_stack(code), gs_exit(1);
	if ( !quiet )
	   {	printf("%s read.\n", file_name);
		fflush(stdout);
	   }
}

/* Debugging code */
/* Dump the stacks after interpretation */
private void
debug_dump_stack(int code)
{	zflush(osp);	/* force out buffered output */
	printf("\nInterp returns %d\n", code);
	debug_dump_refs(osbot, osp + 1, "ostack");
	debug_dump_refs(estack, esp + 1, "estack");
}
