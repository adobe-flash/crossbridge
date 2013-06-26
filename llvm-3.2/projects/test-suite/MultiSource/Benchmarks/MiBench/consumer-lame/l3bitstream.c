/**********************************************************************
 * ISO MPEG Audio Subgroup Software Simulation Group (1996)
 * ISO 13818-3 MPEG-2 Audio Encoder - Lower Sampling Frequency Extension
 *
 **********************************************************************/
/*
  Revision History:

  Date        Programmer                Comment
  ==========  ========================= ===============================
  1995/08/06  mc@fivebats.com           created
  1995/09/06  mc@fivebats.com           modified to use formatBitstream
*/

#include <stdlib.h>
#include "lame.h"
#include "l3bitstream.h" /* the public interface */
#include "encoder.h"
#include "quantize.h"
#include "quantize-pvt.h"
#include "formatBitstream.h"
#include "tables.h"
#include <assert.h>
#include "l3bitstream-pvt.h"

static Bit_stream_struc *bs = NULL;

BF_FrameData    *frameData    = NULL;
BF_FrameResults *frameResults = NULL;

int PartHoldersInitialized = 0;

BF_PartHolder *headerPH;
BF_PartHolder *frameSIPH;
BF_PartHolder *channelSIPH[ MAX_CHANNELS ];
BF_PartHolder *spectrumSIPH[ MAX_GRANULES ][ MAX_CHANNELS ];
BF_PartHolder *scaleFactorsPH[ MAX_GRANULES ][ MAX_CHANNELS ];
BF_PartHolder *codedDataPH[ MAX_GRANULES ][ MAX_CHANNELS ];
BF_PartHolder *userSpectrumPH[ MAX_GRANULES ][ MAX_CHANNELS ];
BF_PartHolder *userFrameDataPH;


void putMyBits( u_int val, u_int len )
{
    putbits( bs, val, len );
}

/*
  III_format_bitstream()
  
  This is called after a frame of audio has been quantized and coded.
  It will write the encoded audio to the bitstream. Note that
  from a layer3 encoder's perspective the bit stream is primarily
  a series of main_data() blocks, with header and side information
  inserted at the proper locations to maintain framing. (See Figure A.7
  in the IS).
  */

void
III_format_bitstream( lame_global_flags *gfp,
                      int              bitsPerFrame,
		      int              l3_enc[2][2][576],
		      III_side_info_t  *l3_side,
		      III_scalefac_t   scalefac[2][2],
		      Bit_stream_struc *in_bs)
{
    int gr, ch;
    bs = in_bs;

    if ( frameData == NULL )
    {
	frameData = calloc( 1,sizeof *frameData);
	assert( frameData );
    }
    if ( frameResults == NULL )
    {
	frameResults = calloc( 1,sizeof *frameResults);
	assert( frameResults );
    }

    if ( !PartHoldersInitialized )
    {
	headerPH = BF_newPartHolder( 14 ); 
	frameSIPH = BF_newPartHolder( 12 );

	for ( ch = 0; ch < MAX_CHANNELS; ch++ )
	    channelSIPH[ch] = BF_newPartHolder( 8 );

	for ( gr = 0; gr < MAX_GRANULES; gr++ )	
	    for ( ch = 0; ch < MAX_CHANNELS; ch++ )
	    {
		spectrumSIPH[gr][ch]   = BF_newPartHolder( 32 );
		scaleFactorsPH[gr][ch] = BF_newPartHolder( 64 );
		codedDataPH[gr][ch]    = BF_newPartHolder( 576 );
		userSpectrumPH[gr][ch] = BF_newPartHolder( 4 );
	    }
	userFrameDataPH = BF_newPartHolder( 8 );
	PartHoldersInitialized = 1;
    }

    encodeSideInfo( gfp,l3_side );
    encodeMainData( gfp,l3_enc, l3_side, scalefac );



    drain_into_ancillary_data( l3_side->resvDrain );
    /*
      Put frameData together for the call
      to BitstreamFrame()
    */
    frameData->frameLength = bitsPerFrame;
    frameData->nGranules   = gfp->mode_gr;
    frameData->nChannels   = gfp->stereo;
    frameData->header      = headerPH->part;
    frameData->frameSI     = frameSIPH->part;

    for ( ch = 0; ch < gfp->stereo; ch++ )
	frameData->channelSI[ch] = channelSIPH[ch]->part;

    for ( gr = 0; gr < gfp->mode_gr; gr++ )
	for ( ch = 0; ch < gfp->stereo; ch++ )
	{
	    frameData->spectrumSI[gr][ch]   = spectrumSIPH[gr][ch]->part;
	    frameData->scaleFactors[gr][ch] = scaleFactorsPH[gr][ch]->part;
	    frameData->codedData[gr][ch]    = codedDataPH[gr][ch]->part;
	    frameData->userSpectrum[gr][ch] = userSpectrumPH[gr][ch]->part;
	}
    frameData->userFrameData = userFrameDataPH->part;

    BF_BitstreamFrame( frameData, frameResults );

    /* we set this here -- it will be tested in the next loops iteration */
    l3_side->main_data_begin = frameResults->nextBackPtr;

}

