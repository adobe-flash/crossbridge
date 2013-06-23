#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#define MAX_SIZE 150000

typedef struct linked_list_int {
  int car;
  void* cdr;
} linked_list_int;

typedef struct linked_list_lli {
  linked_list_int* car;
  void* cdr;
} linked_list_lli;

linked_list_int* cons1(int hd, linked_list_int* tl) {
  linked_list_int *res;
  res = (linked_list_int*) malloc(sizeof(linked_list_int));
  res->car=hd;
  res->cdr=tl;
  return res;
}

linked_list_lli* cons2(linked_list_int* hd, linked_list_lli* tl) {
  linked_list_lli *res;
  res = (linked_list_lli*) malloc(sizeof(linked_list_lli));
  res->car=hd;
  res->cdr=tl;
  return res;
}

void free_list1(linked_list_int* list) {
  linked_list_int *next;
   while (list!=NULL) {
    next = list->cdr;
    free(list);
    list = next;
  }
  return;
}

void free_list2(linked_list_lli* list) {
  linked_list_lli *next;
  linked_list_int *hd;
  while (list!=NULL) {
    next = list->cdr;
    hd = list->car;
    free_list1(hd);
    free(list);
    list = next;
  }
  return;
}
/*==========================================================================*/

int read_x_bits(unsigned char* buf, int offset, int x) {			/*1*/
  int byte_offset, bit_offset,present,initbits,res;				/*2*/
  byte_offset = offset >> 3;							/*3*/
  bit_offset = offset & 7;							/*4*/
  initbits = 8-bit_offset;							/*5*/
  present = (int)buf[byte_offset] & 255;					/*6*/
  res = present & ((1 << initbits) - 1);					/*7*/
  while (x > initbits) {							/*8*/
    byte_offset++;								/*9*/
    present = (int)buf[byte_offset] & 255;					/*10*/
    res = (res << 8) | present;							/*11*/
    initbits+=8;								/*12*/
  }
  return res >> (initbits-x);							/*13*/
}

int read_11_bits(unsigned char* buf, int offset) {				/*14*/
  int byte_offset, ioffset,present,initbits,res;				/*15*/
  int b0,b1,b2;									/*16*/
  byte_offset = offset >> 3;							/*17*/
  ioffset = (8-(offset & 7));							/*18*/
  b0 = (int)buf[byte_offset] & 255;						/*19*/
  b1 = (int)buf[byte_offset+1] & 255; 						/*20*/
  res = b0 << (11-ioffset);							/*21*/
  if (ioffset >= 3) {								/*22*/
     return  (res | (b1 >> (ioffset-3)))  &  ((1 << 11) - 1);			/*23*/
  }						
  else
    b2 = (int)buf[byte_offset+2] & 255;						/*24*/
    return (res | (b1 << (3-ioffset)) | b2 >> (ioffset+5)) &  ((1 << 11) - 1);	/*25*/
}

linked_list_lli* five11(unsigned char* inbuf) {					/*26*/
  int offset,nof_packets, nof_channels,i,j,chan;				/*27*/
  linked_list_int *packet=NULL;							/*28*/
  linked_list_lli *res=NULL;							/*29*/
  offset = 0;									/*30*/
  nof_packets=read_x_bits(inbuf,offset,16);					/*31*/
  offset=offset+16;								/*32*/
  for(i=0;i<nof_packets;i++){							/*33*/
    nof_channels=read_x_bits(inbuf,offset,5);					/*34*/
    offset+=5;									/*35*/
    for(j=0;j<nof_channels;j++){						/*36*/
      chan=read_11_bits(inbuf,offset);						/*37*/
      offset=offset+11;								/*38*/
      packet = cons1(chan,packet);						/*39*/
    }
    offset=offset+pad_size(nof_channels);					/*40*/
    res = cons2(packet,res);							/*41*/
    packet=NULL;								/*42*/
  }
    return res;									/*43*/
}
 
int pad_size(int nof_channels) {						/*44*/
  int bits = 5+nof_channels*11;							/*45*/
  return ((8 - (bits & 7)) & 7);						/*46*/
}

int calc_sum2(linked_list_lli* list) {						/*47*/
  linked_list_lli *next;							/*48*/
  linked_list_int *hd;								/*49*/
  int res=0;									/*50*/
  while (list!=NULL) {								/*51*/
    next = list->cdr;								/*52*/
    hd = list->car;								/*53*/
    res+=calc_sum1(hd);								/*54*/
    list = next;								/*55*/
  }
  return res;
}

int calc_sum1(linked_list_int* list) {						/*56*/
  linked_list_int *next;							/*57*/
  int hd,res=0;									/*58*/
  while (list!=NULL) {								/*59*/
    next = list->cdr;								/*60*/
    hd = list->car;								/*61*/
    res+=hd;									/*62*/
    list = next;								/*63*/
  }
  return res;									/*64*/
}

/*==========================================================================*/

static size_t read_data(FILE *in, void *buffer)
{ 
  return fread(buffer, 1, MAX_SIZE, in);
}

int main(int argc, char *argv[])
{
  FILE *in;
  int i;
  size_t size;
  int outsize,time;
  linked_list_lli *list;
  unsigned char *inbuf, *outbuf, *temp;
  
  struct timeval pre,post;
  
  /* optional input arg */
  inbuf = malloc(MAX_SIZE);

  if (argc > 1) {
    //create_test_data(argv[1]); // for testing purposes
    if ((in = fopen(argv[1], "r")) == NULL) {
      perror(argv[1]);
      exit(1);
    }
    argv++; argc--;
  }
  else
    in = stdin;
  if (argc != 1) {
    printf("Usage: five11 [infile]\n");
    exit(2);
  }
  size = read_data(in, inbuf);
  gettimeofday(&pre,0);
#ifdef SMALL_PROBLEM_SIZE
  for(i=0;i<500;i++){
#else
  for(i=0;i<10000;i++){
#endif    
    list = five11(inbuf);
    outsize = calc_sum2(list);
    free_list2(list);
  }
  gettimeofday(&post,0);
  printf("%d\n", outsize);  
  exit(0); 
}
