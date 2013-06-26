/* For copyright information, see olden_v1.0/COPYRIGHT */

#ifndef TORONTO
#include <cm/cmmd.h>
#include <fcntl.h>
#endif

extern int atoi(const char *);

#ifndef TORONTO
extern int __NumNodes;
#else
int NumNodes;
#endif

#ifndef TORONTO
void filestuff()
{
  CMMD_fset_io_mode(stdout, CMMD_independent);
  fcntl(fileno(stdout), F_SETFL, O_APPEND);
  if (CMMD_self_address()) exit(0);
  __InitRegs(0);
}
#endif

int dealwithargs(int argc, char *argv[])
{
  int level;

  if (argc > 2)
#ifndef TORONTO
    __NumNodes = atoi(argv[2]);
  else
    __NumNodes = 4;
#else
    NumNodes = atoi(argv[2]);
  else
    NumNodes = 1;
#endif

  if (argc > 1)
    level = atoi(argv[1]);
  else
    level = 11;

  return level;

}

