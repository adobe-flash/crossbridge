
/*!
 ***************************************************************************
 * \file ratectl.c
 *
 * \brief
 *    Rate Control algorithm
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *     - Siwei Ma             <swma@jdl.ac.cn>
 *     - Zhengguo LI          <ezgli@lit.a-star.edu.sg>
 *     - Athanasios Leontaris <aleon@dolby.com>
 *
 * \date
 *   16 Jan. 2003
 **************************************************************************
 */

#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <memory.h>
#include <limits.h>

#include "global.h"
#include "ratectl.h"
#include "rc_quadratic.h"

static const float THETA = 1.3636F;
static const float OMEGA = 0.9F;
static const float MINVALUE = 4.0F;
/*!
 *************************************************************************************
 * \brief
 *    Dynamically allocate memory needed for rate control
 *
 *************************************************************************************
 */
void rc_alloc( rc_quadratic **prc )
{
  int rcBufSize = img->FrameSizeInMbs/input->basicunit;
  rc_quadratic *lprc;

  (*prc) = (rc_quadratic *) malloc ( sizeof( rc_quadratic ) );
  if (NULL==(*prc))
  {
    no_mem_exit("init_global_buffers: (*prc)");
  }
  lprc = *prc;

  lprc->PreviousFrameMAD = 1.0;
  lprc->CurrentFrameMAD = 1.0;
  lprc->Pprev_bits = 0;
  lprc->Iprev_bits = 0;
  lprc->Target = 0;
  lprc->TargetField = 0;
  lprc->LowerBound = 0;
  lprc->UpperBound1 = INT_MAX;
  lprc->UpperBound2 = INT_MAX;
  lprc->Wp = 0.0;
  lprc->Wb = 0.0;
  lprc->PAveFrameQP   = input->qp0;
  lprc->m_Qc          = lprc->PAveFrameQP;
  lprc->FieldQPBuffer = lprc->PAveFrameQP;
  lprc->FrameQPBuffer = lprc->PAveFrameQP;
  lprc->PAverageQp    = lprc->PAveFrameQP;
  lprc->MyInitialQp   = lprc->PAveFrameQP;

  lprc->RC_MAX_QUANT = 51;
  lprc->RC_MIN_QUANT = -img->bitdepth_luma_qp_scale;//clipping

  lprc->BUPFMAD = (double*) calloc ((rcBufSize), sizeof (double));
  if (NULL==lprc->BUPFMAD)
  {
    no_mem_exit("rc_alloc: lprc->BUPFMAD");
  }

  lprc->BUCFMAD = (double*) calloc ((rcBufSize), sizeof (double));
  if (NULL==lprc->BUCFMAD)
  {
    no_mem_exit("rc_alloc: lprc->BUCFMAD");
  }

  lprc->FCBUCFMAD = (double*) calloc ((rcBufSize), sizeof (double));
  if (NULL==lprc->FCBUCFMAD)
  {
    no_mem_exit("rc_alloc: lprc->FCBUCFMAD");
  }

  lprc->FCBUPFMAD = (double*) calloc ((rcBufSize), sizeof (double));
  if (NULL==lprc->FCBUPFMAD)
  {
    no_mem_exit("rc_alloc: lprc->FCBUPFMAD");
  }
}

/*!
 *************************************************************************************
 * \brief
 *    Copy JVT rate control objects
 *
 *************************************************************************************
*/
void copy_rc_jvt( rc_quadratic *dst, rc_quadratic *src )
{
  int rcBufSize = img->FrameSizeInMbs/input->basicunit;
  /* buffer original addresses for which memory has been allocated */
  double   *tmpBUPFMAD = dst->BUPFMAD;
  double   *tmpBUCFMAD = dst->BUCFMAD;
  double *tmpFCBUPFMAD = dst->FCBUPFMAD;
  double *tmpFCBUCFMAD = dst->FCBUCFMAD;

  /* copy object */
  memcpy( (void *)dst, (void *)src, sizeof(rc_quadratic) );

  /* restore original addresses */
  dst->BUPFMAD   = tmpBUPFMAD;
  dst->BUCFMAD   = tmpBUCFMAD;
  dst->FCBUPFMAD = tmpFCBUPFMAD;
  dst->FCBUCFMAD = tmpFCBUCFMAD;

  /* copy MADs */
  memcpy( (void *)dst->BUPFMAD,   (void *)src->BUPFMAD,   (rcBufSize) * sizeof (double) );
  memcpy( (void *)dst->BUCFMAD,   (void *)src->BUCFMAD,   (rcBufSize) * sizeof (double) );
  memcpy( (void *)dst->FCBUPFMAD, (void *)src->FCBUPFMAD, (rcBufSize) * sizeof (double) );
  memcpy( (void *)dst->FCBUCFMAD, (void *)src->FCBUCFMAD, (rcBufSize) * sizeof (double) );
}

/*!
 *************************************************************************************
 * \brief
 *    Free memory needed for rate control
 *
 *************************************************************************************
*/
void rc_free(rc_quadratic **prc)
{
  if (NULL!=(*prc)->BUPFMAD)
  {
    free ((*prc)->BUPFMAD);
    (*prc)->BUPFMAD = NULL;
  }
  if (NULL!=(*prc)->BUCFMAD)
  {
    free ((*prc)->BUCFMAD);
    (*prc)->BUCFMAD = NULL;
  }
  if (NULL!=(*prc)->FCBUCFMAD)
  {
    free ((*prc)->FCBUCFMAD);
    (*prc)->FCBUCFMAD = NULL;
  }
  if (NULL!=(*prc)->FCBUPFMAD)
  {
    free ((*prc)->FCBUPFMAD);
    (*prc)->FCBUPFMAD = NULL;
  }
  if (NULL!=(*prc))
  {
    free ((*prc));
    (*prc) = NULL;
  }
}


/*!
 *************************************************************************************
 * \brief
 *    Initialize rate control parameters
 *
 *************************************************************************************
*/
void rc_init_seq(rc_quadratic *prc)
{
  double L1,L2,L3,bpp;
  int qp, i;

  switch ( input->RCUpdateMode )
  {
  case RC_MODE_0:
    updateQP = updateQPRC0;
    break;
  case RC_MODE_1:
    updateQP = updateQPRC1;
    break;
  case RC_MODE_2:
    updateQP = updateQPRC2;
    break;
  case RC_MODE_3:
    updateQP = updateQPRC3;
    break;
  default:
    updateQP = updateQPRC0;
    break;
  }

  prc->Xp=0;
  prc->Xb=0;

  prc->bit_rate = (float) input->bit_rate;
  prc->frame_rate = (img->framerate *(float)(input->successive_Bframe + 1)) / (float) (input->jumpd + 1);
  prc->PrevBitRate = prc->bit_rate;

  /*compute the total number of MBs in a frame*/
  if(input->basicunit > img->FrameSizeInMbs)
    input->basicunit = img->FrameSizeInMbs;
  if(input->basicunit < img->FrameSizeInMbs)
    prc->TotalNumberofBasicUnit = img->FrameSizeInMbs/input->basicunit;

  /*initialize the parameters of fluid flow traffic model*/
  generic_RC->CurrentBufferFullness = 0;
  prc->GOPTargetBufferLevel = (double) generic_RC->CurrentBufferFullness;

  /*initialize the previous window size*/
  prc->m_windowSize    = 0;
  prc->MADm_windowSize = 0;
  generic_RC->NumberofCodedBFrame = 0;
  generic_RC->NumberofCodedPFrame = 0;
  generic_RC->NumberofGOP         = 0;
  /*remaining # of bits in GOP */
  generic_RC->RemainingBits = 0;
  /*control parameter */
  if(input->successive_Bframe>0)
  {
    prc->GAMMAP=0.25;
    prc->BETAP=0.9;
  }
  else
  {
    prc->GAMMAP=0.5;
    prc->BETAP=0.5;
  }

  /*quadratic rate-distortion model*/
  prc->PPreHeader=0;

  prc->Pm_X1 = prc->bit_rate * 1.0;
  prc->Pm_X2 = 0.0;
  /* linear prediction model for P picture*/
  prc->PMADPictureC1 = 1.0;
  prc->PMADPictureC2 = 0.0;

  // Initialize values
  for(i=0;i<21;i++)
  {
    prc->Pm_rgQp[i] = 0;
    prc->Pm_rgRp[i] = 0.0;
    prc->PPictureMAD[i] = 0.0;
  }

  //Define the largest variation of quantization parameters
  prc->PDuantQp=2;

  /*basic unit layer rate control*/
  prc->PAveHeaderBits1 = 0;
  prc->PAveHeaderBits3 = 0;
  prc->DDquant = (prc->TotalNumberofBasicUnit>=9 ? 1 : 2);

  prc->MBPerRow = img->PicWidthInMbs;

  /*adaptive field/frame coding*/
  generic_RC->FieldControl=0;

  /*compute the initial QP*/
  bpp = 1.0*prc->bit_rate /(prc->frame_rate*img->size);

  if (img->width == 176)
  {
    L1 = 0.1;
    L2 = 0.3;
    L3 = 0.6;
  }
  else if (img->width == 352)
  {
    L1 = 0.2;
    L2 = 0.6;
    L3 = 1.2;
  }
  else
  {
    L1 = 0.6;
    L2 = 1.4;
    L3 = 2.4;
  }

  if (input->SeinitialQP==0)
  {
    if (bpp<= L1)
      qp = 35;
    else if(bpp<=L2)
      qp = 25;
    else if(bpp<=L3)
      qp = 20;
    else
      qp = 10;
    input->SeinitialQP = qp;
  }
}

