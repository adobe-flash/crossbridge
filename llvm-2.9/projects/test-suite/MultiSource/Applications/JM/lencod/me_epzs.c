
/*!
*************************************************************************************
* \file me_epzs.c
*
* \brief
*    Motion Estimation using EPZS
*
* \author
*    Main contributors (see contributors.h for copyright, address and affiliation details)
*      - Alexis Michael Tourapis <alexismt@ieee.org>
*      - Athanasios Leontaris    <aleon@dolby.com>
*
*************************************************************************************
*/

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
#include "me_epzs.h"

#define EPZSREF 1

extern int *mvbits;
extern int *byte_abs;

// Define Global Parameters
static const short blk_parent[8] = {1, 1, 1, 1, 2, 4, 4, 5}; //!< {skip, 16x16, 16x8, 8x16, 8x8, 8x4, 4x8, 4x4}
//static const short blk_child[8]  = {1, 2, 4, 4, 5, 7, 7, 7}; //!< {skip, 16x16, 16x8, 8x16, 8x8, 8x4, 4x8, 4x4}
static const int   minthres_base[8] = {0,  64,  32,  32,  16,  8,  8,  4};
static const int   medthres_base[8] = {0, 256, 128, 128,  64, 32, 32, 16};
static const int   maxthres_base[8] = {0, 768, 384, 384, 192, 96, 96, 48};
static const short search_point_hp[10][2] = {{0,0},{-2,0}, {0,2}, {2,0},  {0,-2}, {-2,2},  {2,2},  {2,-2}, {-2,-2}, {-2,2}};
static const short search_point_qp[10][2] = {{0,0},{-1,0}, {0,1}, {1,0},  {0,-1}, {-1,1},  {1,1},  {1,-1}, {-1,-1}, {-1,1}};
//static const int   next_subpel_pos_start[5][5] = {};
//static const int   next_subpel_pos_end  [5][5] = {};



static short EPZSBlkCount;
static int   searcharray;
static int   mv_rescale;

//! Define EPZS Refinement patterns
static int pattern_data[5][12][4] =
{
  { // Small Diamond pattern
    {  0,  4,  3, 3 }, {  4,  0,  0, 3 }, {  0, -4,  1, 3 }, { -4,  0, 2, 3 }
  },
  { // Square pattern
    {  0,  4,  7, 3 }, {  4,  4,  7, 5 }, {  4,  0,  1, 3 }, {  4, -4, 1, 5 },
    {  0, -4,  3, 3 }, { -4, -4,  3, 5 }, { -4,  0,  5, 3 }, { -4,  4, 5, 5 }
  },
  { // Enhanced Diamond pattern
    { -4,  4, 10, 5 }, {  0,  8, 10, 8 }, {  0,  4, 10, 7 }, {  4,  4, 1, 5 },
    {  8,  0, 1,  8 }, {  4,  0,  1, 7 }, {  4, -4,  4, 5 }, {  0, -8, 4, 8 },
    {  0, -4, 4,  7 }, { -4, -4, 7,  5 }, { -8,  0,  7, 8 }, { -4,  0, 7, 7 }

  },
  { // Large Diamond pattern
    {  0,  8, 6,  5 }, {  4,  4, 0,  3 }, {  8,  0, 0,  5 }, {  4, -4, 2, 3 },
    {  0, -8, 2,  5 }, { -4, -4, 4,  3 }, { -8,  0, 4,  5 }, { -4,  4, 6, 3 }
  },
  { // Extended Subpixel pattern
    {  0,  8, 6, 12 }, {  4,  4, 0, 12 }, {  8,  0, 0, 12 }, {  4, -4, 2, 12 },
    {  0, -8, 2, 12 }, { -4, -4, 4, 12 }, { -8,  0, 4, 12 }, { -4,  4, 6, 12 },
    {  0,  2, 6, 12 }, {  2,  0, 0, 12 }, {  0, -2, 2, 12 }, { -2,  0, 4, 12 }
  }
};

// Other definitions
const  char c_EPZSPattern[6][20]    = { "Diamond", "Square", "Extended Diamond", "Large Diamond", "SBP Large Diamond", "PMVFAST"};
const  char c_EPZSDualPattern[7][20] = { "Disabled","Diamond", "Square", "Extended Diamond", "Large Diamond", "SBP Large Diamond", "PMVFAST"};
const  char c_EPZSFixed[3][20] = { "Disabled","All P", "All P + B"};
const  char c_EPZSOther[2][20] = { "Disabled","Enabled"};

static int medthres[8];
static int maxthres[8];
static int minthres[8];
static int subthres[8];
static int mv_scale[6][MAX_REFERENCE_PICTURES][MAX_REFERENCE_PICTURES];

static short **EPZSMap;  //!< Memory Map definition
int ***EPZSDistortion;  //!< Array for storing SAD Values
#if EPZSREF
short ******EPZSMotion;  //!< Array for storing Motion Vectors
#else
short *****EPZSMotion;  //!< Array for storing Motion Vectors
#endif

//
EPZSStructure *searchPattern,*searchPatternD, *predictor;
EPZSStructure *window_predictor, *window_predictor_extended;
EPZSStructure *sdiamond,*square,*ediamond,*ldiamond, *sbdiamond, *pmvfast;
EPZSColocParams *EPZSCo_located;

/*!
************************************************************************
* \brief
*    Allocate co-located memory
*
* \param size_x
*    horizontal luma size
* \param size_y
*    vertical luma size
* \param mb_adaptive_frame_field_flag
*    flag that indicates macroblock adaptive frame/field coding
*
* \return
*    the allocated EPZSColocParams structure
************************************************************************
*/
EPZSColocParams* allocEPZScolocated(int size_x, int size_y, int mb_adaptive_frame_field_flag)
{
  EPZSColocParams *s;

  s = calloc(1, sizeof(EPZSColocParams));
  if (NULL == s)
    no_mem_exit("alloc_EPZScolocated: s");

  s->size_x = size_x;
  s->size_y = size_y;
  get_mem4Dshort (&(s->mv), 2, size_y / BLOCK_SIZE, size_x / BLOCK_SIZE, 2);

  if (mb_adaptive_frame_field_flag)
  {
    get_mem4Dshort (&(s->top_mv),   2, size_y / BLOCK_SIZE/2, size_x / BLOCK_SIZE, 2);
    get_mem4Dshort (&(s->bottom_mv),2, size_y / BLOCK_SIZE/2, size_x / BLOCK_SIZE, 2);
  }

  s->mb_adaptive_frame_field_flag  = mb_adaptive_frame_field_flag;

  return s;
}

/*!
************************************************************************
* \brief
*    Free co-located memory.
*
* \param p
*    structure to be freed
*
************************************************************************
*/
void freeEPZScolocated(EPZSColocParams* p)
{
  if (p)
  {
    free_mem4Dshort (p->mv, 2, p->size_y / BLOCK_SIZE);

    if (p->mb_adaptive_frame_field_flag)
    {
      free_mem4Dshort (p->top_mv, 2, p->size_y / BLOCK_SIZE / 2);
      free_mem4Dshort (p->bottom_mv, 2, p->size_y / BLOCK_SIZE / 2);
    }

    free(p);

    p=NULL;
  }
}

/*!
************************************************************************
* \brief
*    Allocate EPZS pattern memory
*
* \param searchpoints
*    number of searchpoints to allocate
*
* \return
*    the allocated EPZSStructure structure
************************************************************************
*/
EPZSStructure* allocEPZSpattern(int searchpoints)
{
  EPZSStructure *s;

  s = calloc(1, sizeof(EPZSStructure));
  if (NULL == s)
    no_mem_exit("alloc_EPZSpattern: s");

  s->searchPoints = searchpoints;
  s->point = (SPoint*) calloc(searchpoints, sizeof(SPoint));

  return s;
}

/*!
************************************************************************
* \brief
*    Free EPZS pattern memory.
*
* \param p
*    structure to be freed
*
************************************************************************
*/
void freeEPZSpattern(EPZSStructure* p)
{
  if (p)
  {
    free ( (SPoint*) p->point);
    free(p);
    p=NULL;
  }
}

void assignEPZSpattern(EPZSStructure *pattern,int type, int stopSearch, int nextLast, EPZSStructure *nextpattern)
{
  int i;

  for (i = 0; i < pattern->searchPoints; i++)
  {
    pattern->point[i].mv[0]       = pattern_data[type][i][0] >> mv_rescale;
    pattern->point[i].mv[1]       = pattern_data[type][i][1] >> mv_rescale;
    pattern->point[i].start_nmbr  = pattern_data[type][i][2];
    pattern->point[i].next_points = pattern_data[type][i][3];
  }
  pattern->stopSearch = stopSearch;
  pattern->nextLast = nextLast;
  pattern->nextpattern = nextpattern;
}

/*!
************************************************************************
* \brief
*    calculate RoundLog2(uiVal)
************************************************************************
*/
static int RoundLog2 (int iValue)
{
  int iRet = 0;
  int iValue_square = iValue * iValue;

  while ((1 << (iRet + 1)) <= iValue_square)
    iRet++;

  iRet = (iRet + 1) >> 1;

  return iRet;
}


/*!
************************************************************************
* \brief
*    EPZS Search Window Predictor Initialization
************************************************************************
*/
void EPZSWindowPredictorInit (short search_range, EPZSStructure * predictor, short mode)
{
  int pos;
  int searchpos, fieldsearchpos;
  int prednum = 0;
  int i;
  int search_range_qpel = input->EPZSSubPelGrid ? 2 : 0;
  if (mode == 0)
  {
    for (pos = RoundLog2 (search_range) - 2; pos > -1; pos--)
    {
      searchpos = ((search_range << search_range_qpel)>> pos);

      for (i=1; i>=-1; i-=2)
      {
        predictor->point[prednum  ].mv[0] =  i * searchpos;
        predictor->point[prednum++].mv[1] =  0;
        predictor->point[prednum  ].mv[0] =  i * searchpos;
        predictor->point[prednum++].mv[1] =  i * searchpos;
        predictor->point[prednum  ].mv[0] =  0;
        predictor->point[prednum++].mv[1] =  i * searchpos;
        predictor->point[prednum  ].mv[0] = -i * searchpos;
        predictor->point[prednum++].mv[1] =  i * searchpos;
      }
    }
  }
  else // if (mode == 0)
  {
    for (pos = RoundLog2 (search_range) - 2; pos > -1; pos--)
    {
      searchpos = ((search_range << search_range_qpel) >> pos);
      fieldsearchpos = ((3 * searchpos + 1) << search_range_qpel) >> 1;

      for (i=1; i>=-1; i-=2)
      {
        predictor->point[prednum  ].mv[0] =  i * searchpos;
        predictor->point[prednum++].mv[1] =  0;
        predictor->point[prednum  ].mv[0] =  i * searchpos;
        predictor->point[prednum++].mv[1] =  i * searchpos;
        predictor->point[prednum  ].mv[0] =  0;
        predictor->point[prednum++].mv[1] =  i * searchpos;
        predictor->point[prednum  ].mv[0] = -i * searchpos;
        predictor->point[prednum++].mv[1] =  i * searchpos;
      }

      for (i=1; i>=-1; i-=2)
      {
        predictor->point[prednum  ].mv[0] =  i * fieldsearchpos;
        predictor->point[prednum++].mv[1] = -i * searchpos;
        predictor->point[prednum  ].mv[0] =  i * fieldsearchpos;
        predictor->point[prednum++].mv[1] =  0;
        predictor->point[prednum  ].mv[0] =  i * fieldsearchpos;
        predictor->point[prednum++].mv[1] =  i * searchpos;
        predictor->point[prednum  ].mv[0] =  i * searchpos;
        predictor->point[prednum++].mv[1] =  i * fieldsearchpos;
        predictor->point[prednum  ].mv[0] =  0;
        predictor->point[prednum++].mv[1] =  i * fieldsearchpos;
        predictor->point[prednum  ].mv[0] = -i * searchpos;
        predictor->point[prednum++].mv[1] =  i * fieldsearchpos;
      }
    }
  }
  predictor->searchPoints = prednum;
}

/*!
************************************************************************
* \brief
*    EPZS Global Initialization
************************************************************************
*/
int
EPZSInit (void)
{
  int pel_error_me = 1 << (img->bitdepth_luma - 8);
  int i, memory_size = 0;
  int searchlevels = RoundLog2 (input->search_range) - 1;

  searcharray = input->BiPredMotionEstimation? (2 * imax (input->search_range, input->BiPredMESearchRange) + 1) << (2 * input->EPZSSubPelGrid) : (2 * input->search_range + 1)<< (2 * input->EPZSSubPelGrid);

  mv_rescale = input->EPZSSubPelGrid ? 0 : 2;
  //! In this implementation we keep threshold limits fixed.
  //! However one could adapt these limits based on lagrangian
  //! optimization considerations (i.e. qp), while also allow
  //! adaptation of the limits themselves based on content or complexity.
  for (i=0;i<8;i++)
  {
    medthres[i] = input->EPZSMedThresScale * medthres_base[i] * pel_error_me;
    maxthres[i] = input->EPZSMaxThresScale * maxthres_base[i] * pel_error_me;
    minthres[i] = input->EPZSMinThresScale * minthres_base[i] * pel_error_me;
    subthres[i] = input->EPZSSubPelThresScale * medthres_base[i] * pel_error_me;
  }

  //! Definition of pottential EPZS patterns.
  //! It is possible to also define other patterns, or even use
  //! resizing patterns (such as the PMVFAST scheme. These patterns
  //! are only shown here as reference, while the same also holds
  //! for this implementation (i.e. new conditions could be added
  //! on adapting predictors, or thresholds etc. Note that search
  //! could also be performed on subpel positions directly while
  //! pattern needs not be restricted on integer positions only.

  //! Allocate memory and assign search patterns
  sdiamond = allocEPZSpattern(4);
  assignEPZSpattern(sdiamond, SDIAMOND, TRUE, TRUE, sdiamond);
  square = allocEPZSpattern(8);
  assignEPZSpattern(square, SQUARE, TRUE, TRUE, square);
  ediamond = allocEPZSpattern(12);
  assignEPZSpattern(ediamond, EDIAMOND, TRUE, TRUE, ediamond);
  ldiamond = allocEPZSpattern(8);
  assignEPZSpattern(ldiamond, LDIAMOND, TRUE, TRUE, ldiamond);
  sbdiamond = allocEPZSpattern(12);
  assignEPZSpattern(sbdiamond, SBDIAMOND, FALSE, TRUE, sdiamond);
  pmvfast = allocEPZSpattern(8);
  assignEPZSpattern(pmvfast, LDIAMOND, FALSE, TRUE, sdiamond);

  //! Allocate and assign window based predictors.
  //! Other window types could also be used, while method could be
  //! made a bit more adaptive (i.e. patterns could be assigned
  //! based on neighborhood
  window_predictor = allocEPZSpattern(searchlevels * 8);
  window_predictor_extended = allocEPZSpattern(searchlevels * 20);
  EPZSWindowPredictorInit ((short) input->search_range, window_predictor, 0);
  EPZSWindowPredictorInit ((short) input->search_range, window_predictor_extended, 1);
  //! Also assing search predictor memory
  // maxwindow + spatial + blocktype + temporal + memspatial
  predictor = allocEPZSpattern(searchlevels * 20 + 5 + 5 + 9 * (input->EPZSTemporal) + 3 * (input->EPZSSpatialMem));

  //! Finally assign memory for all other elements
  //! (distortion, EPZSMap, and temporal predictors)

  memory_size += get_mem3Dint (&EPZSDistortion, 6, 7, img->width/BLOCK_SIZE);
  memory_size += get_mem2Dshort (&EPZSMap, searcharray, searcharray );
  if (input->EPZSSpatialMem)
  {
#if EPZSREF
    memory_size += get_mem6Dshort (&EPZSMotion, 6, img->max_num_references, 7, 4, img->width/BLOCK_SIZE, 2);
#else
    memory_size += get_mem5Dshort (&EPZSMotion, 6, 7, 4, img->width/BLOCK_SIZE, 2);
#endif
  }

  if (input->EPZSTemporal)
    EPZSCo_located = allocEPZScolocated (img->width, img->height,
    active_sps->mb_adaptive_frame_field_flag);

  switch (input->EPZSPattern)
  {
  case 5:
    searchPattern = pmvfast;
    break;
  case 4:
    searchPattern = sbdiamond;
    break;
  case 3:
    searchPattern = ldiamond;
    break;
  case 2:
    searchPattern = ediamond;
    break;
  case 1:
    searchPattern = square;
    break;
  case 0:
  default:
    searchPattern = sdiamond;
    break;
  }

  switch (input->EPZSDual)
  {
  case 6:
    searchPatternD = pmvfast;
    break;
  case 5:
    searchPatternD = sbdiamond;
    break;
  case 4:
    searchPatternD = ldiamond;
    break;
  case 3:
    searchPatternD = ediamond;
    break;
  case 2:
    searchPatternD = square;
    break;
  case 1:
  default:
    searchPatternD = sdiamond;
    break;
  }

  return memory_size;
}

