
/*!
 *************************************************************************************
 * \file
 *      erc_do_p.c
 *
 * \brief
 *      Inter (P) frame error concealment algorithms for decoder
 *
 *  \author
 *      - Viktor Varsa                     <viktor.varsa@nokia.com>
 *      - Ye-Kui Wang                      <wyk@ieee.org>
 *      - Jill Boyce                       <jill.boyce@thomson.net>
 *      - Saurav K Bandyopadhyay           <saurav@ieee.org>
 *      - Zhenyu Wu                        <Zhenyu.Wu@thomson.net
 *      - Purvin Pandit                    <Purvin.Pandit@thomson.net>
 *
 *************************************************************************************
 */

#include <stdlib.h>
#include <assert.h>
#include "mbuffer.h"
#include "global.h"
#include "memalloc.h"
#include "erc_do.h"
#include "image.h"

extern int erc_mvperMB;
struct img_par *erc_img;

// picture error concealment
// concealment_head points to first node in list, concealment_end points to
// last node in list. Initialize both to NULL, meaning no nodes in list yet
struct concealment_node *concealment_head = NULL;
struct concealment_node *concealment_end = NULL;

// static function declarations
static int concealByCopy(frame *recfr, int currMBNum, objectBuffer_t *object_list, int picSizeX);
static int concealByTrial(frame *recfr, imgpel *predMB,
                          int currMBNum, objectBuffer_t *object_list, int predBlocks[],
                          int picSizeX, int picSizeY, int *yCondition);
static int edgeDistortion (int predBlocks[], int currYBlockNum, imgpel *predMB,
                           imgpel *recY, int picSizeX, int regionSize);
static void copyBetweenFrames (frame *recfr, int currYBlockNum, int picSizeX, int regionSize);
static void buildPredRegionYUV(struct img_par *img, int *mv, int x, int y, imgpel *predMB);

// picture error concealment
static void buildPredblockRegionYUV(struct img_par *img, int *mv,
                                    int x, int y, imgpel *predMB, int list);
static void CopyImgData(imgpel **inputY, imgpel ***inputUV, imgpel **outputY,
                        imgpel ***outputUV, int img_width, int img_height);


static void copyPredMB (int currYBlockNum, imgpel *predMB, frame *recfr,
                        int picSizeX, int regionSize);

extern const unsigned char subblk_offset_y[3][8][4];
extern const unsigned char subblk_offset_x[3][8][4];
static int uv_div[2][4] = {{0, 1, 1, 0}, {0, 1, 0, 0}}; //[x/y][yuv_format]

/*!
 ************************************************************************
 * \brief
 *      The main function for Inter (P) frame concealment.
 * \return
 *      0, if the concealment was not successful and simple concealment should be used
 *      1, otherwise (even if none of the blocks were concealed)
 * \param recfr
 *      Reconstructed frame buffer
 * \param object_list
 *      Motion info for all MBs in the frame
 * \param picSizeX
 *      Width of the frame in pixels
 * \param picSizeY
 *      Height of the frame in pixels
 * \param errorVar
 *      Variables for error concealment
 * \param chroma_format_idc
 *      Chroma format IDC
 ************************************************************************
 */
int ercConcealInterFrame(frame *recfr, objectBuffer_t *object_list,
                         int picSizeX, int picSizeY, ercVariables_t *errorVar, int chroma_format_idc )
{
  int lastColumn = 0, lastRow = 0, predBlocks[8];
  int lastCorruptedRow = -1, firstCorruptedRow = -1, currRow = 0,
    row, column, columnInd, areaHeight = 0, i = 0;
  imgpel *predMB;

  /* if concealment is on */
  if ( errorVar && errorVar->concealment )
  {
    /* if there are segments to be concealed */
    if ( errorVar->nOfCorruptedSegments )
    {
      if (chroma_format_idc != YUV400)
        predMB = (imgpel *) malloc ( (256 + (img->mb_cr_size_x*img->mb_cr_size_y)*2) * sizeof (imgpel));
      else
        predMB = (imgpel *) malloc(256 * sizeof (imgpel));

      if ( predMB == NULL ) no_mem_exit("ercConcealInterFrame: predMB");

      lastRow = (int) (picSizeY>>4);
      lastColumn = (int) (picSizeX>>4);

      for ( columnInd = 0; columnInd < lastColumn; columnInd ++)
      {

        column = ((columnInd%2) ? (lastColumn - columnInd/2 -1) : (columnInd/2));

        for ( row = 0; row < lastRow; row++)
        {

          if ( errorVar->yCondition[MBxy2YBlock(column, row, 0, picSizeX)] <= ERC_BLOCK_CORRUPTED )
          {                           // ERC_BLOCK_CORRUPTED (1) or ERC_BLOCK_EMPTY (0)
            firstCorruptedRow = row;
            /* find the last row which has corrupted blocks (in same continuous area) */
            for ( lastCorruptedRow = row+1; lastCorruptedRow < lastRow; lastCorruptedRow++)
            {
              /* check blocks in the current column */
              if (errorVar->yCondition[MBxy2YBlock(column, lastCorruptedRow, 0, picSizeX)] > ERC_BLOCK_CORRUPTED)
              {
                /* current one is already OK, so the last was the previous one */
                lastCorruptedRow --;
                break;
              }
            }
            if ( lastCorruptedRow >= lastRow )
            {
              /* correct only from above */
              lastCorruptedRow = lastRow-1;
              for ( currRow = firstCorruptedRow; currRow < lastRow; currRow++ )
              {

                ercCollect8PredBlocks (predBlocks, (currRow<<1), (column<<1),
                  errorVar->yCondition, (lastRow<<1), (lastColumn<<1), 2, 0);

                if(erc_mvperMB >= MVPERMB_THR)
                  concealByTrial(recfr, predMB,
                    currRow*lastColumn+column, object_list, predBlocks,
                    picSizeX, picSizeY,
                    errorVar->yCondition);
                else
                  concealByCopy(recfr, currRow*lastColumn+column,
                    object_list, picSizeX);

                ercMarkCurrMBConcealed (currRow*lastColumn+column, -1, picSizeX, errorVar);
              }
              row = lastRow;
            }
            else if ( firstCorruptedRow == 0 )
            {
              /* correct only from below */
              for ( currRow = lastCorruptedRow; currRow >= 0; currRow-- )
              {

                ercCollect8PredBlocks (predBlocks, (currRow<<1), (column<<1),
                  errorVar->yCondition, (lastRow<<1), (lastColumn<<1), 2, 0);

                if(erc_mvperMB >= MVPERMB_THR)
                  concealByTrial(recfr, predMB,
                    currRow*lastColumn+column, object_list, predBlocks,
                    picSizeX, picSizeY,
                    errorVar->yCondition);
                else
                  concealByCopy(recfr, currRow*lastColumn+column,
                    object_list, picSizeX);

                ercMarkCurrMBConcealed (currRow*lastColumn+column, -1, picSizeX, errorVar);
              }

              row = lastCorruptedRow+1;
            }
            else
            {
              /* correct bi-directionally */

              row = lastCorruptedRow+1;

              areaHeight = lastCorruptedRow-firstCorruptedRow+1;

              /*
              *  Conceal the corrupted area switching between the up and the bottom rows
              */
              for ( i = 0; i < areaHeight; i++)
              {
                if ( i % 2 )
                {
                  currRow = lastCorruptedRow;
                  lastCorruptedRow --;
                }
                else
                {
                  currRow = firstCorruptedRow;
                  firstCorruptedRow ++;
                }

                ercCollect8PredBlocks (predBlocks, (currRow<<1), (column<<1),
                  errorVar->yCondition, (lastRow<<1), (lastColumn<<1), 2, 0);

                if(erc_mvperMB >= MVPERMB_THR)
                  concealByTrial(recfr, predMB,
                    currRow*lastColumn+column, object_list, predBlocks,
                    picSizeX, picSizeY,
                    errorVar->yCondition);
                else
                  concealByCopy(recfr, currRow*lastColumn+column,
                    object_list, picSizeX);

                ercMarkCurrMBConcealed (currRow*lastColumn+column, -1, picSizeX, errorVar);

              }
            }
            lastCorruptedRow = -1;
            firstCorruptedRow = -1;
          }
        }
      }

      free(predMB);
    }
    return 1;
  }
  else
    return 0;
}

