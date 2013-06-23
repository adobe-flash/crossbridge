/*!
 *****************************************************************************
 *
 * \file fmo.c
 *
 * \brief
 *    Support for Flexible Macroblock Ordering for different Slice Group Modes: MBAmap handling
 *
 * \date
 *    16 June, 2002  Modified April 25, 2004
 *
 * \author
 *    Stephan Wenger   stewe@cs.tu-berlin.de
 *    Dong Wang        Dong.Wang@bristol.ac.uk
 *
 *****************************************************************************/

/*!
 ****************************************************************************
 *   Notes by Dong Wang (April 25 2004)
 *
 *  Source codes are modified to support 7 slice group types (fmo modes).
 *  The functions for generating map are very similar to that in decoder, but have
 *  a little difference.
 *
 *  The MB map is calculated at the beginning of coding of each picture (frame or field).
 *
 *  'slice_group_change_cycle' in structure 'ImageParameters' is the syntax in the slice
 *  header. It's set to be 1 before the initialization of FMO in function code_a_picture().
 *  It can be changed every time if needed.
 *
 ****************************************************************************
 */

/*!
 *****************************************************************************
 *  How does a MBAmap look like?
 *
 *  An MBAmap is a one-diemnsional array of ints.  Each int
 *  represents an MB in scan order.  A zero or positive value represents
 *  a slice group ID.  Negative values are reserved for future extensions.
 *  The numbering range for the SliceGroupIDs is 0..7 as per JVT-C167.
 *
 *  This module contains a static variable MBAmap.  This is the MBAmap of the
 *  picture currently coded.  It can be accessed only through the access
 *  functions.
 *****************************************************************************
*/

//#define PRINT_FMO_MAPS  1


#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <memory.h>

#include "global.h"

#include "fmo.h"


static int FirstMBInSlice[MAXSLICEGROUPIDS];

byte *MBAmap = NULL;
byte *MapUnitToSliceGroupMap = NULL;
unsigned int PicSizeInMapUnits;


static void FmoGenerateType0MapUnitMap (ImageParameters * img, pic_parameter_set_rbsp_t * pps);
static void FmoGenerateType1MapUnitMap (ImageParameters * img, pic_parameter_set_rbsp_t * pps);
static void FmoGenerateType2MapUnitMap (ImageParameters * img, pic_parameter_set_rbsp_t * pps);
static void FmoGenerateType3MapUnitMap (ImageParameters * img, pic_parameter_set_rbsp_t * pps);
static void FmoGenerateType4MapUnitMap (ImageParameters * img, pic_parameter_set_rbsp_t * pps);
static void FmoGenerateType5MapUnitMap (ImageParameters * img, pic_parameter_set_rbsp_t * pps);
static void FmoGenerateType6MapUnitMap (ImageParameters * img, pic_parameter_set_rbsp_t * pps);


static int FmoGenerateMapUnitToSliceGroupMap (ImageParameters * img, pic_parameter_set_rbsp_t * pps);
static int FmoGenerateMBAmap (ImageParameters * img, seq_parameter_set_rbsp_t* sps);


/*!
 ************************************************************************
 * \brief
 *    Generates MapUnitToSliceGroupMap
 *
 * \param img
 *    Image Parameter to be used for map generation
 * \param pps
 *    Picture Parameter set to be used for map generation
 *
 ************************************************************************
 */
