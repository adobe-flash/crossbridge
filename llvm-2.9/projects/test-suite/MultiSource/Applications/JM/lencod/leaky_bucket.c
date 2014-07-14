
/*!
 ***************************************************************************
 * \file leaky_bucket.c
 *
 * \brief
 *    calculate Leaky Buffer parameters
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *    - Shankar Regunathan                   <shanre@microsoft.com>
 ***************************************************************************
 */
#include <stdlib.h>

#include "contributors.h"
#include "global.h"

#ifdef _LEAKYBUCKET_

//long Bit_Buffer[20000];
unsigned long total_frame_buffer = 0;


/*!
 ***********************************************************************
 * \brief
 *   Function to get Leaky Bucket rates from rate file
 * \param NumberLeakyBuckets
 *    Number of Leaky Bucket Parameters
 * \param Rmin
 *    Rate values for each Bucket.
 * \return
 *    returns 1 if successful; else returns zero.
 * \para SideEffects
 *     None.
 * \para Notes
 *     Failure if LeakyBucketRate is missing or if it does not have
 *     the correct number of entries.
 * \author
 *    Shankar Regunathan                   shanre@microsoft.com
 *  \date
 *      December 06, 2001.
 ***********************************************************************
 */

int get_LeakyBucketRate(unsigned long NumberLeakyBuckets, unsigned long *Rmin)
{
  FILE *f;
  unsigned long i, buf;

  if((f = fopen(input->LeakyBucketRateFile, "r")) == NULL)
  {
    printf(" LeakyBucketRate File does not exist. Using rate calculated from avg. rate \n");
    return 0;
  }

  for(i=0; i<NumberLeakyBuckets; i++)
  {
    if(1 != fscanf(f, "%lu", &buf))
    {
      printf(" Leaky BucketRateFile does not have valid entries.\n Using rate calculated from avg. rate \n");
      fclose (f);
      return 0;
    }
    Rmin[i] = buf;
  }
  fclose (f);
  return 1;
}
/*!
 ***********************************************************************
 * \brief
 *   Writes one unsigned long word in big endian order to a file.
 * \param dw
 *    Value to be written
 * \param fp
 *    File pointer
 * \return
 *    None.
 * \para SideEffects
 *     None.
 * \author
 *    Shankar Regunathan                   shanre@microsoft.com
 *  \date
 *      December 06, 2001.
 ***********************************************************************
 */

void PutBigDoubleWord(unsigned long dw, FILE *fp)
{
  fputc((dw >> 0x18) & 0xFF, fp);
  fputc((dw >> 0x10) & 0xFF, fp);
  fputc((dw >> 0x08) & 0xFF, fp);
  fputc(dw & 0xFF, fp);
}

/*!
 ***********************************************************************
 * \brief
 *   Stores the Leaky BucketParameters in file input->LeakyBucketParamFile.
 * \param NumberLeakyBuckets
 *    Number of LeakyBuckets.
 * \param Rmin
 *    Rate values of the buckets.
 * \param Bmin
 *    Minimum buffer values of the buckets.
 *  \param Fmin
 *     Minimum initial buffer fullness of the buckets
 * \return
 *    None.
 * \para
 *    Returns error if LeakyBucketParamFile cannot be opened.
 * \para SideEffects
 *     Prints the LeakyBucket Parameters in standard output.
 * \author
 *    Shankar Regunathan                   shanre@microsoft.com
 *  \date
 *      December 06, 2001.
 ***********************************************************************
 */


void write_buffer(unsigned long NumberLeakyBuckets, unsigned long Rmin[], unsigned long Bmin[], unsigned long Fmin[])
{
  FILE *outf;
  unsigned long iBucket;

  if ((outf=fopen(input->LeakyBucketParamFile,"wb"))==NULL)
  {
    snprintf(errortext, ET_SIZE, "Error open file lk %s  \n",input->LeakyBucketParamFile);
    error(errortext,1);
  }

  PutBigDoubleWord(NumberLeakyBuckets, outf);
  for(iBucket =0; iBucket < NumberLeakyBuckets; iBucket++)
  {
    //assert(Rmin[iBucket]<4294967296); //Overflow should be corrected already.
    //assert(Bmin[iBucket]<4294967296);
    //assert(Fmin[iBucket]<4294967296);
    PutBigDoubleWord(Rmin[iBucket], outf);
    PutBigDoubleWord(Bmin[iBucket], outf);
    PutBigDoubleWord(Fmin[iBucket], outf);
  }
  fclose(outf);
}

/*!
 ***********************************************************************
 * \brief
 *    Sorts the rate array in ascending order.
 * \param NumberLeakyBuckets
 *    Number of LeakyBuckets.
 * \param Rmin
 *    Rate values of the buckets.
 * \return
 *    None.
 * \author
 *    Shankar Regunathan                   shanre@microsoft.com
 *  \date
 *      December 06, 2001.
 ***********************************************************************
 */


