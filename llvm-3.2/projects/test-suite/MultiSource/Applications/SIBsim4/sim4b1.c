/* $Id: sim4b1.c 34335 2007-02-15 23:46:09Z nicholas $
 *
 * Christian Iseli, LICR ITO, Christian.Iseli@licr.org
 *
 * Copyright (c) 2001-2006 Swiss Institute of Bioinformatics.
 * Copyright (C) 1998-2001  Liliana Florea.
 */

#define _GNU_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <search.h>
#include <ctype.h>
#include "sim4.h"
#include "sim4b1.h"
#include "align.h"
#include "misc.h"

int encoding[NACHARS];

static void merge(collec_p_t, collec_p_t, unsigned int, unsigned int);
static void slide_intron(result_p_t, uchar *, uchar *);
static void compact_exons(collec_p_t, unsigned int);
static int greedy(uchar *, uchar *, unsigned int, unsigned int,
		  unsigned int, unsigned int, unsigned int, collec_p_t);
static int extend_bw(uchar *,uchar *,int,int,int,int,int *,int *, int);
static int extend_fw(uchar *,uchar *,int,int,int,int,int *,int *, int);
static int  pluri_align(uchar *, uchar *, unsigned int *, collec_p_t,
			edit_script_list_p_t *, int, int);
static exon_p_t new_exon(unsigned int, unsigned int,
			 unsigned int,unsigned int);
static void extend_hit(int, int, hash_env_p_t, const uchar * const,
		       unsigned int, int, collec_p_t, int *);
static int msp_compare(const void *, const void *);
static int msp_rna_compare(const void *, const void *);
static void search(hash_env_p_t, uchar *,
		   unsigned int, int, collec_p_t);
static void trim_small_repeated_msps(collec_p_t);
static void combine_msps(collec_p_t);
static int link_msps(collec_p_t, unsigned int, unsigned int);
static void msp2exons(exon_p_t *, int, collec_p_t, int);
static void exon_cores(hash_env_p_t, uchar *, unsigned int, int, int, int,
		       collec_p_t, collec_p_t, collec_p_t);
static int good_ratio(int, int);
static void swap_seqs(collec_p_t);
static int SWscore(uchar *, uchar *, unsigned int);

#ifdef DEBUG
static void debug_print_exons(collec_p_t, const char *,
  const unsigned char *, const unsigned char *);
#endif

static int
is_polyAT_exon_p(exon_p_t e, const unsigned char *s)
{
  unsigned int cntA = 0;
  unsigned int cntC = 0;
  unsigned int cntG = 0;
  unsigned int cntT = 0;
  unsigned int cntN = 0;
  unsigned int i;
  unsigned int len = e->to2 - e->from2 + 1;
  for (i = e->from2 - 1; i < e->to2; i++)
    switch (s[i]) {
    case 'A':
      cntA += 1;
      break;
    case 'C':
      cntC += 1;
      break;
    case 'G':
      cntG += 1;
      break;
    case 'T':
      cntT += 1;
      break;
    default:
      cntN += 1;
    }
  len -= cntN;
  if (len < MIN_INTRON) {
    if ((cntA * 10) / len >= 7
	|| ((cntA + cntG) * 10) / len >= 8
	|| (cntT * 10) / len >= 7
	|| ((cntT + cntC) * 10) / len >= 8)
      return 1;
  } else {
    if ((cntA * 10) / len >= 8
	|| ((cntA + cntG) * 100) / len >= 95
	|| (cntT * 10) / len >= 8
	|| ((cntT + cntC) * 100) / len >= 95)
      return 1;
  }
  return 0;
}

static void
kill_polyA(result_p_t res, const unsigned char *s1, const unsigned char *s2)
{
  unsigned int i;
  collec_p_t eCol = &res->eCol;
  /* Stupid initialization below to avoid spurious uninitialized warning
   * from GCC...  */
  struct {int score; unsigned int cnt; unsigned int d;} best = best;
  i = 0;
  while (i < eCol->nb && is_polyAT_exon_p(eCol->e.exon[i], s2))
    i += 1;
  if (i > 0) {
    unsigned int j;
    for (j = 0; j < i; j++)
      free(eCol->e.exon[j]);
    memmove(eCol->e.elt, eCol->e.elt + i,
	    (eCol->nb - i) * sizeof(void *));
    eCol->nb -= i;
  }
  i = 0;
  while (i < eCol->nb
	 && is_polyAT_exon_p(eCol->e.exon[eCol->nb - i - 1], s2))
    i += 1;
  if (i > 0) {
    unsigned int j;
    for (j = eCol->nb - i; j < eCol->nb; j++)
      free(eCol->e.exon[j]);
    eCol->nb -= i;
  }
  if (eCol->nb > 0) {
    exon_p_t e = eCol->e.exon[eCol->nb - 1];
    unsigned int cntAs1 = 0, cntAs2 = 0, j = 0;
    int score = 0;
    const unsigned char *s = s2 + e->to2;
    best.score = 0;
    while (*s && best.score - score < 10) {
      j += 1;
      switch (*s) {
      case 'A':
	cntAs2 += 1;
	score += 1;
	if (score > best.score) {
	  best.score = score;
	  best.cnt = cntAs2;
	  best.d = j;
	}
	break;
      case 'N':
	break;
      default:
	score -= 2;
      }
      s += 1;
    }
    if (best.score > 0 && best.cnt >= 8 && (best.cnt * 10) / best.d >= 8) {
      s = s1 + e->to1;
      j = 0;
      while (*s && j < best.d) {
	j += 1;
	if (*s == 'A')
	  cntAs1 += 1;
	s += 1;
      }
      if (j > 0 && (cntAs1 * 10) / j < 8) {
	res->st.polyA_cut = 1;
      }
    }
  }
  if (eCol->nb > 0) {
    exon_p_t e = eCol->e.exon[0];
    unsigned int cntTs1 = 0, cntTs2 = 0, j = 0;
    int score = 0;
    const unsigned char *s = s2 + e->from2 - 2;
    best.score = 0;
    while (s >= s2 && best.score - score < 10) {
      j += 1;
      switch (*s) {
      case 'T':
	cntTs2 += 1;
	score += 1;
	if (score > best.score) {
	  best.score = score;
	  best.cnt = cntTs2;
	  best.d = j;
	}
	break;
      case 'N':
	break;
      default:
	score -= 2;
      }
      s -= 1;
    }
    if (best.score > 0 && best.cnt >= 8 && (best.cnt * 10) / best.d >= 8) {
      s = s1 + e->from1 - 2;
      j = 0;
      while (s >= s1 && j < best.d) {
	j += 1;
	if (*s == 'T')
	  cntTs1 += 1;
	s -= 1;
      }
      if (j > 0 && (cntTs1 * 10) / j < 8) {
	res->st.polyT_cut = 1;
      }
    }
  }
}

static void
grow_exon_left(exon_p_t e, uchar *s1, uchar *s2)
{
  uchar *p1 = s1 + e->from1 - 2;
  uchar *p2 = s2 + e->from2 - 2;
  while (p1 >= s1 && p2 >= s2 && *p1 == *p2) {
    p1 -= 1;
    p2 -= 1;
    e->from1 -= 1;
    e->from2 -= 1;
  }
}

static void
grow_exon_right(exon_p_t e, uchar *s1, unsigned int l1,
		uchar *s2, unsigned int l2)
{
  while (e->to1 < l1 && e->to2 < l2 && s1[e->to1] == s2[e->to2]) {
    e->to1 += 1;
    e->to2 += 1;
  }
}