/*!
 *************************************************************************************
 * \brief
 *    Initialize one GOP
 *
 *************************************************************************************
*/
void rc_init_GOP(rc_quadratic *prc, int np, int nb)
{
  Boolean Overum=FALSE;
  int OverBits, OverDuantQp;
  int AllocatedBits, GOPDquant;

  // bit allocation for RC_MODE_3
  switch( input->RCUpdateMode )
  {
  case RC_MODE_3:
    {
      int sum = 0, tmp, level, levels = 0, num_frames[RC_MAX_TEMPORAL_LEVELS];
      float numer, denom;
      int gop = input->successive_Bframe + 1;
      memset( num_frames, 0, RC_MAX_TEMPORAL_LEVELS * sizeof(int) );
      // are there any B frames?
      if ( input->successive_Bframe )
      {
        if ( input->HierarchicalCoding == 1 ) // two layers: even/odd
        {
          levels = 2;
          num_frames[0] = input->successive_Bframe >> 1;
          num_frames[1] = (input->successive_Bframe - num_frames[0]) >= 0 ? (input->successive_Bframe - num_frames[0]) : 0;
        }
        else if ( input->HierarchicalCoding == 2 ) // binary hierarchical structure
        {
          // check if gop is power of two
          tmp = gop;
          while ( tmp )
          {
            sum += tmp & 1;
            tmp >>= 1;
          }
          assert( sum == 1 );

          // determine number of levels
          levels = 0;
          tmp = gop;
          while ( tmp > 1 )
          {
            tmp >>= 1; // divide by 2          
            num_frames[levels] = 1 << levels;
            levels++;          
          }
          assert( levels >= 1 && levels <= RC_MAX_TEMPORAL_LEVELS );        
        }
        else if ( input->HierarchicalCoding == 3 )
        {
          fprintf(stderr, "\n RCUpdateMode=3 and HierarchicalCoding == 3 are currently not supported"); // This error message should be moved elsewhere and have proper memory deallocation
          exit(1);
        }
        else // all frames of the same priority - level
        {
          levels = 1;
          num_frames[0] = input->successive_Bframe;
        }
        generic_RC->temporal_levels = levels;      
      }
      else
      {
        for ( level = 0; level < RC_MAX_TEMPORAL_LEVELS; level++ )
        {
          input->RCBSliceBitRatio[level] = 0.0F;
        }
        generic_RC->temporal_levels = 0;
      }
      // calculate allocated bits for each type of frame
      numer = (float)(( (!input->intra_period ? 1 : input->intra_period) * gop) * ((double)input->bit_rate / input->FrameRate));
      denom = 0.0F;

      for ( level = 0; level < levels; level++ )
      {
        denom += (float)(num_frames[level] * input->RCBSliceBitRatio[level]);
        generic_RC->hierNb[level] = num_frames[level] * np;
      }
      denom += 1.0F;
      if ( input->intra_period >= 1 )
      {
        denom *= (float)input->intra_period;
        denom += (float)input->RCISliceBitRatio - 1.0F;
      }

      // set bit targets for each type of frame
      generic_RC->RCPSliceBits = (int) floor( numer / denom + 0.5F );
      generic_RC->RCISliceBits = (input->intra_period) ? (int)(input->RCISliceBitRatio * generic_RC->RCPSliceBits + 0.5) : 0;

      for ( level = 0; level < levels; level++ )
      {
        generic_RC->RCBSliceBits[level] = (int)floor(input->RCBSliceBitRatio[level] * generic_RC->RCPSliceBits + 0.5);
      }

      generic_RC->NIslice = (input->intra_period) ? ((input->no_frames - 1) / input->intra_period) : 0;
      generic_RC->NPslice = input->no_frames - 1 - generic_RC->NIslice;
    }
    break;
  default:
    break;
  }

  /* check if the last GOP over uses its budget. If yes, the initial QP of the I frame in
  the coming  GOP will be increased.*/

  if(generic_RC->RemainingBits<0)
    Overum=TRUE;
  OverBits=-generic_RC->RemainingBits;

  /*initialize the lower bound and the upper bound for the target bits of each frame, HRD consideration*/
  prc->LowerBound  = (int)(generic_RC->RemainingBits + prc->bit_rate / prc->frame_rate);
  prc->UpperBound1 = (int)(generic_RC->RemainingBits + (prc->bit_rate * 2.048));

  /*compute the total number of bits for the current GOP*/
  AllocatedBits = (int) floor((1 + np + nb) * prc->bit_rate / prc->frame_rate + 0.5);
  generic_RC->RemainingBits += AllocatedBits;
  prc->Np = np;
  prc->Nb = nb;

  OverDuantQp=(int)(8 * OverBits/AllocatedBits+0.5);
  prc->GOPOverdue=FALSE;

  /*field coding*/
  //generic_RC->NoGranularFieldRC = ( input->PicInterlace || !input->MbInterlace || input->basicunit != img->FrameSizeInMbs );
  if ( !input->PicInterlace && input->MbInterlace && input->basicunit == img->FrameSizeInMbs )
    generic_RC->NoGranularFieldRC = 0;
  else
    generic_RC->NoGranularFieldRC = 1;

  /*Compute InitialQp for each GOP*/
  prc->TotalPFrame=np;
  generic_RC->NumberofGOP++;
  if(generic_RC->NumberofGOP==1)
  {
    prc->MyInitialQp = input->SeinitialQP;
    prc->CurrLastQP = prc->MyInitialQp - 1; //recent change -0;
    prc->QPLastGOP   = prc->MyInitialQp;

    prc->PAveFrameQP   = prc->MyInitialQp;
    prc->m_Qc          = prc->PAveFrameQP;
    prc->FieldQPBuffer = prc->PAveFrameQP;
    prc->FrameQPBuffer = prc->PAveFrameQP;
    prc->PAverageQp    = prc->PAveFrameQP;
  }
  else
  {
    /*adaptive field/frame coding*/
    if( input->PicInterlace == ADAPTIVE_CODING || input->MbInterlace )
    {
      if (generic_RC->FieldFrame == 1)
      {
        generic_RC->TotalQpforPPicture += prc->FrameQPBuffer;
        prc->QPLastPFrame = prc->FrameQPBuffer;
      }
      else
      {
        generic_RC->TotalQpforPPicture += prc->FieldQPBuffer;
        prc->QPLastPFrame = prc->FieldQPBuffer;
      }
    }
    /*compute the average QP of P frames in the previous GOP*/
    prc->PAverageQp=(int)(1.0 * generic_RC->TotalQpforPPicture / generic_RC->NumberofPPicture+0.5);

    GOPDquant=(int)((1.0*(np+nb+1)/15.0) + 0.5);
    if(GOPDquant>2)
      GOPDquant=2;

    prc->PAverageQp -= GOPDquant;

    if (prc->PAverageQp > (prc->QPLastPFrame - 2))
      prc->PAverageQp--;

    // QP is constrained by QP of previous QP
    prc->PAverageQp = iClip3(prc->QPLastGOP - 2, prc->QPLastGOP + 2, prc->PAverageQp);
    // Also clipped within range.
    prc->PAverageQp = iClip3(prc->RC_MIN_QUANT,  prc->RC_MAX_QUANT,  prc->PAverageQp);

    prc->MyInitialQp = prc->PAverageQp;
    prc->Pm_Qp       = prc->PAverageQp;
    prc->PAveFrameQP = prc->PAverageQp;
    prc->QPLastGOP   = prc->MyInitialQp;
    prc->PrevLastQP = prc->CurrLastQP;
    prc->CurrLastQP = prc->MyInitialQp - 1;
  }

  generic_RC->TotalQpforPPicture=0;
  generic_RC->NumberofPPicture=0;
  prc->NumberofBFrames=0;
}


/*!
 *************************************************************************************
 * \brief
 *    Initialize one picture
 *
 *************************************************************************************
*/
void rc_init_pict(rc_quadratic *prc, int fieldpic,int topfield,int targetcomputation, float mult)
{
  int tmp_T;

  /* compute the total number of basic units in a frame */
  if(input->MbInterlace)
    prc->TotalNumberofBasicUnit = img->FrameSizeInMbs / img->BasicUnit;

  img->NumberofCodedMacroBlocks = 0;

  /* Normally, the bandwidth for the VBR case is estimated by
     a congestion control algorithm. A bandwidth curve can be predefined if we only want to
     test the proposed algorithm */
  if(input->channel_type==1)
  {
    if(generic_RC->NumberofCodedPFrame==58)
      prc->bit_rate *= 1.5;
    else if(generic_RC->NumberofCodedPFrame==59)
      prc->PrevBitRate = prc->bit_rate;
  }

  /* predefine a target buffer level for each frame */
  if((fieldpic||topfield) && targetcomputation)
  {
    if ( (img->type == P_SLICE || input->RCUpdateMode == RC_MODE_1) && (IMG_NUMBER) )
    {
      /* Since the available bandwidth may vary at any time, the total number of
      bits is updated picture by picture*/
      if(prc->PrevBitRate!=prc->bit_rate)
        generic_RC->RemainingBits +=(int) floor((prc->bit_rate-prc->PrevBitRate)*(prc->Np + prc->Nb)/prc->frame_rate+0.5);

      /* predefine the  target buffer level for each picture.
      frame layer rate control */
      if(img->BasicUnit==img->FrameSizeInMbs)
      {
        if(generic_RC->NumberofPPicture==1)
        {
          prc->TargetBufferLevel = (double) generic_RC->CurrentBufferFullness;
          prc->DeltaP = (generic_RC->CurrentBufferFullness - prc->GOPTargetBufferLevel) / (prc->TotalPFrame-1);
          prc->TargetBufferLevel -= prc->DeltaP;
        }
        else if(generic_RC->NumberofPPicture>1)
          prc->TargetBufferLevel -= prc->DeltaP;
      }
      /* basic unit layer rate control */
      else
      {
        if(generic_RC->NumberofCodedPFrame>0)
        {
          /* adaptive frame/field coding */
          if(((input->PicInterlace==ADAPTIVE_CODING)||(input->MbInterlace))&&(generic_RC->FieldControl==1))
            memcpy((void *)prc->FCBUPFMAD,(void *)prc->FCBUCFMAD, prc->TotalNumberofBasicUnit * sizeof(double));
          else
            memcpy((void *)prc->BUPFMAD,(void *)prc->BUCFMAD, prc->TotalNumberofBasicUnit * sizeof(double));
        }

        if(generic_RC->NumberofGOP==1)
        {
          if(generic_RC->NumberofPPicture==1)
          {
            prc->TargetBufferLevel = (double) generic_RC->CurrentBufferFullness;
            prc->DeltaP = (generic_RC->CurrentBufferFullness - prc->GOPTargetBufferLevel)/(prc->TotalPFrame - 1);
            prc->TargetBufferLevel -= prc->DeltaP;
          }
          else if(generic_RC->NumberofPPicture>1)
            prc->TargetBufferLevel -= prc->DeltaP;
        }
        else if(generic_RC->NumberofGOP>1)
        {
          if(generic_RC->NumberofPPicture==0)
          {
            prc->TargetBufferLevel = (double) generic_RC->CurrentBufferFullness;
            prc->DeltaP = (generic_RC->CurrentBufferFullness - prc->GOPTargetBufferLevel) / prc->TotalPFrame;
            prc->TargetBufferLevel -= prc->DeltaP;
          }
          else if(generic_RC->NumberofPPicture>0)
            prc->TargetBufferLevel -= prc->DeltaP;
        }
      }

      if(generic_RC->NumberofCodedPFrame==1)
        prc->AveWp = prc->Wp;

      if((generic_RC->NumberofCodedPFrame<8)&&(generic_RC->NumberofCodedPFrame>1))
        prc->AveWp = (prc->AveWp + prc->Wp * (generic_RC->NumberofCodedPFrame-1))/generic_RC->NumberofCodedPFrame;
      else if(generic_RC->NumberofCodedPFrame>1)
        prc->AveWp = (prc->Wp + 7 * prc->AveWp) / 8;

      // compute the average complexity of B frames
      if(input->successive_Bframe>0)
      {
        // compute the target buffer level
        prc->TargetBufferLevel += (prc->AveWp * (input->successive_Bframe + 1)*prc->bit_rate\
          /(prc->frame_rate*(prc->AveWp+prc->AveWb*input->successive_Bframe))-prc->bit_rate/prc->frame_rate);
      }
    }
    else if ( img->type == B_SLICE )
    {
      /* update the total number of bits if the bandwidth is changed*/
      if(prc->PrevBitRate != prc->bit_rate)
        generic_RC->RemainingBits +=(int) floor((prc->bit_rate-prc->PrevBitRate) * (prc->Np + prc->Nb) / prc->frame_rate+0.5);
      if((generic_RC->NumberofCodedPFrame==1)&&(generic_RC->NumberofCodedBFrame==1))
      {
        prc->AveWp = prc->Wp;
        prc->AveWb = prc->Wb;
      }
      else if(generic_RC->NumberofCodedBFrame > 1)
      {
        //compute the average weight
        if(generic_RC->NumberofCodedBFrame<8)
          prc->AveWb = (prc->AveWb + prc->Wb*(generic_RC->NumberofCodedBFrame-1)) / generic_RC->NumberofCodedBFrame;
        else
          prc->AveWb = (prc->Wb + 7 * prc->AveWb) / 8;
      }
    }
    /* Compute the target bit for each frame */
    if( (img->type==P_SLICE || input->RCUpdateMode == RC_MODE_1 || input->RCUpdateMode == RC_MODE_3 ) && (IMG_NUMBER) )
    {
      /* frame layer rate control */
      if(img->BasicUnit==img->FrameSizeInMbs || (input->RCUpdateMode == RC_MODE_3) )
      {
        if(generic_RC->NumberofCodedPFrame>0)
        {
          if (input->RCUpdateMode == RC_MODE_3)
          {
            int level_idx = (img->type == B_SLICE && input->HierarchicalCoding) ? (generic_RC->temporal_levels - 1 - gop_structure[img->b_frame_to_code-1].hierarchy_layer) : 0;
            int bitrate = (img->type == B_SLICE) ? generic_RC->RCBSliceBits[ level_idx ]
            : ( img->type == P_SLICE ? generic_RC->RCPSliceBits : generic_RC->RCISliceBits );
            int level, denom = generic_RC->NIslice * generic_RC->RCISliceBits + generic_RC->NPslice * generic_RC->RCPSliceBits;
            if ( input->HierarchicalCoding )
            {
              for ( level = 0; level < generic_RC->temporal_levels; level++ )
                denom += generic_RC->hierNb[ level ] * generic_RC->RCBSliceBits[ level ];
            }
            else
            {
              denom += generic_RC->hierNb[0] * generic_RC->RCBSliceBits[0];
            }
            // target due to remaining bits
            prc->Target = (int) floor( (float)(1.0 * bitrate * generic_RC->RemainingBits) / (float)denom + 0.5F );
            // target given original taget rate and buffer considerations
            tmp_T  = imax(0, (int) floor( (double)bitrate - prc->GAMMAP * (generic_RC->CurrentBufferFullness-prc->TargetBufferLevel) + 0.5) );
            // translate Target rate from B or I "domain" to P domain since the P RC model is going to be used to select the QP
            // for hierarchical coding adjust the target QP to account for different temporal levels
            switch( img->type )
            {
            case B_SLICE:
              prc->Target = (int) floor( (float)prc->Target / input->RCBoverPRatio + 0.5F);
              break;
            case I_SLICE:
              prc->Target = (int) floor( (float)prc->Target / (input->RCIoverPRatio * 4.0) + 0.5F); // 4x accounts for the fact that header bits reduce the percentage of texture
              break;
            case P_SLICE:
            default:
              break;
            }
          }
          else
          {
            prc->Target = (int) floor( prc->Wp * generic_RC->RemainingBits / (prc->Np * prc->Wp + prc->Nb * prc->Wb) + 0.5);
            tmp_T  = imax(0, (int) floor(prc->bit_rate / prc->frame_rate - prc->GAMMAP * (generic_RC->CurrentBufferFullness-prc->TargetBufferLevel) + 0.5));
            prc->Target = (int) floor(prc->BETAP * (prc->Target - tmp_T) + tmp_T + 0.5);
          }
        }
      }
      /* basic unit layer rate control */
      else
      {
        if(((generic_RC->NumberofGOP == 1)&&(generic_RC->NumberofCodedPFrame>0))
          || (generic_RC->NumberofGOP > 1))
        {
          prc->Target = (int) (floor( prc->Wp * generic_RC->RemainingBits / (prc->Np * prc->Wp + prc->Nb * prc->Wb) + 0.5));
          tmp_T  = imax(0, (int) (floor(prc->bit_rate / prc->frame_rate - prc->GAMMAP * (generic_RC->CurrentBufferFullness-prc->TargetBufferLevel) + 0.5)));
          prc->Target = (int) (floor(prc->BETAP * (prc->Target - tmp_T) + tmp_T + 0.5));
        }
      }
      prc->Target = (int)(mult * prc->Target);

      /* reserve some bits for smoothing */
      prc->Target = (int)((1.0 - 0.0 * input->successive_Bframe) * prc->Target);

      /* HRD consideration */
      if ( input->RCUpdateMode != RC_MODE_3 || img->type == P_SLICE )
        prc->Target = iClip3(prc->LowerBound,prc->UpperBound2, prc->Target);
      if((topfield) || (fieldpic && ((input->PicInterlace==ADAPTIVE_CODING)||(input->MbInterlace))))
        prc->TargetField=prc->Target;
    }
  }

  if(fieldpic || topfield)
  {
    /* frame layer rate control */
    generic_RC->NumberofHeaderBits  = 0;
    generic_RC->NumberofTextureBits = 0;

    /* basic unit layer rate control */
    if(img->BasicUnit<img->FrameSizeInMbs)
    {
      prc->TotalFrameQP = 0;
      generic_RC->NumberofBasicUnitHeaderBits  = 0;
      generic_RC->NumberofBasicUnitTextureBits = 0;
      generic_RC->TotalMADBasicUnit = 0;
      if(generic_RC->FieldControl==0)
        prc->NumberofBasicUnit = prc->TotalNumberofBasicUnit;
      else
        prc->NumberofBasicUnit = prc->TotalNumberofBasicUnit >> 1;
    }
  }

  if( ( img->type==P_SLICE || input->RCUpdateMode == RC_MODE_1 ) && img->BasicUnit < img->FrameSizeInMbs && generic_RC->FieldControl == 1 && (IMG_NUMBER) )
  {
    /* top field at basic unit layer rate control */
    if(topfield)
    {
      prc->bits_topfield=0;
      prc->Target=(int)(prc->TargetField*0.6);
    }
    /* bottom field at basic unit layer rate control */
    else
    {
      prc->Target=prc->TargetField-prc->bits_topfield;
      generic_RC->NumberofBasicUnitHeaderBits=0;
      generic_RC->NumberofBasicUnitTextureBits=0;
      generic_RC->TotalMADBasicUnit=0;
      prc->NumberofBasicUnit=prc->TotalNumberofBasicUnit >> 1;
    }
  }
}