void Sort(unsigned long NumberLeakyBuckets, unsigned long *Rmin)
{
  unsigned long i, j;
  unsigned long temp;
  for(i=0; i< NumberLeakyBuckets-1; i++)
  {
    for(j=i+1; j<NumberLeakyBuckets; j++)
    {
      if(Rmin[i] > Rmin[j]) {
        temp = Rmin[i];
        Rmin[i] = Rmin[j];
        Rmin[j] = temp;
      }
    }
  }
}

/*!
 ***********************************************************************
 * \brief
 *    Main Routine to calculate Leaky Buffer parameters
 * \param NumberLeakyBuckets
 *    None.
 * \return
 *    None.
 * \author
 *    Shankar Regunathan                   shanre@microsoft.com
 *  \date
 *      December 06, 2001.
 ***********************************************************************
 */

void calc_buffer()
{
  unsigned long AvgRate, TotalRate, NumberLeakyBuckets;
  long *buffer_frame, minB;
  unsigned long iBucket, iFrame,  FrameIndex = 0;
  long maxBuffer, actualBuffer, InitFullness, iChannelRate;
  unsigned long *Rmin, *Bmin, *Fmin;

  fprintf(stdout,"-------------------------------------------------------------------------------\n");
  printf(" Total Frames:  %ld (%d) \n", total_frame_buffer, input->no_frames);
  NumberLeakyBuckets = (unsigned long) input->NumberLeakyBuckets;
  buffer_frame = calloc(total_frame_buffer+1, sizeof(long));
  if(!buffer_frame)
    no_mem_exit("init_buffer: buffer_frame");
  Rmin = calloc(NumberLeakyBuckets, sizeof(unsigned long));
  if(!Rmin)
    no_mem_exit("init_buffer: Rmin");
  Bmin = calloc(NumberLeakyBuckets, sizeof(unsigned long));
  if(!Bmin)
    no_mem_exit("init_buffer: Bmin");
  Fmin = calloc(NumberLeakyBuckets, sizeof(unsigned long));
  if(!Fmin)
    no_mem_exit("init_buffer: Fmin");

  TotalRate = 0;
  for(iFrame=0; iFrame < total_frame_buffer; iFrame++)
  {
    TotalRate += (unsigned long) Bit_Buffer[iFrame];
  }
  AvgRate = (unsigned long) ((float) TotalRate/ total_frame_buffer);

  if(1 != get_LeakyBucketRate(NumberLeakyBuckets, Rmin))
  { /* if rate file is not present, use default calculated from avg.rate */
    for(iBucket=0; iBucket < NumberLeakyBuckets; iBucket++)
    {
      if(iBucket == 0)
        Rmin[iBucket] = (unsigned long)((float) AvgRate * img->framerate)/(input->jumpd+1); /* convert bits/frame to bits/second */
      else
        Rmin[iBucket] = (unsigned long) ((float) Rmin[iBucket-1] + (AvgRate/4) * (img->framerate) / (input->jumpd+1));
    }
  }
  Sort(NumberLeakyBuckets, Rmin);

  maxBuffer = AvgRate * 20; /* any initialization is good. */
  for(iBucket=0; iBucket< NumberLeakyBuckets; iBucket++)
  {
    iChannelRate = (long) (Rmin[iBucket] * (input->jumpd+1)/(img->framerate)); /* converts bits/second to bits/frame */
    /* To calculate initial buffer size */
    InitFullness = maxBuffer; /* set Initial Fullness to be buffer size */
    buffer_frame[0] = InitFullness;
    minB = maxBuffer;

    for(iFrame=0; iFrame<total_frame_buffer ; iFrame++)
    {
      buffer_frame[iFrame] = buffer_frame[iFrame] - Bit_Buffer[iFrame];
      if(buffer_frame[iFrame] < minB)
      {
        minB = buffer_frame[iFrame];
        FrameIndex = iFrame;
      }

      buffer_frame[iFrame+1] = buffer_frame[iFrame] + iChannelRate;
      if(buffer_frame[iFrame+1] > maxBuffer)
        buffer_frame[iFrame+1] = maxBuffer;
    }
    actualBuffer = (maxBuffer - minB);

    /* To calculate initial buffer Fullness */
    InitFullness = Bit_Buffer[0];
    buffer_frame[0] = InitFullness;
    for(iFrame=0; iFrame < FrameIndex+1; iFrame++)
    {
      buffer_frame[iFrame] = buffer_frame[iFrame] - Bit_Buffer[iFrame];
      if(buffer_frame[iFrame] < 0) {
        InitFullness -= buffer_frame[iFrame];
        buffer_frame[iFrame] = 0;
      }
      buffer_frame[iFrame+1] = buffer_frame[iFrame] + iChannelRate;
      if(buffer_frame[iFrame+1] > actualBuffer)
        break;
    }
    Bmin[iBucket] = (unsigned long) actualBuffer;
    Fmin[iBucket] = (unsigned long) InitFullness;
  }

  write_buffer(NumberLeakyBuckets, Rmin, Bmin, Fmin);

  free(buffer_frame);
  free(Rmin);
  free(Bmin);
  free(Fmin);
  return;
}
#endif

