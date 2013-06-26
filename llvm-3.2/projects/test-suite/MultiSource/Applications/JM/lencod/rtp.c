
/*!
 *****************************************************************************
 *
 * \file rtp.c
 *
 * \brief
 *    Functions to handle RTP headers and packets per RFC1889 and RTP NAL spec
 *    Functions support little endian systems only (Intel, not Motorola/Sparc)
 *
 * \date
 *    30 September 2001
 *
 * \author
 *    Stephan Wenger   stewe@cs.tu-berlin.de
 *****************************************************************************/

#include <stdlib.h>
#include <assert.h>
#include <memory.h>

#include "global.h"

#include "rtp.h"

#ifdef WIN32
#include <Winsock2.h>
#else
#include <netinet/in.h>
#endif

// A little trick to avoid those horrible #if TRACE all over the source code
#if TRACE
#define SYMTRACESTRING(s) strncpy(sym.tracestring,s,TRACESTRING_SIZE)
#else
#define SYMTRACESTRING(s) // to nothing
#endif


int CurrentRTPTimestamp = 0;      //! The RTP timestamp of the current packet,
                                  //! incremented with all P and I frames
int CurrentRTPSequenceNumber = 0; //! The RTP sequence number of the current packet
                                  //! incremented by one for each sent packet

FILE *f;
/*!
 *****************************************************************************
 *
 * \brief
 *    ComposeRTPpacket composes the complete RTP packet using the various
 *    structure members of the RTPpacket_t structure
 *
 * \return
 *    0 in case of success
 *    negative error code in case of failure
 *
 * \par Parameters
 *    Caller is responsible to allocate enough memory for the generated packet
 *    in parameter->packet. Typically a malloc of 12+paylen bytes is sufficient
 *
 * \par Side effects
 *    none
 *
 * \note
 *    Function contains assert() tests for debug purposes (consistency checks
 *    for RTP header fields
 *
 * \date
 *    30 Spetember 2001
 *
 * \author
 *    Stephan Wenger   stewe@cs.tu-berlin.de
 *****************************************************************************/


int ComposeRTPPacket (RTPpacket_t *p)

{
  unsigned int temp32;
  unsigned short temp16;

  // Consistency checks through assert, only used for debug purposes
  assert (p->v == 2);
  assert (p->p == 0);
  assert (p->x == 0);
  assert (p->cc == 0);    // mixer designers need to change this one
  assert (p->m == 0 || p->m == 1);
  assert (p->pt < 128);
  assert (p->seq < 65536);
  assert (p->payload != NULL);
  assert (p->paylen < 65536 - 40);  // 2**16 -40 for IP/UDP/RTP header
  assert (p->packet != NULL);

  // Compose RTP header, little endian

  p->packet[0] = (byte)
    ( ((p->v  & 0x03) << 6)
    | ((p->p  & 0x01) << 5)
    | ((p->x  & 0x01) << 4)
    | ((p->cc & 0x0F) << 0) );

  p->packet[1] = (byte)
    ( ((p->m  & 0x01) << 7)
    | ((p->pt & 0x7F) << 0) );

  // sequence number, msb first
  temp16 = htons((unsigned short)p->seq);
  memcpy (&p->packet[2], &temp16, 2);  // change to shifts for unified byte sex

  //declare a temporary variable to perform network byte order converson
  temp32 = htonl(p->timestamp);
  memcpy (&p->packet[4], &temp32, 4);  // change to shifts for unified byte sex

  temp32 = htonl(p->ssrc);
  memcpy (&p->packet[8], &temp32, 4);// change to shifts for unified byte sex

  // Copy payload

  memcpy (&p->packet[12], p->payload, p->paylen);
  p->packlen = p->paylen+12;
  return 0;
}



/*!
 *****************************************************************************
 *
 * \brief
 *    WriteRTPPacket writes the supplied RTP packet to the output file
 *
 * \return
 *    0 in case of access
 *    <0 in case of write failure (typically fatal)
 *
 * \param p
 *    the RTP packet to be written (after ComposeRTPPacket() )
 * \param f
 *    output file
 *
 * \date
 *    October 23, 2001
 *
 * \author
 *    Stephan Wenger   stewe@cs.tu-berlin.de
 *****************************************************************************/

