/* For copyright information, see olden_v1.0/COPYRIGHT */

#include "tsp.h"
#include <stdio.h>
#define conquer_thresold 150  /* tsp() will use conquer for problems <= conquer_thresold */
extern int flag;

void print_tree(Tree t)
{
  Tree left,right;

  double x,y;

  if (!t) return;
  x = t->x; y = t->y;
  chatting("x=%f,y=%f\n",x,y);
  left = t->left; right=t->right;
  print_tree(left);
  print_tree(right);
}

void print_list(Tree t)
{
  Tree tmp;
  double x,y;

  if (!t) return;
  x = t->x; y = t->y;
  chatting("%f %f\n",x,y);
  for (tmp=t->next; tmp!=t; tmp=tmp->next) 
    {
    x = tmp->x; y = tmp->y;
    chatting("%f %f\n",x,y);
    }
}

int main(int argc,char *argv[])
{
  Tree t;
  int num;
 
  num=dealwithargs(argc,argv);

  chatting("Building tree of size %d\n",num);
  t=build_tree(num,0,0,NumNodes,0.0,1.0,0.0,1.0);
  if (!flag) chatting("Past build\n");
  if (flag) chatting("newgraph\n");
  if (flag) chatting("newcurve pts\n");

  printf("Call tsp(t, %d, %d)\n", conquer_thresold, NumNodes); 
  tsp(t,conquer_thresold, NumNodes);

  if (flag) print_list(t);
  if (flag) chatting("linetype solid\n");

  return 0;
}