/*!
 *************************************************************************************
 * \brief
 *    update one picture after frame/field encoding
 *
 * \param nbits
 *    number of bits used for picture
 *
 *************************************************************************************
*/
void rc_update_pict(rc_quadratic *prc, int nbits)
{
  int delta_bits = (nbits - (int)floor(prc->bit_rate / prc->frame_rate + 0.5F) );
  generic_RC->RemainingBits -= nbits; /* remaining # of bits in GOP */
  generic_RC->CurrentBufferFullness += delta_bits;

  /*update the lower bound and the upper bound for the target bits of each frame, HRD consideration*/
  prc->LowerBound  -= (int) delta_bits;
  prc->UpperBound1 -= (int) delta_bits;
  prc->UpperBound2  = (int)(OMEGA * prc->UpperBound1);

  return;
}

int updateComplexity( rc_quadratic *prc, Boolean is_updated, int nbits )
{
  double Avem_Qc;

  /* frame layer rate control */
  if(img->BasicUnit == img->FrameSizeInMbs)
    return ((int) floor(nbits * prc->m_Qc + 0.5));
  /* basic unit layer rate control */
  else
  {
    if( is_updated )
    {
      if( (generic_RC->NoGranularFieldRC == 0 && generic_RC->FieldControl == 1) || generic_RC->FieldControl == 0 )
      {
        Avem_Qc = (double)prc->TotalFrameQP / (double)prc->TotalNumberofBasicUnit;
        return ((int)floor(nbits * Avem_Qc + 0.5));
      }
    }
    else if( img->type == B_SLICE )
      return ((int) floor(nbits * prc->m_Qc + 0.5));
  }
  return 0;
}

void updatePparams( rc_quadratic *prc, int complexity )
{
  prc->Xp = complexity;
  prc->Np--;
  prc->Wp = prc->Xp;
  prc->Pm_Hp = generic_RC->NumberofHeaderBits;
  generic_RC->NumberofCodedPFrame++;
  generic_RC->NumberofPPicture++;
}

void updateBparams( rc_quadratic *prc, int complexity )
{
  prc->Xb = complexity;
  prc->Nb--;
  prc->Wb = prc->Xb / THETA;     
  prc->NumberofBFrames++;
  generic_RC->NumberofCodedBFrame++;
}

/*! 
 *************************************************************************************
 * \brief
 *    update after frame encoding
 *
 * \param nbits
 *    number of bits used for frame
 *
 *************************************************************************************
*/
void rc_update_pict_frame(rc_quadratic *prc, int nbits)
{
  /* update the complexity weight of I, P, B frame */  
  int complexity = 0;

  switch( input->RCUpdateMode )
  {
  case RC_MODE_0:
  case RC_MODE_2:
  default:
    complexity = updateComplexity( prc, (Boolean) (img->type == P_SLICE && (IMG_NUMBER)), nbits );
    if ( img->type == P_SLICE && (IMG_NUMBER) )
    {
      if( generic_RC->NoGranularFieldRC == 0 || generic_RC->FieldControl == 0 )
        updatePparams( prc, complexity );
      else
        generic_RC->NoGranularFieldRC = 0;
    }
    else if ( img->type == B_SLICE )
      updateBparams( prc, complexity );
    break;
  case RC_MODE_1:
    complexity = updateComplexity( prc, (Boolean) (IMG_NUMBER), nbits );
    if ( (IMG_NUMBER) )
    {
      if( generic_RC->NoGranularFieldRC == 0 || generic_RC->FieldControl == 0 )
        updatePparams( prc, complexity );
      else
        generic_RC->NoGranularFieldRC = 0;
    }
    break;
  case RC_MODE_3:
    complexity = updateComplexity( prc, (Boolean) (img->type == P_SLICE && (IMG_NUMBER)), nbits );
    if (img->type == I_SLICE && IMG_NUMBER)
      generic_RC->NIslice--;

    if ( (img->type == P_SLICE) && (IMG_NUMBER) )
    {
      if( generic_RC->NoGranularFieldRC == 0 || generic_RC->FieldControl == 0 )
      {
        updatePparams( prc, complexity );
        generic_RC->NPslice--;
      }
      else
        generic_RC->NoGranularFieldRC = 0;
    }
    else if ( img->type == B_SLICE )
    {
      updateBparams( prc, complexity );
      generic_RC->hierNb[ input->HierarchicalCoding ? (generic_RC->temporal_levels - 1 - gop_structure[img->b_frame_to_code-1].hierarchy_layer) : 0 ]--;
    }
    break;
  }   
}