int WriteRTPPacket (RTPpacket_t *p, FILE *f)

{
  int intime = -1;

  assert (f != NULL);
  assert (p != NULL);


  if (1 != fwrite (&p->packlen, 4, 1, f))
    return -1;
  if (1 != fwrite (&intime, 4, 1, f))
    return -1;
  if (1 != fwrite (p->packet, p->packlen, 1, f))
    return -1;
  return 0;
}





/*!
 *****************************************************************************
 *
 * \brief
 *    int RTPWriteNALU write a NALU to the RTP file
 *
 * \return
 *    Number of bytes written to output file
 *
 * \par Side effects
 *    Packet written, RTPSequenceNumber and RTPTimestamp updated
 *
 * \date
 *    December 13, 2002
 *
 * \author
 *    Stephan Wenger   stewe@cs.tu-berlin.de
 *****************************************************************************/


int WriteRTPNALU (NALU_t *n)
{
  RTPpacket_t *p;

  assert (f != NULL);
  assert (n != NULL);
  assert (n->len < 65000);

  n->buf[0] = (byte)
    (n->forbidden_bit << 7      |
     n->nal_reference_idc << 5  |
     n->nal_unit_type );

  // Set RTP structure elements and alloca() memory foor the buffers
  if ((p = (RTPpacket_t *) malloc (sizeof (RTPpacket_t))) == NULL)
    no_mem_exit ("RTPWriteNALU-1");
  if ((p->packet = malloc (MAXRTPPACKETSIZE)) == NULL)
    no_mem_exit ("RTPWriteNALU-2");
  if ((p->payload = malloc (MAXRTPPACKETSIZE)) == NULL)
    no_mem_exit ("RTPWriteNALU-3");

  p->v=2;
  p->p=0;
  p->x=0;
  p->cc=0;
  p->m=(n->startcodeprefix_len==4)&1;     // a long startcode of Annex B sets marker bit of RTP
                                          // Not exactly according to the RTP paylaod spec, but
                                          // good enough for now (hopefully).
                                          //! For error resilience work, we need the correct
                                          //! marker bit.  Introduce a nalu->marker and set it in
                                          //! terminate_slice()?
  p->pt=H264PAYLOADTYPE;
  p->seq=CurrentRTPSequenceNumber++;
  p->timestamp=CurrentRTPTimestamp;
  p->ssrc=H264SSRC;
  p->paylen = n->len;
  memcpy (p->payload, n->buf, n->len);

  // Generate complete RTP packet
  if (ComposeRTPPacket (p) < 0)
  {
    printf ("Cannot compose RTP packet, exit\n");
    exit (-1);
  }
  if (WriteRTPPacket (p, f) < 0)
  {
    printf ("Cannot write %d bytes of RTP packet to outfile, exit\n", p->packlen);
    exit (-1);
  }
  free (p->packet);
  free (p->payload);
  free (p);
  return (n->len * 8);
}


/*!
 ********************************************************************************************
 * \brief
 *    RTPUpdateTimestamp: patches the RTP timestamp depending on the TR
 *
 * \param
 *    tr: TRof the following NALUs
 *
 * \return
 *    none.
 *
 ********************************************************************************************
*/


void RTPUpdateTimestamp (int tr)
{
  int delta;
  static int oldtr = -1;

  if (oldtr == -1)            // First invocation
  {
    CurrentRTPTimestamp = 0;  //! This is a violation of the security req. of
                              //! RTP (random timestamp), but easier to debug
    oldtr = 0;
    return;
  }

  /*! The following code assumes a wrap around of TR at 256, and
      needs to be changed as soon as this is no more true.

      The support for B frames is a bit tricky, because it is not easy to distinguish
      between a natural wrap-around of the tr, and the intentional going back of the
      tr because of a B frame.  It is solved here by a heuristic means: It is assumed that
      B frames are never "older" than 10 tr ticks.  Everything higher than 10 is considered
      a wrap around.
  */

  delta = tr - oldtr;

  if (delta < -10)        // wrap-around
    delta+=256;

  CurrentRTPTimestamp += delta * RTP_TR_TIMESTAMP_MULT;
  oldtr = tr;
}