/*!
************************************************************************
* \brief
*    Delete EPZS Alocated memory
************************************************************************
*/
void EPZSDelete (void)
{
  if (input->EPZSTemporal)
    freeEPZScolocated (EPZSCo_located);

  free_mem2Dshort(EPZSMap);
  free_mem3Dint  (EPZSDistortion, 6);
  freeEPZSpattern(window_predictor_extended);
  freeEPZSpattern(window_predictor);
  freeEPZSpattern(predictor);
  // Free search patterns
  freeEPZSpattern(pmvfast);
  freeEPZSpattern(sbdiamond);
  freeEPZSpattern(ldiamond);
  freeEPZSpattern(ediamond);
  freeEPZSpattern(sdiamond);
  freeEPZSpattern(square);
  if (input->EPZSSpatialMem)
  {
#if EPZSREF
    free_mem6Dshort (EPZSMotion, 6, img->max_num_references, 7, 4);
#else
    free_mem5Dshort (EPZSMotion, 6, 7, 4);
#endif
  }

}

//! For ME purposes restricting the co-located partition is not necessary.
/*!
************************************************************************
* \brief
*    EPZS Slice Level Initialization
************************************************************************
*/
void
EPZSSliceInit (EPZSColocParams * p,
               StorablePicture ** listX[6])
{
  StorablePicture *fs, *fs_top, *fs_bottom;
  StorablePicture *fs1, *fs_top1, *fs_bottom1, *fsx;
  int i, j, k, jj, jdiv, loffset;
  int prescale, iTRb, iTRp;
  int list = img->type == B_SLICE ? LIST_1 : LIST_0;
  int tempmv_scale[2];
  int epzs_scale[2][6][MAX_LIST_SIZE];
  int iref;
  int invmv_precision = 8;

  // Lets compute scaling factoes between all references in lists.
  // Needed to scale spatial predictors.
  for (j = LIST_0; j < 2 + (img->MbaffFrameFlag * 4); j ++)
  {
    for (k = 0; k < listXsize[j]; k++)
    {
      for (i = 0; i < listXsize[j]; i++)
      {
        if (j/2 == 0)
        {
          iTRb = iClip3 (-128, 127, enc_picture->poc - listX[j][i]->poc);
          iTRp = iClip3 (-128, 127, enc_picture->poc - listX[j][k]->poc);
        }
        else if (j/2 == 1)
        {
          iTRb = iClip3 (-128, 127, enc_picture->top_poc - listX[j][i]->poc);
          iTRp = iClip3 (-128, 127, enc_picture->top_poc - listX[j][k]->poc);
        }
        else
        {
          iTRb = iClip3 (-128, 127, enc_picture->bottom_poc - listX[j][i]->poc);
          iTRp = iClip3 (-128, 127, enc_picture->bottom_poc - listX[j][k]->poc);
        }

        if (iTRp != 0)
        {
          prescale = (16384 + iabs (iTRp / 2)) / iTRp;
          mv_scale[j][i][k] = iClip3 (-2048, 2047, rshift_rnd_sf((iTRb * prescale), 6));
        }
        else
          mv_scale[j][i][k] = 256;
      }
    }
  }

  if (input->EPZSTemporal)
  {
    fs_top = fs_bottom = fs = listX[list][0];
    if (listXsize[list]> 1)
      fs_top1 = fs_bottom1 = fs1 = listX[list][1];
    else
      fs_top1 = fs_bottom1 = fs1 = listX[list][0];

    for (j = 0; j < 6; j++)
    {
      for (i = 0; i < 6; i++)
      {
        epzs_scale[0][j][i] = 256;
        epzs_scale[1][j][i] = 256;
      }
    }

    for (j = 0; j < 2 + (img->MbaffFrameFlag * 4); j += 2)
    {
      for (i = 0; i < listXsize[j]; i++)
      {
        if (j == 0)
          iTRb = iClip3 (-128, 127, enc_picture->poc - listX[LIST_0 + j][i]->poc);
        else if (j == 2)
          iTRb = iClip3 (-128, 127, enc_picture->top_poc - listX[LIST_0 + j][i]->poc);
        else
          iTRb = iClip3 (-128, 127, enc_picture->bottom_poc - listX[LIST_0 + j][i]->poc);
        iTRp = iClip3 (-128, 127, listX[list + j][0]->poc - listX[LIST_0 + j][i]->poc);
        if (iTRp != 0)
        {
          prescale = (16384 + iabs (iTRp / 2)) / iTRp;
          prescale = iClip3 (-2048, 2047, rshift_rnd_sf((iTRb * prescale), 6));
          //prescale = (iTRb * prescale + 32) >> 6;
        }
        else      // This could not happen but lets use it in case that reference is removed.
          prescale = 256;
        epzs_scale[0][j][i] = rshift_rnd_sf((mv_scale[j][0][i] * prescale), 8);
        epzs_scale[0][j + 1][i] = prescale - 256;
        if (listXsize[list + j]>1)
        {
          iTRp = iClip3 (-128, 127, listX[list + j][1]->poc - listX[LIST_0 + j][i]->poc);
          if (iTRp != 0)
          {
            prescale = (16384 + iabs (iTRp / 2)) / iTRp;
            prescale = iClip3 (-2048, 2047, rshift_rnd_sf((iTRb * prescale), 6));
            //prescale = (iTRb * prescale + 32) >> 6;
          }
          else      // This could not happen but lets use it for case that reference is removed.
            prescale = 256;
          epzs_scale[1][j][i] = rshift_rnd_sf((mv_scale[j][1][i] * prescale), 8);
          epzs_scale[1][j + 1][i] = prescale - 256;
        }
        else
        {
          epzs_scale[1][j][i] = epzs_scale[0][j][i];
          epzs_scale[1][j + 1][i] = epzs_scale[0][j + 1][i];
        }
      }
    }
    if (img->MbaffFrameFlag)
    {
      fs_top = listX[list + 2][0];
      fs_bottom = listX[list + 4][0];
      if (listXsize[0]> 1)
      {
        fs_top1 = listX[list + 2][1];
        fs_bottom = listX[list + 4][1];
      }
    }
    else
    {
      if (img->structure != FRAME)
      {
        if ((img->structure != fs->structure) && (fs->coded_frame))
        {
          if (img->structure == TOP_FIELD)
          {
            fs_top = fs_bottom = fs = listX[list][0]->top_field;
            fs_top1 = fs_bottom1 = fs1 = listX[list][0]->bottom_field;
          }
          else
          {
            fs_top = fs_bottom = fs = listX[list][0]->bottom_field;
            fs_top1 = fs_bottom1 = fs1 = listX[list][0]->top_field;
          }
        }
      }
    }

    //if (!active_sps->frame_mbs_only_flag || active_sps->direct_8x8_inference_flag)
    if (!active_sps->frame_mbs_only_flag)
    {
      for (j = 0; j < fs->size_y >> 2; j++)
      {
        jj = j / 2;
        jdiv = j / 2 + 4 * (j / 8);
        for (i = 0; i < fs->size_x >> 2; i++)
        {
          if (img->MbaffFrameFlag && fs->field_frame[j][i])
          {
            //! Assign frame buffers for field MBs
            //! Check whether we should use top or bottom field mvs.
            //! Depending on the assigned poc values.
            if (iabs (enc_picture->poc - fs_bottom->poc) > iabs (enc_picture->poc - fs_top->poc))
            {
              tempmv_scale[LIST_0] = 256;
              tempmv_scale[LIST_1] = 0;

              if (fs->ref_id [LIST_0][jdiv][i] < 0 && listXsize[LIST_0] > 1)
              {
                fsx = fs_top1;
                loffset = 1;
              }
              else
              {
                fsx = fs_top;
                loffset = 0;
              }

              if (fs->ref_id [LIST_0][jdiv][i] != -1)
              {
                for (iref = 0; iref < imin(img->num_ref_idx_l0_active,listXsize[LIST_0]); iref++)
                {
                  if (enc_picture->ref_pic_num[LIST_0][iref]==fs->ref_id [LIST_0][jdiv][i])
                  {
                    tempmv_scale[LIST_0] = epzs_scale[loffset][LIST_0][iref];
                    tempmv_scale[LIST_1] = epzs_scale[loffset][LIST_1][iref];
                    break;
                  }
                }
                p->mv[LIST_0][j][i][0] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_0] * fsx->mv[LIST_0][jj][i][0]), invmv_precision));
                p->mv[LIST_0][j][i][1] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_0] * fsx->mv[LIST_0][jj][i][1]), invmv_precision));
                p->mv[LIST_1][j][i][0] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_1] * fsx->mv[LIST_0][jj][i][0]), invmv_precision));
                p->mv[LIST_1][j][i][1] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_1] * fsx->mv[LIST_0][jj][i][1]), invmv_precision));
              }
              else
              {
                p->mv[LIST_0][j][i][0] = 0;
                p->mv[LIST_0][j][i][1] = 0;
                p->mv[LIST_1][j][i][0] = 0;
                p->mv[LIST_1][j][i][1] = 0;
              }

            }
            else
            {
              tempmv_scale[LIST_0] = 256;
              tempmv_scale[LIST_1] = 0;

              if (fs->ref_id [LIST_0][jdiv + 4][i] < 0 && listXsize[LIST_0] > 1)
              {
                fsx = fs_bottom1;
                loffset = 1;
              }
              else
              {
                fsx = fs_bottom;
                loffset = 0;
              }

              if (fs->ref_id [LIST_0][jdiv + 4][i] != -1)
              {
                for (iref = 0; iref < imin(img->num_ref_idx_l0_active,listXsize[LIST_0]); iref++)
                {
                  if (enc_picture->ref_pic_num[LIST_0][iref]==fs->ref_id [LIST_0][jdiv + 4][i])
                  {
                    tempmv_scale[LIST_0] = epzs_scale[loffset][LIST_0][iref];
                    tempmv_scale[LIST_1] = epzs_scale[loffset][LIST_1][iref];
                    break;
                  }
                }
                p->mv[LIST_0][j][i][0] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_0] * fsx->mv[LIST_0][jj][i][0]), invmv_precision));
                p->mv[LIST_0][j][i][1] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_0] * fsx->mv[LIST_0][jj][i][1]), invmv_precision));
                p->mv[LIST_1][j][i][0] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_1] * fsx->mv[LIST_0][jj][i][0]), invmv_precision));
                p->mv[LIST_1][j][i][1] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_1] * fsx->mv[LIST_0][jj][i][1]), invmv_precision));
              }
              else
              {
                p->mv[LIST_0][j][i][0] = 0;
                p->mv[LIST_0][j][i][1] = 0;
                p->mv[LIST_1][j][i][0] = 0;
                p->mv[LIST_1][j][i][1] = 0;
              }
            }
          }
          else
          {
            tempmv_scale[LIST_0] = 256;
            tempmv_scale[LIST_1] = 0;
            if (fs->ref_id [LIST_0][j][i] < 0 && listXsize[LIST_0] > 1)
            {
              fsx = fs1;
              loffset = 1;
            }
            else
            {
              fsx = fs;
              loffset = 0;
            }

            if (fsx->ref_id [LIST_0][j][i] != -1)
            {
              for (iref = 0; iref < imin(img->num_ref_idx_l0_active,listXsize[LIST_0]); iref++)
              {
                if (enc_picture->ref_pic_num[LIST_0][iref]==fsx->ref_id [LIST_0][j][i])
                {
                  tempmv_scale[LIST_0] = epzs_scale[loffset][LIST_0][iref];
                  tempmv_scale[LIST_1] = epzs_scale[loffset][LIST_1][iref];
                  break;
                }
              }
              p->mv[LIST_0][j][i][0] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_0] * fsx->mv[LIST_0][j][i][0]), invmv_precision));
              p->mv[LIST_0][j][i][1] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_0] * fsx->mv[LIST_0][j][i][1]), invmv_precision));
              p->mv[LIST_1][j][i][0] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_1] * fsx->mv[LIST_0][j][i][0]), invmv_precision));
              p->mv[LIST_1][j][i][1] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_1] * fsx->mv[LIST_0][j][i][1]), invmv_precision));
            }
            else
            {
              p->mv[LIST_0][j][i][0] = 0;
              p->mv[LIST_0][j][i][1] = 0;
              p->mv[LIST_1][j][i][0] = 0;
              p->mv[LIST_1][j][i][1] = 0;
            }
          }
        }
      }
    }

    //! Generate field MVs from Frame MVs
    if (img->structure || img->MbaffFrameFlag)
    {
      for (j = 0; j < fs->size_y / 8; j++)
      {
        for (i = 0; i < fs->size_x / 4; i++)
        {
          if (!img->MbaffFrameFlag)
          {
            tempmv_scale[LIST_0] = 256;
            tempmv_scale[LIST_1] = 0;

            if (fs->ref_id [LIST_0][j][i] < 0 && listXsize[LIST_0] > 1)
            {
              fsx = fs1;
              loffset = 1;
            }
            else
            {
              fsx = fs;
              loffset = 0;
            }

            if (fsx->ref_id [LIST_0][j][i] != -1)
            {
              for (iref = 0; iref < imin(img->num_ref_idx_l0_active,listXsize[LIST_0]); iref++)
              {
                if (enc_picture->ref_pic_num[LIST_0][iref]==fsx->ref_id [LIST_0][j][i])
                {
                  tempmv_scale[LIST_0] = epzs_scale[loffset][LIST_0][iref];
                  tempmv_scale[LIST_1] = epzs_scale[loffset][LIST_1][iref];
                  break;
                }
              }
              p->mv[LIST_0][j][i][0] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_0] * fsx->mv[LIST_0][j][i][0]), invmv_precision));
              p->mv[LIST_0][j][i][1] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_0] * fsx->mv[LIST_0][j][i][1]), invmv_precision));
              p->mv[LIST_1][j][i][0] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_1] * fsx->mv[LIST_0][j][i][0]), invmv_precision));
              p->mv[LIST_1][j][i][1] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_1] * fsx->mv[LIST_0][j][i][1]), invmv_precision));
            }
            else
            {
              p->mv[LIST_0][j][i][0] = 0;
              p->mv[LIST_0][j][i][1] = 0;
              p->mv[LIST_1][j][i][0] = 0;
              p->mv[LIST_1][j][i][1] = 0;
            }
          }
          else
          {
            tempmv_scale[LIST_0] = 256;
            tempmv_scale[LIST_1] = 0;

            if (fs_bottom->ref_id [LIST_0][j][i] < 0 && listXsize[LIST_0] > 1)
            {
              fsx = fs_bottom1;
              loffset = 1;
            }
            else
            {
              fsx = fs_bottom;
              loffset = 0;
            }

            if (fsx->ref_id [LIST_0][j][i] != -1)
            {
              for (iref = 0; iref < imin(2*img->num_ref_idx_l0_active,listXsize[LIST_0 + 4]); iref++)
              {
                if (enc_picture->ref_pic_num[LIST_0 + 4][iref]==fsx->ref_id [LIST_0][j][i])
                {
                  tempmv_scale[LIST_0] = epzs_scale[loffset][LIST_0 + 4][iref];
                  tempmv_scale[LIST_1] = epzs_scale[loffset][LIST_1 + 4][iref];
                  break;
                }
              }
              p->bottom_mv[LIST_0][j][i][0] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_0] * fsx->mv[LIST_0][j][i][0]), invmv_precision));
              p->bottom_mv[LIST_0][j][i][1] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_0] * fsx->mv[LIST_0][j][i][1]), invmv_precision));
              p->bottom_mv[LIST_1][j][i][0] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_1] * fsx->mv[LIST_0][j][i][0]), invmv_precision));
              p->bottom_mv[LIST_1][j][i][1] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_1] * fsx->mv[LIST_0][j][i][1]), invmv_precision));
            }
            else
            {
              p->bottom_mv[LIST_0][j][i][0] = 0;
              p->bottom_mv[LIST_0][j][i][1] = 0;
              p->bottom_mv[LIST_1][j][i][0] = 0;
              p->bottom_mv[LIST_1][j][i][1] = 0;
            }

            if (!fs->field_frame[2 * j][i])
            {
              p->bottom_mv[LIST_0][j][i][1] = (p->bottom_mv[LIST_0][j][i][1] + 1) >> 1;
              p->bottom_mv[LIST_1][j][i][1] = (p->bottom_mv[LIST_1][j][i][1] + 1) >> 1;
            }

            tempmv_scale[LIST_0] = 256;
            tempmv_scale[LIST_1] = 0;

            if (fs_top->ref_id [LIST_0][j][i] < 0 && listXsize[LIST_0] > 1)
            {
              fsx = fs_top1;
              loffset = 1;
            }
            else
            {
              fsx = fs_top;
              loffset = 0;
            }

            if (fsx->ref_id [LIST_0][j][i] != -1)
            {
              for (iref = 0; iref < imin(2*img->num_ref_idx_l0_active,listXsize[LIST_0 + 2]); iref++)
              {
                if (enc_picture->ref_pic_num[LIST_0 + 2][iref]==fsx->ref_id [LIST_0][j][i])
                {
                  tempmv_scale[LIST_0] = epzs_scale[loffset][LIST_0 + 2][iref];
                  tempmv_scale[LIST_1] = epzs_scale[loffset][LIST_1 + 2][iref];
                  break;
                }
              }
              p->top_mv[LIST_0][j][i][0] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_0] * fsx->mv[LIST_0][j][i][0]), invmv_precision));
              p->top_mv[LIST_0][j][i][1] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_0] * fsx->mv[LIST_0][j][i][1]), invmv_precision));
              p->top_mv[LIST_1][j][i][0] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_1] * fsx->mv[LIST_0][j][i][0]), invmv_precision));
              p->top_mv[LIST_1][j][i][1] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_1] * fsx->mv[LIST_0][j][i][1]), invmv_precision));
            }
            else
            {
              p->top_mv[LIST_0][j][i][0] = 0;
              p->top_mv[LIST_0][j][i][1] = 0;
              p->top_mv[LIST_1][j][i][0] = 0;
              p->top_mv[LIST_1][j][i][1] = 0;
            }

            if (!fs->field_frame[2 * j][i])
            {
              p->top_mv[LIST_0][j][i][1] = rshift_rnd_sf((p->top_mv[LIST_0][j][i][1]), 1);
              p->top_mv[LIST_1][j][i][1] = rshift_rnd_sf((p->top_mv[LIST_1][j][i][1]), 1);
            }
          }
        }
      }
    }

    //if (!active_sps->frame_mbs_only_flag || active_sps->direct_8x8_inference_flag)
    if (!active_sps->frame_mbs_only_flag )
    {
      //! Use inference flag to remap mvs/references
      //! Frame with field co-located
      if (!img->structure)
      {
        for (j = 0; j < fs->size_y >> 2; j++)
        {
          jj = j>>1;
          jdiv = (j>>1) + ((j>>3) << 2);
          for (i = 0; i < fs->size_x >> 2; i++)
          {
            if (fs->field_frame[j][i])
            {
              tempmv_scale[LIST_0] = 256;
              tempmv_scale[LIST_1] = 0;

              if (fs->ref_id [LIST_0][jdiv][i] < 0 && listXsize[LIST_0] > 1)
              {
                fsx = fs1;
                loffset = 1;
              }
              else
              {
                fsx = fs;
                loffset = 0;
              }
              if (fsx->ref_id [LIST_0][jdiv][i] != -1)
              {
                for (iref = 0; iref < imin(img->num_ref_idx_l0_active,listXsize[LIST_0]); iref++)
                {
                  if (enc_picture->ref_pic_num[LIST_0][iref]==fsx->ref_id [LIST_0][jdiv][i])
                  {
                    tempmv_scale[LIST_0] = epzs_scale[loffset][LIST_0][iref];
                    tempmv_scale[LIST_1] = epzs_scale[loffset][LIST_1][iref];
                    break;
                  }
                }

                if (iabs (enc_picture->poc - fsx->bottom_field->poc) > iabs (enc_picture->poc - fsx->top_field->poc))
                {
                  p->mv[LIST_0][j][i][0] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_0] * fsx->top_field->mv[LIST_0][jj][i][0]), invmv_precision));
                  p->mv[LIST_0][j][i][1] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_0] * fsx->top_field->mv[LIST_0][jj][i][1]), invmv_precision));
                  p->mv[LIST_1][j][i][0] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_1] * fsx->top_field->mv[LIST_0][jj][i][0]), invmv_precision));
                  p->mv[LIST_1][j][i][1] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_1] * fsx->top_field->mv[LIST_0][jj][i][1]), invmv_precision));
                }
                else
                {
                  p->mv[LIST_0][j][i][0] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_0] * fsx->bottom_field->mv[LIST_0][jj][i][0]), invmv_precision));
                  p->mv[LIST_0][j][i][1] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_0] * fsx->bottom_field->mv[LIST_0][jj][i][1]), invmv_precision));
                  p->mv[LIST_1][j][i][0] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_1] * fsx->bottom_field->mv[LIST_0][jj][i][0]), invmv_precision));
                  p->mv[LIST_1][j][i][1] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_1] * fsx->bottom_field->mv[LIST_0][jj][i][1]), invmv_precision));
                }
              }
              else
              {
                p->mv[LIST_0][j][i][0] = 0;
                p->mv[LIST_0][j][i][1] = 0;
                p->mv[LIST_1][j][i][0] = 0;
                p->mv[LIST_1][j][i][1] = 0;
              }
            }
          }
        }
      }
    }

    if (active_sps->frame_mbs_only_flag)
    {
      for (j = 0; j < fs->size_y >> 2; j++)
      {
        for (i = 0; i < fs->size_x >> 2; i++)
        {
          tempmv_scale[LIST_0] = 256;
          tempmv_scale[LIST_1] = 0;
          if (fs->ref_id [LIST_0][j][i] < 0 && listXsize[LIST_0] > 1)
          {
            fsx = fs1;
            loffset = 1;
          }
          else
          {
            fsx = fs;
            loffset = 0;
          }
          if (fsx->ref_id [LIST_0][j][i] != -1)
          {
            for (iref = 0; iref < imin(img->num_ref_idx_l0_active,listXsize[LIST_0]); iref++)
            {
              if (enc_picture->ref_pic_num[LIST_0][iref]==fsx->ref_id [LIST_0][j][i])
              {
                tempmv_scale[LIST_0] = epzs_scale[loffset][LIST_0][iref];
                tempmv_scale[LIST_1] = epzs_scale[loffset][LIST_1][iref];
                break;
              }
            }

            p->mv[LIST_0][j][i][0] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_0] * fsx->mv[LIST_0][j][i][0]), invmv_precision));
            p->mv[LIST_0][j][i][1] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_0] * fsx->mv[LIST_0][j][i][1]), invmv_precision));
            p->mv[LIST_1][j][i][0] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_1] * fsx->mv[LIST_0][j][i][0]), invmv_precision));
            p->mv[LIST_1][j][i][1] = iClip3 (-32768, 32767, rshift_rnd_sf((tempmv_scale[LIST_1] * fsx->mv[LIST_0][j][i][1]), invmv_precision));
          }
          else
          {
            p->mv[LIST_0][j][i][0] = 0;
            p->mv[LIST_0][j][i][1] = 0;
            p->mv[LIST_1][j][i][0] = 0;
            p->mv[LIST_1][j][i][1] = 0;
          }
        }
      }
    }

    if (!active_sps->frame_mbs_only_flag)
    {
      for (j = 0; j < fs->size_y >> 2; j++)
      {
        for (i = 0; i < fs->size_x >> 2; i++)
        {
          if ((!img->MbaffFrameFlag && !img->structure && fs->field_frame[j][i]) || (img->MbaffFrameFlag && fs->field_frame[j][i]))
          {
            p->mv[LIST_0][j][i][1] *= 2;
            p->mv[LIST_1][j][i][1] *= 2;
          }
          else if (img->structure && !fs->field_frame[j][i])
          {
            p->mv[LIST_0][j][i][1] = rshift_rnd_sf((p->mv[LIST_0][j][i][1]), 1);
            p->mv[LIST_1][j][i][1] = rshift_rnd_sf((p->mv[LIST_1][j][i][1]), 1);
          }
        }
      }
    }
  }
}

