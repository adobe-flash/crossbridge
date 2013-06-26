
/*!
 ************************************************************************
 * \file  nal_part.c
 *
 * \brief
 *    Network Adaptation layer for partition file
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *    - Tobias Oelbaum <oelbaum@hhi.de, oelbaum@drehvial.de>
 ************************************************************************
 */

#include <string.h>

#include "contributors.h"
#include "global.h"
#include "elements.h"

int assignSE2partition[][SE_MAX_ELEMENTS] =
{
  // 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19  // elementnumber (no not uncomment)
  {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },   //!< all elements in one partition no data partitioning
  {  0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 2, 2, 2, 2, 0, 0, 0, 0 }    //!< three partitions per slice
};

int PartitionMode;

/*!
 ************************************************************************
 * \brief
 *    Resets the entries in the bitstream struct
 ************************************************************************
 */
void free_Partition(Bitstream *currStream)
{
  byte *buf = currStream->streamBuffer;

  currStream->bitstream_length = 0;
  currStream->frame_bitoffset = 0;
  currStream->ei_flag =0;
  memset (buf, 0x00, MAX_CODED_FRAME_SIZE);
}
