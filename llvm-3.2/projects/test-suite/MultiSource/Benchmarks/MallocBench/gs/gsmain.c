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

/* gsmain.c */
/* Framework for GhostScript drivers */
#include "string_.h"
#include "malloc_.h"
#include "memory_.h"
#include "gx.h"
#include "gsmatrix.h"			/* for gxdevice.h */
#include "gxdevice.h"
#include "gxdevmem.h"

typedef struct gx_device_s gx_device;

/*
 * This routine provides the following standard services for parsing
 * a command line:
 *	- setting debug flags (-Z switch) [if debugging];
 *	- tracing (-T switch);
 *	- passing other arguments and switches back to the caller
 *
 * Calling convention:
 *	gs_main(argc, argv, map_name, switch_proc, arg_proc)
 * Calls
 *	switch_proc(switch_char, rest_of_arg) for switches,
 *	arg_proc(arg, index) for non-switch args.
 * If switch_proc returns a negative value, gs_main prints an
 *   "unknown switch" error message and aborts.
 * gs_main returns the number of non-switch args handed to arg_proc.
 */

/* Imported data */
extern gx_device *gx_device_list[];

private long proc_reloc;	/* relocation of procedures */

int
gs_main(int argc, char *argv[], char *map_name,
    int (*switch_proc)(P2(char, char *)),
    void (*arg_proc)(P2(char *, int)))
{	int argi = 1;
	FILE *mapf = NULL;
	int arg_count = 0;
	proc_reloc = 0;
	/* Do platform-dependent initialization. */
	/* We have to do this as the very first thing, */
	/* because it detects attempts to run 80N86 executables (N>0) */
	/* on incompatible processors. */
	   {	extern void gp_init();
		gp_init();		/* Platform-dependent init */
	   }
#ifdef DEBUG
	/* Reset debugging flags */
	memset(gs_debug, 0, 128);
#endif
	/* If debugging is enabled, trace the device calls. */
#ifdef DEBUG
	   {	extern gx_device *gs_trace_device(P1(gx_device *));
		extern gx_device_memory
			mem_mono_device, mem_mapped_color_device,
			mem_true24_color_device, mem_true32_color_device;
		static gx_device_memory *mdevs[5] =
		   {	&mem_mono_device, &mem_mapped_color_device,
			&mem_true24_color_device, &mem_true32_color_device,
			0
		   };
		gx_device **pdevs[3];
		gx_device ***ppdev;
		gx_device **pdev;
		pdevs[0] = gx_device_list;
		pdevs[1] = (gx_device **)mdevs;
		pdevs[2] = 0;
		for ( ppdev = pdevs; *ppdev != 0; ppdev++ )
		 for ( pdev = *ppdev; *pdev != 0; pdev++ )
		   {	gx_device *tdev = gs_trace_device(*pdev);
			if ( tdev == 0 )
			   {	dprintf("Can't allocate traced device!\n");
				exit(1);
			   }
			*pdev = tdev;
		   }
	   }
#endif
	for ( ; argi < argc; argi++ )
	   {	char *arg = argv[argi];
		if ( *arg == '-' )
		   {	switch ( *++arg )
			   {
			default:
				if ( (*switch_proc)(*arg, arg + 1) < 0 )
					printf("Unknown switch %s - ignoring\n", arg - 1);
				break;
			case 'Z':
#ifdef DEBUG
				/* Print the address of 'main' so that */
				/* we can decipher stack traces later. */
			   {	extern main();
				printf("[Z]main = %lx\n", (ulong)main);
			   }
				if ( !arg[1] )
				   {	/* No options, set all flags */
					memset(gs_debug, -1, 128);
				   }
				else
				   {	while ( *++arg )
						gs_debug[*arg & 127] = -1;
				   }
#else
				printf("Not a debugging configuration, -Z switch ignored\n");
#endif
				break;
			case 'T':
			if ( mapf == NULL )
			   {	/* Open the map file and look up 'main' */
				extern FILE *trace_open_map(P2(char *, long *));
				mapf = trace_open_map(map_name, &proc_reloc);
				if ( mapf == NULL )
				   {	printf("Map file %s is apparently missing or malformed\n", map_name);
					break;
				   }
				/* Print the address of 'main' so that */
				/* we can decipher return addresses later. */
			   {	extern main();
				printf("[T]main = %lx\n", (ulong)main);
			   }
			   }
			   {	char *delim;
				char *tname;
				char *targs = NULL;
				int rsize = 0;
				extern int trace_flush_flag;
				delim = strchr(arg, ':');
				if ( delim != NULL )
				   {	sscanf(delim + 1, "%d", &rsize);
					*delim = 0;	/* terminate name */
					delim = strchr(delim + 1, ':');
					if ( delim != NULL )
						targs = delim + 1;
				   }
				tname = gs_malloc(strlen(arg) + 1, 1,
						  "-T switch");
				strcpy(tname, arg);
				*tname = '_';
				strupr(tname);
				if ( trace_name(tname, mapf, targs, rsize) < 0 )
					printf("%s not found\n", tname);
				trace_flush_flag = 1;
			   }
				break;
			   }
		   }
		else
			(*arg_proc)(arg, arg_count++);
	   }
	return arg_count;
}

/* Close the device(s) and exit. */
void
gs_exit(int code)
{	gx_device **pdev = gx_device_list;
	if ( code != 0 ) fflush(stderr);	/* in case of error exit */
	for ( ; *pdev != 0; pdev++ )
	  if ( (*pdev)->is_open )
	    ((*pdev)->procs->close_device)(*pdev);
	exit(code);
}

/* ------ Debugging routines ------ */

#ifdef DEBUG			/* ------ ------ */

void gs_dump_C_stack();

/* Log an error return */
int
gs_log_error(int err)
{	if ( gs_debug['e'] )
	  { printf("Returning error %d:\n", err);
	    gs_dump_C_stack();
	  }
	return err;
}

#endif				/* ------ ifdef DEBUG ------ */

/* Dump the C stack. */
/* This is actually only used for debugging. */
extern char *stack_top_frame();
extern unsigned long stack_return(P1(char *));
extern char *stack_next_frame(P1(char *));
void
gs_dump_C_stack()
{	char *nbp = stack_top_frame();
	char *bp;
	do
	   {	bp = nbp;
		printf("frame %8lx called from %8lx (%8lx)\n",
			(unsigned long)bp, stack_return(bp),
			stack_return(bp) - proc_reloc);
		nbp = stack_next_frame(bp);
	   }
	while ( nbp != 0 );
}