/*!
***********************************************************************
* \brief
*    Spatial Predictors
*    AMT/HYC
***********************************************************************
*/
static short EPZSSpatialPredictors (PixelPos block_a,
                                    PixelPos block_b,
                                    PixelPos block_c,
                                    PixelPos block_d,
                                    int list,
                                    int list_offset,
                                    short ref,
                                    signed char **refPic,
                                    short ***tmp_mv,
                                    EPZSStructure * predictor)
{
  int refA, refB, refC, refD;
  int *mot_scale = mv_scale[list + list_offset][ref];
  short sp_shift_mv = 8 + mv_rescale;

  // zero predictor
  predictor->point[0].mv[0] = 0;
  predictor->point[0].mv[1] = 0;

  // Non MB-AFF mode
  if (!img->MbaffFrameFlag)
  {
    refA = block_a.available ? (int) refPic[block_a.pos_y][block_a.pos_x] : -1;
    refB = block_b.available ? (int) refPic[block_b.pos_y][block_b.pos_x] : -1;
    refC = block_c.available ? (int) refPic[block_c.pos_y][block_c.pos_x] : -1;
    refD = block_d.available ? (int) refPic[block_d.pos_y][block_d.pos_x] : -1;

    // Left Predictor
    if (block_a.available)
    {
      predictor->point[1].mv[0] = rshift_rnd_sf((mot_scale[refA] * tmp_mv[block_a.pos_y][block_a.pos_x][0]), sp_shift_mv);
      predictor->point[1].mv[1] = rshift_rnd_sf((mot_scale[refA] * tmp_mv[block_a.pos_y][block_a.pos_x][1]), sp_shift_mv);
    }
    else
    {
      predictor->point[1].mv[0] = (12 >> mv_rescale);
      predictor->point[1].mv[1] = 0;
    }
    // Up predictor
    if (block_b.available)
    {
      predictor->point[2].mv[0] = rshift_rnd_sf((mot_scale[refB] * tmp_mv[block_b.pos_y][block_b.pos_x][0]), sp_shift_mv);
      predictor->point[2].mv[1] = rshift_rnd_sf((mot_scale[refB] * tmp_mv[block_b.pos_y][block_b.pos_x][1]), sp_shift_mv);
    }
    else
    {
      predictor->point[2].mv[0] = 0;
      predictor->point[2].mv[1] = (12 >> mv_rescale);
    }

    // Up-Right predictor
    if (block_c.available)
    {
      predictor->point[3].mv[0] = rshift_rnd_sf((mot_scale[refC] * tmp_mv[block_c.pos_y][block_c.pos_x][0]), sp_shift_mv);
      predictor->point[3].mv[1] = rshift_rnd_sf((mot_scale[refC] * tmp_mv[block_c.pos_y][block_c.pos_x][1]), sp_shift_mv);
    }
    else
    {
      predictor->point[3].mv[0] = -(12 >> mv_rescale);
      predictor->point[3].mv[1] = 0;
    }

    //Up-Left predictor
    if (block_d.available)
    {
      predictor->point[4].mv[0] = rshift_rnd_sf((mot_scale[refD] * tmp_mv[block_d.pos_y][block_d.pos_x][0]), sp_shift_mv);
      predictor->point[4].mv[1] = rshift_rnd_sf((mot_scale[refD] * tmp_mv[block_d.pos_y][block_d.pos_x][1]), sp_shift_mv);
    }
    else
    {
      predictor->point[4].mv[0] = 0;
      predictor->point[4].mv[1] = -(12 >> mv_rescale);
    }
  }
  else  // MB-AFF mode
  {
    // Field Macroblock
    if (list_offset)
    {
      refA = block_a.available
        ? img->mb_data[block_a.mb_addr].mb_field
        ? (int) refPic[block_a.pos_y][block_a.pos_x]
        : (int) refPic[block_a.pos_y][block_a.pos_x] * 2 : -1;
      refB =block_b.available
        ? img->mb_data[block_b.mb_addr].mb_field
        ? (int) refPic[block_b.pos_y][block_b.pos_x]
        : (int) refPic[block_b.pos_y][block_b.pos_x] * 2 : -1;
      refC = block_c.available
        ? img->mb_data[block_c.mb_addr].mb_field
        ? (int) refPic[block_c.pos_y][block_c.pos_x]
        : (int) refPic[block_c.pos_y][block_c.pos_x] * 2 : -1;
      refD = block_d.available
        ? img->mb_data[block_d.mb_addr].mb_field
        ? (int) refPic[block_d.pos_y][block_d.pos_x]
        : (int) refPic[block_d.pos_y][block_d.pos_x] * 2 : -1;

      // Left Predictor
      predictor->point[1].mv[0] = (block_a.available)
        ? rshift_rnd_sf((mot_scale[refA] * tmp_mv[block_a.pos_y][block_a.pos_x][0]), sp_shift_mv) :  (12 >> mv_rescale);
      predictor->point[1].mv[1] = (block_a.available)
        ? img->mb_data[block_a.mb_addr].mb_field
        ? rshift_rnd_sf((mot_scale[refA] * tmp_mv[block_a.pos_y][block_a.pos_x][1]), sp_shift_mv)
        : rshift_rnd_sf((mot_scale[refA] * tmp_mv[block_a.pos_y][block_a.pos_x][1]), sp_shift_mv + 1) :  0;

      // Up predictor
      predictor->point[2].mv[0] = (block_b.available)
        ? rshift_rnd_sf((mot_scale[refB] * tmp_mv[block_b.pos_y][block_b.pos_x][0]), sp_shift_mv) : 0;
      predictor->point[2].mv[1] = (block_b.available)
        ? img->mb_data[block_b.mb_addr].mb_field
        ? rshift_rnd_sf((mot_scale[refB] * tmp_mv[block_b.pos_y][block_b.pos_x][1]), sp_shift_mv)
        : rshift_rnd_sf((mot_scale[refB] * tmp_mv[block_b.pos_y][block_b.pos_x][1]), sp_shift_mv + 1) : (12 >> mv_rescale);

      // Up-Right predictor
      predictor->point[3].mv[0] = (block_c.available)
        ? rshift_rnd_sf((mot_scale[refC] * tmp_mv[block_c.pos_y][block_c.pos_x][0]), sp_shift_mv) : -(12 >> mv_rescale);
      predictor->point[3].mv[1] = (block_c.available)
        ? img->mb_data[block_c.mb_addr].mb_field
        ? rshift_rnd_sf((mot_scale[refC] * tmp_mv[block_c.pos_y][block_c.pos_x][1]), sp_shift_mv)
        : rshift_rnd_sf((mot_scale[refC] * tmp_mv[block_c.pos_y][block_c.pos_x][1]), sp_shift_mv + 1) : 0;

      //Up-Left predictor
      predictor->point[4].mv[0] = (block_d.available)
        ? rshift_rnd_sf((mot_scale[refD] * tmp_mv[block_d.pos_y][block_d.pos_x][0]), sp_shift_mv) : 0;
      predictor->point[4].mv[1] = (block_d.available)
        ? img->mb_data[block_d.mb_addr].mb_field
        ? rshift_rnd_sf((mot_scale[refD] * tmp_mv[block_d.pos_y][block_d.pos_x][1]), sp_shift_mv)
        : rshift_rnd_sf((mot_scale[refD] * tmp_mv[block_d.pos_y][block_d.pos_x][1]), sp_shift_mv + 1) : -(12 >> mv_rescale);
    }
    else // Frame macroblock
    {
      refA = block_a.available
        ? img->mb_data[block_a.mb_addr].mb_field
        ? (int) refPic[block_a.pos_y][block_a.pos_x] >> 1
        : (int) refPic[block_a.pos_y][block_a.pos_x] : -1;
      refB = block_b.available
        ? img->mb_data[block_b.mb_addr].mb_field
        ? (int) refPic[block_b.pos_y][block_b.pos_x] >> 1
        : (int) refPic[block_b.pos_y][block_b.pos_x] : -1;
      refC = block_c.available
        ? img->mb_data[block_c.mb_addr].mb_field
        ? (int) refPic[block_c.pos_y][block_c.pos_x] >> 1
        : (int) refPic[block_c.pos_y][block_c.pos_x] : -1;
      refD = block_d.available
        ? img->mb_data[block_d.mb_addr].mb_field
        ? (int) refPic[block_d.pos_y][block_d.pos_x] >> 1
        : (int) refPic[block_d.pos_y][block_d.pos_x] : -1;

      // Left Predictor
      predictor->point[1].mv[0] = (block_a.available)
        ? rshift_rnd_sf((mot_scale[refA] * tmp_mv[block_a.pos_y][block_a.pos_x][0]), sp_shift_mv) : (12 >> mv_rescale);
      predictor->point[1].mv[1] = (block_a.available)
        ? img->mb_data[block_a.mb_addr].mb_field
        ? rshift_rnd_sf((mot_scale[refA] * tmp_mv[block_a.pos_y][block_a.pos_x][1]), sp_shift_mv - 1)
        : rshift_rnd_sf((mot_scale[refA] * tmp_mv[block_a.pos_y][block_a.pos_x][1]), sp_shift_mv) : 0;

      // Up predictor
      predictor->point[2].mv[0] = (block_b.available)
        ? rshift_rnd_sf((mot_scale[refB] * tmp_mv[block_b.pos_y][block_b.pos_x][0]), sp_shift_mv) : 0;
      predictor->point[2].mv[1] = (block_b.available)
        ? img->mb_data[block_b.mb_addr].mb_field
        ? rshift_rnd_sf((mot_scale[refB] * tmp_mv[block_b.pos_y][block_b.pos_x][1]), sp_shift_mv - 1)
        : rshift_rnd_sf((mot_scale[refB] * tmp_mv[block_b.pos_y][block_b.pos_x][1]), sp_shift_mv) : (12 >> mv_rescale);

      // Up-Right predictor
      predictor->point[3].mv[0] = (block_c.available)
        ? rshift_rnd_sf((mot_scale[refC] * tmp_mv[block_c.pos_y][block_c.pos_x][0]), sp_shift_mv) : -(12 >> mv_rescale);
      predictor->point[3].mv[1] = (block_c.available)
        ? img->mb_data[block_c.mb_addr].mb_field
        ? rshift_rnd_sf((mot_scale[refC] * tmp_mv[block_c.pos_y][block_c.pos_x][1]), sp_shift_mv - 1)
        : rshift_rnd_sf((mot_scale[refC] * tmp_mv[block_c.pos_y][block_c.pos_x][1]), sp_shift_mv) : 0;

      //Up-Left predictor
      predictor->point[4].mv[0] = (block_d.available)
        ? rshift_rnd_sf((mot_scale[refD] * tmp_mv[block_d.pos_y][block_d.pos_x][0]), sp_shift_mv) : 0;
      predictor->point[4].mv[1] = (block_d.available)
        ? img->mb_data[block_d.mb_addr].mb_field
        ? rshift_rnd_sf((mot_scale[refD] * tmp_mv[block_d.pos_y][block_d.pos_x][1]), sp_shift_mv - 1)
        : rshift_rnd_sf((mot_scale[refD] * tmp_mv[block_d.pos_y][block_d.pos_x][1]), sp_shift_mv) : -(12 >> mv_rescale);
    }
  }

  return ((refA == -1) + (refB == -1) + (refC == -1 && refD == -1));
}