static int FmoGenerateMapUnitToSliceGroupMap (ImageParameters * img, pic_parameter_set_rbsp_t * pps)
{
  PicSizeInMapUnits = img->PicHeightInMapUnits * img->PicWidthInMbs;


  if (pps->slice_group_map_type == 6)
  {
    if ((pps->pic_size_in_map_units_minus1+1) != PicSizeInMapUnits)
    {
      error ("wrong pps->pic_size_in_map_units_minus1 for used SPS and FMO type 6", 500);
    }
  }

  // allocate memory for MapUnitToSliceGroupMap
  if (MapUnitToSliceGroupMap)
    free (MapUnitToSliceGroupMap);

  if ((MapUnitToSliceGroupMap = malloc ((PicSizeInMapUnits) * sizeof (byte))) == NULL)
  {
    printf ("cannot allocated %d bytes for MapUnitToSliceGroupMap, exit\n", (int) ( PicSizeInMapUnits * sizeof (byte)));
    exit (-1);
  }

  if (pps->num_slice_groups_minus1 == 0)    // only one slice group
  {
    memset (MapUnitToSliceGroupMap, 0,  PicSizeInMapUnits * sizeof (byte));
    return 0;
  }

  switch (pps->slice_group_map_type)
  {
  case 0:
    FmoGenerateType0MapUnitMap (img, pps);
    break;
  case 1:
    FmoGenerateType1MapUnitMap (img, pps);
    break;
  case 2:
    FmoGenerateType2MapUnitMap (img, pps);
    break;
  case 3:
    FmoGenerateType3MapUnitMap (img, pps);
    break;
  case 4:
    FmoGenerateType4MapUnitMap (img, pps);
    break;
  case 5:
    FmoGenerateType5MapUnitMap (img, pps);
    break;
  case 6:
    FmoGenerateType6MapUnitMap (img, pps);
    break;
  default:
    printf ("Illegal slice_group_map_type %d , exit \n", pps->slice_group_map_type);
    exit (-1);
  }
  return 0;
}


/*!
 ************************************************************************
 * \brief
 *    Generates MBAmap from MapUnitToSliceGroupMap
 *
 * \param img
 *    Image Parameter to be used for map generation
 * \param sps
 *    Sequence Parameter set to be used for map generation
 *
 ************************************************************************
 */
static int FmoGenerateMBAmap (ImageParameters * img, seq_parameter_set_rbsp_t* sps)
{
  unsigned i;

  // allocate memory for MBAmap
  if (MBAmap)
    free (MBAmap);


  if ((MBAmap = malloc ((img->PicSizeInMbs) * sizeof (byte))) == NULL)
  {
    printf ("cannot allocated %d bytes for MBAmap, exit\n", (int) ((img->PicSizeInMbs) * sizeof (byte)));
    exit (-1);
  }

  if ((sps->frame_mbs_only_flag) || img->field_picture)
  {
    for (i=0; i<img->PicSizeInMbs; i++)
    {
      MBAmap[i] = MapUnitToSliceGroupMap[i];
    }
  }
  else
    if (sps->mb_adaptive_frame_field_flag  &&  (! img->field_picture))
    {
      for (i=0; i<img->PicSizeInMbs; i++)
      {
        MBAmap[i] = MapUnitToSliceGroupMap[i/2];
      }
    }
    else
    {
      for (i=0; i<img->PicSizeInMbs; i++)
      {
        MBAmap[i] = MapUnitToSliceGroupMap[(i/(2*img->PicWidthInMbs))*img->PicWidthInMbs+(i%img->PicWidthInMbs)];
      }
    }
    return 0;
}


/*!
 ************************************************************************
 * \brief
 *    FMO initialization: Generates MapUnitToSliceGroupMap and MBAmap.
 *
 * \param img
 *    Image Parameter to be used for map generation
 * \param pps
 *    Picture Parameter set to be used for map generation
 * \param sps
 *    Sequence Parameter set to be used for map generation
 ************************************************************************
 */
