
/*!
*************************************************************************************
* \file me_fullfast.c
*
* \brief
*    Motion Estimation using Full Search Fast
*
* \author
*    Main contributors (see contributors.h for copyright, address and affiliation details)
*      - Alexis Michael Tourapis <alexismt@ieee.org>
*
*************************************************************************************
*/

// Includes
#include "contributors.h"
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include "global.h"
#include "image.h"
#include "memalloc.h"
#include "mb_access.h"
#include "refbuf.h"

#include "me_distortion.h"
#include "me_fullsearch.h"

// Define External Global Parameters
extern int *mvbits;
extern short*   spiral_search_x;
extern short*   spiral_search_y;
extern int *byte_abs;
extern const int LEVELMVLIMIT[17][6];

/*****
 *****  static variables for fast integer motion estimation
 *****
 */
static int  **search_setup_done;  //!< flag if all block SAD's have been calculated yet
static int  **search_center_x;    //!< absolute search center for fast full motion search
static int  **search_center_y;    //!< absolute search center for fast full motion search
static int  **pos_00;             //!< position of (0,0) vector
static distpel  *****BlockSAD;        //!< SAD for all blocksize, ref. frames and motion vectors
static int  **max_search_range;

extern void SetMotionVectorPredictor (short  pmv[2], signed char **refPic,
                                      short  ***tmp_mv, short  ref_frame,
                                      int    list, int    block_x,
                                      int    block_y, int    blockshape_x,
                                      int    blockshape_y);

// Functions
/*!
 ***********************************************************************
 * \brief
 *    Full pixel block motion search
 ***********************************************************************
 */


/*!
 ***********************************************************************
 * \brief
 *    function creating arrays for fast integer motion estimation
 ***********************************************************************
 */
void
InitializeFastFullIntegerSearch ()
{
  int  i, j, k, list;
  int  search_range = input->search_range;
  int  max_pos      = (2*search_range+1) * (2*search_range+1);

  if ((BlockSAD = (distpel*****)malloc (2 * sizeof(distpel****))) == NULL)
    no_mem_exit ("InitializeFastFullIntegerSearch: BlockSAD");

  for (list=0; list<2;list++)
  {
    if ((BlockSAD[list] = (distpel****)malloc ((img->max_num_references) * sizeof(distpel***))) == NULL)
      no_mem_exit ("InitializeFastFullIntegerSearch: BlockSAD");
    for (i = 0; i < img->max_num_references; i++)
    {
      if ((BlockSAD[list][i] = (distpel***)malloc (8 * sizeof(distpel**))) == NULL)
        no_mem_exit ("InitializeFastFullIntegerSearch: BlockSAD");
      for (j = 1; j < 8; j++)
      {
        if ((BlockSAD[list][i][j] = (distpel**)malloc (16 * sizeof(distpel*))) == NULL)
          no_mem_exit ("InitializeFastFullIntegerSearch: BlockSAD");
        for (k = 0; k < 16; k++)
        {
          if ((BlockSAD[list][i][j][k] = (distpel*)malloc (max_pos * sizeof(distpel))) == NULL)
            no_mem_exit ("InitializeFastFullIntegerSearch: BlockSAD");
        }
      }
    }
  }

  if ((search_setup_done = (int**)malloc (2*sizeof(int*)))==NULL)
    no_mem_exit ("InitializeFastFullIntegerSearch: search_setup_done");
  if ((search_center_x = (int**)malloc (2*sizeof(int*)))==NULL)
    no_mem_exit ("InitializeFastFullIntegerSearch: search_center_x");
  if ((search_center_y = (int**)malloc (2*sizeof(int*)))==NULL)
    no_mem_exit ("InitializeFastFullIntegerSearch: search_center_y");
  if ((pos_00 = (int**)malloc (2*sizeof(int*)))==NULL)
    no_mem_exit ("InitializeFastFullIntegerSearch: pos_00");
  if ((max_search_range = (int**)malloc (2*sizeof(int*)))==NULL)
    no_mem_exit ("InitializeFastFullIntegerSearch: max_search_range");

  for (list=0; list<2; list++)
  {
    if ((search_setup_done[list] = (int*)malloc ((img->max_num_references)*sizeof(int)))==NULL)
      no_mem_exit ("InitializeFastFullIntegerSearch: search_setup_done");
    if ((search_center_x[list] = (int*)malloc ((img->max_num_references)*sizeof(int)))==NULL)
      no_mem_exit ("InitializeFastFullIntegerSearch: search_center_x");
    if ((search_center_y[list] = (int*)malloc ((img->max_num_references)*sizeof(int)))==NULL)
      no_mem_exit ("InitializeFastFullIntegerSearch: search_center_y");
    if ((pos_00[list] = (int*)malloc ((img->max_num_references)*sizeof(int)))==NULL)
      no_mem_exit ("InitializeFastFullIntegerSearch: pos_00");
    if ((max_search_range[list] = (int*)malloc ((img->max_num_references)*sizeof(int)))==NULL)
      no_mem_exit ("InitializeFastFullIntegerSearch: max_search_range");
  }

  // assign max search ranges for reference frames
  if (input->full_search == 2)
  {
    for (list=0;list<2;list++)
      for (i=0; i<img->max_num_references; i++)
        max_search_range[list][i] = search_range;
  }
  else
  {
    for (list=0;list<2;list++)
    {
      max_search_range[list][0] = search_range;
      for (i=1; i< img->max_num_references; i++)  max_search_range[list][i] = search_range / 2;
    }
  }
}

