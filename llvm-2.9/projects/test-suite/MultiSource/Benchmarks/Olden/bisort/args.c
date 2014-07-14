/* For copyright information, see olden_v1.0/COPYRIGHT */
#include <stdlib.h>

extern int NumNodes,NDim;
extern int flag;

int mylog(int num) {
  int j=0,k=1;
  
  while(k<num) { k*=2; j++; }
  return j;
} 

int dealwithargs(int argc, char *argv[])
{
  int size;

  if (argc > 3)
    flag = atoi(argv[3]);
  else
    flag = 1;

  if (argc > 2) 
    NumNodes = atoi(argv[2]);
  else 
    NumNodes = 4;

  if (argc > 1)
    size = atoi(argv[1]);
  else
    size = 1 << 15;

  NDim = mylog(NumNodes);
  return size;
}

