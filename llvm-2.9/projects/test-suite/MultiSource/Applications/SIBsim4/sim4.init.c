/* $Id: sim4.init.c 42667 2007-10-06 00:04:11Z asl $
 *
 * Christian Iseli, LICR ITO, Christian.Iseli@licr.org
 *
 * Copyright (c) 2001-2006 Swiss Institute of Bioinformatics.
 * Copyright (C) 1998-2001  Liliana Florea.
 */

/*
 * TODO
 *  - See why tst23 is split
 *  - See why tst50 has strange alignment
 */

/*
* sim4 - Align a cDNA sequence with a genomic sequence for it.
*
* The basic command syntax is
*
*       sim4 [options] dna.seq rna.seq
*
* where dna.seq names a file containing a DNA sequence and rna.seq
* names a file containing one or more RNA sequences.
* The files are to be in FASTA format.  Thus a typical sequence file
* might begin:
*
*       >BOVHBPP3I  Bovine beta-globin psi-3 pseudogene, 5' end.
*       GGAGAATAAAGTTTCTGAGTCTAGACACACTGGATCAGCCAATCACAGATGAAGGGCACT
*       GAGGAACAGGAGTGCATCTTACATTCCCCCAAACCAATGAACTTGTATTATGCCCTGGGC
*/

#ifdef __sun
#define _XOPEN_SOURCE /* tell sun we want getopt, etc.  */
#define _XPG5 /* and we want snprintf  */
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <locale.h>
#include <signal.h>
#include "sim4.h"
#include "align.h"
#include "misc.h"
#include "sim4b1.h"
#if defined(DEBUG) && (DEBUG > 1)
#include <mcheck.h>
#endif


static void init_seq(const char *, seq_p_t);
static int get_next_seq(seq_p_t, unsigned int, int);
static void seq_revcomp_inplace(seq_p_t);
static void print_align_lat(uchar *, uchar *, result_p_t);
static void print_polyA_info(seq_p_t, seq_p_t, collec_p_t, sim4_stats_p_t);
static void print_res(result_p_t, int, seq_p_t, seq_p_t);
static void init_splice_junctions(void);
static void bug_handler(int);
#ifdef DEBUG
static void free_seq(seq_p_t);
#endif

static const char Usage[] =
"%s [options] dna est_db\n\n"
"This is SIBsim4 version 0.14.\n\n"
#ifdef DEBUG
"Debug version\n\n"
#endif
"Available options (default value in braces[]):\n"
"  -A <int>  output format\n"
"             0: exon endpoints only\n"
"             1: alignment text\n"
"             3: both exon endpoints and alignment text\n"
"             4: both exon endpoints and alignment text with polyA info\n"
"            Note that 2 is unimplemented [%d]\n"
"  -C <int>  MSP score threshold for the second pass [%d]\n"
"  -c <int>  minimum score cutoff [%d]\n"
"  -E <int>  cutoff value [%d]\n"
"  -f <int>  score filter in percent (0 to disable filtering) [%d]\n"
"  -g <int>  join exons when gap on genomic and RNA have lengths which\n"
"            differ at most by this percentage [%d]\n"
"  -I <int>  window width in which to search for intron splicing [%d]\n"
"  -K <int>  MSP score threshold for the first pass [%d]\n"
"  -L <str>  a comma separated list of forward splice-types [%s]\n"
"  -M <int>  scoring splice sites, evaluate match within M nucleotides [%d]\n"
"  -o <int>  offset nt positions in dna sequence by this amount [%u]\n"
"  -q <int>  penalty for a nucleotide mismatch [%d]\n"
"  -R <int>  direction of search\n"
"             0: search the '+' (direct) strand only\n"
"             1: search the '-' strand only\n"
"             2: search both strands and report the best match\n"
"            [%d]\n"
"  -r <int>  reward for a nucleotide match [%d]\n"
"  -W <int>  word size [%d]\n"
"  -X <int>  value for terminating word extensions [%d]\n";

options_t options;
char *argv0;
char dna_seq_head[256];
char rna_seq_head[256];