/* seq1 = genomic  DNA (text); seq2 = cDNA */
void
SIM4(hash_env_p_t he, seq_p_t seq2, collec_p_t res)
{
  collec_t mCol;
  collec_t tem_eCol;
  int align_status;
  unsigned int curRes;

  if (he->len == 0 || seq2->len == 0)
    return;
  init_col(&mCol, 5);
  /* Compute the distance between two sequences A and B */
  exon_cores(he, seq2->seq, seq2->len, 1, 1, options.K, &mCol, res, NULL);
  init_col(&tem_eCol, 0);
  for (curRes = 0; curRes < res->nb; curRes++) {
    result_p_t r = res->e.result[curRes];
    collec_p_t eCol = &r->eCol;
    sim4_stats_p_t st = &r->st;
#ifdef DEBUG
    debug_print_exons(eCol, "LSIS", he->seq, seq2->seq);
#endif
    /* Chase down polyA tails.  */
    st->polyA_cut = 0;
    st->polyT_cut = 0;
    kill_polyA(r, he->seq, seq2->seq);
#ifdef DEBUG
    debug_print_exons(eCol, "LSIS 2", he->seq, seq2->seq);
#endif
    if (eCol->nb == 0)
      continue;
    /* Look at the first exon, and try to extend it backward.  */
    if (!st->polyT_cut && eCol->e.exon[0]->from2 > 1) {
      exon_p_t e = eCol->e.exon[0];
      unsigned int i = 0;
      if (e->from2 - 1 > (MIN_INTRON << 1)
	  && e->from1 - 1 > r->dStart) {
	hash_env_t tem_he;
#ifdef DEBUG
	fprintf(stderr, "Find new exons (head) %d %d\n", e->from1, e->from2);
#endif
	init_hash_env(&tem_he, min(10, he->W), seq2->seq, e->from2 - 1);
	bld_table(&tem_he);
	exon_cores(&tem_he, he->seq + r->dStart, e->from1 - r->dStart - 1,
		   1, r->dStart + 1, options.C, &mCol, NULL, &tem_eCol);
	free_hash_env(&tem_he);
	/* Insert new exons (merging if needed), swaping seqs.  */
	if (tem_eCol.nb > 0) {
	  swap_seqs(&tem_eCol);
	  grow_exon_right(tem_eCol.e.exon[tem_eCol.nb - 1],
			  he->seq, he->len, seq2->seq, seq2->len);
	  merge(eCol, &tem_eCol, 0, he->W);
	  tem_eCol.nb = 0;
	  e = eCol->e.exon[0];
	}
      }
      while (i < eCol->nb && is_polyAT_exon_p(eCol->e.exon[i], seq2->seq))
	i += 1;
      if (i > 0) {
	unsigned int j;
	for (j = 0; j < i; j++)
	  free(eCol->e.exon[j]);
	memmove(eCol->e.elt, eCol->e.elt + i,
		(eCol->nb - i) * sizeof(void *));
	eCol->nb -= i;
	if (eCol->nb == 0)
	  continue;
	e = eCol->e.exon[0];
      }
      if (e->from2 - 1 > 0) {
	int diff = min(e->from2 - 1, MAX_GRINIT >> 1);
	int u = min(4 * diff, (int) e->from1 - 1);
	int I, J, cost;
#ifdef DEBUG
	fprintf(stderr, "extend_bw from %d\n", e->from2);
#endif
	cost = extend_bw(seq2->seq + e->from2 - 1 - diff,
			 he->seq + e->from1 - 1 - u,
			 diff, u, e->from2 - 1 - diff, e->from1 - 1 - u,
			 &I, &J, he->W);
#ifdef DEBUG
	fprintf(stderr, "extend_bw returned %d, I: %d J: %d\n", cost, I, J);
#endif
	if (((int) e->from2 - 1 - I) * options.matchScore
	    + cost * options.mismatchScore >= 0) {
	  e->from2 = I + 1;
	  e->from1 = J + 1;
	}
      }
    }
    /* Look at the last exon, and try to extend it forward.  */
    if (!st->polyA_cut && eCol->e.exon[eCol->nb - 1]->to2 < seq2->len) {
      exon_p_t e = eCol->e.exon[eCol->nb - 1];
      unsigned int i = 0;
      if (seq2->len - e->to2 > (MIN_INTRON << 1)
	  && e->to1 < r->dStart + r->dLen) {
	hash_env_t tem_he;
#ifdef DEBUG
	fprintf(stderr, "Find new exons (tail) %d %d\n", e->to1, e->to2);
#endif
	init_hash_env(&tem_he, min(10, he->W),
		      seq2->seq + e->to2, seq2->len - e->to2);
	bld_table(&tem_he);
	exon_cores(&tem_he, he->seq + e->to1, r->dStart + r->dLen - e->to1,
		   e->to2 + 1, e->to1 + 1, options.C, &mCol, NULL, &tem_eCol);
	free_hash_env(&tem_he);
	/* Append new exons (merging if needed), swaping seqs.  */
	if (tem_eCol.nb > 0) {
	  swap_seqs(&tem_eCol);
	  grow_exon_left(tem_eCol.e.exon[0], he->seq, seq2->seq);
	  merge(eCol, &tem_eCol, eCol->nb, he->W);
	  tem_eCol.nb = 0;
	  e = eCol->e.exon[eCol->nb - 1];
	}
      }
      while (i < eCol->nb
	     && is_polyAT_exon_p(eCol->e.exon[eCol->nb - i - 1], seq2->seq))
	i += 1;
      if (i > 0) {
	unsigned int j;
	for (j = eCol->nb - i; j < eCol->nb; j++)
	  free(eCol->e.exon[j]);
	eCol->nb -= i;
	if (eCol->nb == 0)
	  continue;
	e = eCol->e.exon[eCol->nb - 1];
      }
      if (seq2->len - e->to2 > 0) {
	int diff = min(seq2->len - e->to2, MAX_GRINIT >> 1);
	int  cost, I, J;
#ifdef DEBUG
	fprintf(stderr, "extend_fw from %d (%d)\n", e->to2, diff);
#endif
	cost = extend_fw(seq2->seq + e->to2, he->seq + e->to1, diff,
			 min(4 * diff, (int) (he->len - e->to1)),
			 e->to2, e->to1, &I, &J, he->W);
#ifdef DEBUG
	fprintf(stderr, "extend_fw returned %d, I: %d J: %d\n", cost, I, J);
#endif
	if ((I - (int) e->to2) * options.matchScore
	    + cost * options.mismatchScore >= 0) {
	  e->to2 = I;
	  e->to1 = J;
	}
      }
    }
    /* Proceed in case of several exons.  */
    if (eCol->nb > 1) {
      unsigned int i;
      for (i = 1; i < eCol->nb; i++) {
	exon_p_t cur = eCol->e.exon[i - 1];
	exon_p_t next = eCol->e.exon[i];
	int diff = next->from2 - cur->to2 - 1;
	if (diff > 0) {
	  /* bridge the gap (provided there is one...)  */
	  if (next->from1 - 1 > cur->to1) {
	    hash_env_t tem_he;
	    if (diff <= MAX_GRINIT) {
	      int cost;
#ifdef DEBUG
	      fprintf(stderr, "Trying greedy %d %d\n",
		      cur->to2, next->from2);
#endif
	      cost = greedy(seq2->seq + cur->to2, he->seq + cur->to1, diff,
			    next->from1 - cur->to1 - 1,
			    cur->to2, cur->to1, he->W, &tem_eCol);
	      if (tem_eCol.nb > 0
		  && cost <= max(he->W, P * diff + 1)) {
		grow_exon_left(tem_eCol.e.exon[0], he->seq, seq2->seq);
		grow_exon_right(tem_eCol.e.exon[tem_eCol.nb - 1],
				he->seq, he->len, seq2->seq, seq2->len);
		merge(eCol, &tem_eCol, i, he->W);
		tem_eCol.nb = 0;
		i -= 1;
		continue;
	      }
	    }
#ifdef DEBUG
	    fprintf(stderr, "Find new exons %d %d\n",
		    cur->to1, next->from1);
#endif
	    init_hash_env(&tem_he, min(8, he->W), he->seq + cur->to1,
			  next->from1 - cur->to1 - 1);
	    bld_table(&tem_he);
	    exon_cores(&tem_he, seq2->seq + cur->to2, diff, cur->to1 + 1,
		       cur->to2 + 1, options.C, &mCol, NULL, &tem_eCol);
	    free_hash_env(&tem_he);
	    if (tem_eCol.nb > 0) {
	      grow_exon_left(tem_eCol.e.exon[0], he->seq, seq2->seq);
	      grow_exon_right(tem_eCol.e.exon[tem_eCol.nb - 1],
			      he->seq, he->len, seq2->seq, seq2->len);
	      merge(eCol, &tem_eCol, i, he->W);
	      tem_eCol.nb = 0;
	      i -= 1;
	    }
	  }
	}
      }
    }
    /* Re-check for polyA.  */
    kill_polyA(r, he->seq, seq2->seq);

    /* just printing ... */
#ifdef DEBUG
    debug_print_exons(eCol, "EXTENSIONS", he->seq, seq2->seq);
#endif

    /* compaction step; note: it resets the right end of the list to   */
    /* the last item in the block list                                 */
    compact_exons(eCol, he->W);

    /* just printing ... */
#ifdef DEBUG
    debug_print_exons(eCol, "NORMALIZATION", he->seq, seq2->seq);
#endif

    /* eliminate marginal small blocks at the start of the sequence;   */
    if (eCol->nb > 0) {
      unsigned int i = 0;
      while (i < eCol->nb) {
	exon_p_t e = eCol->e.exon[i];
	if (e->to2 - e->from2 + 1 >= he->W)
	  break;
	free(e);
	i += 1;
      }
      if (i > 0) {
	memmove(eCol->e.elt, eCol->e.elt + i,
		(eCol->nb - i) * sizeof(void *));
	eCol->nb -= i;
      }
    }

    /* eliminate marginal small blocks at the end of the sequence      */
    if (eCol->nb > 0) {
      int i = eCol->nb - 1;
      while (i >= 0) {
	exon_p_t e = eCol->e.exon[i];
	if (e->to2 - e->from2 + 1 >= he->W)
	  break;
	free(e);
	i -= 1;
	eCol->nb -= 1;
      }
    }

    /* Slide exon boundaries for optimal intron signals */
    slide_intron(r, he->seq, seq2->seq);

    /*  */
    align_status = pluri_align(he->seq, seq2->seq, &(st->nmatches), eCol,
			       &r->sList, he->len, seq2->len);
    if (align_status != 0 || !options.ali_flag) {
      free_align(r->sList);
      r->sList = NULL;
    }
  }
  free(mCol.e.elt);
  free(tem_eCol.e.elt);
}

void
init_col(collec_p_t c, unsigned int size)
{
  c->size = size;
  c->nb = 0;
  if (size > 0)
    c->e.elt = (void **) xmalloc(size * sizeof(void *));
  else
    c->e.elt = NULL;
}

static void
add_col_elt(collec_p_t c, void *elt)
{
  if (c->size <= c->nb) {
    c->size += 5;
    c->e.elt = (void **) xrealloc(c->e.elt, c->size * sizeof(void *));
  }
  c->e.elt[c->nb++] = elt;
}

#ifdef DEBUG
static void
debug_msps(hash_env_p_t he, uchar *s2, collec_p_t mCol, char *title)
{
  unsigned int j;
  fputs(title, stderr);
  for (j = 0; j < mCol->nb; ++j) {
    exon_p_t m = mCol->e.exon[j];
    fprintf(stderr, "[%d] %d-%d %d-%d, %d %d\n", j,
	    m->from1, m->to1, m->from2, m->to2, m->score, m->Score);
  }
  if (he == NULL)
    return;
  for (j = 0; j < mCol->nb; ++j) {
    exon_p_t m = mCol->e.exon[j];
    fprintf(stderr, "%.10s %.*s %.10s\n%.10s %.*s %.10s\n",
	    (m->from1 >= 10)
	      ? he->seq + m->from1 - 10
	      : he->seq,
	    m->to1 - m->from1 + 1, he->seq + m->from1,
	    he->seq + m->to1 + 1,
	    (m->from2 >= 10)
	      ? s2 + m->from2 - 10
	      : s2,
	    m->to2 - m->from2 + 1, s2 + m->from2,
	    s2 + m->to2 + 1);
  }
}

static void
debug_organized_msps(collec_p_t mCol, int last_msp, char *title)
{
  int i;
  fputs(title, stderr);
  for (i = last_msp; i >= 0; i = mCol->e.exon[i]->prev) {
    exon_p_t m = mCol->e.exon[i];
    fprintf(stderr, "[%d] %d-%d %d-%d, %d %d\n", i,
	    m->from1, m->to1, m->from2, m->to2, m->score, m->Score);
  }
}
#endif

