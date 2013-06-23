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

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

FILE *fpi,*fpo; 
unsigned int *rot; /* Rotation info per byte */
static unsigned char *in; /* The infile */
unsigned char *bw; /* The B&W:d data */
unsigned char *rle; /* The RLE:d data */
unsigned char *ari; /* The ARI:d data */
static unsigned int size;

static void do_bwe();
static unsigned int do_rle();
unsigned int do_ari(unsigned int insize); /* In "arithmetic.c" */

void compress(int argc, char *argv[]) 
{
  char *filename;
  char outname[1000];
  struct stat buf; 
  unsigned int filesize, outsize;

  if (argc < 2) { 
    fprintf(stderr,"USAGE: %s <FILENAME>\n",argv[0]);
    exit(1);
  }
  filename = argv[1];

  
  /* Find the size of the infile */
  stat(filename,&buf);
  filesize=buf.st_size; 
  
  fpi=fopen(filename,"r"); /* open the infile */
  if (fpi==NULL) {
    fprintf(stderr,"ERROR: Could not find infile %s\n",filename);
    exit(1);
  }
  
  strcpy(outname,filename); /* name the outfile */
  strcat(outname,".compr"); /* add the suffix '.compr' */
  fpo=fopen(outname,"w");
  if (fpo==NULL) {
    fprintf(stderr,"ERROR: Could not open outfile (do you have write permission here?)\n");
    exit(1);
  }

  /* Write the infile size to the outfile */
  fwrite(&filesize,sizeof(unsigned int),1,fpo);

  /* Allocate some memory... */
  in=(unsigned char *)malloc(2*filesize*sizeof(unsigned char));
  bw=(unsigned char *)malloc(filesize*sizeof(unsigned char));
  rot=(unsigned int *)malloc(filesize*sizeof(unsigned int));
  rle=(unsigned char *)malloc(2*filesize*sizeof(unsigned char));
  ari=(unsigned char *)malloc(2*filesize*sizeof(unsigned char));
  if (!in || !bw || !rot || !rle || !ari) {
    fprintf(stderr,"ERROR: Out of memory\n");
    exit(1);
  }

  if (fread(in,sizeof(unsigned char),filesize,fpi)!=filesize) {
    printf("Something is fishy regarding the file size\n");
    exit(1);
  } 

  size=filesize;
  /* Do the Burrows Wheeler encoding */
  do_bwe();
  free(in); /* We can get rid of 'in' now */
  free(rot); /* We can get rid of 'rot' now */
  /* Do the RLE */
  outsize=do_rle();
  free(bw); /* We can get rid of 'bw' now */
  /* Do the arithmetic encoding */
  outsize=do_ari(outsize);
  free(rle); /* We can get rid of 'rle' now */
  /* Write to file */
  fwrite(ari,sizeof(unsigned char),outsize,fpo);
  free(ari); /* We can get rid of 'ari' now */

  fclose(fpi);
  fclose(fpo);

}

/* Compare two strings */
static int compare(const void *a, const void *b) 
{
  unsigned int *first=(unsigned int *)a;
  unsigned int *sec=(unsigned int *)b;
  
  /* Compare strings using memcmp */
  return (memcmp(in+*first,in+*sec,size));
}

static void do_bwe() 
{
  unsigned int i;
  
  /* 
   * Put a copy of the string at the end of the string,
   * this speeds up rotating.
   */
  memcpy(in+size,in,size);
  
  for (i=0;i<size;i++)  /* Initialize 'rot' vector... */
    rot[i]=i;
  
  /* sort the strings using STDLIB qsort */
  qsort(rot,size,sizeof(unsigned int),(*compare));
  
  /* make BW array... */
  for (i=0;i<size;i++) { 
    bw[i]=in[(rot[i]+size-1)%size];
  }
  
  /* Find place of original string, and write it to the outfile*/
  for (i=0;i<size;i++) { 
    if (rot[i]==0) {
      fwrite(&i,sizeof(unsigned int),1,fpo);
      break;
    }
  }
}

static unsigned int do_rle()
{
  unsigned int i, c, rlepos=0;
  unsigned char teck, count;
  /* RLE -- 
   * If the same byte occurs twice or more in s row, put a byte 
   * before to show number of repeats. If different bytes occur in a row
   * put a byte before to show how large the block of unique bytes is.
   * A set (1) bit 7 in the describer byte indicates repeats, a cleared
   * bit 7 indicates block of unique bytes.
   */
  for (i=0;i<size;) {
    c=1;
    teck=bw[i];
    while ((i+c)<size && teck==bw[i+c]) { /* How many repeats? */
      c++;
      if (c>=127)
	break;
    }
    if (c==1) { /* No repeats */
      if ((i+c)<size) {
	while (bw[i+c-1]!=bw[i+c] && bw[i+c]!=bw[i+c+1])
	  c++;
      }

      count=(unsigned char)c & 0x7f;
      rle[rlepos++]=count;
      memcpy(rle+rlepos,bw+i,c);
      rlepos+=c;
      i+=c;
    } else {  /* c repeats */
      if ((rlepos+2)>2*size) {
	fprintf(stderr,"PANIC: RLE buf larger than %d bytes needed (repeat)\n",size);
	exit(1);
      }

      count=(unsigned char)c | 0x80;
      rle[rlepos]=count;
      rle[rlepos+1]=teck;
      rlepos+=2;
      i+=c;
    }
  }      
  
  return rlepos;
}



