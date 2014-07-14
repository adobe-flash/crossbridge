
/*!
*************************************************************************************
* \file weighted_prediction.c
*
* \brief
*    Estimate weights for WP
*
* \author
*    Main contributors (see contributors.h for copyright, address and affiliation details)
*     - Alexis Michael Tourapis         <alexismt@ieee.org>
*     - Athanasios Leontaris            <aleon@dolby.com>
*************************************************************************************
*/
#include <stdlib.h>
#include "contributors.h"

#include "global.h"
#include <memory.h>
#include "image.h"


static SubImageContainer ref_pic_sub;
static SubImageContainer ref_qpic_sub;


/*!
************************************************************************
* \brief
*    Estimates reference picture weighting factors
************************************************************************
*/

void estimate_weighting_factor_P_slice(int select_offset)
{
  int i, j, n;

  double dc_org = 0.0;
  int index;
  int comp;
  double dc_ref[MAX_REFERENCE_PICTURES];

  int default_weight[3];

  int list_offset   = ((img->MbaffFrameFlag)&&(img->mb_data[img->current_mb_nr].mb_field))? (img->current_mb_nr & 0x01) ? 4 : 2 : 0;
  int weight[2][MAX_REFERENCE_PICTURES][3];
  int offset[2][MAX_REFERENCE_PICTURES][3];
  int clist;

  imgpel **tmpPtr;

  luma_log_weight_denom = 5;
  chroma_log_weight_denom = 5;
  wp_luma_round = 1 << (luma_log_weight_denom - 1);
  wp_chroma_round = 1 << (chroma_log_weight_denom - 1);
  default_weight[0] = 1<<luma_log_weight_denom;
  default_weight[1] = default_weight[2] = 1<<chroma_log_weight_denom;

  /* set all values to defaults */
  for (i = 0; i < 2 + list_offset; i++)
  {
    for (j = 0; j < listXsize[i]; j++)
    {
      for (n = 0; n < 3; n++)
      {
        weight[i][j][n] = default_weight[n];
        wp_weight[i][j][n] = default_weight[n];
        wp_offset[i][j][n] = 0;
        offset[i][j][n] = 0;
      }
    }
  }

  for (i = 0; i < img->height; i++)
  {
    for (j = 0; j < img->width; j++)
    {
      dc_org += (double) imgY_org[i][j];
    }
  }

  for (clist=0; clist<2 + list_offset; clist++)
  {
    for (n = 0; n < listXsize[clist]; n++)
    {
      dc_ref[n] = 0.0;

      ref_pic_sub.luma       = listX[clist][n]->imgY_sub;
      tmpPtr = ref_pic_sub.luma[0][0];

      // Y
      for (j = IMG_PAD_SIZE; j < img->height + IMG_PAD_SIZE; j++)
      {
        for ( i = IMG_PAD_SIZE; i < img->width + IMG_PAD_SIZE; i++ )
        {
          dc_ref[n] += (double) tmpPtr[j][i];
        }
      }

      if (select_offset==0.0)
      {
        if (dc_ref[n] != 0)
          weight[clist][n][0] = (int) (default_weight[0] * dc_org / dc_ref[n] + 0.5);
        else
          weight[clist][n][0] = default_weight[0];  // only used when reference picture is black
        if (weight[clist][n][0] < -64 || weight[clist][n][0] >127)
          weight[clist][n][0] = default_weight[0];
      }
      else
      {
        offset[clist][n][0] = (int) ((dc_org-dc_ref[n])/(img->size)+0.5);
        offset[clist][n][0] = (offset[clist][n][0]<-128) ? -128: (offset[clist][n][0]>127) ? 127:offset[clist][n][0];
        weight[clist][n][0] = default_weight[0];
      }


      /* for now always use default weight for chroma weight */
      weight[clist][n][1] = default_weight[1];
      weight[clist][n][2] = default_weight[2];
    }
  }

  for (clist=0; clist<2 + list_offset; clist++)
  {
    for (index = 0; index < listXsize[clist]; index++)
    {
      for (comp=0; comp < 3; comp ++)
      {
        wp_weight[clist][index][comp] = weight[clist][index][comp];
        wp_offset[clist][index][comp] = offset[clist][index][comp];
        // printf("index %d component %d weight %d offset %d\n",index,comp,weight[0][index][comp],offset[0][index][comp]);
      }
    }
  }

}

