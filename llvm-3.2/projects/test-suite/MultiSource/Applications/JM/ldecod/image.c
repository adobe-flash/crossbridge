
/*!
 ***********************************************************************
 * \file image.c
 *
 * \brief
 *    Decode a Slice
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *    - Inge Lille-Langoy               <inge.lille-langoy@telenor.com>
 *    - Rickard Sjoberg                 <rickard.sjoberg@era.ericsson.se>
 *    - Jani Lainema                    <jani.lainema@nokia.com>
 *    - Sebastian Purreiter             <sebastian.purreiter@mch.siemens.de>
 *    - Byeong-Moon Jeon                <jeonbm@lge.com>
 *    - Thomas Wedi                     <wedi@tnt.uni-hannover.de>
 *    - Gabi Blaettermann               <blaetter@hhi.de>
 *    - Ye-Kui Wang                     <wyk@ieee.org>
 *    - Antti Hallapuro                 <antti.hallapuro@nokia.com>
 *    - Alexis Tourapis                 <alexismt@ieee.org>
 *    - Jill Boyce                      <jill.boyce@thomson.net>
 *    - Saurav K Bandyopadhyay          <saurav@ieee.org>
 *    - Zhenyu Wu                       <Zhenyu.Wu@thomson.net
 *    - Purvin Pandit                   <Purvin.Pandit@thomson.net>
 *
 ***********************************************************************
 */

#include "contributors.h"

#include <math.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include "global.h"
#include "errorconcealment.h"
#include "image.h"
#include "mbuffer.h"
#include "fmo.h"
#include "nalu.h"
#include "parsetcommon.h"
#include "parset.h"
#include "header.h"
#include "rtp.h"
#include "sei.h"
#include "output.h"
#include "biaridecod.h"
#include "mb_access.h"
#include "memalloc.h"
#include "annexb.h"

#include "context_ini.h"
#include "cabac.h"
#include "loopfilter.h"

#include "vlc.h"

#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__APPLE__)
#include <sys/time.h>
int ftime(struct timeb *tp)
{ 
  struct timeval tv;
  struct timezone tz;
  gettimeofday(&tv, &tz);
  tp->time = tv.tv_sec;
  tp->millitm = tv.tv_usec / 1000;
  tp->timezone = tz.tz_minuteswest;
  tp->dstflag = tz.tz_dsttime;
}
#endif

#include "erc_api.h"
extern objectBuffer_t *erc_object_list;
extern ercVariables_t *erc_errorVar;
extern frame erc_recfr;
extern int erc_mvperMB;
extern struct img_par *erc_img;

//extern FILE *p_out2;

extern StorablePicture **listX[6];
extern ColocatedParams *Co_located;

extern StorablePicture *no_reference_picture;
int non_conforming_stream;

StorablePicture *dec_picture;

OldSliceParams old_slice;

void MbAffPostProc()
{
  imgpel temp[16][32];

  imgpel ** imgY  = dec_picture->imgY;
  imgpel ***imgUV = dec_picture->imgUV;

  int i, x, y, x0, y0, uv;
  for (i=0; i<(int)dec_picture->PicSizeInMbs; i+=2)
  {
    if (dec_picture->mb_field[i])
    {
      get_mb_pos(i, &x0, &y0, IS_LUMA);
      for (y=0; y<(2*MB_BLOCK_SIZE);y++)
        for (x=0; x<MB_BLOCK_SIZE; x++)
          temp[x][y] = imgY[y0+y][x0+x];

      for (y=0; y<MB_BLOCK_SIZE;y++)
        for (x=0; x<MB_BLOCK_SIZE; x++)
        {
          imgY[y0+(2*y)][x0+x]   = temp[x][y];
          imgY[y0+(2*y+1)][x0+x] = temp[x][y+MB_BLOCK_SIZE];
        }

      if (dec_picture->chroma_format_idc != YUV400)
      {
        x0 = x0 / (16/img->mb_cr_size_x);
        y0 = y0 / (16/img->mb_cr_size_y);

        for (uv=0; uv<2; uv++)
        {
          for (y=0; y<(2*img->mb_cr_size_y);y++)
            for (x=0; x<img->mb_cr_size_x; x++)
              temp[x][y] = imgUV[uv][y0+y][x0+x];

          for (y=0; y<img->mb_cr_size_y;y++)
            for (x=0; x<img->mb_cr_size_x; x++)
            {
              imgUV[uv][y0+(2*y)][x0+x]   = temp[x][y];
              imgUV[uv][y0+(2*y+1)][x0+x] = temp[x][y+img->mb_cr_size_y];
            }
        }
      }
    }
  }
}

/*!
 ***********************************************************************
 * \brief
 *    decodes one I- or P-frame
 *
 ***********************************************************************
 */

int decode_one_frame(struct img_par *img,struct inp_par *inp, struct snr_par *snr)
{
  int current_header;
  Slice *currSlice = img->currentSlice;
  int i;

  img->current_slice_nr = 0;
  img->current_mb_nr = -4711;     // initialized to an impossible value for debugging -- correct value is taken from slice header
  currSlice->next_header = -8888; // initialized to an impossible value for debugging -- correct value is taken from slice header
  img->num_dec_mb = 0;
  img->newframe = 1;

  while ((currSlice->next_header != EOS && currSlice->next_header != SOP))
  {
    current_header = read_new_slice();

    // error tracking of primary and redundant slices.
    Error_tracking();

    // If primary and redundant are received and primary is correct, discard the redundant
    // else, primary slice will be replaced with redundant slice.
    if(img->frame_num == previous_frame_num && img->redundant_pic_cnt !=0
      && Is_primary_correct !=0 && current_header != EOS)
    {
      continue;
    }

    // update reference flags and set current ref_flag
    if(!(img->redundant_pic_cnt != 0 && previous_frame_num == img->frame_num))
    {
      for(i=16;i>0;i--)
      {
        ref_flag[i] = ref_flag[i-1];
      }
    }
    ref_flag[0] = img->redundant_pic_cnt==0 ? Is_primary_correct : Is_redundant_correct;
    previous_frame_num = img->frame_num;

    if (current_header == EOS)
    {
      exit_picture();
      return EOS;
    }

    decode_slice(img, inp, current_header);

    img->newframe = 0;
    img->current_slice_nr++;
  }

  exit_picture();

  return (SOP);
}


/*!
 ************************************************************************
 * \brief
 *    Convert file read buffer to source picture structure
 * \param imgX
 *    Pointer to image plane
 * \param buf
 *    Buffer for file output
 * \param size_x
 *    horizontal image size in pixel
 * \param size_y
 *    vertical image size in pixel
 * \param symbol_size_in_bytes
 *    number of bytes used per pel
 ************************************************************************
 */
void buf2img (imgpel** imgX, unsigned char* buf, int size_x, int size_y, int symbol_size_in_bytes)
{
  int i,j;

  unsigned short tmp16, ui16;
  unsigned long  tmp32, ui32;

  if (symbol_size_in_bytes> sizeof(imgpel))
  {
    error ("Source picture has higher bit depth than imgpel data type. Please recompile with larger data type for imgpel.", 500);
  }

  if (( sizeof(char) == sizeof (imgpel)) && ( sizeof(char) == symbol_size_in_bytes))
  {
    // imgpel == pixel_in_file == 1 byte -> simple copy
    for(j=0;j<size_y;j++)
      memcpy(&imgX[j][0], buf+j*size_x, size_x);
  }
  else
  {
    // sizeof (imgpel) > sizeof(char)
    if (testEndian())
    {
      // big endian
      switch (symbol_size_in_bytes)
      {
      case 1:
        {
          for(j=0;j<size_y;j++)
            for(i=0;i<size_x;i++)
            {
              imgX[j][i]= buf[i+j*size_x];
            }
          break;
        }
      case 2:
        {
          for(j=0;j<size_y;j++)
            for(i=0;i<size_x;i++)
            {
              memcpy(&tmp16, buf+((i+j*size_x)*2), 2);
              ui16  = (tmp16 >> 8) | ((tmp16&0xFF)<<8);
              imgX[j][i] = (imgpel) ui16;
            }
          break;
        }
      case 4:
        {
          for(j=0;j<size_y;j++)
            for(i=0;i<size_x;i++)
            {
              memcpy(&tmp32, buf+((i+j*size_x)*4), 4);
              ui32  = ((tmp32&0xFF00)<<8) | ((tmp32&0xFF)<<24) | ((tmp32&0xFF0000)>>8) | ((tmp32&0xFF000000)>>24);
              imgX[j][i] = (imgpel) ui32;
            }
        }
      default:
        {
           error ("reading only from formats of 8, 16 or 32 bit allowed on big endian architecture", 500);
           break;
        }
      }

    }
    else
    {
      // little endian
      if (symbol_size_in_bytes == 1)
      {
        for (j=0; j < size_y; j++)
        {
          for (i=0; i < size_x; i++)
          {
            imgX[j][i]=*(buf++);
          }
        }
      }
      else
      {
        for (j=0; j < size_y; j++)
        {
          int jpos = j*size_x;
          for (i=0; i < size_x; i++)
          {
            imgX[j][i]=0;
            memcpy(&(imgX[j][i]), buf +((i+jpos)*symbol_size_in_bytes), symbol_size_in_bytes);
          }
        }
      }

    }
  }
}


