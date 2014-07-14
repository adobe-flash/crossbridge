/* Token type definitions for bison's input reader,
   Copyright (C) 1984 Bob Corbett and Free Software Foundation, Inc.

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

#define	ENDFILE		0
#define	IDENTIFIER	1
#define	COMMA		2
#define COLON		3
#define	SEMICOLON	4
#define	BAR		5
#define LEFT_CURLY	6
#define TWO_PERCENTS	7
#define PERCENT_LEFT_CURLY	8
#define	TOKEN		9
#define	NTERM		10
#define GUARD	       11
#define	TYPE	       12
#define	UNION	       13
#define START	       14
#define LEFT	       15
#define RIGHT	       16
#define NONASSOC       17
#define PREC	       18
#define SEMANTIC_PARSER 19
#define PURE_PARSER    20
#define TYPENAME       21
#define NUMBER	       22
#define EXPECT	       23
#define	ILLEGAL	       24

#define	MAXTOKEN	1024
