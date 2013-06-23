
/* This program contains the source code from the 1987 CACM
   article by Witten, Neal, and Cleary. */

#include <stdio.h>
#include <stdlib.h>

#define No_of_chars 256                 /* Number of character symbols      */
#define EOF_symbol (No_of_chars+1)      /* Index of EOF symbol              */

#define No_of_symbols (No_of_chars+1)   /* Total number of symbols          */

/* The indata and outdata buffer, defined and allocated in "compress.c" */
extern unsigned char *rle;
extern unsigned char *ari;

/* Positions in buffers */
unsigned int rle_pos;
unsigned int ari_pos;

/* TRANSLATION TABLES BETWEEN CHARACTERS AND SYMBOL INDEXES. */

static int char_to_index[No_of_chars];         /* To index from character          */
static unsigned char index_to_char[No_of_symbols+1]; /* To character from index    */

/* ADAPTIVE SOURCE MODEL */

static int freq[No_of_symbols+1];      /* Symbol frequencies                       */
static int cum_freq[No_of_symbols+1];  /* Cumulative symbol frequencies            */

/* DECLARATIONS USED FOR ARITHMETIC ENCODING AND DECODING */

/* SIZE OF ARITHMETIC CODE VALUES. */

#define Code_value_bits 16              /* Number of bits in a code value   */
typedef long code_value;                /* Type of an arithmetic code value */

#define Top_value (((long)1<<Code_value_bits)-1)      /* Largest code value */


/* HALF AND QUARTER POINTS IN THE CODE VALUE RANGE. */

#define First_qtr (Top_value/4+1)       /* Point after first quarter        */
#define Half	  (2*First_qtr)            /* Point after first half           */
#define Third_qtr (3*First_qtr)         /* Point after third quarter        */


/* CUMULATIVE FREQUENCY TABLE. */

#define Max_frequency 16383             /* Maximum allowed frequency count  */


static void start_model( void );
static void start_encoding( void );
static void encode_symbol(int symbol,int cum_freq[] );
static void update_model(int symbol);
static void done_encoding( void );
static void done_outputing_bits( void );

/* BIT OUTPUT ROUTINES. */

/* THE BIT BUFFER. */

static int buffer;                     /* Bits buffered for output                 */
static int bits_to_go;                 /* Number of bits free in buffer            */


/* INITIALIZE FOR BIT OUTPUT. */

static void start_outputing_bits( void )
{   buffer = 0;                                 /* Buffer is empty to start */
    bits_to_go= 8;                              /* with.                    */
}

/* OUTPUT A BIT. */

static void output_bit( int bit )
{   buffer >>= 1; if (bit) buffer |= 0x80;      /* Put bit in top of buffer.*/
    bits_to_go -= 1;
    if (bits_to_go==0) {                        /* Output buffer if full.   */
        ari[ari_pos++]=(unsigned char)buffer;
	bits_to_go = 8;
    }
}

/* FLUSH OUT THE LAST BITS. */

static void done_outputing_bits( void )
{
  ari[ari_pos++]=(unsigned char)(buffer >> bits_to_go);
}

/* CURRENT STATE OF THE ENCODING. */

code_value low, high;           /* Ends of the current code region          */
long bits_to_follow;            /* Number of opposite bits to output after  */
                                /* the next bit.                            */
/* OUTPUT BITS PLUS FOLLOWING OPPOSITE BITS. */

static void bit_plus_follow( int bit )
{   output_bit(bit);                            /* Output the bit.          */
    while (bits_to_follow>0) {
        output_bit(!bit);                       /* Output bits_to_follow    */
        bits_to_follow -= 1;                    /* opposite bits. Set       */
    }                                           /* bits_to_follow to zero.  */
}

unsigned int do_ari(unsigned int insize)
{
    rle_pos=0;
    ari_pos=0;
    
    start_model();                              /* Set up other modules.    */
    start_outputing_bits();
    start_encoding();
    for (;;) {                                  /* Loop through characters. */
        int ch; int symbol;
        ch = rle[rle_pos++];                    /* Read the next character. */
        if (rle_pos>insize) break;             /* Exit loop when done.     */
        symbol = char_to_index[ch];             /* Translate to an index.   */
        encode_symbol(symbol,cum_freq);         /* Encode that symbol.      */
        update_model(symbol);                   /* Update the model.        */
    }
    encode_symbol(EOF_symbol,cum_freq);         /* Encode the EOF symbol.   */
    done_encoding();                            /* Send the last few bits.  */
    done_outputing_bits();
    return ari_pos;
}