/*!
************************************************************************
* \brief
*    Find PSNR for all three components.Compare decoded frame with
*    the original sequence. Read inp->jumpd frames to reflect frame skipping.
************************************************************************
*/
void find_snr(
              struct snr_par  *snr,   //!< pointer to snr parameters
              StorablePicture *p,     //!< picture to be compared
              int p_ref)              //!< open reference YUV file
{
  static const int SubWidthC  [4]= { 1, 2, 2, 1};
  static const int SubHeightC [4]= { 1, 2, 1, 1};
  int crop_left, crop_right, crop_top, crop_bottom;

  int i,j;
  int64 diff_y,diff_u,diff_v;
  int uv;
  int64  status;
  int symbol_size_in_bytes = img->pic_unit_bitsize_on_disk/8;
  int size_x, size_y;
  int size_x_cr, size_y_cr;
  int64 framesize_in_bytes;
  unsigned int max_pix_value_sqd = img->max_imgpel_value * img->max_imgpel_value;
  unsigned int max_pix_value_sqd_uv = img->max_imgpel_value_uv * img->max_imgpel_value_uv;
  Boolean rgb_output = (Boolean) (active_sps->vui_seq_parameters.matrix_coefficients==0);
  unsigned char *buf;

  // picture error concealment
  char yuv_types[4][6]= {"4:0:0","4:2:0","4:2:2","4:4:4"};

  // calculate frame number
  int  psnrPOC = active_sps->mb_adaptive_frame_field_flag ? p->poc /(input->poc_scale) : p->poc/(input->poc_scale);

  // cropping for luma
  if (p->frame_cropping_flag)
  {
    crop_left   = SubWidthC[p->chroma_format_idc] * p->frame_cropping_rect_left_offset;
    crop_right  = SubWidthC[p->chroma_format_idc] * p->frame_cropping_rect_right_offset;
    crop_top    = SubHeightC[p->chroma_format_idc]*( 2 - p->frame_mbs_only_flag ) *  p->frame_cropping_rect_top_offset;
    crop_bottom = SubHeightC[p->chroma_format_idc]*( 2 - p->frame_mbs_only_flag ) *  p->frame_cropping_rect_bottom_offset;
  }
  else
  {
    crop_left = crop_right = crop_top = crop_bottom = 0;
  }

  size_x = p->size_x - crop_left - crop_right;
  size_y = p->size_y - crop_top - crop_bottom;

  // cropping for chroma
  if (p->frame_cropping_flag)
  {
    crop_left   = p->frame_cropping_rect_left_offset;
    crop_right  = p->frame_cropping_rect_right_offset;
    crop_top    = ( 2 - p->frame_mbs_only_flag ) *  p->frame_cropping_rect_top_offset;
    crop_bottom = ( 2 - p->frame_mbs_only_flag ) *   p->frame_cropping_rect_bottom_offset;
  }
  else
  {
    crop_left = crop_right = crop_top = crop_bottom = 0;
  }

  if ((p->chroma_format_idc==YUV400) && input->write_uv)
  {
    size_x_cr = p->size_x/2;
    size_y_cr = p->size_y/2;
  }
  else
  {
    size_x_cr = p->size_x_cr - crop_left - crop_right;
    size_y_cr = p->size_y_cr - crop_top  - crop_bottom;
  }

  framesize_in_bytes = (((int64)size_y*size_x) + ((int64)size_y_cr*size_x_cr)*2) * symbol_size_in_bytes;

  if (psnrPOC==0 && img->psnr_number)
    img->idr_psnr_number = img->number*img->ref_poc_gap/(input->poc_scale);

  img->psnr_number=imax(img->psnr_number,img->idr_psnr_number+psnrPOC);

  frame_no = img->idr_psnr_number+psnrPOC;

  // KS: this buffer should actually be allocated only once, but this is still much faster than the previous version
  buf = malloc ( size_y * size_x * symbol_size_in_bytes );

  if (NULL == buf)
  {
    no_mem_exit("find_snr: buf");
  }

  status = lseek (p_ref, framesize_in_bytes * frame_no, SEEK_SET);
  if (status == -1)
  {
    fprintf(stderr, "Error in seeking frame number: %d\n", frame_no);
    free (buf);
    return;
  }

  if(rgb_output)
    lseek (p_ref, framesize_in_bytes/3, SEEK_CUR);

  read(p_ref, buf, size_y * size_x * symbol_size_in_bytes);
  buf2img(imgY_ref, buf, size_x, size_y, symbol_size_in_bytes);

  if (p->chroma_format_idc != YUV400)
  {
    for (uv=0; uv < 2; uv++)
    {
      if(rgb_output && uv==1)
        lseek (p_ref, -framesize_in_bytes, SEEK_CUR);

      read(p_ref, buf, size_y_cr * size_x_cr*symbol_size_in_bytes);
      buf2img(imgUV_ref[uv], buf, size_x_cr, size_y_cr, symbol_size_in_bytes);
    }
  }

   if(rgb_output)
     lseek (p_ref, framesize_in_bytes*2/3, SEEK_CUR);

  free (buf);

  img->quad[0]=0;
  diff_y=0;
  for (j=0; j < size_y; ++j)
  {
    for (i=0; i < size_x; ++i)
    {
      diff_y += img->quad[p->imgY[j][i]-imgY_ref[j][i]];
    }
  }

  // Chroma
  diff_u=0;
  diff_v=0;

  if (p->chroma_format_idc != YUV400)
  {
    for (j=0; j < size_y_cr; ++j)
    {
      for (i=0; i < size_x_cr; ++i)
      {
        diff_u += img->quad[imgUV_ref[0][j][i]-p->imgUV[0][j][i]];
        diff_v += img->quad[imgUV_ref[1][j][i]-p->imgUV[1][j][i]];
      }
    }
  }

#if ZEROSNR
  if (diff_y == 0)
    diff_y = 1;
  if (diff_u == 0)
    diff_u = 1;
  if (diff_v == 0)
    diff_v = 1;
#endif

  // Collecting SNR statistics
  if (diff_y != 0)
    snr->snr_y=(float)(10*log10(max_pix_value_sqd*(double)((double)(size_x)*(size_y) / diff_y)));        // luma snr for current frame
  else
    snr->snr_y=0.0;
  if (diff_u != 0)
    snr->snr_u=(float)(10*log10(max_pix_value_sqd_uv*(double)((double)(size_x_cr)*(size_y_cr) / (diff_u))));    //  chroma snr for current frame
  else
    snr->snr_u=0.0;
  if (diff_v != 0)
    snr->snr_v=(float)(10*log10(max_pix_value_sqd_uv*(double)((double)(size_x_cr)*(size_y_cr) / (diff_v))));    //  chroma snr for current frame
  else
    snr->snr_v=0;

  if (img->number == 0) // first
  {
    snr->snr_ya=snr->snr_y1=snr->snr_y;                                                        // keep luma snr for first frame
    snr->snr_ua=snr->snr_u1=snr->snr_u;                                                        // keep chroma snr for first frame
    snr->snr_va=snr->snr_v1=snr->snr_v;                                                        // keep chroma snr for first frame

  }
  else
  {
    snr->snr_ya=(float)(snr->snr_ya*(snr->frame_ctr)+snr->snr_y)/(snr->frame_ctr+1); // average snr chroma for all frames
    snr->snr_ua=(float)(snr->snr_ua*(snr->frame_ctr)+snr->snr_u)/(snr->frame_ctr+1); // average snr luma for all frames
    snr->snr_va=(float)(snr->snr_va*(snr->frame_ctr)+snr->snr_v)/(snr->frame_ctr+1); // average snr luma for all frames
  }

  // picture error concealment
  if(p->concealed_pic)
  {
      fprintf(stdout,"%04d(P)  %8d %5d %5d %7.4f %7.4f %7.4f  %s %5d\n",
          frame_no, p->frame_poc, p->pic_num, p->qp,
          snr->snr_y, snr->snr_u, snr->snr_v, yuv_types[p->chroma_format_idc], 0);

  }
}


/*!
 ************************************************************************
 * \brief
 *    Interpolation of 1/4 subpixel
 ************************************************************************
 */