/*!
***********************************************************************
* \brief
*    Spatial Predictors
*    AMT/HYC
***********************************************************************
*/

static void EPZSSpatialMemPredictors (int list,
                                      short ref,
                                      int blocktype,
                                      int pic_x,
                                      int bs_x,
                                      int bs_y,
                                      int by,
                                      int *prednum,
                                      int img_width,
                                      EPZSStructure * predictor)
{
#if EPZSREF
  short ***mv = EPZSMotion[list][ref][blocktype];
  int *cur_mv = predictor->point[*prednum].mv;

  // Left Predictor
  cur_mv[0] = (pic_x > 0) ? mv[by][pic_x - bs_x][0] :  0;
  cur_mv[1] = (pic_x > 0) ? mv[by][pic_x - bs_x][1] :  0;
  *prednum += (cur_mv[0] | cur_mv[1])!=0;

  // Up predictor
  cur_mv = predictor->point[*prednum].mv;
  cur_mv[0] = (by > 0) ? mv[by - bs_y][pic_x][0] : mv[4 - bs_y][pic_x][0];
  cur_mv[1] = (by > 0) ? mv[by - bs_y][pic_x][1] : mv[4 - bs_y][pic_x][1];
  *prednum += (cur_mv[0] | cur_mv[1])!=0;

  // Up-Right predictor
  cur_mv = predictor->point[*prednum].mv;
  cur_mv[0] = (pic_x + bs_x < img_width) ? (by > 0)
    ? mv[by - bs_y][pic_x + bs_x][0] : mv[4 - bs_y][pic_x + bs_x][0] : 0;
  cur_mv[1] = (pic_x + bs_x < img_width) ? (by > 0)
    ? mv[by - bs_y][pic_x + bs_x][1] : mv[4 - bs_y][pic_x + bs_x][1] : 0;
  *prednum += (cur_mv[0] | cur_mv[1])!=0;

#else
  int mot_scale = mv_scale[list][ref][0];
  short **mv = EPZSMotion[list][blocktype];

  // Left Predictor
  predictor->point[*prednum].mv[0] = (pic_x > 0)
    ? rshift_rnd_sf((mot_scale * mv[by][pic_x - bs_x][0]), 8)
    : 0;
  predictor->point[*prednum].mv[1] = (pic_x > 0)
    ? rshift_rnd_sf((mot_scale * mv[by][pic_x - bs_x][1]), 8)
    : 0;
  *prednum += ((predictor->point[*prednum].mv[0] != 0) || (predictor->point[*prednum].mv[1] != 0));

  // Up predictor
  predictor->point[*prednum].mv[0] = (by > 0)
    ? rshift_rnd_sf((mot_scale * mv[by - bs_y][pic_x][0]), 8)
    : rshift_rnd_sf((mot_scale * mv[4  - bs_y][pic_x][0]), 8);
  predictor->point[*prednum].mv[1] = (by > 0)
    ? rshift_rnd_sf((mot_scale * mv[by - bs_y][pic_x][1]), 8)
    : rshift_rnd_sf((mot_scale * mv[4  - bs_y][pic_x][1]), 8);
  *prednum += ((predictor->point[*prednum].mv[0] != 0) || (predictor->point[*prednum].mv[1] != 0));

  // Up-Right predictor
  predictor->point[*prednum].mv[0] = (pic_x + bs_x < img_width)
    ? (by > 0)
    ? rshift_rnd_sf((mot_scale * mv[by - bs_y][pic_x + bs_x][0]), 8)
    : rshift_rnd_sf((mot_scale * mv[4  - bs_y][pic_x + bs_x][0]), 8)
    : 0;
  predictor->point[*prednum].mv[1] = (pic_x + bs_x < img_width)
    ? (by > 0)
    ? rshift_rnd_sf((mot_scale * mv[by - bs_y][pic_x + bs_x][1]), 8)
    : rshift_rnd_sf((mot_scale * mv[4  - bs_y][pic_x + bs_x][1]), 8)
    : 0;
  *prednum += ((predictor->point[*prednum].mv[0] != 0) || (predictor->point[*prednum].mv[1] != 0));
#endif
}

/*!
***********************************************************************
* \brief
*    Temporal Predictors
*    AMT/HYC
***********************************************************************
*/
static void
EPZSTemporalPredictors (int list,         // <--  current list
                        int list_offset,  // <--  list offset for MBAFF
                        short ref,        // <--  current reference frame
                        int o_block_x,    // <--  absolute x-coordinate of regarded AxB block
                        int o_block_y,    // <--  absolute y-coordinate of regarded AxB block
                        EPZSStructure * predictor,
                        int *prednum,
                        int block_available_left,
                        int block_available_up,
                        int block_available_right,
                        int block_available_below,
                        int blockshape_x,
                        int blockshape_y,
                        int stopCriterion,
                        int min_mcost)
{
  int mvScale = mv_scale[list + list_offset][ref][0];
  short ***col_mv = (list_offset == 0) ? EPZSCo_located->mv[list]
    : (list_offset == 2) ? EPZSCo_located->top_mv[list] : EPZSCo_located->bottom_mv[list];
  short temp_shift_mv = 8 + mv_rescale; // 16 - invmv_precision + mv_rescale
  int *cur_mv = predictor->point[*prednum].mv;

  cur_mv[0] = rshift_rnd_sf((mvScale * col_mv[o_block_y][o_block_x][0]), temp_shift_mv);
  cur_mv[1] = rshift_rnd_sf((mvScale * col_mv[o_block_y][o_block_x][1]), temp_shift_mv);
  *prednum += (cur_mv[0] | cur_mv[1])!=0;

  if (min_mcost > stopCriterion && ref < 2)
  {
    if (block_available_left)
    {
      cur_mv = predictor->point[*prednum].mv;
      cur_mv[0] = rshift_rnd_sf((mvScale * col_mv[o_block_y][o_block_x - 1][0]), temp_shift_mv);
      cur_mv[1] = rshift_rnd_sf((mvScale * col_mv[o_block_y][o_block_x - 1][1]), temp_shift_mv);
      *prednum += (cur_mv[0] | cur_mv[1])!=0;

      //Up_Left
      if (block_available_up)
      {
        cur_mv = predictor->point[*prednum].mv;
        cur_mv[0] = rshift_rnd_sf((mvScale * col_mv[o_block_y - 1][o_block_x - 1][0]), temp_shift_mv);
        cur_mv[1] = rshift_rnd_sf((mvScale * col_mv[o_block_y - 1][o_block_x - 1][1]), temp_shift_mv);
        *prednum += (cur_mv[0] | cur_mv[1])!=0;
      }
      //Down_Left
      if (block_available_below)
      {
        cur_mv = predictor->point[*prednum].mv;
        cur_mv[0] = rshift_rnd_sf((mvScale * col_mv[o_block_y + blockshape_y][o_block_x - 1][0]), temp_shift_mv);
        cur_mv[1] = rshift_rnd_sf((mvScale * col_mv[o_block_y + blockshape_y][o_block_x - 1][1]), temp_shift_mv);
        *prednum += (cur_mv[0] | cur_mv[1])!=0;
      }
    }
    // Up
    if (block_available_up)
    {
      cur_mv = predictor->point[*prednum].mv;
      cur_mv[0] = rshift_rnd_sf((mvScale * col_mv[o_block_y - 1][o_block_x][0]), temp_shift_mv);
      cur_mv[1] = rshift_rnd_sf((mvScale * col_mv[o_block_y - 1][o_block_x][1]), temp_shift_mv);
      *prednum += (cur_mv[0] | cur_mv[1])!=0;
    }

    // Up - Right
    if (block_available_right)
    {
      cur_mv = predictor->point[*prednum].mv;
      cur_mv[0] = rshift_rnd_sf((mvScale * col_mv[o_block_y][o_block_x + blockshape_x][0]), temp_shift_mv);
      cur_mv[1] = rshift_rnd_sf((mvScale * col_mv[o_block_y][o_block_x + blockshape_x][1]), temp_shift_mv);
      *prednum += (cur_mv[0] | cur_mv[1])!=0;

      if (block_available_up)
      {
        cur_mv = predictor->point[*prednum].mv;
        cur_mv[0] = rshift_rnd_sf((mvScale * col_mv[o_block_y - 1][o_block_x + blockshape_x][0]), temp_shift_mv);
        cur_mv[1] = rshift_rnd_sf((mvScale * col_mv[o_block_y - 1][o_block_x + blockshape_x][1]), temp_shift_mv);
        *prednum += (cur_mv[0] | cur_mv[1])!=0;
      }
      if (block_available_below)
      {
        cur_mv = predictor->point[*prednum].mv;
        cur_mv[0] = rshift_rnd_sf((mvScale * col_mv[o_block_y + blockshape_y][o_block_x + blockshape_x][0]), temp_shift_mv);
        cur_mv[1] = rshift_rnd_sf((mvScale * col_mv[o_block_y + blockshape_y][o_block_x + blockshape_x][1]), temp_shift_mv);
        *prednum += (cur_mv[0] | cur_mv[1])!=0;
      }
    }

    if (block_available_below)
    {
      cur_mv = predictor->point[*prednum].mv;
      cur_mv[0] = rshift_rnd_sf((mvScale * col_mv[o_block_y + blockshape_y][o_block_x][0]), temp_shift_mv);
      cur_mv[1] = rshift_rnd_sf((mvScale * col_mv[o_block_y + blockshape_y][o_block_x][1]), temp_shift_mv);
      *prednum += (cur_mv[0] | cur_mv[1])!=0;
    }
  }
}

/*!
************************************************************************
* \brief
*    EPZS Block Type Predictors
************************************************************************
*/
static void EPZSBlockTypePredictors (int block_x, int block_y, int blocktype, int ref, int list,
                                     EPZSStructure * predictor, int *prednum)
{
  short ***all_mv = img->all_mv[block_y][block_x][list];
  short block_shift_mv = 8 + mv_rescale;
  int *cur_mv = predictor->point[*prednum].mv;
  
  cur_mv[0] = rshift_rnd((all_mv[ref][(blk_parent[blocktype])][0]), mv_rescale);
  cur_mv[1] = rshift_rnd((all_mv[ref][(blk_parent[blocktype])][1]), mv_rescale);
  *prednum += ((cur_mv[0] | cur_mv[1])!=0);

  if ((ref > 0) && (blocktype < 5 || img->structure != FRAME))
  {
    cur_mv = predictor->point[*prednum].mv;
    cur_mv[0] = rshift_rnd_sf((mv_scale[list][ref][ref-1] * all_mv[ref-1][blocktype][0]), block_shift_mv );
    cur_mv[1] = rshift_rnd_sf((mv_scale[list][ref][ref-1] * all_mv[ref-1][blocktype][1]), block_shift_mv );
    *prednum += (cur_mv[0] | cur_mv[1])!=0;

    cur_mv = predictor->point[*prednum].mv;
    cur_mv[0] = rshift_rnd_sf((mv_scale[list][ref][0] * all_mv[0][blocktype][0]), block_shift_mv );
    cur_mv[1] = rshift_rnd_sf((mv_scale[list][ref][0] * all_mv[0][blocktype][1]), block_shift_mv );
    *prednum += (cur_mv[0] | cur_mv[1])!=0;
  }

  if (blocktype != 1)
  {
    cur_mv = predictor->point[*prednum].mv;
    cur_mv[0] = rshift_rnd((all_mv[ref][1][0]), mv_rescale);
    cur_mv[1] = rshift_rnd((all_mv[ref][1][1]), mv_rescale);
    *prednum += (cur_mv[0] | cur_mv[1])!=0;
  }

  if (blocktype != 4)
  {
    cur_mv = predictor->point[*prednum].mv;
    cur_mv[0] = rshift_rnd((all_mv[ref][4][0]), mv_rescale);
    cur_mv[1] = rshift_rnd((all_mv[ref][4][1]), mv_rescale);
    *prednum += (cur_mv[0] | cur_mv[1])!=0;
  }
}

/*!
************************************************************************
* \brief
*    EPZS Window Based Predictors
************************************************************************
*/
static void EPZSWindowPredictors (short mv[2], EPZSStructure *predictor, int *prednum, int extended)
{
  int pos;
  EPZSStructure *windowPred = (extended) ? window_predictor_extended : window_predictor;

  for (pos = 0; pos < windowPred->searchPoints; pos++)
  {
    predictor->point[(*prednum)  ].mv[0] = mv[0] + windowPred->point[pos].mv[0];
    predictor->point[(*prednum)++].mv[1] = mv[1] + windowPred->point[pos].mv[1];
  }
}

