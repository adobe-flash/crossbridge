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

/* gvirtmem.c */
/* Software virtual memory for bitmaps for Ghostscript */
#include <stdio.h>
#include "std.h"
#include "memory_.h"
#include "string_.h"
#include "gvirtmem.h"

/* Things that aren't in stdio.h (at least on some systems) */
extern char *mktemp(P1(char *));
#ifndef SEEK_SET
#  define SEEK_SET 0
#endif

/* Define the scratch file name for the paging file. */
#ifdef __MSDOS__
#  define SCRATCH_TEMP "_gvm_XXXXXX"
#else
#  define SCRATCH_TEMP "/tmp/gvmem_XXXXXX"
#endif

/* Forward declarations */
private int vmem_get_page(P3(gx_vmem *, char **, int));
private int vmem_choose_page(P2(gx_vmem *, int));
private void vmem_read_page(P2(gx_vmem *, int));
private void vmem_write_page(P2(gx_vmem *, int));
private void vmem_annul_page(P2(gx_vmem *, int));
private void vmem_assign_page(P3(gx_vmem *, int, char *));
private FILE *vmem_file(P1(gx_vmem *));
private void vmem_seek(P2(gx_vmem *, int));

/* ------ Public interface ------ */

/* Initialize an instance of virtual memory */
int
vmem_init(register gx_vmem *vmem,
  char **lines, int line_size, int num_lines, char init_value,
  int page_size, long max_memory,
  proc_alloc_t alloc, proc_free_t free)
{	char *exists;
	vmem->lines = lines;
	   {	int lnum;
		for ( lnum = 0; lnum < num_lines; lnum++ )
			lines[lnum] = NO_LINE;
	   }
	if ( page_size < line_size ) page_size = line_size;
	vmem->line_size = line_size;
	vmem->num_lines = num_lines;
	vmem->init_value = init_value;
	vmem->page_size = page_size / line_size * line_size;
	vmem->alloc = alloc;
	vmem->free = free;
	vmem->file = NULL;		/* open when needed */
	vmem->lines_per_page = page_size / line_size;
	vmem->num_pages =
		(num_lines + vmem->lines_per_page - 1) / vmem->lines_per_page;
	vmem->next_file_page = 0;
	vmem->num_in_memory = 0;
	vmem->max_in_memory = (int)(max_memory / page_size);

	/* Allocate and initialize the page existence table */
	exists = (*alloc)(vmem->num_pages, 1, "vmem_init(exists)");
	if ( exists == NULL )
	   {	dprintf("Couldn't allocate page status table (vmem_init)\n");
		exit(1);
	   }
	vmem->exists = exists;
	memset(exists, 0, vmem->num_pages);
	vmem->num_reads = 0;
	vmem->num_writes = 0;

	return 0;
}

/* Close the instance, deallocating the buffers and removing the */
/* temporary file.  This doesn't deallocate the instance itself: */
/* that is the client's responsibility.  The instance must be */
/* initialized before it can be used again. */
int
vmem_close(register gx_vmem *vmem)
{	proc_free_t free = vmem->free;
	/* We deallocate in the reverse order of allocating, */
	/* in the hope of pacifying a LIFO memory manager.... */
	if ( vmem->file != NULL )
	   {	fclose(vmem->file);
		vmem->file = NULL;
		remove(vmem->filename);
		(*free)(vmem->filename, strlen(vmem->filename)+1, 1,
			"vmem_close(filename)");
		vmem->filename = NULL;
	   }
	   {	/* Deallocate the buffers in a random order, */
		/* and hope for the best. */
		int pnum;
		for ( pnum = 0; pnum < vmem->num_pages; pnum++ )
		   {	char *page = page_ptr(vmem, pnum);
			if ( page != NO_LINE )
				(*free)(page, 1, vmem->page_size, "vmem_close(page)");
		   }
	   }
	(*free)(vmem->exists, vmem->num_pages, 1, "vmem_close(exists)");
	return 0;
}

/* Make a page resident in memory.  Return 0 normally. */
/* If the page doesn't exist, then if force is true, */
/* allocate and clear the page and return 0; if force is false, */
/* do nothing and return 1. */
int
vmem_bring_in_page(register gx_vmem *vmem, char **pline, int force)
{	int fault;
	if ( *pline != NO_LINE ) return 0;	/* false alarm */
	vmem->lock_first = 0, vmem->lock_last = -1;	/* no lock */
	fault = vmem_get_page(vmem, pline, force);
	if ( fault < 0 )
	   {	dprintf1("vmem_get_page failed in vmem_bring_in_page, lnum = %d!\n",
			pline - vmem->lines);
		exit(1);
	   }
	return 0;
}

