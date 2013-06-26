#include "util.h"
#include <assert.h>

/***********************************************************************
*
*  Global Variable Definitions
*
***********************************************************************/


/* 1: MPEG-1, 0: MPEG-2 LSF, 1995-07-11 shn */
FLOAT8  s_freq_table[2][4] = {{22.05, 24, 16, 0}, {44.1, 48, 32, 0}};

/* 1: MPEG-1, 0: MPEG-2 LSF, 1995-07-11 shn */
int     bitrate_table[2][15] = {
          {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160},
          {0,32,40,48,56,64,80,96,112,128,160,192,224,256,320}};


enum byte_order NativeByteOrder = order_unknown;

/***********************************************************************
*
*  Global Function Definitions
*
***********************************************************************/


/***********************************************************************
 * compute bitsperframe and mean_bits for a layer III frame 
 **********************************************************************/
void getframebits(lame_global_flags *gfp,int *bitsPerFrame, int *mean_bits) {
  int whole_SpF;
  FLOAT8 bit_rate,samp;
  int bitsPerSlot;
  int sideinfo_len;
  
  samp =      gfp->out_samplerate/1000.0;
  bit_rate = bitrate_table[gfp->version][gfp->bitrate_index];
  bitsPerSlot = 8;

  /* determine the mean bitrate for main data */
  sideinfo_len = 32;
  if ( gfp->version == 1 )
    {   /* MPEG 1 */
      if ( gfp->stereo == 1 )
	sideinfo_len += 136;
      else
	sideinfo_len += 256;
    }
  else
    {   /* MPEG 2 */
      if ( gfp->stereo == 1 )
	sideinfo_len += 72;
      else
	sideinfo_len += 136;
    }
  
  if (gfp->error_protection) sideinfo_len += 16;
  
  /* -f fast-math option causes some strange rounding here, be carefull: */  
  whole_SpF = floor( (gfp->framesize /samp)*(bit_rate /  (FLOAT8)bitsPerSlot) + 1e-9);
  *bitsPerFrame = 8 * whole_SpF + (gfp->padding * 8);
  *mean_bits = (*bitsPerFrame - sideinfo_len) / gfp->mode_gr;
}




void display_bitrates(FILE *out_fh)
{
  int index,version;

  version = 1;
  fprintf(out_fh,"\n");
  fprintf(out_fh,"MPEG1 samplerates(kHz): 32 44.1 48 \n");

  fprintf(out_fh,"bitrates(kbs): ");
  for (index=1;index<15;index++) {
    fprintf(out_fh,"%i ",bitrate_table[version][index]);
  }
  fprintf(out_fh,"\n");
  
  
  version = 0;
  fprintf(out_fh,"\n");
  fprintf(out_fh,"MPEG2 samplerates(kHz): 16 22.05 24 \n");
  fprintf(out_fh,"bitrates(kbs): ");
  for (index=1;index<15;index++) {
    fprintf(out_fh,"%i ",bitrate_table[version][index]);
  }
  fprintf(out_fh,"\n");
}


int BitrateIndex(
int bRate,        /* legal rates from 32 to 448 */
int version,      /* MPEG-1 or MPEG-2 LSF */
int samplerate)   /* convert bitrate in kbps to index */
{
int     index = 0;
int     found = 0;

    while(!found && index<15)   {
        if(bitrate_table[version][index] == bRate)
            found = 1;
        else
            ++index;
    }
    if(found)
        return(index);
    else {
        fprintf(stderr,"Bitrate %dkbs not legal for %iHz output sampling.\n",
                bRate, samplerate);
        return(-1);     /* Error! */
    }
}

int SmpFrqIndex(  /* convert samp frq in Hz to index */
long sRate,             /* legal rates 16000, 22050, 24000, 32000, 44100, 48000 */
int  *version)
{
	/* Assign default value */
	*version=0;

    if (sRate == 44100L) {
        *version = 1; return(0);
    }
    else if (sRate == 48000L) {
        *version = 1; return(1);
    }
    else if (sRate == 32000L) {
        *version = 1; return(2);
    }
    else if (sRate == 24000L) {
        *version = 0; return(1);
    }
    else if (sRate == 22050L) {
        *version = 0; return(0);
    }
    else if (sRate == 16000L) {
        *version = 0; return(2);
    }
    else {
        fprintf(stderr, "SmpFrqIndex: %ldHz is not a legal sample rate\n", sRate);
        return(-1);     /* Error! */
    }
}

/*******************************************************************************
*
*  Allocate number of bytes of memory equal to "block".
*
*******************************************************************************/
/* exit(0) changed to exit(1) on memory allocation
 * error -- 1999/06 Alvaro Martinez Echevarria */

void  *mem_alloc(unsigned long block, char *item)
{

    void    *ptr;

    /* what kind of shit does ISO put out?  */
    ptr = (void *) malloc((size_t) block /* <<1 */ ); /* allocate twice as much memory as needed. fixes dodgy
					    memory problem on most systems */


    if (ptr != NULL) {
        memset(ptr, 0, (size_t) block);
    } else {
        fprintf(stderr,"Unable to allocate %s\n", item);
        exit(1);
    }
    return(ptr);
}