/*!
***********************************************************************
* \brief
*    FAST Motion Estimation using EPZS
*    AMT/HYC
***********************************************************************
*/
int                                           //  ==> minimum motion cost after search
EPZSPelBlockMotionSearch (imgpel * cur_pic,  // <--  original pixel values for the AxB block
                          short ref,          // <--  reference picture
                          int list,           // <--  reference list
                          int list_offset,    // <--  offset for Mbaff
                          signed char ***refPic, // <--  reference array
                          short ****tmp_mv,   // <--  mv array
                          int pic_pix_x,      // <--  absolute x-coordinate of regarded AxB block
                          int pic_pix_y,      // <--  absolute y-coordinate of regarded AxB block
                          int blocktype,      // <--  block type (1-16x16 ... 7-4x4)
                          short pred_mv[2],    // <--  motion vector predictor (x) in sub-pel units
                          short mv[2],        // <--> in: search center (x) / out: motion vector (x) - in pel units
                          int search_range,   // <--  1-d search range in pel units
                          int min_mcost,      // <--  minimum motion cost (cost for center or huge value)
                          int lambda_factor)      // <--  lagrangian parameter for determining motion cost
{
  StorablePicture *ref_picture = listX[list+list_offset][ref];
  short blocksize_y = input->blc_size[blocktype][1];  // vertical block size
  short blocksize_x = input->blc_size[blocktype][0];  // horizontal block size
  short blockshape_x = (blocksize_x >> 2);  // horizontal block size in 4-pel units
  short blockshape_y = (blocksize_y >> 2);  // vertical block size in 4-pel units

  short mb_x = pic_pix_x - img->opix_x;
  short mb_y = pic_pix_y - img->opix_y;
  short pic_pix_x2 = pic_pix_x >> 2;
  short pic_pix_y2 = pic_pix_y >> 2;
  short block_x = (mb_x >> 2);
  short block_y = (mb_y >> 2);

  int   pred_x = (pic_pix_x << 2) + pred_mv[0];  // predicted position x (in sub-pel units)
  int   pred_y = (pic_pix_y << 2) + pred_mv[1];  // predicted position y (in sub-pel units)
  int   center_x = (pic_pix_x << (2 - mv_rescale))+ mv[0]; // center position x (in pel units)
  int   center_y = (pic_pix_y << (2 - mv_rescale))+ mv[1]; // center position y (in pel units)
  int   cand_x = center_x << mv_rescale;
  int   cand_y = center_y << mv_rescale;
  int   tempmv[2]  = {mv[0], mv[1]};
  int   tempmv2[2] = {0, 0};
  int   stopCriterion = medthres[blocktype];
  int   mapCenter_x = search_range - mv[0];
  int   mapCenter_y = search_range - mv[1];
  int   second_mcost = INT_MAX;
  short apply_weights = (active_pps->weighted_pred_flag > 0 || (active_pps->weighted_bipred_idc && (img->type == B_SLICE))) && input->UseWeightedReferenceME;
  int   *prevSad = EPZSDistortion[list + list_offset][blocktype - 1];
  short *motion=NULL;
  int   dist_method = F_PEL + 3 * apply_weights;

  short invalid_refs = 0;
  byte  checkMedian = FALSE;
  EPZSStructure *searchPatternF = searchPattern;
  EPZSBlkCount ++;

  ref_pic_sub.luma = ref_picture->imgY_sub;
  img_width  = ref_picture->size_x;
  img_height = ref_picture->size_y;
  width_pad  = ref_picture->size_x_pad;
  height_pad = ref_picture->size_y_pad;

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
  pic_pix_x = (pic_pix_x << (2 - mv_rescale));
  pic_pix_y = (pic_pix_y << (2 - mv_rescale));

  if (input->EPZSSpatialMem)
  {
#if EPZSREF
    motion = EPZSMotion[list + list_offset][ref][blocktype - 1][block_y][pic_pix_x2];
#else
    motion = EPZSMotion[list + list_offset][blocktype - 1][block_y][pic_pix_x2];
#endif
  }

  //===== set function for getting reference picture lines =====
  ref_access_method = CHECK_RANGE ? FAST_ACCESS : UMV_ACCESS;

  // Clear EPZSMap
  // memset(EPZSMap[0],FALSE,searcharray*searcharray);
  // Check median candidate;
  EPZSMap[search_range][search_range] = EPZSBlkCount;

  //--- initialize motion cost (cost for motion vector) and check ---
  min_mcost = MV_COST_SMP (lambda_factor, cand_x, cand_y, pred_x, pred_y);

  //--- add residual cost to motion cost ---
  min_mcost += computeUniPred[dist_method](cur_pic, blocksize_y, blocksize_x,
    INT_MAX, cand_x + IMG_PAD_SIZE_TIMES4, cand_y + IMG_PAD_SIZE_TIMES4);

  // Additional threshold for ref>0
  if ((ref>0 && img->structure == FRAME)
    && (prevSad[pic_pix_x2] < medthres[blocktype])
    && (prevSad[pic_pix_x2] < min_mcost))
  {
#if EPZSREF
    if (input->EPZSSpatialMem)
#else
    if (input->EPZSSpatialMem && ref == 0)
#endif
    {
      motion[0]  = tempmv[0];
      motion[1]  = tempmv[1];
    }

    return min_mcost;
  }

  //  if ((center_x > search_range) && (center_x < img_width  - search_range - blocksize_x) &&
  //(center_y > search_range) && (center_y < img_height - search_range - blocksize_y)   )
  if ( (center_x > search_range) && (center_x < ((img_width  - blocksize_x) << (input->EPZSSubPelGrid * 2)) - search_range)
    && (center_y > search_range) && (center_y < ((img_height - blocksize_y) << (input->EPZSSubPelGrid * 2)) - search_range))
  {
    ref_access_method = FAST_ACCESS;
  }
  else
  {
    ref_access_method = UMV_ACCESS;
  }

  //! If medthres satisfied, then terminate, otherwise generate Predictors
  //! Condition could be strengthened by consideration distortion of adjacent partitions.
  if (min_mcost > stopCriterion)
  {
    int mb_available_right   = (img->mb_x < (img_width  >> 4) - 1);
    int mb_available_below   = (img->mb_y < (img_height >> 4) - 1);

    int sadA, sadB, sadC;
    int block_available_right;
    int block_available_below;
    int prednum = 5;
    int patternStop = 0, pointNumber = 0, checkPts, nextLast = 0;
    int totalCheckPts = 0, motionDirection = 0;
    int conditionEPZS;
    int tmv[2];
    int pos, mcost;
    PixelPos block_a, block_b, block_c, block_d;

    getLuma4x4Neighbour (img->current_mb_nr, mb_x - 1, mb_y, &block_a);
    getLuma4x4Neighbour (img->current_mb_nr, mb_x, mb_y - 1, &block_b);
    getLuma4x4Neighbour (img->current_mb_nr, mb_x + blocksize_x, mb_y -1, &block_c);
    getLuma4x4Neighbour (img->current_mb_nr, mb_x - 1, mb_y -1, &block_d);

    if (mb_y > 0)
    {
      if (mb_x < 8)   // first column of 8x8 blocks
      {
        if (mb_y == 8)
        {
          block_available_right = (blocksize_x != MB_BLOCK_SIZE) || mb_available_right;
          if (blocksize_x == MB_BLOCK_SIZE)
            block_c.available = 0;
        }
        else
        {
          block_available_right = (mb_x + blocksize_x != 8) || mb_available_right;
          if (mb_x + blocksize_x == 8)
            block_c.available = 0;
        }
      }
      else
      {
        block_available_right = (mb_x + blocksize_x != MB_BLOCK_SIZE) || mb_available_right;
        if (mb_x + blocksize_x == MB_BLOCK_SIZE)
          block_c.available = 0;
      }
    }
    else
    {
      block_available_right = (mb_x + blocksize_x != MB_BLOCK_SIZE) || mb_available_right;
    }
    block_available_below = (mb_y + blocksize_y != MB_BLOCK_SIZE) || (mb_available_below);

    sadA = block_a.available ? prevSad[pic_pix_x2 - blockshape_x] : INT_MAX;
    sadB = block_b.available ? prevSad[pic_pix_x2] : INT_MAX;
    sadC = block_c.available ? prevSad[pic_pix_x2 + blockshape_x] : INT_MAX;

    stopCriterion = imin(sadA,imin(sadB,sadC));
    stopCriterion = imax(stopCriterion,minthres[blocktype]);
    stopCriterion = imin(stopCriterion,maxthres[blocktype]);

    stopCriterion = (9 * imax (medthres[blocktype], stopCriterion) + 2 * medthres[blocktype]) >> 3;

    //! Add Spatial Predictors in predictor list.
    //! Scheme adds zero, left, top-left, top, top-right. Note that top-left adds very little
    //! in terms of performance and could be removed with little penalty if any.
    invalid_refs = EPZSSpatialPredictors (block_a, block_b, block_c, block_d,
      list, list_offset, ref, refPic[list], tmp_mv[list], predictor);
    if (input->EPZSSpatialMem)
      EPZSSpatialMemPredictors (list + list_offset, ref, blocktype - 1, pic_pix_x2,
      blockshape_x, blockshape_y, block_y, &prednum, img_width>>2, predictor);

    // Temporal predictors
    if (input->EPZSTemporal)
      EPZSTemporalPredictors (list, list_offset, ref, pic_pix_x2, pic_pix_y2, predictor, &prednum,
      block_a.available, block_b.available, block_available_right,
      block_available_below, blockshape_x, blockshape_y, stopCriterion, min_mcost);

    //! Window Size Based Predictors
    //! Basically replaces a Hierarchical ME concept and helps escaping local minima, or
    //! determining large motion variations.
    //! Following predictors can be adjusted further (i.e. removed, conditioned etc)
    //! based on distortion, correlation of adjacent MVs, complexity etc. These predictors
    //! and their conditioning could also be moved after all other predictors have been
    //! tested. Adaptation could also be based on type of material and coding mode (i.e.
    //! field/frame coding,MBAFF etc considering the higher dependency with opposite parity field
    //conditionEPZS = ((min_mcost > stopCriterion)
    // && (input->EPZSFixed > 1 || (input->EPZSFixed && img->type == P_SLICE)));
    //conditionEPZS = ((ref == 0) && (blocktype < 5) && (min_mcost > stopCriterion)
    //&& (input->EPZSFixed > 1 || (input->EPZSFixed && img->type == P_SLICE)));
    conditionEPZS = ((min_mcost > stopCriterion) && ((ref < 2 && blocktype < 5)
      || ((img->structure!=FRAME || list_offset) && ref < 3))
      && (input->EPZSFixed > 1 || (input->EPZSFixed && img->type == P_SLICE)));

    if (conditionEPZS)
      EPZSWindowPredictors (mv, predictor, &prednum,
      (blocktype < 5) && (invalid_refs > 2) && (ref < 1 + (img->structure!=FRAME || list_offset)));

    //! Blocktype/Reference dependent predictors.
    //! Since already mvs for other blocktypes/references have been computed, we can reuse
    //! them in order to easier determine the optimal point. Use of predictors could depend
    //! on cost,
    //conditionEPZS = (ref == 0 || (ref > 0 && min_mcost > stopCriterion) || img->structure != FRAME || list_offset);
    conditionEPZS = (ref == 0 || (ref > 0 && min_mcost > stopCriterion));
    // above seems to result in memory leak issues which need to be resolved

    if (conditionEPZS && img->current_mb_nr != 0)
      EPZSBlockTypePredictors (block_x, block_y, blocktype, ref, list, predictor, &prednum);

    //! Check all predictors
    for (pos = 0; pos < prednum; pos++)
    {
      tmv[0] = predictor->point[pos].mv[0];
      tmv[1] = predictor->point[pos].mv[1];
      //if ((iabs (tmv[0] - mv[0]) > search_range || iabs (tmv[1] - mv[1]) > search_range) && (tmv[0] || tmv[1]))
      if (iabs (tmv[0] - mv[0]) > search_range || iabs (tmv[1] - mv[1]) > search_range)
        continue;

      if ((iabs (tmv[0] - mv[0]) <= search_range) && (iabs (tmv[1] - mv[1]) <= search_range))
      {
        if (EPZSMap[mapCenter_y + tmv[1]][mapCenter_x + tmv[0]] == EPZSBlkCount)
          continue;
        else
          EPZSMap[mapCenter_y + tmv[1]][mapCenter_x + tmv[0]] = EPZSBlkCount;
      }

      cand_x = (pic_pix_x + tmv[0])<<mv_rescale;
      cand_y = (pic_pix_y + tmv[1])<<mv_rescale;

      //--- set motion cost (cost for motion vector) and check ---
      mcost = MV_COST_SMP (lambda_factor, cand_x, cand_y, pred_x, pred_y);

      if (mcost >= second_mcost) continue;

      ref_access_method = CHECK_RANGE ? FAST_ACCESS : UMV_ACCESS;

      mcost += computeUniPred[dist_method](cur_pic, blocksize_y,blocksize_x,
        second_mcost - mcost, cand_x + IMG_PAD_SIZE_TIMES4,cand_y + IMG_PAD_SIZE_TIMES4);

      //--- check if motion cost is less than minimum cost ---
      if (mcost < min_mcost)
      {
        tempmv2[0] = tempmv[0];
        tempmv2[1] = tempmv[1];
        tempmv[0] = tmv[0];
        tempmv[1] = tmv[1];
        second_mcost = min_mcost;
        min_mcost = mcost;
        checkMedian = TRUE;
      }
      //else if (mcost < second_mcost && (tempmv[0] != tmv[0] || tempmv[1] != tmv[1]))
      else if (mcost < second_mcost)
      {
        tempmv2[0] = tmv[0];
        tempmv2[1] = tmv[1];
        second_mcost = mcost;
        checkMedian = TRUE;
      }
    }

    //! Refine using EPZS pattern if needed
    //! Note that we are using a conservative threshold method. Threshold
    //! could be tested after checking only a certain number of predictors
    //! instead of the full set. Code could be easily modified for this task.
    if (min_mcost > stopCriterion)
    {
      //! Adapt pattern based on different conditions.
      if (input->EPZSPattern != 0)
      {
        if ((min_mcost < stopCriterion + ((3 * medthres[blocktype]) >> 1)))
        {
          if ((tempmv[0] == 0 && tempmv[1] == 0)
            || (iabs (tempmv[0] - mv[0]) < (2<<(2-mv_rescale)) && iabs (tempmv[1] - mv[1]) < (2<<(2-mv_rescale))))
            searchPatternF = sdiamond;
          else
            searchPatternF = square;
        }
        else if (blocktype > 5 || (ref > 0 && blocktype != 1))
          searchPatternF = square;
        else
          searchPatternF = searchPattern;
      }

      totalCheckPts = searchPatternF->searchPoints;

      //! center on best predictor
      center_x = tempmv[0];
      center_y = tempmv[1];
      while(1)
      {
        do
        {
          checkPts = totalCheckPts;
          do
          {
            tmv[0] = center_x + searchPatternF->point[pointNumber].mv[0];
            tmv[1] = center_y + searchPatternF->point[pointNumber].mv[1];
            cand_x = (pic_pix_x + tmv[0])<<mv_rescale;
            cand_y = (pic_pix_y + tmv[1])<<mv_rescale;

            if ((iabs (tmv[0] - mv[0]) <= search_range)
              && (iabs (tmv[1] - mv[1]) <= search_range))
            {
              if (EPZSMap[mapCenter_y + tmv[1]][mapCenter_x + tmv[0]] != EPZSBlkCount)
                EPZSMap[mapCenter_y + tmv[1]][mapCenter_x + tmv[0]] = EPZSBlkCount;
              else
              {
                pointNumber += 1;
                if (pointNumber >= searchPatternF->searchPoints)
                  pointNumber -= searchPatternF->searchPoints;
                checkPts -= 1;
                continue;
              }
              mcost = MV_COST_SMP (lambda_factor, cand_x, cand_y, pred_x, pred_y);

              if (mcost < min_mcost)
              {
                ref_access_method = CHECK_RANGE ? FAST_ACCESS : UMV_ACCESS;

                mcost += computeUniPred[dist_method](cur_pic, blocksize_y,blocksize_x,
                  min_mcost - mcost, cand_x + IMG_PAD_SIZE_TIMES4, cand_y + IMG_PAD_SIZE_TIMES4);

                if (mcost < min_mcost)
                {
                  min_mcost = mcost;
                  tempmv[0] = tmv[0];
                  tempmv[1] = tmv[1];
                  motionDirection = pointNumber;
                }
              }
            }
            pointNumber += 1;
            if (pointNumber >= searchPatternF->searchPoints)
              pointNumber -= searchPatternF->searchPoints;
            checkPts -= 1;
          }
          while (checkPts > 0);

          if (nextLast || ((tempmv[0] == center_x) && (tempmv[1] == center_y)))
          {
            patternStop     = searchPatternF->stopSearch;
            searchPatternF  = searchPatternF->nextpattern;
            totalCheckPts   = searchPatternF->searchPoints;
            nextLast        = searchPatternF->nextLast;
            motionDirection = 0;
            pointNumber = 0;
          }
          else
          {
            totalCheckPts = searchPatternF->point[motionDirection].next_points;
            pointNumber = searchPatternF->point[motionDirection].start_nmbr;
            center_x = tempmv[0];
            center_y = tempmv[1];
          }
        }
        while (patternStop != 1);

        if ((ref>0) && (img->structure == FRAME)
          && (( 4 * prevSad[pic_pix_x2] < min_mcost) ||
          ((3 * prevSad[pic_pix_x2] < min_mcost) && (prevSad[pic_pix_x2] <= stopCriterion))))
        {
          mv[0] = tempmv[0];
          mv[1] = tempmv[1];
#if EPZSREF
          if (input->EPZSSpatialMem)
#else
          if (input->EPZSSpatialMem && ref == 0)
#endif
          {
            motion[0]  = tempmv[0];
            motion[1]  = tempmv[1];
          }

          return min_mcost;
        }

        //! Check Second best predictor with EPZS pattern
        conditionEPZS = (checkMedian == TRUE)
          && ((img->type == P_SLICE) || (blocktype < 5))
          && (min_mcost > stopCriterion) && (input->EPZSDual > 0);

        if (!conditionEPZS) break;

        pointNumber = 0;
        patternStop = 0;
        motionDirection = 0;
        nextLast = 0;

        if ((tempmv[0] == 0 && tempmv[1] == 0)
          || (tempmv[0] == mv[0] && tempmv[1] == mv[1]))
        {
          if (iabs (tempmv[0] - mv[0]) < (2<<(2-mv_rescale)) && iabs (tempmv[1] - mv[1]) < (2<<(2-mv_rescale)))
            searchPatternF = sdiamond;
          else
            searchPatternF = square;
        }
        else
          searchPatternF = searchPatternD;
        totalCheckPts = searchPatternF->searchPoints;

        //! Second best. Note that following code is identical as for best predictor.
        center_x = tempmv2[0];
        center_y = tempmv2[1];
        checkMedian = FALSE;
      }
    }
  }

  if ((ref==0) || (prevSad[pic_pix_x2] > min_mcost))
    prevSad[pic_pix_x2] = min_mcost;
#if EPZSREF
  if (input->EPZSSpatialMem)
#else
  if (input->EPZSSpatialMem && ref == 0)
#endif
  {
    motion[0]  = tempmv[0];
    motion[1]  = tempmv[1];
  }

  mv[0] = tempmv[0];
  mv[1] = tempmv[1];
  return min_mcost;
}