void
III_FlushBitstream(void)
{
    if (PartHoldersInitialized!=0)
		BF_FlushBitstream( frameData, frameResults );
}

static unsigned slen1_tab[16] = { 0, 0, 0, 0, 3, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4 };
static unsigned slen2_tab[16] = { 0, 1, 2, 3, 0, 1, 2, 3, 1, 2, 3, 1, 2, 3, 2, 3 };

static void
encodeMainData( lame_global_flags *gfp,
		int              l3_enc[2][2][576],
		III_side_info_t  *si,
		III_scalefac_t   scalefac[2][2] )
{
    int i, gr, ch, sfb, window;


    for ( gr = 0; gr < gfp->mode_gr; gr++ )
	for ( ch = 0; ch < gfp->stereo; ch++ )
	    scaleFactorsPH[gr][ch]->part->nrEntries = 0;

    for ( gr = 0; gr < gfp->mode_gr; gr++ )
	for ( ch = 0; ch < gfp->stereo; ch++ )
	    codedDataPH[gr][ch]->part->nrEntries = 0;

    if ( gfp->version == 1 )
    {  /* MPEG 1 */
	for ( gr = 0; gr < 2; gr++ )
	{
	    for ( ch = 0; ch < gfp->stereo; ch++ )
	    {
		BF_PartHolder **pph = &scaleFactorsPH[gr][ch];		
		gr_info *gi = &(si->gr[gr].ch[ch].tt);
		unsigned slen1 = slen1_tab[ gi->scalefac_compress ];
		unsigned slen2 = slen2_tab[ gi->scalefac_compress ];
		int *ix = &l3_enc[gr][ch][0];

		if (gi->block_type == SHORT_TYPE)
		{
#ifdef ALLOW_MIXED
		    if ( gi->mixed_block_flag )
		    {
			for ( sfb = 0; sfb < 8; sfb++ )
			    *pph = BF_addEntry( *pph,  scalefac[gr][ch].l[sfb], slen1 );

			for ( sfb = 3; sfb < 6; sfb++ )
			    for ( window = 0; window < 3; window++ )
				*pph = BF_addEntry( *pph,  scalefac[gr][ch].s[sfb][window], slen1 );

			for ( sfb = 6; sfb < 12; sfb++ )
			    for ( window = 0; window < 3; window++ )
				*pph = BF_addEntry( *pph,  scalefac[gr][ch].s[sfb][window], slen2 );

		    }
		    else
#endif
		    {
			for ( sfb = 0; sfb < 6; sfb++ )
			    for ( window = 0; window < 3; window++ )
				*pph = BF_addEntry( *pph,  scalefac[gr][ch].s[sfb][window], slen1 );

			for ( sfb = 6; sfb < 12; sfb++ )
			    for ( window = 0; window < 3; window++ )
				*pph = BF_addEntry( *pph,  scalefac[gr][ch].s[sfb][window], slen2 );
		    }
		}
		else
		{
		    if ( (gr == 0) || (si->scfsi[ch][0] == 0) )
			for ( sfb = 0; sfb < 6; sfb++ )
			    *pph = BF_addEntry( *pph,  scalefac[gr][ch].l[sfb], slen1 );

		    if ( (gr == 0) || (si->scfsi[ch][1] == 0) )
			for ( sfb = 6; sfb < 11; sfb++ )
			    *pph = BF_addEntry( *pph,  scalefac[gr][ch].l[sfb], slen1 );

		    if ( (gr == 0) || (si->scfsi[ch][2] == 0) )
			for ( sfb = 11; sfb < 16; sfb++ )
			    *pph = BF_addEntry( *pph,  scalefac[gr][ch].l[sfb], slen2 );

		    if ( (gr == 0) || (si->scfsi[ch][3] == 0) )
			for ( sfb = 16; sfb < 21; sfb++ )
			    *pph = BF_addEntry( *pph,  scalefac[gr][ch].l[sfb], slen2 );
		}
		Huffmancodebits( &codedDataPH[gr][ch], ix, gi );
	    } /* for ch */
	} /* for gr */
    }
    else
    {  /* MPEG 2 */
	gr = 0;
	for ( ch = 0; ch < gfp->stereo; ch++ )
	{
	    BF_PartHolder **pph = &scaleFactorsPH[gr][ch];		
	    gr_info *gi = &(si->gr[gr].ch[ch].tt);
	    int *ix = &l3_enc[gr][ch][0];
	    int sfb_partition;
	    assert( gi->sfb_partition_table );

	    if (gi->block_type == SHORT_TYPE)
	    {
#ifdef ALLOW_MIXED
		if ( gi->mixed_block_flag )
		{
		    sfb_partition = 0;
		    for ( sfb = 0; sfb < 8; sfb++ )
			*pph = BF_addEntry( *pph,  scalefac[gr][ch].l[sfb], gi->slen[sfb_partition] );

		    for ( sfb = 3, sfb_partition = 1; sfb_partition < 4; sfb_partition++ )
		    {
			int sfbs = gi->sfb_partition_table[ sfb_partition ] / 3;
			int slen = gi->slen[ sfb_partition ];
			for ( i = 0; i < sfbs; i++, sfb++ )
			    for ( window = 0; window < 3; window++ )
				*pph = BF_addEntry( *pph,  scalefac[gr][ch].s[sfb][window], slen );
		    }
		}
		else
#endif
		{
		    for ( sfb = 0, sfb_partition = 0; sfb_partition < 4; sfb_partition++ )
		    {
			int sfbs = gi->sfb_partition_table[ sfb_partition ] / 3;
			int slen = gi->slen[ sfb_partition ];
			for ( i = 0; i < sfbs; i++, sfb++ )
			    for ( window = 0; window < 3; window++ )
				*pph = BF_addEntry( *pph,  scalefac[gr][ch].s[sfb][window], slen );
		    }
		}
	    }
	    else
	    {
		for ( sfb = 0, sfb_partition = 0; sfb_partition < 4; sfb_partition++ )
		{
		    int sfbs = gi->sfb_partition_table[ sfb_partition ];
		    int slen = gi->slen[ sfb_partition ];
		    for ( i = 0; i < sfbs; i++, sfb++ )
			*pph = BF_addEntry( *pph,  scalefac[gr][ch].l[sfb], slen );
		}
	    }



	    Huffmancodebits( &codedDataPH[gr][ch], ix, gi );
	} /* for ch */
    }
} /* main_data */