int
main(int argc, char *argv[])
{
  int count;
  seq_t seq1, seq2;
  hash_env_t he;
  collec_t res, rev_res;
#if defined(DEBUG) && (DEBUG > 1)
  mcheck(NULL);
  mtrace();
#endif
  argv0 = argv[0];
  if (setlocale(LC_ALL, "POSIX") == NULL)
    fprintf(stderr, "%s: Warning: could not set locale to POSIX\n", argv[0]);
  signal(SIGSEGV, bug_handler);
#ifndef __MINGW32__  
  signal(SIGBUS, bug_handler);
#endif  
  /* Default options.  */
  options.C = DEFAULT_C;
  options.cutoff = DIST_CUTOFF;
  options.gapPct = DEFAULT_GAPPCT;
  options.intron_window = 6;
  options.K = DEFAULT_K;
  options.splice_type_list = "GTAG,GCAG,GTAC,ATAC";
  options.nbSplice = 4;
  options.scoreSplice_window = 10;
  options.mismatchScore = MISMATCH;
  options.reverse = 2;
  options.matchScore = MATCH;
  options.W = DEFAULT_W;
  options.X = DEFAULT_X;
  options.filterPct = DEFAULT_FILTER;
  options.minScore_cutoff = MATCH_CUTOFF;
  while (1) {
    int c = getopt(argc, argv, "A:C:c:E:f:g:I:K:L:M:o:q:R:r:W:X:");
    if (c == -1)
      break;
    switch (c) {
    case 'A':
      options.ali_flag = atoi(optarg);
      if (options.ali_flag < 0 || options.ali_flag > 4)
	fatal("A must be one of 0, 1, 2, 3, or 4.\n");
      break;
    case 'C': {
      int val = atoi(optarg);
      if (val < 0)
	fatal("Value for option C must be non-negative.\n");
      options.C = val;
      break;
    }
    case 'c': {
      int val = atoi(optarg);
      if (val < 0)
	fatal("Value for option c must be non-negative.\n");
      options.minScore_cutoff = val;
      break;
    }
    case 'E':
      options.cutoff = atoi(optarg);
      if (options.cutoff < 3 || options.cutoff > 10)
	fatal("Cutoff (E) must be within [3,10].\n");
      break;
    case 'f':
      options.filterPct = atoi(optarg);
      if (options.filterPct > 100)
	fatal("Filter in percent (f) must be within [0,100].\n");
      break;
    case 'g':
      options.gapPct = atoi(optarg);
      break;
    case 'I':
      options.intron_window = atoi(optarg);
      break;
    case 'K': {
      int val = atoi(optarg);
      if (val < 0)
	fatal("Value for option K must be non-negative.\n");
      options.K = val;
      break;
    }
    case 'L': {
      size_t i;
      size_t len = strlen(optarg);
      options.splice_type_list = optarg;
      options.nbSplice = 1;
      if (len % 5 != 4)
	fatal("Splice types list has illegal length (%zu)\n", len);
      for (i = 0; i < len; i++)
	if (i % 5 == 4) {
	  if (options.splice_type_list[i] != ',')
	    fatal("Comma expected instead of %c at position %zu"
		  "in splice types list.\n",
		  options.splice_type_list[i], i);
	  options.nbSplice += 1;
	} else {
	  if (options.splice_type_list[i] != 'A'
	      && options.splice_type_list[i] != 'C'
	      && options.splice_type_list[i] != 'G'
	      && options.splice_type_list[i] != 'T')
	    fatal("Expected 'A', 'C', 'G' or 'T' instead of '%c' at"
		  "position %zu in splice types list.\n",
		  options.splice_type_list[i], i);
	}
      break;
    }
    case 'M': {
      int val = atoi(optarg);
      if (val < 0)
	fatal("Value for option M must be non-negative.\n");
      options.scoreSplice_window = val;
      break;
    }
    case 'o':
      options.dnaOffset = atoi(optarg);
      break;
    case 'q':
      options.mismatchScore = atoi(optarg);
      break;
    case 'R':
      options.reverse = atoi(optarg);
      if (options.reverse < 0 || options.reverse > 2)
	fatal("R must be one of 0, 1, or 2.\n");
      break;
    case 'r':
      options.matchScore = atoi(optarg);
      break;
    case 'W':
      options.W = atoi(optarg);
      if (options.W < 1 || options.W > 15)
	fatal("W must be within [1,15].\n");
      break;
    case 'X':
      options.X = atoi(optarg);
      if (options.X < 1)
	fatal("X must be positive.\n");
      break;
    case '?':
      break;
    default:
      fprintf(stderr, "?? getopt returned character code 0%o ??\n", c);
    }
  }
  if (optind + 2 != argc) {
    fprintf(stderr, Usage, argv[0], options.ali_flag, options.C,
	    options.minScore_cutoff, options.cutoff,
	    options.filterPct, options.gapPct, options.intron_window,
	    options.K, options.splice_type_list, options.scoreSplice_window,
	    options.dnaOffset, options.mismatchScore, options.reverse,
	    options.matchScore, options.W, options.X);
    return 1;
  }

  /* read seq1 */
  init_seq(argv[optind], &seq1);
  if (get_next_seq(&seq1, options.dnaOffset, 1) != 0)
    fatal("Cannot read sequence from %s.\n", argv[optind]);
  strncpy(dna_seq_head, seq1.header, 256);

  /* read seq2 */
  init_seq(argv[optind + 1], &seq2);
  if (get_next_seq(&seq2, 0, 0) != 0)
    fatal("Cannot read sequence from %s.\n", argv[optind + 1]);

  init_encoding();
  init_hash_env(&he, options.W, seq1.seq, seq1.len);
  init_col(&res, 1);
  init_col(&rev_res, 1);
  bld_table(&he);
  init_splice_junctions();

  count = 0;
  while (!count || get_next_seq(&seq2, 0, 0) == 0) {
    unsigned int curRes;
    strncpy(rna_seq_head, seq2.header, 256);
    ++count;

    switch (options.reverse) {
    case  0:
      SIM4(&he, &seq2, &res);
      break;
    case  2:
      SIM4(&he, &seq2, &res);
    case  1:
      seq_revcomp_inplace(&seq2);
      SIM4(&he, &seq2, &rev_res);
      break;
    default:
      fatal ("Unrecognized request for EST orientation.\n");
    }
    /* Keep only the best matches, according to filterPct.  */
    if (options.filterPct > 0) {
      unsigned int max_nmatches = 0;
      for (curRes = 0; curRes < rev_res.nb; curRes++) {
	result_p_t r = rev_res.e.result[curRes];
	if (r->st.nmatches > max_nmatches)
	  max_nmatches = r->st.nmatches;
      }
      for (curRes = 0; curRes < res.nb; curRes++) {
	result_p_t r = res.e.result[curRes];
	if (r->st.nmatches > max_nmatches)
	  max_nmatches = r->st.nmatches;
      }
      max_nmatches = (max_nmatches * options.filterPct) / 100;
      for (curRes = 0; curRes < rev_res.nb; curRes++) {
	result_p_t r = rev_res.e.result[curRes];
	if (r->st.nmatches < max_nmatches)
	  r->st.nmatches = 0;
      }
      for (curRes = 0; curRes < res.nb; curRes++) {
	result_p_t r = res.e.result[curRes];
	if (r->st.nmatches < max_nmatches)
	  r->st.nmatches = 0;
      }
    }
    /* Now, print results.  */
    for (curRes = 0; curRes < rev_res.nb; curRes++)
      print_res(rev_res.e.result[curRes], 1, &seq1, &seq2);
    rev_res.nb = 0;
    if (options.reverse && options.ali_flag)
      /* reverse-complement back seq2 for alignment */
      seq_revcomp_inplace(&seq2);
    for (curRes = 0; curRes < res.nb; curRes++)
      print_res(res.e.result[curRes], 0, &seq1, &seq2);
    res.nb = 0;
  }
#ifdef DEBUG
  fprintf(stderr, "DEBUG mode: freeing all memory...\n");
  fflush(stdout);
  fflush(stderr);
  free_hash_env(&he);
  free_seq(&seq1);
  free_seq(&seq2);
  free(options.splice);
  free(res.e.elt);
  free(rev_res.e.elt);
#endif
  return 0;
}

