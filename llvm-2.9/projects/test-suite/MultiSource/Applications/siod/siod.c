/* 23-DEC-94 George J. Carrette.

   This is a SIOD main program with additional command-line processing
   functionality. See slib.c and siod.html for more information.

 $Id: siod.c 9206 2003-10-17 18:48:45Z gaeke $

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "siod.h"

static char *siod_argv[] = {
  "siod",
  "-h100000:10",
  "-g0",
  "-o1000",
  "-s200000",
  "-n2048"};

int main(int argc,char **argv
#if defined(unix) || defined(vms) || defined(WIN32)
     ,char **env
#endif
     )
{int nargc = argc;
 char **nargv = argv; 
#if defined(WIN32) || defined(vms)
 if (nargc > 0)
  siod_shuffle_args(&nargc,&nargv);
#endif
 process_cla(sizeof(siod_argv)/sizeof(char *),siod_argv,1);
 return(siod_main(nargc,nargv,
#if defined(unix) || defined(vms) || defined(WIN32)
     env
#else
	NULL
#endif
	));}









