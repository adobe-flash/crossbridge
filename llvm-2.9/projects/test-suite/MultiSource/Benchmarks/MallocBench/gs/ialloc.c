/*
 * Changes:
 *
 * 1) Date Unknown
 *    It appears this file has the NOMEMOPT option added to disable memory
 *    use optimizations.  This change was probably introduced by the
 *    researchers from ftp://ftp.cs.colorado.edu/pub/cs/misc/malloc-benchmarks
 */
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

/* ialloc.c */
/* Memory allocator for Ghostscript interpreter */
#include <stdio.h>		/* for NULL */
#include "std.h"
#include "memory_.h"
#include "alloc.h"

#ifdef DEBUG
extern char gs_debug[128];
#endif

/* Forward references */
typedef struct alloc_state_s alloc_state;
typedef alloc_state _ds *alloc_state_ptr;	/****** NOTE _ds ******/
private int alloc_add_chunk(P1(alloc_state_ptr));
private char *alloc_large(P3(alloc_state_ptr, uint, char *));
private void alloc_free_large(P3(char *, uint, char *));

/* Round up sizes of aligned objects. */
#define log2_align_mod 3		/* log2(sizeof(double)) */
#define align_mod (1<<log2_align_mod)
#define align_mask (align_mod-1)
#define align_round(siz) (uint)(((siz) + align_mask) & -align_mod)

/* Max object size for separate free list */
#define max_chain_size 255

/* Structure for a separately allocated block. */
typedef struct alloc_block_s alloc_block;
struct alloc_block_s {
	alloc_block *next;
	uint size;
	int save_level;
	alloc_state_ptr cap;
};
#define alloc_block_size align_round(sizeof(alloc_block))

/* Structure for a single wholesale allocation 'chunk'. */
typedef struct alloc_chunk_s alloc_chunk;
struct alloc_chunk_s {
	/* Note that allocation takes place both from the bottom up */
	/* (aligned objects) and from the top down (byte objects). */
	byte *base;
	byte *bot;			/* bottom of free area */
					/* (top of aligned objects) */
	byte *top;			/* top of free area */
					/* (bottom of byte objects) */
	byte *limit;
	int save_level;			/* save level when this chunk */
					/* was allocated */
	alloc_chunk *next;		/* chain chunks together */
};

#define ptr_le(p1, p2)\
  (ptr_ord_t)(p1) <= (ptr_ord_t)(p2)
#define ptr_lt(p1, p2)\
  (ptr_ord_t)(p1) < (ptr_ord_t)(p2)
#define ptr_between(ptr, lo, hi)\
  (ptr_le(lo, ptr) && ptr_lt(ptr, hi))
#define ptr_is_in_chunk(ptr, chunk)\
  ptr_between(ptr, (chunk)->base, (chunk)->limit)

/* Structure for saved change chain for save/restore. */
/* This is a very space-inefficient implementation: */
/* we'll change it if the need arises. */
typedef struct alloc_change_s alloc_change;
struct alloc_change_s {
	alloc_change *next;
	char *where;
	uint size;			/* # of bytes */
};

/* Structure for allocator state.  If we multi-thread some day, */
/* this might be instantiated more than once. */
struct alloc_state_s {
	alloc_chunk current;		/* the current chunk */
#define cbase current.base
#define cbot current.bot
#define ctop current.top
#define climit current.limit
	alloc_chunk *current_ptr;	/* where to put current */
	uint chunk_size;		/* unit for wholesale malloc */
	uint big_size;			/* min size for separate malloc */
	proc_alloc_t palloc;		/* proc for malloc */
	proc_free_t pfree;		/* proc for free */
	/* Cumulative statistics */
	long used, total;
	unsigned num_chunks;
	/* Chain together freed objects within a save level. */
	/* We only do this for aligned objects. */
#define num_free_chains ((max_chain_size >> log2_align_mod) + 1)
	char *free[num_free_chains];
	/* Chain together any malloc'ed objects */
	alloc_block *malloc_chain;
	/* Keep track of saved states */
	alloc_save *saved;
	int save_level;
	alloc_change *changes;
};
/* The only instance for now */
private alloc_state as_current;

/* Saved state of allocator */
/*typedef struct alloc_save_s alloc_save;*/	/* in alloc.h */
struct alloc_save_s {
	alloc_state state;
	alloc_state_ptr cap;
};