static const unsigned char dna_complement[256] =
  "                                                                "
  " TVGH  CD  M KN   YSA BWXR       tvgh  cd  m kn   ysa bwxr      "
  "                                                                "
  "                                                                ";
/* ................................................................ */
/* @ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~. */
/* ................................................................ */
/* ................................................................ */

static void
init_splice_junctions(void)
{
  unsigned int i;
  options.splice = (junction_p_t) xmalloc(options.nbSplice
			  		  * sizeof(junction_t));
  for (i = 0; i < options.nbSplice; i++) {
    unsigned int j;
    for (j = 0; j < 4; j++) {
      uchar c = options.splice_type_list[i * 5 + j];
      options.splice[i].fwd[j] = c;
      options.splice[i].rev[3 - j] = dna_complement[c];
    }
  }
#ifdef DEBUG
  for (i = 0; i < options.nbSplice; i++)
    fprintf(stderr, "Splice[%u]: %.4s %.4s\n",
	    i, options.splice[i].fwd, options.splice[i].rev);
#endif
}

static void
print_res(result_p_t res, int rev, seq_p_t seq1, seq_p_t seq2)
{
  unsigned int i;
  if (res->st.nmatches >= options.minScore_cutoff) {
    printf("\n%s%s\n", seq1->header, seq2->header);
    if (rev)
      printf("(complement)\n\n");
    switch (options.ali_flag) {
    case 0:
      print_exons(&res->eCol, res->direction);
      break;
    case 1:
      print_align_lat(seq1->seq, seq2->seq, res);
      break;
    case 3:
      print_exons(&res->eCol, res->direction);
      print_align_lat(seq1->seq, seq2->seq, res);
      break;
    case 4:
      print_exons(&res->eCol, res->direction);
      print_polyA_info(seq1, seq2, &res->eCol, &res->st);
      print_align_lat(seq1->seq, seq2->seq, res);
      break;
    default:
      fatal("Unrecognized option for alignment output.\n");
    }
    printf("\n");
  }
  for (i = 0; i < res->eCol.nb; i++)
    free(res->eCol.e.elt[i]);
  free(res->eCol.e.elt);
  if (res->sList)
    free_align(res->sList);
  free(res);
}

