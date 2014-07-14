/*********************************************************************
  Copyright (c) 1995 ISO/IEC JTC1 SC29 WG1, All Rights Reserved
  formatBitstream.c
**********************************************************************/
/*
  Revision History:

  Date        Programmer                Comment
  ==========  ========================= ===============================
  1995/09/06  mc@fivebats.com           created
  1995/09/18  mc@fivebats.com           bugfix: WriteMainDataBits
  1995/09/20  mc@fivebats.com           bugfix: store_side_info
*/

#include "formatBitstream.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/* globals */
static int BitCount       = 0;
static int ThisFrameSize  = 0;
static int BitsRemaining  = 0;

void InitFormatBitStream(void)
{
	BitCount		= 0;
	ThisFrameSize	= 0;
	BitsRemaining	= 0;
}

/* forward declarations */
static int store_side_info( BF_FrameData *frameInfo );
static int main_data( BF_FrameData *frameInfo, BF_FrameResults *results );
static int side_queue_elements( int *forwardFrameLength, int *forwardSILength );
static void free_side_queues(void);
static void WriteMainDataBits( u_int val,u_int nbits,BF_FrameResults *results );

/*
  BitStreamFrame is the public interface to the bitstream
  formatting package. It writes one frame of main data per call.

  Assumptions:
  - The back pointer is zero on the first call
  - An integral number of bytes is written each frame

  You should be able to change the frame length, side info
  length, #channels, #granules on a frame-by-frame basis.

  See formatBitstream.h for more information about the data
  structures and the bitstream syntax.
*/
static int elements, forwardFrameLength, forwardSILength; 
void
BF_BitstreamFrame( BF_FrameData *frameInfo, BF_FrameResults *results )
{
  /*    int elements, forwardFrameLength, forwardSILength; */

    assert( frameInfo->nGranules <= MAX_GRANULES );
    assert( frameInfo->nChannels <= MAX_CHANNELS );

    /* save SI and compute its length */
    results->SILength = store_side_info( frameInfo );

    /* write the main data, inserting SI to maintain framing */
    results->mainDataLength = main_data( frameInfo, results );

    /*
      Caller must ensure that back SI and main data are
      an integral number of bytes, since the back pointer
      can only point to a byte boundary and this code
      does not add stuffing bits
    */
    assert( (BitsRemaining % 8) == 0 );

    /* calculate nextBackPointer */
    elements = side_queue_elements( &forwardFrameLength, &forwardSILength );
    results->nextBackPtr = (BitsRemaining / 8) + (forwardFrameLength / 8) - (forwardSILength / 8);
}

/*
  FlushBitstream writes zeros into main data
  until all queued headers are written. The
  queue data buffers are also freed.
*/
void
BF_FlushBitstream( BF_FrameData *frameInfo, BF_FrameResults *results )
{
  /*    int elements, forwardFrameLength, forwardSILength; */

    if ( elements )
    {
      int bitsRemaining = forwardFrameLength - forwardSILength;
      int wordsRemaining = bitsRemaining / 32;
      while ( wordsRemaining-- ) {
	WriteMainDataBits( 0, 32, results );
      }
      WriteMainDataBits( 0, (bitsRemaining % 32), results );	
    }
    

    results->mainDataLength = forwardFrameLength - forwardSILength;
    results->SILength       = forwardSILength;
    results->nextBackPtr    = 0;

    /* reclaim queue space */
    free_side_queues();

    /* reinitialize globals */
    BitCount       = 0;
    ThisFrameSize  = 0;
    BitsRemaining  = 0;    
    return;
}

int
BF_PartLength( BF_BitstreamPart *part )
{
    BF_BitstreamElement *ep = part->element;
    u_int i;
	int	bits=0;

    for ( i = 0; i < part->nrEntries; i++, ep++ )
		bits += ep->length;
    return bits;
}


/*
  The following is all private to this file
*/

typedef struct
{
    int frameLength;
    int SILength;
    int nGranules;
    int nChannels;
    BF_PartHolder *headerPH;
    BF_PartHolder *frameSIPH;
    BF_PartHolder *channelSIPH[MAX_CHANNELS];
    BF_PartHolder *spectrumSIPH[MAX_GRANULES][MAX_CHANNELS];
} MYSideInfo;

