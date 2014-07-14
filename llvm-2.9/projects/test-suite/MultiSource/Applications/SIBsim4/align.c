/* $Id: align.c 34335 2007-02-15 23:46:09Z nicholas $
 *
 * Christian Iseli, LICR ITO, Christian.Iseli@licr.org
 *
 * Copyright (c) 2001-2006 Swiss Institute of Bioinformatics.
 * Copyright (C) 1998-2001  Liliana Florea.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include "sim4.h"
#include "sim4b1.h"
#include "align.h"
#include "misc.h"

static int snake(uchar *,uchar *,int, int, int, int);
static int rsnake(uchar *,uchar *,int, int, int, int, int, int);


void
align_path(uchar *seq1, uchar *seq2, int i1, int j1, int i2, int j2, int dist,
	   edit_script_p_t *head, edit_script_p_t *tail, int M, int N)
{

        int     *last_d, *temp_d,       /* forward vectors */
                *rlast_d, *rtemp_d;     /* backward vectors */

        edit_script_p_t head1, tail1, head2, tail2;
        int midc, rmidc;
        int start, lower, upper;
        int rstart, rlower, rupper;
        int c, k, row;
        int mi, mj, tmp, ll, uu;
        char flag;

        *head = *tail = NULL;

        /* Boundary cases */
        if (i1 == i2) {
           if (j1 == j2) *head = NULL;
           else {
                head1 = (edit_script_p_t) xmalloc(sizeof(edit_script_t));
                head1->op_type = INSERT;
                head1->num = j2-j1;
                head1->next = NULL;
                *head = *tail = head1;
           }
           return;
        }

        if (j1 == j2) {
                head1 = (edit_script_p_t) xmalloc(sizeof(edit_script_t));
                head1->op_type = DELETE;
                head1->num = i2-i1;
                head1->next = NULL;
                *head = *tail = head1;
                return;
        }

        if (dist <= 1) {
           start = j1-i1;
           if (j2-i2 == j1-i1) {
                head1 = (edit_script_p_t) xmalloc(sizeof(edit_script_t));
                head1->op_type = SUBSTITUTE;
                head1->num = i2-i1;
                head1->next = NULL;
                *head = *tail = head1;
           } else if (j2-j1 == i2-i1+1) {

                tmp = snake(seq1,seq2,start,i1,i2,j2);
                if (tmp>i1) {
                    head1 = (edit_script_p_t) xmalloc(sizeof(edit_script_t));
                    head1->op_type = SUBSTITUTE;
                    head1->num = tmp-i1;
                    *head = head1;
                }
                head2 = (edit_script_p_t) xmalloc(sizeof(edit_script_t));
                head2->op_type = INSERT;
                head2->num = 1;

                if (*head) head1->next = head2;
                else *head = head2;
                *tail = head2;
                head2->next = NULL;

                if (i2-tmp) {
                    head1 = head2;
                    *tail = head2 = (edit_script_p_t) xmalloc(sizeof(edit_script_t));
                    head2->op_type = SUBSTITUTE;
                    head2->num = i2-tmp;
                    head2->next = NULL;
                    head1->next = head2;
                }
           } else if (j2-j1+1 == i2-i1) {

                tmp = snake(seq1,seq2,start,i1,i2,j2);
                if (tmp>i1) {
                    head1 = (edit_script_p_t) xmalloc(sizeof(edit_script_t));
                    head1->op_type = SUBSTITUTE;
                    head1->num = tmp-i1;
                    *head = head1;
                }
                head2 = (edit_script_p_t) xmalloc(sizeof(edit_script_t));
                head2->op_type = DELETE;
                head2->num = 1;

                if (*head) head1->next = head2;
                else *head = head2;
                *tail = head2;
                head2->next = NULL;

                if (i2>tmp+1) {
                    head1 = head2;
                    *tail = head2 = (edit_script_p_t) xmalloc(sizeof(edit_script_t));
                    head2->op_type = SUBSTITUTE;
                    head2->num = i2-tmp-1;
                    head2->next = NULL;
                    head1->next = head2;
                }
           } else {
                fprintf(stderr,
                      "align.c: warning: something wrong when aligning.");
           }
           return;
        }

        /* Divide the problem at the middle cost */
        midc = dist/2;
        rmidc = dist - midc;

        /* Compute the boundary diagonals */
        start = j1 - i1;
        lower = max(j1-i2, start-midc);
        upper = min(j2-i1, start+midc);
        rstart = j2-i2;
        rlower = max(j1-i2, rstart-rmidc);
        rupper = min(j2-i1, rstart+rmidc);

        /* Allocate space for forward vectors */
        last_d = (int *)xmalloc((upper-lower+1)*sizeof(int)) - lower;
        temp_d = (int *)xmalloc((upper-lower+1)*sizeof(int)) - lower;

        for (k=lower; k<=upper; k++) last_d[k] = -1;
        last_d[start] = snake(seq1,seq2,start,i1,i2,j2);

        /* Forward computation */
        for (c=1; c<=midc; ++c) {
             ll = max(lower,start-c);
             uu = min(upper,start+c);
             for (k=ll; k<=uu; ++k) {
                  if (k == ll) {
                      /* DELETE : down from (k+1,c-1) */
                      row = last_d[k+1]+1;
                  } else if (k == uu) {
                      /* INSERT : right from (k-1,c-1) */
                      row = last_d[k-1];
                  } else if ((last_d[k]>=last_d[k+1]) &&
                             (last_d[k]+1>=last_d[k-1])) {
                      /* SUBSTITUTE */
                      row = last_d[k]+1;
                  } else if ((last_d[k+1]+1>=last_d[k-1]) &&
                             (last_d[k+1]>=last_d[k])) {
                      /* DELETE */
                      row = last_d[k+1]+1;
                  } else {
                      /* INSERT */
                      row = last_d[k-1];
                  }

                  temp_d[k] = snake(seq1,seq2,k,row,i2,j2);
             }
             for (k=ll; k<=uu; ++k)
                  last_d[k] = temp_d[k];
       }

        /* Allocate space for backward vectors */
        rlast_d = (int *)xmalloc((rupper-rlower+1)*sizeof(int)) - rlower;
        rtemp_d = (int *)xmalloc((rupper-rlower+1)*sizeof(int)) - rlower;

        for (k=rlower; k<=rupper; k++) rlast_d[k] = i2+1;
        rlast_d[rstart] = rsnake(seq1,seq2,rstart,i2,i1,j1,M,N);

        /* Backward computation */
        for (c=1; c<=rmidc; ++c) {
             ll = max(rlower,rstart-c);
             uu = min(rupper,rstart+c);
             for (k=ll; k<=uu; ++k) {
                  if (k == ll) {
                      /* INSERT : left from (k+1,c-1) */
                      row = rlast_d[k+1];
                  } else if (k == uu) {
                      /* DELETE : up from (k-1,c-1) */
                      row = rlast_d[k-1]-1;
                  } else if ((rlast_d[k]-1<=rlast_d[k+1]) &&
                             (rlast_d[k]-1<=rlast_d[k-1]-1)) {
                      /* SUBSTITUTE */
                      row = rlast_d[k]-1;
                  } else if ((rlast_d[k-1]-1<=rlast_d[k+1]) &&
                             (rlast_d[k-1]-1<=rlast_d[k]-1)) {
                      /* DELETE */
                      row = rlast_d[k-1]-1;
                  } else {
                      /* INSERT */
                      row = rlast_d[k+1];
                  }

                  rtemp_d[k] = rsnake(seq1,seq2,k,row,i1,j1,M,N);
             }
             for (k=ll; k<=uu; ++k)
                  rlast_d[k] = rtemp_d[k];
       }

       /* Find (mi, mj) such that the distance from (i1, j1) to (mi, mj) is
          midc and the distance from (mi, mj) to (i2, j2) is rmidc.
        */

       flag = 0;
       mi = i1; mj = j1;
       ll = max(lower,rlower); uu = min(upper,rupper);
       for (k=ll; k<=uu; ++k) {
            if (last_d[k]>=rlast_d[k]) {
                if (last_d[k]-i1>=i2-rlast_d[k]) {
                    mi = last_d[k]; mj = k+mi;
                } else {
                    mi = rlast_d[k]; mj = k+mi;
                }
                flag = 1;

                break;
            }
       }
       free(last_d+lower); free(rlast_d+rlower);
       free(temp_d+lower); free(rtemp_d+rlower);

       if (flag) {
                /* Find a path from (i1,j1) to (mi,mj) */
                align_path(seq1,seq2,i1,j1,mi,mj,midc,&head1,&tail1,M,N);

                /* Find a path from (mi,mj) to (i2,j2) */
                align_path(seq1,seq2,mi,mj,i2,j2,rmidc,&head2,&tail2,M,N);

                /* Join these two paths together */
                if (head1) tail1->next = head2;
                else head1 = head2;
        } else {
                fprintf(stderr,
                      "align.c: warning: something wrong when dividing\n");
                head1 = NULL;
        }
        *head = head1;
        if (head2) *tail = tail2;
        else *tail = tail1;
}


