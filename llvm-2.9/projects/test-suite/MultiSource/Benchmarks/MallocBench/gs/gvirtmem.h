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

/* gvirtmem.h */
/* Software virtual memory for bitmaps for Ghostscript */

/*
 * This code implements a paged, software virtual memory system.
 * It is designed for bitmaps, and specifically designed to work well
 * with the structures that Ghostscript uses for image devices,
 * as defined in gxdevmem.h.  This impacts the design in several ways:
 *	- The page size is a multiple of the scan line size, which is
 *	    a multiple of 4 bytes but not necessarily a power of 2.
 *	- There is special provision for keeping track of pages that
 *	    are all white.
 *	- The page replacement algorithm is not LRU, but is a simpler
 *	    algorithm based on beliefs about spatial locality.
 *	- Moving pages in and out of memory updates the scan line table
 *	    defined by gxdevmem.h.
 *	- There is a special procedure to bring an entire rectangle
 *	    into memory.
 * Otherwise, the design is quite standard.  Each instance of the
 * virtual memory system (there can be many) has its own file, page
 * size, scan line size, page table / scan line table, and other
 * structures.
 */

/* This is the main instance structure */
typedef struct gx_vmem_s gx_vmem;
struct gx_vmem_s {
	/* The following are arguments at initialization */
	char **lines;			/* the page (scan line) table */
#define NO_LINE (char *)NULL		/* page not in memory */
#define page_ptr(vmem, pnum) (vmem)->lines[(pnum) * (vmem)->lines_per_page]
	int line_size;			/* size of each scan line */
	int num_lines;			/* number of scan lines */
	char init_value;		/* initial value for new pages */
	int page_size;			/* size of each page */
	proc_alloc_t alloc;		/* allocation procedure */
	proc_free_t free;		/* freeing procedure */
	/* The following are set at initialization */
	FILE *file;			/* the paging file */
	char *filename;			/* save the name, for removing */
					/* the file */
	char *exists;			/* status of each page, */
					/* 1 if it has ever been */
					/* written on the disk */
	int lines_per_page;		/* scan lines per page */
	int num_pages;			/* total # of pages */
	int lock_first, lock_last;	/* locked pages during bring_in_rect */
	/* The following are updated dynamically */
	int next_file_page;		/* next file page to write */
	int num_in_memory;		/* number of pages in memory */
	int max_in_memory;		/* max pages in memory */
	/* Statistics */
	long num_reads, num_writes;
};

/* Public procedures */

/* Initialize a virtual memory instance. */
int vmem_init(P9(gx_vmem *, char ** /*lines*/, int /*line_size*/,
		 int /*num_lines*/, char /*init_value*/, int /*page_size*/,
		 long /*max_memory*/, proc_alloc_t, proc_free_t));

/* Bring in a rectangle. */
int vmem_bring_in_rect(P6(gx_vmem *, int /*x*/, int /*y*/,
			  int /*width*/, int /*height*/, int /*writing*/));