static unsigned int crc = 0; /* (jo) current crc */

/* (jo) this wrapper function for BF_addEntry() updates also the crc */
static BF_PartHolder *CRC_BF_addEntry( BF_PartHolder *thePH, u_int value, u_int length )
{
   u_int bit = 1 << length;
   
   while((bit >>= 1)){
      crc <<= 1;
      if (!(crc & 0x10000) ^ !(value & bit))
	crc ^= CRC16_POLYNOMIAL;
   }
   crc &= 0xffff;   
   return BF_addEntry(thePH, value, length);
}




static int encodeSideInfo( lame_global_flags *gfp,III_side_info_t  *si )
{
    int gr, ch, scfsi_band, region, window, bits_sent;
    
    crc = 0xffff; /* (jo) init crc16 for error_protection */

    headerPH->part->nrEntries = 0;
    headerPH = BF_addEntry( headerPH, 0xfff,                    12 );
    headerPH = BF_addEntry( headerPH, gfp->version,            1 );
    headerPH = BF_addEntry( headerPH, 1,                        2 );
    headerPH = BF_addEntry( headerPH, !gfp->error_protection,     1 );
    /* (jo) from now on call the CRC_BF_addEntry() wrapper to update crc */
    headerPH = CRC_BF_addEntry( headerPH, gfp->bitrate_index,      4 );
    headerPH = CRC_BF_addEntry( headerPH, gfp->samplerate_index,   2 );
    headerPH = CRC_BF_addEntry( headerPH, gfp->padding,            1 );
    headerPH = CRC_BF_addEntry( headerPH, gfp->extension,          1 );
    headerPH = CRC_BF_addEntry( headerPH, gfp->mode,               2 );
    headerPH = CRC_BF_addEntry( headerPH, gfp->mode_ext,           2 );
    headerPH = CRC_BF_addEntry( headerPH, gfp->copyright,          1 );
    headerPH = CRC_BF_addEntry( headerPH, gfp->original,           1 );
    headerPH = CRC_BF_addEntry( headerPH, gfp->emphasis,           2 );
    
    bits_sent = 32;
   
    /* (jo) see below for BF_addEntry( headerPH, crc, 16 ); */

    frameSIPH->part->nrEntries = 0;

    for (ch = 0; ch < gfp->stereo; ch++ )
	channelSIPH[ch]->part->nrEntries = 0;

    for ( gr = 0; gr < gfp->mode_gr; gr++ )
	for ( ch = 0; ch < gfp->stereo; ch++ )
	    spectrumSIPH[gr][ch]->part->nrEntries = 0;

    if ( gfp->version == 1 )
    {  /* MPEG1 */
	frameSIPH = CRC_BF_addEntry( frameSIPH, si->main_data_begin, 9 );

	if ( gfp->stereo == 2 )
	    frameSIPH = CRC_BF_addEntry( frameSIPH, si->private_bits, 3 );
	else
	    frameSIPH = CRC_BF_addEntry( frameSIPH, si->private_bits, 5 );
	
	for ( ch = 0; ch < gfp->stereo; ch++ )
	    for ( scfsi_band = 0; scfsi_band < 4; scfsi_band++ )
	    {
		BF_PartHolder **pph = &channelSIPH[ch];
		*pph = CRC_BF_addEntry( *pph, si->scfsi[ch][scfsi_band], 1 );
	    }

	for ( gr = 0; gr < 2; gr++ )
	    for ( ch = 0; ch < gfp->stereo; ch++ )
	    {
		BF_PartHolder **pph = &spectrumSIPH[gr][ch];
		gr_info *gi = &(si->gr[gr].ch[ch].tt);
		*pph = CRC_BF_addEntry( *pph, gi->part2_3_length,        12 );
		*pph = CRC_BF_addEntry( *pph, gi->big_values,            9 );
		*pph = CRC_BF_addEntry( *pph, gi->global_gain,           8 );
		*pph = CRC_BF_addEntry( *pph, gi->scalefac_compress,     4 );
		*pph = CRC_BF_addEntry( *pph, gi->window_switching_flag, 1 );

		if ( gi->window_switching_flag )
		{   
		    *pph = CRC_BF_addEntry( *pph, gi->block_type,       2 );
		    *pph = CRC_BF_addEntry( *pph, gi->mixed_block_flag, 1 );

		    for ( region = 0; region < 2; region++ )
			*pph = CRC_BF_addEntry( *pph, gi->table_select[region],  5 );
		    for ( window = 0; window < 3; window++ )
			*pph = CRC_BF_addEntry( *pph, gi->subblock_gain[window], 3 );
		}
		else
		{
		    assert( gi->block_type == NORM_TYPE );
		    for ( region = 0; region < 3; region++ )
			*pph = CRC_BF_addEntry( *pph, gi->table_select[region], 5 );

		    *pph = CRC_BF_addEntry( *pph, gi->region0_count, 4 );
		    *pph = CRC_BF_addEntry( *pph, gi->region1_count, 3 );
		}

		*pph = CRC_BF_addEntry( *pph, gi->preflag,            1 );
		*pph = CRC_BF_addEntry( *pph, gi->scalefac_scale,     1 );
		*pph = CRC_BF_addEntry( *pph, gi->count1table_select, 1 );
	    }

	if ( gfp->stereo == 2 )
	    bits_sent += 256;
	else
	    bits_sent += 136;
    }
    else
    {  /* MPEG2 */
	frameSIPH = CRC_BF_addEntry( frameSIPH, si->main_data_begin, 8 );

	if ( gfp->stereo == 2 )
	    frameSIPH = CRC_BF_addEntry( frameSIPH, si->private_bits, 2 );
	else
	    frameSIPH = CRC_BF_addEntry( frameSIPH, si->private_bits, 1 );
	
	gr = 0;
	for ( ch = 0; ch < gfp->stereo; ch++ )
	{
	    BF_PartHolder **pph = &spectrumSIPH[gr][ch];
	    gr_info *gi = &(si->gr[gr].ch[ch].tt);
	    *pph = CRC_BF_addEntry( *pph, gi->part2_3_length,        12 );
	    *pph = CRC_BF_addEntry( *pph, gi->big_values,            9 );
	    *pph = CRC_BF_addEntry( *pph, gi->global_gain,           8 );
	    *pph = CRC_BF_addEntry( *pph, gi->scalefac_compress,     9 );
	    *pph = CRC_BF_addEntry( *pph, gi->window_switching_flag, 1 );

	    if ( gi->window_switching_flag )
	    {   
		*pph = CRC_BF_addEntry( *pph, gi->block_type,       2 );
		*pph = CRC_BF_addEntry( *pph, gi->mixed_block_flag, 1 );

		for ( region = 0; region < 2; region++ )
		    *pph = CRC_BF_addEntry( *pph, gi->table_select[region],  5 );
		for ( window = 0; window < 3; window++ )
		    *pph = CRC_BF_addEntry( *pph, gi->subblock_gain[window], 3 );
	    }
	    else
	    {
		for ( region = 0; region < 3; region++ )
		    *pph = CRC_BF_addEntry( *pph, gi->table_select[region], 5 );

		*pph = CRC_BF_addEntry( *pph, gi->region0_count, 4 );
		*pph = CRC_BF_addEntry( *pph, gi->region1_count, 3 );
	    }

	    *pph = CRC_BF_addEntry( *pph, gi->scalefac_scale,     1 );
	    *pph = CRC_BF_addEntry( *pph, gi->count1table_select, 1 );
	}
	if ( gfp->stereo == 2 )
	    bits_sent += 136;
	else
	    bits_sent += 72;
    }

    if ( gfp->error_protection )
    {   /* (jo) error_protection: add crc16 information to header */
	headerPH = BF_addEntry( headerPH, crc, 16 );
	bits_sent += 16;
    }

    return bits_sent;
}

