#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#define MAX_SIZE 4000000
#define ITER 10

int drop_0xx(unsigned char *in, unsigned char *out, size_t size) {/*1*/
  int left = size;                                                /*2*/
  unsigned short ibuf = 0;                                        /*3*/
  unsigned short obuf = 0;                                        /*4*/
  unsigned char res;                                              /*5*/
  int outoff = 0;                                                 /*6*/

  /* The Macro contains lines 7-14 */ 
#define IN(N) \
  { \
    if ((left -= 3) < 0) break; \
    if (N < 3) ibuf |= *in++ << (8 - N); \
    if ((res = (ibuf >> (16-3))) >= 4) \
      { \
        obuf = (obuf << 3) | res; \
        outoff += 3; \
        if ((outoff & 7) < 3) *out++ = (obuf >> (outoff & 7)); \
      } \
    ibuf <<= 3; \
  }

  for (;;) {                                                      /*15*/
    IN(0)                                                         /*16*/
    IN(5)                                                         /*17*/
    IN(2)                                                         /*18*/
    IN(7)                                                         /*19*/
    IN(4)                                                         /*20*/
    IN(1)                                                         /*21*/
    IN(6)                                                         /*22*/
    IN(3)                                                         /*23*/
      }

  if ((outoff & 7) != 0)                                          /*24*/
    {*out++ = (obuf << (8 - (outoff & 7)));}                      /*25*/
  return ((outoff) >> 3);                                         /*26*/
}
/*==========================================================================*/

static size_t read_data(FILE *in, void *buffer)
{ 
  return fread(buffer, 1, MAX_SIZE, in);
}

static size_t write_data(FILE *out, int size, void *buffer)
{ 
  return fwrite(buffer, 1, size, out);
}

int main(int argc, char *argv[])
{
  FILE *in,*out;
  int i;
  size_t size;
  int outsize,time;
  unsigned char *inbuf, *outbuf, *temp;
  char *outfilename;
  char postfix[] = ".c";
  struct timeval pre,post;
  
  
  /* optional input arg */
  inbuf = malloc(MAX_SIZE);
  outbuf = malloc(MAX_SIZE);

  if (argc > 1) {
    //create_test_data(argv[1]); // for testing purposes
    if ((in = fopen(argv[1], "r")) == NULL) {
      perror(argv[1]);
      exit(1);
    }
    outfilename = malloc(strlen(argv[1]) + strlen(postfix) + 1);
    strcpy(outfilename,argv[1]);
    strcat(outfilename,postfix);
    free(outfilename);
    argv++; argc--;
  }
  else
    in = stdin;
  if (argc != 1) {
    printf("Usage: drop_0XX [infile]\n");
    exit(2);
  }
  size = read_data(in, inbuf);
  gettimeofday(&pre,0);
  for(i=0;i<20;i++){
    temp = inbuf;
    outsize = drop_0xx(temp, outbuf, size*8);
   
  }
  gettimeofday(&post,0);
  time = ((post.tv_sec*1000000+post.tv_usec)-(pre.tv_sec*1000000+pre.tv_usec));
  printf("%d\n", outsize);
  exit(0); 
}
