/*
 * search.h - Searching routines for URL switching
 *
 * This file is a part of the NetBench
 *
 * This source file is distributed "as is" in the hope that it will be
 * useful. The suite comes with no warranty, and no author or
 * distributor accepts any responsibility for the consequences of its
 * use. 
 * 
 * Everyone is granted permission to copy, modify and redistribute
 * this tool set under the following conditions:
 * 
 *    Permission is granted to anyone to make or distribute copies
 *    of this source code, either as received or modified, in any
 *    medium, provided that all copyright notices, permission and
 *    nonwarranty notices are preserved, and that the distributor
 *    grants the recipient permission for further redistribution as
 *    permitted by this document.
 *
 *    Permission is granted to distribute this file in compiled
 *    or executable form under the same conditions that apply for
 *    source code, provied that either:
 *
 *    A. it is accompanied by the corresponding machine-readable
 *       source code,
 *    B. it is accompanied by a written offer, with no time limit,
 *       to give anyone a machine-readable copy of the corresponding
 *       source code in return for reimbursement of the cost of
 *       distribution.  This written offer must permit verbatim
 *       duplication by anyone, or
 *    C. it is distributed by someone who received only the
 *       executable form, and is accompanied by a copy of the
 *       written offer of source code that they received concurrently.
 *
 * In other words, you are welcome to use and share this source file.
 * You are forbidden to forbid anyone else to use, share and improve
 * what you give them. 
 */

#ifdef LITTLE_ENDIAN
#define PACKET_LENGTH(packet)	  (((unsigned char)packet[2] << 8) + (unsigned char)packet[3]) 
#else
#define PACKET_LENGTH(packet)	  (*(unsigned short *)&packet[2])
#endif

#define PACKET_SOURCE(packet)	  	  (*(unsigned int *)&packet[12])
#define PACKET_DESTINATION(packet)	  (*(unsigned int *)&packet[16])

#define BUF_SIZE 2048

typedef enum _nodetype {log, route} nodetype;

#define CHAR_SIZE 256

/* Patterns are stored in a two-dimensional link. The headers contain
   a common substring for all the patterns in the nodes stored in the
   link below. The links contain the actual elements to be searched */

typedef struct _PatternNode
{
  int chain_node_number;  /* Label */
  
  nodetype type;           /* log and route, or just route */
  int proto;               /* protocol, currently not used */
  FILE *logto;             /* log file in which to write packets which 
			  match this rule, if the logging is activated. */
  char *pattern;           /* pattern to search for */
  int pattern_length;      /* Length of the pattern to be searched */
  int search_depth;        /* Length of section to be searched */
  int *bm_table;           /* BM Table used for searching */
  struct rtentry *destination;   /* Where to forward packets falling for the rule */
  struct _PatternNode *below;    /* The list ... */
} PatternNode;


typedef struct _StrTreeNode
{
  int chain_node_number;         /* Label */
  char *common_pattern;          /* pattern to search for */
  int common_pattern_length;     /* Length of the pattern to be searched */
  int *bm_table;                 /* BM Table used for searching */
  struct _StrTreeNode *next;     /* Next head in the list */
  struct _PatternNode *list;     /* List of patterns for the head (common subpattern) */
} StrTreeNode;

int boyer_moore (char *, int, char *, int, int *); 
struct rtentry *find_destination (char *, int);
void db_init (char *);
