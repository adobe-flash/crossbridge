/* Generate the nondeterministic finite state machine for bison,
   Copyright (C) 1984, 1986 Bob Corbett and Free Software Foundation, Inc.

BISON is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY.  No author or distributor accepts responsibility to anyone
for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.
Refer to the BISON General Public License for full details.

Everyone is granted permission to copy, modify and redistribute BISON,
but only under the conditions described in the BISON General Public
License.  A copy of this license is supposed to have been given to you
along with BISON so you can know your rights and responsibilities.  It
should be in a file named COPYING.  Among other things, the copyright
notice and this notice must be preserved on all copies.

 In other words, you are welcome to use, share and improve this program.
 You are forbidden to forbid anyone else to use, share and improve
 what you give them.   Help stamp out software-hoarding!  */

/* See comments in state.h for the data structures that represent it.
   The entry point is generate_states.  */

#include <stdio.h>
#include "machine.h"
#include "new.h"
#include "gram.h"
#include "state.h"


extern char *nullable;
extern short *itemset;
extern short *itemsetend;


int nstates;
int final_state;
core *first_state;
shifts *first_shift;
reductions *first_reduction;

int get_state();
core *new_state();

static core *this_state;
static core *last_state;
static shifts *last_shift;
static reductions *last_reduction;

static int nshifts;
static short *shift_symbol;

static short *redset;
static short *shiftset;

static short **kernel_base;
static short **kernel_end;
static short *kernel_items;

/* hash table for states, to recognize equivalent ones.  */

#define	STATE_TABLE_SIZE	1009
static core **state_table;

extern void initialize_closure(int n);
extern void closure(short *core,int n);
extern void finalize_closure(void);
extern void toomany(char *s);

void initialize_states(void);
void save_reductions(void);
void new_itemsets(void);
void append_states(void);
void save_shifts(void);
void augment_automaton(void);
void insert_start_shift(void);

void allocate_itemsets(void)
{
  register short *itemp;
  register int symbol;
  register int i;
  register count;
  register int max;
  register short *symbol_count;

  count = 0;
  symbol_count = NEW2(nsyms, short);

  itemp = ritem;
  symbol = *itemp++;
  while (symbol)
    {
      if (symbol > 0)
	{
	  count++;
	  symbol_count[symbol]++;
	}
      symbol = *itemp++;
    }

  /* see comments before new-itemset.  All the vectors of items
     live inside kernel_items.  The number of active items after
     some symbol cannot be more than the number of times that symbol
     appears as an item, which is symbol_count[symbol].
     We allocate that much space for each symbol.  */

  kernel_base = NEW2(nsyms, short *);
  kernel_items = NEW2(count, short);

  count = 0;
  max = 0;
  for (i = 0; i < nsyms; i++)
    {
      kernel_base[i] = kernel_items + count;
      count += symbol_count[i];
      if (max < symbol_count[i])
	max = symbol_count[i];
    }

  shift_symbol = symbol_count;
  kernel_end = NEW2(nsyms, short *);
}



void allocate_storage(void)
{
  allocate_itemsets();

  shiftset = NEW2(nsyms, short);
  redset = NEW2(nrules + 1, short);
  state_table = NEW2(STATE_TABLE_SIZE, core *);
}



void free_storage(void)
{
  FREE(shift_symbol);
  FREE(redset);
  FREE(shiftset);
  FREE(kernel_base);
  FREE(kernel_end);
  FREE(kernel_items);
  FREE(state_table);
}



/* compute the nondeterministic finite state machine (see state.h for details)
from the grammar.  */

void generate_states(void)
{
  allocate_storage();
  initialize_closure(nitems);
  initialize_states();

  while (this_state)
    {
      /* Set up ruleset and itemset for the transitions out of this state.
         ruleset gets a 1 bit for each rule that could reduce now.
	 itemset gets a vector of all the items that could be accepted next.  */
      closure(this_state->items, this_state->nitems);
      /* record the reductions allowed out of this state */
      save_reductions();
      /* find the itemsets of the states that shifts can reach */
      new_itemsets();
      /* find or create the core structures for those states */
      append_states();

      /* create the shifts structures for the shifts to those states,
         now that the state numbers transitioning to are known */
      if (nshifts > 0)
        save_shifts();

      /* states are queued when they are created; process them all */
      this_state = this_state->next;
    }

  /* discard various storage */
  finalize_closure();
  free_storage();

  /* set up initial and final states as parser wants them */
  augment_automaton();
}



/* Find which symbols can be shifted in the current state,
   and for each one record which items would be active after that shift.
   Uses the contents of itemset.
   shift_symbol is set to a vector of the symbols that can be shifted.
   For each symbol in the grammer, kernel_base[symbol] points to
   a vector of item numbers activated if that symbol is shifted,
   and kernel_end[symbol] points after the end of that vector.  */