static void
exon_cores(hash_env_p_t he, uchar *s2, unsigned int len2,
	   int offset1, int offset2, int K,
	   collec_p_t mCol, collec_p_t res, collec_p_t eCol)
{
  unsigned int j;
  int last_msp;
  int swapped = eCol != NULL; /* True when sequences were swapped.  */

  search(he, s2, len2, K, mCol);
#ifdef DEBUG
  debug_msps(he, s2, mCol, "==== unsorted MSPs\n");
#endif
  /* Kill small repeated segments.  */
  qsort(mCol->e.exon, mCol->nb, sizeof(exon_p_t), msp_rna_compare);
  trim_small_repeated_msps(mCol);
#ifdef DEBUG
  debug_msps(he, s2, mCol, "==== sorted MSPs\n");
#endif
  /* sort in order of mp->pos1.  */
  qsort(mCol->e.exon, mCol->nb, sizeof(exon_p_t), msp_compare);
  combine_msps(mCol);
#ifdef DEBUG
  debug_msps(NULL, NULL, mCol, "==== sorted, combined MSPs\n");
#endif
  /* Check for duplicated genes if requested.  */
  if (eCol == NULL) {
    result_p_t r;
    unsigned int minMPos = len2;
    unsigned int maxMPos = 0;
    unsigned int cov;
    unsigned int globScore;
    int tested = 0;
    assert(res != NULL);
    for (j = 0; j < mCol->nb; j++) {
      exon_p_t m = mCol->e.exon[j];
      if (m->from2 < minMPos)
	minMPos = m->from2;
      if (m->to2 > maxMPos)
	maxMPos = m->to2;
    }
    cov = maxMPos - minMPos + 1;
    cov = cov / 4;
    minMPos += cov;
    if (maxMPos > cov)
      maxMPos -= cov;
    for (j = 0; j < mCol->nb; j++) {
      exon_p_t m = mCol->e.exon[j];
      m->bot = m->from2 < minMPos;
      m->top = m->to2 > maxMPos;
    }
#ifdef DEBUG
    fprintf(stderr, "==== top, max: %d\n", maxMPos);
    for (j = 0; j < mCol->nb; ++j) {
      exon_p_t m = mCol->e.exon[j];
      fprintf(stderr, "[%d] %d-%d %d-%d, %d %d, %d\n", j,
	      m->from1, m->to1, m->from2, m->to2,
	      m->score, m->Score, m->top);
    }
#endif
    last_msp = link_msps(mCol, 0, mCol->nb);
    if (last_msp < 0)
      return;
    globScore = mCol->e.exon[last_msp]->Score;
    /* When filtering above 50%, check that both pieces have good scores.
       When filtering below 50%, check that one of the pieces has 75%.  */
    if (options.filterPct >= 50) {
      globScore = globScore * options.filterPct / 100;
    } else {
      globScore -= globScore / 4;
    }
    /* See if we have split points, and if the parts have good scores.  */
    minMPos = 0;
    maxMPos = 0;
    for (j = 1; j < mCol->nb; j++) {
      exon_p_t p = mCol->e.exon[j - 1];
      exon_p_t m = mCol->e.exon[j];
      if ((p->top && !m->top)
	  || (!p->bot && m->bot)
	  || (p->top && m->bot)) {
	/* We have a split.  */
	int lLast;
	unsigned int lScore, rScore;
	tested = 1;
	lLast = link_msps(mCol, minMPos, j);
	assert(lLast >= 0);
	lScore = mCol->e.exon[lLast]->Score;
	last_msp = link_msps(mCol, j, mCol->nb);
	assert(last_msp >= 0);
	rScore = mCol->e.exon[last_msp]->Score;
#ifdef DEBUG
	fprintf(stderr,
		"glob: %d, l: %d, r: %d, minP: %d, maxP: %d, j: %d\n",
		globScore, lScore, rScore, minMPos, maxMPos, j);
#endif
	if ((options.filterPct >= 50
	     && lScore >= globScore
	     && rScore >= globScore)
	    || (options.filterPct < 50
	        && (lScore >= globScore || rScore >= globScore))) {
	  unsigned int k;
	  /* Good split.  Store it for processing.  */
	  add_col_elt(res, xcalloc(1, sizeof(result_t)));
	  r = res->e.result[res->nb - 1];
	  r->dStart = maxMPos;
	  r->dLen = m->from1 - maxMPos;
	  eCol = &r->eCol;
#ifdef DEBUG
	  fprintf(stderr, "dStart: %u, dLen: %u\n", r->dStart, r->dLen);
	  debug_organized_msps(mCol, lLast, "==== organized MSPs (part)\n");
#endif
	  init_col(eCol, j - minMPos);
	  msp2exons(mCol->e.exon, lLast, eCol, 0);
	  for (k = 0; k < eCol->nb; k++) {
	    exon_p_t e = eCol->e.exon[k];
	    e->to1 += offset1;
	    e->from1 += offset1;
	    e->to2 += offset2;
	    e->from2 += offset2;
	  }
	  minMPos = j;
	  maxMPos = mCol->e.exon[lLast]->to1;
	  tested = 0;
	}
      }
    }
    if (tested)
      last_msp = link_msps(mCol, minMPos, mCol->nb);
    add_col_elt(res, xcalloc(1, sizeof(result_t)));
    r = res->e.result[res->nb - 1];
    r->dStart = maxMPos;
    r->dLen = he->len - maxMPos;
#ifdef DEBUG
    fprintf(stderr, "dStart: %u, dLen: %u\n", r->dStart, r->dLen);
#endif
    eCol = &r->eCol;
  } else
    last_msp = link_msps(mCol, 0, mCol->nb);
  /* organize Blast hits (MSPs) into exons */
#ifdef DEBUG
  debug_organized_msps(mCol, last_msp, "==== organized MSPs\n");
#endif
  if (eCol->size == 0)
    init_col(eCol, mCol->nb);
  msp2exons(mCol->e.exon, last_msp, eCol, swapped);
  for (j = 0; j < eCol->nb; j++) {
    exon_p_t e = eCol->e.exon[j];
    e->to1 += offset1;
    e->from1 += offset1;
    e->to2 += offset2;
    e->from2 += offset2;
  }
  mCol->nb = 0;
}

static inline int
lies_after_p(exon_p_t a, exon_p_t b)
{
  /* When we have some overlap, make sure it is only a small part.  */
  /* ------------------
               ---------------------
     |   p1    |  p2  |     p3     |  */

  if (b->from1 > a->to1) {
    unsigned int p1;
    unsigned int p2;
    unsigned int p3;
    if (b->from2 > a->to2)
      return 1;
    if (b->from2 < a->from2 || b->to2 < a->to2)
      return 0;
    p1 = b->from2 - a->from2;
    p2 = a->to2 - b->from2;
    p3 = b->to2 - a->to2;
    if (p1 > p2 && p3 > p2 && p1 > options.K && p3 > options.K)
      return 1;
  } else if  (b->from2 > a->to2) {
    unsigned int p1;
    unsigned int p2;
    unsigned int p3;
    if (b->from1 < a->from1 || b->to1 < a->to1)
      return 0;
    p1 = b->from1 - a->from1;
    p2 = a->to1 - b->from1;
    p3 = b->to1 - a->to1;
    if (p1 > p2 && p3 > p2 && p1 > options.K && p3 > options.K)
      return 1;
  }
  return 0;
}

#define SMALL_EXON 50
#define MIN_REPEAT 20
#define JITTER_FACTOR 5

static void
trim_small_repeated_msps(collec_p_t mCol)
{
  unsigned int i = 0;
  while (i < mCol->nb) {
    exon_p_t m = mCol->e.exon[i];
    unsigned int j, k, end;
    if (m->to2 - m->from2 >= SMALL_EXON) {
      i += 1;
      continue;
    }
    end = m->to2 + JITTER_FACTOR;
    j = i + 1;
    while (j < mCol->nb && mCol->e.exon[j]->to2 <= end)
      j += 1;
    if (j - i < MIN_REPEAT) {
      i += 1;
      continue;
    }
    for (k = i; k < j; k++)
      free(mCol->e.exon[k]);
    memmove(mCol->e.exon + i, mCol->e.exon + j,
	    (mCol->nb - j) * sizeof(exon_p_t));
    mCol->nb -= (j - i);
  }
}

static void
combine_msps(collec_p_t mCol)
{
  unsigned int i = 0;
  while (i < mCol->nb) {
    exon_p_t m = mCol->e.exon[i];
    unsigned int ovl = 0;
    unsigned int j;
    for (j = i + 1; j < mCol->nb; j++) {
      exon_p_t n = mCol->e.exon[j];
      unsigned int o = 0;
      if (n->from2 <= m->to2 + 1)
	ovl = m->to2 - n->from2 + 2;
      if (n->from1 > m->from1
	  && n->from1 <= m->to1 + 1)
	o = m->to1 - n->from1 + 2;
      if ((ovl == 0) == (o == 0)
	  && abs((int) ovl - (int) o) <= 10)
	break;
      ovl = 0;
    }
    if (ovl != 0) {
      exon_p_t n = mCol->e.exon[j];
      unsigned int nScore = m->score + n->score;
      if (nScore >= ovl + 1)
	nScore -= ovl + 1;
      else
	nScore = 0;
      m->from1 = min(m->from1, n->from1);
      m->from2 = min(m->from2, n->from2);
      m->to1 = max(m->to1, n->to1);
      m->to2 = max(m->to2, n->to2);
      if (nScore > m->score)
	m->score = nScore;
      mCol->nb -= 1;
      free(n);
      memmove(mCol->e.exon + j, mCol->e.exon + j + 1,
	      (mCol->nb - j) * sizeof(exon_p_t));
    } else
      i += 1;
  }
}

static int
link_msps(collec_p_t mCol, unsigned int start, unsigned int stop)
{
  struct {
    unsigned int elt;
    unsigned int score;
  } best;
  unsigned int i;
  if (start >= stop)
    return -1;
  memset(&best, 0, sizeof(best));
  for (i = start; i < stop; i++) {
    exon_p_t m = mCol->e.exon[i];
    m->Score = 0;
    m->prev = -1;
  }
  for (i = start; i < stop; i++) {
    exon_p_t m = mCol->e.exon[i];
    unsigned int j;
    m->Score += m->score;
    if (m->Score > best.score) {
      best.score = m->Score;
      best.elt = i;
    }
    for (j = i + 1; j < stop; j++) {
      exon_p_t n = mCol->e.exon[j];
      if (lies_after_p(m, n) && m->Score >= n->Score) {
	unsigned int penalty;
	penalty = abs(n->from1 - m->from1) >> 15;
	penalty += abs(n->from2 - m->from2) >> 15;
	if (penalty < m->Score) {
	  n->Score = m->Score - penalty;
	  n->prev = i;
	}
      }
    }
  }
  return best.elt;
}

void
init_encoding(void)
{
  unsigned int i;
  for (i = 0; i < NACHARS; i++)
    encoding[i] = 4;
  encoding['A'] = 0;
  encoding['C'] = 1;
  encoding['G'] = 2;
  encoding['T'] = 3;
}

void
init_hash_env(hash_env_p_t he, unsigned int W, uchar *seq, unsigned int len)
{
  he->W = W;
  he->seq = seq;
  he->len = len;
  he->mask = (1 << (W + W - 2)) - 1;
  he->next_pos = (int *) xmalloc((len + 1) * sizeof(int));
  he->hashtab = (void **)
    xcalloc(HASH_SIZE, sizeof(void *));
}