/*!
 ********************************************************************************************
 * \brief
 *    Opens the output file for the RTP packet stream
 *
 * \param Filename
 *    The filename of the file to be opened
 *
 * \return
 *    none.  Function terminates the program in case of an error
 *
 ********************************************************************************************
*/

void OpenRTPFile (char *Filename)
{
  if ((f = fopen (Filename, "wb")) == NULL)
  {
    printf ("Fatal: cannot open bitstream file '%s', exit (-1)\n", Filename);
    exit (-1);
  }
}


/*!
 ********************************************************************************************
 * \brief
 *    Closes the output file for the RTP packet stream
 *
 * \return
 *    none.  Function terminates the program in case of an error
 *
 ********************************************************************************************
*/

void CloseRTPFile ()
{
  fclose(f);
}








#if 0
/*!
 *****************************************************************************
 *
 * \brief
 *    int aggregationRTPWriteBits (int marker) write the Slice header for the RTP NAL
 *
 * \return
 *    Number of bytes written to output file
 *
 * \param marker
 *    marker bit,
 *
 * \par Side effects
 *    Packet written, RTPSequenceNumber and RTPTimestamp updated
 *
 * \date
 *    September 10, 2002
 *
 * \author
 *    Dong Tian   tian@cs.tut.fi
 *****************************************************************************/

int aggregationRTPWriteBits (int Marker, int PacketType, int subPacketType, void * bitstream,
                    int BitStreamLenInByte, FILE *out)
{
  RTPpacket_t *p;
  int offset;

//  printf( "writing aggregation packet...\n");
  assert (out != NULL);
  assert (BitStreamLenInByte < 65000);
  assert (bitstream != NULL);
  assert ((PacketType&0xf) == 4);

  // Set RTP structure elements and alloca() memory foor the buffers
  p = (RTPpacket_t *) alloca (sizeof (RTPpacket_t));
  p->packet=alloca (MAXRTPPACKETSIZE);
  p->payload=alloca (MAXRTPPACKETSIZE);
  p->v=2;
  p->p=0;
  p->x=0;
  p->cc=0;
  p->m=Marker&1;
  p->pt=H264PAYLOADTYPE;
  p->seq=CurrentRTPSequenceNumber++;
  p->timestamp=CurrentRTPTimestamp;
  p->ssrc=H264SSRC;

  offset = 0;
  p->payload[offset++] = PacketType; // This is the first byte of the compound packet

  // FIRST, write the sei message to aggregation packet, if it is available
  if ( HaveAggregationSEI() )
  {
    p->payload[offset++] = sei_message[AGGREGATION_SEI].subPacketType; // this is the first byte of the first subpacket
    *(short*)&(p->payload[offset]) = sei_message[AGGREGATION_SEI].payloadSize;
    offset += 2;
    memcpy (&p->payload[offset], sei_message[AGGREGATION_SEI].data, sei_message[AGGREGATION_SEI].payloadSize);
    offset += sei_message[AGGREGATION_SEI].payloadSize;

    clear_sei_message(AGGREGATION_SEI);
  }

  // SECOND, write other payload to the aggregation packet
  // to do ...

  // LAST, write the slice data to the aggregation packet
  p->payload[offset++] = subPacketType;  // this is the first byte of the second subpacket
  *(short*)&(p->payload[offset]) = BitStreamLenInByte;
  offset += 2;
  memcpy (&p->payload[offset], bitstream, BitStreamLenInByte);
  offset += BitStreamLenInByte;

  p->paylen = offset;  // 1 +3 +seiPayload.payloadSize +3 +BitStreamLenInByte

  // Now the payload is ready, we can ...
  // Generate complete RTP packet
  if (ComposeRTPPacket (p) < 0)
  {
    printf ("Cannot compose RTP packet, exit\n");
    exit (-1);
  }
  if (WriteRTPPacket (p, out) < 0)
  {
    printf ("Cannot write %d bytes of RTP packet to outfile, exit\n", p->packlen);
    exit (-1);
  }
  return (p->packlen);

}


