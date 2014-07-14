
/*!
 ************************************************************************
 * \file  erc_api.h
 *
 * \brief
 *      External (still inside video decoder) interface for error concealment module
 *
 * \author
 *      - Ari Hourunranta                <ari.hourunranta@nokia.com>
 *      - Ye-Kui Wang                   <wyk@ieee.org>
 *      - Jill Boyce                     <jill.boyce@thomson.net>
 *      - Saurav K Bandyopadhyay         <saurav@ieee.org>
 *      - Zhenyu Wu                      <Zhenyu.Wu@thomson.net
 *      - Purvin Pandit                  <Purvin.Pandit@thomson.net>
 *
 * ************************************************************************
 */


#ifndef _ERC_API_H_
#define _ERC_API_H_

#include "erc_globals.h"

/*
* Defines
*/

/* If the average motion vector of the correctly received macroblocks is less than the
threshold, concealByCopy is used, otherwise concealByTrial is used. */
#define MVPERMB_THR 8

/* used to determine the size of the allocated memory for a temporal Region (MB) */
#define DEF_REGION_SIZE 384  /* 8*8*6 */

#define ERC_BLOCK_OK                3
#define ERC_BLOCK_CONCEALED         2
#define ERC_BLOCK_CORRUPTED         1
#define ERC_BLOCK_EMPTY             0

#define mabs(a) ( (a) < 0 ? -(a) : (a) )
#define mmax(a,b) ((a) > (b) ? (a) : (b))
#define mmin(a,b) ((a) < (b) ? (a) : (b))

/*
* Functions to convert MBNum representation to blockNum
*/

#define xPosYBlock(currYBlockNum,picSizeX) \
((currYBlockNum)%((picSizeX)>>3))

#define yPosYBlock(currYBlockNum,picSizeX) \
((currYBlockNum)/((picSizeX)>>3))

#define xPosMB(currMBNum,picSizeX) \
((currMBNum)%((picSizeX)>>4))

#define yPosMB(currMBNum,picSizeX) \
((currMBNum)/((picSizeX)>>4))

#define MBxy2YBlock(currXPos,currYPos,comp,picSizeX) \
((((currYPos)<<1)+((comp)>>1))*((picSizeX)>>3)+((currXPos)<<1)+((comp)&1))

#define MBNum2YBlock(currMBNum,comp,picSizeX) \
MBxy2YBlock(xPosMB((currMBNum),(picSizeX)),yPosMB((currMBNum),(picSizeX)),(comp),(picSizeX))


/*
* typedefs
*/

/* segment data structure */
typedef struct ercSegment_s
{
  int      startMBPos;
  int      endMBPos;
  int      fCorrupted;
} ercSegment_t;

/* Error detector & concealment instance data structure */
typedef struct ercVariables_s
{
  /*  Number of macroblocks (size or size/4 of the arrays) */
  int   nOfMBs;
  /* Number of segments (slices) in frame */
  int     nOfSegments;

  /*  Array for conditions of Y blocks */
  int     *yCondition;
  /*  Array for conditions of U blocks */
  int     *uCondition;
  /*  Array for conditions of V blocks */
  int     *vCondition;

  /* Array for Slice level information */
  ercSegment_t *segments;
  int     currSegment;

  /* Conditions of the MBs of the previous frame */
  int   *prevFrameYCondition;

  /* Flag telling if the current segment was found to be corrupted */
  int   currSegmentCorrupted;
  /* Counter for corrupted segments per picture */
  int   nOfCorruptedSegments;

  /* State variables for error detector and concealer */
  int   concealment;

} ercVariables_t;

/*
* External function interface
*/

void ercInit(int pic_sizex, int pic_sizey, int flag);
ercVariables_t *ercOpen( void );
void ercReset( ercVariables_t *errorVar, int nOfMBs, int numOfSegments, int picSizeX );
void ercClose( ercVariables_t *errorVar );
void ercSetErrorConcealment( ercVariables_t *errorVar, int value );

void ercStartSegment( int currMBNum, int segment, unsigned int bitPos, ercVariables_t *errorVar );
void ercStopSegment( int currMBNum, int segment, unsigned int bitPos, ercVariables_t *errorVar );
void ercMarkCurrSegmentLost(int picSizeX, ercVariables_t *errorVar );
void ercMarkCurrSegmentOK(int picSizeX, ercVariables_t *errorVar );
void ercMarkCurrMBConcealed( int currMBNum, int comp, int picSizeX, ercVariables_t *errorVar );

int ercConcealIntraFrame( frame *recfr, int picSizeX, int picSizeY, ercVariables_t *errorVar );
int ercConcealInterFrame( frame *recfr, objectBuffer_t *object_list,
                          int picSizeX, int picSizeY, ercVariables_t *errorVar, int chroma_format_idc );


/* Thomson APIs for concealing entire frame loss */

#include "mbuffer.h"
#include "output.h"

struct concealment_node {
    StorablePicture* picture;
    int  missingpocs;
    struct concealment_node *next;
};

struct concealment_node * init_node(StorablePicture* , int );
void print_node( struct concealment_node * );
void print_list( struct concealment_node * );
void add_node( struct concealment_node * );
void delete_node( struct concealment_node * );
void init_lists_for_non_reference_loss(int , PictureStructure );

void conceal_non_ref_pics(int diff);
void conceal_lost_frames(ImageParameters *img);

void sliding_window_poc_management(StorablePicture *p);

void write_lost_non_ref_pic(int poc, int p_out);
void write_lost_ref_after_idr(int pos);

FrameStore *last_out_fs;
int pocs_in_dpb[100];
int comp(const void *, const void *);


#endif