/*!
************************************************************************
* \brief
*    Estimates reference picture weighting factors
************************************************************************
*/
void estimate_weighting_factor_B_slice()
{
  int i, j, n;

  int tx,DistScaleFactor;
  double dc_org = 0.0;
  int index;
  int comp;
  double dc_ref[6][MAX_REFERENCE_PICTURES];

  int log_weight_denom;

  int default_weight[3];
  int list_offset   = ((img->MbaffFrameFlag)&&(img->mb_data[img->current_mb_nr].mb_field))? (img->current_mb_nr & 0x01) ? 4 : 2 : 0;
  int weight[6][MAX_REFERENCE_PICTURES][3];
  int offset[6][MAX_REFERENCE_PICTURES][3];
  int im_weight[6][MAX_REFERENCE_PICTURES][MAX_REFERENCE_PICTURES][3];
  int im_offset[6][MAX_REFERENCE_PICTURES][MAX_REFERENCE_PICTURES][3];
  int clist;
  int wf_weight, wf_offset;
  imgpel **tmpPtr;

  if (active_pps->weighted_bipred_idc == 2) //! implicit mode
  {
    luma_log_weight_denom = 5;
    chroma_log_weight_denom = 5;
  }
  else
  {
    //luma_log_weight_denom = 6;
    //chroma_log_weight_denom = 6;
    luma_log_weight_denom = 5;
    chroma_log_weight_denom = 5;
  }

  wp_luma_round = 1 << (luma_log_weight_denom - 1);
  wp_chroma_round = 1 << (chroma_log_weight_denom - 1);
  default_weight[0] = 1<<luma_log_weight_denom;
  default_weight[1] = 1<<chroma_log_weight_denom;
  default_weight[2] = 1<<chroma_log_weight_denom;

  /* set all values to defaults */
  for (i = 0; i < 2 + list_offset; i++)
  {
    for (j = 0; j < listXsize[i]; j++)
    {
      for (n = 0; n < 3; n++)
      {
        wp_weight[i][j][n] = default_weight[n];
        wp_offset[i][j][n] = 0;
        offset   [i][j][n] = 0;
        weight   [i][j][n] = default_weight[n];
      }
    }
  }

  for (i = 0; i < listXsize[LIST_0]; i++)
  {
    for (j = 0; j < listXsize[LIST_1]; j++)
    {
      int td, tb;
      td = iClip3(-128,127,(listX[LIST_1][j]->poc - listX[LIST_0][i]->poc));
      tb = iClip3(-128,127,(enc_picture->poc - listX[LIST_0][i]->poc));
      for (comp = 0; comp < 3; comp++)
      {
        // implicit weights
        if (td == 0)
        {
          im_weight[1][i][j][comp] = default_weight[comp];
          im_weight[0][i][j][comp] = default_weight[comp];
          im_offset[1][i][j][comp] = 0;
          im_offset[0][i][j][comp] = 0;
        }
        else
        {
          tx = (16384 + iabs(td/2))/td;
          DistScaleFactor = iClip3(-1024, 1023, (tx*tb + 32 )>>6);
          im_weight[1][i][j][comp] = DistScaleFactor>>2;
          if (im_weight[1][i][j][comp] < -64 || im_weight[1][i][j][comp] >128)
            im_weight[1][i][j][comp] = default_weight[comp];
          im_weight[0][i][j][comp] = 64 - im_weight[1][i][j][comp];
          im_offset[1][i][j][comp] = 0;
          im_offset[0][i][j][comp] = 0;
        }
      }
      /*
      printf ("%d imp weight[%d][%d] = %d  , %d (%d %d %d) (%d %d) (%d %d)\n",enc_picture->poc, i, j,  im_weight[0][i][j][0], im_weight[1][i][j][0],
        enc_picture->poc,listX[LIST_0][i]->poc, listX[LIST_1][j]->poc,
        DistScaleFactor ,tx,td,tb);
      */
    }
  }


  if (active_pps->weighted_bipred_idc == 2) //! implicit mode
  {

    for (i = 0; i < listXsize[LIST_0]; i++)
    {
      for (j = 0; j < listXsize[LIST_1]; j++)
      {
        for (comp = 0; comp < 3; comp++)
        {
          log_weight_denom = (comp == 0) ? luma_log_weight_denom : chroma_log_weight_denom;
          wbp_weight[1][i][j][comp] = im_weight[1][i][j][comp] ;
          wbp_weight[0][i][j][comp] = im_weight[0][i][j][comp];
        }
      }
    }

    for (clist=0; clist<2 + list_offset; clist++)
    {
      for (index = 0; index < listXsize[clist]; index++)
      {
        wp_weight[clist][index][0] = default_weight[0];
        wp_weight[clist][index][1] = default_weight[1];
        wp_weight[clist][index][2] = default_weight[2];
        wp_offset[clist][index][0] = 0;
        wp_offset[clist][index][1] = 0;
        wp_offset[clist][index][2] = 0;
      }
    }
  }
  else
  {
    for (i = 0; i < img->height; i++)
    {
      for (j = 0; j < img->width; j++)
      {
        dc_org += (double) imgY_org[i][j];
      }
    }

    for (clist=0; clist<2 + list_offset; clist++)
    {
      for (n = 0; n < listXsize[clist]; n++)
      {
        dc_ref[clist][n] = 0;

        ref_qpic_sub.luma      = listX[clist][n]->imgY_sub;
        tmpPtr = ref_qpic_sub.luma[0][0];
        // Y
        for (j = IMG_PAD_SIZE; j < (img->height + IMG_PAD_SIZE); j++)
        {
          for (i = IMG_PAD_SIZE; i < img->width + IMG_PAD_SIZE; i++)
          {
            dc_ref[clist][n] += (double) tmpPtr[j][i];
          }
        }
        if (dc_ref[clist][n] != 0.0)
          wf_weight = (int) (default_weight[0] * dc_org / dc_ref[clist][n] + 0.5);
        else
          wf_weight = default_weight[0];  // only used when reference picture is black

        //wf_weight = iClip3(-64, 64, wf_weight);

        if ( (wf_weight<-128) || (wf_weight>127) )
        {
          wf_weight = default_weight[0];
        }
        wf_offset = 0;

        //    printf("dc_org = %d, dc_ref = %d, weight[%d] = %d\n",dc_org, dc_ref[n],n,weight[n][0]);

        weight[clist][n][0] = wf_weight;
        weight[clist][n][1] = default_weight[1];
        weight[clist][n][2] = default_weight[2];
        offset[clist][n][0] = 0;
        offset[clist][n][1] = 0;
        offset[clist][n][2] = 0;

      }
    }

    if (active_pps->weighted_bipred_idc == 1)
    {
      for (clist=0; clist<2 + list_offset; clist++)
      {
        for (index = 0; index < listXsize[clist]; index++)
        {
          for (comp = 0; comp < 3; comp++)
          {
            wp_weight[clist][index][comp] = weight[clist][index][comp];
            wp_offset[clist][index][comp] = offset[clist][index][comp];
            //printf("%d %d\n",wp_weight[clist][index][comp],wp_offset[clist][index][comp]);
          }
        }
      }
    }
    else
    {
      for (clist=0; clist<2 + list_offset; clist++)
      {
        for (index = 0; index < listXsize[clist]; index++)
        {
          wp_weight[clist][index][0] = default_weight[0];
          wp_weight[clist][index][1] = default_weight[1];
          wp_weight[clist][index][2] = default_weight[2];
          wp_offset[clist][index][0] = 0;
          wp_offset[clist][index][1] = 0;
          wp_offset[clist][index][2] = 0;
        }
      }
    }
    for (i = 0; i < listXsize[LIST_0]; i++)
    {
      for (j = 0; j < listXsize[LIST_1]; j++)
      {
        for (comp = 0; comp < 3; comp++)
        {
          log_weight_denom = (comp == 0) ? luma_log_weight_denom : chroma_log_weight_denom;
          wbp_weight[0][i][j][comp] = wp_weight[0][i][comp];
          wbp_weight[1][i][j][comp] = wp_weight[1][j][comp];
        }
        /*
        printf ("bpw weight[%d][%d] = %d  , %d (%d %d %d) (%d %d) (%d %d)\n", i, j, wbp_weight[0][i][j][0], wbp_weight[1][i][j][0],
          enc_picture->poc,listX[LIST_0][i]->poc, listX[LIST_1][j]->poc,
          DistScaleFactor ,tx,tx,tx);
        */
      }
    }
  }
}