/*!
 ************************************************************************
 * \brief
 *      It conceals a given MB by simply copying the pixel area from the reference image
 *      that is at the same location as the macroblock in the current image. This correcponds
 *      to COPY MBs.
 * \return
 *      Always zero (0).
 * \param recfr
 *      Reconstructed frame buffer
 * \param currMBNum
 *      current MB index
 * \param object_list
 *      Motion info for all MBs in the frame
 * \param picSizeX
 *      Width of the frame in pixels
 ************************************************************************
 */
static int concealByCopy(frame *recfr, int currMBNum,
  objectBuffer_t *object_list, int picSizeX)
{
  objectBuffer_t *currRegion;

  currRegion = object_list+(currMBNum<<2);
  currRegion->regionMode = REGMODE_INTER_COPY;

  currRegion->xMin = (xPosMB(currMBNum,picSizeX)<<4);
  currRegion->yMin = (yPosMB(currMBNum,picSizeX)<<4);

  copyBetweenFrames (recfr, MBNum2YBlock(currMBNum,0,picSizeX), picSizeX, 16);

  return 0;
}

/*!
 ************************************************************************
 * \brief
 *      Copies the co-located pixel values from the reference to the current frame.
 *      Used by concealByCopy
 * \param recfr
 *      Reconstructed frame buffer
 * \param currYBlockNum
 *      index of the block (8x8) in the Y plane
 * \param picSizeX
 *      Width of the frame in pixels
 * \param regionSize
 *      can be 16 or 8 to tell the dimension of the region to copy
 ************************************************************************
 */
static void copyBetweenFrames (frame *recfr, int currYBlockNum, int picSizeX, int regionSize)
{
  int j, k, location, xmin, ymin;
  StorablePicture* refPic = listX[0][0];

  /* set the position of the region to be copied */
  xmin = (xPosYBlock(currYBlockNum,picSizeX)<<3);
  ymin = (yPosYBlock(currYBlockNum,picSizeX)<<3);

  for (j = ymin; j < ymin + regionSize; j++)
    for (k = xmin; k < xmin + regionSize; k++)
    {
      location = j * picSizeX + k;
//th      recfr->yptr[location] = dec_picture->imgY[j][k];
      recfr->yptr[location] = refPic->imgY[j][k];
    }

    for (j = ymin >> uv_div[1][dec_picture->chroma_format_idc]; j < (ymin + regionSize) >> uv_div[1][dec_picture->chroma_format_idc]; j++)
      for (k = xmin >> uv_div[0][dec_picture->chroma_format_idc]; k < (xmin + regionSize) >> uv_div[0][dec_picture->chroma_format_idc]; k++)
      {
//        location = j * picSizeX / 2 + k;
        location = ((j * picSizeX) >> uv_div[0][dec_picture->chroma_format_idc]) + k;

//th        recfr->uptr[location] = dec_picture->imgUV[0][j][k];
//th        recfr->vptr[location] = dec_picture->imgUV[1][j][k];
        recfr->uptr[location] = refPic->imgUV[0][j][k];
        recfr->vptr[location] = refPic->imgUV[1][j][k];
      }
}

/*!
 ************************************************************************
 * \brief
 *      It conceals a given MB by using the motion vectors of one reliable neighbor. That MV of a
 *      neighbor is selected wich gives the lowest pixel difference at the edges of the MB
 *      (see function edgeDistortion). This corresponds to a spatial smoothness criteria.
 * \return
 *      Always zero (0).
 * \param recfr
 *      Reconstructed frame buffer
 * \param predMB
 *      memory area for storing temporary pixel values for a macroblock
 *      the Y,U,V planes are concatenated y = predMB, u = predMB+256, v = predMB+320
 * \param currMBNum
 *      current MB index
 * \param object_list
 *      array of region structures storing region mode and mv for each region
 * \param predBlocks
 *      status array of the neighboring blocks (if they are OK, concealed or lost)
 * \param picSizeX
 *      Width of the frame in pixels
 * \param picSizeY
 *      Height of the frame in pixels
 * \param yCondition
 *      array for conditions of Y blocks from ercVariables_t
 ************************************************************************
 */
