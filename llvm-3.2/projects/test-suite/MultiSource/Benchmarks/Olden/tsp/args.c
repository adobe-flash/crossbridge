/* For copyright information, see olden_v1.0/COPYRIGHT */

#include <fcntl.h>
#include "tsp.h"
int NumNodes, NDim;
int flag;

int mylog(int num)
{
  int j=0,k=1;
  
  while(k<num) { k*=2; j++; }
  return j;
} 

int dealwithargs(int argc, char *argv[])
{
  int num;

  if (argc > 3)
    flag = atoi(argv[3]);
  else 
    flag = 0;

  if (argc > 2) 
    NumNodes = atoi(argv[2]);
  else 
    NumNodes = 4;

  NDim = mylog(NumNodes);

  if (argc > 1)
    num = atoi(argv[1]);
  else
    num = 65535;

  return num;
}