/* Debugging printout */
#ifdef DEBUG
#  define alloc_print(rtag, tag, blk, sz)\
	if ( gs_debug['A'] )\
	  printf("[a:%c:%c%s]%lx(%u)\n", rtag, tag,\
		 client_name, (ulong)blk, sz)
#  define alloc_print_large(rtag, tag, blk, sz)\
	if ( gs_debug['A'] | gs_debug['a'] )\
	  printf("[a:%c:%c%s]%lx(%u)\n", rtag, tag,\
		 client_name, (ulong)blk, sz)
#else
#  define alloc_print(rtag, tag, blk, sz)
#  define alloc_print_large(rtag, tag, blk, sz)
#endif

/* ------ Initialize/status ------ */

/* Initialize the allocator */
void
alloc_init(proc_alloc_t palloc, proc_free_t pfree, uint chunk_size)
{	register alloc_state_ptr ap = &as_current;
	memset(ap, 0, sizeof(alloc_state));	/* do it all at once */
	ap->chunk_size = chunk_size;
#if defined(NOMEMOPT)
	ap->big_size = 8;
#else	
	ap->big_size = chunk_size / 4;
#endif	
	ap->palloc = palloc;
	ap->pfree = pfree;
}

/* Return the status of the allocator: space used, total space. */
void
alloc_status(long *pused, long *ptotal)
{	register alloc_state_ptr ap = &as_current;
	*pused = (ap->cbot - ap->cbase) + (ap->climit - ap->ctop) + ap->used;
	*ptotal = ap->climit - ap->cbase + ap->total;
}

/* ------ Allocation and freeing ------ */

/* Allocate an object.  Return 0 if not enough room. */
char *
alloc(uint num_elts, uint elt_size, char *client_name)
{	register alloc_state_ptr ap = &as_current;
	uint size = num_elts * elt_size;
	uint block_size;
	uint left;
	if ( size >= ap->big_size )
	   {	/* Large object, do a separate malloc. */
		char *block = alloc_large(ap, size, client_name);
		if ( block != NULL ) return block;
	   }
	block_size = align_round(size);
	if ( block_size <= max_chain_size )
	   {	/* See if we can use a freed block. */
		char **fptr = &ap->free[block_size >> log2_align_mod];
		char *block = *fptr;
		if ( block != 0 )
		   {	*fptr = *(char **)block;
			alloc_print('+', '#', block, size);
			return block;
		   }
	   }
	left = ap->ctop - ap->cbot;
	if ( block_size > left )
	   {	if ( !alloc_add_chunk(ap) )
		   {	alloc_print('+', '?', (ulong)0, size);
			return 0;
		   }
	   }
	if ( elt_size == 1 )
	   {	/* Unaligned block */
		ap->ctop -= size;
		alloc_print('+', '>', ap->ctop, size);
		return (char *)ap->ctop;
	   }
	else
	   {	/* Aligned block */
		char *block = (char *)ap->cbot;
		ap->cbot += block_size;
		alloc_print('+', '<', block, size);
		return block;
	   }
}

/* Free an object, if possible. */
/* Note that if a save is in effect, objects in chunks older than */
/* the save, and objects allocated with malloc before the save, */
/* must not be freed. */
void
alloc_free(char *cobj, uint num_elts, uint elt_size, char *client_name)
{	register alloc_state_ptr ap = &as_current;
	uint size = num_elts * elt_size;
	uint block_size;
	if ( size >= ap->big_size )
	   {	/* Object was allocated with malloc. */
		alloc_free_large(cobj, size, client_name);
		return;
	   }
#define obj ((byte *)cobj)
	else if ( obj == ap->ctop )
	   {	/* Don't free the object if we're in a save and */
		/* this object wasn't allocated since the save. */
		if ( ap->save_level == 0 ||
		     ap->current.save_level >= ap->save_level ||
		     /* We know the current chunk is the same as */
		     /* the one in as->saved->state */
		     obj < ap->saved->state.ctop
		   )
			ap->ctop += size;
		alloc_print('-', '>', obj, size);
		return;
	   }
	else if ( obj + (block_size = align_round(size)) == ap->cbot )
	   {	/* Freeing an aligned object.  Same check. */
		if ( ap->save_level == 0 ||
		     ap->current.save_level >= ap->save_level ||
		     obj >= ap->saved->state.cbot
		   )
			ap->cbot = obj;
		alloc_print('-', '<', obj, size);
		return;
	   }
	else if ( !ptr_is_in_chunk(obj, &ap->current) )
	   {	/* In another segment, check its save level. */
		int level = ap->save_level;
		alloc_chunk *cp = ap->current.next;
		for ( ; ; cp = cp->next )
		  { if ( cp != 0 )
		      { switch ( cp->save_level - level )
			  {
			  case 0:
			    if ( ptr_is_in_chunk(obj, cp) )
			      { if ( ptr_lt(obj, cp->bot) ) goto pbf;
			      else break;
			      }
			    else continue;
			  case -1:
			    /* Might be alloc'ed since the save, */
			    /* or might not be aligned. */
			    if ( ptr_lt(obj, ap->saved->state.cbot) )
			      goto pbf;
			  }
		      }
		    /* Older save level, not freeable. */
		    alloc_print('-', '\\', obj, size);
		    return;
		  }
pbf:		/* If we get here, OK to put the block on a free list. */
		;
	   }
	else if ( obj >= ap->cbot )	/* not aligned object, punt */
	   {	alloc_print('-', '~', obj, size);
		return;
	   }
	/* Put on a free list if small enough */
	alloc_print('-', '#', obj, size);
	if ( block_size <= max_chain_size && block_size >= sizeof(char **) )
	   {	char **fptr = &ap->free[block_size >> log2_align_mod];
		*(char **)cobj = *fptr;
		*fptr = cobj;
	   }
#undef obj
}

