/*
 *      program:        Graph partition via Kernighan-Lin, modified
 *                      Kernighan-Lin, or Kernighan-Schweikert
 *
 *      author:         Todd M. Austin
 *                      ECE 756
 *
 *      date:           Thursday, February 25, 1993
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*
 *      module configuration
 */

/* define WEIGHTED for 1/(1-n) weighted node cost matrix, otherwise 1 */
/* #define WEIGHTED */

/* define KS_MODE to execute with Kernighan-Schweikert algorithm, rather
   than the Kernighan-Lin algorithm */
/* #undef KS_MODE */

#define BUF_LEN 1024    /* maximum line length */
#define G_SZ    1024    /* maximum group size */

/* simple exception handler */
#define TRY(exp, accpt_tst, fn, fail_fmt, arg1, arg2, arg3, fail_action) { \
              (exp); \
              if (!(accpt_tst)) { \
                  fprintf(stderr, "(%s:%s():%d): ", __FILE__, fn, __LINE__); \
                  fprintf(stderr, fail_fmt, arg1, arg2, arg3); \
                  fprintf(stderr, "\n"); \
                  fail_action; \
                  } \
                  }

/*
 *      the network, first the modules, then the nets
 */
/* modular view */
typedef struct _Net {
    struct _Net * next;
    unsigned long net;
} Net;
typedef Net * NetPtr;

extern NetPtr modules[G_SZ];	/* all modules -> nets */
extern unsigned long numModules;

/* net-ular view */
typedef struct _Module {
    struct _Module * next;
    unsigned long module;
} Module;
typedef Module * ModulePtr;

extern ModulePtr nets[G_SZ];	/* all nets -> modules */
extern unsigned long numNets;

typedef struct _ModuleRec {
    struct _ModuleRec * next;
    unsigned long module;
} ModuleRec;
typedef ModuleRec * ModuleRecPtr;

typedef struct _ModuleList {
    ModuleRecPtr head;
    ModuleRecPtr tail;
} ModuleList;
typedef ModuleList * ModuleListPtr;

extern ModuleList groupA, groupB;		/* current A, B */
extern ModuleList swapToA, swapToB;		/* swapped from A,B, ordered */
extern float GP[G_SZ];		/* GPs, ordered */

typedef enum { GroupA, GroupB, SwappedToA, SwappedToB } Groups;

extern Groups moduleToGroup[G_SZ];	/* current inverse mapping */
extern float D[G_SZ];		/* module costs */
extern float cost[G_SZ];		/* net costs */

void ReadNetList(char *fname);
void NetsToModules(void);
void ComputeNetCosts(void);
void InitLists(void);
void ComputeDs(ModuleListPtr group, Groups myGroup, Groups mySwap);
float CAiBj(ModuleRecPtr mrA, ModuleRecPtr mrB);
void SwapNode(ModuleRecPtr maxPrev, ModuleRecPtr max,
	      ModuleListPtr group, ModuleListPtr swapTo);
void UpdateDs(ModuleRecPtr max, Groups group);
float FindMaxGpAndSwap();
void SwapSubsetAndReset(unsigned long iMax);
void PrintResults(int verbose);
int main(int argc, char **argv);