/*!
 *****************************************************************************
 * \isAggregationPacket
 * \brief
 *    Determine if current packet is normal packet or compound packet (aggregation
 *    packet)
 *
 * \return
 *    return TRUE, if it is compound packet.
 *    return FALSE, otherwise.
 *
 * \date
 *    September 10, 2002
 *
 * \author
 *    Dong Tian   tian@cs.tut.fi
 *****************************************************************************/
Boolean isAggregationPacket()
{
  if (HaveAggregationSEI())
  {
    return TRUE;
  }
  // Until Sept 2002, the JM will produce aggregation packet only for some SEI messages

  return FALSE;
}

/*!
 *****************************************************************************
 * \PrepareAggregationSEIMessage
 * \brief
 *    Prepare the aggregation sei message.
 *
 * \date
 *    September 10, 2002
 *
 * \author
 *    Dong Tian   tian@cs.tut.fi
 *****************************************************************************/
void PrepareAggregationSEIMessage()
{
  Boolean has_aggregation_sei_message = FALSE;
  // prepare the sei message here
  // write the spare picture sei payload to the aggregation sei message
  if (seiHasSparePicture && img->type != B_SLICE)
  {
    FinalizeSpareMBMap();
    assert(seiSparePicturePayload.data->byte_pos == seiSparePicturePayload.payloadSize);
    write_sei_message(AGGREGATION_SEI, seiSparePicturePayload.data->streamBuffer, seiSparePicturePayload.payloadSize, SEI_SPARE_PICTURE);
    has_aggregation_sei_message = TRUE;
  }
  // write the sub sequence information sei paylaod to the aggregation sei message
  if (seiHasSubseqInfo)
  {
    FinalizeSubseqInfo(img->layer);
    write_sei_message(AGGREGATION_SEI, seiSubseqInfo[img->layer].data->streamBuffer, seiSubseqInfo[img->layer].payloadSize, SEI_SUBSEQ_INFORMATION);
    ClearSubseqInfoPayload(img->layer);
    has_aggregation_sei_message = TRUE;
  }
  // write the sub sequence layer information sei paylaod to the aggregation sei message
  if (seiHasSubseqLayerInfo && img->number == 0)
  {
    FinalizeSubseqLayerInfo();
    write_sei_message(AGGREGATION_SEI, seiSubseqLayerInfo.data, seiSubseqLayerInfo.payloadSize, SEI_SUBSEQ_LAYER_CHARACTERISTICS);
    seiHasSubseqLayerInfo = FALSE;
    has_aggregation_sei_message = TRUE;
  }
  // write the sub sequence characteristics payload to the aggregation sei message
  if (seiHasSubseqChar)
  {
    FinalizeSubseqChar();
    write_sei_message(AGGREGATION_SEI, seiSubseqChar.data->streamBuffer, seiSubseqChar.payloadSize, SEI_SUBSEQ_CHARACTERISTICS);
    ClearSubseqCharPayload();
    has_aggregation_sei_message = TRUE;
  }
  // write the pan scan rectangle info sei playload to the aggregation sei message
  if (seiHasPanScanRectInfo)
  {
    FinalizePanScanRectInfo();
    write_sei_message(AGGREGATION_SEI, seiPanScanRectInfo.data->streamBuffer, seiPanScanRectInfo.payloadSize, SEI_PANSCAN_RECT);
    ClearPanScanRectInfoPayload();
    has_aggregation_sei_message = TRUE;
  }
  // write the arbitrary (unregistered) info sei playload to the aggregation sei message
  if (seiHasUser_data_unregistered_info)
  {
    FinalizeUser_data_unregistered();
    write_sei_message(AGGREGATION_SEI, seiUser_data_unregistered.data->streamBuffer, seiUser_data_unregistered.payloadSize, SEI_USER_DATA_UNREGISTERED);
    ClearUser_data_unregistered();
    has_aggregation_sei_message = TRUE;
  }
  // write the arbitrary (unregistered) info sei playload to the aggregation sei message
  if (seiHasUser_data_registered_itu_t_t35_info)
  {
    FinalizeUser_data_registered_itu_t_t35();
    write_sei_message(AGGREGATION_SEI, seiUser_data_registered_itu_t_t35.data->streamBuffer, seiUser_data_registered_itu_t_t35.payloadSize, SEI_USER_DATA_REGISTERED_ITU_T_T35);
    ClearUser_data_registered_itu_t_t35();
    has_aggregation_sei_message = TRUE;
  }
  //write RandomAccess info sei payload to the aggregation sei message
  if (seiHasRandomAccess_info)
  {
    FinalizeRandomAccess();
    write_sei_message(AGGREGATION_SEI, seiRandomAccess.data->streamBuffer, seiRandomAccess.payloadSize, SEI_RANDOM_ACCESS_POINT);
    ClearRandomAccess();
    has_aggregation_sei_message = TRUE;
  }
  // more aggregation sei payload is written here...

  // JVT-D099 write the scene information SEI payload
  if (seiHasSceneInformation)
  {
    FinalizeSceneInformation();
    write_sei_message(AGGREGATION_SEI, seiSceneInformation.data->streamBuffer, seiSceneInformation.payloadSize, SEI_SCENE_INFORMATION);
    has_aggregation_sei_message = TRUE;
  }
  // End JVT-D099

  // after all the sei payload is written
  if (has_aggregation_sei_message)
    finalize_sei_message(AGGREGATION_SEI);
}