static void
print_polyA_info(seq_p_t s1, seq_p_t s2, collec_p_t eCol,
		 sim4_stats_p_t st)
{
  if (st->polyA_cut) {
    unsigned int cnt = 0, cntDna = 0, pos, i, scanLen = 50;
    char *pSig, buf[51];
    exon_p_t e = eCol->e.exon[eCol->nb - 1];
    for (pos = 0; pos < 10 && e->to2 + pos < s2->len; pos++)
      if (s2->seq[e->to2 + pos] == 'A')
	cnt += 1;
    while (e->to2 + pos < s2->len && s2->seq[e->to2 + pos] == 'A') {
      pos += 1;
      cnt += 1;
    }
    for (i = 0; i < s1->len && i < pos; i++)
      if (s1->seq[e->to1 + i] == 'A')
	cntDna += 1;
    printf("\nPolyA site %u nt, %u/%u A's %u\n R %.*s %u\n D %*.*s %u\n",
	   pos, cnt, cntDna, e->to1 + 1 + options.dnaOffset,
	   pos, s2->seq + e->to2, e->to2 + 1,
	   pos, i, s1->seq + e->to1, e->to1 + 1 + options.dnaOffset);
    if (e->to1 < scanLen)
      scanLen = e->to1;
    strncpy(buf, (char *) s1->seq + e->to1 - scanLen, scanLen);
    buf[scanLen] = 0;
    pSig = strstr(buf, "AATAAA");
    if (pSig == NULL)
      pSig = strstr(buf, "ATTAAA");
    if (pSig != NULL)
      printf("PolyA signal %u\n",
	     (unsigned int) (pSig - buf + e->to1 - scanLen + 1
			     + options.dnaOffset));
  }
  if (st->polyT_cut) {
    unsigned int cnt = 0, cntDna = 0, pos, i;
    char *pSig, buf[51];
    exon_p_t e = eCol->e.exon[0];
    for (pos = 0; pos < 10 && pos < e->from2 - 1; pos++)
      if (s2->seq[e->from2 - 2 - pos] == 'T')
	cnt += 1;
    while (pos < e->from2 - 1 && s2->seq[e->from2 - 2 - pos] == 'T') {
      pos += 1;
      cnt += 1;
    }
    for (i = 0; i < e->from1 - 1 && i < pos; i++)
      if (s1->seq[e->from1 - 2 - i] == 'T')
	cntDna += 1;
    printf("\nPolyA site %u nt, %u/%u A's %u minus strand\n R %.*s %u\n D %*.*s %u\n",
	   pos, cnt, cntDna, e->from1 - 1 + options.dnaOffset,
	   pos, s2->seq + (e->from2 - 1 - pos), e->from2 - 1,
	   pos, i, s1->seq + (e->from1 - 1 - i),
	   e->from1 - 1 + options.dnaOffset);
    strncpy(buf, (char *) s1->seq + e->from1 - 1, 50);
    buf[50] = 0;
    pSig = strstr(buf, "TTTATT");
    if (pSig == NULL)
      pSig = strstr(buf, "TTTAAT");
    if (pSig != NULL)
      printf("PolyA signal %u minus strand\n",
	     (unsigned int) (pSig - buf + e->from1 + 5 + options.dnaOffset));
  }
}