static MYSideInfo *get_side_info(void);
static int write_side_info(void);
typedef int (*PartWriteFcnPtr)( BF_BitstreamPart *part, BF_FrameResults *results );


static int
writePartMainData( BF_BitstreamPart *part, BF_FrameResults *results )
{
    BF_BitstreamElement *ep;
    u_int	i;
	int		bits=0;

    assert( results );
    assert( part );

    ep = part->element;
    for ( i = 0; i < part->nrEntries; i++, ep++ )
    {
		WriteMainDataBits( ep->value, ep->length, results );
		bits += ep->length;
    }
    return bits;
}

static int
writePartSideInfo( BF_BitstreamPart *part, BF_FrameResults *results )
{
    BF_BitstreamElement *ep;
    u_int	i;
	int		bits=0;

    assert( part );

    ep = part->element;
    for ( i = 0; i < part->nrEntries; i++, ep++ )
    {
		putMyBits( ep->value, ep->length );
		bits += ep->length;
    }
    return bits;
}

static int
main_data( BF_FrameData *fi, BF_FrameResults *results )
{
    int gr, ch, bits;
    PartWriteFcnPtr wp = writePartMainData;
    bits = 0;
    results->mainDataLength = 0;

    for ( gr = 0; gr < fi->nGranules; gr++ )
	for ( ch = 0; ch < fi->nChannels; ch++ )
	{
	    bits += (*wp)( fi->scaleFactors[gr][ch], results );
	    bits += (*wp)( fi->codedData[gr][ch],    results );
	    bits += (*wp)( fi->userSpectrum[gr][ch], results );
	}
    bits += (*wp)( fi->userFrameData, results );
    return bits;
}

/*
  This is a wrapper around PutBits() that makes sure that the
  framing header and side info are inserted at the proper
  locations
*/

static void
WriteMainDataBits( u_int val,
		   u_int nbits,
		   BF_FrameResults *results )
{
    assert( nbits <= 32 );
    if ( nbits == 0 )
	return;
    if ( BitCount == ThisFrameSize )
    {
	BitCount = write_side_info();
	BitsRemaining = ThisFrameSize - BitCount;
    }
    if ( nbits > (u_int)BitsRemaining )
    {
	unsigned extra = val >> (nbits - BitsRemaining);
	nbits -= BitsRemaining;
	putMyBits( extra, BitsRemaining );
	BitCount = write_side_info();
	BitsRemaining = ThisFrameSize - BitCount;
	putMyBits( val, nbits );
    }
    else
	putMyBits( val, nbits );
    BitCount += nbits;
    BitsRemaining -= nbits;
    assert( BitCount <= ThisFrameSize );
    assert( BitsRemaining >= 0 );
    assert( (BitCount + BitsRemaining) == ThisFrameSize );
}


static int
write_side_info(void)
{
    MYSideInfo *si;
    int bits, ch, gr;
    PartWriteFcnPtr wp = writePartSideInfo;

    bits = 0;
    si = get_side_info();
    ThisFrameSize = si->frameLength;
    bits += (*wp)( si->headerPH->part,  NULL );
    bits += (*wp)( si->frameSIPH->part, NULL );

    for ( ch = 0; ch < si->nChannels; ch++ )
	bits += (*wp)( si->channelSIPH[ch]->part, NULL );

    for ( gr = 0; gr < si->nGranules; gr++ )
	for ( ch = 0; ch < si->nChannels; ch++ )
	    bits += (*wp)( si->spectrumSIPH[gr][ch]->part, NULL );
    return bits;
}

typedef struct side_info_link
{
    struct side_info_link *next;
    MYSideInfo           side_info;
} side_info_link;

static struct side_info_link *side_queue_head   = NULL;
static struct side_info_link *side_queue_free   = NULL;

static void free_side_info_link( side_info_link *l );

static int
side_queue_elements( int *frameLength, int *SILength )
{
    int elements = 0;
    side_info_link *l;

    *frameLength = 0;
    *SILength    = 0;

    for ( l = side_queue_head; l; l = l->next )
    {
	elements++;
	*frameLength += l->side_info.frameLength;
	*SILength    += l->side_info.SILength;
    }
    return elements;
}