int
align_get_dist(uchar *seq1, uchar *seq2, int i1, int j1, int i2, int j2,
	       int limit)
{
        int *last_d, *temp_d;
        int goal_diag, ll, uu;
        int c, k, row;
        int start, lower, upper;

        /* Compute the boundary diagonals */
        start = j1 - i1;
        lower = max(j1-i2, start-limit);
        upper = min(j2-i1, start+limit);
        goal_diag = j2-i2;

        if (goal_diag > upper || goal_diag < lower)
           return -1;

        /* Allocate space for forward vectors */
        last_d = (int *)xmalloc((upper-lower+1)*sizeof(int)) - lower;
        temp_d = (int *)xmalloc((upper-lower+1)*sizeof(int)) - lower;

        /* Initialization */
        for (k=lower; k<=upper; ++k) last_d[k] = INT_MIN;
        last_d[start] = snake(seq1,seq2,start, i1, i2, j2);

        if (last_d[goal_diag] >= i2) {
                /* Free working vectors */
                free(last_d+lower);
                free(temp_d+lower);
                return 0;
        }

        for (c=1; c<=limit; ++c) {
          ll = max(lower,start-c); uu = min(upper, start+c);
          for (k=ll; k<=uu; ++k) {
               if (k == ll)
                        row = last_d[k+1]+1;    /* DELETE */
               else if (k == uu)
                        row = last_d[k-1];      /* INSERT */
               else if ((last_d[k]>=last_d[k+1]) &&
                             (last_d[k]+1>=last_d[k-1]))
                        row = last_d[k]+1;      /*SUBSTITUTE */
               else if ((last_d[k+1]+1>=last_d[k-1]) &&
                             (last_d[k+1]>=last_d[k]))
                        row = last_d[k+1]+1;    /* DELETE */
               else
                        row = last_d[k-1];      /* INSERT */

               temp_d[k] = snake(seq1,seq2,k,row,i2,j2);
          }

          for (k=ll; k<=uu; ++k) last_d[k] = temp_d[k];

          if (last_d[goal_diag] >= i2) {
                 /* Free working vectors */
                    free(last_d+lower);
                    free(temp_d+lower);
                    return c;
           }
        }

        /* Ran out of distance limit */
        return -1;
}

