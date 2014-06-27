
/*!
 ************************************************************************
 * \file  rtp.c
 *
 * \brief
 *    Network Adaptation layer for RTP packets
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *    - Stephan Wenger   <stewe@cs.tu-berlin.de>
 ************************************************************************
 */


/*!

  A quick guide to the basics of the RTP decoder implementation

  This module contains the RTP packetization, de-packetization, and the
  handling of Parameter Sets, see VCEG-N52 and accompanying documents.
  Note: Compound packets are not yet implemented!

  The interface between every NAL (including the RTP NAL) and the VCL is
  based on Slices.  The slice data structure on which the VCL is working
  is defined in the type Slice (in defines.h).  This type contains the
  various fields of the slice header and a partition array, which itself
  contains the data partitions the slice consists of.  When data
  partitioning is not used, then the whole slice bit string is stored
  in partition #0.  When individual partitions are missing, this is
  indicated by the size of the bit strings in the partition array.
  A complete missing slice (e.g. if a Full Slice packet was lost) is
  indicated in a similar way.

  part of the slice structure is the error indication (ei-flag).  The
  Ei-flag is set in such cases in which at least one partition of a slice
  is damaged or missing.When data partitioning is used, it can happen that
  one partition does not contain any symbols but the ei_flag is cleared,
  which indicates the intentional missing of symbols of that partition.
  A typical example for this behaviour is the Intra Slice, which does not
  have symnbols in its type C partition.

  The VCL requests new data to work on through the call of readSliceRTP().
  This function calls the main state machine of this module in ReadRTPpaacket().

  ReadRTPpacket assumes, when called, that in an error free environment
  a complete slice, consisting of one Full Slice RTP packet, or three Partition
  packets of types A, B, C with consecutive sequence numbers, can be read.
  It first interprets any trailing SUPP and Parameter Update (Header) packets.
  Then it reads one video data packet.  Two cases have to be distinguished:

  1. Type A, or Full Slice packet
  In this case, the PictureID and the macroblock mumbers are used to
  identify the potential loss of a slice.  A slice is lost, when the
  StartMB of the newly read slice header is not equal to the current
  state of the decoder
    1.1 Loss detected
      In this case the last packet is unread (fseek back), and a dummy slice
      containing the missing macroblocks is conveyed to the VCL.  At the next
      call of the NAL, the same packet is read again, but this time no packet
      loss is detected by the above algorithm,
    1.2. No loss
      In this case it is checked whether a Full Slice packet or a type A data
      partition was read
        1.2.1 Full Slice
          The Full Slice packet is conveyed to the NAL
        1.2.2 Type A Partition
          The function RTPReadDataPartitionedSlice() is called, which collects
          the remaining type B, C partitions and handles them appropriately.

  Paraneter Update Packets (aka Header packets) are in an SDP-like syntax
  and are interpreted by a simple parser in the function
  RTPInterpretParameterSetPacket()

  Each Slice header contaions the information on which parameter set to be used.
  The function RTPSetImgInp() copies the information of the relevant parameter
  set in the VCL's global variables img-> and inp->  IMPORTANT: any changes
  in the semantics of the img-> and inp-> structure members must be represented
  in this function as well!

  A note to the stream-buffer data structure: The stream buffer always contains
  only the contents of the partition in question, and not the slice/partition
  header.  Decoding has to start at bitoffset 0 (UVLC) or bytreoffset 0 (CABAC).

  The remaining functions should be self-explanatory.

*/

#include "contributors.h"

#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#include "global.h"
#include "errorconcealment.h"
#include "rtp.h"
#include "fmo.h"
#include "sei.h"
#include "memalloc.h"

#ifdef WIN32
#include <Winsock2.h>
#else
#include <netinet/in.h>
#endif

FILE *bits;

int RTPReadPacket (RTPpacket_t *p, FILE *bits);

/*!
 ************************************************************************
 * \brief
 *    Opens the bit stream file named fn
 * \return
 *    none
 ************************************************************************
 */
void OpenRTPFile (char *fn)
{
  if (NULL == (bits=fopen(fn, "rb")))
  {
    snprintf (errortext, ET_SIZE, "Cannot open RTP file '%s'", input->infile);
    error(errortext,500);
  }
}


/*!
 ************************************************************************
 * \brief
 *    Closes the bit stream file
 ************************************************************************
 */
void CloseRTPFile()
{
  fclose (bits);
}


/*!
 ************************************************************************
 * \brief
 *    Fills nalu->buf and nalu->len with the payload of an RTP packet.
 *    Other fields in nalu-> remain uninitialized (will be taken care of
 *    by NALUtoRBSP.
 *
 * \return
 *     4 in case of ok (for compatibility with GetAnnexbNALU)
 *     0 if there is nothing any more to read (EOF)
 *    -1 in case of any error
 *
 ************************************************************************
 */

int GetRTPNALU (NALU_t *nalu)
{
  RTPpacket_t *p;
  int ret;

  if ((p=malloc (sizeof (RTPpacket_t)))== NULL)
    no_mem_exit ("GetRTPNALU-1");
  if ((p->packet=malloc (MAXRTPPACKETSIZE))== NULL)
    no_mem_exit ("GetRTPNALU-2");
  if ((p->payload=malloc (MAXRTPPACKETSIZE))== NULL)
    no_mem_exit ("GetRTPNALU-3");

  ret = RTPReadPacket (p, bits);
  nalu->forbidden_bit = 1;
  nalu->len = 0;

  if (ret < 0)
    return -1;
  if (ret == 0)
    return 0;

  assert (p->paylen < nalu->max_size);

  nalu->len = p->paylen;
  memcpy (nalu->buf, p->payload, p->paylen);
  nalu->forbidden_bit = (nalu->buf[0]>>7) & 1;
  nalu->nal_reference_idc = (nalu->buf[0]>>5) & 3;
  nalu->nal_unit_type = (nalu->buf[0]) & 0x1f;

  free (p->payload);
  free (p->packet);
  free (p);
//  printf ("Got an RTP NALU, len %d, first byte %x\n", nalu->len, nalu->buf[0]);
  return nalu->len;
}