static int
store_side_info( BF_FrameData *info )
{
    int ch, gr;
    side_info_link *l;
    /* obtain a side_info_link to store info */
    side_info_link *f = side_queue_free;
    int bits = 0;

    if ( f == NULL )
    { /* must allocate another */
#ifdef DEBUG
	static int n_si = 0;
	n_si += 1;
	fprintf( stderr, "allocating side_info_link number %d\n", n_si );
#endif
	l = (side_info_link *) calloc( 1, sizeof(side_info_link) );
	if ( l == NULL )
	{
	    fprintf( stderr, "cannot allocate side_info_link" );
	    exit( 1);
	}
	l->next = NULL;
	l->side_info.headerPH  = BF_newPartHolder( info->header->nrEntries );
	l->side_info.frameSIPH = BF_newPartHolder( info->frameSI->nrEntries );
	for ( ch = 0; ch < info->nChannels; ch++ )
	    l->side_info.channelSIPH[ch] = BF_newPartHolder( info->channelSI[ch]->nrEntries );
	for ( gr = 0; gr < info->nGranules; gr++ )
	    for ( ch = 0; ch < info->nChannels; ch++ )
		l->side_info.spectrumSIPH[gr][ch] = BF_newPartHolder( info->spectrumSI[gr][ch]->nrEntries );
	
    }
    else
    { /* remove from the free list */
	side_queue_free = f->next;
	f->next = NULL;
	l = f;
    }
    /* copy data */
    l->side_info.frameLength = info->frameLength;
    l->side_info.nGranules   = info->nGranules;
    l->side_info.nChannels   = info->nChannels;
    l->side_info.headerPH    = BF_LoadHolderFromBitstreamPart( l->side_info.headerPH,  info->header );
    l->side_info.frameSIPH   = BF_LoadHolderFromBitstreamPart( l->side_info.frameSIPH, info->frameSI );

    bits += BF_PartLength( info->header );
    bits += BF_PartLength( info->frameSI );

    for ( ch = 0; ch < info->nChannels; ch++ )
    {
	l->side_info.channelSIPH[ch] = BF_LoadHolderFromBitstreamPart( l->side_info.channelSIPH[ch],
								       info->channelSI[ch] );
	bits += BF_PartLength( info->channelSI[ch] );
    }

    for ( gr = 0; gr < info->nGranules; gr++ )
	for ( ch = 0; ch < info->nChannels; ch++ )
	{
	    l->side_info.spectrumSIPH[gr][ch] = BF_LoadHolderFromBitstreamPart( l->side_info.spectrumSIPH[gr][ch],
										info->spectrumSI[gr][ch] );
	    bits += BF_PartLength( info->spectrumSI[gr][ch] );
	}
    l->side_info.SILength = bits;
    /* place at end of queue */
    f = side_queue_head;
    if ( f == NULL )
    {  /* empty queue */
	side_queue_head = l;
    }
    else
    { /* find last element */
	while ( f->next )
	    f = f->next;
	f->next = l;
    }
    return bits;
}

static MYSideInfo*
get_side_info(void)
{
    side_info_link *f = side_queue_free;
    side_info_link *l = side_queue_head;
    
    /*
      If we stop here it means you didn't provide enough
      headers to support the amount of main data that was
      written.
    */
    assert( l );
    
    /* update queue head */
    side_queue_head = l->next;

    /*
      Append l to the free list. You can continue
      to use it until store_side_info is called
      again, which will not happen again for this
      frame.
    */
    side_queue_free = l;
    l->next = f;
    return &l->side_info;
}

static void
free_side_queues(void)
{
    side_info_link *l, *next;
    
    for ( l = side_queue_head; l; l = next )
    {
	next = l->next;
	free_side_info_link( l );
    }
    side_queue_head = NULL;

    for ( l = side_queue_free; l; l = next )
    {
	next = l->next;
	free_side_info_link( l );
    }
    side_queue_free = NULL;
}