void get_block(int ref_frame, StorablePicture **list, int x_pos, int y_pos, struct img_par *img, int block[BLOCK_SIZE][BLOCK_SIZE])
{

  int dx, dy;
  int i, j;
  int maxold_x,maxold_y;
  int result;
  int pres_x, pres_y;

  int tmp_res[9][9];
  static const int COEF[6] = {    1, -5, 20, 20, -5, 1  };
  StorablePicture *curr_ref = list[ref_frame];
  static imgpel **cur_imgY, *cur_lineY;
  static int jpos_m2, jpos_m1, jpos, jpos_p1, jpos_p2, jpos_p3;
  static int ipos_m2, ipos_m1, ipos, ipos_p1, ipos_p2, ipos_p3;

  if (curr_ref == no_reference_picture && img->framepoc < img->recovery_poc)
  {
      printf("list[ref_frame] is equal to 'no reference picture' before RAP\n");

      /* fill the block with sample value 128 */
      for (j = 0; j < BLOCK_SIZE; j++)
        for (i = 0; i < BLOCK_SIZE; i++)
          block[j][i] = 128;
      return;
  }
  cur_imgY = curr_ref->imgY;
  dx = x_pos&3;
  dy = y_pos&3;
  x_pos = (x_pos-dx)>>2;
  y_pos = (y_pos-dy)>>2;

  maxold_x = dec_picture->size_x_m1;
  maxold_y = dec_picture->size_y_m1;

  if (dec_picture->mb_field[img->current_mb_nr])
    maxold_y = (dec_picture->size_y >> 1) - 1;

  if (dx == 0 && dy == 0)
  {  /* fullpel position */
    for (j = 0; j < BLOCK_SIZE; j++)
    {
      cur_lineY = cur_imgY[iClip3(0,maxold_y,y_pos+j)];

      block[j][0] = cur_lineY[iClip3(0,maxold_x,x_pos  )];
      block[j][1] = cur_lineY[iClip3(0,maxold_x,x_pos+1)];
      block[j][2] = cur_lineY[iClip3(0,maxold_x,x_pos+2)];
      block[j][3] = cur_lineY[iClip3(0,maxold_x,x_pos+3)];
    }
  }
  else
  { /* other positions */

    if (dy == 0)
    { /* No vertical interpolation */

      for (i = 0; i < BLOCK_SIZE; i++)
      {
        ipos_m2 = iClip3(0, maxold_x, x_pos + i - 2);
        ipos_m1 = iClip3(0, maxold_x, x_pos + i - 1);
        ipos    = iClip3(0, maxold_x, x_pos + i    );
        ipos_p1 = iClip3(0, maxold_x, x_pos + i + 1);
        ipos_p2 = iClip3(0, maxold_x, x_pos + i + 2);
        ipos_p3 = iClip3(0, maxold_x, x_pos + i + 3);

        for (j = 0; j < BLOCK_SIZE; j++)
        {
          cur_lineY = cur_imgY[iClip3(0,maxold_y,y_pos+j)];

          result  = (cur_lineY[ipos_m2] + cur_lineY[ipos_p3]) * COEF[0];
          result += (cur_lineY[ipos_m1] + cur_lineY[ipos_p2]) * COEF[1];
          result += (cur_lineY[ipos   ] + cur_lineY[ipos_p1]) * COEF[2];

          block[j][i] = iClip1(img->max_imgpel_value, ((result+16)>>5));
        }
      }

      if ((dx&1) == 1)
      {
        for (j = 0; j < BLOCK_SIZE; j++)
        {
          cur_lineY = cur_imgY[iClip3(0,maxold_y,y_pos+j)];
          block[j][0] = (block[j][0] + cur_lineY[iClip3(0,maxold_x,x_pos  +(dx>>1))] + 1 )>>1;
          block[j][1] = (block[j][1] + cur_lineY[iClip3(0,maxold_x,x_pos+1+(dx>>1))] + 1 )>>1;
          block[j][2] = (block[j][2] + cur_lineY[iClip3(0,maxold_x,x_pos+2+(dx>>1))] + 1 )>>1;
          block[j][3] = (block[j][3] + cur_lineY[iClip3(0,maxold_x,x_pos+3+(dx>>1))] + 1 )>>1;
        }
      }
    }
    else if (dx == 0)
    {  /* No horizontal interpolation */

      for (j = 0; j < BLOCK_SIZE; j++)
      {
        jpos_m2 = iClip3(0, maxold_y, y_pos + j - 2);
        jpos_m1 = iClip3(0, maxold_y, y_pos + j - 1);
        jpos    = iClip3(0, maxold_y, y_pos + j    );
        jpos_p1 = iClip3(0, maxold_y, y_pos + j + 1);
        jpos_p2 = iClip3(0, maxold_y, y_pos + j + 2);
        jpos_p3 = iClip3(0, maxold_y, y_pos + j + 3);
        for (i = 0; i < BLOCK_SIZE; i++)
        {
          pres_x = iClip3(0,maxold_x,x_pos+i);

          result  = (cur_imgY[jpos_m2][pres_x] + cur_imgY[jpos_p3][pres_x]) * COEF[0];
          result += (cur_imgY[jpos_m1][pres_x] + cur_imgY[jpos_p2][pres_x]) * COEF[1];
          result += (cur_imgY[jpos   ][pres_x] + cur_imgY[jpos_p1][pres_x]) * COEF[2];
          block[j][i] = iClip1(img->max_imgpel_value, ((result+16)>>5));
        }
      }

      if ((dy&1) == 1)
      {
        for (j = 0; j < BLOCK_SIZE; j++)
        {
          cur_lineY = cur_imgY[iClip3(0,maxold_y,y_pos+j+(dy>>1))];
          block[j][0] = (block[j][0] + cur_lineY[iClip3(0,maxold_x,x_pos  )] + 1 )>>1;
          block[j][1] = (block[j][1] + cur_lineY[iClip3(0,maxold_x,x_pos+1)] + 1 )>>1;
          block[j][2] = (block[j][2] + cur_lineY[iClip3(0,maxold_x,x_pos+2)] + 1 )>>1;
          block[j][3] = (block[j][3] + cur_lineY[iClip3(0,maxold_x,x_pos+3)] + 1 )>>1;
        }
      }
    }
    else if (dx == 2)
    {  /* Vertical & horizontal interpolation */

      for (i = 0; i < BLOCK_SIZE; i++)
      {
        ipos_m2 = iClip3(0, maxold_x, x_pos + i - 2);
        ipos_m1 = iClip3(0, maxold_x, x_pos + i - 1);
        ipos    = iClip3(0, maxold_x, x_pos + i    );
        ipos_p1 = iClip3(0, maxold_x, x_pos + i + 1);
        ipos_p2 = iClip3(0, maxold_x, x_pos + i + 2);
        ipos_p3 = iClip3(0, maxold_x, x_pos + i + 3);

        for (j = 0; j < BLOCK_SIZE + 5; j++)
        {
          cur_lineY = cur_imgY[iClip3(0,maxold_y,y_pos + j - 2)];

          tmp_res[j][i]  = (cur_lineY[ipos_m2] + cur_lineY[ipos_p3]) * COEF[0];
          tmp_res[j][i] += (cur_lineY[ipos_m1] + cur_lineY[ipos_p2]) * COEF[1];
          tmp_res[j][i] += (cur_lineY[ipos   ] + cur_lineY[ipos_p1]) * COEF[2];
        }
      }

      for (j = 0; j < BLOCK_SIZE; j++)
      {
        jpos_m2 = j    ;
        jpos_m1 = j + 1;
        jpos    = j + 2;
        jpos_p1 = j + 3;
        jpos_p2 = j + 4;
        jpos_p3 = j + 5;

        for (i = 0; i < BLOCK_SIZE; i++)
        {
          result  = (tmp_res[jpos_m2][i] + tmp_res[jpos_p3][i]) * COEF[0];
          result += (tmp_res[jpos_m1][i] + tmp_res[jpos_p2][i]) * COEF[1];
          result += (tmp_res[jpos   ][i] + tmp_res[jpos_p1][i]) * COEF[2];

          block[j][i] = iClip1(img->max_imgpel_value, ((result+512)>>10));
        }
      }

      if ((dy&1) == 1)
      {
        for (j = 0; j < BLOCK_SIZE; j++)
        {
          pres_y = j+2+(dy>>1);

          block[j][0] = (block[j][0] + iClip1(img->max_imgpel_value, ((tmp_res[pres_y][0]+16)>>5)) +1 )>>1;
          block[j][1] = (block[j][1] + iClip1(img->max_imgpel_value, ((tmp_res[pres_y][1]+16)>>5)) +1 )>>1;
          block[j][2] = (block[j][2] + iClip1(img->max_imgpel_value, ((tmp_res[pres_y][2]+16)>>5)) +1 )>>1;
          block[j][3] = (block[j][3] + iClip1(img->max_imgpel_value, ((tmp_res[pres_y][3]+16)>>5)) +1 )>>1;
        }
      }
    }
    else if (dy == 2)
    {  /* Horizontal & vertical interpolation */

      for (j = 0; j < BLOCK_SIZE; j++)
      {
        jpos_m2 = iClip3(0, maxold_y, y_pos + j - 2);
        jpos_m1 = iClip3(0, maxold_y, y_pos + j - 1);
        jpos    = iClip3(0, maxold_y, y_pos + j    );
        jpos_p1 = iClip3(0, maxold_y, y_pos + j + 1);
        jpos_p2 = iClip3(0, maxold_y, y_pos + j + 2);
        jpos_p3 = iClip3(0, maxold_y, y_pos + j + 3);
        for (i = 0; i < BLOCK_SIZE+5; i++)
        {
          pres_x = iClip3(0,maxold_x,x_pos+i - 2);
          tmp_res[j][i]  = (cur_imgY[jpos_m2][pres_x] + cur_imgY[jpos_p3][pres_x])*COEF[0];
          tmp_res[j][i] += (cur_imgY[jpos_m1][pres_x] + cur_imgY[jpos_p2][pres_x])*COEF[1];
          tmp_res[j][i] += (cur_imgY[jpos   ][pres_x] + cur_imgY[jpos_p1][pres_x])*COEF[2];
        }
      }

      for (j = 0; j < BLOCK_SIZE; j++)
      {
        for (i = 0; i < BLOCK_SIZE; i++)
        {
          result  = (tmp_res[j][i    ] + tmp_res[j][i + 5]) * COEF[0];
          result += (tmp_res[j][i + 1] + tmp_res[j][i + 4]) * COEF[1];
          result += (tmp_res[j][i + 2] + tmp_res[j][i + 3]) * COEF[2];
          block[j][i] = iClip1(img->max_imgpel_value, ((result+512)>>10));
        }
      }

      if ((dx&1) == 1)
      {
        for (j = 0; j < BLOCK_SIZE; j++)
        {
          block[j][0] = (block[j][0] + iClip1(img->max_imgpel_value, ((tmp_res[j][2+(dx>>1)]+16)>>5))+1)>>1;
          block[j][1] = (block[j][1] + iClip1(img->max_imgpel_value, ((tmp_res[j][3+(dx>>1)]+16)>>5))+1)>>1;
          block[j][2] = (block[j][2] + iClip1(img->max_imgpel_value, ((tmp_res[j][4+(dx>>1)]+16)>>5))+1)>>1;
          block[j][3] = (block[j][3] + iClip1(img->max_imgpel_value, ((tmp_res[j][5+(dx>>1)]+16)>>5))+1)>>1;
        }
      }
    }
    else
    {  /* Diagonal interpolation */

      for (i = 0; i < BLOCK_SIZE; i++)
      {
        ipos_m2 = iClip3(0, maxold_x, x_pos + i - 2);
        ipos_m1 = iClip3(0, maxold_x, x_pos + i - 1);
        ipos    = iClip3(0, maxold_x, x_pos + i    );
        ipos_p1 = iClip3(0, maxold_x, x_pos + i + 1);
        ipos_p2 = iClip3(0, maxold_x, x_pos + i + 2);
        ipos_p3 = iClip3(0, maxold_x, x_pos + i + 3);

        for (j = 0; j < BLOCK_SIZE; j++)
        {
          cur_lineY = cur_imgY[iClip3(0,maxold_y,(dy == 1 ? y_pos+j : y_pos+j+1))];

          result  = (cur_lineY[ipos_m2] + cur_lineY[ipos_p3]) * COEF[0];
          result += (cur_lineY[ipos_m1] + cur_lineY[ipos_p2]) * COEF[1];
          result += (cur_lineY[ipos   ] + cur_lineY[ipos_p1]) * COEF[2];

          block[j][i] = iClip1(img->max_imgpel_value, ((result+16)>>5));
        }
      }

      for (j = 0; j < BLOCK_SIZE; j++)
      {
        jpos_m2 = iClip3(0, maxold_y, y_pos + j - 2);
        jpos_m1 = iClip3(0, maxold_y, y_pos + j - 1);
        jpos    = iClip3(0, maxold_y, y_pos + j    );
        jpos_p1 = iClip3(0, maxold_y, y_pos + j + 1);
        jpos_p2 = iClip3(0, maxold_y, y_pos + j + 2);
        jpos_p3 = iClip3(0, maxold_y, y_pos + j + 3);
        for (i = 0; i < BLOCK_SIZE; i++)
        {
          pres_x = dx == 1 ? x_pos+i : x_pos+i+1;
          pres_x = iClip3(0,maxold_x,pres_x);

          result  = (cur_imgY[jpos_m2][pres_x] + cur_imgY[jpos_p3][pres_x]) * COEF[0];
          result += (cur_imgY[jpos_m1][pres_x] + cur_imgY[jpos_p2][pres_x]) * COEF[1];
          result += (cur_imgY[jpos   ][pres_x] + cur_imgY[jpos_p1][pres_x]) * COEF[2];

          block[j][i] = (block[j][i] + iClip1(img->max_imgpel_value, ((result+16)>>5)) +1 ) >>1;
        }
      }

    }
  }
}