int FmoInit(ImageParameters * img, pic_parameter_set_rbsp_t * pps, seq_parameter_set_rbsp_t * sps)
{

#ifdef PRINT_FMO_MAPS
  unsigned i,j;
  int bottom;
#endif

  int k;
  for (k=0;k<MAXSLICEGROUPIDS;k++)
    FirstMBInSlice[k] = -1;



  FmoGenerateMapUnitToSliceGroupMap(img, pps);
  FmoGenerateMBAmap(img, sps);

#ifdef PRINT_FMO_MAPS
  printf("\n");
  printf("FMO Map (Units):\n");

  for (j=0; j<img->PicHeightInMapUnits; j++)
  {
    for (i=0; i<img->PicWidthInMbs; i++)
    {
      printf("%d ",MapUnitToSliceGroupMap[i+j*img->PicWidthInMbs]);
    }
    printf("\n");
  }
  printf("\n");

  if(sps->mb_adaptive_frame_field_flag==0)
  {
    printf("FMO Map (Mb):\n");
    for (j=0; j<(img->PicSizeInMbs/img->PicWidthInMbs); j++)
    {
      for (i=0; i<img->PicWidthInMbs; i++)
      {
        printf("%d ",MBAmap[i+j*img->PicWidthInMbs]);
      }
      printf("\n");
    }
    printf("\n");
  }
  else
  {
    printf("FMO Map (Mb in scan order for MBAFF):\n");
    for (j=0; j<(img->PicSizeInMbs/img->PicWidthInMbs); j++)
    {
      for (i=0; i<img->PicWidthInMbs; i++)
      {
        bottom=(j%2);
        printf("%d ",MBAmap[(j-bottom)*img->PicWidthInMbs+i*2+bottom]);
      }
      printf("\n");

    }
    printf("\n");

  }

#endif

  return 0;
}


/*!
 ************************************************************************
 * \brief
 *    Free memory if allocated by FMO functions
 ************************************************************************
 */
void FmoUninit()
{
  if (MBAmap)
  {
    free (MBAmap);
    MBAmap = NULL;
  }
  if (MapUnitToSliceGroupMap)
  {
    free (MapUnitToSliceGroupMap);
    MapUnitToSliceGroupMap = NULL;
  }

}


/*!
 ************************************************************************
 * \brief
 *    Generate interleaved slice group map type MapUnit map (type 0)
 *
 * \param img
 *    Image Parameter to be used for map generation
 * \param pps
 *    Picture Parameter set to be used for map generation
 ************************************************************************
 */
static void FmoGenerateType0MapUnitMap (ImageParameters * img, pic_parameter_set_rbsp_t * pps )
{
  unsigned iGroup, j;
  unsigned i = 0;
  do
  {
    for( iGroup = 0;
    (iGroup <= pps->num_slice_groups_minus1) && (i < PicSizeInMapUnits);
    i += pps->run_length_minus1[iGroup++] + 1)
    {
      for( j = 0; j <= pps->run_length_minus1[ iGroup ] && i + j < PicSizeInMapUnits; j++ )
        MapUnitToSliceGroupMap[i+j] = iGroup;
    }
  }
  while( i < PicSizeInMapUnits );
}


/*!
 ************************************************************************
 * \brief
 *    Generate dispersed slice group map type MapUnit map (type 1)
 *
 * \param img
 *    Image Parameter to be used for map generation
 * \param pps
 *    Picture Parameter set to be used for map generation
 ************************************************************************
 */
static void FmoGenerateType1MapUnitMap (ImageParameters * img, pic_parameter_set_rbsp_t * pps )
{
  unsigned i;
  for( i = 0; i < PicSizeInMapUnits; i++ )
  {
    MapUnitToSliceGroupMap[i] = ((i%img->PicWidthInMbs)+(((i/img->PicWidthInMbs)*(pps->num_slice_groups_minus1+1))/2))
      %(pps->num_slice_groups_minus1+1);
  }
}

/*!
 ************************************************************************
 * \brief
 *    Generate foreground with left-over slice group map type MapUnit map (type 2)
 *
 * \param img
 *    Image Parameter to be used for map generation
 * \param pps
 *    Picture Parameter set to be used for map generation
 ************************************************************************
 */