static void
free_side_info_link( side_info_link *l )
{
    int gr, ch;

    l->side_info.headerPH  = BF_freePartHolder( l->side_info.headerPH );
    l->side_info.frameSIPH = BF_freePartHolder( l->side_info.frameSIPH );

    for ( ch = 0; ch < l->side_info.nChannels; ch++ )
	l->side_info.channelSIPH[ch] = BF_freePartHolder( l->side_info.channelSIPH[ch] );

    for ( gr = 0; gr < l->side_info.nGranules; gr++ )
	for ( ch = 0; ch < l->side_info.nChannels; ch++ )
	    l->side_info.spectrumSIPH[gr][ch] = BF_freePartHolder( l->side_info.spectrumSIPH[gr][ch] );

    free( l );
}
/*
  Allocate a new holder of a given size
*/
BF_PartHolder *BF_newPartHolder( int max_elements )
{
    BF_PartHolder *newPH    = (BF_PartHolder*) calloc( 1, sizeof(BF_PartHolder) );
    assert( newPH );
    newPH->max_elements  = max_elements;
    newPH->part          = (BF_BitstreamPart*) calloc( 1, sizeof(BF_BitstreamPart) );
    assert( newPH->part );
    newPH->part->element = (BF_BitstreamElement*) calloc( max_elements, sizeof(BF_BitstreamElement) );
    if (max_elements>0) assert( newPH->part->element );
    newPH->part->nrEntries = 0;
    return newPH;
}

BF_PartHolder *BF_NewHolderFromBitstreamPart( BF_BitstreamPart *thePart )
{
    BF_PartHolder *newHolder = BF_newPartHolder( thePart->nrEntries );
    return BF_LoadHolderFromBitstreamPart( newHolder, thePart );
}

BF_PartHolder *BF_LoadHolderFromBitstreamPart( BF_PartHolder *theHolder, BF_BitstreamPart *thePart )
{
    BF_BitstreamElement *pElem;
    u_int i;

    theHolder->part->nrEntries = 0;
    for ( i = 0; i < thePart->nrEntries; i++ )
    {
	pElem = &(thePart->element[i]);
	theHolder = BF_addElement( theHolder, pElem );
    }
    return theHolder;
}

/*
  Grow or shrink a part holder. Always creates a new
  one of the right length and frees the old one after
  copying the data.
*/
BF_PartHolder *BF_resizePartHolder( BF_PartHolder *oldPH, int max_elements )
{
    int elems, i;
    BF_PartHolder *newPH;

#ifdef DEBUG
    fprintf( stderr, "Resizing part holder from %d to %d\n",
	     oldPH->max_elements, max_elements );
#endif
    /* create new holder of the right length */
    newPH = BF_newPartHolder( max_elements );

    /* copy values from old to new */
    elems = (oldPH->max_elements > max_elements) ? max_elements : oldPH->max_elements;
    newPH->part->nrEntries = elems;
    for ( i = 0; i < elems; i++ )
	newPH->part->element[i] = oldPH->part->element[i];

    /* free old holder */
    BF_freePartHolder( oldPH );
    
    return newPH;
}

BF_PartHolder *BF_freePartHolder( BF_PartHolder *thePH )
{
    free( thePH->part->element );
    free( thePH->part );
    free( thePH );
    return NULL;
}

/*
  Add theElement to thePH, growing the holder if
  necessary. Returns ptr to the holder, which may
  not be the one you called it with!
*/
BF_PartHolder *BF_addElement( BF_PartHolder *thePH, BF_BitstreamElement *theElement )
{
    BF_PartHolder *retPH = thePH;
    int needed_entries = thePH->part->nrEntries + 1;
    int extraPad = 8;  /* add this many more if we need to resize */

    /* grow if necessary */
    if ( needed_entries > thePH->max_elements )
	retPH = BF_resizePartHolder( thePH, needed_entries + extraPad );

    /* copy the data */
    retPH->part->element[retPH->part->nrEntries++] = *theElement;
    return retPH;
}

/*
  Add a bit value and length to the element list in thePH
*/
BF_PartHolder *BF_addEntry( BF_PartHolder *thePH, u_int value, u_int length )
{
    BF_BitstreamElement myElement;
    myElement.value  = value;
    myElement.length = length;

    if ( length )
		return BF_addElement( thePH, &myElement );
    else
		return thePH;
}