/*!
***********************************************************************
* \brief
*    FAST Motion Estimation using EPZS
*    AMT/HYC
***********************************************************************
*/
int                                               //  ==> minimum motion cost after search
EPZSBiPredBlockMotionSearch (imgpel * cur_pic,   // <--  original pixel values for the AxB block
                             short  ref,          // <--  reference picture
                             int    list,         // <--  reference list
                             int    list_offset,  // <--  offset for Mbaff
                             signed char ***refPic, // <--  reference array
                             short  ****tmp_mv,   // <--  mv array
                             int    pic_pix_x,    // <--  absolute x-coordinate of regarded AxB block
                             int    pic_pix_y,    // <--  absolute y-coordinate of regarded AxB block
                             int    blocktype,    //<--  block type (1-16x16 ... 7-4x4)
                             short  *pred_mv1,   // <--  motion vector predictor (x) in sub-pel units
                             short  *pred_mv2,   // <--  motion vector predictor (x) in sub-pel units
                             short  mv[2],        // <--> in: search center (x) / out: motion vector (x) - in pel units
                             short  s_mv[2],      // <--> in: search center (x) / out: motion vector (x) - in pel units
                             int    search_range, // <--  1-d search range in pel units
                             int    min_mcost,    // <--  minimum motion cost (cost for center or huge value)
                             int    lambda_factor)        // <--  lagrangian parameter for determining motion cost
{
  short blocksize_y = input->blc_size[blocktype][1];  // vertical block size
  short blocksize_x = input->blc_size[blocktype][0];  // horizontal block size
  short mb_x = pic_pix_x - img->opix_x;
  short mb_y = pic_pix_y - img->opix_y;
  int   pred_x1 = (pic_pix_x << 2) + pred_mv1[0]; // predicted position x (in sub-pel units)
  int   pred_y1 = (pic_pix_y << 2) + pred_mv1[1]; // predicted position y (in sub-pel units)
  int   pred_x2 = (pic_pix_x << 2) + pred_mv2[0]; // predicted position x (in sub-pel units)
  int   pred_y2 = (pic_pix_y << 2) + pred_mv2[1]; // predicted position y (in sub-pel units)
  int   center2_x = (pic_pix_x << (input->EPZSSubPelGrid * 2))+ mv[0];    // center position x (in pel units)
  int   center2_y = (pic_pix_y << (input->EPZSSubPelGrid * 2))+ mv[1];    // center position y (in pel units)
  int   center1_x = (pic_pix_x << (input->EPZSSubPelGrid * 2))+ s_mv[0];  // center position x (in pel units)
  int   center1_y = (pic_pix_y << (input->EPZSSubPelGrid * 2))+ s_mv[1];  // center position y (in pel units)

  int tempmv[2]  = {mv[0], mv[1]};
  int tempmv2[2] = {0, 0};
  int stopCriterion = medthres[blocktype];
  int mapCenter_x = search_range - mv[0];
  int mapCenter_y = search_range - mv[1];
  int second_mcost = INT_MAX;
  StorablePicture *ref_picture1 = listX[list       + list_offset][ref];
  StorablePicture *ref_picture2 = listX[(list ^ 1) + list_offset][0];

  short apply_weights = (active_pps->weighted_bipred_idc != 0);
  short offset1 = apply_weights ? list == 0
    ? wp_offset[list_offset         ][ref][0]
    : wp_offset[list_offset + LIST_1][0  ][0]
    : 0;
  short offset2 = apply_weights ? list == 0
    ? wp_offset[list_offset + LIST_1][ref][0]
    : wp_offset[list_offset         ][0  ][0]
    : 0;
  short invalid_refs = 0;
  byte checkMedian = FALSE;
  EPZSStructure *searchPatternF = searchPattern;
  EPZSBlkCount ++;

  pic_pix_x = (pic_pix_x << (2 - mv_rescale));
  pic_pix_y = (pic_pix_y << (2 - mv_rescale));

  ref_pic1_sub.luma = ref_picture1->imgY_sub;
  ref_pic2_sub.luma = ref_picture2->imgY_sub;
  img_width  = ref_picture1->size_x;
  img_height = ref_picture1->size_y;
  width_pad  = ref_picture1->size_x_pad;
  height_pad = ref_picture1->size_y_pad;

  if (apply_weights)
  {
    weight1 = list == 0 ? wbp_weight[list_offset         ][ref][0][0] : wbp_weight[list_offset + LIST_1][0  ][ref][0];
    weight2 = list == 0 ? wbp_weight[list_offset + LIST_1][ref][0][0] : wbp_weight[list_offset         ][0  ][ref][0];
    offsetBi=(offset1 + offset2 + 1)>>1;
    computeBiPred = computeBiPred2[F_PEL];
  }
  else
  {
    weight1 = 1<<luma_log_weight_denom;
    weight2 = 1<<luma_log_weight_denom;
    offsetBi = 0;
    computeBiPred = computeBiPred1[F_PEL];
  }

  if ( ChromaMEEnable ) {
    ref_pic1_sub.crcb[0] = ref_picture1->imgUV_sub[0];
    ref_pic1_sub.crcb[1] = ref_picture1->imgUV_sub[1];
    ref_pic2_sub.crcb[0] = ref_picture2->imgUV_sub[0];
    ref_pic2_sub.crcb[1] = ref_picture2->imgUV_sub[1];
    width_pad_cr  = ref_picture1->size_x_cr_pad;
    height_pad_cr = ref_picture1->size_y_cr_pad;
    if (apply_weights)
    {
      weight1_cr[0] = list == 0 ? wbp_weight[list_offset         ][ref][0][1] : wbp_weight[list_offset + LIST_1][0  ][ref][1];
      weight1_cr[1] = list == 0 ? wbp_weight[list_offset         ][ref][0][2] : wbp_weight[list_offset + LIST_1][0  ][ref][2];
      weight2_cr[0] = list == 0 ? wbp_weight[list_offset + LIST_1][ref][0][1] : wbp_weight[list_offset         ][0  ][ref][1];
      weight2_cr[1] = list == 0 ? wbp_weight[list_offset + LIST_1][ref][0][2] : wbp_weight[list_offset         ][0  ][ref][2];
      offsetBi_cr[0] = (list == 0)
        ? (wp_offset[list_offset         ][ref][1] + wp_offset[list_offset + LIST_1][ref][1] + 1) >> 1
        : (wp_offset[list_offset + LIST_1][0  ][1] + wp_offset[list_offset         ][0  ][1] + 1) >> 1;
      offsetBi_cr[1] = (list == 0)
        ? (wp_offset[list_offset         ][ref][2] + wp_offset[list_offset + LIST_1][ref][2] + 1) >> 1
        : (wp_offset[list_offset + LIST_1][0  ][2] + wp_offset[list_offset         ][0  ][2] + 1) >> 1;
    }
    else
    {
      weight1_cr[0] = 1<<chroma_log_weight_denom;
      weight1_cr[1] = 1<<chroma_log_weight_denom;
      weight2_cr[0] = 1<<chroma_log_weight_denom;
      weight2_cr[1] = 1<<chroma_log_weight_denom;
      offsetBi_cr[0] = 0;
      offsetBi_cr[1] = 0;
    }
  }


  //===== set function for getting reference picture lines from reference 1=====
  if ( (center2_x > search_range) && (center2_x < ((img_width  - blocksize_x) << (input->EPZSSubPelGrid * 2)) - search_range)
    && (center2_y > search_range) && (center2_y < ((img_height - blocksize_y) << (input->EPZSSubPelGrid * 2)) - search_range))
  {
    bipred2_access_method = FAST_ACCESS;
  }
  else
  {
   bipred2_access_method = UMV_ACCESS;
  }

  //===== set function for getting reference picture lines from reference 2=====
  if ( (center1_x > search_range) && (center1_x < ((img_width  - blocksize_x) << (input->EPZSSubPelGrid * 2)) - search_range)
    && (center1_y > search_range) && (center1_y < ((img_height - blocksize_y) << (input->EPZSSubPelGrid * 2)) - search_range))
  {
    bipred1_access_method = FAST_ACCESS;
  }
  else
  {
    bipred1_access_method = UMV_ACCESS;
  }

  // Clear EPZSMap
  //memset(EPZSMap[0],FALSE,searcharray*searcharray);
  // Check median candidate;
  EPZSMap[search_range][search_range] = EPZSBlkCount;

  //--- initialize motion cost (cost for motion vector) and check ---
  min_mcost  = MV_COST_SMP (lambda_factor, (center1_x<<mv_rescale), (center1_y<<mv_rescale), pred_x1, pred_y1);
  min_mcost += MV_COST_SMP (lambda_factor, (center2_x<<mv_rescale), (center2_y<<mv_rescale), pred_x2, pred_y2);

  //--- add residual cost to motion cost ---
  min_mcost += computeBiPred(cur_pic,
    blocksize_y, blocksize_x, INT_MAX,
    (center1_x << mv_rescale) + IMG_PAD_SIZE_TIMES4,
    (center1_y << mv_rescale) + IMG_PAD_SIZE_TIMES4,
    (center2_x << mv_rescale) + IMG_PAD_SIZE_TIMES4,
    (center2_y << mv_rescale) + IMG_PAD_SIZE_TIMES4);

  //! If medthres satisfied, then terminate, otherwise generate Predictors
  if (min_mcost > stopCriterion)
  {
    int prednum = 5;
    int patternStop = 0, pointNumber = 0, checkPts, nextLast = 0;
    int totalCheckPts = 0, motionDirection = 0;
    int conditionEPZS;
    int tmv[2], cand_x, cand_y;
    int pos, mcost;
    PixelPos block_a, block_b, block_c, block_d;

    getLuma4x4Neighbour (img->current_mb_nr, mb_x - 1, mb_y, &block_a);
    getLuma4x4Neighbour (img->current_mb_nr, mb_x, mb_y - 1, &block_b);
    getLuma4x4Neighbour (img->current_mb_nr, mb_x + blocksize_x, mb_y -1, &block_c);
    getLuma4x4Neighbour (img->current_mb_nr, mb_x - 1, mb_y -1, &block_d);

    if (mb_y > 0)
    {
      if (mb_x < 8)  // first column of 8x8 blocks
      {
        if (mb_y==8)
        {
          if (blocksize_x == MB_BLOCK_SIZE)
            block_c.available  = 0;
        }
        else if (mb_x+blocksize_x == 8)
          block_c.available = 0;
      }
      else if (mb_x+blocksize_x == MB_BLOCK_SIZE)
        block_c.available = 0;
    }

    stopCriterion = (11 * medthres[blocktype]) >> 3;

    //! Add Spatial Predictors in predictor list.
    //! Scheme adds zero, left, top-left, top, top-right. Note that top-left adds very little
    //! in terms of performance and could be removed with little penalty if any.
    invalid_refs = EPZSSpatialPredictors (block_a, block_b, block_c, block_d,
      list, list_offset, ref, refPic[list], tmp_mv[list], predictor);

    //! Check all predictors
    for (pos = 0; pos < prednum; pos++)
    {
      tmv[0] = predictor->point[pos].mv[0];
      tmv[1] = predictor->point[pos].mv[1];
      if ((iabs (tmv[0] - mv[0]) > search_range || iabs (tmv[1] - mv[1]) > search_range) && (tmv[0] || tmv[1]))
        continue;

      if ((iabs (tmv[0] - mv[0]) <= search_range) && (iabs (tmv[1] - mv[1]) <= search_range))
      {
        if (EPZSMap[mapCenter_y + tmv[1]][mapCenter_x + tmv[0]] == EPZSBlkCount)
          continue;
        else
          EPZSMap[mapCenter_y + tmv[1]][mapCenter_x + tmv[0]] = EPZSBlkCount;
      }

      cand_x = (pic_pix_x + tmv[0])<<mv_rescale;
      cand_y = (pic_pix_y + tmv[1])<<mv_rescale;

      //--- set motion cost (cost for motion vector) and check ---
      mcost  = MV_COST_SMP (lambda_factor, (center1_x<<mv_rescale), (center1_y<<mv_rescale), pred_x1, pred_y1);
      mcost += MV_COST_SMP (lambda_factor, cand_x, cand_y, pred_x2, pred_y2);

      if (mcost >= second_mcost) continue;

      mcost += computeBiPred(cur_pic,
        blocksize_y, blocksize_x, second_mcost - mcost,
        (center1_x << mv_rescale) + IMG_PAD_SIZE_TIMES4,
        (center1_y << mv_rescale) + IMG_PAD_SIZE_TIMES4,
        cand_x + IMG_PAD_SIZE_TIMES4, cand_y + IMG_PAD_SIZE_TIMES4);

      //--- check if motion cost is less than minimum cost ---
      if (mcost < min_mcost)
      {
        tempmv2[0] = tempmv[0];
        tempmv2[1] = tempmv[1];
        second_mcost = min_mcost;
        tempmv[0] = tmv[0];
        tempmv[1] = tmv[1];
        min_mcost = mcost;
        checkMedian = TRUE;
      }
      //else if (mcost < second_mcost && (tempmv[0] != tmv[0] || tempmv[1] != tmv[1]))
      else if (mcost < second_mcost)
      {
        tempmv2[0] = tmv[0];
        tempmv2[1] = tmv[1];
        second_mcost = mcost;
        checkMedian = TRUE;
      }
    }

    //! Refine using EPZS pattern if needed.
    //! Note that we are using a simplistic threshold computation.
    if (min_mcost > stopCriterion)
    {
      //! Adapt pattern based on different conditions.
      if (input->EPZSPattern != 0)
      {
        if ((min_mcost < stopCriterion + ((3 * medthres[blocktype]) >> 1)))
        {
          if ((tempmv[0] == 0 && tempmv[1] == 0)
            || (iabs (tempmv[0] - mv[0]) < (2<<(2-mv_rescale)) && iabs (tempmv[1] - mv[1]) < (2<<(2-mv_rescale))))
            searchPatternF = sdiamond;
          else
            searchPatternF = square;
        }
        else if (blocktype > 5 || (ref > 0 && blocktype != 1))
          searchPatternF = square;
        else
          searchPatternF = searchPattern;
      }

      totalCheckPts = searchPatternF->searchPoints;

      //! center on best predictor
      center2_x = tempmv[0];
      center2_y = tempmv[1];
      while (1)
      {
        do
        {
          checkPts = totalCheckPts;
          do
          {
            tmv[0] = center2_x + searchPatternF->point[pointNumber].mv[0];
            tmv[1] = center2_y + searchPatternF->point[pointNumber].mv[1];
            cand_x = (pic_pix_x + tmv[0])<<mv_rescale;
            cand_y = (pic_pix_y + tmv[1])<<mv_rescale;


            if ((iabs (tmv[0] - mv[0]) <= search_range)
              && (iabs (tmv[1] - mv[1]) <= search_range))
            {
              if (EPZSMap[mapCenter_y + tmv[1]][mapCenter_x + tmv[0]] != EPZSBlkCount)
                EPZSMap[mapCenter_y + tmv[1]][mapCenter_x + tmv[0]] = EPZSBlkCount;
              else
              {
                pointNumber += 1;
                if (pointNumber >= searchPatternF->searchPoints)
                  pointNumber -= searchPatternF->searchPoints;
                checkPts -= 1;
                continue;
              }

              mcost  = MV_COST_SMP (lambda_factor, (center1_x << mv_rescale), (center1_y << mv_rescale), pred_x1, pred_y1);
              mcost += MV_COST_SMP (lambda_factor, cand_x, cand_y, pred_x2, pred_y2);

              if (mcost < min_mcost)
              {
                mcost += computeBiPred(cur_pic,
                  blocksize_y, blocksize_x, min_mcost - mcost,
                  (center1_x << mv_rescale) + IMG_PAD_SIZE_TIMES4,
                  (center1_y << mv_rescale) + IMG_PAD_SIZE_TIMES4,
                  cand_x + IMG_PAD_SIZE_TIMES4, cand_y + IMG_PAD_SIZE_TIMES4);

                if (mcost < min_mcost)
                {
                  min_mcost = mcost;
                  tempmv[0] = tmv[0];
                  tempmv[1] = tmv[1];
                  motionDirection = pointNumber;
                }
              }
            }
            pointNumber += 1;
            if (pointNumber >= searchPatternF->searchPoints)
              pointNumber -= searchPatternF->searchPoints;
            checkPts -= 1;
          }
          while (checkPts > 0);

          if (nextLast || ((tempmv[0] == center2_x) && (tempmv[1] == center2_y)))
          {
            patternStop     = searchPatternF->stopSearch;
            searchPatternF  = searchPatternF->nextpattern;
            totalCheckPts   = searchPatternF->searchPoints;
            nextLast        = searchPatternF->nextLast;
            motionDirection = 0;
            pointNumber = 0;
          }
          else
          {
            totalCheckPts = searchPatternF->point[motionDirection].next_points;
            pointNumber = searchPatternF->point[motionDirection].start_nmbr;
            center2_x = tempmv[0];
            center2_y = tempmv[1];
          }
        }
        while (patternStop != 1);

        //! Check Second best predictor with EPZS pattern

        conditionEPZS = (checkMedian == TRUE) && (blocktype < 5) && (min_mcost > stopCriterion) && (input->EPZSDual > 0);

        if (!conditionEPZS) break;

        pointNumber = 0;
        patternStop = 0;
        motionDirection = 0;
        nextLast = 0;

        if ((tempmv[0] == 0 && tempmv[1] == 0)
          || (tempmv[0] == mv[0] && tempmv[1] == mv[1]))
        {
          if (iabs (tempmv[0] - mv[0]) < (2<<(2-mv_rescale)) && iabs (tempmv[1] - mv[1]) < (2<<(2-mv_rescale)))
            searchPatternF = sdiamond;
          else
            searchPatternF = square;
        }
        else
          searchPatternF = searchPatternD;
        totalCheckPts = searchPatternF->searchPoints;

        //! Second best. Note that following code is identical as for best predictor.
        center2_x = tempmv2[0];
        center2_y = tempmv2[1];

        checkMedian = FALSE;
      }
    }
  }
  mv[0] = tempmv[0];
  mv[1] = tempmv[1];
  return min_mcost;
}