/*!
************************************************************************
* \brief
*    Tests P slice weighting factors to perform or not WP RD decision
************************************************************************
*/

int test_wp_P_slice(int select_offset)
{
  int i, j, n;

  double dc_org = 0.0;
  int index;
  int comp;
  double dc_ref[MAX_REFERENCE_PICTURES];

  int default_weight;
  int default_weight_chroma;
  int list_offset   = ((img->MbaffFrameFlag)&&(img->mb_data[img->current_mb_nr].mb_field))? (img->current_mb_nr & 0x01) ? 4 : 2 : 0;
  int weight[2][MAX_REFERENCE_PICTURES][3];
  int offset[2][MAX_REFERENCE_PICTURES][3];
  int clist;
  int perform_wp = 0;
  imgpel **tmpPtr;


  luma_log_weight_denom = 5;
  chroma_log_weight_denom = 5;
  wp_luma_round = 1 << (luma_log_weight_denom - 1);
  wp_chroma_round = 1 << (chroma_log_weight_denom - 1);
  default_weight = 1<<luma_log_weight_denom;
  default_weight_chroma = 1<<chroma_log_weight_denom;

  /* set all values to defaults */
  for (i = 0; i < 2 + list_offset; i++)
  {
    for (j = 0; j < listXsize[i]; j++)
    {
      for (n = 0; n < 3; n++)
      {
        weight[i][j][n] = default_weight;
        wp_weight[i][j][n] = default_weight;
        wp_offset[i][j][n] = 0;
        offset[i][j][n] = 0;
      }
    }
  }

  for (i = 0; i < img->height; i++)
  {
    for (j = 0; j < img->width; j++)
    {
      dc_org += (double) imgY_org[i][j];
    }
  }

  for (clist=0; clist<2 + list_offset; clist++)
  {
    for (n = 0; n < listXsize[clist]; n++)
    {
      dc_ref[n] = 0.0;

      ref_pic_sub.luma       = listX[clist][n]->imgY_sub;
      tmpPtr = ref_pic_sub.luma[0][0];
      // Y
      for (j = IMG_PAD_SIZE; j < img->height + IMG_PAD_SIZE; j++)
      {
        for (i = IMG_PAD_SIZE; i < img->width + IMG_PAD_SIZE; i++)
        {
          dc_ref[n] += (double) tmpPtr[j][i];
        }
      }

      if (select_offset==0)
      {
        if (dc_ref[n] != 0.0)
          weight[clist][n][0] = (int) (default_weight * dc_org / dc_ref[n] + 0.5);
        else
          weight[clist][n][0] = default_weight;  // only used when reference picture is black
        if (weight[clist][n][0] < -64 || weight[clist][n][0] >127)
          weight[clist][n][0] = 32;
      }
      else
      {
        offset[clist][n][0] = (int) ((dc_org-dc_ref[n])/(img->size)+0.5);
        offset[clist][n][0] = (offset[clist][n][0]<-128) ? -128: (offset[clist][n][0]>127) ? 127:offset[clist][n][0];
        weight[clist][n][0] = default_weight;
      }

      /* for now always use default weight for chroma weight */
      weight[clist][n][1] = default_weight_chroma;
      weight[clist][n][2] = default_weight_chroma;

    }
  }

  for (clist=0; clist<2 + list_offset; clist++)
  {
    for (index = 0; index < listXsize[clist]; index++)
    {
      for (comp=0; comp < 3; comp ++)
      {
        int offset_test = input->RDPSliceBTest && active_sps->profile_idc != 66
          ? iabs(offset[clist][index][comp]) > 2
          : offset[clist][index][comp] != 0;

        if (weight[clist][index][comp] != default_weight ||  offset_test)
        {
          perform_wp = 1;
          break;
        }
      }
      if (perform_wp == 1) break;
    }
    if (perform_wp == 1) break;
  }

  return perform_wp;
}