static int concealByTrial(frame *recfr, imgpel *predMB,
                          int currMBNum, objectBuffer_t *object_list, int predBlocks[],
                          int picSizeX, int picSizeY, int *yCondition)
{
  int predMBNum = 0, numMBPerLine,
      compSplit1 = 0, compSplit2 = 0, compLeft = 1, comp = 0, compPred, order = 1,
      fInterNeighborExists, numIntraNeighbours,
      fZeroMotionChecked, predSplitted = 0,
      threshold = ERC_BLOCK_OK,
      minDist, currDist, i, k, bestDir;
  int regionSize;
  objectBuffer_t *currRegion;
  int mvBest[3] , mvPred[3], *mvptr;

  numMBPerLine = (int) (picSizeX>>4);

  comp = 0;
  regionSize = 16;

  do
  { /* 4 blocks loop */

    currRegion = object_list+(currMBNum<<2)+comp;

    /* set the position of the region to be concealed */

    currRegion->xMin = (xPosYBlock(MBNum2YBlock(currMBNum,comp,picSizeX),picSizeX)<<3);
    currRegion->yMin = (yPosYBlock(MBNum2YBlock(currMBNum,comp,picSizeX),picSizeX)<<3);

    do
    { /* reliability loop */

      minDist = 0;
      fInterNeighborExists = 0;
      numIntraNeighbours = 0;
      fZeroMotionChecked = 0;

      /* loop the 4 neighbours */
      for (i = 4; i < 8; i++)
      {

        /* if reliable, try it */
        if (predBlocks[i] >= threshold)
        {
          switch (i)
          {
          case 4:
            predMBNum = currMBNum-numMBPerLine;
            compSplit1 = 2;
            compSplit2 = 3;
            break;

          case 5:
            predMBNum = currMBNum-1;
            compSplit1 = 1;
            compSplit2 = 3;
            break;

          case 6:
            predMBNum = currMBNum+numMBPerLine;
            compSplit1 = 0;
            compSplit2 = 1;
            break;

          case 7:
            predMBNum = currMBNum+1;
            compSplit1 = 0;
            compSplit2 = 2;
            break;
          }

          /* try the concealment with the Motion Info of the current neighbour
          only try if the neighbour is not Intra */
          if (isBlock(object_list,predMBNum,compSplit1,INTRA) ||
            isBlock(object_list,predMBNum,compSplit2,INTRA))
          {
            numIntraNeighbours++;
          }
          else
          {
            /* if neighbour MB is splitted, try both neighbour blocks */
            for (predSplitted = isSplitted(object_list, predMBNum),
              compPred = compSplit1;
              predSplitted >= 0;
              compPred = compSplit2,
              predSplitted -= ((compSplit1 == compSplit2) ? 2 : 1))
            {

              /* if Zero Motion Block, do the copying. This option is tried only once */
              if (isBlock(object_list, predMBNum, compPred, INTER_COPY))
              {

                if (fZeroMotionChecked)
                {
                  continue;
                }
                else
                {
                  fZeroMotionChecked = 1;

                  mvPred[0] = mvPred[1] = 0;
                  mvPred[2] = 0;

                  buildPredRegionYUV(erc_img, mvPred, currRegion->xMin, currRegion->yMin, predMB);
                }
              }
              /* build motion using the neighbour's Motion Parameters */
              else if (isBlock(object_list,predMBNum,compPred,INTRA))
              {
                continue;
              }
              else
              {
                mvptr = getParam(object_list, predMBNum, compPred, mv);

                mvPred[0] = mvptr[0];
                mvPred[1] = mvptr[1];
                mvPred[2] = mvptr[2];

                buildPredRegionYUV(erc_img, mvPred, currRegion->xMin, currRegion->yMin, predMB);
              }

              /* measure absolute boundary pixel difference */
              currDist = edgeDistortion(predBlocks,
                MBNum2YBlock(currMBNum,comp,picSizeX),
                predMB, recfr->yptr, picSizeX, regionSize);

              /* if so far best -> store the pixels as the best concealment */
              if (currDist < minDist || !fInterNeighborExists)
              {

                minDist = currDist;
                bestDir = i;

                for (k=0;k<3;k++)
                  mvBest[k] = mvPred[k];

                currRegion->regionMode =
                  (isBlock(object_list, predMBNum, compPred, INTER_COPY)) ?
                  ((regionSize == 16) ? REGMODE_INTER_COPY : REGMODE_INTER_COPY_8x8) :
                  ((regionSize == 16) ? REGMODE_INTER_PRED : REGMODE_INTER_PRED_8x8);

                copyPredMB(MBNum2YBlock(currMBNum,comp,picSizeX), predMB, recfr,
                  picSizeX, regionSize);
              }

              fInterNeighborExists = 1;
            }
          }
        }
    }

    threshold--;

    } while ((threshold >= ERC_BLOCK_CONCEALED) && (fInterNeighborExists == 0));

    /* always try zero motion */
    if (!fZeroMotionChecked)
    {
      mvPred[0] = mvPred[1] = 0;
      mvPred[2] = 0;

      buildPredRegionYUV(erc_img, mvPred, currRegion->xMin, currRegion->yMin, predMB);

      currDist = edgeDistortion(predBlocks,
        MBNum2YBlock(currMBNum,comp,picSizeX),
        predMB, recfr->yptr, picSizeX, regionSize);

      if (currDist < minDist || !fInterNeighborExists)
      {

        minDist = currDist;
        for (k=0;k<3;k++)
          mvBest[k] = mvPred[k];

        currRegion->regionMode =
          ((regionSize == 16) ? REGMODE_INTER_COPY : REGMODE_INTER_COPY_8x8);

        copyPredMB(MBNum2YBlock(currMBNum,comp,picSizeX), predMB, recfr,
          picSizeX, regionSize);
      }
    }

    for (i=0; i<3; i++)
      currRegion->mv[i] = mvBest[i];

    yCondition[MBNum2YBlock(currMBNum,comp,picSizeX)] = ERC_BLOCK_CONCEALED;
    comp = (comp+order+4)%4;
    compLeft--;

    } while (compLeft);

    return 0;
}

/*!
************************************************************************
* \brief
*      Builds the motion prediction pixels from the given location (in 1/4 pixel units)
*      of the reference frame. It not only copies the pixel values but builds the interpolation
*      when the pixel positions to be copied from is not full pixel (any 1/4 pixel position).
*      It copies the resulting pixel vlaues into predMB.
* \param img
*      The pointer of img_par struture of current frame
* \param mv
*      The pointer of the predicted MV of the current (being concealed) MB
* \param x
*      The x-coordinate of the above-left corner pixel of the current MB
* \param y
*      The y-coordinate of the above-left corner pixel of the current MB
* \param predMB
*      memory area for storing temporary pixel values for a macroblock
*      the Y,U,V planes are concatenated y = predMB, u = predMB+256, v = predMB+320
************************************************************************
*/
static void buildPredRegionYUV(struct img_par *img, int *mv, int x, int y, imgpel *predMB)
{
  int tmp_block[BLOCK_SIZE][BLOCK_SIZE];
  int i=0,j=0,ii=0,jj=0,i1=0,j1=0,j4=0,i4=0;
  int jf=0;
  int uv;
  int vec1_x=0,vec1_y=0;
  int ioff,joff;
  imgpel *pMB = predMB;

  int ii0,jj0,ii1,jj1,if1,jf1,if0,jf0;
  int mv_mul;

  //FRExt
  int f1_x, f1_y, f2_x, f2_y, f3, f4, ifx;
  int b8, b4;
  int yuv = dec_picture->chroma_format_idc - 1;

  int ref_frame = imax (mv[2], 0); // !!KS: quick fix, we sometimes seem to get negative ref_pic here, so restrict to zero an above

  /* Update coordinates of the current concealed macroblock */
  img->mb_x = x/MB_BLOCK_SIZE;
  img->mb_y = y/MB_BLOCK_SIZE;
  img->block_y = img->mb_y * BLOCK_SIZE;
  img->pix_c_y = img->mb_y * img->mb_cr_size_y;
  img->block_x = img->mb_x * BLOCK_SIZE;
  img->pix_c_x = img->mb_x * img->mb_cr_size_x;

  mv_mul=4;

  // luma *******************************************************

  for(j=0;j<MB_BLOCK_SIZE/BLOCK_SIZE;j++)
  {
    joff=j*4;
    j4=img->block_y+j;
    for(i=0;i<MB_BLOCK_SIZE/BLOCK_SIZE;i++)
    {
      ioff=i*4;
      i4=img->block_x+i;

      vec1_x = i4*4*mv_mul + mv[0];
      vec1_y = j4*4*mv_mul + mv[1];

      get_block(ref_frame, listX[0], vec1_x,vec1_y,img,tmp_block);

      for(ii=0;ii<BLOCK_SIZE;ii++)
        for(jj=0;jj<MB_BLOCK_SIZE/BLOCK_SIZE;jj++)
          img->mpr[jj+joff][ii+ioff]=tmp_block[jj][ii];
    }
  }


  for (j = 0; j < 16; j++)
  {
    for (i = 0; i < 16; i++)
    {
      pMB[j*16+i] = img->mpr[j][i];
    }
  }
  pMB += 256;

  if (dec_picture->chroma_format_idc != YUV400)
  {
    // chroma *******************************************************
    f1_x = 64/img->mb_cr_size_x;
    f2_x=f1_x-1;

    f1_y = 64/img->mb_cr_size_y;
    f2_y=f1_y-1;

    f3=f1_x*f1_y;
    f4=f3>>1;

    for(uv=0;uv<2;uv++)
    {
      for (b8=0;b8<(img->num_blk8x8_uv/2);b8++)
      {
        for(b4=0;b4<4;b4++)
        {
          joff = subblk_offset_y[yuv][b8][b4];
          j4=img->pix_c_y+joff;
          ioff = subblk_offset_x[yuv][b8][b4];
          i4=img->pix_c_x+ioff;

          for(jj=0;jj<4;jj++)
          {
            jf=(j4+jj)/(img->mb_cr_size_y/4);     // jf  = Subblock_y-coordinate
            for(ii=0;ii<4;ii++)
            {
              ifx=(i4+ii)/(img->mb_cr_size_x/4);  // ifx = Subblock_x-coordinate

              i1=(i4+ii)*f1_x + mv[0];
              j1=(j4+jj)*f1_y + mv[1];

              ii0=iClip3 (0, dec_picture->size_x_cr-1, i1/f1_x);
              jj0=iClip3 (0, dec_picture->size_y_cr-1, j1/f1_y);
              ii1=iClip3 (0, dec_picture->size_x_cr-1, ((i1+f2_x)/f1_x));
              jj1=iClip3 (0, dec_picture->size_y_cr-1, ((j1+f2_y)/f1_y));

              if1=(i1 & f2_x);
              jf1=(j1 & f2_y);
              if0=f1_x-if1;
              jf0=f1_y-jf1;

              img->mpr[jj+joff][ii+ioff]=(if0*jf0*listX[0][ref_frame]->imgUV[uv][jj0][ii0]+
                                          if1*jf0*listX[0][ref_frame]->imgUV[uv][jj0][ii1]+
                                          if0*jf1*listX[0][ref_frame]->imgUV[uv][jj1][ii0]+
                                          if1*jf1*listX[0][ref_frame]->imgUV[uv][jj1][ii1]+f4)/f3;
            }
          }
        }
      }

      for (j = 0; j < 8; j++)
      {
        for (i = 0; i < 8; i++)
        {
          pMB[j*8+i] = img->mpr[j][i];
        }
      }
      pMB += 64;

    }
  }
}
/*!
 ************************************************************************
 * \brief
 *      Copies pixel values between a YUV frame and the temporary pixel value storage place. This is
 *      used to save some pixel values temporarily before overwriting it, or to copy back to a given
 *      location in a frame the saved pixel values.
 * \param currYBlockNum
 *      index of the block (8x8) in the Y plane
 * \param predMB
 *      memory area where the temporary pixel values are stored
 *      the Y,U,V planes are concatenated y = predMB, u = predMB+256, v = predMB+320
 * \param recfr
 *      pointer to a YUV frame
 * \param picSizeX
 *      picture width in pixels
 * \param regionSize
 *      can be 16 or 8 to tell the dimension of the region to copy
 ************************************************************************
 */