static void
print_align_lat(uchar *seq1, uchar *seq2, result_p_t r)
{
  int *S;
  edit_script_list_p_t head, aligns;

  if (r->sList == NULL)
    return;
  aligns = r->sList;
  while (aligns != NULL) {
    head = aligns;
    aligns = aligns->next_script;
    S = (int *) xmalloc((2 * head->len2 + 1 + 1) * sizeof(int));
    S++;
    S2A(head->script, S, 0);
    Free_script(head->script);
    IDISPLAY(seq1 + head->offset1 - 1 - 1, seq2 + head->offset2 - 1 - 1,
	     head->len1, head->len2, S,
	     head->offset1, head->offset2, &r->eCol, r->direction);
    free(S - 1);
    free(head);
  }
  r->sList = NULL;
}

#ifdef DEBUG
static void
free_buf(read_buf_p_t b)
{
  free(b->line);
}

static void
free_seq(seq_p_t sp)
{
  free(sp->seq);
  free(sp->header);
  free_buf(&sp->rb);
  if (sp->fName != NULL)
    close(sp->fd);
}
#endif

static void
grow_read_buf(read_buf_p_t b)
{
  b->lmax += BUF_SIZE;
  b->line = xrealloc(b->line, b->lmax * sizeof(char));
}

static char *
shuffle_line(read_buf_p_t b, size_t *cur)
{
  if (b->ic == 0 || *cur >= b->ic)
    return NULL;
  /* Make sure we have enough room in line.  */
  if (b->lmax <= b->lc + (b->ic - *cur))
    grow_read_buf(b);
  while (*cur < b->ic && b->in[*cur] != '\n')
    b->line[b->lc++] = b->in[(*cur)++];
  if (*cur < b->ic) {
    /* Ok, we have our string.  */
    /* Copy the newline.  */
    b->line[b->lc++] = b->in[(*cur)++];
    /* We should be fine, since we read BUF_SIZE -1 at most...  */
    b->line[b->lc] = 0;
    /* Adjust the input buffer.  */
    if (*cur < b->ic) {
      memmove(b->in, b->in + *cur, (b->ic - *cur) * sizeof(char));
      b->ic -= *cur;
    } else
      b->ic = 0;
    *cur = 0;
    return b->line;
  }
  /* Go read some more.  */
  b->ic = 0, *cur = 0;
  return NULL;
}

static char *
read_line_buf(read_buf_p_t b, int fd)
{
  char *s = NULL;
  ssize_t rc;
  size_t cur = 0;
  b->lc = 0;
  if ((s = shuffle_line(b, &cur)) != NULL)
    return s;
  do {
    if ((rc = read(fd, b->in + b->ic, BUF_SIZE - b->ic - 1)) == -1) {
      if (errno != EINTR)
	fatal("Could not read from %d: %s(%d)\n",
	      fd, strerror(errno), errno);
    } else
      b->ic += rc;
    s = shuffle_line(b, &cur);
    if (s == NULL && rc == 0) {
      /* Got to the EOF...  */
      b->line[b->lc] = 0;
      s = b->line;
    }
  } while (s == NULL);
  return s;
}