/*!
************************************************************************
* \brief
*    test_wp_B_slice:
*    Tests B slice weighting prediction
************************************************************************
*/
int test_wp_B_slice(int select_method)
{
  int i, j, n;

  int tx,DistScaleFactor;
  double dc_org = 0.0;
  int index;
  int comp;
  double dc_ref[6][MAX_REFERENCE_PICTURES];

  int log_weight_denom;

  int default_weight[3];
  // this needs to be fixed.
  int list_offset   = ((img->MbaffFrameFlag)&&(img->mb_data[img->current_mb_nr].mb_field))? (img->current_mb_nr & 0x01) ? 4 : 2 : 0;
  int weight[6][MAX_REFERENCE_PICTURES][3];
  int offset[6][MAX_REFERENCE_PICTURES][3];
  int im_weight[6][MAX_REFERENCE_PICTURES][MAX_REFERENCE_PICTURES][3];
  int im_offset[6][MAX_REFERENCE_PICTURES][MAX_REFERENCE_PICTURES][3];
  int clist;
  int wf_weight, wf_offset;
  int perform_wp = 0;
  imgpel **tmpPtr;

  if (select_method == 1) //! implicit mode
  {
    luma_log_weight_denom = 5;
    chroma_log_weight_denom = 5;
  }
  else
  {
    luma_log_weight_denom = 6;
    chroma_log_weight_denom = 6;
  }

  wp_luma_round = 1 << (luma_log_weight_denom - 1);
  wp_chroma_round = 1 << (chroma_log_weight_denom - 1);
  default_weight[0] = 1<<luma_log_weight_denom;
  default_weight[1] = 1<<chroma_log_weight_denom;
  default_weight[2] = 1<<chroma_log_weight_denom;

  /* set all values to defaults */
  for (i = 0; i < 2 + list_offset; i++)
  {
    for (j = 0; j < listXsize[i]; j++)
    {
      for (n = 0; n < 3; n++)
      {
        wp_weight[i][j][n] = default_weight[n];
        wp_offset[i][j][n] = 0;
        offset   [i][j][n] = 0;
        weight   [i][j][n] = default_weight[n];
      }
    }
  }

  for (i = 0; i < listXsize[LIST_0]; i++)
  {
    for (j = 0; j < listXsize[LIST_1]; j++)
    {
      int td, tb;
      td = iClip3(-128,127,(listX[LIST_1][j]->poc - listX[LIST_0][i]->poc));
      tb = iClip3(-128,127,(enc_picture->poc - listX[LIST_0][i]->poc));
      for (comp = 0; comp < 3; comp++)
      {
        // implicit weights
        if (td == 0)
        {
          im_weight[1][i][j][comp] = default_weight[comp];
          im_weight[0][i][j][comp] = default_weight[comp];
          im_offset[1][i][j][comp] = 0;
          im_offset[0][i][j][comp] = 0;
        }
        else
        {
          tx = (16384 + iabs(td/2))/td;
          DistScaleFactor = iClip3(-1024, 1023, (tx*tb + 32 )>>6);
          im_weight[1][i][j][comp] = DistScaleFactor>>2;
          if (im_weight[1][i][j][comp] < -64 || im_weight[1][i][j][comp] >128)
            im_weight[1][i][j][comp] = 32;
          im_weight[0][i][j][comp] = 64 - im_weight[1][i][j][comp];
          im_offset[1][i][j][comp] = 0;
          im_offset[0][i][j][comp] = 0;
        }
      }
    }
  }


  if (select_method == 1) //! implicit mode
  {
    for (i = 0; i < listXsize[LIST_0]; i++)
    {
      for (j = 0; j < listXsize[LIST_1]; j++)
      {
        for (comp = 0; comp < 3; comp++)
        {
          log_weight_denom = (comp == 0) ? luma_log_weight_denom : chroma_log_weight_denom;
          wbp_weight[1][i][j][comp] = im_weight[1][i][j][comp] ;
          wbp_weight[0][i][j][comp] = im_weight[0][i][j][comp];
        }
      }
    }

    for (clist=0; clist<2 + list_offset; clist++)
    {
      for (index = 0; index < listXsize[clist]; index++)
      {
        wp_weight[clist][index][0] = default_weight[0];
        wp_weight[clist][index][1] = default_weight[1];
        wp_weight[clist][index][2] = default_weight[2];
        wp_offset[clist][index][0] = 0;
        wp_offset[clist][index][1] = 0;
        wp_offset[clist][index][2] = 0;
      }
    }
  }
  else
  {
    for (i = 0; i < img->height; i++)
    {
      for (j = 0; j < img->width; j++)
      {
        dc_org += (double) imgY_org[i][j];
      }
    }

    for (clist=0; clist<2 + list_offset; clist++)
    {
      for (n = 0; n < listXsize[clist]; n++)
      {
        dc_ref[clist][n] = 0;
        ref_pic_sub.luma       = listX[clist][n]->imgY_sub;
        tmpPtr = ref_pic_sub.luma[0][0];

        // Y
        for (j = IMG_PAD_SIZE; j < img->height + IMG_PAD_SIZE; j++)
        {
          for (i = IMG_PAD_SIZE; i < img->width + IMG_PAD_SIZE; i++)
          {
            dc_ref[clist][n] += (double) tmpPtr[j][i];
          }
        }

        if (dc_ref[clist][n] != 0.0)
          wf_weight = (int) (default_weight[0] * dc_org / dc_ref[clist][n] + 0.5);
        else
          wf_weight = default_weight[0];  // only used when reference picture is black

        if ( (wf_weight<-64) || (wf_weight>127) )
        {
          wf_weight = default_weight[0];
        }
        wf_offset = 0;


        weight[clist][n][0] = wf_weight;
        weight[clist][n][1] = default_weight[1];
        weight[clist][n][2] = default_weight[2];
        offset[clist][n][0] = 0;
        offset[clist][n][1] = 0;
        offset[clist][n][2] = 0;

      }
    }

    if (select_method == 0) //! explicit mode
    {
      for (clist=0; clist<2 + list_offset; clist++)
      {
        for (index = 0; index < listXsize[clist]; index++)
        {
          for (comp = 0; comp < 3; comp++)
          {
            wp_weight[clist][index][comp] = weight[clist][index][comp];
            wp_offset[clist][index][comp] = offset[clist][index][comp];
          }
        }
      }
    }
    else
    {
      for (clist=0; clist<2 + list_offset; clist++)
      {
        for (index = 0; index < listXsize[clist]; index++)
        {
          wp_weight[clist][index][0] = default_weight[0];
          wp_weight[clist][index][1] = default_weight[1];
          wp_weight[clist][index][2] = default_weight[2];
          wp_offset[clist][index][0] = 0;
          wp_offset[clist][index][1] = 0;
          wp_offset[clist][index][2] = 0;
        }
      }
    }
    for (i = 0; i < listXsize[LIST_0]; i++)
    {
      for (j = 0; j < listXsize[LIST_1]; j++)
      {
        for (comp = 0; comp < 3; comp++)
        {
          log_weight_denom = (comp == 0) ? luma_log_weight_denom : chroma_log_weight_denom;
          wbp_weight[0][i][j][comp] = wp_weight[0][i][comp];
          wbp_weight[1][i][j][comp] = wp_weight[1][j][comp];
        }
        /*
        printf ("bpw weight[%d][%d] = %d  , %d (%d %d %d) (%d %d) (%d %d)\n", i, j, wbp_weight[0][i][j][0], wbp_weight[1][i][j][0],
          enc_picture->poc,listX[LIST_0][i]->poc, listX[LIST_1][j]->poc,
          DistScaleFactor ,tx,tx,tx);
        */
      }
    }
  }

  if (select_method == 0) //! implicit mode
  {
    int active_refs[2];

    active_refs[0]=input->B_List0_refs == 0 ? listXsize[0] : imin(input->B_List0_refs,listXsize[0]);
    active_refs[1]=input->B_List1_refs == 0 ? listXsize[1] : imin(input->B_List0_refs,listXsize[1]);

    for (clist=0; clist<2 + list_offset; clist++)
    {
      for (index = 0; index < active_refs[clist]; index++)
      {
        for (comp=0; comp < 3; comp ++)
        {
          if (wp_weight[clist][index][comp] != default_weight[comp])
          {
            perform_wp = 1;
            break;
          }
        }
        if (perform_wp == 1) break;
      }
      if (perform_wp == 1) break;
    }
  }
  return perform_wp;
}