static void copyPredMB (int currYBlockNum, imgpel *predMB, frame *recfr,
                        int picSizeX, int regionSize)
{

  int j, k, xmin, ymin, xmax, ymax;
  int locationTmp, locationPred;
  int uv_x = uv_div[0][dec_picture->chroma_format_idc];
  int uv_y = uv_div[1][dec_picture->chroma_format_idc];

  xmin = (xPosYBlock(currYBlockNum,picSizeX)<<3);
  ymin = (yPosYBlock(currYBlockNum,picSizeX)<<3);
  xmax = xmin + regionSize -1;
  ymax = ymin + regionSize -1;

  for (j = ymin; j <= ymax; j++)
  {
    for (k = xmin; k <= xmax; k++)
    {
      locationPred = j * picSizeX + k;
      locationTmp = (j-ymin) * 16 + (k-xmin);
      dec_picture->imgY[j][k] = predMB[locationTmp];
    }
  }

  if (dec_picture->chroma_format_idc != YUV400)
  {
    for (j = (ymin>>uv_y); j <= (ymax>>uv_y); j++)
    {
      for (k = (xmin>>uv_x); k <= (xmax>>uv_x); k++)
      {
        locationPred = ((j * picSizeX) >> uv_x) + k;
        locationTmp = (j-(ymin>>uv_y)) * img->mb_cr_size_x + (k-(xmin>>1)) + 256;
        dec_picture->imgUV[0][j][k] = predMB[locationTmp];

        locationTmp += 64;

        dec_picture->imgUV[1][j][k] = predMB[locationTmp];
      }
    }
  }
}

/*!
 ************************************************************************
 * \brief
 *      Calculates a weighted pixel difference between edge Y pixels of the macroblock stored in predMB
 *      and the pixels in the given Y plane of a frame (recY) that would become neighbor pixels if
 *      predMB was placed at currYBlockNum block position into the frame. This "edge distortion" value
 *      is used to determine how well the given macroblock in predMB would fit into the frame when
 *      considering spatial smoothness. If there are correctly received neighbor blocks (status stored
 *      in predBlocks) only they are used in calculating the edge distorion; otherwise also the already
 *      concealed neighbor blocks can also be used.
 * \return
 *      The calculated weighted pixel difference at the edges of the MB.
 * \param predBlocks
 *      status array of the neighboring blocks (if they are OK, concealed or lost)
 * \param currYBlockNum
 *      index of the block (8x8) in the Y plane
 * \param predMB
 *      memory area where the temporary pixel values are stored
 *      the Y,U,V planes are concatenated y = predMB, u = predMB+256, v = predMB+320
 * \param recY
 *      pointer to a Y plane of a YUV frame
 * \param picSizeX
 *      picture width in pixels
 * \param regionSize
 *      can be 16 or 8 to tell the dimension of the region to copy
 ************************************************************************
 */
static int edgeDistortion (int predBlocks[], int currYBlockNum, imgpel *predMB,
                           imgpel *recY, int picSizeX, int regionSize)
{
  int i, j, distortion, numOfPredBlocks, threshold = ERC_BLOCK_OK;
  imgpel *currBlock = NULL, *neighbor = NULL;
  int currBlockOffset = 0;

  currBlock = recY + (yPosYBlock(currYBlockNum,picSizeX)<<3)*picSizeX + (xPosYBlock(currYBlockNum,picSizeX)<<3);

  do
  {

    distortion = 0; numOfPredBlocks = 0;

    // loop the 4 neighbors
    for (j = 4; j < 8; j++)
    {
      /* if reliable, count boundary pixel difference */
      if (predBlocks[j] >= threshold)
      {

        switch (j)
        {
        case 4:
          neighbor = currBlock - picSizeX;
          for ( i = 0; i < regionSize; i++ )
          {
            distortion += mabs((int)(predMB[i] - neighbor[i]));
          }
          break;
        case 5:
          neighbor = currBlock - 1;
          for ( i = 0; i < regionSize; i++ )
          {
            distortion += mabs((int)(predMB[i*16] - neighbor[i*picSizeX]));
          }
          break;
        case 6:
          neighbor = currBlock + regionSize*picSizeX;
          currBlockOffset = (regionSize-1)*16;
          for ( i = 0; i < regionSize; i++ )
          {
            distortion += mabs((int)(predMB[i+currBlockOffset] - neighbor[i]));
          }
          break;
        case 7:
          neighbor = currBlock + regionSize;
          currBlockOffset = regionSize-1;
          for ( i = 0; i < regionSize; i++ )
          {
            distortion += mabs((int)(predMB[i*16+currBlockOffset] - neighbor[i*picSizeX]));
          }
          break;
        }

        numOfPredBlocks++;
      }
    }

    threshold--;
    if (threshold < ERC_BLOCK_CONCEALED)
      break;
  } while (numOfPredBlocks == 0);

  if(numOfPredBlocks == 0)
  {
    return 0;
    // assert (numOfPredBlocks != 0); !!!KS hmm, trying to continue...
  }
  return (distortion/numOfPredBlocks);
}