/*!
 ***********************************************************************
 * \brief
 *    function for deleting the arrays for fast integer motion estimation
 ***********************************************************************
 */
void
ClearFastFullIntegerSearch ()
{
  int  i, j, k, list;

  for (list=0; list<2; list++)
  {
    for (i = 0; i < img->max_num_references; i++)
    {
      for (j = 1; j < 8; j++)
      {
        for (k = 0; k < 16; k++)
        {
          free (BlockSAD[list][i][j][k]);
        }
        free (BlockSAD[list][i][j]);
      }
      free (BlockSAD[list][i]);
    }
    free (BlockSAD[list]);
  }
  free (BlockSAD);

  for (list=0; list<2; list++)
  {
    free (search_setup_done[list]);
    free (search_center_x[list]);
    free (search_center_y[list]);
    free (pos_00[list]);
    free (max_search_range[list]);
  }
  free (search_setup_done);
  free (search_center_x);
  free (search_center_y);
  free (pos_00);
  free (max_search_range);

}


/*!
 ***********************************************************************
 * \brief
 *    function resetting flags for fast integer motion estimation
 *    (have to be called in start_macroblock())
 ***********************************************************************
 */
void
ResetFastFullIntegerSearch ()
{
  int list;
  for (list=0; list<2; list++)
    memset(&search_setup_done [list][0], 0, img->max_num_references * sizeof(int));
}
/*!
 ***********************************************************************
 * \brief
 *    calculation of SAD for larger blocks on the basis of 4x4 blocks
 ***********************************************************************
 */
void
SetupLargerBlocks (int list, int refindex, int max_pos)
{
#define ADD_UP_BLOCKS()   _o=*_bo; _i=*_bi; _j=*_bj; for(pos=0;pos<max_pos;pos++) _o[pos] = _i[pos] + _j[pos];
#define INCREMENT(inc)    _bo+=inc; _bi+=inc; _bj+=inc;

  distpel   pos, **_bo, **_bi, **_bj;
  register distpel *_o,   *_i,   *_j;

  //--- blocktype 6 ---
  _bo = BlockSAD[list][refindex][6];
  _bi = BlockSAD[list][refindex][7];
  _bj = _bi + 4;
  ADD_UP_BLOCKS(); INCREMENT(1);
  ADD_UP_BLOCKS(); INCREMENT(1);
  ADD_UP_BLOCKS(); INCREMENT(1);
  ADD_UP_BLOCKS(); INCREMENT(5);
  ADD_UP_BLOCKS(); INCREMENT(1);
  ADD_UP_BLOCKS(); INCREMENT(1);
  ADD_UP_BLOCKS(); INCREMENT(1);
  ADD_UP_BLOCKS();

  //--- blocktype 5 ---
  _bo = BlockSAD[list][refindex][5];
  _bi = BlockSAD[list][refindex][7];
  _bj = _bi + 1;
  ADD_UP_BLOCKS(); INCREMENT(2);
  ADD_UP_BLOCKS(); INCREMENT(2);
  ADD_UP_BLOCKS(); INCREMENT(2);
  ADD_UP_BLOCKS(); INCREMENT(2);
  ADD_UP_BLOCKS(); INCREMENT(2);
  ADD_UP_BLOCKS(); INCREMENT(2);
  ADD_UP_BLOCKS(); INCREMENT(2);
  ADD_UP_BLOCKS();

  //--- blocktype 4 ---
  _bo = BlockSAD[list][refindex][4];
  _bi = BlockSAD[list][refindex][6];
  _bj = _bi + 1;
  ADD_UP_BLOCKS(); INCREMENT(2);
  ADD_UP_BLOCKS(); INCREMENT(6);
  ADD_UP_BLOCKS(); INCREMENT(2);
  ADD_UP_BLOCKS();

  //--- blocktype 3 ---
  _bo = BlockSAD[list][refindex][3];
  _bi = BlockSAD[list][refindex][4];
  _bj = _bi + 8;
  ADD_UP_BLOCKS(); INCREMENT(2);
  ADD_UP_BLOCKS();

  //--- blocktype 2 ---
  _bo = BlockSAD[list][refindex][2];
  _bi = BlockSAD[list][refindex][4];
  _bj = _bi + 2;
  ADD_UP_BLOCKS(); INCREMENT(8);
  ADD_UP_BLOCKS();

  //--- blocktype 1 ---
  _bo = BlockSAD[list][refindex][1];
  _bi = BlockSAD[list][refindex][3];
  _bj = _bi + 2;
  ADD_UP_BLOCKS();
}