static void
init_buf(read_buf_p_t b)
{
  b->line = xmalloc(BUF_SIZE * sizeof(char));
  b->lmax = BUF_SIZE;
  b->lc = 0;
  b->ic = 0;
}

static void
init_seq(const char *fName, seq_p_t sp)
{
  sp->fName = fName;
  sp->header = NULL;
  sp->seq = NULL;
  init_buf(&sp->rb);
  if (fName != NULL) {
    sp->fd = open(fName, O_RDONLY);
    if (sp->fd == -1)
      fatal("Could not open file %s: %s(%d)\n",
	    fName, strerror(errno), errno);
  } else
    sp->fd = 0;
  sp->len = 0;
  sp->maxHead = 0;
  sp->max = 0;
  read_line_buf(&sp->rb, sp->fd);
}

static int
get_next_seq(seq_p_t sp, unsigned int offset, int warnMultiSeq)
{
  const int lenStr = 24;
  unsigned int headerLen;
  char *buf = sp->rb.line;
  int res;
  while (sp->rb.lc > 0 && buf[0] != '>')
    buf = read_line_buf(&sp->rb, sp->fd);
  if (sp->rb.lc == 0)
    return -1;
  /* We have the FASTA header.  */
  if (sp->rb.lc + lenStr + 1 > sp->maxHead) {
    sp->maxHead = sp->rb.lc + lenStr + 1;
    sp->header = (char *) xrealloc(sp->header, sp->maxHead * sizeof(char));
  }
  headerLen = sp->rb.lc;
  memcpy(sp->header, buf, (sp->rb.lc + 1) * sizeof(char));
  sp->len = 0;
  buf = read_line_buf(&sp->rb, sp->fd);
  while (sp->rb.lc > 0 && buf[0] != '>') {
    unsigned char c;
    /* Make sure we have enough room for this additional line.  */
    if (sp->len + sp->rb.lc + 1 > sp->max) {
      sp->max = max(sp->len + sp->rb.lc + 1,
		    sp->max + 0x40000);
      sp->seq = (unsigned char *)
	xrealloc(sp->seq, sp->max * sizeof(unsigned char));
    }
    while ((c = *buf++) != 0) {
      if (isupper(c)) {
	sp->seq[sp->len++] = c;
      } else if (islower(c)) {
	sp->seq[sp->len++] = toupper(c);
      }
    }
    buf = read_line_buf(&sp->rb, sp->fd);
  }
  if (warnMultiSeq && sp->rb.lc > 0)
    fprintf(stderr, "\n"
	    "***  WARNING                                           ***\n"
	    "***  there appears to be several sequences in the DNA  ***\n"
	    "***  sequence file.  Only the first one will be used,  ***\n"
	    "***  which might not be what was intended.             ***\n"
	    "\n");
  sp->seq[sp->len] = 0;
  buf = strstr(sp->header, "; LEN=");
  if (buf) {
    char *s = buf + 6;
    headerLen -= 6;
    while (isdigit(*s)) {
      s += 1;
      headerLen -= 1;
    }
    while (*s)
      *buf++ = *s++;
  }
  buf = sp->header + headerLen - 1;
  while (iscntrl(*buf) || isspace(*buf))
    buf -= 1;
  res = snprintf(buf + 1, lenStr, "; LEN=%u\n", sp->len + offset);
  if (res < 0 || res >= lenStr)
    fatal("Sequence too long: %u\n", sp->len);
  return 0;
}

static void
seq_revcomp_inplace(seq_p_t seq)
{
  unsigned char *s = seq->seq;
  unsigned char *t = seq->seq + seq->len;
  unsigned char c;
  while (s < t) {
    c = dna_complement[*--t];
    *t = dna_complement[*s];
    *s++ = c;
  }
}

static void
bug_handler(int signum)
{
  fflush(stdout);
  fflush(stderr);
  fprintf(stderr, "\nCaught signal %d while processing:\n%.256s\n%.256s\n",
	  signum, dna_seq_head, rna_seq_head);
  abort();
}