/* Make a rectangle resident in memory.  Return 0 normally. */
/* If this isn't possible, return -2 to indicate to the caller */
/* that it should divide the rectangle along the Y axis and try again. */
/* (See the description of bring_in_proc in gxdevmem.h for details.) */
int
vmem_bring_in_rect(gx_vmem *vmem, int ignore_x, int lnum,
  int ignore_w, int lcount, int writing)
{	char **lines = vmem->lines;
	int lpp = vmem->lines_per_page;
	int line_first = (lnum / lpp) * lpp;	/* first line on page */
	int line_last = lnum + lcount - 1;
	register char **pl = lines + line_first;
	char **pl_last = lines + line_last;
	/* Prevent pages from being thrown out once they're in. */
	vmem->lock_first = line_first / lpp;
	vmem->lock_last = line_last / lpp;
	for ( ; pl <= pl_last; pl += lpp )
	  if ( *pl == NO_LINE && vmem_get_page(vmem, pl, 1) != 0 )
	    return -2;
	return 0;
}

/* ------ Private code ------ */

/* Bring in a single page.  The caller has set lock_first and lock_last. */
private int
vmem_get_page(gx_vmem *vmem, char **pline, int force)
{	int lnum = pline - vmem->lines;
	int pnum = lnum / vmem->lines_per_page;
	int clear_it = 0;
	char *page;
	if ( !vmem->exists[pnum] )
	   {	if ( !force ) return 1;
		clear_it = 1;
	   }
	if ( vmem->num_in_memory < vmem->max_in_memory &&
	     (page = (*vmem->alloc)(1, vmem->page_size, "vmem_get_page")) != NULL
	   )
	   {	vmem->num_in_memory++;
	   }
	else				/* fully allocated, or can't alloc */
	   {	int purge_pnum = vmem_choose_page(vmem, pnum);
		if ( purge_pnum < 0 ) return purge_pnum;
		page = page_ptr(vmem, purge_pnum);
		vmem_write_page(vmem, purge_pnum);
		vmem_annul_page(vmem, purge_pnum);
	   }
	vmem_assign_page(vmem, pnum, page);
	if ( clear_it )
	   {	memset(page, vmem->init_value, vmem->page_size);
		vmem->exists[pnum] = 1;
	   }
	else
		vmem_read_page(vmem, pnum);
	return 0;
}

/* Choose a page to purge.  This always succeeds, unless */
/* we are bringing in a rectangle and pages are locked. */
private int /* page # */
vmem_choose_page(register gx_vmem *vmem, int pnum)
{	/* Search for an allocated page, starting with the one */
	/* farthest away from the one being read in. */
	int bottom = 0, top = vmem->num_pages - 1;
	while ( bottom <= top )
	   {	if ( pnum - bottom > top - pnum )
		   {	if ( page_ptr(vmem, bottom) != NO_LINE &&
			     (bottom < vmem->lock_first ||
			      bottom > vmem->lock_last)
			   )
			  return bottom;
			bottom++;
		   }
		else
		   {	if ( page_ptr(vmem, top) != NO_LINE &&
			     (top < vmem->lock_first ||
			      top > vmem->lock_last)
			   )
			  return top;
			top--;
		   }
	   }
	return -1;			/* no unlocked page */
}

/* Read a page from the disk. */
/* The memory page is already allocated. */
private void
vmem_read_page(register gx_vmem *vmem, int pnum)
{	char *page = page_ptr(vmem, pnum);
	FILE *file = vmem_file(vmem);
	vmem_seek(vmem, pnum);
	if ( fread((void *)page, sizeof(char), vmem->page_size, file)
		!= vmem->page_size )
	   {	dprintf1("fread failed in vmem_read, pnum = %d!\n",
			pnum);	
		exit(1);
	   }
	vmem->num_reads++;
}

/* Write a page to the disk */
private void
vmem_write_page(register gx_vmem *vmem, int pnum)
{	char *page = page_ptr(vmem, pnum);
	if ( page != NO_LINE )
	   {	FILE *file = vmem_file(vmem);
		while ( pnum > vmem->next_file_page )
		   {	int count;
			char value = vmem->init_value;
			vmem_seek(vmem, vmem->next_file_page);
			for ( count = vmem->page_size; count > 0; count-- )
				putc(value, file);
			vmem->next_file_page++;
		   }
		vmem_seek(vmem, pnum);
		if ( fwrite((void *)page, sizeof(char), vmem->page_size, file)
			!= vmem->page_size )
		   {	dprintf1("fwrite failed in vmem_write, pnum = %d!\n",
				pnum);	
			exit(1);
		   }
		if ( pnum == vmem->next_file_page )
			vmem->next_file_page++;
	   }
	vmem->num_writes++;
}