/*!
***********************************************************************
* \brief
*    Report function for EPZS Fast ME
*    AMT/HYC
***********************************************************************
*/
void
EPZSOutputStats (FILE * stat, short stats_file)
{
  if (stats_file == 1)
  {
    fprintf (stat, " EPZS Pattern                 : %s\n",c_EPZSPattern[input->EPZSPattern]);
    fprintf (stat, " EPZS Dual Pattern            : %s\n",c_EPZSDualPattern[input->EPZSDual]);
    fprintf (stat, " EPZS Fixed Predictors        : %s\n",c_EPZSFixed[input->EPZSFixed]);
    fprintf (stat, " EPZS Temporal Predictors     : %s\n",c_EPZSOther[input->EPZSTemporal]);
    fprintf (stat, " EPZS Spatial Predictors      : %s\n",c_EPZSOther[input->EPZSSpatialMem]);
    fprintf (stat, " EPZS Thresholds (16x16)      : (%d %d %d)\n",medthres[1], minthres[1], maxthres[1]);
    fprintf (stat, " EPZS Subpel ME               : %s\n",c_EPZSOther[input->EPZSSubPelME]);
    fprintf (stat, " EPZS Subpel ME BiPred        : %s\n",c_EPZSOther[input->EPZSSubPelMEBiPred]);
  }
  else
  {
    fprintf (stat, " EPZS Pattern                      : %s\n",c_EPZSPattern[input->EPZSPattern]);
    fprintf (stat, " EPZS Dual Pattern                 : %s\n",c_EPZSDualPattern[input->EPZSDual]);
    fprintf (stat, " EPZS Fixed Predictors             : %s\n",c_EPZSFixed[input->EPZSFixed]);
    fprintf (stat, " EPZS Temporal Predictors          : %s\n",c_EPZSOther[input->EPZSTemporal]);
    fprintf (stat, " EPZS Spatial Predictors           : %s\n",c_EPZSOther[input->EPZSSpatialMem]);
    fprintf (stat, " EPZS Thresholds (16x16)           : (%d %d %d)\n",medthres[1], minthres[1], maxthres[1]);
    fprintf (stat, " EPZS Subpel ME                    : %s\n",c_EPZSOther[input->EPZSSubPelME]);
    fprintf (stat, " EPZS Subpel ME BiPred             : %s\n",c_EPZSOther[input->EPZSSubPelMEBiPred]);
  }
}


/*!
 ***********************************************************************
 * \brief
 *    Fast sub pixel block motion search to support EPZS
 ***********************************************************************
 */
int                                               //  ==> minimum motion cost after search
EPZSSubPelBlockMotionSearch (imgpel*   orig_pic,      // <--  original pixel values for the AxB block
                             short     ref,           // <--  reference frame (0... or -1 (backward))
                             int       list,          // <--  reference picture list
                             int       pic_pix_x,     // <--  absolute x-coordinate of regarded AxB block
                             int       pic_pix_y,     // <--  absolute y-coordinate of regarded AxB block
                             int       blocktype,     // <--  block type (1-16x16 ... 7-4x4)
                             short     pred_mv[2],    // <--  motion vector predictor (x) in sub-pel units
                             short     mv[2],         // <--> in: search center (x) / out: motion vector (x) - in pel units
                             int       search_pos2,   // <--  search positions for    half-pel search  (default: 9)
                             int       search_pos4,   // <--  search positions for quarter-pel search  (default: 9)
                             int       min_mcost,     // <--  minimum motion cost (cost for center or huge value)
                             int*      lambda         // <--  lagrangian parameter for determining motion cost
                             )
{

  int   pos, best_pos = 0, second_pos = 0, mcost;
  int   second_mcost = INT_MAX;

  int   cand_mv_x, cand_mv_y;

  int   blocksize_x     = input->blc_size[blocktype][0];
  int   blocksize_y     = input->blc_size[blocktype][1];
  int   pic4_pix_x      = ((pic_pix_x + IMG_PAD_SIZE)<< 2);
  int   pic4_pix_y      = ((pic_pix_y + IMG_PAD_SIZE)<< 2);

  int   max_pos2        = ( (!start_me_refinement_hp || !start_me_refinement_qp) ? imax(1,search_pos2) : search_pos2);
  int   list_offset     = img->mb_data[img->current_mb_nr].list_offset;
  int   apply_weights   = ((active_pps->weighted_pred_flag && (img->type == P_SLICE || img->type == SP_SLICE)) ||
    (active_pps->weighted_bipred_idc && (img->type == B_SLICE))) && input->UseWeightedReferenceME;

  StorablePicture *ref_picture = listX[list+list_offset][ref];

  int max_pos_x4 = ((ref_picture->size_x - blocksize_x + 2*IMG_PAD_SIZE)<<2);
  int max_pos_y4 = ((ref_picture->size_y - blocksize_y + 2*IMG_PAD_SIZE)<<2);
  int start_pos = 5, end_pos = max_pos2;
  int dist_method = H_PEL + 3 * apply_weights;
  int lambda_factor = lambda[H_PEL];

  ref_pic_sub.luma = ref_picture->imgY_sub;
  width_pad  = ref_picture->size_x_pad;
  height_pad = ref_picture->size_y_pad;

  if (apply_weights)
  {
    weight_luma = wp_weight[list + list_offset][ref][0];
    offset_luma = wp_offset[list + list_offset][ref][0];
  }

  if ( ChromaMEEnable )
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

  /*********************************
   *****                       *****
   *****  HALF-PEL REFINEMENT  *****
   *****                       *****
   *********************************/

  //===== set function for getting pixel values =====
  if ((pic4_pix_x + mv[0] > 1) && (pic4_pix_x + mv[0] < max_pos_x4 - 1) &&
    (pic4_pix_y + mv[1] > 1) && (pic4_pix_y + mv[1] < max_pos_y4 - 1))
  {
    ref_access_method = FAST_ACCESS;
  }
  else
  {
    ref_access_method = UMV_ACCESS;
  }

  //===== loop over search positions =====
  for (best_pos = 0, pos = start_me_refinement_hp; pos < 5; pos++)
  {
    cand_mv_x = mv[0] + search_point_hp[pos][0];    // quarter-pel units
    cand_mv_y = mv[1] + search_point_hp[pos][1];    // quarter-pel units

    //----- set motion vector cost -----
    mcost = MV_COST_SMP (lambda_factor, cand_mv_x, cand_mv_y, pred_mv[0], pred_mv[1]);
    mcost += computeUniPred[dist_method]( orig_pic, blocksize_y, blocksize_x,
      INT_MAX, cand_mv_x + pic4_pix_x, cand_mv_y + pic4_pix_y);

    if (mcost < min_mcost)
    {
      second_mcost = min_mcost;
      second_pos  = best_pos;
      min_mcost = mcost;
      best_pos  = pos;
    }
    else if (mcost < second_mcost)
    {
      second_mcost = mcost;
      second_pos  = pos;
    }
  }

  if (best_pos ==0 && (pred_mv[0] == mv[0]) && (pred_mv[1] - mv[1])== 0 && min_mcost < subthres[blocktype])
      return min_mcost;

  if (best_pos != 0 && second_pos != 0)
  {
    switch (best_pos ^ second_pos)
    {
    case 1:
      start_pos = 6;
      end_pos   = 7;
      break;
    case 3:
      start_pos = 5;
      end_pos   = 6;
      break;
    case 5:
      start_pos = 8;
      end_pos   = 9;
      break;
    case 7:
      start_pos = 7;
      end_pos   = 8;
      break;
    default:
      break;
    }
  }
  else
  {
    switch (best_pos + second_pos)
    {
    case 0:
      start_pos = 5;
      end_pos   = 5;
      break;
    case 1:
      start_pos = 8;
      end_pos   = 10;
      break;
    case 2:
      start_pos = 5;
      end_pos   = 7;
      break;
    case 5:
      start_pos = 6;
      end_pos   = 8;
      break;
    case 7:
      start_pos = 7;
      end_pos   = 9;
      break;
    default:
      break;
    }
  }

  if (best_pos !=0 || (iabs(pred_mv[0] - mv[0]) + iabs(pred_mv[1] - mv[1])))
  {
    for (pos = start_pos; pos < end_pos; pos++)
    {
      cand_mv_x = mv[0] + search_point_hp[pos][0];    // quarter-pel units
      cand_mv_y = mv[1] + search_point_hp[pos][1];    // quarter-pel units

      //----- set motion vector cost -----
      mcost = MV_COST_SMP (lambda_factor, cand_mv_x, cand_mv_y, pred_mv[0], pred_mv[1]);

      if (mcost >= min_mcost) continue;

      mcost += computeUniPred[dist_method]( orig_pic, blocksize_y, blocksize_x,
        min_mcost - mcost, cand_mv_x + pic4_pix_x, cand_mv_y + pic4_pix_y);

      if (mcost < min_mcost)
      {
        min_mcost = mcost;
        best_pos  = pos;
      }
    }
  }

  if (best_pos)
  {
    mv[0] += search_point_hp[best_pos][0];
    mv[1] += search_point_hp[best_pos][1];
  }

  if ( !start_me_refinement_qp )
    min_mcost = INT_MAX;

  /************************************
  *****                          *****
  *****  QUARTER-PEL REFINEMENT  *****
  *****                          *****
  ************************************/
  //===== set function for getting pixel values =====
  if ((pic4_pix_x + mv[0] > 0) && (pic4_pix_x + mv[0] < max_pos_x4) &&
    (pic4_pix_y + mv[1] > 0) && (pic4_pix_y + mv[1] < max_pos_y4)   )
  {
    ref_access_method = FAST_ACCESS;
  }
  else
  {
    ref_access_method = UMV_ACCESS;
  }

  dist_method = Q_PEL + 3 * apply_weights;
  lambda_factor = lambda[Q_PEL];
  second_pos = 0;
  second_mcost = INT_MAX;
  //===== loop over search positions =====
  for (best_pos = 0, pos = start_me_refinement_qp; pos < 5; pos++)
  {
    cand_mv_x = mv[0] + search_point_qp[pos][0];    // quarter-pel units
    cand_mv_y = mv[1] + search_point_qp[pos][1];    // quarter-pel units

    //----- set motion vector cost -----
    mcost = MV_COST_SMP (lambda_factor, cand_mv_x, cand_mv_y, pred_mv[0], pred_mv[1]);
    mcost += computeUniPred[dist_method]( orig_pic, blocksize_y, blocksize_x,
      INT_MAX, cand_mv_x + pic4_pix_x, cand_mv_y + pic4_pix_y);

    if (mcost < min_mcost)
    {
      second_mcost = min_mcost;
      second_pos  = best_pos;
      min_mcost = mcost;
      best_pos  = pos;
    }
    else if (mcost < second_mcost)
    {
      second_mcost = mcost;
      second_pos  = pos;
    }
  }

  if (best_pos ==0 && (pred_mv[0] == mv[0]) && (pred_mv[1] - mv[1])== 0 && min_mcost < subthres[blocktype])
  {
    return min_mcost;
  }

  start_pos = 5;
  end_pos = search_pos4;

  if (best_pos != 0 && second_pos != 0)
  {
    switch (best_pos ^ second_pos)
    {
    case 1:
      start_pos = 6;
      end_pos   = 7;
      break;
    case 3:
      start_pos = 5;
      end_pos   = 6;
      break;
    case 5:
      start_pos = 8;
      end_pos   = 9;
      break;
    case 7:
      start_pos = 7;
      end_pos   = 8;
      break;
    default:
      break;
    }
  }
  else
  {
    switch (best_pos + second_pos)
    {
      //case 0:
      //start_pos = 5;
      //end_pos   = 5;
      //break;
    case 1:
      start_pos = 8;
      end_pos   = 10;
      break;
    case 2:
      start_pos = 5;
      end_pos   = 7;
      break;
    case 5:
      start_pos = 6;
      end_pos   = 8;
      break;
    case 7:
      start_pos = 7;
      end_pos   = 9;
      break;
    default:
      break;
    }
  }

  if (best_pos !=0 || (iabs(pred_mv[0] - mv[0]) + iabs(pred_mv[1] - mv[1])))
  {
    for (pos = start_pos; pos < end_pos; pos++)
    {
      cand_mv_x = mv[0] + search_point_qp[pos][0];    // quarter-pel units
      cand_mv_y = mv[1] + search_point_qp[pos][1];    // quarter-pel units

      //----- set motion vector cost -----
      mcost = MV_COST_SMP (lambda_factor, cand_mv_x, cand_mv_y, pred_mv[0], pred_mv[1]);

      if (mcost >= min_mcost) continue;
      mcost += computeUniPred[dist_method]( orig_pic, blocksize_y, blocksize_x,
        min_mcost - mcost, cand_mv_x + pic4_pix_x, cand_mv_y + pic4_pix_y);

      if (mcost < min_mcost)
      {
        min_mcost = mcost;
        best_pos  = pos;
      }
    }
  }
  if (best_pos)
  {
    mv[0] += search_point_qp [best_pos][0];
    mv[1] += search_point_qp [best_pos][1];
  }

  //===== return minimum motion cost =====
  return min_mcost;
}