void new_itemsets(void)
{
  register int i;
  register int shiftcount;
  register short *isp;
  register short *ksp;
  register int symbol;

#ifdef	TRACE
  fprintf(stderr, "Entering new_itemsets\n");
#endif

  for (i = 0; i < nsyms; i++)
    kernel_end[i] = NULL;

  shiftcount = 0;

  isp = itemset;

  while (isp < itemsetend)
    {
      i = *isp++;
      symbol = ritem[i];
      if (symbol > 0)
	{
          ksp = kernel_end[symbol];

          if (!ksp)
	    {
	      shift_symbol[shiftcount++] = symbol;
	      ksp = kernel_base[symbol];
	    }

          *ksp++ = i + 1;
          kernel_end[symbol] = ksp;
	}
    }

  nshifts = shiftcount;
}



/* Use the information computed by new_itemset to find the state numbers
   reached by each shift transition from the current state.

   shiftset is set up as a vector of state numbers of those states.  */

void append_states(void)
{
  register int i;
  register int j;
  register int symbol;

#ifdef	TRACE
  fprintf(stderr, "Entering append_states\n");
#endif

  /* first sort shift_symbol into increasing order */

  for (i = 1; i < nshifts; i++)
    {
      symbol = shift_symbol[i];
      j = i;
      while (j > 0 && shift_symbol[j - 1] > symbol)
	{
	  shift_symbol[j] = shift_symbol[j - 1];
	  j--;
	}
      shift_symbol[j] = symbol;
    }

  for (i = 0; i < nshifts; i++)
    {
      symbol = shift_symbol[i];
      shiftset[i] = get_state(symbol);
    }
}



/* find the state number for the state we would get to
(from the current state) by shifting symbol.
Create a new state if no equivalent one exists already.
Used by append_states  */

int get_state(int symbol)
{
  register int key;
  register short *isp1;
  register short *isp2;
  register short *iend;
  register core *sp;
  register int found;

  int n;

#ifdef	TRACE
  fprintf(stderr, "Entering get_state, symbol = %d\n", symbol);
#endif

  isp1 = kernel_base[symbol];
  iend = kernel_end[symbol];
  n = iend - isp1;

  /* add up the target state's active item numbers to get a hash key */
  key = 0;
  while (isp1 < iend)
    key += *isp1++;

  key = key % STATE_TABLE_SIZE;

  sp = state_table[key];

  if (sp)
    {
      found = 0;
      while (!found)
	{
	  if (sp->nitems == n)
	    {
	      found = 1;
	      isp1 = kernel_base[symbol];
	      isp2 = sp->items;

	      while (found && isp1 < iend)
		{
		  if (*isp1++ != *isp2++)
		    found = 0;
		}
	    }

	  if (!found)
	    {
	      if (sp->link)
		{
		  sp = sp->link;
		}
	      else   /* bucket exhausted and no match */
		{
		  sp = sp->link = new_state(symbol);
		  found = 1;
		}
	    }
	}
    }
  else      /* bucket is empty */
    {
      state_table[key] = sp = new_state(symbol);
    }

  return (sp->number);
}



/* subroutine of get_state.  create a new state for those items, if necessary.  */

core *new_state(int symbol)
{
  register int n;
  register core *p;
  register short *isp1;
  register short *isp2;
  register short *iend;

#ifdef	TRACE
  fprintf(stderr, "Entering new_state, symbol = %d\n", symbol);
#endif

  if (nstates >= MAXSHORT)
    toomany("states");

  isp1 = kernel_base[symbol];
  iend = kernel_end[symbol];
  n = iend - isp1;

  p = (core *) mallocate((unsigned) (sizeof(core) + (n - 1) * sizeof(short)));
  p->accessing_symbol = symbol;
  p->number = nstates;
  p->nitems = n;

  isp2 = p->items;
  while (isp1 < iend)
    *isp2++ = *isp1++;

  last_state->next = p;
  last_state = p;

  nstates++;

  return (p);
}



void initialize_states(void)
{
  register core *p;
/*  register unsigned *rp1; JF unused */
/*  register unsigned *rp2; JF unused */
/*  register unsigned *rend; JF unused */

  p = (core *) mallocate((unsigned) (sizeof(core) - sizeof(short)));
  first_state = last_state = this_state = p;
  nstates = 1;
}



void save_shifts(void)
{
  register shifts *p;
  register short *sp1;
  register short *sp2;
  register short *send;

  p = (shifts *) mallocate((unsigned) (sizeof(shifts) +
				       (nshifts - 1) * sizeof(short)));

  p->number = this_state->number;
  p->nshifts = nshifts;

  sp1 = shiftset;
  sp2 = p->shifts;
  send = shiftset + nshifts;

  while (sp1 < send)
    *sp2++ = *sp1++;

  if (last_shift)
    {
      last_shift->next = p;
      last_shift = p;
    }
  else
    {
      first_shift = p;
      last_shift = p;
    }
}



/* find which rules can be used for reduction transitions from the current state
   and make a reductions structure for the state to record their rule numbers.  */