// picture error concealment below

/*!
************************************************************************
* \brief
* The motion prediction pixels are calculated from the given location (in
* 1/4 pixel units) of the referenced frame. It copies the sub block from the
* corresponding reference to the frame to be concealed.
*
*************************************************************************
*/
static void buildPredblockRegionYUV(struct img_par *img, int *mv,
                                    int x, int y, imgpel *predMB, int list)
{
    int tmp_block[BLOCK_SIZE][BLOCK_SIZE];
    int i=0,j=0,ii=0,jj=0,i1=0,j1=0,j4=0,i4=0;
    int jf=0;
    int uv;
    int vec1_x=0,vec1_y=0;
    int ioff,joff;
    imgpel *pMB = predMB;

    int ii0,jj0,ii1,jj1,if1,jf1,if0,jf0;
    int mv_mul;

    //FRExt
    int f1_x, f1_y, f2_x, f2_y, f3, f4, ifx;
    int yuv = dec_picture->chroma_format_idc - 1;

    int ref_frame = mv[2];

    /* Update coordinates of the current concealed macroblock */

    img->mb_x = x/BLOCK_SIZE;
    img->mb_y = y/BLOCK_SIZE;
    img->block_y = img->mb_y * BLOCK_SIZE;
    img->pix_c_y = img->mb_y * img->mb_cr_size_y/4;
    img->block_x = img->mb_x * BLOCK_SIZE;
    img->pix_c_x = img->mb_x * img->mb_cr_size_x/4;

    mv_mul=4;

    // luma *******************************************************

    vec1_x = x*mv_mul + mv[0];
    vec1_y = y*mv_mul + mv[1];

    get_block(ref_frame, listX[list], vec1_x,vec1_y,img,tmp_block);

    for(jj=0;jj<MB_BLOCK_SIZE/BLOCK_SIZE;jj++)
      for(ii=0;ii<BLOCK_SIZE;ii++)
        img->mpr[jj][ii]=tmp_block[jj][ii];


    for (j = 0; j < 4; j++)
    {
      for (i = 0; i < 4; i++)
      {
        pMB[j*4+i] = img->mpr[j][i];
      }
    }
    pMB += 16;

    if (dec_picture->chroma_format_idc != YUV400)
    {
        // chroma *******************************************************
        f1_x = 64/(img->mb_cr_size_x);
        f2_x=f1_x-1;

        f1_y = 64/(img->mb_cr_size_y);
        f2_y=f1_y-1;

        f3=f1_x*f1_y;
        f4=f3>>1;

        for(uv=0;uv<2;uv++)
        {
            joff = subblk_offset_y[yuv][0][0];
            j4=img->pix_c_y+joff;
            ioff = subblk_offset_x[yuv][0][0];
            i4=img->pix_c_x+ioff;

            for(jj=0;jj<2;jj++)
            {
                jf=(j4+jj)/(img->mb_cr_size_y/4);     // jf  = Subblock_y-coordinate
                for(ii=0;ii<2;ii++)
                {
                    ifx=(i4+ii)/(img->mb_cr_size_x/4);  // ifx = Subblock_x-coordinate

                    i1=(i4+ii)*f1_x + mv[0];
                    j1=(j4+jj)*f1_y + mv[1];

                    ii0=iClip3 (0, dec_picture->size_x_cr-1, i1/f1_x);
                    jj0=iClip3 (0, dec_picture->size_y_cr-1, j1/f1_y);
                    ii1=iClip3 (0, dec_picture->size_x_cr-1, ((i1+f2_x)/f1_x));
                    jj1=iClip3 (0, dec_picture->size_y_cr-1, ((j1+f2_y)/f1_y));

                    if1=(i1 & f2_x);
                    jf1=(j1 & f2_y);
                    if0=f1_x-if1;
                    jf0=f1_y-jf1;

                    img->mpr[jj][ii]=(if0*jf0*listX[list][ref_frame]->imgUV[uv][jj0][ii0]+
                        if1*jf0*listX[list][ref_frame]->imgUV[uv][jj0][ii1]+
                        if0*jf1*listX[list][ref_frame]->imgUV[uv][jj1][ii0]+
                        if1*jf1*listX[list][ref_frame]->imgUV[uv][jj1][ii1]+f4)/f3;
                }
            }

            for (j = 0; j < 2; j++)
            {
              for (i = 0; i < 2; i++)
              {
                pMB[j*2+i] = img->mpr[j][i];
              }
            }
            pMB += 4;

        }
    }
}

/*!
************************************************************************
* \brief
*    compares two stored pictures by picture number for qsort in descending order
*
************************************************************************
*/
static int compare_pic_by_pic_num_desc( const void *arg1, const void *arg2 )
{
    if ( (*(StorablePicture**)arg1)->pic_num < (*(StorablePicture**)arg2)->pic_num)
        return 1;
    if ( (*(StorablePicture**)arg1)->pic_num > (*(StorablePicture**)arg2)->pic_num)
        return -1;
    else
        return 0;
}

/*!
************************************************************************
* \brief
*    compares two stored pictures by picture number for qsort in descending order
*
************************************************************************
*/
static int compare_pic_by_lt_pic_num_asc( const void *arg1, const void *arg2 )
{
    if ( (*(StorablePicture**)arg1)->long_term_pic_num < (*(StorablePicture**)arg2)->long_term_pic_num)
        return -1;
    if ( (*(StorablePicture**)arg1)->long_term_pic_num > (*(StorablePicture**)arg2)->long_term_pic_num)
        return 1;
    else
        return 0;
}

/*!
************************************************************************
* \brief
*    compares two stored pictures by poc for qsort in ascending order
*
************************************************************************
*/
static int compare_pic_by_poc_asc( const void *arg1, const void *arg2 )
{
    if ( (*(StorablePicture**)arg1)->poc < (*(StorablePicture**)arg2)->poc)
        return -1;
    if ( (*(StorablePicture**)arg1)->poc > (*(StorablePicture**)arg2)->poc)
        return 1;
    else
        return 0;
}


/*!
************************************************************************
* \brief
*    compares two stored pictures by poc for qsort in descending order
*
************************************************************************
*/
static int compare_pic_by_poc_desc( const void *arg1, const void *arg2 )
{
    if ( (*(StorablePicture**)arg1)->poc < (*(StorablePicture**)arg2)->poc)
        return 1;
    if ( (*(StorablePicture**)arg1)->poc > (*(StorablePicture**)arg2)->poc)
        return -1;
    else
        return 0;
}

/*!
************************************************************************
* \brief
*    Copy image data from one array to another array
************************************************************************
*/

static
void CopyImgData(imgpel **inputY, imgpel ***inputUV, imgpel **outputY,
                 imgpel ***outputUV, int img_width, int img_height)
{
    int x, y;

    for (y=0; y<img_height; y++)
        for (x=0; x<img_width; x++)
            outputY[y][x] = inputY[y][x];

    for (y=0; y<img_height/2; y++)
        for (x=0; x<img_width/2; x++)
        {
            outputUV[0][y][x] = inputUV[0][y][x];
            outputUV[1][y][x] = inputUV[1][y][x];
        }
}

/*!
************************************************************************
* \brief
*    Copies the last reference frame for concealing reference frame loss.
************************************************************************
*/