/*!
 *************************************************************************************
 * \brief
 *    update the parameters of quadratic R-D model
 *
 *************************************************************************************
*/
void updateRCModel (rc_quadratic *prc)
{
  int n_windowSize;
  int i;
  double std = 0.0, threshold;
  int m_Nc = generic_RC->NumberofCodedPFrame;
  Boolean MADModelFlag = FALSE;
  static Boolean m_rgRejected[RC_MODEL_HISTORY];
  static double  error       [RC_MODEL_HISTORY];

  if( (img->type == P_SLICE || input->RCUpdateMode == RC_MODE_1) && (IMG_NUMBER) )
  {
    /*frame layer rate control*/
    if(img->BasicUnit == img->FrameSizeInMbs)
    {
      prc->CurrentFrameMAD = ComputeFrameMAD();
      m_Nc=generic_RC->NumberofCodedPFrame;
    }
    /*basic unit layer rate control*/
    else
    {
      /*compute the MAD of the current basic unit*/
      prc->CurrentFrameMAD = (double) ((generic_RC->TotalMADBasicUnit >> 8)/img->BasicUnit);
      generic_RC->TotalMADBasicUnit=0;

      /* compute the average number of header bits*/
      prc->CodedBasicUnit=prc->TotalNumberofBasicUnit-prc->NumberofBasicUnit;
      if(prc->CodedBasicUnit > 0)
      {
        prc->PAveHeaderBits1=(int)((double)(prc->PAveHeaderBits1*(prc->CodedBasicUnit-1)+
          generic_RC->NumberofBasicUnitHeaderBits)/prc->CodedBasicUnit+0.5);
        if(prc->PAveHeaderBits3 == 0)
          prc->PAveHeaderBits2 = prc->PAveHeaderBits1;
        else
        {
          prc->PAveHeaderBits2 = (int)((double)(prc->PAveHeaderBits1 * prc->CodedBasicUnit+
            prc->PAveHeaderBits3 * prc->NumberofBasicUnit)/prc->TotalNumberofBasicUnit+0.5);
        }
      }
      /*update the record of MADs for reference*/
      if(((input->PicInterlace == ADAPTIVE_CODING) || (input->MbInterlace)) && (generic_RC->FieldControl == 1))
        prc->FCBUCFMAD[prc->TotalNumberofBasicUnit-1-prc->NumberofBasicUnit]=prc->CurrentFrameMAD;
      else
        prc->BUCFMAD[prc->TotalNumberofBasicUnit-1-prc->NumberofBasicUnit]=prc->CurrentFrameMAD;

      if(prc->NumberofBasicUnit != 0)
        m_Nc = generic_RC->NumberofCodedPFrame * prc->TotalNumberofBasicUnit + prc->CodedBasicUnit;
      else
        m_Nc = (generic_RC->NumberofCodedPFrame-1) * prc->TotalNumberofBasicUnit + prc->CodedBasicUnit;
    }

    if(m_Nc > 1)
      MADModelFlag=TRUE;

    prc->PPreHeader = generic_RC->NumberofHeaderBits;
    for (i = (RC_MODEL_HISTORY-2); i > 0; i--)
    {// update the history
      prc->Pm_rgQp[i] = prc->Pm_rgQp[i - 1];
      prc->m_rgQp[i]  = prc->Pm_rgQp[i];
      prc->Pm_rgRp[i] = prc->Pm_rgRp[i - 1];
      prc->m_rgRp[i]  = prc->Pm_rgRp[i];
    }
    prc->Pm_rgQp[0] = QP2Qstep(prc->m_Qc); //*1.0/prc->CurrentFrameMAD;
    /*frame layer rate control*/
    if(img->BasicUnit==img->FrameSizeInMbs)
      prc->Pm_rgRp[0] = generic_RC->NumberofTextureBits*1.0/prc->CurrentFrameMAD;
    /*basic unit layer rate control*/
    else
      prc->Pm_rgRp[0] = generic_RC->NumberofBasicUnitTextureBits*1.0/prc->CurrentFrameMAD;

    prc->m_rgQp[0] = prc->Pm_rgQp[0];
    prc->m_rgRp[0] = prc->Pm_rgRp[0];
    prc->m_X1 = prc->Pm_X1;
    prc->m_X2 = prc->Pm_X2;

    /*compute the size of window*/
    n_windowSize = (prc->CurrentFrameMAD>prc->PreviousFrameMAD)
      ? (int)(prc->PreviousFrameMAD/prc->CurrentFrameMAD * (RC_MODEL_HISTORY-1) )
      : (int)(prc->CurrentFrameMAD/prc->PreviousFrameMAD *(RC_MODEL_HISTORY-1));
    n_windowSize=iClip3(1, m_Nc, n_windowSize);
    n_windowSize=imin(n_windowSize,prc->m_windowSize+1);
    n_windowSize=imin(n_windowSize,(RC_MODEL_HISTORY-1));

    /*update the previous window size*/
    prc->m_windowSize=n_windowSize;

    for (i = 0; i < (RC_MODEL_HISTORY-1); i++)
    {
      m_rgRejected[i] = FALSE;
    }

    // initial RD model estimator
    RCModelEstimator (prc, n_windowSize, m_rgRejected);

    n_windowSize = prc->m_windowSize;
    // remove outlier

    for (i = 0; i < (int) n_windowSize; i++)
    {
      error[i] = prc->m_X1 / prc->m_rgQp[i] + prc->m_X2 / (prc->m_rgQp[i] * prc->m_rgQp[i]) - prc->m_rgRp[i];
      std += error[i] * error[i];
    }
    threshold = (n_windowSize == 2) ? 0 : sqrt (std / n_windowSize);
    for (i = 0; i < (int) n_windowSize; i++)
    {
      if (fabs(error[i]) > threshold)
        m_rgRejected[i] = TRUE;
    }
    // always include the last data point
    m_rgRejected[0] = FALSE;

    // second RD model estimator
    RCModelEstimator (prc, n_windowSize, m_rgRejected);

    if( MADModelFlag )
      updateMADModel(prc);
    else if( (img->type == P_SLICE || input->RCUpdateMode == RC_MODE_1) && (IMG_NUMBER) )
      prc->PPictureMAD[0]=prc->CurrentFrameMAD;
  }
}

/*!
 *************************************************************************************
 * \brief
 *    Model Estimator
 *
 *************************************************************************************
*/
void RCModelEstimator (rc_quadratic *prc, int n_windowSize, Boolean *m_rgRejected)
{
  int n_realSize = n_windowSize;
  int i;
  double oneSampleQ = 0;
  double a00 = 0.0, a01 = 0.0, a10 = 0.0, a11 = 0.0, b0 = 0.0, b1 = 0.0;
  double MatrixValue;
  Boolean estimateX2 = FALSE;

  for (i = 0; i < n_windowSize; i++)
  {// find the number of samples which are not rejected
    if (m_rgRejected[i])
      n_realSize--;
  }

  // default RD model estimation results
  prc->m_X1 = prc->m_X2 = 0.0;

  for (i = 0; i < n_windowSize; i++)
  {
    if (!m_rgRejected[i])
      oneSampleQ = prc->m_rgQp[i];
  }
  for (i = 0; i < n_windowSize; i++)
  {// if all non-rejected Q are the same, take 1st order model
    if ((prc->m_rgQp[i] != oneSampleQ) && !m_rgRejected[i])
      estimateX2 = TRUE;
    if (!m_rgRejected[i])
      prc->m_X1 += (prc->m_rgQp[i] * prc->m_rgRp[i]) / n_realSize;
  }

  // take 2nd order model to estimate X1 and X2
  if ((n_realSize >= 1) && estimateX2)
  {
    for (i = 0; i < n_windowSize; i++)
    {
      if (!m_rgRejected[i])
      {
        a00  = a00 + 1.0;
        a01 += 1.0 / prc->m_rgQp[i];
        a10  = a01;
        a11 += 1.0 / (prc->m_rgQp[i] * prc->m_rgQp[i]);
        b0  += prc->m_rgQp[i] * prc->m_rgRp[i];
        b1  += prc->m_rgRp[i];
      }
    }
    // solve the equation of AX = B
    MatrixValue=a00*a11-a01*a10;
    if(fabs(MatrixValue) > 0.000001)
    {
      prc->m_X1 = (b0 * a11 - b1 * a01) / MatrixValue;
      prc->m_X2 = (b1 * a00 - b0 * a10) / MatrixValue;
    }
    else
    {
      prc->m_X1 = b0 / a00;
      prc->m_X2 = 0.0;
    }
  }
  if( (img->type == P_SLICE || input->RCUpdateMode == RC_MODE_1) && (IMG_NUMBER) )
  {
    prc->Pm_X1 = prc->m_X1;
    prc->Pm_X2 = prc->m_X2;
  }
}

/*!
 *************************************************************************************
 * \brief
 *    update the parameters of linear prediction model
 *
 *************************************************************************************
*/
void updateMADModel (rc_quadratic *prc)
{
  int    n_windowSize;
  int    i;
  double std = 0.0, threshold;
  int    m_Nc = generic_RC->NumberofCodedPFrame;
  static Boolean PictureRejected[RC_MODEL_HISTORY];
  static double  error          [RC_MODEL_HISTORY];

  if(generic_RC->NumberofCodedPFrame>0)
  {
    //assert (img->type!=P_SLICE);
    /*frame layer rate control*/
    if(img->BasicUnit == img->FrameSizeInMbs)
      m_Nc=generic_RC->NumberofCodedPFrame;
    /*basic unit layer rate control*/
    else
      m_Nc=generic_RC->NumberofCodedPFrame*prc->TotalNumberofBasicUnit+prc->CodedBasicUnit;

    for (i = (RC_MODEL_HISTORY-2); i > 0; i--)
    {// update the history
      prc->PPictureMAD[i]  = prc->PPictureMAD[i - 1];
      prc->PictureMAD[i]   = prc->PPictureMAD[i];
      prc->ReferenceMAD[i] = prc->ReferenceMAD[i-1];
    }
    prc->PPictureMAD[0] = prc->CurrentFrameMAD;
    prc->PictureMAD[0]  = prc->PPictureMAD[0];

    if(img->BasicUnit == img->FrameSizeInMbs)
      prc->ReferenceMAD[0]=prc->PictureMAD[1];
    else
    {
      if(((input->PicInterlace==ADAPTIVE_CODING)||(input->MbInterlace)) &&(generic_RC->FieldControl==1))
        prc->ReferenceMAD[0]=prc->FCBUPFMAD[prc->TotalNumberofBasicUnit-1-prc->NumberofBasicUnit];
      else
        prc->ReferenceMAD[0]=prc->BUPFMAD[prc->TotalNumberofBasicUnit-1-prc->NumberofBasicUnit];
    }
    prc->MADPictureC1 = prc->PMADPictureC1;
    prc->MADPictureC2 = prc->PMADPictureC2;

    /*compute the size of window*/
    n_windowSize = (prc->CurrentFrameMAD > prc->PreviousFrameMAD)
      ? (int) ((float)(RC_MODEL_HISTORY-1) * prc->PreviousFrameMAD / prc->CurrentFrameMAD)
      : (int) ((float)(RC_MODEL_HISTORY-1) * prc->CurrentFrameMAD / prc->PreviousFrameMAD);
    n_windowSize = iClip3(1, (m_Nc-1), n_windowSize);
    n_windowSize=imin(n_windowSize, imin(20, prc->MADm_windowSize + 1));

    /*update the previous window size*/
    prc->MADm_windowSize=n_windowSize;

    for (i = 0; i < (RC_MODEL_HISTORY-1); i++)
    {
      PictureRejected[i] = FALSE;
    }

    //update the MAD for the previous frame
    if( (img->type == P_SLICE || input->RCUpdateMode == RC_MODE_1) && (IMG_NUMBER) )
      prc->PreviousFrameMAD=prc->CurrentFrameMAD;

    // initial MAD model estimator
    MADModelEstimator (prc, n_windowSize, PictureRejected);

    // remove outlier
    for (i = 0; i < n_windowSize; i++)
    {
      error[i] = prc->MADPictureC1 * prc->ReferenceMAD[i] + prc->MADPictureC2 - prc->PictureMAD[i];
      std += (error[i] * error[i]);
    }

    threshold = (n_windowSize == 2) ? 0 : sqrt (std / n_windowSize);
    for (i = 0; i < n_windowSize; i++)
    {
      if (fabs(error[i]) > threshold)
        PictureRejected[i] = TRUE;
    }
    // always include the last data point
    PictureRejected[0] = FALSE;

    // second MAD model estimator
    MADModelEstimator (prc, n_windowSize, PictureRejected);
  }
}