void reorder_lists(int currSliceType, Slice * currSlice)
{

  if ((currSliceType != I_SLICE)&&(currSliceType != SI_SLICE))
  {
    if (currSlice->ref_pic_list_reordering_flag_l0)
    {
      reorder_ref_pic_list(listX[0], &listXsize[0],
                           img->num_ref_idx_l0_active - 1,
                           currSlice->reordering_of_pic_nums_idc_l0,
                           currSlice->abs_diff_pic_num_minus1_l0,
                           currSlice->long_term_pic_idx_l0);
    }
    if (no_reference_picture == listX[0][img->num_ref_idx_l0_active-1])
    {
      if (non_conforming_stream)
        printf("RefPicList0[ num_ref_idx_l0_active_minus1 ] is equal to 'no reference picture'\n");
      else
        error("RefPicList0[ num_ref_idx_l0_active_minus1 ] is equal to 'no reference picture', invalid bitstream",500);
    }
    // that's a definition
    listXsize[0] = img->num_ref_idx_l0_active;
  }
  if (currSliceType == B_SLICE)
  {
    if (currSlice->ref_pic_list_reordering_flag_l1)
    {
      reorder_ref_pic_list(listX[1], &listXsize[1],
                           img->num_ref_idx_l1_active - 1,
                           currSlice->reordering_of_pic_nums_idc_l1,
                           currSlice->abs_diff_pic_num_minus1_l1,
                           currSlice->long_term_pic_idx_l1);
    }
    if (no_reference_picture == listX[1][img->num_ref_idx_l1_active-1])
    {
      if (non_conforming_stream)
        printf("RefPicList1[ num_ref_idx_l1_active_minus1 ] is equal to 'no reference picture'\n");
      else
        error("RefPicList1[ num_ref_idx_l1_active_minus1 ] is equal to 'no reference picture', invalid bitstream",500);
    }
    // that's a definition
    listXsize[1] = img->num_ref_idx_l1_active;
  }

  free_ref_pic_list_reordering_buffer(currSlice);
}


/*!
 ************************************************************************
 * \brief
 *    initialize ref_pic_num array
 ************************************************************************
 */
void set_ref_pic_num()
{
  int i,j;

  int slice_id=img->current_slice_nr;

  for (i=0;i<listXsize[LIST_0];i++)
  {
    dec_picture->ref_pic_num        [slice_id][LIST_0][i]=listX[LIST_0][i]->poc * 2 + ((listX[LIST_0][i]->structure==BOTTOM_FIELD)?1:0) ;
    dec_picture->frm_ref_pic_num    [slice_id][LIST_0][i]=listX[LIST_0][i]->frame_poc * 2;
    dec_picture->top_ref_pic_num    [slice_id][LIST_0][i]=listX[LIST_0][i]->top_poc * 2;
    dec_picture->bottom_ref_pic_num [slice_id][LIST_0][i]=listX[LIST_0][i]->bottom_poc * 2 + 1;
    //printf("POCS %d %d %d %d ",listX[LIST_0][i]->frame_poc,listX[LIST_0][i]->bottom_poc,listX[LIST_0][i]->top_poc,listX[LIST_0][i]->poc);
    //printf("refid %d %d %d %d\n",(int) dec_picture->frm_ref_pic_num[LIST_0][i],(int) dec_picture->top_ref_pic_num[LIST_0][i],(int) dec_picture->bottom_ref_pic_num[LIST_0][i],(int) dec_picture->ref_pic_num[LIST_0][i]);
  }

  for (i=0;i<listXsize[LIST_1];i++)
  {
    dec_picture->ref_pic_num        [slice_id][LIST_1][i]=listX[LIST_1][i]->poc  *2 + ((listX[LIST_1][i]->structure==BOTTOM_FIELD)?1:0);
    dec_picture->frm_ref_pic_num    [slice_id][LIST_1][i]=listX[LIST_1][i]->frame_poc * 2;
    dec_picture->top_ref_pic_num    [slice_id][LIST_1][i]=listX[LIST_1][i]->top_poc * 2;
    dec_picture->bottom_ref_pic_num [slice_id][LIST_1][i]=listX[LIST_1][i]->bottom_poc * 2 + 1;
  }

  if (!active_sps->frame_mbs_only_flag)
  {
    if (img->structure==FRAME)
      for (j=2;j<6;j++)
        for (i=0;i<listXsize[j];i++)
        {
          dec_picture->ref_pic_num        [slice_id][j][i] = listX[j][i]->poc * 2 + ((listX[j][i]->structure==BOTTOM_FIELD)?1:0);
          dec_picture->frm_ref_pic_num    [slice_id][j][i] = listX[j][i]->frame_poc * 2 ;
          dec_picture->top_ref_pic_num    [slice_id][j][i] = listX[j][i]->top_poc * 2 ;
          dec_picture->bottom_ref_pic_num [slice_id][j][i] = listX[j][i]->bottom_poc * 2 + 1;
        }
  }

}


/*!
 ************************************************************************
 * \brief
 *    Reads new slice from bit_stream
 ************************************************************************
 */