/* Grow an object.  This may require allocating a new copy. */
/* Return 0 if not enough room. */
byte *
alloc_grow(byte *obj, uint old_num, uint new_num, uint elt_size,
  char *client_name)
{	register alloc_state_ptr ap = &as_current;
	uint old_size = old_num * elt_size;
	uint new_size = new_num * elt_size;
	byte *nobj;
	if ( new_size == old_size ) return obj;
	if ( new_size < ap->big_size ) /* try to grow in place */
	  { uint old_block_size;
	    uint new_block_size;
	    if ( obj == ap->ctop )
	      { /* Might be able to grow in place */
		uint diff = new_size - old_size;
		if ( diff <= ap->ctop - ap->cbot )
		  { alloc_print('>', '>', obj, new_size);
		    ap->ctop -= diff;
		    memcpy(ap->ctop, obj, old_size);
		    return ap->ctop;
		  }
	      }
	    old_block_size = align_round(old_size);
	    new_block_size = align_round(new_size);
	    if ( obj + old_block_size == ap->cbot )
	      { /* Might be able to grow in place */
		uint diff = new_block_size - old_block_size;
		if ( diff <= ap->ctop - ap->cbot )
		  { alloc_print('>', '<', obj, new_size);
		    ap->cbot += diff;
		    return obj;
		  }
	      }
	  }
	/* Can't grow in place.  Allocate a new object and copy. */
	nobj = (byte *)alloc(new_num, elt_size, client_name);
	if ( nobj == 0 ) return 0;
	memcpy(nobj, obj, old_size);
	alloc_free((char *)obj, old_num, elt_size, client_name);
	alloc_print('>', '&', obj, new_size);
	return nobj;
}

/* Shrink an object. */
byte *
alloc_shrink(byte *obj, uint old_num, uint new_num, uint elt_size,
  char *client_name)
{	register alloc_state_ptr ap = &as_current;
	uint old_size = old_num * elt_size;
	uint new_size = new_num * elt_size;
	if ( new_size == old_size ) return obj;
	if ( old_size >= ap->big_size )
	  { /* Allocate a new block. */
	    byte *nobj = (byte *)alloc(new_num, elt_size, client_name);
	    if ( nobj == 0 ) return obj; /* can't shrink, leave as is */
	    memcpy(nobj, obj, new_size);
	    alloc_free((char *)obj, old_num, elt_size, client_name);
	    alloc_print('<', '&', obj, new_size);
	    return nobj;
	  }
	else if ( obj == ap->ctop )
	  { /* Move the object up in place. */
	    /* memcpy doesn't do this properly. */
	    register byte *from = obj + new_size;
	    register byte *to = obj + old_size;
	    while ( from > obj ) *--to = *--from;
	    obj = ap->ctop = to;
	  }
	else
	  { uint new_block_size = align_round(new_size);
	    alloc_free((char *)(obj + new_block_size),
		       1, align_round(old_size) - new_block_size,
		       "alloc_shrink");
	  }
	alloc_print('<', ' ', obj, new_size);
	return obj;
}

/* ------ Save and restore state ------ */

/*
 * The logic for saving and restore the state is rather subtle.
 * Both the changes to individual objects, and the overall state
 * of the memory manager, must be saved and restored.
 */

