
/*!
 ***************************************************************************
 * \file ratectl.c
 *
 * \brief
 *    Rate Control algorithm
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *     - Siwei Ma <swma@jdl.ac.cn>
 *     - Zhengguo LI<ezgli@lit.a-star.edu.sg>
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

/*!
 *************************************************************************************
 * \brief
 *    Update Rate Control Parameters
 *************************************************************************************
 */
void update_rc(Macroblock *currMB, short best_mode)
{
  generic_RC->MADofMB[img->current_mb_nr] = calc_MAD();

  if(input->basicunit < img->FrameSizeInMbs)
  {
    generic_RC->TotalMADBasicUnit += generic_RC->MADofMB[img->current_mb_nr];

    // delta_qp is present only for non-skipped macroblocks
    if ((currMB->cbp!=0 || best_mode==I16MB) && (best_mode!=IPCM))
      currMB->prev_cbp = 1;
    else
    {
      currMB->delta_qp = 0;
      currMB->qp = currMB->prev_qp;
      img->qp = currMB->qp;
      currMB->prev_cbp = 0;
    }

    if (input->MbInterlace)
    {
      // update rdopt buffered qps...
      rdopt->delta_qp = currMB->delta_qp;
      rdopt->qp = currMB->qp;
      rdopt->prev_cbp = currMB->prev_cbp;

      delta_qp_mbaff[currMB->mb_field][img->bot_MB] = currMB->delta_qp;
      qp_mbaff      [currMB->mb_field][img->bot_MB] = currMB->qp;
    }
  }
  set_chroma_qp(currMB);
}

/*!
 *************************************************************************************
 * \brief
 *    map QP to Qstep
 *
 *************************************************************************************
*/
double QP2Qstep( int QP )
{
  int i;
  double Qstep;
  static const double QP2QSTEP[6] = { 0.625, 0.6875, 0.8125, 0.875, 1.0, 1.125 };

  Qstep = QP2QSTEP[QP % 6];
  for( i=0; i<(QP/6); i++)
    Qstep *= 2;

  return Qstep;
}


/*!
 *************************************************************************************
 * \brief
 *    map Qstep to QP
 *
 *************************************************************************************
*/
int Qstep2QP( double Qstep )
{
  int q_per = 0, q_rem = 0;

  //  assert( Qstep >= QP2Qstep(0) && Qstep <= QP2Qstep(51) );
  if( Qstep < QP2Qstep(0))
    return 0;
  else if (Qstep > QP2Qstep(51) )
    return 51;

  while( Qstep > QP2Qstep(5) )
  {
    Qstep /= 2.0;
    q_per += 1;
  }

  if (Qstep <= 0.65625)
  {
    Qstep = 0.625;
    q_rem = 0;
  }
  else if (Qstep <= 0.75)
  {
    Qstep = 0.6875;
    q_rem = 1;
  }
  else if (Qstep <= 0.84375)
  {
    Qstep = 0.8125;
    q_rem = 2;
  }
  else if (Qstep <= 0.9375)
  {
    Qstep = 0.875;
    q_rem = 3;
  }
  else if (Qstep <= 1.0625)
  {
    Qstep = 1.0;
    q_rem = 4;
  }
  else
  {
    Qstep = 1.125;
    q_rem = 5;
  }

  return (q_per * 6 + q_rem);
}

/*!
 ************************************************************************************
 * \brief
 *    calculate MAD for the current macroblock
 *
 * \return
 *    calculated MAD
 *
 *************************************************************************************
*/
int calc_MAD()
{
  int k, l, sum = 0;

  for (k = 0; k < 16; k++)
    for (l = 0; l < 16; l++)
      sum += iabs(diffy[k][l]);

  return sum;
}

/*!
 *************************************************************************************
 * \brief
 *    Compute Frame MAD
 *
 *************************************************************************************
*/
double ComputeFrameMAD()
{
  int64 TotalMAD = 0;
  unsigned int i;
  for(i = 0; i < img->FrameSizeInMbs; i++)
    TotalMAD += generic_RC->MADofMB[i];
  return (double)TotalMAD / (256.0 * (double)img->FrameSizeInMbs);
}


/*!
 *************************************************************************************
 * \brief
 *    Copy JVT rate control objects
 *
 *************************************************************************************
*/
void copy_rc_generic( rc_generic *dst, rc_generic *src )
{
  /* buffer original addresses for which memory has been allocated */
  int *tmpMADofMB = dst->MADofMB;

  /* copy object */
  memcpy( (void *)dst, (void *)src, sizeof(rc_generic) );

  /* restore original addresses */
  dst->MADofMB = tmpMADofMB;

  /* copy MADs */
  memcpy( (void *)dst->MADofMB, (void *)src->MADofMB, img->FrameSizeInMbs * sizeof (int) );
}

/*!
 *************************************************************************************
 * \brief
 *    Dynamically allocate memory needed for generic rate control
 *
 *************************************************************************************
 */
void generic_alloc( rc_generic **prc )
{
  *prc = (rc_generic *) malloc ( sizeof( rc_generic ) );
  if (NULL==*prc)
  {
    no_mem_exit("init_global_buffers: generic_alloc");
  }
  (*prc)->MADofMB = (int *) calloc (img->FrameSizeInMbs, sizeof (int));
  if (NULL==(*prc)->MADofMB)
  {
    no_mem_exit("init_global_buffers: (*prc)->MADofMB");
  }
  (*prc)->FieldFrame = 1;
}


/*!
 *************************************************************************************
 * \brief
 *    Free memory needed for generic rate control
 *
 *************************************************************************************
 */
void generic_free(rc_generic **prc)
{
  if (NULL!=(*prc)->MADofMB)
  {
    free ((*prc)->MADofMB);
    (*prc)->MADofMB = NULL;
  }
  if (NULL!=(*prc))
  {
    free ((*prc));
    (*prc) = NULL;
  }
}
