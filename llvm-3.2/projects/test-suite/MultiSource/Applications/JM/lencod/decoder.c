
/*!
 *************************************************************************************
 * \file decoder.c
 *
 * \brief
 *    Contains functions that implement the "decoders in the encoder" concept for the
 *    rate-distortion optimization with losses.
 * \date
 *    October 22nd, 2001
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and
 *    affiliation details)
 *    - Dimitrios Kontopodis                    <dkonto@eikon.tum.de>
 *************************************************************************************
 */

#include <stdlib.h>
#include <memory.h>

#include "global.h"
#include "refbuf.h"
#include "image.h"

/*!
 *************************************************************************************
 * \brief
 *    decodes one 8x8 partition
 *
 * \note
 *    Gives the expected value in the decoder of one 8x8 block. This is done based on the
 *    stored reconstructed residue decs->resY[][], the reconstructed values imgY[][]
 *    and the motion vectors. The decoded 8x8 block is moved to decs->decY[][].
 *************************************************************************************
 */
void decode_one_b8block (int decoder, int mbmode, int b8block, int b8mode, int b8ref)
{
  int i,j,block_y,block_x,bx,by;
  int ref_inx = (IMG_NUMBER-1)%img->num_ref_frames;

  int mv[2][BLOCK_MULTIPLE][BLOCK_MULTIPLE];
  int resY_tmp[MB_BLOCK_SIZE][MB_BLOCK_SIZE];

  int i0 = (b8block%2)<<3,   i1 = i0+8,   bx0 = i0>>2,   bx1 = bx0+2;
  int j0 = (b8block/2)<<3,   j1 = j0+8,   by0 = j0>>2,   by1 = by0+2;

  if (img->type==I_SLICE)
  {
    for(i=i0;i<i1;i++)
    for(j=j0;j<j1;j++)
    {
      decs->decY[decoder][img->pix_y+j][img->pix_x+i]=enc_picture->imgY[img->pix_y+j][img->pix_x+i];
    }
  }
  else
  {
    if (mbmode==0 && (img->type==P_SLICE || (img->type==B_SLICE && img->nal_reference_idc>0)))
    {
      for(i=i0;i<i1;i++)
      for(j=j0;j<j1;j++)
      {
        resY_tmp[j][i]=0;
      }
      for (by=by0; by<by1; by++)
      for (bx=bx0; bx<bx1; bx++)
      {
        mv[0][by][bx] = mv[1][by][bx] = 0;
      }
    }
    else
    {
      if (b8mode>=1 && b8mode<=7)
      {
        for (by=by0; by<by1; by++)
        for (bx=bx0; bx<bx1; bx++)
        {
          mv[0][by][bx] = img->all_mv[by][bx][LIST_0][b8ref][b8mode][0];
          mv[1][by][bx] = img->all_mv[by][bx][LIST_0][b8ref][b8mode][1];
        }
      }
      else
      {
        for (by=by0; by<by1; by++)
        for (bx=bx0; bx<bx1; bx++)
        {
          mv[0][by][bx] = mv[1][by][bx] = 0;
        }
      }

      for(i=i0;i<i1;i++)
      for(j=j0;j<j1;j++)
      {
        resY_tmp[j][i]=decs->resY[j][i];
      }
    }

    // Decode Luminance
    if ((b8mode>=1 && b8mode<=7) || (mbmode==0 && (img->type==P_SLICE || (img->type==B_SLICE && img->nal_reference_idc>0))))
    {
      for (by=by0; by<by1; by++)
      for (bx=bx0; bx<bx1; bx++)
      {
        block_x = img->block_x+bx;
        block_y = img->block_y+by;
        if (img->type == B_SLICE && enc_picture != enc_frame_picture)
          ref_inx = (IMG_NUMBER-b8ref-2)%img->num_ref_frames;

        Get_Reference_Block (decs->decref[decoder][ref_inx],
                             block_y, block_x,
                             mv[0][by][bx],
                             mv[1][by][bx],
                             decs->RefBlock);
        for (j=0; j<4; j++)
        for (i=0; i<4; i++)
        {
          decs->decY[decoder][block_y*4+j][block_x*4+i] = resY_tmp[by*4+j][bx*4+i] + decs->RefBlock[j][i];
        }
      }
    }
    else
    {
      // Intra Refresh - Assume no spatial prediction
      for(i=i0;i<i1;i++)
      for(j=j0;j<j1;j++)
      {
        decs->decY[decoder][img->pix_y+j][img->pix_x+i] = enc_picture->imgY[img->pix_y+j][img->pix_x+i];
      }
    }
  }
}


