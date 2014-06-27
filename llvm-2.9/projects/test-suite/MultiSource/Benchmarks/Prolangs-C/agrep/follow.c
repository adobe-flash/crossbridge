/* the functions in this file take a syntax tree for a regular
   expression and produce a DFA using the McNaughton-Yamada
   construction.						*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re.h"

/*
extern char *strncpy(), *strcat(), *strcpy();
extern int  strlen();
extern char *strcat(char *s1, const char *s2);
*/

#define TRUE	1
/*
extern char *malloc();
*/
extern Pset pset_union(Pset s1, Pset s2); 
extern int pos_cnt;
extern Re_node parse(char *s);

Re_lit_array lpos; 


/*  extend_re() extends the RE by adding a ".*(" at the front and a "("
    at the back.						   	*/

char *extend_re(char *s)
{
    char *s1;

    s1 = (char *) malloc((unsigned) strlen(s)+4+1);
    return (char *) strcat(strcat(strcpy(s1, ".*("), s), ")");
}

/* mk_followpos() takes a syntax tree for a regular expression and
   traverses it once, computing the followpos function at each node
   and returns a pointer to an array whose ith element is a pointer
   to a list of position nodes, representing the positions in
   followpos(i).							*/

void mk_followpos_1(Re_node e, Pset_array fpos)
{
    Pset pos;
    int i;

    switch (Op(e)) {
	case EOS: break;
	case OPSTAR:
	    pos = Lastpos(e);
	    while (pos != NULL) {
		i = pos->posnum;
		(*fpos)[i] = pset_union(Firstpos(e), (*fpos)[i]);
		pos = pos->nextpos;
	    }
	    mk_followpos_1(Child(e), fpos);
	    break;
	case OPCAT:
	    pos = Lastpos(Lchild(e));
	    while (pos != NULL) {
		i = pos->posnum;
		(*fpos)[i] = pset_union(Firstpos(Rchild(e)), (*fpos)[i]);
		pos = pos->nextpos;
	    }
	    mk_followpos_1(Lchild(e), fpos);
	    mk_followpos_1(Rchild(e), fpos);
	    break;
	case OPOPT:
	    mk_followpos_1(Child(e), fpos);
	    break;
	case OPALT:
	    mk_followpos_1(Lchild(e), fpos);
	    mk_followpos_1(Rchild(e), fpos);
	    break;
	case LITERAL:
	    break;
	default: printf("mk_followpos: unknown node type %d\n", Op(e));
    }
    return;
}

Pset_array mk_followpos(Re_node tree, int npos)
{
    int i;
    Pset_array fpos;

    if (tree == NULL || npos < 0) return NULL;
    fpos = (Pset_array) malloc((unsigned) (npos+1)*sizeof(Pset));
    if (fpos == NULL) return NULL;
    for (i = 0; i <= npos; i++) (*fpos)[i] = NULL;
    mk_followpos_1(tree, fpos);
    return fpos;
}

/* mk_poslist() sets a static array whose i_th element is a pointer to
   the RE-literal at position i.  It returns 1 if everything is OK,  0
   otherwise.								*/

/* init performs initialization actions; it returns -1 in case of error,
   0 if everything goes OK.						*/

int init(char *s, int table[32][32])
{
    Pset_array fpos;
    Re_node e;
    Pset l;
    int i, j;

    if ((e = parse(extend_re(s))) == NULL) return -1;
    if ((fpos = mk_followpos(e, pos_cnt)) == NULL) return -1;
    for (i = 0; i <= pos_cnt; i += 1) {
#ifdef Debug
	printf("followpos[%d] = ", i);
#endif
        l = (*fpos)[i];
        j = 0;
        for ( ; l != NULL; l = l->nextpos)  {
#ifdef Debug
            printf("%d ", l->posnum);
#endif
            table[i][j] = l->posnum;
            j++;
        } 
#ifdef Debug
        printf("\n");
#endif
    }
#ifdef Debug
    for (i=0; i <= pos_cnt; i += 1)  {
       j = 0;
       while (table[i][j] != 0) {
          printf(" %d ", table[i][j]);
          j++;
      }
      printf("\n");
   }
#endif
    return (pos_cnt);
}

