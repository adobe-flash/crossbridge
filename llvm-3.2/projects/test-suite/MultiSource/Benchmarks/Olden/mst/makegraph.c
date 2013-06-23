/* For copyright information, see olden_v1.0/COPYRIGHT */

#include "mst.h"

/*#define assert(num,a) \
   if (!(a)) {printf("Assertion failure:%d in makegraph\n",num); exit(-1);}*/

#define CONST_m1 10000
#define CONST_b 31415821
#define RANGE 2048
static int HashRange;

static int mult(int p, int q)
{
   int p1, p0, q1, q0;

   p1=p/CONST_m1; p0=p%CONST_m1;
   q1=q/CONST_m1; q0=q%CONST_m1;
   return (((p0*q1+p1*q0) % CONST_m1)*CONST_m1+p0*q0);
}

static int random2(int seed)
{
  int tmp;
  tmp = (mult(seed,CONST_b)+1);
  return tmp;
}

static int compute_dist(int i,int j, int numvert)
{
  int less, gt;
  if (i<j) {less = i; gt = j;} else {less = j; gt = i;}
  return (random2(less*numvert+gt) % RANGE)+1;
}

static int hashfunc(unsigned int key)
{
  return ((key>>3) % HashRange);
}

static void AddEdges(int count1, Graph retval, int numproc, 
                     int perproc, int numvert, int j) 
{
  Vertex tmp;
  Vertex helper[MAXPROC];
  int i;

  for (i=0; i<numproc; i++) {
    helper[i] = retval->vlist[i];
  }

  for (tmp = retval->vlist[j]; tmp; tmp=tmp->next) 
    {
      for (i=0; i<numproc*perproc; i++) 
        {
          int pn,offset,dist;
          Vertex dest;
          Hash hash;
          
          if (i!=count1) 
            {
              dist = compute_dist(i,count1,numvert);
              pn = i/perproc;
              offset = i % perproc;
              dest = ((helper[pn])+offset);
              hash = tmp->edgehash;
              HashInsert((void *) dist,(unsigned int) dest,hash);
              /*assert(4, HashLookup((unsigned int) dest,hash) == (void*) dist);*/
            }
        } /* for i... */
      count1++;
    } /* for tmp... */
}

Graph MakeGraph(int numvert, int numproc) 
{
  int perproc = numvert/numproc;
  int i,j;
  int count1;
  Vertex v,tmp;
  Vertex block;
  Graph retval;
  retval = (Graph)malloc(sizeof(*retval));
  for (i=0; i<MAXPROC; i++) 
    {
      retval->vlist[i]=NULL;
    }
  chatting("Make phase 2\n");
  for (j=numproc-1; j>=0; j--) 
    {
      block = (Vertex) malloc(perproc*(sizeof(*tmp)));
      v = NULL;
      for (i=0; i<perproc; i++) 
        {
          tmp = block+(perproc-i-1);
          HashRange = numvert/4;
          tmp->mindist = 9999999;
          tmp->edgehash = MakeHash(numvert/4,hashfunc);
          tmp->next = v;
          v=tmp;
        }
      retval->vlist[j] = v;
    }

  chatting("Make phase 3\n");
  for (j=numproc-1; j>=0; j--) 
    {
      count1 = j*perproc;
      AddEdges(count1, retval, numproc, perproc, numvert, j);
    } /* for j... */
  chatting("Make phase 4\n");

  chatting("Make returning\n");
  return retval;
}

  