/*!
 *************************************************************************************
 * \brief
 *    decodes one macroblock
 *************************************************************************************
 */
void decode_one_mb (int decoder, Macroblock* currMB)
{
  decode_one_b8block (decoder, currMB->mb_type, 0, currMB->b8mode[0], enc_picture->ref_idx[LIST_0][img->block_y+0][img->block_x+0]);//refFrArr[img->block_y+0][img->block_x+0]);
  decode_one_b8block (decoder, currMB->mb_type, 1, currMB->b8mode[1], enc_picture->ref_idx[LIST_0][img->block_y+0][img->block_x+2]);//refFrArr[img->block_y+0][img->block_x+2]);
  decode_one_b8block (decoder, currMB->mb_type, 2, currMB->b8mode[2], enc_picture->ref_idx[LIST_0][img->block_y+2][img->block_x+0]);//refFrArr[img->block_y+2][img->block_x+0]);
  decode_one_b8block (decoder, currMB->mb_type, 3, currMB->b8mode[3], enc_picture->ref_idx[LIST_0][img->block_y+2][img->block_x+2]);//refFrArr[img->block_y+2][img->block_x+2]);
}

/*!
 *************************************************************************************
 * \brief
 *    Finds the reference MB given the decoded reference frame
 * \note
 *    This is based on the function UnifiedOneForthPix, only it is modified to
 *    be used at the "many decoders in the encoder" RD optimization. In this case
 *    we dont want to keep full upsampled reference frames for all decoders, so
 *    we just upsample when it is necessary.
 * \param imY
 *    The frame to be upsampled
 * \param block_y
 *    The row of the block, whose prediction we want to find
 * \param block_x
 *    The column of the block, whose prediction we want to track
 * \param mvhor
 *    Motion vector, horizontal part
 * \param mvver
 *    Motion vector, vertical part
 * \param out
 *    Output: The prediction for the block (block_y, block_x)
 *************************************************************************************
 */
void Get_Reference_Block(imgpel **imY,
                         int block_y,
                         int block_x,
                         int mvhor,
                         int mvver,
                         imgpel **out)
{
  int i,j,y,x;

  y = block_y * BLOCK_SIZE * 4 + mvver;
  x = block_x * BLOCK_SIZE * 4 + mvhor;

  for (j=0; j<BLOCK_SIZE; j++)
    for (i=0; i<BLOCK_SIZE; i++)
      out[j][i] = Get_Reference_Pixel(imY, y+j*4, x+i*4);
}

/*!
 *************************************************************************************
 * \brief
 *    Finds a pixel (y,x) of the upsampled reference frame
 * \note
 *    This is based on the function UnifiedOneForthPix, only it is modified to
 *    be used at the "many decoders in the encoder" RD optimization. In this case
 *    we dont want to keep full upsampled reference frames for all decoders, so
 *    we just upsample when it is necessary.
 *************************************************************************************
 */