/*
  Some combinations of bitrate, Fs, and stereo make it impossible to stuff
  out a frame using just main_data, due to the limited number of bits to
  indicate main_data_length. In these situations, we put stuffing bits into
  the ancillary data...
*/
static void
drain_into_ancillary_data( int lengthInBits )
{
    /*
     */
    int wordsToSend   = lengthInBits / 32;
    int remainingBits = lengthInBits % 32;
    int i;

    /*
      userFrameDataPH->part->nrEntries set by call to write_ancillary_data()
    */
    
    userFrameDataPH->part->nrEntries = 0;
    for ( i = 0; i < wordsToSend; i++ )
	userFrameDataPH = BF_addEntry( userFrameDataPH, 0, 32 );
    if ( remainingBits )
	userFrameDataPH = BF_addEntry( userFrameDataPH, 0, remainingBits );
}

/*
  Note the discussion of huffmancodebits() on pages 28
  and 29 of the IS, as well as the definitions of the side
  information on pages 26 and 27.
  */
static void
Huffmancodebits( BF_PartHolder **pph, int *ix, gr_info *gi )
{
    int L3_huffman_coder_count1( BF_PartHolder **pph, struct huffcodetab *h, int v, int w, int x, int y );

    int region1Start;
    int region2Start;
    int i, bigvalues, count1End;
    int v, w, x, y, bits, cbits, xbits, stuffingBits;
    unsigned int code, ext;
#ifdef DEBUG
    int bvbits, c1bits;
#endif
    int bitsWritten = 0;

    
    /* 1: Write the bigvalues */
    bigvalues = gi->big_values * 2;
    if ( bigvalues )
    {
	if ( !(gi->mixed_block_flag) && (gi->block_type == SHORT_TYPE) )
	{ /* Three short blocks */
	    /*
	      Within each scalefactor band, data is given for successive
	      time windows, beginning with window 0 and ending with window 2.
	      Within each window, the quantized values are then arranged in
	      order of increasing frequency...
	      */
	    int sfb, window, line, start, end;

	    I192_3 *ix_s;
	    
	    ix_s = (I192_3 *) ix;
	    region1Start = 12;
	    region2Start = 576;

	    for ( sfb = 0; sfb < 13; sfb++ )
	    {
		unsigned tableindex = 100;
		start = scalefac_band.s[ sfb ];
		end   = scalefac_band.s[ sfb+1 ];

		if ( start < region1Start )
		    tableindex = gi->table_select[ 0 ];
		else
		    tableindex = gi->table_select[ 1 ];
		assert( tableindex < 32 );

		for ( window = 0; window < 3; window++ )
		    for ( line = start; line < end; line += 2 )
		    {
			x = (*ix_s)[line][window];
			y = (*ix_s)[line + 1][window];
			bits = HuffmanCode( tableindex, x, y, &code, &ext, &cbits, &xbits );
			*pph = BF_addEntry( *pph,  code, cbits );
			*pph = BF_addEntry( *pph,  ext, xbits );
			bitsWritten += bits;
		    }
		
	    }
	}
	else
#ifdef ALLOW_MIXED
	    if ( gi->mixed_block_flag && gi->block_type == SHORT_TYPE )
	    {  /* Mixed blocks long, short */
		int sfb, window, line, start, end;
		unsigned tableindex;
		I192_3 *ix_s;
		
		ix_s = (I192_3 *) ix;

		/* Write the long block region */
		tableindex = gi->table_select[0];
		if ( tableindex )
		    for ( i = 0; i < 36; i += 2 )
		    {
			x = ix[i];
			y = ix[i + 1];
			bits = HuffmanCode( tableindex, x, y, &code, &ext, &cbits, &xbits );
			*pph = BF_addEntry( *pph,  code, cbits );
			*pph = BF_addEntry( *pph,  ext, xbits );
			bitsWritten += bits;
			
		    }
		/* Write the short block region */
		tableindex = gi->table_select[ 1 ];
		assert( tableindex < 32 );

		for ( sfb = 3; sfb < 13; sfb++ )
		{
		    start = scalefac_band.s[ sfb ];
		    end   = scalefac_band.s[ sfb+1 ];           
		    
		    for ( window = 0; window < 3; window++ )
			for ( line = start; line < end; line += 2 )
			{
			    x = (*ix_s)[line][window];
			    y = (*ix_s)[line + 1][window];
			    bits = HuffmanCode( tableindex, x, y, &code, &ext, &cbits, &xbits );
			    *pph = BF_addEntry( *pph,  code, cbits );
			    *pph = BF_addEntry( *pph,  ext, xbits );
			    bitsWritten += bits;
			}
		}

	    }
	    else
#endif
	    { /* Long blocks */
		unsigned scalefac_index = 100;
		
		if ( gi->mixed_block_flag )
		{
		    region1Start = 36;
		    region2Start = 576;
		}
		else
		{
		    scalefac_index = gi->region0_count + 1;
		    assert( scalefac_index < 23 );
		    region1Start = scalefac_band.l[ scalefac_index ];
		    scalefac_index += gi->region1_count + 1;
		    assert( scalefac_index < 23 );    
		    region2Start = scalefac_band.l[ scalefac_index ];
		}

		for ( i = 0; i < bigvalues; i += 2 )
		{
		    unsigned tableindex = 100;
		    /* get table pointer */
		    if ( i < region1Start )
		    {
			tableindex = gi->table_select[0];
		    }
		    else
			if ( i < region2Start )
			{
			    tableindex = gi->table_select[1];
			}
			else
			{
			    tableindex = gi->table_select[2];
			}
		    assert( tableindex < 32 );
		    /* get huffman code */
		    x = ix[i];
		    y = ix[i + 1];

		    if ( tableindex )
		    {
			bits = HuffmanCode( tableindex, x, y, &code, &ext, &cbits, &xbits );
			*pph = BF_addEntry( *pph,  code, cbits );
			*pph = BF_addEntry( *pph,  ext, xbits );
			bitsWritten += bits;
		    }
		}
	    }
    }
#ifdef DEBUG
    bvbits = bitsWritten; 
#endif

    /* 2: Write count1 area */
    assert( (gi->count1table_select < 2) );
    count1End = bigvalues + (gi->count1 * 4);

    assert( count1End <= 576 );

    for ( i = bigvalues; i < count1End; i += 4 )
    {
	v = ix[i];
	w = ix[i+1];
	x = ix[i+2];
	y = ix[i+3];
	bitsWritten += L3_huffman_coder_count1( pph, &ht[gi->count1table_select + 32], v, w, x, y );
    }
#ifdef DEBUG
    c1bits = bitsWritten - bvbits;
#endif
    if ( (stuffingBits = gi->part2_3_length - gi->part2_length - bitsWritten) )
    {
	int stuffingWords = stuffingBits / 32;
	int remainingBits = stuffingBits % 32;

        fprintf(stderr,"opps - adding stuffing bits = %i.\n",stuffingBits);
        fprintf(stderr,"this should not happen...\n");

	/*
	  Due to the nature of the Huffman code
	  tables, we will pad with ones
	*/
	while ( stuffingWords-- )
	    *pph = BF_addEntry( *pph, ~(u_int)0, 32 );
	if ( remainingBits )
	    *pph = BF_addEntry( *pph, ~(u_int)0, remainingBits );
	bitsWritten += stuffingBits;
    }
    assert( bitsWritten == (int)(gi->part2_3_length - gi->part2_length) );
#ifdef DEBUG
    fprintf(stderr, "## %d Huffman bits written (%02d + %02d), part2_length = %d, part2_3_length = %d, %d stuffing ##\n",
	    bitsWritten, bvbits, c1bits, gi->part2_length, gi->part2_3_length, stuffingBits );
#endif
}