static StorablePicture *
get_last_ref_pic_from_dpb()
{
    int used_size = dpb.used_size - 1;
    int i;

    for(i = used_size; i >= 0; i--)
    {
        if (dpb.fs[i]->is_used==3)
        {
            if (((dpb.fs[i]->frame->used_for_reference) &&
                (!dpb.fs[i]->frame->is_long_term)) /*||  ((dpb.fs[i]->frame->used_for_reference==0)
                                                   && (dpb.fs[i]->frame->slice_type == P_SLICE))*/ )
            {
                return dpb.fs[i]->frame;
            }
        }
    }

    return NULL;
}

/*!
************************************************************************
* \brief
* Conceals the lost reference or non reference frame by either frame copy
* or motion vector copy concealment.
*
************************************************************************
*/

static void copy_to_conceal(StorablePicture *src, StorablePicture *dst, ImageParameters *img)
{
    int i=0;
    int mv[3];
    int multiplier;
    imgpel *predMB, *storeYUV;
    int j, y, x, mb_height, mb_width, ii=0, jj=0;
    int uv;
    int mm, nn;
    int scale = 1;
    // struct inp_par *test;

    img->current_mb_nr = 0;

    dst->PicSizeInMbs  = src->PicSizeInMbs;

    dst->slice_type = src->slice_type = img->conceal_slice_type;

    dst->idr_flag = 0; //since we do not want to clears the ref list

    dst->no_output_of_prior_pics_flag = src->no_output_of_prior_pics_flag;
    dst->long_term_reference_flag = src->long_term_reference_flag;
    dst->adaptive_ref_pic_buffering_flag = src->adaptive_ref_pic_buffering_flag = 0;
    dst->chroma_format_idc = src->chroma_format_idc;
    dst->frame_mbs_only_flag = src->frame_mbs_only_flag;
    dst->frame_cropping_flag = src->frame_cropping_flag;
    dst->frame_cropping_rect_left_offset = src->frame_cropping_rect_left_offset;
    dst->frame_cropping_rect_right_offset = src->frame_cropping_rect_right_offset;
    dst->frame_cropping_rect_bottom_offset = src->frame_cropping_rect_bottom_offset;
    dst->frame_cropping_rect_top_offset = src->frame_cropping_rect_top_offset;
    dst->qp = src->qp;
    dst->slice_qp_delta = src->slice_qp_delta;

    dec_picture = src;

    // Conceals the missing frame by frame copy concealment
    if (img->conceal_mode==1)
    {
        // We need these initializations for using deblocking filter for frame copy
        // concealment as well.
        dst->PicWidthInMbs = src->PicWidthInMbs;
        dst->PicSizeInMbs = src->PicSizeInMbs;

        CopyImgData(src->imgY, src->imgUV,
            dst->imgY, dst->imgUV,
            img->width, img->height);

    }

    // Conceals the missing frame by motion vector copy concealment
    if (img->conceal_mode==2)
    {
        if (dec_picture->chroma_format_idc != YUV400)
        {
            storeYUV = (imgpel *) malloc ( (16 + (img->mb_cr_size_x*img->mb_cr_size_y)*2/16) * sizeof (imgpel));
        }
        else
        {
            storeYUV = (imgpel *) malloc (16  * sizeof (imgpel));
        }

        erc_img = img;

        dst->PicWidthInMbs = src->PicWidthInMbs;
        dst->PicSizeInMbs = src->PicSizeInMbs;
        mb_width = dst->PicWidthInMbs;
        mb_height = (dst->PicSizeInMbs)/(dst->PicWidthInMbs);
        scale = (img->conceal_slice_type == B_SLICE) ? 2 : 1;

        if(img->conceal_slice_type == B_SLICE)
            init_lists_for_non_reference_loss(dst->slice_type, img->currentSlice->structure);
        else
            init_lists(dst->slice_type, img->currentSlice->structure);

        multiplier = BLOCK_SIZE;

        for(i=0;i<mb_height*4;i++)
        {
            mm = i*BLOCK_SIZE;
            for(j=0;j<mb_width*4;j++)
            {
                nn = j*BLOCK_SIZE;

                mv[0] = src->mv[LIST_0][i][j][0] / scale;
                mv[1] = src->mv[LIST_0][i][j][1] / scale;
                mv[2] = src->ref_idx[LIST_0][i][j];


                if(mv[2]<0)
                    mv[2]=0;

                dst->mv[LIST_0][i][j][0] = mv[0];
                dst->mv[LIST_0][i][j][1] = mv[1];
                dst->ref_idx[LIST_0][i][j] = mv[2];

                x = (j)*multiplier;
                y = (i)*multiplier;

                if ((mm%16==0) && (nn%16==0))
                    img->current_mb_nr++;

                buildPredblockRegionYUV(erc_img, mv, x, y, storeYUV, LIST_0);

                predMB = storeYUV;

                for(ii=0;ii<multiplier;ii++)
                {
                    for(jj=0;jj<multiplier;jj++)
                    {
                        dst->imgY[i*multiplier+ii][j*multiplier+jj] = predMB[ii*(multiplier)+jj];
                    }
                }

                predMB = predMB + (multiplier*multiplier);

                if (dec_picture->chroma_format_idc != YUV400)
                {

                    for(uv=0;uv<2;uv++)
                    {
                        for(ii=0;ii< (multiplier/2);ii++)
                        {
                            for(jj=0;jj< (multiplier/2);jj++)
                            {
                                dst->imgUV[uv][i*multiplier/2 +ii][j*multiplier/2 +jj] = predMB[ii*(multiplier/2)+jj];
                            }
                        }
                        predMB = predMB + (multiplier*multiplier/4);
                    }
                }
            }
        }
        free(storeYUV);
    }
}

/*!
************************************************************************
* \brief
* Uses the previous reference pic for concealment of reference frames
*
************************************************************************
*/

static void
copy_prev_pic_to_concealed_pic(StorablePicture *picture, ImageParameters *img)
{

    StorablePicture *ref_pic;
    /* get the last ref pic in dpb */
    ref_pic = get_last_ref_pic_from_dpb();

    assert(ref_pic != NULL);

    /* copy all the struc from this to current concealment pic */
    img->conceal_slice_type = P_SLICE;
    copy_to_conceal(ref_pic, picture, img);
}


/*!
************************************************************************
* \brief
* This function conceals a missing reference frame. The routine is called
* based on the difference in frame number. It conceals an IDR frame loss
* based on the sudden decrease in frame number.
*
************************************************************************
*/