static void FmoGenerateType2MapUnitMap (ImageParameters * img, pic_parameter_set_rbsp_t * pps )
{
  int iGroup;
  unsigned i, x, y;
  unsigned yTopLeft, xTopLeft, yBottomRight, xBottomRight;

  for( i = 0; i < PicSizeInMapUnits; i++ )
    MapUnitToSliceGroupMap[ i ] = pps->num_slice_groups_minus1;

  for( iGroup = pps->num_slice_groups_minus1 - 1 ; iGroup >= 0; iGroup-- )
  {
    yTopLeft = pps->top_left[ iGroup ] / img->PicWidthInMbs;
    xTopLeft = pps->top_left[ iGroup ] % img->PicWidthInMbs;
    yBottomRight = pps->bottom_right[ iGroup ] / img->PicWidthInMbs;
    xBottomRight = pps->bottom_right[ iGroup ] % img->PicWidthInMbs;
    for( y = yTopLeft; y <= yBottomRight; y++ )
      for( x = xTopLeft; x <= xBottomRight; x++ )
        MapUnitToSliceGroupMap[ y * img->PicWidthInMbs + x ] = iGroup;
  }
}


/*!
 ************************************************************************
 * \brief
 *    Generate box-out slice group map type MapUnit map (type 3)
 *
 * \param img
 *    Image Parameter to be used for map generation
 * \param pps
 *    Picture Parameter set to be used for map generation
 ************************************************************************
 */
static void FmoGenerateType3MapUnitMap (ImageParameters * img, pic_parameter_set_rbsp_t * pps )
{
  unsigned i, k;
  int leftBound, topBound, rightBound, bottomBound;
  int x, y, xDir, yDir;
  int mapUnitVacant;

  unsigned mapUnitsInSliceGroup0 = imin((pps->slice_group_change_rate_minus1 + 1) * img->slice_group_change_cycle, PicSizeInMapUnits);

  for( i = 0; i < PicSizeInMapUnits; i++ )
    MapUnitToSliceGroupMap[ i ] = 2;

  x = ( img->PicWidthInMbs - pps->slice_group_change_direction_flag ) / 2;
  y = ( img->PicHeightInMapUnits - pps->slice_group_change_direction_flag ) / 2;

  leftBound   = x;
  topBound    = y;
  rightBound  = x;
  bottomBound = y;

  xDir =  pps->slice_group_change_direction_flag - 1;
  yDir =  pps->slice_group_change_direction_flag;

  for( k = 0; k < PicSizeInMapUnits; k += mapUnitVacant )
  {
    mapUnitVacant = ( MapUnitToSliceGroupMap[ y * img->PicWidthInMbs + x ]  ==  2 );
    if( mapUnitVacant )
      MapUnitToSliceGroupMap[ y * img->PicWidthInMbs + x ] = ( k >= mapUnitsInSliceGroup0 );

    if( xDir  ==  -1  &&  x  ==  leftBound )
    {
      leftBound = imax( leftBound - 1, 0 );
      x = leftBound;
      xDir = 0;
      yDir = 2 * pps->slice_group_change_direction_flag - 1;
    }
    else
      if( xDir  ==  1  &&  x  ==  rightBound )
      {
        rightBound = imin( rightBound + 1, (int)img->PicWidthInMbs - 1 );
        x = rightBound;
        xDir = 0;
        yDir = 1 - 2 * pps->slice_group_change_direction_flag;
      }
      else
        if( yDir  ==  -1  &&  y  ==  topBound )
        {
          topBound = imax( topBound - 1, 0 );
          y = topBound;
          xDir = 1 - 2 * pps->slice_group_change_direction_flag;
          yDir = 0;
        }
        else
          if( yDir  ==  1  &&  y  ==  bottomBound )
          {
            bottomBound = imin( bottomBound + 1, (int)img->PicHeightInMapUnits - 1 );
            y = bottomBound;
            xDir = 2 * pps->slice_group_change_direction_flag - 1;
            yDir = 0;
          }
          else
          {
            x = x + xDir;
            y = y + yDir;
          }
  }

}

/*!
 ************************************************************************
 * \brief
 *    Generate raster scan slice group map type MapUnit map (type 4)
 *
 * \param img
 *    Image Parameter to be used for map generation
 * \param pps
 *    Picture Parameter set to be used for map generation
 ************************************************************************
 */
