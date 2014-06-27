/* $Id: sim4.h 34335 2007-02-15 23:46:09Z nicholas $
 *
 * Christian Iseli, LICR ITO, Christian.Iseli@licr.org
 *
 * Copyright (c) 2001-2006 Swiss Institute of Bioinformatics.
 * Copyright (C) 1998-2001  Liliana Florea.
 */

#ifndef SIM4_H
#define SIM4_H

#define DIST_CUTOFF     3
#define DEFAULT_GAPPCT  10
#define MIN_INTRON      30
#define MAX_GRINIT      500
#define MATCH_CUTOFF    50
#define DEFAULT_FILTER  75
#define DEFAULT_W       12
#define DEFAULT_X       12    
#define DEFAULT_K       15
#define DEFAULT_C       15
#define P               (.2)
#define MATCH           1
#define MISMATCH        (-5)
#define DELETE          1
#define INSERT          2
#define SUBSTITUTE      3
#define BUF_SIZE        4096
#define NACHARS         128
#define HASH_SIZE       (1 << 19)
#define HASH_MASK       (HASH_SIZE - 1)

#define min(x, y)       ((x > y) ? (y) : (x))
#define max(x, y)       ((x < y) ? (y) : (x))


/* data structures */

typedef unsigned char uchar;

typedef struct _hash_node_t {
  unsigned int ecode;        /* integer encoding of the word */
  int pos;                   /* positions where word hits query sequence */
} hash_node_t, *hash_node_p_t;

typedef struct _hash_env_t {
  void **hashtab;
  uchar *seq;
  unsigned int len;
  int *next_pos;
  unsigned int mask;
  unsigned int W;
} hash_env_t, *hash_env_p_t;

typedef struct _read_buf_t {
  char *line;
  unsigned int lmax;
  unsigned int lc;
  unsigned int ic;
  char in[BUF_SIZE];
} read_buf_t, *read_buf_p_t;

typedef struct _seq_t {
  const char *fName;
  char *header;
  unsigned char *seq;
  read_buf_t rb;
  int fd;
  unsigned int len;
  unsigned int maxHead;
  unsigned int max;
} seq_t, *seq_p_t;

typedef struct _exon_t {
  unsigned int from1, from2, to1, to2;
  unsigned int score;
  unsigned int Score;
  unsigned int top : 1;
  unsigned int bot : 1;
  int prev : 30;
  int direction : 2;
  unsigned int splScore : 22;
  int type : 8;
} exon_t, *exon_p_t;

typedef struct _sim4_stats {
  unsigned int nmatches;
  int polyA_cut;
  int polyT_cut;
} sim4_stats_t, *sim4_stats_p_t;

/* used only in the alignment stage */
typedef struct _edit_script {
  struct _edit_script *next;
  int num;        /* Number of operations */
  char op_type;   /* SUB, INS, or DEL */
} edit_script_t, *edit_script_p_t;

typedef struct _edit_script_list {
  struct _edit_script_list *next_script;
  edit_script_p_t script;
  unsigned int offset1, offset2;
  unsigned int len1, len2; 
  int score;
} edit_script_list_t, *edit_script_list_p_t;

typedef union _collec_elt_t {
  void **elt;
  exon_p_t *exon;
  struct _result_t **result;
} collec_elt_t;

typedef struct _collec_t {
  collec_elt_t e;
  unsigned int nb;
  unsigned int size;
} collec_t, *collec_p_t;

typedef struct _result_t {
  edit_script_list_p_t sList;
  collec_t eCol;
  unsigned int dStart;
  unsigned int dLen;
  int direction;
  sim4_stats_t st;
} result_t, *result_p_t;

typedef struct _junction_t {
  uchar fwd[4];
  uchar rev[4];
} junction_t, *junction_p_t;

typedef struct _splice_score_t {
  unsigned int to1;
  unsigned int to2;
  unsigned int nFrom1;
  unsigned int type;
  unsigned int score;
  unsigned int splScore;
  int direction;
} splice_score_t, *splice_score_p_t;

typedef struct _options_t {
  char *splice_type_list;
  junction_p_t splice;
  unsigned int nbSplice;
  int ali_flag;
  unsigned int C;
  int cutoff;
  unsigned int gapPct;
  unsigned int intron_window;
  unsigned int K;
  unsigned int scoreSplice_window;
  int mismatchScore;
  int reverse;
  int matchScore;
  int W;
  int X;
  unsigned int dnaOffset;
  unsigned int filterPct;
  unsigned int minScore_cutoff;
} options_t;
extern options_t options;
extern char *argv0;

#endif
