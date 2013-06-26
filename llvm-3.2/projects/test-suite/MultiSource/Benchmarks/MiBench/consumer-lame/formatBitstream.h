#ifndef _FORMAT_BITSTREAM_H
#define _FORMAT_BITSTREAM_H
/*********************************************************************
  Copyright (c) 1995 ISO/IEC JTC1 SC29 WG1, All Rights Reserved
  formatBitstream.h
**********************************************************************/

/*
  Revision History:

  Date        Programmer                Comment
  ==========  ========================= ===============================
  1995/09/06  mc@fivebats.com           created

*/

#include "machine.h"

#ifndef MAX_CHANNELS
#define MAX_CHANNELS 2
#endif

#ifndef MAX_GRANULES
#define MAX_GRANULES 2
#endif

/*
  This is the prototype for the function pointer you must
  provide to write bits to the bitstream. It should write
  'length' bits from 'value,' msb first. Bits in value are
  assumed to be right-justified.
*/
void putMyBits( u_int value, u_int length );

/*
  A BitstreamElement contains encoded data
  to be written to the bitstream.
  'length' bits of 'value' will be written to
  the bitstream msb-first.
*/
typedef struct
{
    u_int value;
    u_short length;
} BF_BitstreamElement;

/*
  A BitstreamPart contains a group
  of 'nrEntries' of BitstreamElements.
  Each BitstreamElement will be written
  to the bitstream in the order it appears
  in the 'element' array.
*/
typedef struct
{
    u_int              nrEntries;
    BF_BitstreamElement *element;
} BF_BitstreamPart;

/*
  This structure contains all the information needed by the
  bitstream formatter to encode one frame of data. You must
  fill this out and provide a pointer to it when you call
  the formatter.
  Maintainers: If you add or remove part of the side
  information, you will have to update the routines that
  make local copies of that information (in formatBitstream.c)
*/

typedef struct BF_FrameData
{
    int              frameLength;
    int              nGranules;
    int              nChannels;
    BF_BitstreamPart *header;
    BF_BitstreamPart *frameSI;
    BF_BitstreamPart *channelSI[MAX_CHANNELS];
    BF_BitstreamPart *spectrumSI[MAX_GRANULES][MAX_CHANNELS];
    BF_BitstreamPart *scaleFactors[MAX_GRANULES][MAX_CHANNELS];
    BF_BitstreamPart *codedData[MAX_GRANULES][MAX_CHANNELS];
    BF_BitstreamPart *userSpectrum[MAX_GRANULES][MAX_CHANNELS];
    BF_BitstreamPart *userFrameData;
} BF_FrameData;

/*
  This structure contains information provided by
  the bitstream formatter. You can use this to
  check to see if your code agrees with the results
  of the call to the formatter.
*/
typedef struct BF_FrameResults
{
    int SILength;
    int mainDataLength;
    int nextBackPtr;
} BF_FrameResults;


/*
  public functions in formatBitstream.c
*/


/* Initialize the bitstream */
void InitFormatBitStream(void);

/* count the bits in a BitstreamPart */
int  BF_PartLength( BF_BitstreamPart *part );

/* encode a frame of audio and write it to your bitstream */
void BF_BitstreamFrame( BF_FrameData *frameInfo, BF_FrameResults *results );

/* write any remaining frames to the bitstream, padding with zeros */
void BF_FlushBitstream( BF_FrameData *frameInfo, BF_FrameResults *results );



typedef struct BF_PartHolder
{
    int              max_elements;
    BF_BitstreamPart *part;
} BF_PartHolder;

BF_PartHolder *BF_newPartHolder( int max_elements );
BF_PartHolder *BF_resizePartHolder( BF_PartHolder *oldPH, int max_elements );
BF_PartHolder *BF_addElement( BF_PartHolder *thePH, BF_BitstreamElement *theElement );
BF_PartHolder *BF_addEntry( BF_PartHolder *thePH, u_int value, u_int length );
BF_PartHolder *BF_NewHolderFromBitstreamPart( BF_BitstreamPart *thePart );
BF_PartHolder *BF_LoadHolderFromBitstreamPart( BF_PartHolder *theHolder, BF_BitstreamPart *thePart );
BF_PartHolder *BF_freePartHolder( BF_PartHolder *thePH );

#endif  /* #ifndef _FORMAT_BITSTREAM_H */