byte Get_Reference_Pixel(imgpel **imY, int y_pos, int x_pos)
{

  int dx, x;
  int dy, y;
  int maxold_x,maxold_y;

  int result = 0, result1, result2;
  int pres_x;
  int pres_y;

  int tmp_res[6];

  static const int COEF[6] = {
    1, -5, 20, 20, -5, 1
  };


  dx = x_pos&3;
  dy = y_pos&3;
  x_pos = (x_pos-dx)/4;
  y_pos = (y_pos-dy)/4;
  maxold_x = img->width-1;
  maxold_y = img->height-1;

  if (dx == 0 && dy == 0) { /* fullpel position */
    result = imY[iClip3(0,maxold_y,y_pos)][iClip3(0,maxold_x,x_pos)];
  }
  else { /* other positions */

    if (dy == 0) {

      pres_y = iClip3(0,maxold_y,y_pos);
      for(x=-2;x<4;x++) {
        pres_x = iClip3(0,maxold_x,x_pos+x);
        result += imY[pres_y][pres_x]*COEF[x+2];
      }

      result = iClip3(0, img->max_imgpel_value, (result+16)/32);

      if (dx == 1) {
        result = (result + imY[pres_y][iClip3(0,maxold_x,x_pos)])/2;
      }
      else if (dx == 3) {
        result = (result + imY[pres_y][iClip3(0,maxold_x,x_pos+1)])/2;
      }
    }
    else if (dx == 0) {

      pres_x = iClip3(0,maxold_x,x_pos);
      for(y=-2;y<4;y++) {
        pres_y = iClip3(0,maxold_y,y_pos+y);
        result += imY[pres_y][pres_x]*COEF[y+2];
      }

      result = iClip3(0, img->max_imgpel_value, (result+16)/32);

      if (dy == 1) {
        result = (result + imY[iClip3(0,maxold_y,y_pos)][pres_x])/2;
      }
      else if (dy == 3) {
        result = (result + imY[iClip3(0,maxold_y,y_pos+1)][pres_x])/2;
      }
    }
    else if (dx == 2) {

      for(y=-2;y<4;y++) {
        result = 0;
        pres_y = iClip3(0,maxold_y,y_pos+y);
        for(x=-2;x<4;x++) {
          pres_x = iClip3(0,maxold_x,x_pos+x);
          result += imY[pres_y][pres_x]*COEF[x+2];
        }
        tmp_res[y+2] = result;
      }

      result = 0;
      for(y=-2;y<4;y++) {
        result += tmp_res[y+2]*COEF[y+2];
      }

      result = iClip3(0, img->max_imgpel_value, (result+512)/1024);

      if (dy == 1) {
        result = (result + iClip3(0, img->max_imgpel_value, (tmp_res[2]+16)/32))/2;
      }
      else if (dy == 3) {
        result = (result + iClip3(0, img->max_imgpel_value, (tmp_res[3]+16)/32))/2;
      }
    }
    else if (dy == 2) {

      for(x=-2;x<4;x++) {
        result = 0;
        pres_x = iClip3(0,maxold_x,x_pos+x);
        for(y=-2;y<4;y++) {
          pres_y = iClip3(0,maxold_y,y_pos+y);
          result += imY[pres_y][pres_x]*COEF[y+2];
        }
        tmp_res[x+2] = result;
      }

      result = 0;
      for(x=-2;x<4;x++) {
        result += tmp_res[x+2]*COEF[x+2];
      }

      result = iClip3(0, img->max_imgpel_value, (result+512)/1024);

      if (dx == 1) {
        result = (result + iClip3(0, img->max_imgpel_value, (tmp_res[2]+16)/32))/2;
      }
      else {
        result = (result + iClip3(0, img->max_imgpel_value, (tmp_res[3]+16)/32))/2;
      }
    }
    else {

      result = 0;
      pres_y = dy == 1 ? y_pos : y_pos+1;
      pres_y = iClip3(0,maxold_y,pres_y);

      for(x=-2;x<4;x++) {
        pres_x = iClip3(0,maxold_x,x_pos+x);
        result += imY[pres_y][pres_x]*COEF[x+2];
      }

      result1 = iClip3(0, img->max_imgpel_value, (result+16)/32);

      result = 0;
      pres_x = dx == 1 ? x_pos : x_pos+1;
      pres_x = iClip3(0,maxold_x,pres_x);

      for(y=-2;y<4;y++) {
        pres_y = iClip3(0,maxold_y,y_pos+y);
        result += imY[pres_y][pres_x]*COEF[y+2];
      }

      result2 = iClip3(0, img->max_imgpel_value, (result+16)/32);
      result = (result1+result2)/2;
    }
  }

  return result;
}