/* ARITHMETIC ENCODING ALGORITHM. */


/* START ENCODING A STREAM OF SYMBOLS. */

static void start_encoding( void )
{   low = 0;                                    /* Full code range.         */
    high = Top_value;
    bits_to_follow = 0;                         /* No bits to follow next.  */
}


/* ENCODE A SYMBOL. */

static void encode_symbol(int symbol,int cum_freq[] )
{   long range;                 /* Size of the current code region          */
    range = (long)(high-low)+1;
    high = low +                                /* Narrow the code region   */
      (range*cum_freq[symbol-1])/cum_freq[0]-1; /* to that allotted to this */
    low = low +                                 /* symbol.                  */
      (range*cum_freq[symbol])/cum_freq[0];
    for (;;) {                                  /* Loop to output bits.     */
        if (high<Half) {
            bit_plus_follow(0);                 /* Output 0 if in low half. */
        }
        else if (low>=Half) {                   /* Output 1 if in high half.*/
            bit_plus_follow(1);
            low -= Half;
            high -= Half;                       /* Subtract offset to top.  */
        }
        else if (low>=First_qtr                 /* Output an opposite bit   */
              && high<Third_qtr) {              /* later if in middle half. */
            bits_to_follow += 1;
            low -= First_qtr;                   /* Subtract offset to middle*/
            high -= First_qtr;
        }
        else break;                             /* Otherwise exit loop.     */
        low = 2*low;
        high = 2*high+1;                        /* Scale up code range.     */
    }
}


/* FINISH ENCODING THE STREAM. */

static void done_encoding( void )
{   bits_to_follow += 1;                        /* Output two bits that     */
    if (low<First_qtr) bit_plus_follow(0);      /* select the quarter that  */
    else bit_plus_follow(1);                    /* the current code range   */
}                                               /* contains.                */


/* THE ADAPTIVE SOURCE MODEL */

/* INITIALIZE THE MODEL. */

static void start_model( void )
{   int i;
    for (i = 0; i<No_of_chars; i++) {           /* Set up tables that       */
        char_to_index[i] = i+1;                 /* translate between symbol */
        index_to_char[i+1] = (unsigned char) i; /* indexes and characters.  */
    }
    for (i = 0; i<=No_of_symbols; i++) {        /* Set up initial frequency */
        freq[i] = 1;                            /* counts to be one for all */
        cum_freq[i] = No_of_symbols-i;          /* symbols.                 */
    }
    freq[0] = 0;                                /* Freq[0] must not be the  */
}                                               /* same as freq[1].         */


/* UPDATE THE MODEL TO ACCOUNT FOR A NEW SYMBOL. */

static void update_model( int symbol )
{   int i;                      /* New index for symbol                     */
    if (cum_freq[0]==Max_frequency) {           /* See if frequency counts  */
        int cum;                                /* are at their maximum.    */
        cum = 0;
        for (i = No_of_symbols; i>=0; i--) {    /* If so, halve all the     */
            freq[i] = (freq[i]+1)/2;            /* counts (keeping them     */
            cum_freq[i] = cum;                  /* non-zero).               */
            cum += freq[i];
        }
    }
    for (i = symbol; freq[i]==freq[i-1]; i--) ; /* Find symbol's new index. */
    if (i<symbol) {
        int ch_i, ch_symbol;
        ch_i = index_to_char[i];                /* Update the translation   */
        ch_symbol = index_to_char[symbol];      /* tables if the symbol has */
        index_to_char[i] = (unsigned char) ch_symbol;           /* moved.                   */
        index_to_char[symbol] = (unsigned char) ch_i;
        char_to_index[ch_i] = symbol;
        char_to_index[ch_symbol] = i;
    }
    freq[i] += 1;                               /* Increment the frequency  */
    while (i>0) {                               /* count for the symbol and */
        i -= 1;                                 /* update the cumulative    */
        cum_freq[i] += 1;                       /* frequencies.             */
    }
}