/*!
 ***********************************************************************
 * \brief
 *    Setup the fast search for an macroblock
 ***********************************************************************
 */
#define GEN_ME 0
#if GEN_ME
void SetupFastFullPelSearch (short ref, int list)  // <--  reference frame parameter, list0 or 1
{
  short   pmv[2];
  static imgpel   orig_pels[768];

  imgpel  *srcptr = orig_pels;
  int     offset_x, offset_y, range_partly_outside, ref_x, ref_y, pos, abs_x, abs_y, bindex, blky;
  int     max_width, max_height;
  int     img_width, img_height;

  StorablePicture *ref_picture;
  distpel**   block_sad = BlockSAD[list][ref][7];
  int     search_range  = max_search_range[list][ref];
  int     max_pos       = (2*search_range+1) * (2*search_range+1);

  int     list_offset   = img->mb_data[img->current_mb_nr].list_offset;

  int     apply_weights = ( (active_pps->weighted_pred_flag && (img->type == P_SLICE || img->type == SP_SLICE)) ||
    (active_pps->weighted_bipred_idc && (img->type == B_SLICE))) && input->UseWeightedReferenceME;
  int abs_y4, abs_x4;

  int   i, j, k;
  int   level_dist = F_PEL + apply_weights * 3;
  int   pixel_x, pixel_y;


  for ( j= img->opix_y; j < img->opix_y + MB_BLOCK_SIZE; j +=BLOCK_SIZE)
  {
    for (pixel_y = j; pixel_y < j + BLOCK_SIZE; pixel_y++)
    {
      memcpy( srcptr, &imgY_org[pixel_y][img->opix_x], BLOCK_SIZE * sizeof(imgpel));
      memcpy( srcptr + 16, &imgY_org[pixel_y][img->opix_x + BLOCK_SIZE], BLOCK_SIZE * sizeof(imgpel));
      memcpy( srcptr + 32, &imgY_org[pixel_y][img->opix_x + 2 * BLOCK_SIZE], BLOCK_SIZE * sizeof(imgpel));
      memcpy( srcptr + 48, &imgY_org[pixel_y][img->opix_x + 3 * BLOCK_SIZE], BLOCK_SIZE * sizeof(imgpel));
      srcptr += BLOCK_SIZE;
    }
    srcptr += 48;
  }
  // storage format is different from that of orig_pic
  // for YUV 4:2:0 we have:
  // YYYY
  // YYYY
  // U U
  //
  // V V
  //
  if (ChromaMEEnable)
  {
    imgpel *auxptr;
    int   bsx_c = BLOCK_SIZE >> (chroma_shift_x - 2);
    int   bsy_c = BLOCK_SIZE >> (chroma_shift_y - 2);
    int   pic_pix_x_c = img->opix_x >> (chroma_shift_x - 2);
    int   pic_pix_y_c = img->opix_y >> (chroma_shift_y - 2);

    // copy the original cmp1 and cmp2 data to the orig_pic matrix
    // This seems to be wrong.
    for (k=0; k<2; k++)
    {
      srcptr = auxptr = orig_pels + (256 << k);
      for ( pixel_y = 0, i = 0; i < (BLOCK_SIZE >> (chroma_shift_y - 2)); i++, pixel_y += bsy_c )
      {
        for ( pixel_x = 0, k = 0; k < (BLOCK_SIZE >> (chroma_shift_x - 2)); k++, pixel_x += bsx_c )
        {
          srcptr = auxptr;
          for (j = 0; j < bsy_c; j++)
          {
            memcpy( srcptr, &imgUV_org[k][pic_pix_y_c + pixel_y + j][pic_pix_x_c + pixel_x], bsx_c * sizeof(imgpel));
            srcptr += bsx_c;
          }
          auxptr += MB_BLOCK_SIZE;
        }
      }
    }
  }


  ref_picture     = listX[list+list_offset][ref];
  ref_access_method = FAST_ACCESS;

  //===== Use weighted Reference for ME ====
  ref_pic_sub.luma = ref_picture->imgY_sub;

  if (apply_weights)
  {
    weight_luma = wp_weight[list + list_offset][ref][0];
    offset_luma = wp_offset[list + list_offset][ref][0];
  }

  if ( ChromaMEEnable)
  {
    ref_pic_sub.crcb[0] = ref_picture->imgUV_sub[0];
    ref_pic_sub.crcb[1] = ref_picture->imgUV_sub[1];
    width_pad_cr  = ref_picture->size_x_cr_pad;
    height_pad_cr = ref_picture->size_y_cr_pad;
    if (apply_weights)
    {
      weight_cr[0] = wp_weight[list + list_offset][ref][1];
      weight_cr[1] = wp_weight[list + list_offset][ref][2];
      offset_cr[0] = wp_offset[list + list_offset][ref][1];
      offset_cr[1] = wp_offset[list + list_offset][ref][2];
    }
  }

  max_width     = ref_picture->size_x - 17;
  max_height    = ref_picture->size_y - 17;

  img_width     = ref_picture->size_x;
  img_height    = ref_picture->size_y;
  width_pad     = ref_picture->size_x_pad;
  height_pad    = ref_picture->size_y_pad;

  //===== get search center: predictor of 16x16 block =====
  SetMotionVectorPredictor (pmv, enc_picture->ref_idx[list], enc_picture->mv[list], ref, list, 0, 0, 16, 16);

  search_center_x[list][ref] = pmv[0] / 4;
  search_center_y[list][ref] = pmv[1] / 4;

  if (!input->rdopt)
  {
    //--- correct center so that (0,0) vector is inside ---
    search_center_x[list][ref] = iClip3(-search_range, search_range, search_center_x[list][ref]);
    search_center_y[list][ref] = iClip3(-search_range, search_range, search_center_y[list][ref]);
  }
  search_center_x[list][ref] = iClip3(-2047 + search_range, 2047 - search_range, search_center_x[list][ref]);
  search_center_y[list][ref] = iClip3(LEVELMVLIMIT[img->LevelIndex][0] + search_range, LEVELMVLIMIT[img->LevelIndex][1]  - search_range, search_center_y[list][ref]);

  search_center_x[list][ref] += img->opix_x;
  search_center_y[list][ref] += img->opix_y;

  offset_x = search_center_x[list][ref];
  offset_y = search_center_y[list][ref];


  //===== check if whole search range is inside image =====
  if (offset_x >= search_range && offset_x <= max_width  - search_range &&
    offset_y >= search_range && offset_y <= max_height - search_range   )
  {
    range_partly_outside = 0;
  }
  else
  {
    range_partly_outside = 1;
  }

  //===== determine position of (0,0)-vector =====
  if (!input->rdopt)
  {
    ref_x = img->opix_x - offset_x;
    ref_y = img->opix_y - offset_y;

    for (pos = 0; pos < max_pos; pos++)
    {
      if (ref_x == spiral_search_x[pos] &&
        ref_y == spiral_search_y[pos])
      {
        pos_00[list][ref] = pos;
        break;
      }
    }
  }

  //===== loop over search range (spiral search): get blockwise SAD =====
  for (pos = 0; pos < max_pos; pos++)
  {
    abs_y = offset_y + spiral_search_y[pos];
    abs_x = offset_x + spiral_search_x[pos];

    abs_y4 = (abs_y + IMG_PAD_SIZE) << 2;
    abs_x4 = (abs_x + IMG_PAD_SIZE) << 2;

    if (range_partly_outside)
    {
      if (abs_y >= 0 && abs_y <= max_height &&
        abs_x >= 0 && abs_x <= max_width    )
      {
        ref_access_method = FAST_ACCESS;
      }
      else
      {
        ref_access_method = UMV_ACCESS;
      }
    }

    srcptr = orig_pels;
    bindex = 0;
    for (blky = 0; blky < 4; blky++)
    {
      block_sad[bindex++][pos] = computeUniPred[level_dist](srcptr, 4, 4, INT_MAX, abs_x4,      abs_y4);
      srcptr += 16;
      block_sad[bindex++][pos] = computeUniPred[level_dist](srcptr, 4, 4, INT_MAX, abs_x4 + 16, abs_y4);
      srcptr += 16;
      block_sad[bindex++][pos] = computeUniPred[level_dist](srcptr, 4, 4, INT_MAX, abs_x4 + 32, abs_y4);
      srcptr += 16;
      block_sad[bindex++][pos] = computeUniPred[level_dist](srcptr, 4, 4, INT_MAX, abs_x4 + 48, abs_y4);
      srcptr += 16;
      abs_y4 += 16;
    }
  }

  //===== combine SAD's for larger block types =====
  SetupLargerBlocks (list, ref, max_pos);

  //===== set flag marking that search setup have been done =====
  search_setup_done[list][ref] = 1;
}

