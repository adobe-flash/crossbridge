/* Allocate input grammar variables for bison,
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

/* comments for these variables are in gram.h  */

int nitems;
int nrules;
int nsyms;
int ntokens;
int nvars;

short *ritem;
short *rlhs;
short *rrhs;
short *rprec;
short *sprec;
short *rassoc;
short *sassoc;
short *token_translations;
short *rline;

int start_symbol;

int translations;

int max_user_token_number;

int semantic_parser;

int pure_parser;

int error_token_number;
