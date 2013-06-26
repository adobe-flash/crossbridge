/* Print information on generated parser, for bison,
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

#include <stdio.h>
#include "machine.h"
#include "new.h"
#include "files.h"
#include "gram.h"
#include "state.h"


extern char **tags;
extern int nstates;
extern short *accessing_symbol;
extern core **state_table;
extern shifts **shift_table;
extern errs **err_table;
extern reductions **reduction_table;
extern char *consistent;
extern char any_conflicts;
extern char *conflicts;

extern void conflict_log(void);
extern void verbose_conflict_log(void);
extern void print_reductions(int state);

void print_token(int extnum,int token);
void print_state(int state);
void print_core(int state);
void print_actions(int state);

void terse(void)
{
  if (any_conflicts)
    {
      conflict_log();
    }
}

void verbose(void)
{
  register int i;

  if (any_conflicts)
    verbose_conflict_log();

  fprintf(foutput, "\n\ntoken types:\n");
  print_token (-1, 0);
  if (translations)
    {
      for (i = 0; i <= max_user_token_number; i++)
	/* Don't mention all the meaningless ones.  */
	if (token_translations[i] != 2)
	  print_token (i, token_translations[i]);
    }
  else
    for (i = 1; i < ntokens; i++)
      print_token (i, i);

  for (i = 0; i < nstates; i++)
    {
      print_state(i);
    }
}

void print_token(int extnum,int token)
{
  fprintf(foutput, " type %d is %s\n", extnum, tags[token]);
}

void print_state(int state)
{
  fprintf(foutput, "\n\nstate %d\n\n", state);
  print_core(state);
  print_actions(state);
}

void print_core(int state)
{
  register int i;
  register int k;
  register int rule;
  register core *statep;
  register short *sp;
  register short *sp1;

  statep = state_table[state];
  k = statep->nitems;

  if (k == 0) return;

  for (i = 0; i < k; i++)
    {
      sp1 = sp = ritem + statep->items[i];

      while (*sp > 0)
	sp++;

      rule = -(*sp);
      fprintf(foutput, "    %s  ->  ", tags[rlhs[rule]]);

      for (sp = ritem + rrhs[rule]; sp < sp1; sp++)
	{
	  fprintf(foutput, "%s ", tags[*sp]);
	}

      putc('.', foutput);

      while (*sp > 0)
	{
	  fprintf(foutput, " %s", tags[*sp]);
	  sp++;
	}

      fprintf (foutput, "   (%d)", rule);
      putc('\n', foutput);
    }

  putc('\n', foutput);
}

void print_actions(int state)
{
  register int i;
  register int k;
  register int state1;
  register int symbol;
  register shifts *shiftp;
  register errs *errp;
  register reductions *redp;
  register int rule;

  shiftp = shift_table[state];
  redp = reduction_table[state];
  errp = err_table[state];

  if (!shiftp && !redp)
    {
      fprintf(foutput, "    NO ACTIONS\n");
      return;
    }

  if (shiftp)
    {
      k = shiftp->nshifts;

      for (i = 0; i < k; i++)
	{
	  if (! shiftp->shifts[i]) continue;
	  state1 = shiftp->shifts[i];
	  symbol = accessing_symbol[state1];
/*	  if (ISVAR(symbol)) break;  */
	  fprintf(foutput, "    %-4s\tshift  %d\n", tags[symbol], state1);
	}

      if (i > 0)
	putc('\n', foutput);
    }
  else
    {
      i = 0;
      k = 0;
    }

  if (errp)
    {
      k = errp->nerrs;

      for (i = 0; i < k; i++)
	{
	  if (! errp->errs[i]) continue;
	  symbol = errp->errs[i];
	  fprintf(foutput, "    %-4s\terror (nonassociative)\n", tags[symbol]);
	}

      if (i > 0)
	putc('\n', foutput);
    }
  else
    {
      i = 0;
      k = 0;
    }

  if (consistent[state] && redp)
    {
      rule = redp->rules[0];
      symbol = rlhs[rule];
      fprintf(foutput, "    $default\treduce  %d  (%s)\n\n",
     	        rule, tags[symbol]);
    }
  else if (redp)
    {
      print_reductions(state);
    }

  if (i < k)
    {
      for (; i < k; i++)
	{
	  if (! shiftp->shifts[i]) continue;
	  state1 = shiftp->shifts[i];
	  symbol = accessing_symbol[state1];
	  fprintf(foutput, "    %-4s\tgoto  %d\n", tags[symbol], state1);
	}

      putc('\n', foutput);
    }
}