/* Mark a page as not in memory. */
private void
vmem_annul_page(register gx_vmem *vmem, int pnum)
{	int lnum = pnum * vmem->lines_per_page;
	char **ppage = &vmem->lines[lnum];
	int limit = min(lnum + vmem->lines_per_page, vmem->num_lines);
	while ( lnum++ < limit ) *ppage = NO_LINE;
}

/* Mark a page as in memory. */
/* The page must have been marked not in memory before. */
private void
vmem_assign_page(register gx_vmem *vmem, int pnum, char *page)
{	int lnum = pnum * vmem->lines_per_page;
	char **ppage = &vmem->lines[lnum];
	char *line = page;
	int limit = min(lnum + vmem->lines_per_page, vmem->num_lines);
	while ( lnum++ < limit )
		*ppage++ = line,
		line += vmem->line_size;
}

/* Open the virtual memory file if needed */
private FILE *
vmem_file(gx_vmem *vmem)
{	FILE *file = vmem->file;
	if ( file == NULL )		/* first time, open the file */
	   {	char *fname = (*vmem->alloc)(strlen(SCRATCH_TEMP)+1, 1,
					     "vmem_file(filename)");
		if ( fname == NULL )
		   {	dprintf("Can't alloc vmem temporary file name\n");
			exit(1);
		   }
		strcpy(fname, SCRATCH_TEMP);
		if ( mktemp(fname) == NULL )
		   {	dprintf("Can't create vmem temporary file\n");
			exit(1);
		   }
		file = fopen(fname, "w+b");
		if ( file == NULL )
		   {	dprintf1("Can't open vmem temporary file %s\n",
				fname);
			exit(1);
		   }
		vmem->file = file;
		vmem->filename = fname;
	   }
	return file;
}

/* Seek to a given page */
private void
vmem_seek(gx_vmem *vmem, int pnum)
{	if ( fseek(vmem_file(vmem), (long)pnum * vmem->page_size, SEEK_SET) )
	   {	dprintf1("fseek failed in vmem_seek, pnum = %d!\n",
			pnum);
		exit(1);
	   }
}

/* ------ Test program ------ */

#ifdef VMDEBUG

#include "malloc_.h"

char *talloc(unsigned size, char *str)
{	return (char *)malloc(size);
}
void tfree(char *blk, unsigned size, char *str)
{	free(blk);
}

#define MY_LINE_SIZE 8
#define MY_NUM_LINES 14
#define MY_PAGE_SIZE 25
#define MY_MAX_MEMORY (MY_PAGE_SIZE*2)
gx_vmem my_vmem;			/* static so we can print it */
char *my_lines[MY_NUM_LINES];		/* ditto */

main()
{	/* Following may be enabled if necessary
		trace(vmem_read_page, "read", NULL, 0);
		trace(vmem_write_page, "write", NULL, 0);
		trace(vmem_annul_page, "annul", NULL, 0);
		trace(vmem_assign_page, "assign", NULL, 0);
		trace(talloc, "alloc", "%d from %s", 4);
		trace(tfree, "free", "%lx %d from %s", 0);
	 */
	vmem_init(&my_vmem, my_lines, MY_LINE_SIZE, MY_NUM_LINES, 0xbd,
	  MY_PAGE_SIZE, (long)MY_MAX_MEMORY, talloc, tfree);
	   {	/* Simple write/read loop test */
		int lnum;
		for ( lnum = 0; lnum < MY_NUM_LINES; lnum++ )
		   {	if ( my_lines[lnum] == NO_LINE )
				vmem_get_page(&my_vmem, &my_lines[lnum], 1);
			my_lines[lnum][3] = lnum;
		   }
		for ( lnum = 0; lnum < MY_NUM_LINES; lnum++ )
		   {	if ( my_lines[lnum] == NO_LINE )
				vmem_get_page(&my_vmem, &my_lines[lnum], 1);
			if ( my_lines[lnum][3] != lnum )
				printf("Error: lnum=%d, data=%d\n",
					lnum, my_lines[lnum][3]);
		   }
	   }
	vmem_close(&my_vmem);
	printf("Test completed, num_reads=%ld, num_writes=%ld\n",
		my_vmem.num_reads, my_vmem.num_writes);
	exit(0);
}

#endif					/* ifdef VMDEBUG */