static void FmoGenerateType4MapUnitMap (ImageParameters * img, pic_parameter_set_rbsp_t * pps )
{

  unsigned mapUnitsInSliceGroup0 = imin((pps->slice_group_change_rate_minus1 + 1) * img->slice_group_change_cycle, PicSizeInMapUnits);
  unsigned sizeOfUpperLeftGroup = pps->slice_group_change_direction_flag ? ( PicSizeInMapUnits - mapUnitsInSliceGroup0 ) : mapUnitsInSliceGroup0;

  unsigned i;

  for( i = 0; i < PicSizeInMapUnits; i++ )
    if( i < sizeOfUpperLeftGroup )
      MapUnitToSliceGroupMap[ i ] = pps->slice_group_change_direction_flag;
    else
      MapUnitToSliceGroupMap[ i ] = 1 - pps->slice_group_change_direction_flag;

}

/*!
 ************************************************************************
 * \brief
 *    Generate wipe slice group map type MapUnit map (type 5)
 *
 * \param img
 *    Image Parameter to be used for map generation
 * \param pps
 *    Picture Parameter set to be used for map generation
 ************************************************************************
*/
static void FmoGenerateType5MapUnitMap (ImageParameters * img, pic_parameter_set_rbsp_t * pps )
{

  unsigned mapUnitsInSliceGroup0 = imin((pps->slice_group_change_rate_minus1 + 1) * img->slice_group_change_cycle, PicSizeInMapUnits);
  unsigned sizeOfUpperLeftGroup = pps->slice_group_change_direction_flag ? ( PicSizeInMapUnits - mapUnitsInSliceGroup0 ) : mapUnitsInSliceGroup0;

  unsigned i,j, k = 0;

  for( j = 0; j < img->PicWidthInMbs; j++ )
    for( i = 0; i < img->PicHeightInMapUnits; i++ )
      if( k++ < sizeOfUpperLeftGroup )
        MapUnitToSliceGroupMap[ i * img->PicWidthInMbs + j ] = pps->slice_group_change_direction_flag;
      else
        MapUnitToSliceGroupMap[ i * img->PicWidthInMbs + j ] = 1 - pps->slice_group_change_direction_flag;

}

/*!
 ************************************************************************
 * \brief
 *    Generate explicit slice group map type MapUnit map (type 6)
 *
 * \param img
 *    Image Parameter to be used for map generation
 * \param pps
 *    Picture Parameter set to be used for map generation
 ************************************************************************
 */
static void FmoGenerateType6MapUnitMap (ImageParameters * img, pic_parameter_set_rbsp_t * pps )
{
  unsigned i;
  for (i=0; i<PicSizeInMapUnits; i++)
  {
    MapUnitToSliceGroupMap[i] = pps->slice_group_id[i];
  }
}

/*!
 ************************************************************************
 * \brief
 *    FmoStartPicture: initializes FMO at the begin of each new picture
 *
 * \par Input:
 *    None
 ************************************************************************
 */
int FmoStartPicture ()
{
  int i;

  assert (MBAmap != NULL);

  for (i=0; i<MAXSLICEGROUPIDS; i++)
    FirstMBInSlice[i] = FmoGetFirstMBOfSliceGroup (i);
  return 0;
}



/*!
 ************************************************************************
 * \brief
 *    FmoEndPicture: Ends the Scattered Slices Module (called once
 *    per picture).
 *
 * \par Input:
 *    None
 ************************************************************************
 */
int FmoEndPicture ()
{
  // Do nothing
  return 0;
}


/*!
 ************************************************************************
 * \brief
 *    FmoMB2Slice: Returns SliceID for a given MB
 *
 * \par Input:
 *    Macroblock Nr (in scan order)
 ************************************************************************
 */
int FmoMB2SliceGroup ( int mb)
{
  assert (mb < (int)img->PicSizeInMbs);
  assert (MBAmap != NULL);
  return MBAmap[mb];
}