/*****************************************************************************
*
*  Routines to determine byte order and swap bytes
*
*****************************************************************************/

enum byte_order DetermineByteOrder(void)
{
    char s[ sizeof(int) + 1 ];
    union
    {
        int longval;
        char charval[ sizeof(int) ];
    } probe;
    probe.longval = 0x41424344L;  /* ABCD in ASCII */
    strncpy( s, probe.charval, sizeof(int) );
    s[ sizeof(int) ] = '\0';
    assert (sizeof(int) == 4);
    /* fprintf( stderr, "byte order is %s\n", s ); */
    if ( strcmp(s, "ABCD") == 0 )
        return order_bigEndian;
    else
        if ( strcmp(s, "DCBA") == 0 )
            return order_littleEndian;
        else
            return order_unknown;
}

void SwapBytesInWords( short *loc, int words )
{
    int i;
    short thisval;
    char *dst, *src;
    src = (char *) &thisval;
    for ( i = 0; i < words; i++ )
    {
        thisval = *loc;
        dst = (char *) loc++;
        dst[0] = src[1];
        dst[1] = src[0];
    }
}






/*****************************************************************************
*
*  bit_stream.c package
*  Author:  Jean-Georges Fritsch, C-Cube Microsystems
*
*****************************************************************************/

/********************************************************************
  This package provides functions to write (exclusive or read)
  information from (exclusive or to) the bit stream.

  If the bit stream is opened in read mode only the get functions are
  available. If the bit stream is opened in write mode only the put
  functions are available.
********************************************************************/

/*alloc_buffer();      open and initialize the buffer;                    */
/*desalloc_buffer();   empty and close the buffer                         */
/*back_track_buffer();     goes back N bits in the buffer                 */
/*unsigned int get1bit();  read 1 bit from the bit stream                 */
/*unsigned long look_ahead(); grep the next N bits in the bit stream without*/
/*                            changing the buffer pointer                   */
/*putbits(); write N bits from the bit stream */
/*int seek_sync(); return 1 if a sync word was found in the bit stream      */
/*                 otherwise returns 0                                      */



void empty_buffer(Bit_stream_struc *bs)
{
   int minimum=1+bs->buf_byte_idx;    /* end of the buffer to empty */
   if (bs->buf_size-minimum <= 0) return;
   bs->buf_byte_idx = bs->buf_size -1;
   bs->buf_bit_idx = 8;

   bs->buf[bs->buf_byte_idx] = 0;  /* what does this do? */

}
int copy_buffer(char *buffer,int size,Bit_stream_struc *bs)
{
  int i,j=0;
  if (size!=0 && (bs->buf_size-1 - bs->buf_byte_idx) > size ) return -1;
  for (i=bs->buf_size-1 ; i > bs->buf_byte_idx ; (i-- ))
    buffer[j++]=bs->buf[i];
  assert(j == (bs->buf_size-1 - bs->buf_byte_idx));
  empty_buffer(bs);  /* empty buffer, (changes bs->buf_size) */
  return j;
}





void init_bit_stream_w(Bit_stream_struc* bs)
{
   alloc_buffer(bs, BUFFER_SIZE);
   bs->buf_byte_idx = BUFFER_SIZE-1;
   bs->buf_bit_idx=8;
   bs->totbit=0;
}


/*open and initialize the buffer; */
void alloc_buffer(
Bit_stream_struc *bs,   /* bit stream structure */
int size)
{
   bs->buf = (unsigned char *)
	mem_alloc((unsigned long) (size * sizeof(unsigned char)), "buffer");
   bs->buf_size = size;
}

/*empty and close the buffer */
void desalloc_buffer(Bit_stream_struc *bs)   /* bit stream structure */
{
   free(bs->buf);
}

int putmask[9]={0x0, 0x1, 0x3, 0x7, 0xf, 0x1f, 0x3f, 0x7f, 0xff};


/*write N bits into the bit stream */
void putbits(
Bit_stream_struc *bs,   /* bit stream structure */
unsigned int val,       /* val to write into the buffer */
int N)                  /* number of bits of val */
{
 register int j = N;
 register int k, tmp;

 if (N > MAX_LENGTH)
    fprintf(stderr,"Cannot read or write more than %d bits at a time.\n", MAX_LENGTH);

 bs->totbit += N;
 while (j > 0) {
   k = Min(j, bs->buf_bit_idx);
   tmp = val >> (j-k);
   bs->buf[bs->buf_byte_idx] |= (tmp&putmask[k]) << (bs->buf_bit_idx-k);
   bs->buf_bit_idx -= k;
   if (!bs->buf_bit_idx) {
       bs->buf_bit_idx = 8;
       bs->buf_byte_idx--;
       assert(bs->buf_byte_idx >= 0);
       bs->buf[bs->buf_byte_idx] = 0;
   }
   j -= k;
 }
}



/*****************************************************************************
*
*  End of bit_stream.c package
*
*****************************************************************************/