#else
void SetupFastFullPelSearch (short ref, int list)  // <--  reference frame parameter, list0 or 1
{
  short   pmv[2];
  static imgpel orig_pels[768];
  imgpel  *srcptr = orig_pels, *refptr;
  int     k, x, y;
  int     abs_y4, abs_x4;
  int     offset_x, offset_y, range_partly_outside, ref_x, ref_y, pos, abs_x, abs_y, bindex, blky;
  int     LineSadBlk0, LineSadBlk1, LineSadBlk2, LineSadBlk3;
  int     max_width, max_height;
  int     img_width, img_height;

  StorablePicture *ref_picture;
  distpel**   block_sad = BlockSAD[list][ref][7];
  int     search_range  = max_search_range[list][ref];
  int     max_pos       = (2*search_range+1) * (2*search_range+1);

  int     list_offset   = img->mb_data[img->current_mb_nr].list_offset;
  int     apply_weights = ( (active_pps->weighted_pred_flag && (img->type == P_SLICE || img->type == SP_SLICE)) ||
    (active_pps->weighted_bipred_idc && (img->type == B_SLICE))) && input->UseWeightedReferenceME;
  int     weighted_pel;
  int *dist_method = input->MEErrorMetric[0] ? img->quad : byte_abs;

  ref_picture     = listX[list+list_offset][ref];
  ref_access_method = FAST_ACCESS;
  ref_pic_sub.luma = ref_picture->imgY_sub;

  max_width     = ref_picture->size_x - 17;
  max_height    = ref_picture->size_y - 17;

  img_width     = ref_picture->size_x;
  img_height    = ref_picture->size_y;
  width_pad     = ref_picture->size_x_pad;
  height_pad    = ref_picture->size_y_pad;

  if (apply_weights)
  {
    weight_luma = wp_weight[list + list_offset][ref][0];
    offset_luma = wp_offset[list + list_offset][ref][0];
  }

  if ( ChromaMEEnable)
  {
    ref_pic_sub.crcb[0] = ref_picture->imgUV_sub[0];
    ref_pic_sub.crcb[1] = ref_picture->imgUV_sub[1];
    width_pad_cr  = ref_picture->size_x_cr_pad;
    height_pad_cr = ref_picture->size_y_cr_pad;

    if (apply_weights)
    {
      weight_cr[0] = wp_weight[list + list_offset][ref][1];
      weight_cr[1] = wp_weight[list + list_offset][ref][2];
      offset_cr[0] = wp_offset[list + list_offset][ref][1];
      offset_cr[1] = wp_offset[list + list_offset][ref][2];
    }
  }

  //===== get search center: predictor of 16x16 block =====
  SetMotionVectorPredictor (pmv, enc_picture->ref_idx[list], enc_picture->mv[list], ref, list, 0, 0, 16, 16);

  search_center_x[list][ref] = pmv[0] / 4;
  search_center_y[list][ref] = pmv[1] / 4;

  if (!input->rdopt)
  {
    //--- correct center so that (0,0) vector is inside ---
    search_center_x[list][ref] = iClip3(-search_range, search_range, search_center_x[list][ref]);
    search_center_y[list][ref] = iClip3(-search_range, search_range, search_center_y[list][ref]);
  }

  search_center_x[list][ref] = iClip3(-2047 + search_range, 2047 - search_range, search_center_x[list][ref]);
  search_center_y[list][ref] = iClip3(LEVELMVLIMIT[img->LevelIndex][0] + search_range, LEVELMVLIMIT[img->LevelIndex][1]  - search_range, search_center_y[list][ref]);

  search_center_x[list][ref] += img->opix_x;
  search_center_y[list][ref] += img->opix_y;

  offset_x = search_center_x[list][ref];
  offset_y = search_center_y[list][ref];


  //===== copy original block for fast access =====
  for   (y = img->opix_y; y < img->opix_y+MB_BLOCK_SIZE; y++)
  {
    memcpy(srcptr, &imgY_org[y][img->opix_x], MB_BLOCK_SIZE * sizeof(imgpel));
    srcptr += MB_BLOCK_SIZE;
  }
  if ( ChromaMEEnable)
  {
    for (k = 0; k < 2; k++)
    {
      for   (y = img->opix_c_y; y < img->opix_c_y + img->mb_cr_size_y; y++)
      {
        memcpy(srcptr, &imgUV_org[k][y][img->opix_c_x], img->mb_cr_size_x * sizeof(imgpel));
        srcptr += img->mb_cr_size_x;
      }
    }
  }

  //===== check if whole search range is inside image =====
  if (offset_x >= search_range && offset_x <= max_width  - search_range &&
    offset_y >= search_range && offset_y <= max_height - search_range   )
  {
    range_partly_outside = 0;
  }
  else
  {
    range_partly_outside = 1;
  }

  //===== determine position of (0,0)-vector =====
  if (!input->rdopt)
  {
    ref_x = img->opix_x - offset_x;
    ref_y = img->opix_y - offset_y;

    for (pos = 0; pos < max_pos; pos++)
    {
      if (ref_x == spiral_search_x[pos] &&
        ref_y == spiral_search_y[pos])
      {
        pos_00[list][ref] = pos;
        break;
        }
      }
    }

    //===== loop over search range (spiral search): get blockwise SAD =====
    for (pos = 0; pos < max_pos; pos++)
    {
      abs_y = offset_y + spiral_search_y[pos];
      abs_x = offset_x + spiral_search_x[pos];

      abs_y4 = (abs_y + IMG_PAD_SIZE) << 2;
      abs_x4 = (abs_x + IMG_PAD_SIZE) << 2;

      if (range_partly_outside)
      {
      if (abs_y >= 0 && abs_y <= max_height&&
        abs_x >= 0 && abs_x <= max_width  )
        {
        ref_access_method = FAST_ACCESS;
        }
        else
        {
        ref_access_method = UMV_ACCESS;
      }
    }

    if (apply_weights)
    {
      srcptr = orig_pels;
      bindex = 0;

      refptr = get_line[ref_access_method] (ref_pic_sub.luma, abs_y4, abs_x4);

      for (blky = 0; blky < 4; blky++)
      {
        LineSadBlk0 = LineSadBlk1 = LineSadBlk2 = LineSadBlk3 = 0;

        for (y = 0; y < 4; y++)
        {
          weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *refptr++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
          LineSadBlk0 += dist_method [weighted_pel - *srcptr++];
          weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *refptr++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
          LineSadBlk0 += dist_method [weighted_pel - *srcptr++];
          weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *refptr++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
          LineSadBlk0 += dist_method [weighted_pel - *srcptr++];
          weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *refptr++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
          LineSadBlk0 += dist_method [weighted_pel - *srcptr++];
          weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *refptr++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
          LineSadBlk1 += dist_method [weighted_pel - *srcptr++];
          weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *refptr++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
          LineSadBlk1 += dist_method [weighted_pel - *srcptr++];
          weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *refptr++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
          LineSadBlk1 += dist_method [weighted_pel - *srcptr++];
          weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *refptr++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
          LineSadBlk1 += dist_method [weighted_pel - *srcptr++];
          weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *refptr++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
          LineSadBlk2 += dist_method [weighted_pel - *srcptr++];
          weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *refptr++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
          LineSadBlk2 += dist_method [weighted_pel - *srcptr++];
          weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *refptr++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
          LineSadBlk2 += dist_method [weighted_pel - *srcptr++];
          weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *refptr++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
          LineSadBlk2 += dist_method [weighted_pel - *srcptr++];
          weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *refptr++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
          LineSadBlk3 += dist_method [weighted_pel - *srcptr++];
          weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *refptr++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
          LineSadBlk3 += dist_method [weighted_pel - *srcptr++];
          weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *refptr++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
          LineSadBlk3 += dist_method [weighted_pel - *srcptr++];
          weighted_pel = iClip1( img->max_imgpel_value, ((weight_luma * *refptr++  + wp_luma_round) >> luma_log_weight_denom) + offset_luma);
          LineSadBlk3 += dist_method [weighted_pel - *srcptr++];
          refptr += img_padded_size_x - MB_BLOCK_SIZE;
        }

        block_sad[bindex++][pos] = LineSadBlk0;
        block_sad[bindex++][pos] = LineSadBlk1;
        block_sad[bindex++][pos] = LineSadBlk2;
        block_sad[bindex++][pos] = LineSadBlk3;
      }
      if (ChromaMEEnable)
      {
        for (k = 0; k < 2; k ++)
        {
          bindex = 0;

          refptr = get_crline[ref_access_method] (ref_pic_sub.crcb[k], abs_y4, abs_x4);
          for (blky = 0; blky < 4; blky++)
          {
            LineSadBlk0 = LineSadBlk1 = LineSadBlk2 = LineSadBlk3 = 0;

            for (y = 0; y < img->mb_cr_size_y; y+=BLOCK_SIZE)
            {
              for (x = 0; x < img->mb_cr_size_x; x += BLOCK_SIZE)
              {
                weighted_pel = iClip1( img->max_imgpel_value_uv, ((weight_cr[k] * *refptr++  + wp_chroma_round) >> chroma_log_weight_denom) + offset_cr[k]);
                LineSadBlk0 += dist_method [weighted_pel - *srcptr++];
              }
              for (x = 0; x < img->mb_cr_size_x; x += BLOCK_SIZE)
              {
                weighted_pel = iClip1( img->max_imgpel_value_uv, ((weight_cr[k] * *refptr++  + wp_chroma_round) >> chroma_log_weight_denom) + offset_cr[k]);
                LineSadBlk1 += dist_method [weighted_pel - *srcptr++];
              }
              for (x = 0; x < img->mb_cr_size_x; x += BLOCK_SIZE)
              {
                weighted_pel = iClip1( img->max_imgpel_value_uv, ((weight_cr[k] * *refptr++  + wp_chroma_round) >> chroma_log_weight_denom) + offset_cr[k]);
                LineSadBlk2 += dist_method [weighted_pel - *srcptr++];
              }
              for (x = 0; x < img->mb_cr_size_x; x += BLOCK_SIZE)
              {
                weighted_pel = iClip1( img->max_imgpel_value_uv, ((weight_cr[k] * *refptr++  + wp_chroma_round) >> chroma_log_weight_denom) + offset_cr[k]);
                LineSadBlk3 += dist_method [weighted_pel - *srcptr++];
              }
              refptr += img_cr_padded_size_x - img->mb_cr_size_x;
            }

            block_sad[bindex++][pos] += LineSadBlk0;
            block_sad[bindex++][pos] += LineSadBlk1;
            block_sad[bindex++][pos] += LineSadBlk2;
            block_sad[bindex++][pos] += LineSadBlk3;
          }
        }
      }
    }
    else
    {
      srcptr = orig_pels;
      bindex = 0;

      refptr = get_line[ref_access_method] (ref_pic_sub.luma, abs_y4, abs_x4);

      for (blky = 0; blky < 4; blky++)
      {
        LineSadBlk0 = LineSadBlk1 = LineSadBlk2 = LineSadBlk3 = 0;

        for (y = 0; y < 4; y++)
        {
          LineSadBlk0 += dist_method [*refptr++ - *srcptr++];
          LineSadBlk0 += dist_method [*refptr++ - *srcptr++];
          LineSadBlk0 += dist_method [*refptr++ - *srcptr++];
          LineSadBlk0 += dist_method [*refptr++ - *srcptr++];
          LineSadBlk1 += dist_method [*refptr++ - *srcptr++];
          LineSadBlk1 += dist_method [*refptr++ - *srcptr++];
          LineSadBlk1 += dist_method [*refptr++ - *srcptr++];
          LineSadBlk1 += dist_method [*refptr++ - *srcptr++];
          LineSadBlk2 += dist_method [*refptr++ - *srcptr++];
          LineSadBlk2 += dist_method [*refptr++ - *srcptr++];
          LineSadBlk2 += dist_method [*refptr++ - *srcptr++];
          LineSadBlk2 += dist_method [*refptr++ - *srcptr++];
          LineSadBlk3 += dist_method [*refptr++ - *srcptr++];
          LineSadBlk3 += dist_method [*refptr++ - *srcptr++];
          LineSadBlk3 += dist_method [*refptr++ - *srcptr++];
          LineSadBlk3 += dist_method [*refptr++ - *srcptr++];
          refptr += img_padded_size_x - MB_BLOCK_SIZE;
        }

        block_sad[bindex++][pos] = LineSadBlk0;
        block_sad[bindex++][pos] = LineSadBlk1;
        block_sad[bindex++][pos] = LineSadBlk2;
        block_sad[bindex++][pos] = LineSadBlk3;
      }

      if (ChromaMEEnable)
      {
        for (k = 0; k < 2; k ++)
        {
          bindex = 0;

          refptr = get_crline[ref_access_method] (ref_pic_sub.crcb[k], abs_y4, abs_x4);
          for (blky = 0; blky < 4; blky++)
          {
            LineSadBlk0 = LineSadBlk1 = LineSadBlk2 = LineSadBlk3 = 0;

            for (y = 0; y < img->mb_cr_size_y; y+=BLOCK_SIZE)
            {
              for (x = 0; x < img->mb_cr_size_x; x += BLOCK_SIZE)
              {
                LineSadBlk0 += dist_method [*refptr++ - *srcptr++];
              }
              for (x = 0; x < img->mb_cr_size_x; x += BLOCK_SIZE)
              {
                LineSadBlk1 += dist_method [*refptr++ - *srcptr++];
              }
              for (x = 0; x < img->mb_cr_size_x; x += BLOCK_SIZE)
              {
                LineSadBlk2 += dist_method [*refptr++ - *srcptr++];
              }
              for (x = 0; x < img->mb_cr_size_x; x += BLOCK_SIZE)
              {
                LineSadBlk3 += dist_method [*refptr++ - *srcptr++];
              }
              refptr += img_cr_padded_size_x - img->mb_cr_size_x;
            }

            block_sad[bindex++][pos] += LineSadBlk0;
            block_sad[bindex++][pos] += LineSadBlk1;
            block_sad[bindex++][pos] += LineSadBlk2;
            block_sad[bindex++][pos] += LineSadBlk3;
          }
        }
      }
    }
  }

  //===== combine SAD's for larger block types =====
  SetupLargerBlocks (list, ref, max_pos);

  //===== set flag marking that search setup have been done =====
  search_setup_done[list][ref] = 1;
}
#endif