int
abs_and_sign( int *x )
{
    if ( *x > 0 )
	return 0;
    *x *= -1;
    return 1;
}

int
L3_huffman_coder_count1( BF_PartHolder **pph, struct huffcodetab *h, int v, int w, int x, int y )
{
    HUFFBITS huffbits;
    unsigned int signv, signw, signx, signy, p;
    int len;
    int totalBits = 0;
    
    signv = abs_and_sign( &v );
    signw = abs_and_sign( &w );
    signx = abs_and_sign( &x );
    signy = abs_and_sign( &y );
    
    /* bug fix from Leonid A. Kulakov 9/1999:*/
    p = (v << 3) + (w << 2) + (x << 1) + y;  

    huffbits = h->table[p];
    len = h->hlen[ p ];
    *pph = BF_addEntry(*pph, huffbits, len);
    totalBits= 0;
#if 0
    if ( v )
    {
	*pph = BF_addEntry( *pph,  signv, 1 );
	totalBits += 1;
    }
    if ( w )
    {
	*pph = BF_addEntry( *pph,  signw, 1 );
	totalBits += 1;
    }

    if ( x )
    {
	*pph = BF_addEntry( *pph,  signx, 1 );
	totalBits += 1;
    }
    if ( y )
    {
	*pph = BF_addEntry( *pph,  signy, 1 );
	totalBits += 1;
    }
#endif   

    p=0;
    if ( v ) {
	p = signv;
	++totalBits;
    }

    if ( w ){
	p = 2*p + signw;
	++totalBits;
    }

    if ( x ) {
	p = 2*p + signx;
	++totalBits;
    }

    if ( y ) {
	p = 2*p + signy;
	++totalBits;
    }

    *pph = BF_addEntry(*pph, p, totalBits);

    return totalBits+len;  
}