#ifndef __GLIBC__
void
tdestroy(void *VROOT, void(*FREEFCT)(void *))
{
}
#endif

void
free_hash_env(hash_env_p_t he)
{
  unsigned int hval;
  free(he->next_pos);
  for (hval = 0; hval < HASH_SIZE; hval++) {
    tdestroy(he->hashtab[hval], free);
  }
  free(he->hashtab);
}

static int
hash_node_compare(const void *a, const void *b)
{
  const hash_node_p_t ha = (hash_node_p_t) a, hb = (hash_node_p_t)b;
  if (ha->ecode < hb->ecode)
    return -1;
  if (ha->ecode > hb->ecode)
    return 1;
  return 0;
}

/* add_word - add a word to the table of critical words */
static inline void
add_word(hash_env_p_t he, unsigned int ecode, unsigned int pos)
{
  hash_node_p_t h = (hash_node_p_t) xmalloc(sizeof(hash_node_t));
  hash_node_p_t *key;

  h->ecode = ecode;
  key = tsearch(h, he->hashtab + (ecode & HASH_MASK), hash_node_compare);
  assert(key != NULL);
  if (*key != h) {
    free(h);
    he->next_pos[pos] = (*key)->pos;
  } else {
    he->next_pos[pos] = -1;
  }
  (*key)->pos = pos;
}

/* -----------   build table of W-tuples in one of the sequences  ------------*/
void
bld_table(hash_env_p_t he)
{
  unsigned int ecode;
  unsigned int i = 0;
  uchar *t;
  /* skip any word containing an N/X  */
  t = he->seq;
  while (i < he->len) {
    unsigned int j;
  restart:
    ecode = 0;
    for (j = 1; j < he->W && i < he->len; j++) {
      unsigned int tmp = encoding[*t++];
      i += 1;
      if (tmp > 3) goto restart;
      ecode = (ecode << 2) + tmp;
    }

    while (i < he->len) {
      unsigned int tmp = encoding[*t++];
      i += 1;
      if (tmp > 3) goto restart;
      ecode = ((ecode & he->mask) << 2) + tmp;
      add_word(he, ecode, i);
    }
  }
}

/* -----------------------   search the other sequence   ---------------------*/

static void
search(hash_env_p_t he, uchar *s2, unsigned int len2, int K,
       collec_p_t mCol)
{
  uchar *t;
  unsigned int i = 0;
  int *allocated = xcalloc(he->len + len2 + 1, sizeof(int));
  int *diag_lev = allocated + he->len;
  t = s2;
  while (i < len2) {
    unsigned int j;
    hash_node_t hn;
  restart:
    hn.ecode = 0;
    for (j = 1; j < he->W && i < len2; j++) {
      unsigned int tmp = encoding[*t++];
      i += 1;
      if (tmp > 3) goto restart;
      hn.ecode = (hn.ecode << 2) + tmp;
    }
    while (i < len2) {
      unsigned int tmp = encoding[*t++];
      hash_node_p_t *key;
      i += 1;
      if (tmp > 3) goto restart;
      hn.ecode = ((hn.ecode & he->mask) << 2) + tmp;
      key = tfind(&hn, he->hashtab + (hn.ecode & HASH_MASK),
		  hash_node_compare);
      if (key != NULL) {
	int p;
	for (p = (*key)->pos; p >= 0; p = he->next_pos[p])
	  extend_hit(p, i, he, s2, len2, K, mCol, diag_lev);
      }
    }
  }
  free(allocated);
}

/* extend_hit - extend a word-sized hit to a longer match */
static void
extend_hit(int pos1, int pos2, hash_env_p_t he, const uchar * const s2,
	   unsigned int len2, int K, collec_p_t mCol, int *diag_lev)
{
  const uchar *beg2, *beg1, *end1, *q, *s;
  int right_sum, left_sum, sum, diag, score;

  diag = pos2 - pos1;
  if (diag_lev[diag] > pos1)
    return;

  /* extend to the right */
  left_sum = sum = 0;
  q = he->seq + pos1;
  s = s2 + pos2;
  end1 = q;
  while (s < s2 + len2
	 && q < he->seq + he->len
	 && sum >= left_sum - options.X) {
    sum += ((*s++ == *q++)
	    ? options.matchScore
	    : options.mismatchScore);
    if (sum > left_sum) {
      left_sum = sum;
      end1 = q;
    }
  }

  /* extend to the left */
  right_sum = sum = 0;
  beg1 = q = (he->seq + pos1) - he->W;
  beg2 = s = (s2 + pos2) - he->W;
  while ((s > s2) && (q > he->seq) && sum >= right_sum - options.X) {
    sum += ((*(--s) == *(--q))
	    ? options.matchScore
	    : options.mismatchScore);
    if (sum > right_sum) {
      right_sum = sum;
      beg2 = s;
      beg1 = q;
    }
  }

  score = he->W + left_sum + right_sum;
  if (score >= K) {
    add_col_elt(mCol,
		new_exon(beg1 - he->seq, beg2 - s2,
			 end1 - he->seq - 1, beg2 - s2 + end1 - beg1 - 1));
    mCol->e.exon[mCol->nb - 1]->score = score;
  }
  diag_lev[diag] = (end1 - he->seq) + he->W;
}

/*  ----------------------------   sort the MSPs  ----------------------------*/

/* msp_compare - determine ordering relationship between two MSPs */
static int
msp_compare(const void *a, const void *b)
{
  exon_p_t ki = * (exon_p_t *) a, kj = * (exon_p_t *) b;

  if (ki->from1 > kj->from1)
    return 1;
  if (ki->from1 < kj->from1)
    return -1;
  if (ki->from2 > kj->from2)
    return 1;
  if (ki->from2 < kj->from2)
    return -1;
  return 0;
}

/* msp_rna_compare - determine RNA ordering relationship between two MSPs */
static int
msp_rna_compare(const void *a, const void *b)
{
  exon_p_t ki = * (exon_p_t *) a, kj = * (exon_p_t *) b;

  if (ki->from2 > kj->from2)
    return 1;
  if (ki->from2 < kj->from2)
    return -1;
  if (ki->to2 > kj->to2)
    return -1;
  if (ki->to2 < kj->to2)
    return 1;
  return 0;
}

/* ---------------------  organize the MSPs into exons  ---------------------*/

static void
msp2exons(exon_p_t *msp, int last_msp, collec_p_t eCol, int swapped)
{
  while (last_msp >= 0) {
    exon_p_t mp = msp[last_msp];
    if (eCol->nb > 0) {
      /* See if we merge with next exon (we go in reverse).  */
      exon_p_t next = eCol->e.exon[eCol->nb - 1];
      if (!swapped
	  && next->from1 < mp->to1 + MIN_INTRON
	  && next->from2 > mp->to2 - 1) {
	next->to1 = max(next->to1, mp->to1);
	next->to2 = max(next->to2, mp->to2);
	next->from1 = min(next->from1, mp->from1);
	next->from2 = min(next->from2, mp->from2);
	last_msp = mp->prev;
	free(mp);
	continue;
      }
    }
    add_col_elt(eCol, mp);
    last_msp = mp->prev;
  }
  /* Now, need to reverse the exons...  */
  if (eCol->nb > 1) {
    unsigned int i, j;
    for (i = 0, j = eCol->nb - 1; j > i; i++, j--) {
      exon_p_t e = eCol->e.exon[i];
      eCol->e.exon[i] = eCol->e.exon[j];
      eCol->e.exon[j] = e;
    }
  }
}

/* ----------------------  print endpoints of exons  --------------------*/

void
print_exons(collec_p_t eCol, int direction)
{
  unsigned int i;
  unsigned int last = eCol->nb - 1;
  exon_p_t cur;
  assert(eCol->nb > 0);
  for (i = 0; i < last; i++) {
    cur = eCol->e.exon[i];
    if (direction == 0 || cur->type < 0)
      printf("%u-%u  (%u-%u)   %u%% ==\n",
	     cur->from1 + options.dnaOffset, cur->to1 + options.dnaOffset,
	     cur->from2, cur->to2, cur->score);
    else
      printf("%u-%u  (%u-%u)   %u%% %s (%.2s/%.2s) %u\n",
	     cur->from1 + options.dnaOffset, cur->to1 + options.dnaOffset,
	     cur->from2, cur->to2, cur->score,
	     direction > 0 ? "->" : "<-",
	     options.splice[cur->type].fwd,
	     options.splice[cur->type].fwd + 2,
	     cur->splScore);
  }
  cur = eCol->e.exon[last];
  printf("%u-%u  (%u-%u)   %u%%\n",
	 cur->from1 + options.dnaOffset, cur->to1 + options.dnaOffset,
	 cur->from2, cur->to2, cur->score);
}

