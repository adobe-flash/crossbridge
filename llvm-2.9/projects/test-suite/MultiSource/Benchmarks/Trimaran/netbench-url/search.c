/*
 * search.c - Searching routines for URL switching
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

#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "search.h"
#include "url.h"

#define min(a, b) (a > b ? b : a)

extern StrTreeNode *tree_head;

int * 
calculate_bm_table (char *pattern, int pattern_length) 
{
  int i;
  int *returnval = (int *) malloc (sizeof(int) * CHAR_SIZE);

  if (returnval == NULL)
    fatal ("Not enough virtual memory \n");

  for (i = 0; i < CHAR_SIZE; ++i)
    returnval[i] = pattern_length;
  
  for (i = 0; i < pattern_length; ++i)
    returnval[(unsigned char)pattern[i]] = pattern_length - i - 1;

  return returnval;
}


/********************************************************************\
* boyer_moore: Implements the Boyer-Moore al;gorithm to search the   *
*              string x within string x. This is known to be one of  *
*              the most efficient string search algorithms.          *
*                                                                    *
* Inputs:   Large and small pattern information                      *
*           bmBc: the character table used for the algorithm,        *
*           instead of generating it for every call, we generate the *
*           table once in the creation and use it for every search.  *
*                                                                    *
* Returns:  The location of the match, -1 if no match                *
\********************************************************************/
int 
boyer_moore (char *y, int n, char *x, int m, int *bmBc) 
{
  int j, k, shift;
  
  shift = m - 1;
  j = 0;
  /* while (j < n) */
  /* Changed while-loop bound to prevent reading off the end of the array.
   * -KF 2/2007 */
  while (j + m - 1 < n)
    {
      k = bmBc[(unsigned char)y[j + m -1]];
      /* while ((k !=  0) && (j < n)) */
      /* Changed while-loop bound to prevent reading off the end of the array.
       * -KF 2/2007 */
      while ((k !=  0) && (j + k + m - 1 < n)) 
	{
	  j += k; 
	  k = bmBc[(unsigned char)y[j + m -1]];
	}
      
      if (memcmp(x, y + j, m - 1) == 0 && (j < n))
	return(j);
      j += shift; 
    }
  return -1;
}

struct rtentry *
find_destination (char *packet, int packet_size)
{
  StrTreeNode *iterator = tree_head;

  while (iterator)
    {
#ifdef DEBUG_2
      fprintf (stderr, "Comparing to %s \n", iterator->common_pattern);
#endif

      /* If the packet contains the pattern at the header... */
      if ((boyer_moore (packet, packet_size, iterator->common_pattern, iterator->common_pattern_length, iterator->bm_table) != -1))
	{
	  /* We found a common substring in the header, so look into the list... */
	  PatternNode *list = iterator->list;
	  while (list)
	    {
	      int search_result;      
#ifdef DEBUG_2
	      fprintf (stderr, "Inside comparing to %s \n", iterator->common_pattern);
#endif
	      if (list->search_depth == 0)
		search_result = boyer_moore (packet, packet_size, list->pattern, list->pattern_length, list->bm_table);
	      else
		search_result = boyer_moore (packet, min(packet_size, list->search_depth), list->pattern, list->pattern_length, list->bm_table);
	      
	      if (search_result != -1)
		{
#ifdef DEBUG
		  fprintf (stderr, "Found a packet \n");
#endif
		  /* An exact match */
		  if (list->type == log)
		    fprintf (list->logto, "Source: %x Destination %x \n", PACKET_SOURCE(packet), PACKET_DESTINATION(packet));
		  return list->destination;
		}
	      list = list->below;
	    }
	}
      iterator = iterator->next;
    }

  /* No match, no rule can be find for the packet... */
  return NULL;
}


/********************************************************************\
* Procedures to generate empty nodes.                                *
\********************************************************************/
PatternNode *
NewPatternNode (int chainno, nodetype type, char *pattern, int pattern_length, int search_depth)
{
  PatternNode *returnval;

  if ((returnval = (PatternNode *) malloc (sizeof (PatternNode))) == NULL)
    fatal ("Run out of virtual memory \n");

  returnval->chain_node_number = chainno;
  returnval->type = type;
  if ((returnval->pattern = (char *) malloc (pattern_length)) == NULL)
    fatal ("Not enough virtual memory \n");
  strncpy (returnval->pattern, pattern, pattern_length);
  returnval->pattern_length = pattern_length;
  returnval->search_depth = search_depth;
  returnval->bm_table = calculate_bm_table (pattern, pattern_length);

  return returnval;
}

StrTreeNode *
NewStrTreeNode (int chainno, char *pattern, int pattern_length)
{
  StrTreeNode *returnval;

  if ((returnval = (StrTreeNode *) malloc (sizeof (StrTreeNode))) == NULL)
    fatal ("Run out of virtual memory \n");

  returnval->chain_node_number = chainno;
  if ((returnval->common_pattern = (char *) malloc (pattern_length)) == NULL)
    fatal ("Not enough virtual memory \n");
  strncpy (returnval->common_pattern, pattern, pattern_length);
  returnval->common_pattern_length = pattern_length;
  returnval->bm_table = calculate_bm_table (returnval->common_pattern, pattern_length);

  return returnval;
}