/*!
 *************************************************************************************
 * \brief
 *    MAD mode estimator
 *
 *************************************************************************************
*/
void MADModelEstimator (rc_quadratic *prc, int n_windowSize, Boolean *PictureRejected)
{
  int    n_realSize = n_windowSize;
  int    i;
  double oneSampleQ = 0.0;
  double a00 = 0.0, a01 = 0.0, a10 = 0.0, a11 = 0.0, b0 = 0.0, b1 = 0.0;
  double MatrixValue;
  Boolean estimateX2 = FALSE;

  for (i = 0; i < n_windowSize; i++)
  {// find the number of samples which are not rejected
    if (PictureRejected[i])
      n_realSize--;
  }

  // default MAD model estimation results
  prc->MADPictureC1 = prc->MADPictureC2 = 0.0;

  for (i = 0; i < n_windowSize; i++)
  {
    if (!PictureRejected[i])
      oneSampleQ = prc->PictureMAD[i];
  }

  for (i = 0; i < n_windowSize; i++)
  {// if all non-rejected MAD are the same, take 1st order model
    if ((prc->PictureMAD[i] != oneSampleQ) && !PictureRejected[i])
      estimateX2 = TRUE;
    if (!PictureRejected[i])
      prc->MADPictureC1 += prc->PictureMAD[i] / (prc->ReferenceMAD[i]*n_realSize);
  }

  // take 2nd order model to estimate X1 and X2
  if ((n_realSize >= 1) && estimateX2)
  {
    for (i = 0; i < n_windowSize; i++)
    {
      if (!PictureRejected[i])
      {
        a00  = a00 + 1.0;
        a01 += prc->ReferenceMAD[i];
        a10  = a01;
        a11 += prc->ReferenceMAD[i] * prc->ReferenceMAD[i];
        b0  += prc->PictureMAD[i];
        b1  += prc->PictureMAD[i]   * prc->ReferenceMAD[i];
      }
    }
    // solve the equation of AX = B
    MatrixValue = a00 * a11 - a01 * a10;
    if(fabs(MatrixValue) > 0.000001)
    {
      prc->MADPictureC2 = (b0 * a11 - b1 * a01) / MatrixValue;
      prc->MADPictureC1 = (b1 * a00 - b0 * a10) / MatrixValue;
    }
    else
    {
      prc->MADPictureC1 = b0/a01;
      prc->MADPictureC2 = 0.0;
    }
  }
  if( (img->type == P_SLICE || input->RCUpdateMode == RC_MODE_1) && (IMG_NUMBER) )
  {
    prc->PMADPictureC1 = prc->MADPictureC1;
    prc->PMADPictureC2 = prc->MADPictureC2;
  }
}

/*!
 *************************************************************************************
 * \brief
 *    compute a  quantization parameter for each frame (RC_MODE_0)
 *
 *************************************************************************************
*/
int updateQPRC0(rc_quadratic *prc, int topfield)
{
  int m_Bits;
  int BFrameNumber;
  int StepSize;
  int SumofBasicUnit;
  int DuantQp, m_Qp, m_Hp;

  /* frame layer rate control */
  if(img->BasicUnit==img->FrameSizeInMbs )
  {
    /* fixed quantization parameter is used to coded I frame, the first P frame and the first B frame
    the quantization parameter is adjusted according the available channel bandwidth and
    the type of video */
    /*top field*/
    if((topfield) || (generic_RC->FieldControl==0))
    {
      if (img->type==I_SLICE)
      {
        prc->m_Qc = prc->MyInitialQp;
        return prc->m_Qc;
      }
      else if(img->type == B_SLICE)
      {
        if(input->successive_Bframe==1)
        {
          if((input->PicInterlace==ADAPTIVE_CODING)||(input->MbInterlace))
            updateQPInterlace( prc );

          prc->m_Qc = imin(prc->PrevLastQP, prc->CurrLastQP) + 2;
          prc->m_Qc = imax(prc->m_Qc, imax(prc->PrevLastQP, prc->CurrLastQP));
          prc->m_Qc = imax(prc->m_Qc, prc->CurrLastQP + 1);
          prc->m_Qc = iClip3(prc->RC_MIN_QUANT, prc->RC_MAX_QUANT, prc->m_Qc); // Clipping
        }
        else
        {
          BFrameNumber = (prc->NumberofBFrames + 1) % input->successive_Bframe;
          if(BFrameNumber==0)
            BFrameNumber = input->successive_Bframe;

          /*adaptive field/frame coding*/
          if(BFrameNumber==1)
          {
            if((input->PicInterlace==ADAPTIVE_CODING)||(input->MbInterlace))
              updateQPInterlace( prc );
          }

          if((prc->CurrLastQP-prc->PrevLastQP)<=(-2*input->successive_Bframe-3))
            StepSize=-3;
          else  if((prc->CurrLastQP-prc->PrevLastQP)==(-2*input->successive_Bframe-2))
            StepSize=-2;
          else if((prc->CurrLastQP-prc->PrevLastQP)==(-2*input->successive_Bframe-1))
            StepSize=-1;
          else if((prc->CurrLastQP-prc->PrevLastQP)==(-2*input->successive_Bframe))
            StepSize=0;
          else if((prc->CurrLastQP-prc->PrevLastQP)==(-2*input->successive_Bframe+1))
            StepSize=1;
          else
            StepSize=2;

          prc->m_Qc  = prc->PrevLastQP + StepSize;
          prc->m_Qc += iClip3( -2 * (BFrameNumber - 1), 2*(BFrameNumber-1),
            (BFrameNumber-1)*(prc->CurrLastQP-prc->PrevLastQP)/(input->successive_Bframe-1));
          prc->m_Qc  = iClip3(prc->RC_MIN_QUANT, prc->RC_MAX_QUANT, prc->m_Qc); // Clipping
        }
        return prc->m_Qc;
      }
      else if( img->type == P_SLICE && generic_RC->NumberofPPicture == 0 && (IMG_NUMBER) )
      {
        prc->m_Qc=prc->MyInitialQp;

        if(generic_RC->FieldControl==0)
          updateQPNonPicAFF( prc );
        return prc->m_Qc;
      }
      else
      {
        /*adaptive field/frame coding*/
        if( ( input->PicInterlace == ADAPTIVE_CODING || input->MbInterlace ) && generic_RC->FieldControl == 0 )
          updateQPInterlaceBU( prc );

        prc->m_X1 = prc->Pm_X1;
        prc->m_X2 = prc->Pm_X2;
        prc->MADPictureC1 = prc->PMADPictureC1;
        prc->MADPictureC2 = prc->PMADPictureC2;
        prc->PreviousPictureMAD = prc->PPictureMAD[0];

        DuantQp = prc->PDuantQp;
        m_Qp = prc->Pm_Qp;
        m_Hp = prc->PPreHeader;

        /* predict the MAD of current picture*/
        prc->CurrentFrameMAD=prc->MADPictureC1*prc->PreviousPictureMAD + prc->MADPictureC2;

        /*compute the number of bits for the texture*/
        if(prc->Target < 0)
        {
          prc->m_Qc=m_Qp+DuantQp;
          prc->m_Qc = iClip3(prc->RC_MIN_QUANT, prc->RC_MAX_QUANT, prc->m_Qc); // Clipping
        }
        else
        {
          m_Bits = prc->Target-m_Hp;
          m_Bits = imax(m_Bits, (int)(prc->bit_rate/(MINVALUE*prc->frame_rate)));

          updateModelQPFrame( prc, m_Bits );

          prc->m_Qc = iClip3(prc->RC_MIN_QUANT, prc->RC_MAX_QUANT, prc->m_Qc); // clipping
          prc->m_Qc = iClip3(m_Qp-DuantQp, m_Qp+DuantQp, prc->m_Qc); // control variation
        }

        if( generic_RC->FieldControl == 0 )
          updateQPNonPicAFF( prc );

        return prc->m_Qc;
      }
    }
    /*bottom field*/
    else
    {
      if( img->type==P_SLICE && generic_RC->NoGranularFieldRC == 0 && (IMG_NUMBER) )
        updateBottomField( prc );
      return prc->m_Qc;
    }
  }
  /*basic unit layer rate control*/
  else
  {
    /*top field of I frame*/
    if (img->type==I_SLICE || (!IMG_NUMBER))
    {
      prc->m_Qc = prc->MyInitialQp;
      return prc->m_Qc;
    }
    else if( img->type == B_SLICE )
    {
      /*top field of B frame*/
      if((topfield)||(generic_RC->FieldControl==0))
      {
        if(input->successive_Bframe==1)
        {
          /*adaptive field/frame coding*/
          if((input->PicInterlace==ADAPTIVE_CODING)||(input->MbInterlace))
            updateQPInterlace( prc );

          if(prc->PrevLastQP==prc->CurrLastQP)
            prc->m_Qc=prc->PrevLastQP+2;
          else
            prc->m_Qc=(prc->PrevLastQP+prc->CurrLastQP)/2+1;
          prc->m_Qc = iClip3(prc->RC_MIN_QUANT, prc->RC_MAX_QUANT, prc->m_Qc); // Clipping
        }
        else
        {
          BFrameNumber=(prc->NumberofBFrames+1)%input->successive_Bframe;
          if(BFrameNumber==0)
            BFrameNumber=input->successive_Bframe;

          /*adaptive field/frame coding*/
          if(BFrameNumber==1)
          {
            if((input->PicInterlace==ADAPTIVE_CODING)||(input->MbInterlace))
              updateQPInterlace( prc );
          }

          if((prc->CurrLastQP-prc->PrevLastQP)<=(-2*input->successive_Bframe-3))
            StepSize=-3;
          else  if((prc->CurrLastQP-prc->PrevLastQP)==(-2*input->successive_Bframe-2))
            StepSize=-2;
          else if((prc->CurrLastQP-prc->PrevLastQP)==(-2*input->successive_Bframe-1))
            StepSize=-1;
          else if((prc->CurrLastQP-prc->PrevLastQP)==(-2*input->successive_Bframe))
            StepSize=0;//0
          else if((prc->CurrLastQP-prc->PrevLastQP)==(-2*input->successive_Bframe+1))
            StepSize=1;//1
          else
            StepSize=2;//2
          prc->m_Qc=prc->PrevLastQP+StepSize;
          prc->m_Qc +=
            iClip3( -2*(BFrameNumber-1), 2*(BFrameNumber-1), (BFrameNumber-1)*(prc->CurrLastQP-prc->PrevLastQP)/(input->successive_Bframe-1) );
          prc->m_Qc = iClip3(prc->RC_MIN_QUANT, prc->RC_MAX_QUANT, prc->m_Qc); // Clipping
        }
        return prc->m_Qc;
      }
      /*bottom field of B frame*/
      else
      {
        return prc->m_Qc;
      }
    }
    else if( img->type == P_SLICE )
    {
      if( generic_RC->NumberofGOP == 1 && generic_RC->NumberofPPicture == 0 )
      {
        if((generic_RC->FieldControl==0)||((generic_RC->FieldControl==1) && (generic_RC->NoGranularFieldRC==0)))
          return updateFirstP( prc, topfield );
      }
      else
      {
        prc->m_X1=prc->Pm_X1;
        prc->m_X2=prc->Pm_X2;
        prc->MADPictureC1=prc->PMADPictureC1;
        prc->MADPictureC2=prc->PMADPictureC2;

        m_Qp=prc->Pm_Qp;

        if(generic_RC->FieldControl==0)
          SumofBasicUnit=prc->TotalNumberofBasicUnit;
        else
          SumofBasicUnit=prc->TotalNumberofBasicUnit>>1;

        /*the average QP of the previous frame is used to coded the first basic unit of the current frame or field*/
        if(prc->NumberofBasicUnit==SumofBasicUnit)
          return updateFirstBU( prc, topfield );
        else
        {
          /*compute the number of remaining bits*/
          prc->Target -= (generic_RC->NumberofBasicUnitHeaderBits + generic_RC->NumberofBasicUnitTextureBits);
          generic_RC->NumberofBasicUnitHeaderBits  = 0;
          generic_RC->NumberofBasicUnitTextureBits = 0;
          if(prc->Target<0)
            return updateNegativeTarget( prc, topfield, m_Qp );
          else
          {
            /*predict the MAD of current picture*/
            predictCurrPicMAD( prc );

            /*compute the total number of bits for the current basic unit*/
            updateModelQPBU( prc, topfield, m_Qp );

            prc->TotalFrameQP +=prc->m_Qc;
            prc->Pm_Qp=prc->m_Qc;
            prc->NumberofBasicUnit--;
            if( prc->NumberofBasicUnit == 0 && img->type == P_SLICE && (IMG_NUMBER) )
              updateLastBU( prc, topfield );

            return prc->m_Qc;
          }
        }
      }
    }
  }
  return prc->m_Qc;
}