static int
pluri_align(uchar *seq1, uchar *seq2, unsigned int *num_matches,
	    collec_p_t eCol, edit_script_list_p_t *Aligns, int M, int N)
{
  exon_t eFake;
  exon_p_t cur = &eFake;
  int diff, end1, end2, ali_dist;
  unsigned int nmatches = 0;
  edit_script_p_t head;
  int ii;

  head = NULL;
  *Aligns = NULL;
  ali_dist = 0;
  end1 = M;
  end2 = N;
  eFake.from1 = M + 1;
  eFake.from2 = N + 1;
  eFake.to1 = 0;
  eFake.to2 = 0;

  for (ii = eCol->nb - 1; ii >= 0; ii--) {
    exon_p_t prev = eCol->e.exon[ii];
    edit_script_p_t left, right, prevE, tmp_script;
    uchar *a, *b;
    int tmpi, di_count, alen;
    if ((diff = cur->from2 - prev->to2 - 1) != 0) {
      if (cur->to1) {
	edit_script_list_p_t
	  enew = (edit_script_list_p_t) xmalloc(sizeof(edit_script_list_t));
	enew->next_script = *Aligns;
	*Aligns = enew;
	enew->script = head;
	enew->offset1 = cur->from1;
	enew->offset2 = cur->from2;
	enew->len1 = end1 - enew->offset1 + 1;
	enew->len2 = end2 - enew->offset2 + 1;
	enew->score = ali_dist;
	ali_dist = 0;
	head = NULL;
      }
      end1 = prev->to1;
      end2 = prev->to2;
    } else if ((diff = cur->from1 - prev->to1 - 1) != 0
	       && cur->to1) {
      edit_script_p_t new = (edit_script_p_t) xmalloc(sizeof(edit_script_t));
      new->op_type = DELETE;
      new->num = diff;
      new->next = head;
      head = new;
    } else if (diff)
      end1 = prev->to1;

    diff = align_get_dist(seq1, seq2, prev->from1 - 1, prev->from2 - 1,
			  prev->to1, prev->to2,
			  max(1000, .2 * (prev->to2 - prev->from2 + 1)));

    if (diff < 0)
      return -1;

    align_path(seq1, seq2, prev->from1 - 1, prev->from2 - 1,
	       prev->to1, prev->to2, diff, &left, &right, M, N);
    if (right == NULL)
      return -1;
    Condense_both_Ends(&left, &right, &prevE);

    if (!cur->to1 && right->op_type == DELETE) {
      /* remove gaps at end of alignment */
      diff -= 0 + right->num;         /* subtract GAP_OPEN = 0 */
      prev->to1 -= right->num;
      end1 -= right->num;
      if (head && (head->op_type == DELETE))
	head->num += right->num;
      free(right);
      prevE->next = NULL;
      right = prevE;
    }
    if (ii == 0 && left && (left->op_type == DELETE)) {
      diff -= 0 + left->num;          /* subtract GAP_OPEN = 0 */
      prev->from1 += left->num;
      tmp_script = left->next;
      if (right == left)
	right = tmp_script;
      free(left);
      left = tmp_script;
    }

    ali_dist += diff;

    a = seq1 + prev->from1 - 1;
    b = seq2 + prev->from2 - 1;
    tmpi = di_count = 0;
    tmp_script = left;
    while (tmp_script) {
      switch (tmp_script->op_type) {
      case  DELETE:
	di_count += tmp_script->num;
	tmpi += tmp_script->num;
	a += tmp_script->num;
	break;
      case  INSERT:
	di_count += tmp_script->num;
	tmpi += tmp_script->num;
	b += tmp_script->num;
	break;
      case  SUBSTITUTE:
	{
	  int j;
	  for (j = 0; j < tmp_script->num; ++j, ++a, ++b)
	    if (*a != *b)
	      tmpi++;
	    else
	      nmatches++;
	  break;
	}
      }
      tmp_script = tmp_script->next;
    }
    alen = (int) ((prev->to1 - prev->from1 + 1
		   + prev->to2 - prev->from2 + 1 + di_count)
		  / (double) 2);
    prev->score = ((alen - tmpi) * 100) / alen;
    right->next = head;
    head = left;
    cur = prev;
  }

  /* at the beginning of the sequences */
  if ((diff = cur->from2 - 1) != 0 && diff != N) {
    edit_script_list_p_t
      enew = (edit_script_list_p_t) xmalloc(sizeof(edit_script_list_t));
    enew->next_script = *Aligns;
    *Aligns = enew;
    enew->offset1 = cur->from1;
    enew->offset2 = cur->from2;
    enew->len1 = end1 - enew->offset1 + 1;
    enew->len2 = end2 - enew->offset2 + 1;
    enew->script = head;
    enew->score = ali_dist;
  } else if (diff != N) {
    /* modified to cut introns at the beginning of the sequence */
    edit_script_list_p_t
      enew = (edit_script_list_p_t) xmalloc(sizeof(edit_script_list_t));
    enew->next_script = *Aligns;
    *Aligns = enew;
    enew->offset1 = cur->from1;
    enew->offset2 = 1;
    enew->len1 = end1 - enew->offset1 + 1;
    enew->len2 = end2 - enew->offset2 + 1;
    enew->script = head;
    enew->score = ali_dist;
  }
  *num_matches = nmatches;
  return 0;
}

static exon_p_t
new_exon(unsigned int f1, unsigned int f2, unsigned int t1, unsigned int t2)
{
  exon_p_t e = (exon_p_t) xmalloc(sizeof(exon_t));
  e->from1 = f1;
  e->from2 = f2;
  e->to1 = t1;
  e->to2 = t2;
  return e;
}

/* FIXME: why are s1 and s2 reversed here, wrt SIM4 ???  */
static int
greedy(uchar *s1, uchar *s2, unsigned int m, unsigned int n,
       unsigned int offset1, unsigned int offset2, unsigned int W,
       collec_p_t eCol)
{
  int col,                 /* column number */
      k,                   /* current diagonal */
      blower,flower,       /* boundaries for searching diagonals */
      bupper,fupper,
      row,                 /* row number */
      DELTA,               /* n-m  */
      B_ORIGIN, F_ORIGIN;
  unsigned int d,	   /* current distance */
      max_d,               /* bound on size of edit script */
      Cost,
      MAX_D,
      i;
  int back, forth;         /* backward and forward limits at exit */
  int *blast_d, *flast_d,  /* rows containing the last d (at crt step, d-1) */
      *btemp_d, *ftemp_d;  /* rows containing tmp values for the last d */
  int *min_row, *min_diag, /* min (b)/ max (f) row (and diagonal) */
      *max_row, *max_diag; /* reached for cost d=0, ... m.  */

  /* No point trying to span megabase-sized holes...  */
  if (n >= 1000000)
    return 0;
  DELTA = n - m;
/*max_d = MAX_D = m+1; */
  max_d = MAX_D = max(W, (unsigned int) (P * m + 1));

  if (DELTA < 0) {
    if (m <= min(W, (1 + P) * n)) {
      add_col_elt(eCol,
		  new_exon(offset2 + 1, offset1 + 1,
			   offset2 + n, offset1 + m));
      return m - n + (unsigned int) (P * n + 1);
    } else {
      return max(W, (unsigned int) (P * m + 1)) + 1;
    }
  }

  F_ORIGIN = MAX_D;
  B_ORIGIN = MAX_D - DELTA;
  for (row = m, col = n;
       row > 0 && col > 0 && (s1[row - 1] == s2[col - 1]);
       row--,col--)
    /*LINTED empty loop body*/;

  if (row == 0) {
    /* hit last row; stop search */
    add_col_elt(eCol,
		new_exon(offset2 - m + n + 1, offset1 + 1,
			 offset2 + n, offset1 + m));
    return 0;
  }

  blast_d = (int *) xmalloc((MAX_D + n + 1) * sizeof(int));
  btemp_d = (int *) xmalloc((MAX_D + n + 1) * sizeof(int));

  for (i = 0; i <= MAX_D + n; ++i) {
    blast_d[i] = m + 1;
    btemp_d[i] = m + 1;
  }
  blast_d[B_ORIGIN + DELTA] = row;

  blower = B_ORIGIN + DELTA - 1;
  bupper = B_ORIGIN + DELTA + 1;

  for (row = 0;
       (unsigned int) row < n
       && (unsigned int) row < m
       && (s1[row] == s2[row]);
       row++)
    /*LINTED empty loop body*/;

  if ((unsigned int) row == m) {
    /* hit last row; stop search */
    add_col_elt(eCol,
		new_exon(offset2 + 1, offset1 + 1,
			 offset2 + m, offset1 + m));
    free(blast_d);
    free(btemp_d);
    return 0;
  }

  flast_d = (int *) xmalloc((MAX_D + n + 1) * sizeof(int));
  ftemp_d = (int *) xmalloc((MAX_D + n + 1) * sizeof(int));

  for (i = 0; i <= MAX_D + n; ++i) {
    flast_d[i] = -1;
    ftemp_d[i] = -1;
  }
  flast_d[F_ORIGIN] = row;

  flower = F_ORIGIN - 1;
  fupper = F_ORIGIN + 1;

  max_row = (int *) xmalloc((MAX_D + 1) * sizeof(int));
  min_row = (int *) xmalloc((MAX_D + 1) * sizeof(int));
  max_diag = (int *) xmalloc((MAX_D + 1) * sizeof(int));
  min_diag = (int *) xmalloc((MAX_D + 1) * sizeof(int));

  for (d = 1; d <= MAX_D; d++) {
    min_row[d] = m + 1;
    max_row[d] = -1;
  }
  min_row[0] = blast_d[B_ORIGIN + DELTA];
  min_diag[0] = B_ORIGIN + DELTA;
  max_row[0] = flast_d[F_ORIGIN];
  max_diag[0] = F_ORIGIN;

  back = forth = -1;

  d = 1;
  while (d <= max_d) {

    /* for each relevant diagonal ... */
    for (k = blower; k <= bupper; k++) {
      /* process the next edit instruction */

      /* find a d on diagonal k */
      if (k == -((int) d) + DELTA + B_ORIGIN) {

	/* move left from the last d-1 on diagonal k+1 */
	row = blast_d[k + 1];   /* INSERT */
      } else if (k == (int) d + DELTA + B_ORIGIN) {

	/* move up from the last d-1 on diagonal k-1 */
	row = blast_d[k - 1] - 1;  /* DELETE */
      } else if ((blast_d[k] <= blast_d[k + 1])
		 && (blast_d[k] - 1 <= blast_d[k - 1])) {

	/* substitution */
	row = blast_d[k] - 1;  /* SUBSTITUTE */

      } else if ((blast_d[k - 1] <= blast_d[k + 1] - 1)
		 && (blast_d[k - 1] <= blast_d[k] - 1)) {
	/* move right from the last d-1 on diagonal k-1 */
	row = blast_d[k - 1] - 1;  /* DELETE */
      } else  {
	/* move left from the last d-1 on diagonal k+1 */
	row = blast_d[k + 1];    /* INSERT */
      }
      /* code common to the three cases */
      col = row + k - B_ORIGIN;

      /* slide up the diagonal */
      while (row > 0 && col > 0 && (s1[row - 1] == s2[col - 1])) {
	--row;
	--col;
      }
      btemp_d[k] = row;

/*           if (row == 0 || col == 0) max_d = d;   */

    }     /* for k */

    min_row[d] = btemp_d[DELTA + B_ORIGIN];
    min_diag[d] = DELTA + B_ORIGIN;
    for (k = blower; k <= bupper; ++k) {
      blast_d[k] = btemp_d[k];
      btemp_d[k] = m + 1;
      if (blast_d[k] < min_row[d]) {
	min_row[d] = blast_d[k];
	min_diag[d] = k;
      }
    }

    /* record cell, if paths overlap with minimum combined cost */
    /* obs: it suffices to search up to Cost=min(d-1,(max_d-d)) */
    for (Cost = 0; Cost < d; Cost++) {
      if ((min_row[d] <= max_row[Cost])
	  && ((max_d > d + Cost) || (max_d == d + Cost && (forth < 0)))) {
	max_d = d + Cost;
	back = d;
	forth = Cost;
	break;
      }
    }

    --blower; ++bupper;

    /* for each relevant diagonal ... */
    for (k = flower; k <= fupper; k++) {
      /* process the next edit instruction */

      /* find a d on diagonal k */
      if (k == -((int) d) + F_ORIGIN) {
	/* move down from the last d-1 on diagonal k+1 */
	row = flast_d[k + 1] + 1; /* DELETE */

      } else if (k == (int) d + F_ORIGIN) {
	/* move right from the last d-1 on diagonal k-1 */
	row = flast_d[k - 1]; /* INSERT */

      } else if ((flast_d[k] >= flast_d[k + 1])
		 && (flast_d[k] + 1 >= flast_d[k - 1])) {

	/* substitution */
	row = flast_d[k] + 1;   /* SUBSTITUTE */

      } else if ((flast_d[k + 1] + 1 >= flast_d[k - 1])
		 && (flast_d[k + 1] >= flast_d[k])) {

	/* move left from the last d-1 on diagonal k+1 */
	row = flast_d[k + 1] + 1;  /* DELETE */
      } else {
	/* move right from the last d-1 on diagonal k-1 */
	row = flast_d[k - 1];    /* INSERT */
      }
      /* code common to the three cases */
      col = row + k - F_ORIGIN;

      /* slide down the diagonal */
      if (row >= 0)
	while ((unsigned int) row < m
	       && (unsigned int) col < n
	       && (s1[row] == s2[col])) {
	  ++row;
	  ++col;
	}
      ftemp_d[k] = row;

/*             if (row == m || col == n) max_d = d;  */
    }     /* for k */

    max_row[d] = ftemp_d[F_ORIGIN];
    max_diag[d] = F_ORIGIN;
    for (k = flower; k <= fupper; ++k) {
      flast_d[k] = ftemp_d[k];
      ftemp_d[k] = -1;
      if (flast_d[k] > max_row[d]) {
	max_row[d] = flast_d[k];
	max_diag[d] = k;
      }
    }

    /* record backward and forward limits, if minimum combined
     * cost in overlapping. Note: it suffices to search up to
     * Cost=min(d,(max_d-d)).
     */
    for (Cost = 0; Cost <= d; Cost++) {
      if ((min_row[Cost] <= max_row[d])
	  && ((max_d > d + Cost) || (max_d == d + Cost && (forth < 0)))) {
	max_d = d + Cost;
	back = Cost;
	forth = d;
	break;
      }
    }
    --flower; ++fupper;

    ++d;  /* for d */
  }

  if (d > MAX_D) {
    free(blast_d); free(btemp_d);
    free(flast_d); free(ftemp_d);
    free(min_row); free(min_diag);
    free(max_row); free(max_diag);

    return d;
  }
/*fin:*/
  {
    unsigned int p1, p2, q1, q2;
    if ((int) m - min_row[back] >= max_row[forth]) {
      p1 = min_row[back];
      p2 = min_row[back] + max_diag[forth] - F_ORIGIN;
      q1 = min_row[back];
      q2 = min_row[back] + min_diag[back] - B_ORIGIN;
    } else {
      p1 = max_row[forth];
      p2 = max_row[forth] + max_diag[forth] - F_ORIGIN;
      q1 = max_row[forth];
      q2 = max_row[forth] + min_diag[back] - B_ORIGIN;
    }
    assert(q1 > 0 || p1 < m);
    if (q1 > 0)
      add_col_elt(eCol,
		  new_exon(offset2 + 1, offset1 + 1,
		 	   offset2 + p2, offset1 + p1));
    if (p1 < m)
      add_col_elt(eCol,
		  new_exon(offset2 + q2 + 1, offset1 + q1 + 1,
			   offset2 + n, offset1 + m));
  }

  free(blast_d); free(btemp_d);
  free(flast_d); free(ftemp_d);
  free(min_row); free(min_diag);
  free(max_row); free(max_diag);

  return back + forth;
}