/*!
 *************************************************************************************
 * \brief
 *    Performs the simulation of the packet losses, calls the error concealment funcs
 *    and copies the decoded images to the reference frame buffers of the decoders
 *
 *************************************************************************************
 */
void UpdateDecoders()
{
  int k;
  for (k=0; k<input->NoOfDecoders; k++)
  {
    Build_Status_Map(decs->status_map); // simulates the packet losses
    Error_Concealment(decs->decY_best[k], decs->status_map, decs->decref[k]); // for the moment error concealment is just a "copy"
    // Move decoded frames to reference buffers: (at the decoders this is done
    // without interpolation (upsampling) - upsampling is done while decoding
    DecOneForthPix(decs->decY_best[k], decs->decref[k]);
  }
}
/*!
 *************************************************************************************
 * \brief
 *    Copies one (reconstructed) image to the respective reference frame buffer
 *
 * \note
 *    This is used at the "many decoders in the encoder"
 * \param dY
 *    The reconstructed image
 * \param dref
 *    The reference buffer
 *************************************************************************************
 */
void DecOneForthPix(imgpel **dY, imgpel ***dref)
{
  int j, ref=IMG_NUMBER%img->buf_cycle;

  for (j=0; j<img->height; j++)
    memcpy(dref[ref][j], dY[j], img->width*sizeof(imgpel));
}

/*!
 *************************************************************************************
 * \brief
 *    Gives the prediction residue for a 8x8 block
 *************************************************************************************
 */
void compute_residue_b8block (int b8block, int i16mode) // if not INTRA16x16 it has to be -1
{
  int i,j;
  int i0 = (b8block%2)<<3,   i1 = i0+8;
  int j0 = (b8block/2)<<3,   j1 = j0+8;

  if (i16mode>=0)
  {
    for (i=i0; i<i1; i++)
    for (j=j0; j<j1; j++)
    {
      decs->resY[j][i] = enc_picture->imgY[img->pix_y+j][img->pix_x+i] - img->mprr_2[i16mode][j][i];
    }
  }
  else
  {
    for (i=i0; i<i1; i++)
    for (j=j0; j<j1; j++)
    {
      decs->resY[j][i] = enc_picture->imgY[img->pix_y+j][img->pix_x+i] - img->mpr[j][i];
    }
  }
}

/*!
 *************************************************************************************
 * \brief
 *    Gives the prediction residue for a macroblock
 *************************************************************************************
 */
void compute_residue_mb (int i16mode)
{
  compute_residue_b8block (0, i16mode);
  compute_residue_b8block (1, i16mode);
  compute_residue_b8block (2, i16mode);
  compute_residue_b8block (3, i16mode);
}


/*!
 *************************************************************************************
 * \brief
 *    Builds a random status map showing whether each MB is received or lost, based
 *    on the packet loss rate and the slice structure.
 *
 * \param s_map
 *    The status map to be filled
 *************************************************************************************
 */
void Build_Status_Map(byte **s_map)
{
  int i,j,slice=-1,mb=0,jj,ii,packet_lost=0;

  jj = img->height/MB_BLOCK_SIZE;
  ii = img->width/MB_BLOCK_SIZE;

  for (j=0 ; j<jj; j++)
  for (i=0 ; i<ii; i++)
  {
    if (!input->slice_mode || img->mb_data[mb].slice_nr != slice) /* new slice */
    {
      packet_lost=0;
      if ((double)rand()/(double)RAND_MAX*100 < input->LossRateC)   packet_lost += 3;
      if ((double)rand()/(double)RAND_MAX*100 < input->LossRateB)   packet_lost += 2;
      if ((double)rand()/(double)RAND_MAX*100 < input->LossRateA)   packet_lost  = 1;
      slice++;
    }
    if (!packet_lost)
    {
      s_map[j][i]=0;  //! Packet OK
    }
    else
    {
      s_map[j][i]=packet_lost;
      if(input->partition_mode == 0)  s_map[j][i]=1;
    }
    mb++;
  }
}

