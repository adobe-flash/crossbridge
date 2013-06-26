/*
 * patricia.c
 *
 * Patricia trie implementation.
 *
 * Functions for inserting nodes, removing nodes, and searching in
 * a Patricia trie designed for IP addresses and netmasks.  A
 * head node must be created with (key,mask) = (0,0).
 *
 * NOTE: The fact that we keep multiple masks per node makes this
 *       more complicated/computationally expensive then a standard
 *       trie.  This is because we need to do longest prefix matching,
 *       which is useful for computer networks, but not as useful
 *       elsewhere.
 *
 * Matthew Smart <mcsmart@eecs.umich.edu>
 *
 * Copyright (c) 2000
 * The Regents of the University of Michigan
 * All rights reserved
 *
 * $Id: patricia.c 33046 2007-01-09 23:57:19Z lattner $
 */

#include <stdlib.h>	/* free(), malloc() */
#include <string.h>	/* bcopy() */
#include "patricia.h"


/*
 * Private function used to return whether
 * or not bit 'i' is set in 'key'.
 */
static __inline
unsigned long
bit(int i, unsigned long key)
{
	return key & (1 << (31-i));
}


/*
 * Count the number of masks (and therefore entries)
 * in the Patricia trie.
 */
static int
pat_count(struct ptree *t, int b)
{
	int count;
	
	if (t->p_b <= b) return 0;

	count = t->p_mlen;
	
	count += pat_count(t->p_left,  t->p_b);
	count += pat_count(t->p_right, t->p_b);

	return count;
}


/*
 * Private function used for inserting a node recursively.
 */
static struct ptree *
insertR(struct ptree *h, struct ptree *n, int d, struct ptree *p)
{
	if ((h->p_b >= d) || (h->p_b <= p->p_b)) {
		n->p_b = d;
		n->p_left = bit(d, n->p_key) ? h : n;
		n->p_right = bit(d, n->p_key) ? n : h;
		return n;
	}

	if (bit(h->p_b, n->p_key))
		h->p_right = insertR(h->p_right, n, d, h);
	else
		h->p_left = insertR(h->p_left, n, d, h);
	return h;
}


/*
 * Patricia trie insert.
 *
 * 1) Go down to leaf.
 * 2) Determine longest prefix match with leaf node.
 * 3) Insert new internal node at appropriate location and
 *    attach new external node.
 */
struct ptree *
pat_insert(struct ptree *n, struct ptree *head)
{
	struct ptree *t;
	struct ptree_mask *buf, *pm;
	int i, copied;

	if (!head || !n || !n->p_m)
		return 0;

	/*
	 * Make sure the key matches the mask.
	 */
	n->p_key &= n->p_m->pm_mask;

	/*
	 * Find closest matching leaf node.
	 */
	t = head;
	do {
		i = t->p_b;
		t = bit(t->p_b, n->p_key) ? t->p_right : t->p_left;
	} while (i < t->p_b);

	/*
	 * If the keys are the same we need to check the masks.
	 */
	if (n->p_key == t->p_key) {
		/*
		 * If we have a duplicate mask, replace the entry
		 * with the new one.
		 */
		for (i=0; i < t->p_mlen; i++) {
			if (n->p_m->pm_mask == t->p_m[i].pm_mask) {
				t->p_m[i].pm_data = n->p_m->pm_data;
				free(n->p_m);
				free(n);
				n = 0;
				return t;
			}
		}
		
		/*
		 * Allocate space for a new set of masks.
		 */
		buf = (struct ptree_mask *)malloc(
		       sizeof(struct ptree_mask)*(t->p_mlen+1));

		/*
		 * Insert the new mask in the proper order from least
		 * to greatest mask.
		 */
		copied = 0;
		for (i=0, pm=buf; i < t->p_mlen; pm++) {
			if (n->p_m->pm_mask > t->p_m[i].pm_mask) {
				bcopy(t->p_m + i, pm, sizeof(struct ptree_mask));
				i++;
			}
			else {
				bcopy(n->p_m, pm, sizeof(struct ptree_mask));
				n->p_m->pm_mask = 0xffffffff;
				copied = 1;
			}
		}
		if (!copied) {
			bcopy(n->p_m, pm, sizeof(struct ptree_mask));
		}
		free(n->p_m);
		free(n);
		n = 0;
		t->p_mlen++;

		/*
		 * Free old masks and point to new ones.
		 */
		free(t->p_m);
		t->p_m = buf;
		
		return t;
	}

	/*
	 * Find the first bit that differs.
	 */
	for (i=1; i < 32 && bit(i, n->p_key) == bit(i, t->p_key); i++); 

	/*
	 * Recursive step.
	 */
	if (bit(head->p_b, n->p_key))
		head->p_right = insertR(head->p_right, n, i, head);
	else
		head->p_left = insertR(head->p_left, n, i, head);
	
	return n;
}


