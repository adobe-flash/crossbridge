
/*!
 **************************************************************************************
 * \file
 *    nal.c
 * \brief
 *    Handles the operations on converting String of Data Bits (SODB)
 *    to Raw Byte Sequence Payload (RBSP), and then
 *    onto Encapsulate Byte Sequence Payload (EBSP).
 *  \date 14 June 2002
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *      - Shankar Regunathan                  <shanre@microsoft.de>
 *      - Stephan Wenger                      <stewe@cs.tu-berlin.de>
 ***************************************************************************************
 */


#include "contributors.h"

#include <stdlib.h>
#include <assert.h>
#include <memory.h>

#include "global.h"

 /*!
 ************************************************************************
 * \brief
 *    Converts String Of Data Bits (SODB) to Raw Byte Sequence
 *    Packet (RBSP)
 * \param currStream
 *        Bitstream which contains data bits.
 * \return None
 * \note currStream is byte-aligned at the end of this function
 *
 ************************************************************************
*/

static byte *NAL_Payload_buffer;

void SODBtoRBSP(Bitstream *currStream)
{
  currStream->byte_buf <<= 1;
  currStream->byte_buf |= 1;
  currStream->bits_to_go--;
  currStream->byte_buf <<= currStream->bits_to_go;
  currStream->streamBuffer[currStream->byte_pos++] = currStream->byte_buf;
  currStream->bits_to_go = 8;
  currStream->byte_buf = 0;
}


/*!
************************************************************************
*  \brief
*     This function converts a RBSP payload to an EBSP payload
*
*  \param streamBuffer
*       pointer to data bits
*  \param begin_bytepos
*            The byte position after start-code, after which stuffing to
*            prevent start-code emulation begins.
*  \param end_bytepos
*           Size of streamBuffer in bytes.
*  \param min_num_bytes
*           Minimum number of bytes in payload. Should be 0 for VLC entropy
*           coding mode. Determines number of stuffed words for CABAC mode.
*  \return
*           Size of streamBuffer after stuffing.
*  \note
*      NAL_Payload_buffer is used as temporary buffer to store data.
*
*
************************************************************************
*/

int RBSPtoEBSP(byte *streamBuffer, int begin_bytepos, int end_bytepos, int min_num_bytes)
{

  int i, j, count;

  memcpy(&NAL_Payload_buffer[begin_bytepos],&streamBuffer[begin_bytepos], (end_bytepos - begin_bytepos) * sizeof(unsigned char));

  count = 0;
  j = begin_bytepos;
  for(i = begin_bytepos; i < end_bytepos; i++)
  {
    if(count == ZEROBYTES_SHORTSTARTCODE && !(NAL_Payload_buffer[i] & 0xFC))
    {
      streamBuffer[j] = 0x03;
      j++;
      count = 0;
    }
    streamBuffer[j] = NAL_Payload_buffer[i];
    if(NAL_Payload_buffer[i] == 0x00)
      count++;
    else
      count = 0;
    j++;
  }

  for (i = 0; i< (min_num_bytes - end_bytepos); i+=3 )
  {
    streamBuffer[j]   = 0x00; // CABAC zero word
    streamBuffer[j+1] = 0x00;
    streamBuffer[j+2] = 0x03;
    j += 3;
    stats->bit_use_stuffingBits[img->type]+=16;
  }
  return j;
}

 /*!
 ************************************************************************
 * \brief
 *    Initializes NAL module (allocates NAL_Payload_buffer)
 ************************************************************************
*/

void AllocNalPayloadBuffer()
{
  const int buffer_size = ((input->img_width+img->auto_crop_right) * (input->img_height+img->auto_crop_bottom) * 5); // AH 190202: There can be data expansion with
                                                          // low QP values. So, we make sure that buffer
                                                          // does not overflow. 4 is probably safe multiplier.
  FreeNalPayloadBuffer();

  NAL_Payload_buffer = (byte *) calloc(buffer_size, sizeof(byte));
  assert (NAL_Payload_buffer != NULL);
}


 /*!
 ************************************************************************
 * \brief
 *   Finits NAL module (frees NAL_Payload_buffer)
 ************************************************************************
*/

void FreeNalPayloadBuffer()
{
  if(NAL_Payload_buffer)
  {
    free(NAL_Payload_buffer);
    NAL_Payload_buffer=NULL;
  }
}