/********************************************************************\
* Find_lcs: A modified version of the boyer-moore algorithm to find  *
*           the largest common substring                             *
*                                                                    *
* Inputs:   Large and small pattern information                      *
*           lcssize: return value of the procedure                   *
*                                                                    *
* Returns:  The largest substring found                              *
\********************************************************************/
char *
find_lcs (char *largepattern, int largepatternsize, char *smallpattern, int smallpatternsize, int *lcssize)
{
  char *sub_pattern = NULL;
  char *exchanger;
  int sub_pat_size = 0;
  int j, k, shift, exchangersize;
  int *bmBc;

  /* Make sure the large is larger */
  if (smallpatternsize > largepatternsize)
    {
      exchanger = smallpattern;
      smallpattern = largepattern;
      largepattern = exchanger;
      exchangersize = smallpatternsize;
      smallpatternsize = largepatternsize;
      largepatternsize = exchangersize;
    }
  
  bmBc = calculate_bm_table (smallpattern, smallpatternsize);

  shift = 1;
  j = 0;
  k = bmBc[(unsigned char)largepattern[j + smallpatternsize -1]];
  while (j < largepatternsize) 
    {
      j += k; 
      k = bmBc[(unsigned char)largepattern[j + smallpatternsize -1]];
      /* Unlike original BM we can not shift the whole subpattern */
      if (k == smallpatternsize)
	k = sub_pat_size + 1;
      else
	{
	  /* Match in the characters, check how big the substring is */	    
	  int first_char = j + smallpatternsize - 1;
	  int temp_sub_size = 0;

	  /* First move back to find the start of the partial match */
	  while ((k < smallpatternsize) && (smallpattern[smallpatternsize - k - 1] == largepattern[first_char])) 
	    {
	      first_char--;
	      k++;
	    }
	  k--;
	  first_char++;

	  /* Now move forward until there is a mismatch */
	  while ((k >= 0) && (smallpattern[smallpatternsize - k - 1] == largepattern[first_char])) 
	    {
	      temp_sub_size++;
	      k--;
	      first_char++;
	    }
	  
	  if (temp_sub_size > sub_pat_size)
	    {
	      sub_pat_size = temp_sub_size;
	      sub_pattern = &largepattern[first_char - temp_sub_size];

	      /* If it is equal to the small pattern, no need to continue */
	      if (sub_pat_size == smallpatternsize)
		{
		  *lcssize = sub_pat_size;
		  return sub_pattern;
		}
	    }

	  k = 1;
	}
    }
  *lcssize = sub_pat_size;
  free(bmBc);
  return sub_pattern;
}

void 
insert_rule (char *pattern, int pattern_length, int depth)
{
  StrTreeNode *head_iterator = tree_head;
  StrTreeNode *found_largestcommon = NULL;
  unsigned int largest_common = 0;
  char *sub_str;

  /* First traverse all the headers finding the longest common substring */
  while (head_iterator)
    {
      int temp_common;
      char *temp_str = find_lcs (head_iterator->common_pattern, head_iterator->common_pattern_length, pattern, pattern_length, &temp_common);
     
#ifdef DEBUG_2
      fprintf (stdout, "Compared %s to %s and found %s with size %d \n", head_iterator->common_pattern, pattern, temp_str, temp_common);
#endif 
     
      if (temp_common > largest_common)
	{
	  largest_common = temp_common;
	  found_largestcommon = head_iterator;
	  sub_str = temp_str;
	}
      head_iterator = head_iterator->next;
    }
  
  if (largest_common < MIN_COMMON_SIZE)
    {
      /* We have to create a new head */
      PatternNode *new_node;
      StrTreeNode *new_head;
      
      if (tree_head != NULL)
	new_head = NewStrTreeNode (tree_head->chain_node_number + 1, pattern, pattern_length);
      else
	new_head = NewStrTreeNode (0, pattern, pattern_length);
      
      new_node = NewPatternNode (0, route, pattern, pattern_length, depth);
      new_node->below = NULL;

      new_head->list = new_node;
      new_head->next = tree_head;
      tree_head = new_head;
    }
  else
    {
      /* Found a header matching the common value that is large enough */
      PatternNode *new_node;
      
      /* First change the head pattern (chunk it off) */
      strncpy (found_largestcommon->common_pattern, sub_str, largest_common);
      found_largestcommon->common_pattern_length = largest_common;
      
      /* Add the entry to the head list */
      new_node = NewPatternNode (found_largestcommon->list->chain_node_number + 1, route, pattern, pattern_length, depth);
      new_node->below = found_largestcommon->list;
      found_largestcommon->list = new_node;
    }  
}

void 
db_init (char *file_name)
{
  FILE *pattern_file;
  char line[BUF_SIZE];
  char *reader;
  int i;
  char *pattern;


  if ((pattern_file = fopen (file_name, "r")) == NULL)
    fatal ("Can not open the input file\n");

  /* Initialize the tree structure */
  while (fgets (line, BUF_SIZE, pattern_file) != NULL)
    {
      int value;
      int pattern_length, search_depth;

      reader = line + 1;
      switch (line[0])
	{
	case '1':
	  sscanf (reader, "(%d)", &pattern_length);
	  search_depth = 0;
	  break;
	case '2':
	  sscanf (reader, "(%d:%d)", &pattern_length, &search_depth);
	  break;
	default:
	  fatal ("unrecognized input line start: %c \n", line[0]);
	}

      if ((pattern = (char *) malloc (pattern_length + 1)) == NULL)
	fatal ("Not enough virtual memory \n");
      
      for (i = 0; i < pattern_length; i++)
	{
	  while (*reader != ',')
	    reader++;
	  reader++;
	  sscanf (reader, " %d", &value);
	  pattern[i] = (char)value;
#ifdef DEBUG_2	      
	  fprintf (stdout, "Read value: %d \n", value);
#endif  
	}
      
      pattern[pattern_length] = '\0';

#ifdef DEBUG_2
      fprintf (stdout, "Pattern: %s, length: %d depth: %d, tree before: %x\n", pattern, pattern_length, search_depth, tree_head);
#endif

      insert_rule (pattern, pattern_length, search_depth);
      
#ifdef DEBUG_2
      fprintf (stdout, "Tree after: %x\n", tree_head);
#endif
    }
}
