/****  main.c  *********************************************************/

#include "global.h"

extern void init(void);
extern void parse(void);

int main(void)
{
  init();
  parse();
  return 0;     /*  successful termination  */
}