void conceal_lost_frames(ImageParameters *img)
{
    int CurrFrameNum;
    int UnusedShortTermFrameNum;
    StorablePicture *picture = NULL;
    int tmp1 = img->delta_pic_order_cnt[0];
    int tmp2 = img->delta_pic_order_cnt[1];
    int i;

    img->delta_pic_order_cnt[0] = img->delta_pic_order_cnt[1] = 0;

    // printf("A gap in frame number is found, try to fill it.\n");

    if(img->IDR_concealment_flag == 1)
    {
        // Conceals an IDR frame loss. Uses the reference frame in the previous
        // GOP for concealment.
        UnusedShortTermFrameNum = 0;
        img->last_ref_pic_poc = -img->poc_gap;
        img->earlier_missing_poc = 0;
    }
    else
        UnusedShortTermFrameNum = (img->pre_frame_num + 1) % img->MaxFrameNum;

    CurrFrameNum = img->frame_num;

    while (CurrFrameNum != UnusedShortTermFrameNum)
    {
        picture = alloc_storable_picture (FRAME, img->width, img->height, img->width_cr, img->height_cr);

        picture->coded_frame = 1;
        picture->pic_num = UnusedShortTermFrameNum;
        picture->frame_num = UnusedShortTermFrameNum;
        picture->non_existing = 0;
        picture->is_output = 0;
        picture->used_for_reference = 1;
        picture->concealed_pic = 1;

        picture->adaptive_ref_pic_buffering_flag = 0;

        img->frame_num = UnusedShortTermFrameNum;

        picture->top_poc=img->last_ref_pic_poc + img->ref_poc_gap;
        picture->bottom_poc=picture->top_poc;
        picture->frame_poc=picture->top_poc;
        picture->poc=picture->top_poc;
        img->last_ref_pic_poc = picture->poc;

        copy_prev_pic_to_concealed_pic(picture, img);

        //if (UnusedShortTermFrameNum == 0)
        if(img->IDR_concealment_flag == 1)
        {
            picture->slice_type = I_SLICE;
            picture->idr_flag = 1;
            flush_dpb();
            picture->top_poc= 0;
            picture->bottom_poc=picture->top_poc;
            picture->frame_poc=picture->top_poc;
            picture->poc=picture->top_poc;
            img->last_ref_pic_poc = picture->poc;
        }

        store_picture_in_dpb(picture);

        picture=NULL;

        img->pre_frame_num = UnusedShortTermFrameNum;
        UnusedShortTermFrameNum = (UnusedShortTermFrameNum + 1) % img->MaxFrameNum;

        // update reference flags and set current flag.
        for(i=16;i>0;i--)
        {
          ref_flag[i] = ref_flag[i-1];
        }
        ref_flag[0] = 0;
    }
    img->delta_pic_order_cnt[0] = tmp1;
    img->delta_pic_order_cnt[1] = tmp2;
    img->frame_num = CurrFrameNum;
}

/*!
************************************************************************
* \brief
* Updates the reference list for motion vector copy concealment for non-
* reference frame loss.
*
************************************************************************
*/

void update_ref_list_for_concealment()
{
    unsigned i, j;
    for (i=0, j=0; i<dpb.used_size; i++)
    {
        if (dpb.fs[i]->concealment_reference)
        {
            dpb.fs_ref[j++]=dpb.fs[i];
        }
    }

    dpb.ref_frames_in_buffer = active_pps->num_ref_idx_l0_active_minus1;
}

/*!
************************************************************************
* \brief
*    Initialize the list based on the B frame or non reference 'p' frame
*    to be concealed. The function initialize listX[0] and list 1 depending
*    on current picture type
*
************************************************************************
*/
void init_lists_for_non_reference_loss(int currSliceType, PictureStructure currPicStructure)
{
    unsigned i;
    int j;
    int MaxFrameNum = 1 << (active_sps->log2_max_frame_num_minus4 + 4);
    int diff;

    int list0idx = 0;
    int list0idx_1 = 0;

    StorablePicture *tmp_s;

    if (currPicStructure == FRAME)
    {
        for(i=0;i<dpb.ref_frames_in_buffer; i++)
        {
            if(dpb.fs[i]->concealment_reference == 1)
            {
                if(dpb.fs[i]->frame_num > img->frame_to_conceal)
                    dpb.fs_ref[i]->frame_num_wrap = dpb.fs[i]->frame_num - MaxFrameNum;
                else
                    dpb.fs_ref[i]->frame_num_wrap = dpb.fs[i]->frame_num;
                dpb.fs_ref[i]->frame->pic_num = dpb.fs_ref[i]->frame_num_wrap;
            }
        }
    }

    if (currSliceType == P_SLICE)
    {
        // Calculate FrameNumWrap and PicNum
        if (currPicStructure == FRAME)
        {
            for(i=0;i<dpb.used_size; i++)
            {
                if(dpb.fs[i]->concealment_reference == 1)
                {
                    listX[0][list0idx++] = dpb.fs[i]->frame;
                }
            }
            // order list 0 by PicNum
            qsort((void *)listX[0], list0idx, sizeof(StorablePicture*), compare_pic_by_pic_num_desc);
            listXsize[0] = list0idx;
        }
    }

    if (currSliceType == B_SLICE)
    {
        if (currPicStructure == FRAME)
        {
            //      for(i=0;i<dpb.ref_frames_in_buffer; i++)
            for(i=0;i<dpb.used_size; i++)
            {
                if(dpb.fs[i]->concealment_reference == 1)
                {
                    if(img->earlier_missing_poc > dpb.fs[i]->frame->poc)
                        listX[0][list0idx++] = dpb.fs[i]->frame;
                }
            }

            qsort((void *)listX[0], list0idx, sizeof(StorablePicture*), compare_pic_by_poc_desc);
            list0idx_1 = list0idx;

            //      for(i=0;i<dpb.ref_frames_in_buffer; i++)
            for(i=0;i<dpb.used_size; i++)
            {
                if(dpb.fs[i]->concealment_reference == 1)
                {
                    if(img->earlier_missing_poc < dpb.fs[i]->frame->poc)
                        listX[0][list0idx++] = dpb.fs[i]->frame;
                }
            }

            qsort((void *)&listX[0][list0idx_1], list0idx-list0idx_1, sizeof(StorablePicture*), compare_pic_by_poc_asc);

            for (j=0; j<list0idx_1; j++)
            {
                listX[1][list0idx-list0idx_1+j]=listX[0][j];
            }
            for (j=list0idx_1; j<list0idx; j++)
            {
                listX[1][j-list0idx_1]=listX[0][j];
            }

            listXsize[0] = listXsize[1] = list0idx;

            qsort((void *)&listX[0][listXsize[0]], list0idx-listXsize[0], sizeof(StorablePicture*), compare_pic_by_lt_pic_num_asc);
            qsort((void *)&listX[1][listXsize[0]], list0idx-listXsize[0], sizeof(StorablePicture*), compare_pic_by_lt_pic_num_asc);
            listXsize[0] = listXsize[1] = list0idx;
        }
    }

    if ((listXsize[0] == listXsize[1]) && (listXsize[0] > 1))
    {
        // check if lists are identical, if yes swap first two elements of listX[1]
        diff=0;
        for (j = 0; j< listXsize[0]; j++)
        {
            if (listX[0][j]!=listX[1][j])
                diff=1;
        }
        if (!diff)
        {
            tmp_s = listX[1][0];
            listX[1][0]=listX[1][1];
            listX[1][1]=tmp_s;
        }
    }


    // set max size
    listXsize[0] = imin (listXsize[0], (int)active_sps->num_ref_frames);
    listXsize[1] = imin (listXsize[1], (int)active_sps->num_ref_frames);

    listXsize[1] = 0;
    // set the unused list entries to NULL
    for (i=listXsize[0]; i< (MAX_LIST_SIZE) ; i++)
    {
        listX[0][i] = NULL;
    }
    for (i=listXsize[1]; i< (MAX_LIST_SIZE) ; i++)
    {
        listX[1][i] = NULL;
    }
}


/*!
************************************************************************
* \brief
* Get from the dpb the picture corresponding to a POC.  The POC varies
* depending on whether it is a frame copy or motion vector copy concealment.
* The frame corresponding to the POC is returned.
*
************************************************************************
*/