/*!
 *************************************************************************************
 * \brief
 *    compute a  quantization parameter for each frame
 *
 *************************************************************************************
*/
int updateQPRC1(rc_quadratic *prc, int topfield)
{
  int m_Bits;
  int SumofBasicUnit;
  int DuantQp, m_Qp, m_Hp;

  /* frame layer rate control */
  if(img->BasicUnit == img->FrameSizeInMbs)
  {
    /* fixed quantization parameter is used to coded I frame, the first P frame and the first B frame
    the quantization parameter is adjusted according the available channel bandwidth and
    the type of vide */
    /*top field*/
    if((topfield) || (generic_RC->FieldControl==0))
    {
      if ((!IMG_NUMBER))
      {
        prc->m_Qc = prc->MyInitialQp;
        return prc->m_Qc;
      }
      else if( generic_RC->NumberofPPicture == 0 && (IMG_NUMBER) )
      {
        prc->m_Qc=prc->MyInitialQp;

        if(generic_RC->FieldControl==0)
          updateQPNonPicAFF( prc );
        return prc->m_Qc;
      }
      else
      {
        /*adaptive field/frame coding*/
        if( ( input->PicInterlace == ADAPTIVE_CODING || input->MbInterlace ) && generic_RC->FieldControl == 0 )
          updateQPInterlaceBU( prc );

        prc->m_X1 = prc->Pm_X1;
        prc->m_X2 = prc->Pm_X2;
        prc->MADPictureC1 = prc->PMADPictureC1;
        prc->MADPictureC2 = prc->PMADPictureC2;
        prc->PreviousPictureMAD = prc->PPictureMAD[0];

        DuantQp = prc->PDuantQp;
        m_Qp = prc->Pm_Qp;
        m_Hp = prc->PPreHeader;

        /* predict the MAD of current picture*/
        prc->CurrentFrameMAD=prc->MADPictureC1*prc->PreviousPictureMAD + prc->MADPictureC2;

        /*compute the number of bits for the texture*/
        if(prc->Target < 0)
        {
          prc->m_Qc=m_Qp+DuantQp;
          prc->m_Qc = iClip3(prc->RC_MIN_QUANT, prc->RC_MAX_QUANT, prc->m_Qc); // Clipping
        }
        else
        {
          m_Bits = prc->Target-m_Hp;
          m_Bits = imax(m_Bits, (int)(prc->bit_rate/(MINVALUE*prc->frame_rate)));

          updateModelQPFrame( prc, m_Bits );

          prc->m_Qc = iClip3(prc->RC_MIN_QUANT, prc->RC_MAX_QUANT, prc->m_Qc); // clipping
          prc->m_Qc = iClip3(m_Qp-DuantQp, m_Qp+DuantQp, prc->m_Qc); // control variation
        }

        if( generic_RC->FieldControl == 0 )
          updateQPNonPicAFF( prc );

        return prc->m_Qc;
      }
    }
    /*bottom field*/
    else
    {
      if( generic_RC->NoGranularFieldRC == 0 && (IMG_NUMBER) )
        updateBottomField( prc );
      return prc->m_Qc;
    }
  }
  /*basic unit layer rate control*/
  else
  {
    /*top field of I frame*/
    if ((!IMG_NUMBER))
    {
      prc->m_Qc = prc->MyInitialQp;
      return prc->m_Qc;
    }
    else
    {
      if((generic_RC->NumberofGOP==1)&&(generic_RC->NumberofPPicture==0))
      {
        if((generic_RC->FieldControl==0)||((generic_RC->FieldControl==1) && (generic_RC->NoGranularFieldRC==0)))
          return updateFirstP( prc, topfield );
      }
      else
      {
        prc->m_X1=prc->Pm_X1;
        prc->m_X2=prc->Pm_X2;
        prc->MADPictureC1=prc->PMADPictureC1;
        prc->MADPictureC2=prc->PMADPictureC2;

        m_Qp=prc->Pm_Qp;

        if(generic_RC->FieldControl==0)
          SumofBasicUnit=prc->TotalNumberofBasicUnit;
        else
          SumofBasicUnit=prc->TotalNumberofBasicUnit>>1;

        /*the average QP of the previous frame is used to coded the first basic unit of the current frame or field*/
        if(prc->NumberofBasicUnit==SumofBasicUnit)
          return updateFirstBU( prc, topfield );
        else
        {
          /*compute the number of remaining bits*/
          prc->Target -= (generic_RC->NumberofBasicUnitHeaderBits + generic_RC->NumberofBasicUnitTextureBits);
          generic_RC->NumberofBasicUnitHeaderBits  = 0;
          generic_RC->NumberofBasicUnitTextureBits = 0;
          if(prc->Target<0)
            return updateNegativeTarget( prc, topfield, m_Qp );
          else
          {
            /*predict the MAD of current picture*/
            predictCurrPicMAD( prc );

            /*compute the total number of bits for the current basic unit*/
            updateModelQPBU( prc, topfield, m_Qp );

            prc->TotalFrameQP +=prc->m_Qc;
            prc->Pm_Qp=prc->m_Qc;
            prc->NumberofBasicUnit--;
            if((prc->NumberofBasicUnit==0) && (IMG_NUMBER) )
              updateLastBU( prc, topfield );

            return prc->m_Qc;
          }
        }
      }
    }
  }
  return prc->m_Qc;
}



/*!
 *************************************************************************************
 * \brief
 *    compute a  quantization parameter for each frame
 *
 *************************************************************************************
*/
int updateQPRC2(rc_quadratic *prc, int topfield)
{
  int m_Bits;
  int SumofBasicUnit;
  int DuantQp, m_Qp, m_Hp;

  /* frame layer rate control */
  if(img->BasicUnit==img->FrameSizeInMbs )
  {
    /* fixed quantization parameter is used to coded I frame, the first P frame and the first B frame
    the quantization parameter is adjusted according the available channel bandwidth and
    the type of vide */
    /*top field*/
    if((topfield) || (generic_RC->FieldControl==0))
    {
      if ((!IMG_NUMBER))
      {
        prc->m_Qc = prc->MyInitialQp;
        return prc->m_Qc;
      }
      else if (img->type==I_SLICE)
      {
        if((input->PicInterlace==ADAPTIVE_CODING)||(input->MbInterlace))
          updateQPInterlace( prc );

        prc->m_Qc = prc->CurrLastQP; // Set QP to average qp of last P frame
        return prc->m_Qc;
      }
      else if(img->type == B_SLICE)
      {
        int prevQP = imax(prc->PrevLastQP, prc->CurrLastQP);
        if((input->PicInterlace==ADAPTIVE_CODING)||(input->MbInterlace))
          updateQPInterlace( prc );

        if (input->HierarchicalCoding)
        {
          if (img->b_frame_to_code == 0)
            prc->m_Qc = prevQP;
          else
            prc->m_Qc = prevQP + img->GopLevels - gop_structure[img->b_frame_to_code-1].hierarchy_layer;
        }
        else
          prc->m_Qc = prevQP + 2 - img->nal_reference_idc;
        prc->m_Qc = iClip3(prc->RC_MIN_QUANT, prc->RC_MAX_QUANT, prc->m_Qc); // Clipping

        return prc->m_Qc;
      }
      else if( img->type == P_SLICE && generic_RC->NumberofPPicture == 0 && (IMG_NUMBER) )
      {
        prc->m_Qc=prc->MyInitialQp;

        if(generic_RC->FieldControl==0)
          updateQPNonPicAFF( prc );
        return prc->m_Qc;
      }
      else
      {
        /*adaptive field/frame coding*/
        if( ( input->PicInterlace == ADAPTIVE_CODING || input->MbInterlace ) && generic_RC->FieldControl == 0 )
          updateQPInterlaceBU( prc );

        prc->m_X1 = prc->Pm_X1;
        prc->m_X2 = prc->Pm_X2;
        prc->MADPictureC1 = prc->PMADPictureC1;
        prc->MADPictureC2 = prc->PMADPictureC2;
        prc->PreviousPictureMAD = prc->PPictureMAD[0];

        DuantQp = prc->PDuantQp;
        m_Qp = prc->Pm_Qp;
        m_Hp = prc->PPreHeader;

        /* predict the MAD of current picture*/
        prc->CurrentFrameMAD=prc->MADPictureC1*prc->PreviousPictureMAD + prc->MADPictureC2;

        /*compute the number of bits for the texture*/
        if(prc->Target < 0)
        {
          prc->m_Qc=m_Qp+DuantQp;
          prc->m_Qc = iClip3(prc->RC_MIN_QUANT, prc->RC_MAX_QUANT, prc->m_Qc); // Clipping
        }
        else
        {
          m_Bits = prc->Target-m_Hp;
          m_Bits = imax(m_Bits, (int)(prc->bit_rate/(MINVALUE*prc->frame_rate)));

          updateModelQPFrame( prc, m_Bits );

          prc->m_Qc = iClip3(prc->RC_MIN_QUANT, prc->RC_MAX_QUANT, prc->m_Qc); // clipping
          prc->m_Qc = iClip3(m_Qp-DuantQp, m_Qp+DuantQp, prc->m_Qc); // control variation
        }

        if( generic_RC->FieldControl == 0 )
          updateQPNonPicAFF( prc );

        return prc->m_Qc;
      }
    }
    /*bottom field*/
    else
    {
      if( img->type==P_SLICE && generic_RC->NoGranularFieldRC == 0 && (IMG_NUMBER) )
        updateBottomField( prc );
      return prc->m_Qc;
    }
  }
  /*basic unit layer rate control*/
  else
  {
    /*top field of I frame*/
    if ((!IMG_NUMBER))
    {
      prc->m_Qc = prc->MyInitialQp;
      return prc->m_Qc;
    }
    else if (img->type==I_SLICE)
    {
      /*adaptive field/frame coding*/
      if((input->PicInterlace==ADAPTIVE_CODING)||(input->MbInterlace))
        updateQPInterlace( prc );

      prc->m_Qc = prc->PrevLastQP; // Set QP to average qp of last P frame
      prc->PrevLastQP = prc->CurrLastQP;
      prc->CurrLastQP = prc->PrevLastQP;
      prc->PAveFrameQP = prc->CurrLastQP;

      return prc->m_Qc;
    }
    else if(img->type == B_SLICE)
    {
      int prevQP = imax(prc->PrevLastQP, prc->CurrLastQP);
      if((input->PicInterlace==ADAPTIVE_CODING)||(input->MbInterlace))
        updateQPInterlace( prc );

      if (input->HierarchicalCoding)
      {

        if (img->b_frame_to_code == 0)
          prc->m_Qc = prevQP;
        else
          prc->m_Qc = prevQP + img->GopLevels - gop_structure[img->b_frame_to_code-1].hierarchy_layer;
      }
      else
        prc->m_Qc = prevQP + 2 - img->nal_reference_idc;
      prc->m_Qc = iClip3(prc->RC_MIN_QUANT, prc->RC_MAX_QUANT, prc->m_Qc); // Clipping

      return prc->m_Qc;

    }
    else if( img->type == P_SLICE )
    {
      if((generic_RC->NumberofGOP==1)&&(generic_RC->NumberofPPicture==0))
      {
        if((generic_RC->FieldControl==0)||((generic_RC->FieldControl==1) && (generic_RC->NoGranularFieldRC==0)))
          return updateFirstP( prc, topfield );
      }
      else
      {
        prc->m_X1=prc->Pm_X1;
        prc->m_X2=prc->Pm_X2;
        prc->MADPictureC1=prc->PMADPictureC1;
        prc->MADPictureC2=prc->PMADPictureC2;

        m_Qp=prc->Pm_Qp;

        if(generic_RC->FieldControl==0)
          SumofBasicUnit=prc->TotalNumberofBasicUnit;
        else
          SumofBasicUnit=prc->TotalNumberofBasicUnit>>1;

        /*the average QP of the previous frame is used to coded the first basic unit of the current frame or field*/
        if(prc->NumberofBasicUnit==SumofBasicUnit)
          return updateFirstBU( prc, topfield );
        else
        {
          /*compute the number of remaining bits*/
          prc->Target -= (generic_RC->NumberofBasicUnitHeaderBits + generic_RC->NumberofBasicUnitTextureBits);
          generic_RC->NumberofBasicUnitHeaderBits  = 0;
          generic_RC->NumberofBasicUnitTextureBits = 0;
          if(prc->Target<0)
            return updateNegativeTarget( prc, topfield, m_Qp );
          else
          {
            /*predict the MAD of current picture*/
            predictCurrPicMAD( prc );

            /*compute the total number of bits for the current basic unit*/
            updateModelQPBU( prc, topfield, m_Qp );

            prc->TotalFrameQP +=prc->m_Qc;
            prc->Pm_Qp=prc->m_Qc;
            prc->NumberofBasicUnit--;
            if((prc->NumberofBasicUnit==0) && img->type == P_SLICE && (IMG_NUMBER) )
              updateLastBU( prc, topfield );

            return prc->m_Qc;
          }
        }
      }
    }
  }
  return prc->m_Qc;
}




