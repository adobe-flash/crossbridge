#include <stdio.h>
int bug(unsigned char inbuff)
{
  int incnum = (signed char)(inbuff);
  if ((incnum!=1)&&(incnum!=-1)) {
    return (incnum > 1) ? 42 : 23 ;
  } else {
    return 17;
  }
}

int main(int argc, char** argv)
{
  printf("%d\n", bug(-1));
}