int read_new_slice()
{
  NALU_t *nalu = AllocNALU(MAX_CODED_FRAME_SIZE);
  int current_header = 0;
  int ret;
  int BitsUsedByHeader;
  Slice *currSlice = img->currentSlice;
  Bitstream *currStream;

  int slice_id_a, slice_id_b, slice_id_c;
  int redundant_pic_cnt_b, redundant_pic_cnt_c;
  long ftell_position;

  while (1)
  {
    ftell_position = ftell(bits);

    if (input->FileFormat == PAR_OF_ANNEXB)
      ret=GetAnnexbNALU (nalu);
    else
      ret=GetRTPNALU (nalu);

    //In some cases, zero_byte shall be present. If current NALU is a VCL NALU, we can't tell
    //whether it is the first VCL NALU at this point, so only non-VCL NAL unit is checked here.
    CheckZeroByteNonVCL(nalu, &ret);

    NALUtoRBSP(nalu);

    if (ret < 0)
      printf ("Error while getting the NALU in file format %s, exit\n", input->FileFormat==PAR_OF_ANNEXB?"Annex B":"RTP");
    if (ret == 0)
    {
      FreeNALU(nalu);
      return EOS;
    }

    // Got a NALU
    if (nalu->forbidden_bit)
    {
      printf ("Found NALU w/ forbidden_bit set, bit error?  Let's try...\n");
    }

    switch (nalu->nal_unit_type)
    {
      case NALU_TYPE_SLICE:
      case NALU_TYPE_IDR:

        if (img->recovery_point || nalu->nal_unit_type == NALU_TYPE_IDR)
        {
          if (img->recovery_point_found == 0)
          {
            if (nalu->nal_unit_type != NALU_TYPE_IDR)
            {
              printf("Warning: Decoding does not start with an IDR picture.\n");
              non_conforming_stream = 1;
            }
            else
              non_conforming_stream = 0;
          }
          img->recovery_point_found = 1;
        }

        if (img->recovery_point_found == 0)
            break;

        img->idr_flag = (nalu->nal_unit_type == NALU_TYPE_IDR);
        img->nal_reference_idc = nalu->nal_reference_idc;
        currSlice->dp_mode = PAR_DP_1;
        currSlice->max_part_nr = 1;
        currSlice->ei_flag = 0;
        currStream = currSlice->partArr[0].bitstream;
        currStream->ei_flag = 0;
        currStream->frame_bitoffset = currStream->read_len = 0;
        memcpy (currStream->streamBuffer, &nalu->buf[1], nalu->len-1);
        currStream->code_len = currStream->bitstream_length = RBSPtoSODB(currStream->streamBuffer, nalu->len-1);

        // Some syntax of the Slice Header depends on the parameter set, which depends on
        // the parameter set ID of the SLice header.  Hence, read the pic_parameter_set_id
        // of the slice header first, then setup the active parameter sets, and then read
        // the rest of the slice header
        BitsUsedByHeader = FirstPartOfSliceHeader();
        UseParameterSet (currSlice->pic_parameter_set_id);
        BitsUsedByHeader+= RestOfSliceHeader ();

        FmoInit (active_pps, active_sps);

        AssignQuantParam (active_pps, active_sps);

        // if primary slice is replaced with redundant slice, set the correct image type
        if(img->redundant_pic_cnt && Is_primary_correct==0 && Is_redundant_correct)
        {
          dec_picture->slice_type=img->type;
        }

        if(is_new_picture())
        {
          init_picture(img, input);

          current_header = SOP;
          //check zero_byte if it is also the first NAL unit in the access unit
          CheckZeroByteVCL(nalu, &ret);
        }
        else
          current_header = SOS;

        init_lists(img->type, img->currentSlice->structure);
        reorder_lists (img->type, img->currentSlice);

        if (img->structure==FRAME)
        {
          init_mbaff_lists();
        }

/*        if (img->frame_num==1) // write a reference list
        {
          count ++;
          if (count==1)
            for (i=0; i<listXsize[0]; i++)
              write_picture(listX[0][i], p_out2);
        }
*/

        // From here on, active_sps, active_pps and the slice header are valid
        if (img->MbaffFrameFlag)
          img->current_mb_nr = currSlice->start_mb_nr << 1;
        else
          img->current_mb_nr = currSlice->start_mb_nr;

        if (active_pps->entropy_coding_mode_flag)
        {
          int ByteStartPosition = currStream->frame_bitoffset/8;
          if (currStream->frame_bitoffset%8 != 0)
          {
            ByteStartPosition++;
          }
          arideco_start_decoding (&currSlice->partArr[0].de_cabac, currStream->streamBuffer, ByteStartPosition, &currStream->read_len, img->type);
        }
// printf ("read_new_slice: returning %s\n", current_header == SOP?"SOP":"SOS");
        FreeNALU(nalu);
        img->recovery_point = 0;
        return current_header;
        break;
      case NALU_TYPE_DPA:
        // read DP_A
        img->idr_flag          = (nalu->nal_unit_type == NALU_TYPE_IDR);
        if (img->idr_flag)
        {
          printf ("Data partiton A cannot have idr_flag set, trying anyway \n");
        }
        img->nal_reference_idc = nalu->nal_reference_idc;
        currSlice->dp_mode     = PAR_DP_3;
        currSlice->max_part_nr = 3;
        currSlice->ei_flag     = 0;
        currStream             = currSlice->partArr[0].bitstream;
        currStream->ei_flag    = 0;
        currStream->frame_bitoffset = currStream->read_len = 0;
        memcpy (currStream->streamBuffer, &nalu->buf[1], nalu->len-1);
        currStream->code_len = currStream->bitstream_length = RBSPtoSODB(currStream->streamBuffer, nalu->len-1);

        BitsUsedByHeader     = FirstPartOfSliceHeader();
        UseParameterSet (currSlice->pic_parameter_set_id);
        BitsUsedByHeader    += RestOfSliceHeader ();

        FmoInit (active_pps, active_sps);

        if(is_new_picture())
        {
          init_picture(img, input);
          current_header = SOP;
          CheckZeroByteVCL(nalu, &ret);
        }
        else
          current_header = SOS;

        init_lists(img->type, img->currentSlice->structure);
        reorder_lists (img->type, img->currentSlice);

        if (img->structure==FRAME)
        {
          init_mbaff_lists();
        }

        // From here on, active_sps, active_pps and the slice header are valid
        if (img->MbaffFrameFlag)
          img->current_mb_nr = currSlice->start_mb_nr << 1;
        else
          img->current_mb_nr = currSlice->start_mb_nr;

        // Now I need to read the slice ID, which depends on the value of
        // redundant_pic_cnt_present_flag

        slice_id_a  = ue_v("NALU: DP_A slice_id", currStream);

        if (active_pps->entropy_coding_mode_flag)
          error ("received data partition with CABAC, this is not allowed", 500);

        // continue with reading next DP
        ftell_position = ftell(bits);
        if (input->FileFormat == PAR_OF_ANNEXB)
          ret=GetAnnexbNALU (nalu);
        else
          ret=GetRTPNALU (nalu);

        CheckZeroByteNonVCL(nalu, &ret);
        NALUtoRBSP(nalu);

        if (ret < 0)
          printf ("Error while getting the NALU in file format %s, exit\n", input->FileFormat==PAR_OF_ANNEXB?"Annex B":"RTP");
        if (ret == 0)
        {
          FreeNALU(nalu);
          return current_header;
        }

        if ( NALU_TYPE_DPB == nalu->nal_unit_type)
        {
          // we got a DPB
          currStream             = currSlice->partArr[1].bitstream;
          currStream->ei_flag    = 0;
          currStream->frame_bitoffset = currStream->read_len = 0;

          memcpy (currStream->streamBuffer, &nalu->buf[1], nalu->len-1);
          currStream->code_len = currStream->bitstream_length = RBSPtoSODB(currStream->streamBuffer, nalu->len-1);

          slice_id_b  = ue_v("NALU: DP_B slice_id", currStream);

          if (slice_id_b != slice_id_a)
          {
            printf ("got a data partition B which does not match DP_A\n");
            // KS: needs error handling !!!
          }

          if (active_pps->redundant_pic_cnt_present_flag)
            redundant_pic_cnt_b = ue_v("NALU: DP_B redudant_pic_cnt", currStream);
          else
            redundant_pic_cnt_b = 0;

          // we're finished with DP_B, so let's continue with next DP
          ftell_position = ftell(bits);
          if (input->FileFormat == PAR_OF_ANNEXB)
            ret=GetAnnexbNALU (nalu);
          else
            ret=GetRTPNALU (nalu);

          CheckZeroByteNonVCL(nalu, &ret);
          NALUtoRBSP(nalu);

          if (ret < 0)
            printf ("Error while getting the NALU in file format %s, exit\n", input->FileFormat==PAR_OF_ANNEXB?"Annex B":"RTP");
          if (ret == 0)
          {
            FreeNALU(nalu);
            return current_header;
          }
        }

        // check if we got DP_C
        if ( NALU_TYPE_DPC == nalu->nal_unit_type)
        {
          currStream             = currSlice->partArr[2].bitstream;
          currStream->ei_flag    = 0;
          currStream->frame_bitoffset = currStream->read_len = 0;

          memcpy (currStream->streamBuffer, &nalu->buf[1], nalu->len-1);
          currStream->code_len = currStream->bitstream_length = RBSPtoSODB(currStream->streamBuffer, nalu->len-1);

          slice_id_c  = ue_v("NALU: DP_C slice_id", currStream);
          if (slice_id_c != slice_id_a)
          {
            printf ("got a data partition C which does not match DP_A\n");
            // KS: needs error handling !!!
          }

          if (active_pps->redundant_pic_cnt_present_flag)
            redundant_pic_cnt_c = ue_v("NALU:SLICE_C redudand_pic_cnt", currStream);
          else
            redundant_pic_cnt_c = 0;
        }

        // check if we read anything else than the expected partitions
        if ((nalu->nal_unit_type != NALU_TYPE_DPB) && (nalu->nal_unit_type != NALU_TYPE_DPC))
        {
          // reset bitstream position and read again in next call
          fseek(bits, ftell_position, SEEK_SET);
        }

        FreeNALU(nalu);
        return current_header;

        break;
      case NALU_TYPE_DPB:
        printf ("found data partition B without matching DP A, discarding\n");
        break;
      case NALU_TYPE_DPC:
        printf ("found data partition C without matching DP A, discarding\n");
        break;
      case NALU_TYPE_SEI:
        printf ("read_new_slice: Found NALU_TYPE_SEI, len %d\n", nalu->len);
        InterpretSEIMessage(nalu->buf,nalu->len,img);
        break;
      case NALU_TYPE_PPS:
        ProcessPPS(nalu);
        break;
      case NALU_TYPE_SPS:
        ProcessSPS(nalu);
        break;
      case NALU_TYPE_AUD:
//        printf ("read_new_slice: Found 'Access Unit Delimiter' NAL unit, len %d, ignored\n", nalu->len);
        break;
      case NALU_TYPE_EOSEQ:
//        printf ("read_new_slice: Found 'End of Sequence' NAL unit, len %d, ignored\n", nalu->len);
        break;
      case NALU_TYPE_EOSTREAM:
//        printf ("read_new_slice: Found 'End of Stream' NAL unit, len %d, ignored\n", nalu->len);
        break;
      case NALU_TYPE_FILL:
        printf ("read_new_slice: Found NALU_TYPE_FILL, len %d\n", nalu->len);
        printf ("Skipping these filling bits, proceeding w/ next NALU\n");
        break;
      default:
        printf ("Found NALU type %d, len %d undefined, ignore NALU, moving on\n", nalu->nal_unit_type, nalu->len);
    }
  }
  FreeNALU(nalu);

  return  current_header;
}


/*!
 ************************************************************************
 * \brief
 *    Initializes the parameters for a new picture
 ************************************************************************
 */
