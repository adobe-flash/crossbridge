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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

unsigned char *in; /* The infile */
unsigned char *deari;
unsigned char *derle;
unsigned char *debw;
unsigned int size;
unsigned int orgpos;

static void do_debwe();
static void do_derle(int insize);
unsigned int do_deari(unsigned int insize);  /* In "unarithmetic.c" */

void uncompress(int argc, char *argv[]) 
{
  FILE *fpi;
#ifndef BENCHMARK
  FILE *fpo;
  char outname[1000];
#endif
  unsigned int insize, outsize;

  fpi=fopen(argv[1],"r"); /* open the infile */
  if (fpi==NULL) {
    fprintf(stderr,"ERROR: Could not find infile.\n");
    exit(1);
  }
  
#ifndef BENCHMARK
  strcpy(outname,argv[1]); /* name the outfile */
  strcat(outname,".uncompr"); /* add the suffix '.uncompr' */
  fpo=fopen(outname,"w");
  if (fpo==NULL) {
    fprintf(stderr,"ERROR: Could not open outfile (do you have write permission here?)\n");
    exit(1);
  }
#endif

  fread(&size,sizeof(unsigned int),1,fpi); /* Read size of original file */
  fread(&orgpos,sizeof(unsigned int),1,fpi); /* Read the position of the original string */
  
  in=(unsigned char *)malloc(2*size*sizeof(unsigned char));
  deari=(unsigned char *)malloc(2*size*sizeof(unsigned char));
  derle=(unsigned char *)malloc(2*size*sizeof(unsigned char));
  debw=(unsigned char *)malloc(2*size*sizeof(unsigned char));
  if (!in || !deari || !derle || !debw) {
    fprintf(stderr,"ERROR: Out of memory\n");
    exit(1);
  }

  insize=fread(in, sizeof(unsigned char), 2*size, fpi);
  fclose(fpi);

  outsize=do_deari(insize);
  free(in); /* We are done with 'in' now... */
  do_derle(outsize);
  free(deari); /* We are done with 'deari' now... */
  do_debwe();
  free(derle); /* We are done with 'derle' now... */

#ifdef BENCHMARK
  fwrite(debw, sizeof(unsigned char), size, stdout);
  free(debw); /* We are done with 'debw' now... */
#else
  /* Write the results to file */
  fwrite(debw, sizeof(unsigned char), size, fpo); 
  free(debw); /* We are done with 'debw' now... */
  fclose(fpo);
#endif
    
}

static void do_derle(int rlesize)
{
  unsigned int j,k;
  unsigned int derlepos=0;
  
  /* Do the deRLE coding */
  for (j=0;j<rlesize;) {
    if (deari[j] & 0x80) { /* is bit 7 set? YES! */
      for (k=0;k<(deari[j] & 0x7F);k++)
	derle[derlepos++]=deari[j+1];
      j+=2;
    } else { /* is bit 7 set? NO! */
      memcpy(derle+derlepos,deari+j+1,deari[j]);
      derlepos+=deari[j];
      j+=deari[j]+1;
    }
  }
}

static void do_debwe()
{ 
  unsigned char *L=derle;
  unsigned int *T;
  unsigned int count[256];
  unsigned int total[256];
  unsigned int k,i,sum=0,indx;
  
  T=(unsigned int *)malloc(size*sizeof(unsigned int));
  
  for (k=0;k<256;k++)
    count[k]=0;
  
  for (k=0;k<size;k++) {
    count[L[k]]++;
  }
  
  for (i=0;i<256;i++) {
    total[i] = sum;
    sum += count[i];
    count[i] = 0;
  }
  
  for (i=0;i<size;i++) {
    indx = L[i];
    T[i] = count[indx]+total[indx];
    count[indx]++;
  }

  debw[size-1]=L[orgpos];
  for (k=1;k<size;k++) {
    debw[size-k-1]=L[T[orgpos]];
    orgpos=T[orgpos];
  }
  
  free(T);

}