/*
 * To save the state of the memory manager:
 *	Save the state of the current chunk in which we are allocating.
 *	Save the identity of the current chunk.
 *	Save and reset the malloc chain and the orphan block chains.
 * By doing this, we guarantee that no object older than the save
 * can be freed.
 *
 * To restore the state of the memory manager:
 *	Free all chunks newer than the save.
 *	Free all malloc'ed blocks newer than the save.
 *	Make current the chunk that was current at the time of the save.
 *	Free all objects allocated in the current chunk since the save.
 */

/* For saving changes to individual objects, we add an "attribute" bit
 * that logically belongs to the slot where the descriptor is stored,
 * not to the descriptor itself.  The bit means "the contents
 * of this slot have been changed since the last save."
 * To keep track of changes since the save, we associate a chain of
 * <slot, old_contents> pairs that remembers the old contents of slots.
 *
 * When creating an object, if the save level is non-zero:
 *	Set the bit in all slots.
 *
 * When storing into a slot, if the save level is non-zero:
 *	If the bit isn't set, save the address and contents of the slot
 *	  on the current contents chain.
 *	Set the bit after storing the new value.
 *
 * To do a save:
 *	Reset the bit in all slots on the contents chain, and in all
 *	  objects created since the previous save.
 *	Push the head of contents chain, and reset the chain to empty.
 *
 * To do a restore:
 *	Check all the stacks to make sure they don't contain references
 *	  to objects created since the save.
 *	Restore all the slots on the contents chain.
 *	Pop the contents chain head.
 *	Scan the newly restored contents chain, and set the bit in all
 *	  the slots it references.
 *	Scan all objects created since the previous save, and set the bit
 *	  in all the slots of each object.
 */

/* Save the state. */
alloc_save *
alloc_save_state()
{	register alloc_state_ptr ap = &as_current;
	alloc_save *save =
		(alloc_save *)alloc(1, sizeof(alloc_save), "alloc_save_state");
	if ( save == 0 ) return 0;
	save->state = *ap;
	/* Clear the free chains, to prevent old objects from being freed. */
	memset(&ap->free[0], 0, num_free_chains * sizeof(char *));
	ap->malloc_chain = 0;
	ap->saved = save;
	ap->save_level++;
	ap->changes = 0;
	return save;
}

/* Record a state change that must be undone for restore. */
int
alloc_save_change(char *where, uint size)
{	register alloc_state_ptr ap = &as_current;
	register alloc_change *cp;
	if ( ap->save_level == 0 ) return 0;	/* no saving */
	cp = (alloc_change *)alloc(1, sizeof(alloc_change) + size,
				   "alloc_save_change");
	if ( cp == 0 ) return -1;
	cp->next = ap->changes;
	cp->where = where;
	cp->size = size;
	memcpy((char *)cp + sizeof(alloc_change), where, size);
	ap->changes = cp;
	return 0;
}

/* Return the current save level */
int
alloc_save_level()
{	return as_current.save_level;
}

/* Test whether a reference would be invalidated by a restore. */
int
alloc_is_since_save(char *ptr, alloc_save *save)
{
	/* A reference can postdate a save in one of three ways: */
	/*	- It is in the chunk that was current at the time */
	/*	    of the save, and allocated more recently. */
	/*	- It is in a chunk allocated since the save; */
	/*	- It was malloc'ed since the save; */

	register alloc_state_ptr ap = save->cap;

	/* Check against current chunk at the time of the save */
	if ( ptr_is_in_chunk(ptr, &save->state.current) )
	   {	/* In the chunk, check against allocation pointers */
		/* at the time of the save */
		return ( (ptr_ord_t)ptr >= (ptr_ord_t)save->state.cbot &&
			 (ptr_ord_t)ptr < (ptr_ord_t)save->state.ctop );
	   }

	/* Check against chunks allocated since the save */
	   {	alloc_chunk *chunk = &ap->current;
		while ( chunk->save_level > save->state.save_level )
		   {	if ( ptr_is_in_chunk(ptr, chunk) ) return 1;
			chunk = chunk->next;
		   }
	   }

	/* Check the malloc chains since the save */
	   {	alloc_state *asp = ap;
		for ( ; asp != &save->state; asp = &asp->saved->state )
		   {	alloc_block *mblk = asp->malloc_chain;
			for ( ; mblk != 0; mblk = mblk->next )
			  if ( alloc_block_size + (char *)mblk == ptr ) return 1;
		   }
	   }

	/* Not in any of those places, must be OK. */
	return 0;
}