/*!
 ************************************************************************
 * \brief
 *    FmoGetNextMBBr: Returns the MB-Nr (in scan order) of the next
 *    MB in the (FMO) Slice, -1 if the SliceGroup is finished
 *
 * \par Input:
 *    CurrentMbNr
 ************************************************************************
 */
int FmoGetNextMBNr (int CurrentMbNr)
{

  int  SliceGroupID = FmoMB2SliceGroup (CurrentMbNr);

  while (++CurrentMbNr<(int)img->PicSizeInMbs &&  MBAmap[CurrentMbNr] != SliceGroupID)
    ;

  if (CurrentMbNr >= (int)img->PicSizeInMbs)
    return -1;    // No further MB in this slice (could be end of picture)
  else
    return CurrentMbNr;
}


/*!
 ************************************************************************
 * \brief
 *    FmoGetNextMBBr: Returns the MB-Nr (in scan order) of the next
 *    MB in the (FMO) Slice, -1 if the SliceGroup is finished
 *
 * \par Input:
 *    CurrentMbNr
 ************************************************************************
 */
int FmoGetPreviousMBNr (int CurrentMbNr)
{

  int  SliceGroupID = FmoMB2SliceGroup (CurrentMbNr);
  CurrentMbNr--;
  while (CurrentMbNr>=0 &&  MBAmap[CurrentMbNr] != SliceGroupID)
    CurrentMbNr--;

  if (CurrentMbNr < 0)
    return -1;    // No previous MB in this slice
  else
    return CurrentMbNr;
}


/*!
 ************************************************************************
 * \brief
 *    FmoGetFirstMBOfSliceGroup: Returns the MB-Nr (in scan order) of the
 *    next first MB of the Slice group, -1 if no such MB exists
 *
 * \par Input:
 *    SliceGroupID: Id of SliceGroup
 ************************************************************************
 */
int FmoGetFirstMBOfSliceGroup (int SliceGroupID)
{
  int i = 0;
  while ((i<(int)img->PicSizeInMbs) && (FmoMB2SliceGroup (i) != SliceGroupID))
    i++;

  if (i < (int)img->PicSizeInMbs)
    return i;
  else
    return -1;
}


/*!
 ************************************************************************
 * \brief
 *    FmoGetLastCodedMBOfSlice: Returns the MB-Nr (in scan order) of
 *    the last MB of the slice group
 *
 * \par Input:
 *    SliceGroupID
 * \par Return
 *    MB Nr in case of success (is always >= 0)
 *    -1 if the SliceGroup doesn't exist
 ************************************************************************
 */
int FmoGetLastCodedMBOfSliceGroup (int SliceGroupID)
{
  int i;
  int LastMB = -1;

  for (i=0; i<(int)img->PicSizeInMbs; i++)
    if (FmoMB2SliceGroup (i) == SliceGroupID)
      LastMB = i;
  return LastMB;
}


void FmoSetLastMacroblockInSlice ( int mb)
{
  // called by terminate_slice(), writes the last processed MB into the
  // FirstMBInSlice[MAXSLICEGROUPIDS] array.  FmoGetFirstMacroblockInSlice()
  // uses this info to identify the first uncoded MB in each slice group

  int currSliceGroup = FmoMB2SliceGroup (mb);
  assert (mb >= 0);
  mb = FmoGetNextMBNr (mb);   // The next (still uncoded) MB, or -1 if SG is finished
  FirstMBInSlice[currSliceGroup] = mb;
}

int FmoGetFirstMacroblockInSlice ( int SliceGroup)
{
  return FirstMBInSlice[SliceGroup];
  // returns the first uncoded MB in each slice group, -1 if there is no
  // more to do in this slice group
}


int FmoSliceGroupCompletelyCoded( int SliceGroupID)
{
  if (FmoGetFirstMacroblockInSlice (SliceGroupID) < 0)  // slice group completelty coded or not present
    return TRUE;
  else
    return FALSE;
}