void save_reductions(void)
{
  register short *isp;
  register short *rp1;
  register short *rp2;
  register int item;
  register int count;
  register reductions *p;

  short *rend;

  /* find and count the active items that represent ends of rules */

  count = 0;
  for (isp = itemset; isp < itemsetend; isp++)
    {
      item = ritem[*isp];
      if (item < 0)
	{
	  redset[count++] = -item;
	}
    }

  /* make a reductions structure and copy the data into it.  */

  if (count)
    {
      p = (reductions *) mallocate((unsigned) (sizeof(reductions) +
					       (count - 1) * sizeof(short)));

      p->number = this_state->number;
      p->nreds = count;

      rp1 = redset;
      rp2 = p->rules;
      rend = rp1 + count;

      while (rp1 < rend)
	*rp2++ = *rp1++;

      if (last_reduction)
	{
	  last_reduction->next = p;
	  last_reduction = p;
	}
      else
	{
	  first_reduction = p;
	  last_reduction = p;
	}
    }
}



/* Make sure that the initial state has a shift that accepts the
grammar's start symbol and goes to the next-to-final state,
which has a shift going to the final state, which has a shift
to the termination state.
Create such states and shifts if they don't happen to exist already.  */

void augment_automaton(void)
{
  register int i;
  register int k;
/*  register int found; JF unused */
  register core *statep;
  register shifts *sp;
  register shifts *sp2;
  register shifts *sp1;

  sp = first_shift;

  if (sp)
    {
      if (sp->number == 0)
	{
	  k = sp->nshifts;
	  statep = first_state->next;

	  while (statep->accessing_symbol < start_symbol
		  && statep->number < k)
	    statep = statep->next;

	  if (statep->accessing_symbol == start_symbol)
	    {
	      k = statep->number;

	      while (sp->number < k)
		{
		  sp1 = sp;
		  sp = sp->next;
		}

	      if (sp->number == k)
		{
		  sp2 = (shifts *) mallocate((unsigned) (sizeof(shifts)
							 + sp->nshifts * sizeof(short)));
		  sp2->next = sp->next;
		  sp2->number = k;
		  sp2->nshifts = sp->nshifts + 1;
		  sp2->shifts[0] = nstates;
		  for (i = sp->nshifts; i > 0; i--)
		    sp2->shifts[i] = sp->shifts[i - 1];

		  sp1->next = sp2;
		  FREE(sp);
		}
	      else
		{
		  sp2 = NEW(shifts);
		  sp2->next = sp;
		  sp2->number = k;
		  sp2->nshifts = 1;
		  sp2->shifts[0] = nstates;

		  sp1->next = sp2;
		  if (!sp)
		    last_shift = sp2;
		}
	    }
	  else
	    {
	      k = statep->number;
	      sp = first_shift;

	      sp2 = (shifts *) mallocate((unsigned) (sizeof(shifts)
						     + sp->nshifts * sizeof(short)));
	      sp2->next = sp->next;
	      sp2->nshifts = sp->nshifts + 1;

	      for (i = 0; i < k; i++)
		sp2->shifts[i] = sp->shifts[i];

	      sp2->shifts[k] = nstates;

	      for (i = k; i < sp->nshifts; i++)
		sp2->shifts[i + 1] = sp->shifts[i];

	      first_shift = sp2;
	      if (last_shift == sp)
		last_shift = sp2;

	      FREE(sp);

	      insert_start_shift();
	    }
	}
      else
	{
	  sp = NEW(shifts);
	  sp->next = first_shift;
	  sp->nshifts = 1;
	  sp->shifts[0] = nstates;

	  first_shift = sp;

	  insert_start_shift();
	}
    }
  else
    {
      sp = NEW(shifts);
      sp->nshifts = 1;
      sp->shifts[0] = nstates;

      first_shift = sp;
      last_shift = sp;

      insert_start_shift();
    }

  statep = (core *) mallocate((unsigned) (sizeof(core) - sizeof(short)));
  statep->number = nstates;
  last_state->next = statep;
  last_state = statep;

  sp = NEW(shifts);
  sp->number = nstates++;
  sp->nshifts = 1;
  sp->shifts[0] = nstates;
  last_shift->next = sp;
  last_shift = sp;

  final_state = nstates;

  statep = (core *) mallocate((unsigned) (sizeof(core) - sizeof(short)));
  statep->number = nstates++;
  last_state->next = statep;
  last_state = statep;
}


/* subroutine of augment_automaton */

void insert_start_shift(void)
{
  register core *statep;
  register shifts *sp;

  statep = (core *) mallocate((unsigned) (sizeof(core) - sizeof(short)));
  statep->number = nstates;
  statep->accessing_symbol = start_symbol;

  last_state->next = statep;
  last_state = statep;

  sp = NEW(shifts);
  sp->number = nstates++;
  sp->nshifts = 1;
  sp->shifts[0] = nstates;

  last_shift->next = sp;
  last_shift = sp;
}