/* Validate a saved state pointer. */
int
alloc_restore_state_check(alloc_save *save)
{	alloc_save *sprev = save->cap->saved;
	while ( sprev != save )
	   {	if ( sprev == 0 ) return -1;	/* not on chain */
		sprev = sprev->state.saved;
	   }
	return 0;
}

/* Restore the state.  The client is responsible for calling */
/* alloc_restore_state_check first, and for ensuring that */
/* there are no surviving pointers for which alloc_is_since_save is true. */
void
alloc_restore_state(alloc_save *save)
{	register alloc_state_ptr ap = save->cap;
	alloc_save *sprev;

	/* Iteratively restore the state */
	do
	   {	sprev = ap->saved;
		/* Free chunks allocated since the save. */
		   {	alloc_chunk *cp = ap->current_ptr;
			*cp = ap->current;	/* update in memory */
		   }
		/* Free blocks allocated with malloc since the save. */
		/* Since we reset the chain when we did the save, */
		/* we just free all the objects on the current chain. */
		   {	while ( ap->malloc_chain != 0 )
			   {	alloc_block *mblock = ap->malloc_chain;
				ap->malloc_chain = mblock->next;
				(*ap->pfree)((char *)mblock,
					    1, alloc_block_size + mblock->size,
					    "alloc_restore_state(malloc'ed)");
			   }
		   }
		/* Undo changes since the save. */
		   {	alloc_change *cp = ap->changes;
			while ( cp )
			   {	memcpy(cp->where, (char *)cp + sizeof(alloc_change), cp->size);
				cp = cp->next;
			   }
		   }
		/* Restore the allocator state. */
		*ap = sprev->state;
		alloc_free((char *)sprev, 1, sizeof(alloc_save),
			   "alloc_restore_state");
	   }
	while ( sprev != save );
}

/* ------ Private routines ------ */

/* Allocate (with malloc) an object too large to be put in a chunk. */
/* Return NULL on failure. */
private char *
alloc_large(alloc_state_ptr ap, uint size, char *client_name)
{	alloc_block *mblock = (alloc_block *)
		(*ap->palloc)(1, alloc_block_size + size, client_name);
	char *block;
	if ( mblock == NULL ) return NULL;
	block = (char *)mblock + alloc_block_size;
   	alloc_print_large('+', '*', block, size);
	mblock->next = ap->malloc_chain;
	mblock->size = size;
	mblock->save_level = ap->save_level;
	mblock->cap = ap;
	ap->malloc_chain = mblock;
	return block;
}

/* Allocate a new chunk.  Return true if successful. */
private int
alloc_add_chunk(register alloc_state_ptr ap)
{	char *space = (*ap->palloc)(1, ap->chunk_size, "alloc chunk");
#ifdef DEBUG
if ( gs_debug['A'] | gs_debug['a'] )
	printf("[a]%lx@%u\n", (ulong)space, ap->chunk_size);
#endif
	if ( space == NULL ) return 0;
	/* Update statistics for the old chunk */
	alloc_status(&ap->used, &ap->total);
	ap->num_chunks++;
	/* Stash the state of the old chunk */
	if ( ap->current_ptr != 0 )	/* check for very first chunk */
		*ap->current_ptr = ap->current;
	/* Initialize the new chunk */
	ap->cbase = ap->cbot = (byte *)space + align_round(sizeof(alloc_chunk));
	ap->climit = ap->ctop = (byte *)(space + ap->chunk_size);
	ap->current.next = ap->current_ptr;
	ap->current.save_level = ap->save_level;
	ap->current_ptr = (alloc_chunk *)space;
	return 1;
}

/* Free a large object (allocated with malloc). */
private void
alloc_free_large(char *cobj, uint size, char *client_name)
{	alloc_block **prev;
	alloc_block *mblock = (alloc_block *)(cobj - alloc_block_size);
	alloc_state_ptr ap = mblock->cap;
	if ( mblock->save_level == ap->save_level )
	 for ( prev = &ap->malloc_chain; *prev != 0; prev = &mblock->next )
	   {	mblock = *prev;
		if ( (char *)mblock + alloc_block_size == cobj )
		   {	*prev = mblock->next;
			(*ap->pfree)((char *)mblock,
				    1, size + alloc_block_size,
				    "large object");
			alloc_print_large('-', '*', cobj, size);
			return;
		   }
	   }
	alloc_print('-', '?', cobj, size);
}