/*!
 *************************************************************************************
 * \brief
 *    Performs some sort of error concealment for the areas that are lost according
 *    to the status_map
 *
 * \param inY
 *    Error concealment is performed on this frame imY[][]
 * \param s_map
 *    The status map shows which areas are lost.
 * \param refY
 *    The set of reference frames - may be used for the error concealment.
 *************************************************************************************
 */
void Error_Concealment(imgpel **inY, byte **s_map, imgpel ***refY)
{
  int mb_y, mb_x, mb_h, mb_w;
  mb_h = img->height/MB_BLOCK_SIZE;
  mb_w = img->width/MB_BLOCK_SIZE;

  for (mb_y=0; mb_y < mb_h; mb_y++)
  for (mb_x=0; mb_x < mb_w; mb_x++)
  {
    if (s_map[mb_y][mb_x])   Conceal_Error(inY, mb_y, mb_x, refY, s_map);
  }
}

/*!
 *************************************************************************************
 * \brief
 *    Copies a certain MB (mb_y,mb_x) of the frame inY[][] from the previous frame.
 *    For the time there is no better EC...
 *************************************************************************************
 */
void Conceal_Error(imgpel **inY, int mb_y, int mb_x, imgpel ***refY, byte **s_map)
{
  int i,j,block_x, block_y;
  int ref_inx = (IMG_NUMBER-1)%img->num_ref_frames;
  int pos_y = mb_y*MB_BLOCK_SIZE, pos_x = mb_x*MB_BLOCK_SIZE;
  int mv[2][BLOCK_MULTIPLE][BLOCK_MULTIPLE];
  int resY[MB_BLOCK_SIZE][MB_BLOCK_SIZE];
  int copy  = (decs->dec_mb_mode[mb_y][mb_x]==0 && (img->type==P_SLICE || (img->type==B_SLICE && img->nal_reference_idc>0)));
  int inter = (((decs->dec_mb_mode[mb_y][mb_x]>=1 && decs->dec_mb_mode[mb_y][mb_x]<=3) || decs->dec_mb_mode[mb_y][mb_x]==P8x8) && (img->type==P_SLICE || (img->type==B_SLICE && img->nal_reference_idc>0)));
  short ***tmp_mv = enc_picture->mv[LIST_0];

  switch(s_map[mb_y][mb_x])
  {
  case 1: //! whole slice lost (at least partition A lost)
    if (img->type!=I_SLICE)
    {
      for (j=0;j<MB_BLOCK_SIZE;j++)
        for (i=0;i<MB_BLOCK_SIZE;i++)
          inY[pos_y+j][pos_x+i] = refY[ref_inx][pos_y+j][pos_x+i];
    }
    else
    {
      for (j=0;j<MB_BLOCK_SIZE;j++)
        for (i=0;i<MB_BLOCK_SIZE;i++)
          inY[pos_y+j][pos_x+i] = 127;
    }
    break;
  case 5: //! partition B and partition C lost

    //! Copy motion vectors
    for (block_y=0; block_y<BLOCK_MULTIPLE; block_y++)
      for (block_x=0; block_x<BLOCK_MULTIPLE; block_x++)
        for (i=0;i<2;i++)
          mv[i][block_y][block_x]=tmp_mv[mb_y*BLOCK_SIZE+block_y][mb_x*BLOCK_SIZE+block_x+4][i];

    //! Residue ist set to zero
    for(i=0;i<MB_BLOCK_SIZE;i++)
      for(j=0;j<MB_BLOCK_SIZE;j++)
        resY[j][i]=0;

    //! not first frame
    if (img->type!=I_SLICE)
    {
      //! if copy mb
      if (copy)
      {
        for (j=0;j<MB_BLOCK_SIZE;j++)
          for (i=0;i<MB_BLOCK_SIZE;i++)
            inY[pos_y+j][pos_x+i] = refY[ref_inx][pos_y+j][pos_x+i];
      }
      //! if inter mb
      else if (inter)
      {
        for (block_y = mb_y*BLOCK_SIZE ; block_y < (mb_y*BLOCK_SIZE + BLOCK_MULTIPLE) ; block_y++)
          for (block_x = mb_x*BLOCK_SIZE ; block_x < (mb_x*BLOCK_SIZE + BLOCK_MULTIPLE) ; block_x++)
          {
            Get_Reference_Block(refY[ref_inx],
                                block_y, block_x,
                                mv[0][block_y - mb_y*BLOCK_SIZE][block_x - mb_x*BLOCK_SIZE],
                                mv[1][block_y - mb_y*BLOCK_SIZE][block_x - mb_x*BLOCK_SIZE],
                                decs->RefBlock);
            for (j=0;j<BLOCK_SIZE;j++)
              for (i=0;i<BLOCK_SIZE;i++)
              {
                inY[block_y*BLOCK_SIZE + j][block_x*BLOCK_SIZE + i] = decs->RefBlock[j][i];
              }
          }
      }
      else //intra; up to now only copy mb, may integrate nokia EC
      {
        for (j=0;j<MB_BLOCK_SIZE;j++)
          for (i=0;i<MB_BLOCK_SIZE;i++)
            inY[pos_y+j][pos_x+i] = refY[ref_inx][pos_y+j][pos_x+i];
      }
    }
    else //! first frame; up to now set value to grey, may integrate nokia EC
    {
      for (j=0;j<MB_BLOCK_SIZE;j++)
        for (i=0;i<MB_BLOCK_SIZE;i++)
          inY[pos_y+j][pos_x+i] = 127;
    }
    break;
  case 3: //! Partition C lost
    if(img->type!=I_SLICE)
    {
      //! Copy motion vectors
      for (block_y=0; block_y<BLOCK_MULTIPLE; block_y++)
        for (block_x=0; block_x<BLOCK_MULTIPLE; block_x++)
          for (i=0;i<2;i++)
            mv[i][block_y][block_x]=tmp_mv[mb_y*BLOCK_SIZE+block_y][mb_x*BLOCK_SIZE+block_x+4][i];

      //! Residue ist set to zero
      for(i=0;i<MB_BLOCK_SIZE;i++)
        for(j=0;j<MB_BLOCK_SIZE;j++)
          resY[j][i]=0;

      //! if copy mb
      if (copy)
      {
        for (j=0;j<MB_BLOCK_SIZE;j++)
          for (i=0;i<MB_BLOCK_SIZE;i++)
            inY[pos_y+j][pos_x+i] = refY[ref_inx][pos_y+j][pos_x+i];
      }
      //! if inter mb
      else if (inter)
      {
        for (block_y = mb_y*BLOCK_SIZE ; block_y < (mb_y*BLOCK_SIZE + BLOCK_MULTIPLE) ; block_y++)
          for (block_x = mb_x*BLOCK_SIZE ; block_x < (mb_x*BLOCK_SIZE + BLOCK_MULTIPLE) ; block_x++)
            {
              Get_Reference_Block(refY[ref_inx],
                                  block_y, block_x,
                                  mv[0][block_y - mb_y*BLOCK_SIZE][block_x - mb_x*BLOCK_SIZE],
                                  mv[1][block_y - mb_y*BLOCK_SIZE][block_x - mb_x*BLOCK_SIZE],
                                  decs->RefBlock);
              for (j=0;j<BLOCK_SIZE;j++)
                for (i=0;i<BLOCK_SIZE;i++)
                {
                  inY[block_y*BLOCK_SIZE + j][block_x*BLOCK_SIZE + i] = decs->RefBlock[j][i];
                }
            }
      }
    }
    break;
  case 2: //! Partition B lost
    if(img->type!=I_SLICE)
    {
      if(!inter)
      {
        for (j=0;j<MB_BLOCK_SIZE;j++)
          for (i=0;i<MB_BLOCK_SIZE;i++)
            inY[pos_y+j][pos_x+i] = refY[ref_inx][pos_y+j][pos_x+i];
      }
    }
    else //! first frame; up to now set value to grey, may integrate nokia EC
    {
      for (j=0;j<MB_BLOCK_SIZE;j++)
        for (i=0;i<MB_BLOCK_SIZE;i++)
          inY[pos_y+j][pos_x+i] = 127;
    }
    break;
  } //! End Switch
}