/*!
 ***********************************************************************
 * \brief
 *    Fast Full pixel block motion search
 ***********************************************************************
 */
int                                                   //  ==> minimum motion cost after search
FastFullPelBlockMotionSearch (imgpel*   orig_pic,     // <--  not used
                              short     ref,          // <--  reference frame (0... or -1 (backward))
                              int       list,
                              int       pic_pix_x,    // <--  absolute x-coordinate of regarded AxB block
                              int       pic_pix_y,    // <--  absolute y-coordinate of regarded AxB block
                              int       blocktype,    // <--  block type (1-16x16 ... 7-4x4)
                              short     pred_mv_x,    // <--  motion vector predictor (x) in sub-pel units
                              short     pred_mv_y,    // <--  motion vector predictor (y) in sub-pel units
                              short*    mv_x,         //  --> motion vector (x) - in pel units
                              short*    mv_y,         //  --> motion vector (y) - in pel units
                              int       search_range, // <--  1-d search range in pel units
                              int       min_mcost,    // <--  minimum motion cost (cost for center or huge value)
                              int       lambda_factor)       // <--  lagrangian parameter for determining motion cost
{
  int   pos, offset_x, offset_y, cand_x, cand_y, mcost;

  int   max_pos       = (2*search_range+1)*(2*search_range+1);              // number of search positions
  int   best_pos      = 0;                                                  // position with minimum motion cost
  int   block_index;                                                        // block index for indexing SAD array
  distpel*  block_sad;                                                          // pointer to SAD array

  block_index   = (pic_pix_y-img->opix_y)+((pic_pix_x-img->opix_x)>>2); // block index for indexing SAD array
  block_sad     = BlockSAD[list][ref][blocktype][block_index];         // pointer to SAD array

  //===== set up fast full integer search if needed / set search center =====
  if (!search_setup_done[list][ref])
  {
    SetupFastFullPelSearch (ref, list);
  }

  offset_x = search_center_x[list][ref] - img->opix_x;
  offset_y = search_center_y[list][ref] - img->opix_y;

  //===== cost for (0,0)-vector: it is done before, because MVCost can be negative =====
  if (!input->rdopt)
  {
    mcost = block_sad[pos_00[list][ref]] + MV_COST_SMP (lambda_factor, 0, 0, pred_mv_x, pred_mv_y);

    if (mcost < min_mcost)
    {
      min_mcost = mcost;
      best_pos  = pos_00[list][ref];
    }
  }

  //===== loop over all search positions =====
  for (pos=0; pos<max_pos; pos++, block_sad++)
  {
    //--- check residual cost ---
    if (*block_sad < min_mcost)
    {
      //--- get motion vector cost ---
      cand_x = (offset_x + spiral_search_x[pos])<<2;
      cand_y = (offset_y + spiral_search_y[pos])<<2;
      mcost  = *block_sad;
      mcost += MV_COST_SMP (lambda_factor, cand_x, cand_y, pred_mv_x, pred_mv_y);

      //--- check motion cost ---
      if (mcost < min_mcost)
      {
        min_mcost = mcost;
        best_pos  = pos;
      }
    }
  }

  //===== set best motion vector and return minimum motion cost =====
  *mv_x = offset_x + spiral_search_x[best_pos];
  *mv_y = offset_y + spiral_search_y[best_pos];
  return min_mcost;
}