StorablePicture *get_pic_from_dpb(int missingpoc, unsigned int *pos)
{
    int used_size = dpb.used_size - 1;
    int i, concealfrom = 0;

    if(img->conceal_mode == 1)
        concealfrom = missingpoc - img->poc_gap;
    else if (img->conceal_mode == 2)
        concealfrom = missingpoc + img->poc_gap;

    for(i = used_size; i >= 0; i--)
    {
        if(dpb.fs[i]->poc == concealfrom)
        {
            *pos = i;
            return dpb.fs[i]->frame;
        }
    }

    return NULL;
}

/*!
************************************************************************
* \brief
* Function to sort the POC and find the lowest number in the POC list
* Compare the integers
*
************************************************************************
*/

int comp(const void *i, const void *j)
{
    return *(int *)i - *(int *)j;
}

/*!
************************************************************************
* \brief
* Initialises a node, allocates memory for the node, and returns
* a pointer to the new node.
*
************************************************************************
*/

struct concealment_node * init_node( StorablePicture* picture, int missingpoc )
{
    struct concealment_node *ptr;

    ptr = (struct concealment_node *) calloc( 1, sizeof(struct concealment_node ) );

    if( ptr == NULL )
        return (struct concealment_node *) NULL;
    else {
        ptr->picture = picture;
        ptr->missingpocs = missingpoc;
        ptr->next = NULL;
        return ptr;
    }
}

/*!
************************************************************************
* \brief
* Prints the details of a node
*
************************************************************************
*/

void print_node( struct concealment_node *ptr )
{
    printf("Missing POC=%d\n", ptr->missingpocs );
}


/*!
************************************************************************
* \brief
* Prints all nodes from the current address passed to it.
*
************************************************************************
*/

void print_list( struct concealment_node *ptr )
{
    while( ptr != NULL )
    {
        print_node( ptr );
        ptr = ptr->next;
    }
}

/*!
************************************************************************
* \brief
* Adds a node to the end of the list.
*
************************************************************************
*/


void add_node( struct concealment_node *concealment_new )
{
    if( concealment_head == NULL )
    {
        concealment_end = concealment_head = concealment_new;
        return;
    }
    concealment_end->next = concealment_new;
    concealment_end = concealment_new;
}


/*!
************************************************************************
* \brief
* Deletes the specified node pointed to by 'ptr' from the list
*
************************************************************************
*/


void delete_node( struct concealment_node *ptr )
{
    // We only need to delete the first node in the linked list
    if( ptr == concealment_head ) {
        concealment_head = concealment_head->next;
        if( concealment_end == ptr )
            concealment_end = concealment_end->next;
        free(ptr);
    }
}

/*!
************************************************************************
* \brief
* Deletes all nodes from the place specified by ptr
*
************************************************************************
*/

void delete_list( struct concealment_node *ptr )
{
    struct concealment_node *temp;

    if( concealment_head == NULL ) return;

    if( ptr == concealment_head ) {
        concealment_head = NULL;
        concealment_end = NULL;
    }
    else
    {
        temp = concealment_head;

        while( temp->next != ptr )
            temp = temp->next;
        concealment_end = temp;
    }

    while( ptr != NULL ) {
        temp = ptr->next;
        free( ptr );
        ptr = temp;
    }
}

/*!
************************************************************************
* \brief
* Stores the missing non reference frames in the concealment buffer. The
* detection is based on the POC difference in the sorted POC array. A missing
* non reference frame is detected when the dpb is full. A singly linked list
* is maintained for storing the missing non reference frames.
*
************************************************************************
*/

void conceal_non_ref_pics(int diff)
{
    int missingpoc = 0;
    unsigned int i, pos;
    StorablePicture *conceal_from_picture = NULL;
    StorablePicture *conceal_to_picture = NULL;
    struct concealment_node *concealment_ptr = NULL;
    int temp_used_size = dpb.used_size;

    if(dpb.used_size == 0 )
        return;

    qsort(pocs_in_dpb, dpb.size, sizeof(int), comp);

    for(i=0;i<dpb.size-diff;i++)
    {
        dpb.used_size = dpb.size;
        if((pocs_in_dpb[i+1]-pocs_in_dpb[i])>img->poc_gap)
        {
            conceal_to_picture = alloc_storable_picture (FRAME, img->width, img->height, img->width_cr, img->height_cr);

            missingpoc = pocs_in_dpb[i] + img->poc_gap;
            // Diagnostics
            // printf("\n missingpoc = %d\n",missingpoc);

            if(missingpoc > img->earlier_missing_poc)
            {
                img->earlier_missing_poc = missingpoc;
                conceal_to_picture->top_poc= missingpoc;
                conceal_to_picture->bottom_poc=missingpoc;
                conceal_to_picture->frame_poc=missingpoc;
                conceal_to_picture->poc=missingpoc;
                conceal_from_picture = get_pic_from_dpb(missingpoc, &pos);

                assert(conceal_from_picture != NULL);

                dpb.used_size = pos+1;

                img->frame_to_conceal = conceal_from_picture->frame_num + 1;

                update_ref_list_for_concealment();
                img->conceal_slice_type = B_SLICE;
                copy_to_conceal(conceal_from_picture, conceal_to_picture, img);
                concealment_ptr = init_node( conceal_to_picture, missingpoc );
                add_node(concealment_ptr);
                // Diagnostics
                // print_node(concealment_ptr);
            }
        }
    }

    //restore the original value
    //dpb.used_size = dpb.size;
    dpb.used_size = temp_used_size;
}

/*!
************************************************************************
* \brief
* Perform Sliding window decoded reference picture marking process. It
* maintains the POC s stored in the dpb at a specific instance.
*
************************************************************************
*/

void sliding_window_poc_management(StorablePicture *p)
{
    unsigned int i;

    if (dpb.used_size == dpb.size)
    {
        for(i=0;i<dpb.size-1; i++)
            pocs_in_dpb[i] = pocs_in_dpb[i+1];
    }

//    pocs_in_dpb[dpb.used_size-1] = p->poc;
}


/*!
************************************************************************
* \brief
* Outputs the non reference frames. The POCs in the concealment buffer are
* sorted in ascending order and outputted when the lowest POC in the
* concealment buffer is lower than the lowest in the dpb. The linked list
* entry corresponding to the outputted POC is immediately deleted.
*
************************************************************************
*/

void write_lost_non_ref_pic(int poc, int p_out)
{
    FrameStore concealment_fs;
    if(poc > 0)
    {
        if((poc - dpb.last_output_poc) > img->poc_gap)
        {

            concealment_fs.frame = concealment_head->picture;
            concealment_fs.is_output = 0;
            concealment_fs.is_reference = 0;
            concealment_fs.is_used = 3;

            write_stored_frame(&concealment_fs, p_out);
            delete_node(concealment_head);
        }
    }
}

/*!
************************************************************************
* \brief
* Conceals frame loss immediately after the IDR. This special case produces
* the same result for either frame copy or motion vector copy concealment.
*
************************************************************************
*/

void write_lost_ref_after_idr(int pos)
{
    int temp = 1;

    if(last_out_fs->frame == NULL)
    {
        last_out_fs->frame = alloc_storable_picture (FRAME, img->width, img->height,
            img->width_cr, img->height_cr);
        last_out_fs->is_used = 3;
    }

    if(img->conceal_mode == 2)
    {
        temp = 2;
        img->conceal_mode = 1;
    }
    copy_to_conceal(dpb.fs[pos]->frame, last_out_fs->frame, img);

    img->conceal_mode = temp;
}