void init_picture(struct img_par *img, struct inp_par *inp)
{
  int i,k,l;
  Slice *currSlice = img->currentSlice;

  if (dec_picture)
  {
    // this may only happen on slice loss
    exit_picture();
  }
  if (img->recovery_point)
    img->recovery_frame_num = (img->frame_num + img->recovery_frame_cnt) % img->MaxFrameNum;

  if (img->idr_flag)
    img->recovery_frame_num = img->frame_num;

  if (img->recovery_point == 0 &&
      img->frame_num != img->pre_frame_num &&
      img->frame_num != (img->pre_frame_num + 1) % img->MaxFrameNum)
  {
    if (active_sps->gaps_in_frame_num_value_allowed_flag == 0)
    {
      // picture error concealment
      if(inp->conceal_mode !=0)
      {
        if((img->frame_num) < ((img->pre_frame_num + 1) % img->MaxFrameNum))
        {
          /* Conceal lost IDR frames and any frames immediately
             following the IDR. Use frame copy for these since
             lists cannot be formed correctly for motion copy*/
          img->conceal_mode = 1;
          img->IDR_concealment_flag = 1;
          conceal_lost_frames(img);
          //reset to original concealment mode for future drops
          img->conceal_mode = inp->conceal_mode;
        }
        else
        {
          //reset to original concealment mode for future drops
          img->conceal_mode = inp->conceal_mode;

          img->IDR_concealment_flag = 0;
          conceal_lost_frames(img);
        }
      }
      else
      {   /* Advanced Error Concealment would be called here to combat unintentional loss of pictures. */
        error("An unintentional loss of pictures occurs! Exit\n", 100);
      }
    }
    if(img->conceal_mode == 0)
      fill_frame_num_gap(img);
  }

  if(img->nal_reference_idc)
  {
    img->pre_frame_num = img->frame_num;
  }

  //img->num_dec_mb = 0;

  //calculate POC
  decode_poc(img);

  if (img->recovery_frame_num == img->frame_num &&
      img->recovery_poc == 0x7fffffff)
    img->recovery_poc = img->framepoc;

  if(img->nal_reference_idc)
    img->last_ref_pic_poc = img->framepoc;

  //  dumppoc (img);

  if (img->structure==FRAME ||img->structure==TOP_FIELD)
  {
#ifdef WIN32
    _ftime (&(img->tstruct_start));             // start time ms
#else
    ftime (&(img->tstruct_start));         // start time ms
#endif
    time( &(img->ltime_start));                // start time s
  }

  dec_picture = alloc_storable_picture ((PictureStructure) img->structure, img->width, img->height, img->width_cr, img->height_cr);
  dec_picture->top_poc=img->toppoc;
  dec_picture->bottom_poc=img->bottompoc;
  dec_picture->frame_poc=img->framepoc;
  dec_picture->qp=img->qp;
  dec_picture->slice_qp_delta=currSlice->slice_qp_delta;
  dec_picture->chroma_qp_offset[0] = active_pps->chroma_qp_index_offset;
  dec_picture->chroma_qp_offset[1] = active_pps->second_chroma_qp_index_offset;

  // reset all variables of the error concealment instance before decoding of every frame.
  // here the third parameter should, if perfectly, be equal to the number of slices per frame.
  // using little value is ok, the code will allocate more memory if the slice number is larger
  ercReset(erc_errorVar, img->PicSizeInMbs, img->PicSizeInMbs, dec_picture->size_x);
  erc_mvperMB = 0;

  switch (img->structure )
  {
  case TOP_FIELD:
    {
      dec_picture->poc=img->toppoc;
      img->number *= 2;
      break;
    }
  case BOTTOM_FIELD:
    {
      dec_picture->poc=img->bottompoc;
      img->number = img->number * 2 + 1;
      break;
    }
  case FRAME:
    {
      dec_picture->poc=img->framepoc;
      break;
    }
  default:
    error("img->structure not initialized", 235);
  }

  img->current_slice_nr=0;

  if (img->type > SI_SLICE)
  {
    set_ec_flag(SE_PTYPE);
    img->type = P_SLICE;  // concealed element
  }

  // CAVLC init
  for (i=0;i < (int)img->PicSizeInMbs; i++)
    for (k=0;k<4;k++)
      for (l=0;l<(4 + img->num_blk8x8_uv);l++)
        img->nz_coeff[i][k][l]=-1;  // CAVLC

  if(active_pps->constrained_intra_pred_flag)
  {
    for (i=0; i<(int)img->PicSizeInMbs; i++)
    {
      img->intra_block[i] = 1;
    }
  }

  // Set the slice_nr member of each MB to -1, to ensure correct when packet loss occurs
  // TO set Macroblock Map (mark all MBs as 'have to be concealed')
  for(i=0; i<(int)img->PicSizeInMbs; i++)
  {
    img->mb_data[i].slice_nr = -1;
    img->mb_data[i].ei_flag = 1;
  }

  img->mb_y = img->mb_x = 0;
  img->block_y = img->pix_y = img->pix_c_y = 0; // define vertical positions
  img->block_x = img->pix_x = img->pix_c_x = 0; // define horizontal positions

  dec_picture->slice_type = img->type;
  dec_picture->used_for_reference = (img->nal_reference_idc != 0);
  dec_picture->idr_flag = img->idr_flag;
  dec_picture->no_output_of_prior_pics_flag = img->no_output_of_prior_pics_flag;
  dec_picture->long_term_reference_flag = img->long_term_reference_flag;
  dec_picture->adaptive_ref_pic_buffering_flag = img->adaptive_ref_pic_buffering_flag;

  dec_picture->dec_ref_pic_marking_buffer = img->dec_ref_pic_marking_buffer;
  img->dec_ref_pic_marking_buffer = NULL;

  dec_picture->MbaffFrameFlag = img->MbaffFrameFlag;
  dec_picture->PicWidthInMbs = img->PicWidthInMbs;

  get_mb_block_pos = dec_picture->MbaffFrameFlag ? get_mb_block_pos_mbaff : get_mb_block_pos_normal;
  getNeighbour = dec_picture->MbaffFrameFlag ? getAffNeighbour : getNonAffNeighbour;



  dec_picture->pic_num = img->frame_num;
  dec_picture->frame_num = img->frame_num;

  dec_picture->recovery_frame = (img->frame_num == img->recovery_frame_num);

  dec_picture->coded_frame = (img->structure==FRAME);

  dec_picture->chroma_format_idc = active_sps->chroma_format_idc;

  dec_picture->frame_mbs_only_flag = active_sps->frame_mbs_only_flag;
  dec_picture->frame_cropping_flag = active_sps->frame_cropping_flag;

  if (dec_picture->frame_cropping_flag)
  {
    dec_picture->frame_cropping_rect_left_offset   = active_sps->frame_cropping_rect_left_offset;
    dec_picture->frame_cropping_rect_right_offset  = active_sps->frame_cropping_rect_right_offset;
    dec_picture->frame_cropping_rect_top_offset    = active_sps->frame_cropping_rect_top_offset;
    dec_picture->frame_cropping_rect_bottom_offset = active_sps->frame_cropping_rect_bottom_offset;
  }
}

/*!
 ************************************************************************
 * \brief
 *    finish decoding of a picture, conceal errors and store it
 *    into the DPB
 ************************************************************************
 */
void exit_picture()
{
  char yuv_types[4][6]= {"4:0:0","4:2:0","4:2:2","4:4:4"};
  int ercStartMB;
  int ercSegment;
  frame recfr;
  unsigned int i;
  int structure, frame_poc, slice_type, refpic, qp, pic_num, chroma_format_idc;

  char   yuvFormat[10];

  // return if the last picture has already been finished
  if (dec_picture==NULL)
  {
    return;
  }

  //deblocking for frame or field
  DeblockPicture( img, dec_picture );

  if (dec_picture->MbaffFrameFlag)
    MbAffPostProc();

  recfr.yptr = &dec_picture->imgY[0][0];
  if (dec_picture->chroma_format_idc != YUV400)
  {
    recfr.uptr = &dec_picture->imgUV[0][0][0];
    recfr.vptr = &dec_picture->imgUV[1][0][0];
  }

  //! this is always true at the beginning of a picture
  ercStartMB = 0;
  ercSegment = 0;

  //! mark the start of the first segment
  if (!dec_picture->MbaffFrameFlag)
  {
    ercStartSegment(0, ercSegment, 0 , erc_errorVar);
    //! generate the segments according to the macroblock map
    for(i = 1; i<dec_picture->PicSizeInMbs; i++)
    {
      if(img->mb_data[i].ei_flag != img->mb_data[i-1].ei_flag)
      {
        ercStopSegment(i-1, ercSegment, 0, erc_errorVar); //! stop current segment

        //! mark current segment as lost or OK
        if(img->mb_data[i-1].ei_flag)
          ercMarkCurrSegmentLost(dec_picture->size_x, erc_errorVar);
        else
          ercMarkCurrSegmentOK(dec_picture->size_x, erc_errorVar);

        ercSegment++;  //! next segment
        ercStartSegment(i, ercSegment, 0 , erc_errorVar); //! start new segment
        ercStartMB = i;//! save start MB for this segment
      }
    }
    //! mark end of the last segment
    ercStopSegment(dec_picture->PicSizeInMbs-1, ercSegment, 0, erc_errorVar);
    if(img->mb_data[i-1].ei_flag)
      ercMarkCurrSegmentLost(dec_picture->size_x, erc_errorVar);
    else
      ercMarkCurrSegmentOK(dec_picture->size_x, erc_errorVar);

    //! call the right error concealment function depending on the frame type.
    erc_mvperMB /= dec_picture->PicSizeInMbs;

    erc_img = img;
    if(dec_picture->slice_type == I_SLICE || dec_picture->slice_type == SI_SLICE) // I-frame
      ercConcealIntraFrame(&recfr, dec_picture->size_x, dec_picture->size_y, erc_errorVar);
    else
      ercConcealInterFrame(&recfr, erc_object_list, dec_picture->size_x, dec_picture->size_y, erc_errorVar, dec_picture->chroma_format_idc);
  }

  if (img->structure == FRAME)         // buffer mgt. for frame mode
    frame_postprocessing(img, input);
  else
    field_postprocessing(img, input);   // reset all interlaced variables

  structure  = dec_picture->structure;
  slice_type = dec_picture->slice_type;
  frame_poc  = dec_picture->frame_poc;
  refpic     = dec_picture->used_for_reference;
  qp         = dec_picture->qp;
  pic_num    = dec_picture->pic_num;

  chroma_format_idc= dec_picture->chroma_format_idc;

  store_picture_in_dpb(dec_picture);
  dec_picture=NULL;

  if (img->last_has_mmco_5)
  {
    img->pre_frame_num = 0;
  }

  if ((structure==FRAME)||structure==BOTTOM_FIELD)
  {

#ifdef WIN32
    _ftime (&(img->tstruct_end));             // start time ms
#else
    ftime (&(img->tstruct_end));         // start time ms
#endif

    time( &(img->ltime_end));                // start time s


    sprintf(yuvFormat,"%s", yuv_types[chroma_format_idc]);
    
    if (input->silent == FALSE)
    {
      if(slice_type == I_SLICE) // I picture
        fprintf(stdout,"%04d(I)  %8d %5d %5d %7.4f %7.4f %7.4f  %s %5d\n",
        frame_no, frame_poc, pic_num, qp, snr->snr_y, snr->snr_u, snr->snr_v, yuvFormat, 0);
      else if(slice_type == P_SLICE) // P pictures
        fprintf(stdout,"%04d(P)  %8d %5d %5d %7.4f %7.4f %7.4f  %s %5d\n",
        frame_no, frame_poc, pic_num, qp, snr->snr_y, snr->snr_u, snr->snr_v, yuvFormat, 0);
      else if(slice_type == SP_SLICE) // SP pictures
        fprintf(stdout,"%04d(SP) %8d %5d %5d %7.4f %7.4f %7.4f  %s %5d\n",
        frame_no, frame_poc, pic_num, qp, snr->snr_y, snr->snr_u, snr->snr_v, yuvFormat, 0);
      else if (slice_type == SI_SLICE)
        fprintf(stdout,"%04d(SI) %8d %5d %5d %7.4f %7.4f %7.4f  %s %5d\n",
        frame_no, frame_poc, pic_num, qp, snr->snr_y, snr->snr_u, snr->snr_v, yuvFormat, 0);
      else if(refpic) // stored B pictures
        fprintf(stdout,"%04d(RB) %8d %5d %5d %7.4f %7.4f %7.4f  %s %5d\n",
        frame_no, frame_poc, pic_num, qp, snr->snr_y, snr->snr_u, snr->snr_v, yuvFormat, 0);
      else // B pictures
        fprintf(stdout,"%04d(B)  %8d %5d %5d %7.4f %7.4f %7.4f  %s %5d\n",
        frame_no, frame_poc, pic_num, qp, snr->snr_y, snr->snr_u, snr->snr_v, yuvFormat, 0);
    }
    else
      fprintf(stdout,"Completed Decoding frame %05d.\r",snr->frame_ctr);

    fflush(stdout);

    if(slice_type == I_SLICE || slice_type == SI_SLICE || slice_type == P_SLICE || refpic)   // I or P pictures
      img->number++;
    else
      Bframe_ctr++;    // B pictures
    snr->frame_ctr++;

    g_nFrame++;
  }

  img->current_mb_nr = -4712;   // impossible value for debugging, StW
  img->current_slice_nr = 0;

}