static int
about_same_gap_p(unsigned int to1, unsigned int nFrom1,
		 unsigned int to2, unsigned int nFrom2)
{
  unsigned int g1, g2, d;
  if (nFrom1 <= to1 || nFrom2 <= to2)
    return 0;
  g1 = nFrom1 - to1 - 1;
  g2 = nFrom2 - to2 - 1;
  if (g2 > g1) {
    unsigned int tem = g1;
    g1 = g2;
    g2 = tem;
  }
  d = g1 - g2;
  if ((d * 100) / g1 <= options.gapPct)
    return 1;
  return 0;
}

/* operates on a list sorted in increasing order of exon coordinates */
static void
compact_exons(collec_p_t eCol, unsigned int W)
{
  unsigned int i = 1;
  /* Kill stupid overlaping exons.  */
  while (i < eCol->nb) {
    exon_p_t cur = eCol->e.exon[i - 1];
    exon_p_t next = eCol->e.exon[i];
    unsigned int diff = next->from2 - cur->from2;
    if (diff <= options.intron_window) {
      eCol->nb -= 1;
      if (cur->to2 > next->to2) {
	free(next);
	memmove(eCol->e.exon + i, eCol->e.exon + i + 1,
		(eCol->nb - i) * sizeof(exon_p_t));
	if (i < eCol->nb) {
	  next = eCol->e.exon[i];
	  cur->to1 += diff;
	  cur->to2 += diff;
	  next->from1 -= diff;
	  next->from2 -= diff;
	}
      } else {
	free(cur);
	memmove(eCol->e.exon + i - 1, eCol->e.exon + i,
		(eCol->nb - i + 1) * sizeof(exon_p_t));
	if (i > 1) {
	  cur = eCol->e.exon[i - 2];
	  cur->to1 += diff;
	  cur->to2 += diff;
	  next->from1 -= diff;
	  next->from2 -= diff;
	}
      }
    } else
      i += 1;
  }
  for (i = 1; i < eCol->nb; i++) {
    exon_p_t cur = eCol->e.exon[i - 1];
    exon_p_t next = eCol->e.exon[i];
    if ((next->from1 < cur->to1 + 1 + MIN_INTRON
	 && next->from2 <= cur->to2 + 1 + W)
	|| about_same_gap_p(cur->to1, next->from1,
			    cur->to2, next->from2)) {
      /* merge blocks cur and next */
      cur->to1 = next->to1;
      cur->to2 = next->to2;
      free(next);
      eCol->nb -= 1;
      memmove(eCol->e.elt + i, eCol->e.elt + i + 1,
	      (eCol->nb - i) * sizeof(void *));
      i -= 1;
    }
  }
}

static int
good_ratio(int length, int W)
{
  if (length<=W/2) return 2;
  else if (length<2*W) return options.cutoff;
  else return (int)(.75*P*length+1);
}

static int
extend_bw(uchar *s1, uchar *s2, int m, int n, int offset1, int offset2,
	  int *line1, int *line2, int W)
{
  int     col,                    /* column number */
          row,                    /* row number */
          max_d,                  /* bound on the length of the edit script */
          d,                      /* current compressed distance */
          k,                      /* current diagonal */
          DELTA,                  /* n-m  */
          ORIGIN,
          lower,
          upper;
  int     *last_d, *temp_d;       /* column containing the last p */
  int     *min_row, *min_diag;    /* min (b)/ max (f) row (and diagonal) */
                                  /* reached for cost d=0, ... m.  */
  DELTA = n-m;
  max_d = m+1;

  ORIGIN = m;
  for (row=m, col=n; row>0 && col>0 && (s1[row-1]==s2[col-1]); row--,col--)
    /*LINTED empty loop body*/;

  if ((row == 0) || (col == 0)) {
    *line1 = row+offset1;
    *line2 = col+offset2;

    return 0;
  }

  last_d = (int *)xmalloc((m+n+1)*sizeof(int));
  temp_d = (int *)xmalloc((m+n+1)*sizeof(int));

  for (k=0; k<=m+n; ++k) last_d[k]=m+1;
  last_d[ORIGIN+DELTA] = row;

  lower = ORIGIN + DELTA - 1;
  upper = ORIGIN + DELTA + 1;

  min_row = (int *)xmalloc((m+1)*sizeof(int));
  min_diag = (int *)xmalloc((m+1)*sizeof(int));

  for (d=1; d<=m; d++)
    min_row[d] = m+1;

  min_row[0] = last_d[ORIGIN+DELTA];
  min_diag[0] = ORIGIN + DELTA;

  d = 0;
  while ((++d<=max_d) &&
         ((d-1<=good_ratio(m-min_row[d-1], W)) ||
          ((d>=2) && (d-2<=good_ratio(m-min_row[d-2], W))))) {

    /* for each relevant diagonal ... */
    for (k = lower; k <= upper; k++) {

      /* find a d on diagonal k */
      if (k==-d+DELTA+ORIGIN) {
	/* move down from the last d-1 on diagonal k+1 */
	row = last_d[k+1];
	/* op = INSERT; */

      } else if (k==d+DELTA+ORIGIN) {
	/* move right from the last d-1 on diagonal k-1 */
	row = last_d[k-1]-1;
	/* op = DELETE; */

      } else if ((last_d[k]-1<=last_d[k+1]) &&
		 (last_d[k]-1<=last_d[k-1]-1)) {
	/* substitution */
	row = last_d[k]-1;
	/* op = SUBSTITUTE; */

      } else if ((last_d[k-1]-1<=last_d[k+1]) &&
		 (last_d[k-1]-1<=last_d[k]-1)) {
	/* move right from the last d-1 on diagonal k-1 */
	row = last_d[k-1]-1;
	/* op = DELETE; */

      } else  {
	/* move left from the last d-1 on diagonal k+1 */
	row = last_d[k+1];
	/* op = INSERT; */

      }

      /* code common to the three cases */
      /* slide down the diagonal */

      col = row+k-ORIGIN;

      while ((row > 0) && (col > 0) && (s1[row-1]==s2[col-1])) {
	row--; col--;
      }

      temp_d[k] = row;

      if ((row == 0) && (col == 0)) {
	/* hit southeast corner; have the answer */

	free(last_d); free(temp_d);
	free(min_row); free(min_diag);

	*line1 = row+offset1;
	*line2 = col+offset2;

	return d;
      }
      if (row == 0) {
	/* hit first row; don't look further */

	free(last_d); free(temp_d);
	free(min_row); free(min_diag);

	*line1 = row+offset1;
	*line2 = col+offset2;

	return d;
      }

      if (col == 0) {
	/* hit last column; don't look further */
	free(last_d); free(temp_d);
	free(min_row); free(min_diag);

	*line1 = row+offset1;
	*line2 = col+offset2;

	return d;
      }
    }

    min_row[d] = last_d[ORIGIN+DELTA];
    min_diag[d] = ORIGIN+DELTA;
    for (k=lower; k<=upper; ++k)
      if (temp_d[k]<min_row[d]) {
	min_row[d] = temp_d[k];
	min_diag[d] = k;
      }

    for (k=lower; k<=upper; k++) {
      last_d[k] = temp_d[k];
    }

    --lower;
    ++upper;
  }

  /* report here the previous maximal match, stored in min_diag and min_row */
  while ((d>0) && (min_row[d-1]-min_row[d]<3))
    d--;

  *line1 = min_row[d]+offset1;
  *line2 = min_row[d]+min_diag[d]-ORIGIN+offset2;

  free(min_row);
  free(min_diag);
  free(last_d);
  free(temp_d);

  return d;
}


