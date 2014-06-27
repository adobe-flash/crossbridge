/* For copyright information, see olden_v1.0/COPYRIGHT */

/* node.c
 */
#ifndef TORONTO
#include <cm/cmmd.h>
#endif

#ifdef FUTURES
#include "future-cell.h"
#endif

#ifndef TORONTO
#include "mem-ref.h"
#endif

#include <stdlib.h>
#include "tree.h"

#ifdef TORONTO
extern int NumNodes;
#endif

int dealwithargs(int argc, char *argv[]);

typedef struct {
    long 	level;
} startmsg_t;

int main (int argc, char *argv[])
{
    tree_t	*root;
    int level,result;

#ifdef FUTURES
    level = SPMDInit(argc,argv);
#else
#ifndef TORONTO
    filestuff();
#endif
    level = dealwithargs(argc, argv);
#endif
#ifndef TORONTO
    CMMD_node_timer_clear(0);
    CMMD_node_timer_clear(1);
#endif
#ifdef TORONTO
    chatting("Treeadd with %d levels on %d processors \n",
	     level, NumNodes);
#else
    chatting("Treeadd with %d levels on %d processors \n",
	     level, __NumNodes);
#endif
    /* only processor 0 will continue here. */
    chatting("About to enter TreeAlloc\n"); 
#ifndef TORONTO
    CMMD_node_timer_start(0);
#endif

#ifdef TORONTO
    root = TreeAlloc (level, 0, NumNodes);
#else
    root = TreeAlloc (level, 0, __NumNodes);
#endif

#ifndef TORONTO
    CMMD_node_timer_stop(0);
#endif
    chatting("About to enter TreeAdd\n"); 
    
#ifndef PLAIN
    ClearAllStats();
#endif
#ifndef TORONTO
    CMMD_node_timer_start(1);
#endif
{ int i; for (i = 0; i < 100; ++i)
    result = TreeAdd (root);
}
#ifndef TORONTO
    CMMD_node_timer_stop(1);
#endif
    chatting("Received result of %d\n",result);

#ifndef TORONTO
    chatting("Alloc Time = %f seconds\n", CMMD_node_timer_elapsed(0));
    chatting("Add Time = %f seconds\n", CMMD_node_timer_elapsed(1));
#endif

#ifdef FUTURES
    __ShutDown();
#endif
    exit(0);


}

/* TreeAdd:
 */
int TreeAdd (tree_t *t)
{
  if (t == NULL)  {
    return 0;
  }
  else {
#ifdef FUTURES    
    future_cell_int leftval;
    int rightval;
    tree_t *tleft, *tright;
    int value;

    tleft = t->left;
    RPC(tleft, tleft,TreeAdd,&(leftval));
    NOTEST();
    tright = t->right;
    rightval = TreeAdd(tright);
    RTOUCH(&leftval);
    /*chatting("after touch @ 0x%x\n",t);*/
    value = t->val;
    /*chatting("returning from treeadd %d\n",*/
	     /*leftval.value + rightval.value + value);*/
    return leftval.value + rightval + value;
    RETEST();
#else
    int leftval;
    int rightval;
    tree_t *tleft, *tright;
    int value;

    tleft = t->left;            /* <---- 57% load penalty */
    leftval = TreeAdd(tleft);
    tright = t->right;          /* <---- 11.4% load penalty */
    rightval = TreeAdd(tright);
    /*chatting("after touch\n");*/
    value = t->val;             
    /*chatting("returning from treeadd %d\n",*/
	     /*leftval.value + rightval.value + value);*/
    return leftval + rightval + value;
#endif
  }
} /* end of TreeAdd */