/*!
 *****************************************************************************
 * \begin_sub_sequence_rtp
 * \brief
 *    do some initialization for sub-sequence under rtp
 *
 * \date
 *    September 10, 2002
 *
 * \author
 *    Dong Tian   tian@cs.tut.fi
 *****************************************************************************/

void begin_sub_sequence_rtp()
{
  if ( input->of_mode != PAR_OF_RTP || input->NumFramesInELSubSeq == 0 )
    return;

  // begin to encode the base layer subseq
  if ( IMG_NUMBER == 0 )
  {
//    printf("begin to encode the base layer subseq\n");
    InitSubseqInfo(0);
    if (1)
      UpdateSubseqChar();
  }
  // begin to encode the enhanced layer subseq
  if ( IMG_NUMBER % (input->NumFramesInELSubSeq+1) == 1 )
  {
//    printf("begin to encode the enhanced layer subseq\n");
    InitSubseqInfo(1);  // init the sub-sequence in the enhanced layer
//    add_dependent_subseq(1);
    if (1)
      UpdateSubseqChar();
  }
}

/*!
 *****************************************************************************
 * \end_sub_sequence_rtp
 * \brief
 *    do nothing
 *
 * \date
 *    September 10, 2002
 *
 * \author
 *    Dong Tian   tian@cs.tut.fi
 *****************************************************************************/
void end_sub_sequence_rtp()
{
  // end of the base layer:
  if ( img->number == input->no_frames-1 )
  {
//    printf("end of encoding the base layer subseq\n");
    CloseSubseqInfo(0);
//    updateSubSequenceBox(0);
  }
  // end of the enhanced layer:
  if ( ((IMG_NUMBER%(input->NumFramesInELSubSeq+1)==0) && (input->successive_Bframe != 0) && (IMG_NUMBER>0)) || // there are B frames
    ((IMG_NUMBER%(input->NumFramesInELSubSeq+1)==input->NumFramesInELSubSeq) && (input->successive_Bframe==0))   // there are no B frames
    )
  {
//    printf("end of encoding the enhanced layer subseq\n");
    CloseSubseqInfo(1);
//    add_dependent_subseq(1);
//    updateSubSequenceBox(1);
  }
}

#endif