/* Condense_both_Ends  --  merge contiguous operations of the same type    */
/* together; return both new ends of the chain.                            */
void Condense_both_Ends (edit_script_p_t *head, edit_script_p_t *tail,
			 edit_script_p_t *prev)
{
        edit_script_p_t tp, tp1;

        tp = *head; *prev = NULL;
        while (tp != NULL) {
           while (((tp1 = tp->next) != NULL) && (tp->op_type == tp1->op_type)) {
                 tp->num = tp->num + tp1->num;
                 tp->next = tp1->next;
                 free(tp1);
           }
           if (tp->next) *prev = tp;
           else *tail = tp;
           tp = tp->next;
        }
}

void S2A(edit_script_p_t head, int *S, int flag)
{
        edit_script_p_t tp;
        int *lastS, i;

        tp = head;
        lastS = S;
        while (tp != NULL) {
/*
        printf("tp->op_type=%d, tp->num=%d\n",tp->op_type, tp->num);
*/
           if (tp->op_type == SUBSTITUTE) {
                for (i=0; i<tp->num; ++i) *lastS++ = 0;
           } else if (tp->op_type == INSERT) {
                *lastS++ = (!flag) ? tp->num : (0-tp->num);
           } else {     /* DELETE */
                *lastS++ = (!flag) ? (0 - tp->num) : tp->num;
           }
           tp = tp->next;
        }
        *(S-1) = lastS - S;
}

/* Alignment display routine */

static uchar ALINE[51], BLINE[51], CLINE[51];

static int
get_pos_width(collec_p_t eCol)
{
  unsigned int last = eCol->e.exon[eCol->nb - 1]->to1 + options.dnaOffset;
  unsigned int w = 1;
  while ((last = last / 10) > 0)
    w += 1;
  if (w < 7)
    w = 7;
  return w;
}