/*
  Implements the pseudocode of page 98 of the IS
  */
int
HuffmanCode( int table_select, int x, int y, unsigned int *code, unsigned int *ext, int *cbits, int *xbits )
{
    unsigned signx, signy, linbitsx, linbitsy, linbits, idx;
    struct huffcodetab *h;

    *cbits = 0;
    *xbits = 0;
    *code  = 0;
    *ext   = 0;
    
    if ( table_select == 0 )
	return 0;
    
    signx = abs_and_sign( &x );
    signy = abs_and_sign( &y );
    h = &(ht[table_select]);

    if ( table_select > 15 )
    { /* ESC-table is used */
      linbits = h->xlen;
      linbitsx = linbitsy = 0;
	if ( x > 14 )
	{
	    linbitsx = x - 15;
	    assert( linbitsx <= h->linmax );
	    x = 15;
	}
	if ( y > 14 )
	{
	    linbitsy = y - 15;
	    assert( linbitsy <= h->linmax );
	    y = 15;
	}
	idx = x * 16 + y;
	*code = h->table[idx];
        *cbits = h->hlen[ idx ];
	if ( x > 14 )
	{
	    *ext |= linbitsx;
	    *xbits += linbits;
	}
	if ( x != 0 )
	{
	    *ext <<= 1;
	    *ext |= signx;
	    *xbits += 1;
	}
	if ( y > 14 )
	{
	    *ext <<= linbits;
	    *ext |= linbitsy;
	    *xbits += linbits;
	}
	if ( y != 0 )
	{
	    *ext <<= 1;
	    *ext |= signy;
	    *xbits += 1;
	}
    }
    else
    { /* No ESC-words */
	idx = x * 16 + y;
	*code = h->table[idx];
	*cbits += h->hlen[ idx ];
	if ( x != 0 )
	{
	    *code <<= 1;
	    *code |= signx;
	    *cbits += 1;
	}
	if ( y != 0 )
	{
	    *code <<= 1;
	    *code |= signy;
            *cbits += 1;
	}
    }
    assert( *cbits <= 32 );
    assert( *xbits <= 32 );
    return *cbits + *xbits;
}
