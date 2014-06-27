/********************************************************/
/* pCompress/pUnCompress                                */
/* A three stage file compressor, using Burrows Wheeler */
/* blocksorting, Run Length Encoding and Arithmetic     */
/* coding to achieve good compression.                  */
/* The fact that the compression is not that good in    */
/* reality is probably my fault.                        */
/* It makes a neat benchmark anyways.                   */
/* ---------------------------------------------------- */
/* This is a part of FreeBench v1 and is only intended  */
/* to be used as a benchmark. The use of this software  */
/* for anyting else (such as compression) is not        */
/* recomended, and certainly not supported. Use gzip or */
/* bzip instead, they are both faster and better.       */
/* Peter Rundberg, April 2001                           */
/********************************************************/

#define BENCHMARK

#include <stdio.h>
#include <string.h>
#include "compress.h"
#include "uncompress.h"

int main(int argc, char *argv[])
{
#ifndef BENCHMARK 
  const char* argv0;
#else
  char filename[1000];
#endif  

#ifdef BENCHMARK  
  fprintf(stderr,"Compile date: %s\n", COMPDATE);
  fprintf(stderr,"Compiler switches: %s\n", CFLAGS);
  compress(argc,argv); /* Compress four times to make it take some time... */
  compress(argc,argv);
  compress(argc,argv); 
  compress(argc,argv);
  strcpy(filename,argv[1]);
  strcat(filename,".compr"); /* add the suffix '.compr' */
  argv[1]=filename;
  uncompress(argc,argv); /* Uncompress the stuff */
  remove(filename);
#else
  if((argv0 = strrchr(argv[0], '/')) == NULL)
        argv0 = argv[0];
    else
        argv0 += 1;

  if (!strcmp(argv0,"pcompress2")) {
    compress(argc,argv);
  } else if (!strcmp(argv0,"puncompress2")) {
    uncompress(argc,argv);
  } else {
    printf("Call pCompress as 'pcompress2' or 'puncompress2', NOT %s\n",argv[0]);
  }
#endif
  return 0;
}