void
IDISPLAY(uchar *A, uchar *B, unsigned int M, unsigned int N,
	 int *S, unsigned int AP, unsigned int BP, collec_p_t eCol,
	 int direction)
{
  uchar *a, *b, *c, sign;
  int op, index, starti;
  unsigned int i, j, lines, ap, bp, pWidth;
  unsigned int ii = 0;
  exon_p_t ep;

  assert(eCol->nb > 0);

  pWidth = get_pos_width(eCol);
  /* find the starting exon for this alignment */
  while (ii < eCol->nb
	 && ((ep = eCol->e.exon[ii])->from1 != AP || ep->from2 != BP))
    ii += 1;
  if (ii >= eCol->nb)
    fatal("align.c: Alignment fragment not found.\n");
  i = j = op = lines = index = 0;
  sign = '*';
  ap = AP;
  bp = BP;
  a = ALINE;
  b = BLINE;
  c = CLINE;
  starti = (ii < eCol->nb - 1) ? (int) ep->to1 + 1 : -1;

  while (i < M || j < N) {
    if (op == 0 && *S == 0) {
      op = *S++;
      *a = A[++i];
      *b = B[++j];
      *c++ = (*a++ == *b++) ? '|' : ' ';
    } else {
      if (op == 0)
	op = *S++;
      if (op > 0) {
	*a++ = ' ';
	*b++ = B[++j];
	*c++ = '-';
	op--;
      } else {
	if ((int) (i + AP) == starti) {
	  /* detected intron */
	  if (ep->type < 0 || direction == 0)
	    sign = '=';
	  else if (direction > 0)
	    sign = '>';
	  else
	    sign = '<';
	  ii += 1;
	  ep = (ii < eCol->nb) ? eCol->e.exon[ii] : NULL;
	  starti = (ii < eCol->nb - 1) ? (int) ep->to1 + 1 : -1;
	  index = 1;
	  *c++ = sign;
	  *a++ = A[++i];
	  *b++ = ' ';
	  op++;
   	} else if (!index) {
	  *c++ = '-';
	  *a++ = A[++i];
	  *b++ = ' ';
	  op++;
   	} else {
	  /* not the first deletion in the intron */
	  switch (index) {
	  case 0:
	  case 1:
	  case 2:
	    *a++ = A[++i];
	    *b++ = ' ';
	    *c++ = sign;
	    op++;
	    index++;
	    break;
	  case 3:
	  case 4:
	    *a++ = '.';
	    *b++ = ' ';
	    *c++ = '.';
	    i++;
	    op++;
	    index++;
	    break;
	  case 5:
	    *a++ = '.';
	    *b++ = ' ';
	    *c++ = '.';
	    i += (-op) - 3;
	    op = -3;
	    index++;
	    break;
	  case 6:
	  case 7:
	    *a++ = A[++i];
	    *b++ = ' ';
	    *c++ = sign;
	    op++;
	    index++;
	    break;
	  case 8:
	    *a++ = A[++i];
	    *b++ = ' ';
	    *c++ = sign;
	    op++;
	    index = 0;
	    break;
   	  }
	}
      }
    }
    if (a >= ALINE + 50 || (i >= M && j >= N)) {
      *a = *b = *c = '\0';
      printf("\n%*u ", pWidth, 50 * lines++);
      for (b = ALINE + 10; b <= a; b += 10)
	printf("    .    :");
      if (b <= a + 5)
	printf("    .");
      printf("\n%*u %s\n%*s %s\n%*u %s\n",
	     pWidth, ap + options.dnaOffset, ALINE,
	     pWidth, " ", CLINE,
	     pWidth, bp, BLINE);
      ap = AP + i;
      bp = BP + j;
      a = ALINE;
      b = BLINE;
      c = CLINE;
    }
  }
}

void
Free_script(edit_script_p_t head)
{
        edit_script_p_t tp, tp1;

        tp = head;
        while (tp != NULL) {
           tp1 = tp->next;
           free(tp);
           tp = tp1;
        }
}

static int
snake(uchar *seq1, uchar *seq2, int k, int x, int endx, int endy)
{
        int y;

        if (x<0) return x;
        y = x+k;
        while (x<endx && y<endy && seq1[x]==seq2[y]) {
                ++x; ++y;
        }
        return x;
}


static int
rsnake(uchar *seq1, uchar *seq2, int k, int x, int startx, int starty,
       int M, int N)
{
  int y;

  if (x > M)
    return x;
  if (startx < 0 || starty < 0)
    fprintf(stderr, "TROUBLE!!! startx:  %5d,  starty:  %5d\n",startx, starty);
  if (x + k > N)
    fprintf(stderr, "TROUBLE!!! x:  %5d,  y:  %5d\n",x,x+k);

  y = x + k;
  while (x > startx && y > starty && seq1[x - 1] == seq2[y - 1]) {
    x -= 1;
    y -= 1;
  }
  return x;
}
