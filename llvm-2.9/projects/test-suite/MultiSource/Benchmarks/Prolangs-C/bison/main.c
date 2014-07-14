/* Top level entry point of bison,
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
#include "machine.h"	/* JF for MAXSHORT */

extern	int lineno;
extern	int verboseflag;

/* Nonzero means failure has been detected; don't write a parser file.  */
int failure;

extern void getargs(int argc,char *argv[]);
extern void openfiles(void);
extern void reader(void);
extern void set_derives(void);
extern void set_nullable(void);
extern void generate_states(void);
extern void lalr(void);
extern void initialize_conflicts(void);
extern void verbose(void);
extern void terse(void);
extern void output(void);
extern void done(int k);

int main(int argc,char *argv[])
{
  failure = 0;
  lineno = 0;
  getargs(argc, argv);
  openfiles();

  /* read the input.  Copy some parts of it to fguard, faction, ftable and fattrs.
     In file reader.
     The other parts are recorded in the grammar; see gram.h.  */
  reader();

  /* record other info about the grammar.  In files derives and nullable.  */
  set_derives();
  set_nullable();

  /* convert to nondeterministic finite state machine.  In file LR0.
     See state.h for more info.  */
  generate_states();

  /* make it deterministic.  In file lalr.  */
  lalr();

  /* Find and record any conflicts: places where one token of lookahead is not
     enough to disambiguate the parsing.  In file conflicts.
     Currently this does not do anything to resolve them;
     the trivial form of conflict resolution that exists is done in output.  */
  initialize_conflicts();

  /* print information about results, if requested.  In file print. */
  if (verboseflag)
    verbose();
  else
    terse();

  /* output the tables and the parser to ftable.  In file output. */
  output();
  done(failure);
  return 0;
}

/* functions to report errors which prevent a parser from being generated */

void fatal(char *s)
{
  extern char *infile;

  if (infile == 0)
    fprintf(stderr, "fatal error: %s\n", s);
  else
    fprintf(stderr, "\"%s\", line %d: %s\n", infile, lineno, s);
  done(1);
}


/* JF changed to accept/deal with variable args.  Is a real kludge since
   we don't support _doprnt calls */
/*VARARGS1*/

void fatals(char *fmt,int x1,int x2,int x3,int x4,int x5,int x6,int x7,int x8)
{
  char buffer[200];

  sprintf(buffer, fmt, x1,x2,x3,x4,x5,x6,x7,x8);
  fatal(buffer);
}



void toomany(char *s)
{
  char buffer[200];

	/* JF new msg */
  sprintf(buffer, "limit of %d exceeded, too many %s", MAXSHORT, s);
  fatal(buffer);
}

void berror(char *s)
{
  fprintf(stderr, "internal error, %s\n", s);
  abort();
}