static int
extend_fw(uchar *s1, uchar *s2, int m, int n, int offset1, int offset2,
	  int *line1, int *line2, int W)
{
  int     col,                    /* column number */
          row,                    /* row number */
          max_d,                  /* bound on the length of the edit script */
          d,                      /* current compressed distance */
          k,                      /* current diagonal */
          ORIGIN,
          lower,
          upper;
  int     *last_d, *temp_d;       /* column containing the last p */
  int     *max_row, *max_diag;    /* min (b)/ max (f) row (and diagonal) */
                                  /* reached for cost d=0, ... m.  */
  max_d = m+1;

  ORIGIN = m;
  for (row=0, col=0; col<n && row<m && (s1[row]==s2[col]); row++, col++)
    /*LINTED empty loop body*/;

  if (row == m) {
    *line1 = row+offset1;
    *line2 = col+offset2;

    return 0;
  }
  if (col == n) {
    *line1 = row+offset1;
    *line2 = col+offset2;

    return 0;
  }

  last_d = (int *)xmalloc((m+n+1)*sizeof(int));
  temp_d = (int *)xmalloc((m+n+1)*sizeof(int));

  for (k=0; k<=m+n; ++k) last_d[k]=-1;
  last_d[ORIGIN] = row;

  lower = ORIGIN - 1;
  upper = ORIGIN + 1;

  max_row = (int *)xmalloc((m+1)*sizeof(int));
  max_diag = (int *)xmalloc((m+1)*sizeof(int));

  for (d=1; d<=m; d++)
    max_row[d] = -1;

  max_row[0] = last_d[ORIGIN];
  max_diag[0] = ORIGIN;

  d = 0;
  while ((++d<=max_d) &&
         ((d-1<=good_ratio(max_row[d-1], W)) ||
          ((d>=2) && (d-2<=good_ratio(max_row[d-2], W))))) {

    /* for each relevant diagonal ... */
    for (k = lower; k <= upper; k++) {

      /* find a d on diagonal k */
      if (k==-d+ORIGIN) {

	/* move down from the last d-1 on diagonal k+1 */
	row = last_d[k+1]+1;
	/* op = DELETE; */
      } else if (k==d+ORIGIN) {

	/* move right from the last d-1 on diagonal k-1 */
	row = last_d[k-1];
	/* op = INSERT; */
      } else if ((last_d[k]>=last_d[k+1]) &&
		 (last_d[k]+1>=last_d[k-1])) {

	/* substitution */
	row = last_d[k]+1;
	/* op = SUBSTITUTE; */
      } else if ((last_d[k+1]+1>=last_d[k-1]) &&
		 (last_d[k+1]>=last_d[k])) {

	/* move down from the last d-1 on diagonal k+1 */
	row = last_d[k+1]+1;
	/* op = DELETE; */
      } else {

	/* move right from the last d-1 on diagonal k-1 */
	row = last_d[k-1];
	/* op = INSERT; */
      }

      /* code common to the three cases */
      /* slide down the diagonal */

      col = row+k-ORIGIN;

      if (row>=0)
	while ((row < m) && (col < n) && (s1[row]==s2[col])) {
	  row++; col++;
	}

      temp_d[k] = row;

      if ((row == m) && (col == n)) {
	/* hit southeast corner; have the answer */
	free(last_d); free(temp_d);
	free(max_row); free(max_diag);
	*line1 = row+offset1;
	*line2 = col+offset2;
	return d;
      }
      if (row == m) {
	/* hit last row; don't look further */
	free(temp_d); free(last_d);
	free(max_row); free(max_diag);
	*line1 = row+offset1;
	*line2 = col+offset2;
	return d;
      }

      if (col == n) {
	/* hit last column; don't look further */
	free(temp_d); free(last_d);
	free(max_row); free(max_diag);
	*line1 = row+offset1;
	*line2 = col+offset2;
	return d;
      }
    }
    max_row[d] = last_d[ORIGIN];
    max_diag[d] = ORIGIN;
    for (k=lower; k<=upper; ++k)
      if (temp_d[k]>max_row[d]) {
	max_row[d] = temp_d[k];
	max_diag[d] = k;
      }

    for (k=lower; k<=upper; k++) {
      last_d[k] = temp_d[k];
    }

    --lower;
    ++upper;
  }

  /* report here the previous maximal match, stored in max_diag and max_row */

  while ((d>0) && (max_row[d]-max_row[d-1]<3))
    d--;

  *line1 = max_row[d]+offset1;
  *line2 = max_row[d]+max_diag[d]-ORIGIN+offset2;
  free(max_row);
  free(max_diag);
  free(last_d);
  free(temp_d);
  return d;

/*
     if ((d>2) && (max_row[d-1]-max_row[d-2]<3)) {
          *line1 = max_row[d-2]+offset1;
          *line2 = max_row[d-2]+max_diag[d-2]-ORIGIN+offset2;

          free(max_row); free(max_diag);
          free(last_d); free(temp_d);

          return d-2;
     }

     *line1 = max_row[d-1]+offset1;
     *line2 = max_row[d-1]+max_diag[d-1]-ORIGIN+offset2;

     free(max_row);
     free(max_diag);
     free(last_d);
     free(temp_d);

     return d-1;
*/
}

static void
swap_seqs(collec_p_t eCol)
{
  unsigned int i;
  for (i = 0; i < eCol->nb; i++) {
    exon_p_t e = eCol->e.exon[i];
    int tem = e->from1;
    e->from1 = e->from2;
    e->from2 = tem;
    tem = e->to1;
    e->to1 = e->to2;
    e->to2 = tem;
  }
}

static void
merge(collec_p_t eCol, collec_p_t aCol, unsigned int pos, unsigned int W)
{
  unsigned int last = pos + aCol->nb;
  unsigned int i;
  if (aCol->nb == 0)
    return;
  /* Make enough room.  */
  if (eCol->nb + aCol->nb > eCol->size) {
    eCol->size = eCol->nb + aCol->nb;
    eCol->e.elt = (void **) xrealloc(eCol->e.elt, eCol->size * sizeof(void *));
  }
  /* Insert the new exons.  */
  memmove(eCol->e.elt + last,
	  eCol->e.elt + pos, (eCol->nb - pos) * sizeof(void *));
  memcpy(eCol->e.elt + pos, aCol->e.elt, aCol->nb * sizeof(void *));
  eCol->nb += aCol->nb;
  if (last < eCol->nb)
    last += 1;
  if (pos == 0)
    pos += 1;
  for (i = pos; i < last; i++) {
    exon_p_t cur = eCol->e.exon[i - 1];
    exon_p_t next = eCol->e.exon[i];
    /* Check for new exons that migth have gobbled up existing ones.  */
    if (next->from2 <= cur->from2) {
      free(cur);
      memmove(eCol->e.elt + i - 1, eCol->e.elt + i,
	      (eCol->nb - i) * sizeof(void *));
      eCol->nb -= 1;
      last -= 1;
      i -= 1;
      continue;
    }
    if (cur->to2 >= next->to2) {
      free(next);
      eCol->nb -= 1;
      memmove(eCol->e.elt + i, eCol->e.elt + i + 1,
	      (eCol->nb - i) * sizeof(void *));
      last -= 1;
      i -= 1;
      continue;
    }
    if (next->from1 < cur->to1 + 1 + MIN_INTRON
	&& next->from2 <= cur->to2 + 1 + W) {
      /* merge blocks cur and next */
      cur->from1 = min(cur->from1, next->from1);
      cur->from2 = min(cur->from2, next->from2);
      cur->to1 = max(next->to1, cur->to1);
      cur->to2 = max(next->to2, cur->to2);
      free(next);
      eCol->nb -= 1;
      memmove(eCol->e.elt + i, eCol->e.elt + i + 1,
	      (eCol->nb - i) * sizeof(void *));
      last -= 1;
      i -= 1;
    }
  }
}

void
free_align(edit_script_list_p_t aligns)
{
  edit_script_list_p_t head;

  head = aligns;

  while ((head=aligns)!=NULL) {
    aligns = aligns->next_script;
    Free_script(head->script);
    free(head);
  }
}

#ifdef DEBUG
static void
debug_print_exons(collec_p_t eCol, const char *label,
		  const unsigned char *s1, const unsigned char *s2)
{
  unsigned int i;

  fprintf(stderr, "\n====================%s:\n\n", label);
  for (i = 0; i < eCol->nb; i++) {
    exon_p_t e = eCol->e.exon[i];
    fprintf(stderr, " [ %u, %u, %u, %u ]\n",
	    e->from1, e->from2, e->to1, e->to2);
  }
  for (i = 0; i < eCol->nb; i++) {
    exon_p_t e = eCol->e.exon[i];
    int len1 = e->to1 - e->from1 + 1;
    int len2 = e->to2 - e->from2 + 1;
    if (len1 > 1) {
      fprintf(stderr, "%.10s %.*s %.10s\n%.10s %.*s %.10s\n",
	      (e->from1 > 10)
		? s1 + e->from1 - 11
		: s1 + e->from1 - 1,
	      len1, s1 + e->from1 - 1,
	      s1 + e->to1,
	      (e->from2 > 10)
		? s2 + e->from2 - 11
		: s2 + e->from2 - 1,
	      len2, s2 + e->from2 - 1,
	      s2 + e->to2);
      if (e->from1 > 1 && e->from2 > 1
	  && s1[e->from1 - 2] == s2[e->from2 - 2])
	fprintf(stderr, "WARNING: further left match: %c\n",
		s1[e->from1 - 2]);
      if (s1[e->to1] != 0 && s1[e->to1] == s2[e->to2])
	fprintf(stderr, "WARNING: further right match: %c\n",
		s1[e->to1]);
    }
  }
}
#endif

static int
perfect_spl_p(uchar *seq1, uchar *seq2, splice_score_p_t splS)
{
  unsigned int score, j;
  uchar splice[4];
  score = SWscore(seq1 + splS->to1 - options.scoreSplice_window,
		  seq2 + splS->to2 - options.scoreSplice_window,
		  options.scoreSplice_window);
  if (score < options.scoreSplice_window)
    return 0;
  score = SWscore(seq1 + splS->nFrom1 - 1, seq2 + splS->to2,
		  options.scoreSplice_window);
  if (score < options.scoreSplice_window)
    return 0;
  memcpy(splice, seq1 + splS->to1, 2);
  memcpy(splice + 2, seq1 + splS->nFrom1 - 3, 2);
  for (j = 0; j < options.nbSplice; j++) {
    if (memcmp(splice, options.splice[j].fwd, 4) == 0) {
      splS->type = j;
      splS->direction = 1;
      return 1;
    }
    if (memcmp(splice, options.splice[j].rev, 4) == 0) {
      splS->type = j;
      splS->direction = -1;
      return 1;
    }
  }
  return 0;
}