/*!
 *************************************************************************************
 * \brief
 *    compute a  quantization parameter for each frame
 *
 *************************************************************************************
*/
int updateQPRC3(rc_quadratic *prc, int topfield)
{
  int m_Bits;
  int SumofBasicUnit;
  int DuantQp, m_Qp, m_Hp;

  /* frame layer rate control */
  if(img->BasicUnit==img->FrameSizeInMbs || img->type != P_SLICE )
  {
    /* fixed quantization parameter is used to coded I frame, the first P frame and the first B frame
    the quantization parameter is adjusted according the available channel bandwidth and
    the type of video */
    /*top field*/
    if((topfield) || (generic_RC->FieldControl==0))
    {
      if ((!IMG_NUMBER))
      {
        prc->m_Qc = prc->MyInitialQp;
        return prc->m_Qc;
      }
      else if( img->type == P_SLICE && generic_RC->NumberofPPicture == 0 && (IMG_NUMBER) )
      {
        prc->m_Qc=prc->MyInitialQp;

        if(generic_RC->FieldControl==0)
          updateQPNonPicAFF( prc );
        return prc->m_Qc;
      }
      else
      {
        /*adaptive field/frame coding*/
        if( img->type == P_SLICE && ( input->PicInterlace == ADAPTIVE_CODING || input->MbInterlace ) && generic_RC->FieldControl == 0 )
          updateQPInterlaceBU( prc );

        prc->m_X1 = prc->Pm_X1;
        prc->m_X2 = prc->Pm_X2;
        prc->MADPictureC1 = prc->PMADPictureC1;
        prc->MADPictureC2 = prc->PMADPictureC2;
        prc->PreviousPictureMAD = prc->PPictureMAD[0];

        DuantQp = prc->PDuantQp;
        m_Qp = prc->Pm_Qp;
        m_Hp = prc->PPreHeader;

        if ( img->BasicUnit < img->FrameSizeInMbs && img->type != P_SLICE )
        {
          // when RC_MODE_3 is set and basic unit is smaller than a frame, note that:
          // the linear MAD model and the quadratic QP model operate on small units and not on a whole frame;
          // we therefore have to account for this
          prc->PreviousPictureMAD = prc->PreviousWholeFrameMAD;
        }
        if ( img->type == I_SLICE )
          m_Hp = 0; // it is usually a very small portion of the total I_SLICE bit budget

        /* predict the MAD of current picture*/
        prc->CurrentFrameMAD=prc->MADPictureC1*prc->PreviousPictureMAD + prc->MADPictureC2;

        /*compute the number of bits for the texture*/
        if(prc->Target < 0)
        {
          prc->m_Qc=m_Qp+DuantQp;
          prc->m_Qc = iClip3(prc->RC_MIN_QUANT, prc->RC_MAX_QUANT, prc->m_Qc); // Clipping
        }
        else
        {
          if ( img->type != P_SLICE )
          {
            if ( img->BasicUnit < img->FrameSizeInMbs )
              m_Bits =(prc->Target-m_Hp)/prc->TotalNumberofBasicUnit;
            else
              m_Bits =prc->Target-m_Hp;
          }
          else {
            m_Bits = prc->Target-m_Hp;
            m_Bits = imax(m_Bits, (int)(prc->bit_rate/(MINVALUE*prc->frame_rate)));
          }
          updateModelQPFrame( prc, m_Bits );

          prc->m_Qc = iClip3(prc->RC_MIN_QUANT, prc->RC_MAX_QUANT, prc->m_Qc); // clipping
          if ( img->type == P_SLICE )
            prc->m_Qc = iClip3(m_Qp-DuantQp, m_Qp+DuantQp, prc->m_Qc); // control variation
        }

        if( img->type == P_SLICE && generic_RC->FieldControl == 0 )
          updateQPNonPicAFF( prc );

        if ( img->type == B_SLICE )
        {
          // hierarchical adjustment
          int prevqp = ((prc->PrevLastQP+prc->CurrLastQP) >> 1) + 1;
          if ( input->HierarchicalCoding && img->b_frame_to_code)
            prc->m_Qc -= gop_structure[img->b_frame_to_code-1].hierarchy_layer;
          // check bounds
          prc->m_Qc = iClip3(prevqp - (input->HierarchicalCoding ? 0 : 5), prevqp + 5, prc->m_Qc); // control variation
          prc->m_Qc = iClip3(prc->RC_MIN_QUANT, prc->RC_MAX_QUANT, prc->m_Qc); // clipping
        }
        return prc->m_Qc;
      }
    }
    /*bottom field*/
    else
    {
      if( img->type==P_SLICE && generic_RC->NoGranularFieldRC == 0 && (IMG_NUMBER) )
        updateBottomField( prc );
      return prc->m_Qc;
    }
  }
  /*basic unit layer rate control*/
  else
  {
    /*top field of I frame*/
    if ((!IMG_NUMBER))
    {
      prc->m_Qc = prc->MyInitialQp;
      return prc->m_Qc;
    }
    else if( img->type == P_SLICE )
    {
      if((generic_RC->NumberofGOP==1)&&(generic_RC->NumberofPPicture==0))
      {
        if((generic_RC->FieldControl==0)||((generic_RC->FieldControl==1) && (generic_RC->NoGranularFieldRC==0)))
          return updateFirstP( prc, topfield );
      }
      else
      {
        prc->m_X1=prc->Pm_X1;
        prc->m_X2=prc->Pm_X2;
        prc->MADPictureC1=prc->PMADPictureC1;
        prc->MADPictureC2=prc->PMADPictureC2;

        m_Qp=prc->Pm_Qp;

        if(generic_RC->FieldControl==0)
          SumofBasicUnit=prc->TotalNumberofBasicUnit;
        else
          SumofBasicUnit=prc->TotalNumberofBasicUnit>>1;

        /*the average QP of the previous frame is used to coded the first basic unit of the current frame or field*/
        if(prc->NumberofBasicUnit==SumofBasicUnit)
          return updateFirstBU( prc, topfield );
        else
        {
          /*compute the number of remaining bits*/
          prc->Target -= (generic_RC->NumberofBasicUnitHeaderBits + generic_RC->NumberofBasicUnitTextureBits);
          generic_RC->NumberofBasicUnitHeaderBits  = 0;
          generic_RC->NumberofBasicUnitTextureBits = 0;
          if(prc->Target<0)
            return updateNegativeTarget( prc, topfield, m_Qp );
          else
          {
            /*predict the MAD of current picture*/
            predictCurrPicMAD( prc );

            /*compute the total number of bits for the current basic unit*/
            updateModelQPBU( prc, topfield, m_Qp );

            prc->TotalFrameQP +=prc->m_Qc;
            prc->Pm_Qp=prc->m_Qc;
            prc->NumberofBasicUnit--;
            if((prc->NumberofBasicUnit==0) && img->type == P_SLICE && (IMG_NUMBER) )
              updateLastBU( prc, topfield );

            return prc->m_Qc;
          }
        }
      }
    }
  }
  return prc->m_Qc;
}

void updateQPInterlace( rc_quadratic *prc )
{
  if(generic_RC->FieldControl==0)
  {
    /*previous choice is frame coding*/
    if(generic_RC->FieldFrame==1)
    {
      prc->PrevLastQP=prc->CurrLastQP;
      prc->CurrLastQP=prc->FrameQPBuffer;
    }
    /*previous choice is field coding*/
    else
    {
      prc->PrevLastQP=prc->CurrLastQP;
      prc->CurrLastQP=prc->FieldQPBuffer;
    }
  }
}

void updateQPNonPicAFF( rc_quadratic *prc )
{
  if(active_sps->frame_mbs_only_flag)
  {
    generic_RC->TotalQpforPPicture +=prc->m_Qc;
    prc->PrevLastQP=prc->CurrLastQP;
    prc->CurrLastQP=prc->m_Qc;
    prc->Pm_Qp=prc->m_Qc;
  }
  /*adaptive field/frame coding*/
  else
    prc->FrameQPBuffer=prc->m_Qc;
}

void updateBottomField( rc_quadratic *prc )
{
  /*field coding*/
  if(input->PicInterlace==FIELD_CODING)
  {
    generic_RC->TotalQpforPPicture +=prc->m_Qc;
    prc->PrevLastQP=prc->CurrLastQP+1;
    prc->CurrLastQP=prc->m_Qc;//+0 Recent change 13/1/2003
    prc->Pm_Qp=prc->m_Qc;
  }
  /*adaptive field/frame coding*/
  else
    prc->FieldQPBuffer=prc->m_Qc;
}

int updateFirstP( rc_quadratic *prc, int topfield )
{
  /*top field of the first P frame*/
  prc->m_Qc=prc->MyInitialQp;
  generic_RC->NumberofBasicUnitHeaderBits=0;
  generic_RC->NumberofBasicUnitTextureBits=0;
  prc->NumberofBasicUnit--;
  /*bottom field of the first P frame*/
  if((!topfield)&&(prc->NumberofBasicUnit==0))
  {
    /*frame coding or field coding*/
    if((active_sps->frame_mbs_only_flag)||(input->PicInterlace==FIELD_CODING))
    {
      generic_RC->TotalQpforPPicture +=prc->m_Qc;
      prc->PrevLastQP=prc->CurrLastQP;
      prc->CurrLastQP=prc->m_Qc;
      prc->PAveFrameQP=prc->m_Qc;
      prc->PAveHeaderBits3=prc->PAveHeaderBits2;
    }
    /*adaptive frame/field coding*/
    else if((input->PicInterlace==ADAPTIVE_CODING)||(input->MbInterlace))
    {
      if(generic_RC->FieldControl==0)
      {
        prc->FrameQPBuffer=prc->m_Qc;
        prc->FrameAveHeaderBits=prc->PAveHeaderBits2;
      }
      else
      {
        prc->FieldQPBuffer=prc->m_Qc;
        prc->FieldAveHeaderBits=prc->PAveHeaderBits2;
      }
    }
  }
  prc->Pm_Qp=prc->m_Qc;
  prc->TotalFrameQP +=prc->m_Qc;
  return prc->m_Qc;
}