/*!
 ************************************************************************
 * \brief
 *    write the encoding mode and motion vectors of current
 *    MB to the buffer of the error concealment module.
 ************************************************************************
 */

void ercWriteMBMODEandMV(struct img_par *img,struct inp_par *inp)
{
  extern objectBuffer_t *erc_object_list;
  int i, ii, jj, currMBNum = img->current_mb_nr;
  int mbx = xPosMB(currMBNum,dec_picture->size_x), mby = yPosMB(currMBNum,dec_picture->size_x);
  objectBuffer_t *currRegion, *pRegion;
  Macroblock *currMB = &img->mb_data[currMBNum];
  short***  mv;

  currRegion = erc_object_list + (currMBNum<<2);

  if(img->type != B_SLICE) //non-B frame
  {
    for (i=0; i<4; i++)
    {
      pRegion             = currRegion + i;
      pRegion->regionMode = (currMB->mb_type  ==I16MB  ? REGMODE_INTRA      :
                             currMB->b8mode[i]==IBLOCK ? REGMODE_INTRA_8x8  :
                             currMB->b8mode[i]==0      ? REGMODE_INTER_COPY :
                             currMB->b8mode[i]==1      ? REGMODE_INTER_PRED : REGMODE_INTER_PRED_8x8);
      if (currMB->b8mode[i]==0 || currMB->b8mode[i]==IBLOCK)  // INTRA OR COPY
      {
        pRegion->mv[0]    = 0;
        pRegion->mv[1]    = 0;
        pRegion->mv[2]    = 0;
      }
      else
      {
        ii              = 4*mbx + (i%2)*2;// + BLOCK_SIZE;
        jj              = 4*mby + (i/2)*2;
        if (currMB->b8mode[i]>=5 && currMB->b8mode[i]<=7)  // SMALL BLOCKS
        {
          pRegion->mv[0]  = (dec_picture->mv[LIST_0][jj][ii][0] + dec_picture->mv[LIST_0][jj][ii+1][0] + dec_picture->mv[LIST_0][jj+1][ii][0] + dec_picture->mv[LIST_0][jj+1][ii+1][0] + 2)/4;
          pRegion->mv[1]  = (dec_picture->mv[LIST_0][jj][ii][1] + dec_picture->mv[LIST_0][jj][ii+1][1] + dec_picture->mv[LIST_0][jj+1][ii][1] + dec_picture->mv[LIST_0][jj+1][ii+1][1] + 2)/4;
        }
        else // 16x16, 16x8, 8x16, 8x8
        {
          pRegion->mv[0]  = dec_picture->mv[LIST_0][jj][ii][0];
          pRegion->mv[1]  = dec_picture->mv[LIST_0][jj][ii][1];
//          pRegion->mv[0]  = dec_picture->mv[LIST_0][4*mby+(i/2)*2][4*mbx+(i%2)*2+BLOCK_SIZE][0];
//          pRegion->mv[1]  = dec_picture->mv[LIST_0][4*mby+(i/2)*2][4*mbx+(i%2)*2+BLOCK_SIZE][1];
        }
        erc_mvperMB      += mabs(pRegion->mv[0]) + mabs(pRegion->mv[1]);
        pRegion->mv[2]    = dec_picture->ref_idx[LIST_0][jj][ii];
      }
    }
  }
  else  //B-frame
  {
    for (i=0; i<4; i++)
    {
      ii                  = 4*mbx + (i%2)*2;// + BLOCK_SIZE;
      jj                  = 4*mby + (i/2)*2;
      pRegion             = currRegion + i;
      pRegion->regionMode = (currMB->mb_type  ==I16MB  ? REGMODE_INTRA      :
                             currMB->b8mode[i]==IBLOCK ? REGMODE_INTRA_8x8  : REGMODE_INTER_PRED_8x8);
      if (currMB->mb_type==I16MB || currMB->b8mode[i]==IBLOCK)  // INTRA
      {
        pRegion->mv[0]    = 0;
        pRegion->mv[1]    = 0;
        pRegion->mv[2]    = 0;
      }
      else
      {
        int idx = (dec_picture->ref_idx[0][jj][ii]<0)?1:0;
//        int idx = (currMB->b8mode[i]==0 && currMB->b8pdir[i]==2 ? LIST_0 : currMB->b8pdir[i]==1 ? LIST_1 : LIST_0);
//        int idx = currMB->b8pdir[i]==0 ? LIST_0 : LIST_1;
        mv                = dec_picture->mv[idx];
        pRegion->mv[0]    = (mv[jj][ii][0] + mv[jj][ii+1][0] + mv[jj+1][ii][0] + mv[jj+1][ii+1][0] + 2)/4;
        pRegion->mv[1]    = (mv[jj][ii][1] + mv[jj][ii+1][1] + mv[jj+1][ii][1] + mv[jj+1][ii+1][1] + 2)/4;
        erc_mvperMB      += mabs(pRegion->mv[0]) + mabs(pRegion->mv[1]);

        pRegion->mv[2]  = (dec_picture->ref_idx[idx][jj][ii]);
/*
        if (currMB->b8pdir[i]==0 || (currMB->b8pdir[i]==2 && currMB->b8mode[i]!=0)) // forward or bidirect
        {
          pRegion->mv[2]  = (dec_picture->ref_idx[LIST_0][jj][ii]);
          ///???? is it right, not only "img->fw_refFrArr[jj][ii-4]"
        }
        else
        {
          pRegion->mv[2]  = (dec_picture->ref_idx[LIST_1][jj][ii]);
//          pRegion->mv[2]  = 0;
        }
        */
      }
    }
  }
}

/*!
 ************************************************************************
 * \brief
 *    set defaults for old_slice
 *    NAL unit of a picture"
 ************************************************************************
 */
void init_old_slice()
{
  old_slice.field_pic_flag = 0;

  old_slice.pps_id = INT_MAX;

  old_slice.frame_num = INT_MAX;

  old_slice.nal_ref_idc = INT_MAX;

  old_slice.idr_flag = 0;

  old_slice.pic_oder_cnt_lsb          = UINT_MAX;
  old_slice.delta_pic_oder_cnt_bottom = INT_MAX;

  old_slice.delta_pic_order_cnt[0] = INT_MAX;
  old_slice.delta_pic_order_cnt[1] = INT_MAX;

}

/*!
 ************************************************************************
 * \brief
 *    save slice parameters that are needed for checking of "first VCL
 *    NAL unit of a picture"
 ************************************************************************
 */
void exit_slice()
{

  old_slice.pps_id = img->currentSlice->pic_parameter_set_id;

  old_slice.frame_num = img->frame_num;

  old_slice.field_pic_flag = img->field_pic_flag;

  if(img->field_pic_flag)
  {
    old_slice.bottom_field_flag = img->bottom_field_flag;
  }

  old_slice.nal_ref_idc   = img->nal_reference_idc;

  old_slice.idr_flag = img->idr_flag;
  if (img->idr_flag)
  {
    old_slice.idr_pic_id = img->idr_pic_id;
  }

  if (active_sps->pic_order_cnt_type == 0)
  {
    old_slice.pic_oder_cnt_lsb          = img->pic_order_cnt_lsb;
    old_slice.delta_pic_oder_cnt_bottom = img->delta_pic_order_cnt_bottom;
  }

  if (active_sps->pic_order_cnt_type == 1)
  {
    old_slice.delta_pic_order_cnt[0] = img->delta_pic_order_cnt[0];
    old_slice.delta_pic_order_cnt[1] = img->delta_pic_order_cnt[1];
  }
}

/*!
 ************************************************************************
 * \brief
 *    detect if current slice is "first VCL NAL unit of a picture"
 ************************************************************************
 */
int is_new_picture()
{
  int result=0;

  result |= (NULL==dec_picture);

  result |= (old_slice.pps_id != img->currentSlice->pic_parameter_set_id);

  result |= (old_slice.frame_num != img->frame_num);

  result |= (old_slice.field_pic_flag != img->field_pic_flag);

  if(img->field_pic_flag && old_slice.field_pic_flag)
  {
    result |= (old_slice.bottom_field_flag != img->bottom_field_flag);
  }

  result |= (old_slice.nal_ref_idc != img->nal_reference_idc) && ((old_slice.nal_ref_idc == 0) || (img->nal_reference_idc == 0));

  result |= ( old_slice.idr_flag != img->idr_flag);

  if (img->idr_flag && old_slice.idr_flag)
  {
    result |= (old_slice.idr_pic_id != img->idr_pic_id);
  }

  if (active_sps->pic_order_cnt_type == 0)
  {
    result |=  (old_slice.pic_oder_cnt_lsb          != img->pic_order_cnt_lsb);
    result |=  (old_slice.delta_pic_oder_cnt_bottom != img->delta_pic_order_cnt_bottom);
  }

  if (active_sps->pic_order_cnt_type == 1)
  {
    result |= (old_slice.delta_pic_order_cnt[0] != img->delta_pic_order_cnt[0]);
    result |= (old_slice.delta_pic_order_cnt[1] != img->delta_pic_order_cnt[1]);
  }

  return result;
}