static int
splice_score_compare(const void *a, const void *b)
{
  const splice_score_p_t sa = (splice_score_p_t) a;
  const splice_score_p_t sb = (splice_score_p_t) b;
  if (sa->score < sb->score)
    return -1;
  if (sa->score > sb->score)
    return 1;
  if (sa->splScore < sb->splScore)
    return -1;
  if (sa->splScore > sb->splScore)
    return 1;
  if (sa->type > sb->type)
    return -1;
  if (sa->type < sb->type)
    return 1;
  return 0;
}

static void
compute_max_score_1(uchar *seq1, uchar *seq2, splice_score_p_t splS,
		    unsigned int type, unsigned int to1, unsigned int to2,
		    unsigned int nFrom1, uchar *s, uchar *jct, int dir)
{
  int j;

  memcpy(s + options.scoreSplice_window, jct, 4);
  for (j = - options.intron_window; j <= (int) options.intron_window; j++) {
    splice_score_t curL, curR;
    int i;
    curL.type = curR.type = type;
    curL.splScore = curR.splScore = 0;
    curL.score = curR.score = 0;
    memcpy(s, seq2 + to2 - options.scoreSplice_window + j,
	   options.scoreSplice_window);
    memcpy(s + options.scoreSplice_window + 4, seq2 + to2 + j,
	   options.scoreSplice_window);
    for (i = -1; i <= 1; i++) {
      splice_score_t cur;
      cur.type = type;
      cur.splScore = 0;
      if (seq1[to1 + j + i] == jct[0])
	cur.splScore += 1;
      if (seq1[to1 + j + i + 1] == jct[1])
	cur.splScore += 1;
      cur.score = SWscore(seq1 + to1 - options.scoreSplice_window
			  + j + i, s, options.scoreSplice_window + 2);
      if (splice_score_compare(&cur, &curL) > 0) {
	curL.score = cur.score;
	curL.splScore = cur.splScore;
	curL.to1 = to1 + j + i;
      }
      cur.splScore = 0;
      if (seq1[nFrom1 - 3 + j + i] == jct[2])
	cur.splScore += 1;
      if (seq1[nFrom1 - 2 + j + i] == jct[3])
	cur.splScore += 1;
      cur.score = SWscore(seq1 + nFrom1 - 3 + j + i,
			  s + options.scoreSplice_window + 2,
			  options.scoreSplice_window + 2);
      if (splice_score_compare(&cur, &curR) > 0) {
	curR.score = cur.score;
	curR.splScore = cur.splScore;
	curR.nFrom1 = nFrom1 + j + i;
      }
    }
    curL.score += curR.score;
    curL.splScore += curR.splScore;
    if (splice_score_compare(&curL, splS) > 0) {
      splS->score = curL.score;
      splS->splScore = curL.splScore;
      splS->to1 = curL.to1;
      splS->to2 = to2 + j;
      splS->nFrom1 = curR.nFrom1;
      splS->type = type;
      splS->direction = dir;
    }
  }
}

/* FIXME : Frame shifts are a real pain.  Look at BM149342 for
 * example.  The scoring is not quite right in that case.  */
static void
compute_max_score(uchar *seq1, uchar *seq2, splice_score_p_t splS,
		  int direction)
{
  unsigned int k;
  unsigned int to1 = splS->to1;
  unsigned int to2 = splS->to2;
  unsigned int nFrom1 = splS->nFrom1;
  uchar *s = (uchar *) xmalloc((options.scoreSplice_window * 2 + 4)
			       * sizeof(uchar));
  splS->score = 0;
  splS->splScore = 0;
  splS->type = -1;
  for (k = 0; k < options.nbSplice; k++) {
    if (direction >= 0)
      compute_max_score_1(seq1, seq2, splS, k, to1, to2, nFrom1, s,
			  options.splice[k].fwd, 1);
    if (direction <= 0)
      compute_max_score_1(seq1, seq2, splS, k, to1, to2, nFrom1, s,
			  options.splice[k].rev, -1);
  }
  free(s);
}

static void
slide_intron(result_p_t r, uchar *seq1, uchar *seq2)
{
  unsigned int i;

  /* First, try to get direction through perfect splices.  */
  for (i = 1; i < r->eCol.nb; i++) {
    exon_p_t cur = r->eCol.e.exon[i - 1];
    exon_p_t next = r->eCol.e.exon[i];
    splice_score_t splS;
    cur->type = -1;
    cur->direction = 0;
    cur->splScore = 0;
    if (next->from2 - cur->to2 != 1)
      continue;
    splS.to1 = cur->to1;
    splS.to2 = cur->to2;
    splS.nFrom1 = next->from1;
    if (perfect_spl_p(seq1, seq2, &splS)) {
      r->direction += splS.direction;
      cur->direction = splS.direction;
      cur->type = splS.type;
      cur->splScore = 4 + options.scoreSplice_window * 2;
    }
  }
  /* Second, go through overlaping exons.  */
  for (i = 1; i < r->eCol.nb; i++) {
    exon_p_t cur = r->eCol.e.exon[i - 1];
    exon_p_t next = r->eCol.e.exon[i];
    splice_score_p_t splS;
    unsigned int nb, j, nbP = 0;
    if (next->from2 > cur->to2)
      continue;
    nb = cur->to2 - next->from2 + 2;
    splS = (splice_score_p_t) xmalloc(nb * sizeof(splice_score_t));
    for (j = 0; j < nb; j++) {
      splS[j].to1 = cur->to1 - nb + j + 1;
      splS[j].to2 = cur->to2 - nb + j + 1;
      splS[j].nFrom1 = next->from1 + j;
      if (perfect_spl_p(seq1, seq2, splS + j))
	nbP += 1;
      else
	splS[j].direction = 0;
    }
    if (nbP == 1)
      for (j = 0; j < nb; j++)
	if (splS[j].direction != 0) {
	  r->direction += splS[j].direction;
	  cur->direction = splS[j].direction;
	  cur->type = splS[j].type;
	  cur->splScore = 4 + options.scoreSplice_window * 2;
	  cur->to1 = splS[j].to1;
	  cur->to2 = splS[j].to2;
	  next->from2 = cur->to2 + 1;
	  next->from1 = splS[j].nFrom1;
	}
    free(splS);
  }
  /* In case we are still undecided...  */
  if (r->direction == 0) {
    unsigned int fwd = 0, rev = 0;
    for (i = 1; i < r->eCol.nb; i++) {
      exon_p_t cur = r->eCol.e.exon[i - 1];
      exon_p_t next = r->eCol.e.exon[i];
      splice_score_t max, cs;
      unsigned int nb, j;
      if (cur->to2 + 1 < next->from2)
	continue;
      if (cur->direction > 0) {
	fwd += cur->splScore;
	continue;
      }
      if (cur->direction < 0) {
	rev += cur->splScore;
	continue;
      }
      nb = cur->to2 - next->from2 + 2;
      max.type = -1;
      max.score = 0;
      max.splScore = 0;
      for (j = 0; j < nb; j++) {
	cs.to1 = cur->to1 - nb + j + 1;
	cs.to2 = cur->to2 - nb + j + 1;
	cs.nFrom1 = next->from1 + j;
	compute_max_score(seq1, seq2, &cs, 0);
	if (splice_score_compare(&cs, &max) > 0)
	  max = cs;
      }
      if (max.direction > 0)
	fwd += max.score;
      if (max.direction < 0)
	rev += max.score;
    }
    if (fwd >= rev)
      r->direction = 1;
    else
      r->direction = -1;
  }
  for (i = 1; i < r->eCol.nb; i++) {
    exon_p_t cur = r->eCol.e.exon[i - 1];
    exon_p_t next = r->eCol.e.exon[i];
    splice_score_t max, cs;
    unsigned int nb, j;
    if ((cur->type >= 0 && cur->direction * r->direction > 0)
	|| cur->to2 + 1 < next->from2)
      continue;
    nb = cur->to2 - next->from2 + 2;
    max.type = -1;
    max.score = 0;
    max.splScore = 0;
    for (j = 0; j < nb; j++) {
      cs.to1 = cur->to1 - nb + j + 1;
      cs.to2 = cur->to2 - nb + j + 1;
      cs.nFrom1 = next->from1 + j;
      compute_max_score(seq1, seq2, &cs, r->direction);
      if (splice_score_compare(&cs, &max) > 0)
	max = cs;
    }
    cur->direction = max.direction;
    cur->type = max.type;
    cur->splScore = max.score;
    cur->to1 = max.to1;
    cur->to2 = max.to2;
    next->from2 = cur->to2 + 1;
    next->from1 = max.nFrom1;
#ifdef DEBUG
    fprintf(stderr, "Resolving intron %d %u %u %u %u scores: %u %u %u\n"
	    "%.10s        ...        %.10s\n"
	    "%.18s...%.18s\n",
	    r->direction, cur->to2, next->from2, cur->to1, next->from1,
	    max.score, max.splScore, max.type,
	    seq2 + cur->to2 - 10, seq2 + next->from2 - 1,
	    seq1 + cur->to1 - 10, seq1 + next->from1 - 9);
#endif
  }
}

/* Compute some sort of score, using a Smith/Waterman style algorithm,
 * but allowing for only one gap.
 * We use a matrix of this form:
 *         T   C   A   G   T ...
 *    +----------------------
 *  A |
 *    |          +===+
 *  T |          | 2 |
 *    |      +===+   |
 *  C |      | 0   1 |
 *    |      +=======+
 *  A |
 *    |
 *  T |
 */
static int
SWscore(uchar *s1, uchar *s2, unsigned int len)
{
  unsigned int i;
  int score[3];
  score[0] = score[2] = 0;
  score[1] = *s1 == *s2 ? 1 : 0;
  for (i = 1; i < len; i++) {
    score[0] = max(score[0] + (s1[i - 1] == s2[i] ? 1 : 0), score[1]);
    score[2] = max(score[2] + (s1[i] == s2[i - 1] ? 1 : 0), score[1]);
    score[1] = max(max(score[0] - 1, score[2] - 1),
		   score[1] + (s1[i] == s2[i] ? 1 : 0));
  }
  return score[1];
}
