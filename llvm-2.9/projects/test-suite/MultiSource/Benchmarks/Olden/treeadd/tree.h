/* For copyright information, see olden_v1.0/COPYRIGHT */

/* tree.h
 */

#ifdef TORONTO
#include <stdio.h>
#define chatting printf
#define PLAIN
#endif

typedef struct tree {
    int		val;
    struct tree *left, *right;
} tree_t;

extern tree_t *TreeAlloc (int level, int lo, int hi);
int TreeAdd (tree_t *t);