/*!
 ************************************************************************
 * \brief
 *    decodes one slice
 ************************************************************************
 */
void decode_one_slice(struct img_par *img,struct inp_par *inp)
{

  Boolean end_of_slice = FALSE;
  int read_flag;
  img->cod_counter=-1;

  set_ref_pic_num();

  if (img->type == B_SLICE)
      compute_colocated(Co_located, listX);

  //reset_ec_flags();

  while (end_of_slice == FALSE) // loop over macroblocks
  {

#if TRACE
  fprintf(p_trace,"\n*********** POC: %i (I/P) MB: %i Slice: %i Type %d **********\n", img->ThisPOC, img->current_mb_nr, img->current_slice_nr, img->type);
#endif

    // Initializes the current macroblock
    start_macroblock(img, img->current_mb_nr);
    // Get the syntax elements from the NAL
    read_flag = read_one_macroblock(img,inp);
    decode_one_macroblock(img,inp);

    if(img->MbaffFrameFlag && dec_picture->mb_field[img->current_mb_nr])
    {
      img->num_ref_idx_l0_active >>= 1;
      img->num_ref_idx_l1_active >>= 1;
    }

    ercWriteMBMODEandMV(img,inp);

    end_of_slice=exit_macroblock(img,inp,(!img->MbaffFrameFlag||img->current_mb_nr%2));
  }

  exit_slice();
  //reset_ec_flags();

}


void decode_slice(struct img_par *img,struct inp_par *inp, int current_header)
{
  Slice *currSlice = img->currentSlice;

  if (active_pps->entropy_coding_mode_flag)
  {
    init_contexts (img);
    cabac_new_slice();
  }

  if ( (active_pps->weighted_bipred_idc > 0  && (img->type == B_SLICE)) || (active_pps->weighted_pred_flag && img->type !=I_SLICE))
    fill_wp_params(img);

  //printf("frame picture %d %d %d\n",img->structure,img->ThisPOC,img->direct_spatial_mv_pred_flag);


  // decode main slice information
  if ((current_header == SOP || current_header == SOS) && currSlice->ei_flag == 0)
    decode_one_slice(img,inp);

  // setMB-Nr in case this slice was lost
//  if(currSlice->ei_flag)
//    img->current_mb_nr = currSlice->last_mb_nr + 1;

}


/*!
 ************************************************************************
 * \brief
 *    Prepare field and frame buffer after frame decoding
 ************************************************************************
 */
void frame_postprocessing(struct img_par *img, struct inp_par *inp)
{
}

/*!
 ************************************************************************
 * \brief
 *    Prepare field and frame buffer after field decoding
 ************************************************************************
 */
void field_postprocessing(struct img_par *img, struct inp_par *inp)
{
  img->number /= 2;
}



void reset_wp_params(struct img_par *img)
{
  int i,comp;
  int log_weight_denom;

  for (i=0; i<MAX_REFERENCE_PICTURES; i++)
  {
    for (comp=0; comp<3; comp++)
    {
      log_weight_denom = (comp == 0) ? img->luma_log2_weight_denom : img->chroma_log2_weight_denom;
      img->wp_weight[0][i][comp] = 1<<log_weight_denom;
      img->wp_weight[1][i][comp] = 1<<log_weight_denom;
    }
  }
}


void fill_wp_params(struct img_par *img)
{
  int i, j, k;
  int comp;
  int log_weight_denom;
  int tb, td;
  int bframe = (img->type==B_SLICE);
  int max_bwd_ref, max_fwd_ref;
  int tx,DistScaleFactor;

  max_fwd_ref = img->num_ref_idx_l0_active;
  max_bwd_ref = img->num_ref_idx_l1_active;

  if (active_pps->weighted_bipred_idc == 2 && bframe)
  {
    img->luma_log2_weight_denom = 5;
    img->chroma_log2_weight_denom = 5;
    img->wp_round_luma = 16;
    img->wp_round_chroma = 16;

    for (i=0; i<MAX_REFERENCE_PICTURES; i++)
    {
      for (comp=0; comp<3; comp++)
      {
        log_weight_denom = (comp == 0) ? img->luma_log2_weight_denom : img->chroma_log2_weight_denom;
        img->wp_weight[0][i][comp] = 1<<log_weight_denom;
        img->wp_weight[1][i][comp] = 1<<log_weight_denom;
        img->wp_offset[0][i][comp] = 0;
        img->wp_offset[1][i][comp] = 0;
      }
    }
  }

  if (bframe)
  {
    for (i=0; i<max_fwd_ref; i++)
    {
      for (j=0; j<max_bwd_ref; j++)
      {
        for (comp = 0; comp<3; comp++)
        {
          log_weight_denom = (comp == 0) ? img->luma_log2_weight_denom : img->chroma_log2_weight_denom;
          if (active_pps->weighted_bipred_idc == 1)
          {
            img->wbp_weight[0][i][j][comp] =  img->wp_weight[0][i][comp];
            img->wbp_weight[1][i][j][comp] =  img->wp_weight[1][j][comp];
          }
          else if (active_pps->weighted_bipred_idc == 2)
          {
            td = iClip3(-128,127,listX[LIST_1][j]->poc - listX[LIST_0][i]->poc);
            if (td == 0 || listX[LIST_1][j]->is_long_term || listX[LIST_0][i]->is_long_term)
            {
              img->wbp_weight[0][i][j][comp] =   32;
              img->wbp_weight[1][i][j][comp] =   32;
            }
            else
            {
              tb = iClip3(-128,127,img->ThisPOC - listX[LIST_0][i]->poc);

              tx = (16384 + iabs(td/2))/td;
              DistScaleFactor = iClip3(-1024, 1023, (tx*tb + 32 )>>6);

              img->wbp_weight[1][i][j][comp] = DistScaleFactor >> 2;
              img->wbp_weight[0][i][j][comp] = 64 - img->wbp_weight[1][i][j][comp];
              if (img->wbp_weight[1][i][j][comp] < -64 || img->wbp_weight[1][i][j][comp] > 128)
              {
                img->wbp_weight[0][i][j][comp] = 32;
                img->wbp_weight[1][i][j][comp] = 32;
                img->wp_offset[0][i][comp] = 0;
                img->wp_offset[1][j][comp] = 0;
              }
            }
          }
        }
     }
   }
 }

  if (bframe && img->MbaffFrameFlag)
  {
    for (i=0; i<2*max_fwd_ref; i++)
    {
      for (j=0; j<2*max_bwd_ref; j++)
      {
        for (comp = 0; comp<3; comp++)
        {
          for (k=2; k<6; k+=2)
          {
            img->wp_offset[k+0][i][comp] = img->wp_offset[0][i/2][comp];
            img->wp_offset[k+1][j][comp] = img->wp_offset[1][j/2][comp];

            log_weight_denom = (comp == 0) ? img->luma_log2_weight_denom : img->chroma_log2_weight_denom;
            if (active_pps->weighted_bipred_idc == 1)
            {
              img->wbp_weight[k+0][i][j][comp] =  img->wp_weight[0][i/2][comp];
              img->wbp_weight[k+1][i][j][comp] =  img->wp_weight[1][j/2][comp];
            }
            else if (active_pps->weighted_bipred_idc == 2)
            {
              td = iClip3(-128,127,listX[k+LIST_1][j]->poc - listX[k+LIST_0][i]->poc);
              if (td == 0 || listX[k+LIST_1][j]->is_long_term || listX[k+LIST_0][i]->is_long_term)
              {
                img->wbp_weight[k+0][i][j][comp] =   32;
                img->wbp_weight[k+1][i][j][comp] =   32;
              }
              else
              {
                tb = iClip3(-128,127,((k==2)?img->toppoc:img->bottompoc) - listX[k+LIST_0][i]->poc);

                tx = (16384 + iabs(td/2))/td;
                DistScaleFactor = iClip3(-1024, 1023, (tx*tb + 32 )>>6);

                img->wbp_weight[k+1][i][j][comp] = DistScaleFactor >> 2;
                img->wbp_weight[k+0][i][j][comp] = 64 - img->wbp_weight[k+1][i][j][comp];
                if (img->wbp_weight[k+1][i][j][comp] < -64 || img->wbp_weight[k+1][i][j][comp] > 128)
                {
                  img->wbp_weight[k+1][i][j][comp] = 32;
                  img->wbp_weight[k+0][i][j][comp] = 32;
                  img->wp_offset[k+0][i][comp] = 0;
                  img->wp_offset[k+1][j][comp] = 0;
                }
              }
            }
          }
        }
      }
    }
  }
}

/*!
 ************************************************************************
 * \brief
 *    Error tracking: if current frame is lost or any reference frame of
 *                    current frame is lost, current frame is incorrect.
 ************************************************************************
 */
void Error_tracking()
{
  int i;

  if(img->redundant_pic_cnt == 0)
    {
      Is_primary_correct = Is_redundant_correct = 1;
    }

  if(img->redundant_pic_cnt == 0 && img->type != I_SLICE)
  {
    for(i=0;i<img->num_ref_idx_l0_active;i++)
    {
      if(ref_flag[i] == 0)  // any reference of primary slice is incorrect
      {
        Is_primary_correct = 0; // primary slice is incorrect
      }
    }
  }
  else if(img->redundant_pic_cnt != 0 && img->type != I_SLICE)
  {
    if(ref_flag[redundant_slice_ref_idx] == 0)  // reference of redundant slice is incorrect
    {
      Is_redundant_correct = 0;  // redundant slice is incorrect
    }
  }
}