/*
 * Remove an entry given a key in a Patricia trie.
 */
int
pat_remove(struct ptree *n, struct ptree *head)
{
	struct ptree *p, *g, *pt, *pp, *t;
	struct ptree_mask *buf, *pm;
	int i;

	if (!n || !n->p_m || !t)
		return 0;

	/*
	 * Search for the target node, while keeping track of the
	 * parent and grandparent nodes.
	 */
	g = p = t = head;
	do {
		i = t->p_b;
		g = p;
		p = t;
		t = bit(t->p_b, n->p_key) ? t->p_right : t->p_left;
	} while (i < t->p_b);

	/*
	 * For removal, we need an exact match.
	 */
	if (t->p_key != n->p_key)
		return 0;

	/*
	 * If there is only 1 mask, we can remove the entire node.
	 */
	if (t->p_mlen == 1) {
		/*
		 * Don't allow removal of the default entry.
		 */
		if (t->p_b == 0)
			return 0;
		
		/*
		 * Must match on the mask.
		 */
		if (t->p_m->pm_mask != n->p_m->pm_mask)
			return 0;
		
		/*
		 * Search for the node that points to the parent, so
		 * we can make sure it doesn't get lost.
		 */
		pp = pt = p;
		do {
			i = pt->p_b;
			pp = pt;
			pt = bit(pt->p_b, p->p_key) ? pt->p_right : pt->p_left;
		} while (i < pt->p_b);

		if (bit(pp->p_b, p->p_key))
			pp->p_right = t;
		else
			pp->p_left = t;

		/*
		 * Point the grandparent to the proper node.
		 */
		if (bit(g->p_b, n->p_key))
			g->p_right = bit(p->p_b, n->p_key) ?
				p->p_left : p->p_right;
		else
			g->p_left = bit(p->p_b, n->p_key) ?
				p->p_left : p->p_right;
	
		/*
		 * Delete the target's data and copy in its parent's
		 * data, but not the bit value.
		 */
		if (t->p_m->pm_data)
			free(t->p_m->pm_data);
		free(t->p_m);
		if (t != p) {
			t->p_key = p->p_key;
			t->p_m = p->p_m;
			t->p_mlen = p->p_mlen;
		}
		free(p);

		return 1;
	}

	/*
	 * Multiple masks, so we need to find the one to remove.
	 * Return if we don't match on any of them.
	 */
	for (i=0; i < t->p_mlen; i++)
		if (n->p_m->pm_mask == t->p_m[i].pm_mask)
			break;
	if (i >= t->p_mlen)
		return 0;
	
	/*
	 * Allocate space for a new set of masks.
	 */
	buf = (struct ptree_mask *)malloc(
	       sizeof(struct ptree_mask)*(t->p_mlen-1));

	for (i=0, pm=buf; i < t->p_mlen; i++) {
		if (n->p_m->pm_mask != t->p_m[i].pm_mask) {
			bcopy(t->p_m + i, pm++, sizeof(struct ptree_mask));
		}
	}
		
	/*
	 * Free old masks and point to new ones.
	 */
	t->p_mlen--;
	free(t->p_m);
	t->p_m = buf;
	return 1;
}


/*
 * Find an entry given a key in a Patricia trie.
 */
struct ptree *
pat_search(unsigned long key, struct ptree *head)
{
	struct ptree *p = 0, *t = head;
	int i;
	
	if (!t)
		return 0;

	/*
	 * Find closest matching leaf node.
	 */
	do {
		/*
		 * Keep track of most complete match so far.
		 */
		if (t->p_key == (key & t->p_m->pm_mask)) {
			p = t;
		}
		
		i = t->p_b;
		t = bit(t->p_b, key) ? t->p_right : t->p_left;
	} while (i < t->p_b);

	/*
	 * Compare keys (and masks) to see if this
	 * is really the node we want.
	 */
	return (t->p_key == (key & t->p_m->pm_mask)) ? t : p;
}
