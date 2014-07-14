
/*!
 ************************************************************************
 * \file
 *     me_epzs.h
 *
 * \author
 *    Alexis Michael Tourapis        <alexis.tourapis@dolby.com>
 *
 * \date
 *    11. August 2006
 *
 * \brief
 *    Headerfile for EPZS motion estimation
 **************************************************************************
 */


#ifndef _ME_EPZS_H_
#define _ME_EPZS_H_


#define CHECK_RANGE  ((cand_x >= 0) && (cand_x < img_width  - blocksize_x) &&(cand_y >= 0) && (cand_y < img_height - blocksize_y))


typedef struct
{
  int         mb_adaptive_frame_field_flag;
  int         size_x, size_y;

  // Frame
  short ****  mv;            //!< motion vector       [list][subblock_x][subblock_y][component]
  // Top field
  short ****  top_mv;        //!< motion vector       [list][subblock_x][subblock_y][component]
  // Bottom field params
  short ****  bottom_mv;     //!< motion vector       [list][subblock_x][subblock_y][component]

} EPZSColocParams;

typedef struct
{
  int mv[2];
  int start_nmbr;
  int next_points;
}
SPoint;

typedef struct MEPatternNode
{
  int    searchPoints;
  SPoint *point;
  int    stopSearch;
  int    nextLast;
  struct MEPatternNode *nextpattern;
}
EPZSStructure;

typedef enum
{
  SDIAMOND  = 0,
  SQUARE    = 1,
  EDIAMOND  = 2,
  LDIAMOND  = 3,
  SBDIAMOND = 4
} EPZSPatterns;

extern EPZSColocParams *EPZSCo_located;
extern int ***EPZSDistortion;  //!< Array for storing SAD Values

extern int EPZSInit(void);
extern void EPZSDelete (void);
extern void EPZSOutputStats(FILE *stat,short stats_file);
extern void EPZSSliceInit(EPZSColocParams* p, StorablePicture **listX[6]);
extern int EPZSPelBlockMotionSearch (imgpel *, short, int, int, signed char ***, short ****,
                                     int, int, int, short[2], short[2], int, int, int);

extern int EPZSBiPredBlockMotionSearch (imgpel *, short, int, int, signed char  ***, short  ****,
                                        int, int, int, short*, short *,
                                        short[2], short[2], int, int, int);

extern int EPZSSubPelBlockMotionSearch (imgpel *, short, int, int, int, int, short[2],
                                        short[2], int, int, int, int*);

extern int EPZSSubPelBlockSearchBiPred  (imgpel* orig_pic, short ref, int list, int pic_pix_x, int pic_pix_y,
                                         int blocktype, short *pred_mv1, short *pred_mv2,
                                         short mv1[2], short mv2[2],
                                         int search_pos2, int search_pos4, int min_mcost, int *lambda_factor);

#endif