/*!
 *****************************************************************************
 *
 * \brief
 *    DecomposeRTPpacket interprets the RTP packet and writes the various
 *    structure members of the RTPpacket_t structure
 *
 * \return
 *    0 in case of success
 *    negative error code in case of failure
 *
 * \param p
 *    Caller is responsible to allocate enough memory for the generated payload
 *    in parameter->payload. Typically a malloc of paclen-12 bytes is sufficient
 *
 * \par Side effects
 *    none
 *
 * \date
 *    30 Spetember 2001
 *
 * \author
 *    Stephan Wenger   stewe@cs.tu-berlin.de
 *****************************************************************************/

int DecomposeRTPpacket (RTPpacket_t *p)

{
  // consistency check
  assert (p->packlen < 65536 - 28);  // IP, UDP headers
  assert (p->packlen >= 12);         // at least a complete RTP header
  assert (p->payload != NULL);
  assert (p->packet != NULL);

  // Extract header information

  p->v  = (p->packet[0] >> 6) & 0x03;
  p->p  = (p->packet[0] >> 5) & 0x01;
  p->x  = (p->packet[0] >> 4) & 0x01;
  p->cc = (p->packet[0] >> 0) & 0x0F;

  p->m  = (p->packet[1] >> 7) & 0x01;
  p->pt = (p->packet[1] >> 0) & 0x7F;

  memcpy (&p->seq, &p->packet[2], 2);
  p->seq = ntohs((unsigned short)p->seq);

  memcpy (&p->timestamp, &p->packet[4], 4);// change to shifts for unified byte sex
  p->timestamp = ntohl(p->timestamp);
  memcpy (&p->ssrc, &p->packet[8], 4);// change to shifts for unified byte sex
  p->ssrc = ntohl(p->ssrc);

  // header consistency checks
  if (     (p->v != 2)
        || (p->p != 0)
        || (p->x != 0)
        || (p->cc != 0) )
  {
    printf ("DecomposeRTPpacket, RTP header consistency problem, header follows\n");
    DumpRTPHeader (p);
    return -1;
  }
  p->paylen = p->packlen-12;
  memcpy (p->payload, &p->packet[12], p->paylen);
  return 0;
}

/*!
 *****************************************************************************
 *
 * \brief
 *    DumpRTPHeader is a debug tool that dumps a human-readable interpretation
 *    of the RTP header
 *
 * \return
 *    n.a.
 * \param p
 *    the RTP packet to be dumped, after DecompositeRTPpacket()
 *
 * \par Side effects
 *    Debug output to stdout
 *
 * \date
 *    30 Spetember 2001
 *
 * \author
 *    Stephan Wenger   stewe@cs.tu-berlin.de
 *****************************************************************************/

void DumpRTPHeader (RTPpacket_t *p)

{
  int i;
  for (i=0; i< 30; i++)
    printf ("%02x ", p->packet[i]);
  printf ("Version (V): %d\n", p->v);
  printf ("Padding (P): %d\n", p->p);
  printf ("Extension (X): %d\n", p->x);
  printf ("CSRC count (CC): %d\n", p->cc);
  printf ("Marker bit (M): %d\n", p->m);
  printf ("Payload Type (PT): %d\n", p->pt);
  printf ("Sequence Number: %d\n", p->seq);
  printf ("Timestamp: %d\n", p->timestamp);
  printf ("SSRC: %d\n", p->ssrc);
}


/*!
 *****************************************************************************
 *
 * \brief
 *    RTPReadPacket reads one packet from file
 *
 * \return
 *    0: EOF
 *    negative: error
 *    positive: size of RTP packet in bytes
 *
 * \param p
 *    packet data structure, with memory for p->packet allocated
 *
 * \param bits
 *    target file
 *
 * \par Side effects:
 *   - File pointer in bits moved
 *   - p->xxx filled by reading and Decomposepacket()
 *
 * \date
 *    04 November, 2001
 *
 * \author
 *    Stephan Wenger, stewe@cs.tu-berlin.de
 *****************************************************************************/

int RTPReadPacket (RTPpacket_t *p, FILE *bits)
{
  int Filepos, intime;

  assert (p != NULL);
  assert (p->packet != NULL);
  assert (p->payload != NULL);

  Filepos = ftell (bits);
  if (4 != fread (&p->packlen,1, 4, bits))
    {
      return 0;
    }

  if (4 != fread (&intime, 1, 4, bits))
    {
      fseek (bits, Filepos, SEEK_SET);
      printf ("RTPReadPacket: File corruption, could not read Timestamp, exit\n");
      exit (-1);
    }

  assert (p->packlen < MAXRTPPACKETSIZE);

  if (p->packlen != fread (p->packet, 1, p->packlen, bits))
    {
      printf ("RTPReadPacket: File corruption, could not read %d bytes\n", p->packlen);
      exit (-1);    // EOF inidication
    }

  if (DecomposeRTPpacket (p) < 0)
    {
      // this should never happen, hence exit() is ok.  We probably do not want to attempt
      // to decode a packet that obviously wasn't generated by RTP
      printf ("Errors reported by DecomposePacket(), exit\n");
      exit (-700);
    }
    assert (p->pt == H264PAYLOADTYPE);
    assert (p->ssrc == H264SSRC);
  return p->packlen;
}