/*!
 ***********************************************************************
 * \brief
 *    Fast bipred sub pixel block motion search to support EPZS
 ***********************************************************************
 */
int                                               //  ==> minimum motion cost after search
EPZSSubPelBlockSearchBiPred (imgpel*   orig_pic,  // <--  original pixel values for the AxB block
                         short     ref,           // <--  reference frame (0... or -1 (backward))
                         int       list,          // <--  reference picture list
                         int       pic_pix_x,     // <--  absolute x-coordinate of regarded AxB block
                         int       pic_pix_y,     // <--  absolute y-coordinate of regarded AxB block
                         int       blocktype,     // <--  block type (1-16x16 ... 7-4x4)
                         short     *pred_mv1,     // <--  motion vector predictor (x) in sub-pel units
                         short     *pred_mv2,     // <--  motion vector predictor (x) in sub-pel units
                         short     mv[2],         // <--> in: search center (x) / out: motion vector (x) - in pel units
                         short     s_mv[2],       // <--> in: search center (x) / out: motion vector (x) - in pel units
                         int       search_pos2,   // <--  search positions for    half-pel search  (default: 9)
                         int       search_pos4,   // <--  search positions for quarter-pel search  (default: 9)
                         int       min_mcost,     // <--  minimum motion cost (cost for center or huge value)
                         int       *lambda        // <--  lagrangian parameter for determining motion cost
                         )
{
  int   apply_weights =  (active_pps->weighted_bipred_idc );
  int   list_offset   = img->mb_data[img->current_mb_nr].list_offset;

  short offset1 = (apply_weights ? (list == 0?  wp_offset[list_offset    ][ref]     [0]:  wp_offset[list_offset + 1][0  ]     [0]) : 0);
  short offset2 = (apply_weights ? (list == 0?  wp_offset[list_offset + 1][ref]     [0]:  wp_offset[list_offset    ][0  ]     [0]) : 0);

  int   pos, best_pos = 0, second_pos = 0, mcost;
  int   second_mcost = INT_MAX;

  int   cand_mv_x, cand_mv_y;

  int   blocksize_x     = input->blc_size[blocktype][0];
  int   blocksize_y     = input->blc_size[blocktype][1];

  int   pic4_pix_x      = ((pic_pix_x + IMG_PAD_SIZE)<< 2);
  int   pic4_pix_y      = ((pic_pix_y + IMG_PAD_SIZE)<< 2);

  int   start_hp        = (min_mcost == INT_MAX) ? 0 : start_me_refinement_hp;
  int   max_pos2        = ( (!start_me_refinement_hp || !start_me_refinement_qp) ? imax(1,search_pos2) : search_pos2);

  int   smv_x = s_mv[0] + pic4_pix_x;
  int   smv_y = s_mv[1] + pic4_pix_y;

  StorablePicture *ref_picture1 = listX[list       + list_offset][ref];
  StorablePicture *ref_picture2 = listX[(list ^ 1) + list_offset][0];


  int max_pos_x4 = ((ref_picture1->size_x - blocksize_x + 2*IMG_PAD_SIZE)<<2);
  int max_pos_y4 = ((ref_picture1->size_y - blocksize_y + 2*IMG_PAD_SIZE)<<2);
  int start_pos = 5, end_pos = max_pos2;
  int lambda_factor = lambda[H_PEL];

  ref_pic1_sub.luma = ref_picture1->imgY_sub;
  ref_pic2_sub.luma = ref_picture2->imgY_sub;
  img_width     = ref_picture1->size_x;
  img_height    = ref_picture1->size_y;
  width_pad    = ref_picture1->size_x_pad;
  height_pad   = ref_picture1->size_y_pad;

  if (apply_weights)
  {
    weight1 = list == 0
      ? wbp_weight[list_offset         ][ref][0][0]
      : wbp_weight[list_offset + LIST_1][0  ][ref][0];
    weight2 = list == 0
      ? wbp_weight[list_offset + LIST_1][ref][0][0]
      : wbp_weight[list_offset         ][0  ][ref][0];
    offsetBi=(offset1 + offset2 + 1)>>1;
    computeBiPred = computeBiPred2[H_PEL];
  }
  else
  {
    weight1 = 1<<luma_log_weight_denom;
    weight2 = 1<<luma_log_weight_denom;
    offsetBi = 0;
    computeBiPred = computeBiPred1[H_PEL];
  }


  if ( ChromaMEEnable )
  {
    ref_pic1_sub.crcb[0] = ref_picture1->imgUV_sub[0];
    ref_pic1_sub.crcb[1] = ref_picture1->imgUV_sub[1];
    ref_pic2_sub.crcb[0] = ref_picture2->imgUV_sub[0];
    ref_pic2_sub.crcb[1] = ref_picture2->imgUV_sub[1];
    width_pad_cr  = ref_picture1->size_x_cr_pad;
    height_pad_cr = ref_picture1->size_y_cr_pad;

    if (apply_weights)
    {
      weight1_cr[0] = list == 0
        ? wbp_weight[list_offset         ][ref][0][1]
        : wbp_weight[list_offset + LIST_1][0  ][ref][1];
      weight1_cr[1] = list == 0
        ? wbp_weight[list_offset         ][ref][0][2]
        : wbp_weight[list_offset + LIST_1][0  ][ref][2];
      weight2_cr[0] = list == 0 ? wbp_weight[list_offset + LIST_1][ref][0][1] : wbp_weight[list_offset         ][0  ][ref][1];
      weight2_cr[1] = list == 0 ? wbp_weight[list_offset + LIST_1][ref][0][2] : wbp_weight[list_offset         ][0  ][ref][2];
      offsetBi_cr[0] = (list == 0)
        ? (wp_offset[list_offset         ][ref][1] + wp_offset[list_offset + LIST_1][ref][1] + 1) >> 1
        : (wp_offset[list_offset + LIST_1][0  ][1] + wp_offset[list_offset         ][0  ][1] + 1) >> 1;
      offsetBi_cr[1] = (list == 0)
        ? (wp_offset[list_offset         ][ref][2] + wp_offset[list_offset + LIST_1][ref][2] + 1) >> 1
        : (wp_offset[list_offset + LIST_1][0  ][2] + wp_offset[list_offset         ][0  ][2] + 1) >> 1;
    }
    else
    {
      weight1_cr[0] = 1<<chroma_log_weight_denom;
      weight1_cr[1] = 1<<chroma_log_weight_denom;
      weight2_cr[0] = 1<<chroma_log_weight_denom;
      weight2_cr[1] = 1<<chroma_log_weight_denom;
      offsetBi_cr[0] = 0;
      offsetBi_cr[1] = 0;
    }
  }


  /*********************************
   *****                       *****
   *****  HALF-PEL REFINEMENT  *****
   *****                       *****
   *********************************/

  //===== set function for getting pixel values =====
  if ((pic4_pix_x + mv[0] > 1) && (pic4_pix_x + mv[0] < max_pos_x4 - 1) &&
    (pic4_pix_y + mv[1] > 1) && (pic4_pix_y + mv[1] < max_pos_y4 - 1))
  {
    bipred2_access_method = FAST_ACCESS;
  }
  else
  {
    bipred2_access_method = UMV_ACCESS;
  }

  if ((pic4_pix_x + s_mv[0] > 1) && (pic4_pix_x + s_mv[0] < max_pos_x4 - 1) &&
    (pic4_pix_y + s_mv[1] > 1) && (pic4_pix_y + s_mv[1] < max_pos_y4 - 1))
  {
    bipred1_access_method = FAST_ACCESS;
  }
  else
  {
    bipred1_access_method = UMV_ACCESS;
  }

  //===== loop over search positions =====
  for (best_pos = 0, pos = start_hp; pos < 5; pos++)
  {
    cand_mv_x = mv[0] + search_point_hp[pos][0];    // quarter-pel units
    cand_mv_y = mv[1] + search_point_hp[pos][1];    // quarter-pel units

    //----- set motion vector cost -----
    mcost  = MV_COST_SMP (lambda_factor, cand_mv_x, cand_mv_y, pred_mv1[0], pred_mv1[1]);
    mcost += MV_COST_SMP (lambda_factor, s_mv[0], s_mv[1], pred_mv2[0], pred_mv2[1]);
    mcost += computeBiPred(orig_pic, blocksize_y, blocksize_x, INT_MAX,
      smv_x, smv_y, cand_mv_x + pic4_pix_x, cand_mv_y + pic4_pix_y);

    if (mcost < min_mcost)
    {
      second_mcost = min_mcost;
      second_pos  = best_pos;
      min_mcost = mcost;
      best_pos  = pos;
    }
    else if (mcost < second_mcost)
    {
      second_mcost = mcost;
      second_pos  = pos;
    }
  }

//  if (best_pos ==0 && (pred_mv1[0] == mv[0]) && (pred_mv1[1] - mv[1])== 0 && min_mcost < subthres[blocktype])
      //return min_mcost;

  if (best_pos != 0 && second_pos != 0)
  {
    switch (best_pos ^ second_pos)
    {
    case 1:
      start_pos = 6;
      end_pos   = 7;
      break;
    case 3:
      start_pos = 5;
      end_pos   = 6;
      break;
    case 5:
      start_pos = 8;
      end_pos   = 9;
      break;
    case 7:
      start_pos = 7;
      end_pos   = 8;
      break;
    default:
      break;
    }
  }
  else
  {
    switch (best_pos + second_pos)
    {
    case 0:
      start_pos = 5;
      end_pos   = 5;
      break;
    case 1:
      start_pos = 8;
      end_pos   = 10;
      break;
    case 2:
      start_pos = 5;
      end_pos   = 7;
      break;
    case 5:
      start_pos = 6;
      end_pos   = 8;
      break;
    case 7:
      start_pos = 7;
      end_pos   = 9;
      break;
    default:
      break;
    }
  }

  if (best_pos !=0 || (iabs(pred_mv1[0] - mv[0]) + iabs(pred_mv1[1] - mv[1])))
  {
    for (pos = start_pos; pos < end_pos; pos++)
    {
      cand_mv_x = mv[0] + search_point_hp[pos][0];    // quarter-pel units
      cand_mv_y = mv[1] + search_point_hp[pos][1];    // quarter-pel units

      //----- set motion vector cost -----
      mcost  = MV_COST_SMP (lambda_factor, cand_mv_x, cand_mv_y, pred_mv1[0], pred_mv1[1]);
      mcost += MV_COST_SMP (lambda_factor, s_mv[0], s_mv[1], pred_mv2[0], pred_mv2[1]);
      if (mcost >= min_mcost) continue;

      mcost += computeBiPred(orig_pic, blocksize_y, blocksize_x, min_mcost - mcost,
        smv_x, smv_y, cand_mv_x + pic4_pix_x, cand_mv_y + pic4_pix_y);

      if (mcost < min_mcost)
      {
        min_mcost = mcost;
        best_pos  = pos;
      }
    }
  }

  if (best_pos)
  {
    mv[0] += search_point_hp [best_pos][0];
    mv[1] += search_point_hp [best_pos][1];
  }

  computeBiPred = apply_weights? computeBiPred2[Q_PEL] : computeBiPred1[Q_PEL];

  /************************************
  *****                          *****
  *****  QUARTER-PEL REFINEMENT  *****
  *****                          *****
  ************************************/
  //===== set function for getting pixel values =====
  if ((pic4_pix_x + mv[0] > 0) && (pic4_pix_x + mv[0] < max_pos_x4) &&
    (pic4_pix_y + mv[1] > 0) && (pic4_pix_y + mv[1] < max_pos_y4))
  {
    bipred2_access_method = FAST_ACCESS;
  }
  else
  {
    bipred2_access_method = UMV_ACCESS;
  }

  if ((pic4_pix_x + s_mv[0] > 0) && (pic4_pix_x + s_mv[0] < max_pos_x4) &&
    (pic4_pix_y + s_mv[1] > 0) && (pic4_pix_y + s_mv[1] < max_pos_y4))
  {
    bipred1_access_method = FAST_ACCESS;
  }
  else
  {
    bipred1_access_method = UMV_ACCESS;
  }

  if ( !start_me_refinement_qp )
    min_mcost = INT_MAX;

  lambda_factor = lambda[Q_PEL];
  second_pos = 0;
  second_mcost = INT_MAX;
  //===== loop over search positions =====
  for (best_pos = 0, pos = start_me_refinement_qp; pos < 5; pos++)
  {
    cand_mv_x = mv[0] + search_point_qp[pos][0];    // quarter-pel units
    cand_mv_y = mv[1] + search_point_qp[pos][1];    // quarter-pel units

    //----- set motion vector cost -----
    mcost  = MV_COST_SMP (lambda_factor, cand_mv_x, cand_mv_y, pred_mv1[0], pred_mv1[1]);
    mcost += MV_COST_SMP (lambda_factor, s_mv[0], s_mv[1], pred_mv2[0], pred_mv2[1]);

    mcost += computeBiPred(orig_pic, blocksize_y, blocksize_x, INT_MAX,
      smv_x, smv_y, cand_mv_x + pic4_pix_x, cand_mv_y + pic4_pix_y);


    if (mcost < min_mcost)
    {
      second_mcost = min_mcost;
      second_pos  = best_pos;
      min_mcost = mcost;
      best_pos  = pos;
    }
    else if (mcost < second_mcost)
    {
      second_mcost = mcost;
      second_pos  = pos;
    }
  }

  start_pos = 5;
  end_pos = search_pos4;

  if (best_pos != 0 && second_pos != 0)
  {
    switch (best_pos ^ second_pos)
    {
    case 1:
      start_pos = 6;
      end_pos   = 7;
      break;
    case 3:
      start_pos = 5;
      end_pos   = 6;
      break;
    case 5:
      start_pos = 8;
      end_pos   = 9;
      break;
    case 7:
      start_pos = 7;
      end_pos   = 8;
      break;
    default:
      break;
    }
  }
  else
  {
    switch (best_pos + second_pos)
    {
      //case 0:
      //start_pos = 5;
      //end_pos   = 5;
      //break;
    case 1:
      start_pos = 8;
      end_pos   = 10;
      break;
    case 2:
      start_pos = 5;
      end_pos   = 7;
      break;
    case 5:
      start_pos = 6;
      end_pos   = 8;
      break;
    case 7:
      start_pos = 7;
      end_pos   = 9;
      break;
    default:
      break;
    }
  }

  if (best_pos !=0 || (iabs(pred_mv1[0] - mv[0]) + iabs(pred_mv1[1] - mv[1])))
  {
    for (pos = start_pos; pos < end_pos; pos++)
    {
      cand_mv_x = mv[0] + search_point_qp[pos][0];    // quarter-pel units
      cand_mv_y = mv[1] + search_point_qp[pos][1];    // quarter-pel units

      //----- set motion vector cost -----
      mcost  = MV_COST_SMP (lambda_factor, cand_mv_x, cand_mv_y, pred_mv1[0], pred_mv1[1]);
      mcost += MV_COST_SMP (lambda_factor, s_mv[0], s_mv[1], pred_mv2[0], pred_mv2[1]);
      if (mcost >= min_mcost) continue;

      mcost += computeBiPred(orig_pic, blocksize_y, blocksize_x, min_mcost - mcost,
        smv_x, smv_y, cand_mv_x + pic4_pix_x, cand_mv_y + pic4_pix_y);


      if (mcost < min_mcost)
      {
        min_mcost = mcost;
        best_pos  = pos;
      }
    }
  }
  if (best_pos)
  {
    mv[0] += search_point_qp[best_pos][0];
    mv[1] += search_point_qp[best_pos][1];
  }

  //===== return minimum motion cost =====
  return min_mcost;
}