int updateNegativeTarget( rc_quadratic *prc, int topfield, int m_Qp )
{
  int PAverageQP;

  if(prc->GOPOverdue==TRUE)
    prc->m_Qc=m_Qp+2;
  else
    prc->m_Qc=m_Qp+prc->DDquant;//2

  prc->m_Qc = imin(prc->m_Qc, prc->RC_MAX_QUANT);  // clipping
  if(input->basicunit>=prc->MBPerRow)
    prc->m_Qc = imin(prc->m_Qc, prc->PAveFrameQP + 6);
  else
    prc->m_Qc = imin(prc->m_Qc, prc->PAveFrameQP + 3);

  prc->TotalFrameQP +=prc->m_Qc;
  prc->NumberofBasicUnit--;
  if(prc->NumberofBasicUnit==0)
  {
    if((!topfield)||(generic_RC->FieldControl==0))
    {
      /*frame coding or field coding*/
      if((active_sps->frame_mbs_only_flag)||(input->PicInterlace==FIELD_CODING))
      {
        PAverageQP=(int)((double)prc->TotalFrameQP/(double)prc->TotalNumberofBasicUnit+0.5);
        if (generic_RC->NumberofPPicture == (input->intra_period - 2))
          prc->QPLastPFrame = PAverageQP;

        generic_RC->TotalQpforPPicture +=PAverageQP;
        if(prc->GOPOverdue==TRUE)
        {
          prc->PrevLastQP=prc->CurrLastQP+1;
          prc->CurrLastQP=PAverageQP;
        }
        else
        {
          if((generic_RC->NumberofPPicture==0)&&(generic_RC->NumberofGOP>1))
          {
            prc->PrevLastQP=prc->CurrLastQP;
            prc->CurrLastQP=PAverageQP;
          }
          else if(generic_RC->NumberofPPicture>0)
          {
            prc->PrevLastQP=prc->CurrLastQP+1;
            prc->CurrLastQP=PAverageQP;
          }
        }
        prc->PAveFrameQP=PAverageQP;
        prc->PAveHeaderBits3=prc->PAveHeaderBits2;
      }
      /*adaptive field/frame coding*/
      else if((input->PicInterlace==ADAPTIVE_CODING)||(input->MbInterlace))
      {
        if(generic_RC->FieldControl==0)
        {
          PAverageQP=(int)((double)prc->TotalFrameQP/(double)prc->TotalNumberofBasicUnit+0.5);
          prc->FrameQPBuffer=PAverageQP;
          prc->FrameAveHeaderBits=prc->PAveHeaderBits2;
        }
        else
        {
          PAverageQP=(int)((double)prc->TotalFrameQP/(double)prc->TotalNumberofBasicUnit+0.5);
          prc->FieldQPBuffer=PAverageQP;
          prc->FieldAveHeaderBits=prc->PAveHeaderBits2;
        }
      }
    }
  }
  if(prc->GOPOverdue==TRUE)
    prc->Pm_Qp=prc->PAveFrameQP;
  else
    prc->Pm_Qp=prc->m_Qc;

  return prc->m_Qc;
}

int updateFirstBU( rc_quadratic *prc, int topfield )
{
  /*adaptive field/frame coding*/
  if(((input->PicInterlace==ADAPTIVE_CODING)||(input->MbInterlace))&&(generic_RC->FieldControl==0))
  {
    /*previous choice is frame coding*/
    if(generic_RC->FieldFrame==1)
    {
      if(generic_RC->NumberofPPicture>0)
        generic_RC->TotalQpforPPicture +=prc->FrameQPBuffer;
      prc->PAveFrameQP=prc->FrameQPBuffer;
      prc->PAveHeaderBits3=prc->FrameAveHeaderBits;
    }
    /*previous choice is field coding*/
    else
    {
      if(generic_RC->NumberofPPicture>0)
        generic_RC->TotalQpforPPicture +=prc->FieldQPBuffer;
      prc->PAveFrameQP=prc->FieldQPBuffer;
      prc->PAveHeaderBits3=prc->FieldAveHeaderBits;
    }
  }

  if(prc->Target<=0)
  {
    prc->m_Qc = prc->PAveFrameQP + 2;
    if(prc->m_Qc > prc->RC_MAX_QUANT)
      prc->m_Qc = prc->RC_MAX_QUANT;

    if(topfield||(generic_RC->FieldControl==0))
      prc->GOPOverdue=TRUE;
  }
  else
  {
    prc->m_Qc=prc->PAveFrameQP;
  }
  prc->TotalFrameQP +=prc->m_Qc;
  prc->NumberofBasicUnit--;
  prc->Pm_Qp = prc->PAveFrameQP;

  return prc->m_Qc;
}

void updateLastBU( rc_quadratic *prc, int topfield )
{
  int PAverageQP;

  if((!topfield)||(generic_RC->FieldControl==0))
  {
    /*frame coding or field coding*/
    if((active_sps->frame_mbs_only_flag)||(input->PicInterlace==FIELD_CODING))
    {
      PAverageQP=(int)((double)prc->TotalFrameQP/(double) prc->TotalNumberofBasicUnit+0.5);
      if (generic_RC->NumberofPPicture == (input->intra_period - 2))
        prc->QPLastPFrame = PAverageQP;

      generic_RC->TotalQpforPPicture +=PAverageQP;
      prc->PrevLastQP=prc->CurrLastQP;
      prc->CurrLastQP=PAverageQP;
      prc->PAveFrameQP=PAverageQP;
      prc->PAveHeaderBits3=prc->PAveHeaderBits2;
    }
    else if((input->PicInterlace==ADAPTIVE_CODING)||(input->MbInterlace))
    {
      if(generic_RC->FieldControl==0)
      {
        PAverageQP=(int)((double) prc->TotalFrameQP/(double)prc->TotalNumberofBasicUnit+0.5);
        prc->FrameQPBuffer=PAverageQP;
        prc->FrameAveHeaderBits=prc->PAveHeaderBits2;
      }
      else
      {
        PAverageQP=(int)((double) prc->TotalFrameQP/(double) prc->TotalNumberofBasicUnit+0.5);
        prc->FieldQPBuffer=PAverageQP;
        prc->FieldAveHeaderBits=prc->PAveHeaderBits2;
      }
    }
  }
}

void predictCurrPicMAD( rc_quadratic *prc )
{
  int i;
  if(((input->PicInterlace==ADAPTIVE_CODING)||(input->MbInterlace))&&(generic_RC->FieldControl==1))
  {
    prc->CurrentFrameMAD=prc->MADPictureC1*prc->FCBUPFMAD[prc->TotalNumberofBasicUnit-prc->NumberofBasicUnit]+prc->MADPictureC2;
    prc->TotalBUMAD=0;
    for(i=prc->TotalNumberofBasicUnit-1; i>=(prc->TotalNumberofBasicUnit-prc->NumberofBasicUnit);i--)
    {
      prc->CurrentBUMAD=prc->MADPictureC1*prc->FCBUPFMAD[i]+prc->MADPictureC2;
      prc->TotalBUMAD +=prc->CurrentBUMAD*prc->CurrentBUMAD;
    }
  }
  else
  {
    prc->CurrentFrameMAD=prc->MADPictureC1*prc->BUPFMAD[prc->TotalNumberofBasicUnit-prc->NumberofBasicUnit]+prc->MADPictureC2;
    prc->TotalBUMAD=0;
    for(i=prc->TotalNumberofBasicUnit-1; i>=(prc->TotalNumberofBasicUnit-prc->NumberofBasicUnit);i--)
    {
      prc->CurrentBUMAD=prc->MADPictureC1*prc->BUPFMAD[i]+prc->MADPictureC2;
      prc->TotalBUMAD +=prc->CurrentBUMAD*prc->CurrentBUMAD;
    }
  }
}

void updateModelQPBU( rc_quadratic *prc, int topfield, int m_Qp )
{
  double dtmp, m_Qstep;
  int m_Bits;
  /*compute the total number of bits for the current basic unit*/
  m_Bits =(int)(prc->Target * prc->CurrentFrameMAD * prc->CurrentFrameMAD / prc->TotalBUMAD);
  /*compute the number of texture bits*/
  m_Bits -=prc->PAveHeaderBits2;

  m_Bits=imax(m_Bits,(int)(prc->bit_rate/(MINVALUE*prc->frame_rate*prc->TotalNumberofBasicUnit)));

  dtmp = prc->CurrentFrameMAD * prc->CurrentFrameMAD * prc->m_X1 * prc->m_X1 \
    + 4 * prc->m_X2 * prc->CurrentFrameMAD * m_Bits;
  if ((prc->m_X2 == 0.0) || (dtmp < 0) || ((sqrt (dtmp) - prc->m_X1 * prc->CurrentFrameMAD) <= 0.0))  // fall back 1st order mode
    m_Qstep = (float)(prc->m_X1 * prc->CurrentFrameMAD / (double) m_Bits);
  else // 2nd order mode
    m_Qstep = (float) ((2 * prc->m_X2 * prc->CurrentFrameMAD) / (sqrt (dtmp) - prc->m_X1 * prc->CurrentFrameMAD));

  prc->m_Qc = Qstep2QP(m_Qstep);
  prc->m_Qc = imin(m_Qp+prc->DDquant,  prc->m_Qc); // control variation

  if(input->basicunit>=prc->MBPerRow)
    prc->m_Qc = imin(prc->PAveFrameQP+6, prc->m_Qc);
  else
    prc->m_Qc = imin(prc->PAveFrameQP+3, prc->m_Qc);

  prc->m_Qc = iClip3(m_Qp-prc->DDquant, prc->RC_MAX_QUANT, prc->m_Qc); // clipping
  if(input->basicunit>=prc->MBPerRow)
    prc->m_Qc = imax(prc->PAveFrameQP-6, prc->m_Qc);
  else
    prc->m_Qc = imax(prc->PAveFrameQP-3, prc->m_Qc);

  prc->m_Qc = imax(prc->RC_MIN_QUANT, prc->m_Qc);
}

void updateQPInterlaceBU( rc_quadratic *prc )
{
  /*previous choice is frame coding*/
  if(generic_RC->FieldFrame==1)
  {
    generic_RC->TotalQpforPPicture +=prc->FrameQPBuffer;
    prc->Pm_Qp=prc->FrameQPBuffer;
  }
  /*previous choice is field coding*/
  else
  {
    generic_RC->TotalQpforPPicture +=prc->FieldQPBuffer;
    prc->Pm_Qp=prc->FieldQPBuffer;
  }
}

void updateModelQPFrame( rc_quadratic *prc, int m_Bits )
{
  double dtmp, m_Qstep;

  dtmp = prc->CurrentFrameMAD * prc->m_X1 * prc->CurrentFrameMAD * prc->m_X1
    + 4 * prc->m_X2 * prc->CurrentFrameMAD * m_Bits;
  if ((prc->m_X2 == 0.0) || (dtmp < 0) || ((sqrt (dtmp) - prc->m_X1 * prc->CurrentFrameMAD) <= 0.0)) // fall back 1st order mode
    m_Qstep = (float) (prc->m_X1 * prc->CurrentFrameMAD / (double) m_Bits);
  else // 2nd order mode
    m_Qstep = (float) ((2 * prc->m_X2 * prc->CurrentFrameMAD) / (sqrt (dtmp) - prc->m_X1 * prc->CurrentFrameMAD));

  prc->m_Qc = Qstep2QP(m_Qstep);
}
