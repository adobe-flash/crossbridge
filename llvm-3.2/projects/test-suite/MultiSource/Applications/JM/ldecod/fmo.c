
/*!
 *****************************************************************************
 *
 * \file fmo.c
 *
 * \brief
 *    Support for Flexible Macroblock Ordering (FMO)
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *    - Stephan Wenger      stewe@cs.tu-berlin.de
 *    - Karsten Suehring    suehring@hhi.de
 ******************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#include "global.h"
#include "elements.h"
#include "defines.h"
#include "header.h"
#include "fmo.h"

//#define PRINT_FMO_MAPS

int *MbToSliceGroupMap = NULL;
int *MapUnitToSliceGroupMap = NULL;

static int NumberOfSliceGroups;    // the number of slice groups -1 (0 == scan order, 7 == maximum)

static void FmoGenerateType0MapUnitMap (pic_parameter_set_rbsp_t* pps, seq_parameter_set_rbsp_t* sps, unsigned PicSizeInMapUnits );
static void FmoGenerateType1MapUnitMap (pic_parameter_set_rbsp_t* pps, seq_parameter_set_rbsp_t* sps, unsigned PicSizeInMapUnits );
static void FmoGenerateType2MapUnitMap (pic_parameter_set_rbsp_t* pps, seq_parameter_set_rbsp_t* sps, unsigned PicSizeInMapUnits );
static void FmoGenerateType3MapUnitMap (pic_parameter_set_rbsp_t* pps, seq_parameter_set_rbsp_t* sps, unsigned PicSizeInMapUnits );
static void FmoGenerateType4MapUnitMap (pic_parameter_set_rbsp_t* pps, seq_parameter_set_rbsp_t* sps, unsigned PicSizeInMapUnits );
static void FmoGenerateType5MapUnitMap (pic_parameter_set_rbsp_t* pps, seq_parameter_set_rbsp_t* sps, unsigned PicSizeInMapUnits );
static void FmoGenerateType6MapUnitMap (pic_parameter_set_rbsp_t* pps, seq_parameter_set_rbsp_t* sps, unsigned PicSizeInMapUnits );


/*!
 ************************************************************************
 * \brief
 *    Generates MapUnitToSliceGroupMap
 *    Has to be called every time a new Picture Parameter Set is used
 *
 * \param pps
 *    Picture Parameter set to be used for map generation
 * \param sps
 *    Sequence Parameter set to be used for map generation
 *
 ************************************************************************
 */
static int FmoGenerateMapUnitToSliceGroupMap (pic_parameter_set_rbsp_t* pps, seq_parameter_set_rbsp_t* sps)
{
  unsigned int NumSliceGroupMapUnits;

  NumSliceGroupMapUnits = (sps->pic_height_in_map_units_minus1+1)* (sps->pic_width_in_mbs_minus1+1);

  if (pps->slice_group_map_type == 6)
  {
    if ((pps->num_slice_group_map_units_minus1+1) != NumSliceGroupMapUnits)
    {
      error ("wrong pps->num_slice_group_map_units_minus1 for used SPS and FMO type 6", 500);
    }
  }

  // allocate memory for MapUnitToSliceGroupMap
  if (MapUnitToSliceGroupMap)
    free (MapUnitToSliceGroupMap);
  if ((MapUnitToSliceGroupMap = malloc ((NumSliceGroupMapUnits) * sizeof (int))) == NULL)
  {
    printf ("cannot allocated %d bytes for MapUnitToSliceGroupMap, exit\n", (int) ( (pps->num_slice_group_map_units_minus1+1) * sizeof (int)));
    exit (-1);
  }

  if (pps->num_slice_groups_minus1 == 0)    // only one slice group
  {
    memset (MapUnitToSliceGroupMap, 0, NumSliceGroupMapUnits * sizeof (int));
    return 0;
  }

  switch (pps->slice_group_map_type)
  {
  case 0:
    FmoGenerateType0MapUnitMap (pps, sps, NumSliceGroupMapUnits);
    break;
  case 1:
    FmoGenerateType1MapUnitMap (pps, sps, NumSliceGroupMapUnits);
    break;
  case 2:
    FmoGenerateType2MapUnitMap (pps, sps, NumSliceGroupMapUnits);
    break;
  case 3:
    FmoGenerateType3MapUnitMap (pps, sps, NumSliceGroupMapUnits);
    break;
  case 4:
    FmoGenerateType4MapUnitMap (pps, sps, NumSliceGroupMapUnits);
    break;
  case 5:
    FmoGenerateType5MapUnitMap (pps, sps, NumSliceGroupMapUnits);
    break;
  case 6:
    FmoGenerateType6MapUnitMap (pps, sps, NumSliceGroupMapUnits);
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
 *    Generates MbToSliceGroupMap from MapUnitToSliceGroupMap
 *
 * \param pps
 *    Picture Parameter set to be used for map generation
 * \param sps
 *    Sequence Parameter set to be used for map generation
 *
 ************************************************************************
 */
static int FmoGenerateMbToSliceGroupMap (pic_parameter_set_rbsp_t* pps, seq_parameter_set_rbsp_t* sps)
{
  unsigned i;

  // allocate memory for MbToSliceGroupMap
  if (MbToSliceGroupMap)
    free (MbToSliceGroupMap);

  if ((MbToSliceGroupMap = malloc ((img->PicSizeInMbs) * sizeof (int))) == NULL)
  {
    printf ("cannot allocate %d bytes for MbToSliceGroupMap, exit\n", (int) ((img->PicSizeInMbs) * sizeof (int)));
    exit (-1);
  }


  if ((sps->frame_mbs_only_flag)|| img->field_pic_flag)
  {
    for (i=0; i<img->PicSizeInMbs; i++)
    {
      MbToSliceGroupMap[i] = MapUnitToSliceGroupMap[i];
    }
  }
  else
    if (sps->mb_adaptive_frame_field_flag  &&  (!img->field_pic_flag))
    {
      for (i=0; i<img->PicSizeInMbs; i++)
      {
        MbToSliceGroupMap[i] = MapUnitToSliceGroupMap[i/2];
      }
    }
    else
    {
      for (i=0; i<img->PicSizeInMbs; i++)
      {
        MbToSliceGroupMap[i] = MapUnitToSliceGroupMap[(i/(2*img->PicWidthInMbs))*img->PicWidthInMbs+(i%img->PicWidthInMbs)];
      }
    }
  return 0;
}


/*!
 ************************************************************************
 * \brief
 *    FMO initialization: Generates MapUnitToSliceGroupMap and MbToSliceGroupMap.
 *
 * \param pps
 *    Picture Parameter set to be used for map generation
 * \param sps
 *    Sequence Parameter set to be used for map generation
 ************************************************************************
 */
int FmoInit(pic_parameter_set_rbsp_t* pps, seq_parameter_set_rbsp_t* sps)
{
#ifdef PRINT_FMO_MAPS
  unsigned i,j;
#endif

  FmoGenerateMapUnitToSliceGroupMap(pps, sps);
  FmoGenerateMbToSliceGroupMap(pps, sps);

  NumberOfSliceGroups = pps->num_slice_groups_minus1+1;

#ifdef PRINT_FMO_MAPS
  printf("\n");
  printf("FMO Map (Units):\n");

  for (j=0; j<img->PicHeightInMapUnits; j++)
  {
    for (i=0; i<img->PicWidthInMbs; i++)
    {
      printf("%c",48+MapUnitToSliceGroupMap[i+j*img->PicWidthInMbs]);
    }
    printf("\n");
  }
  printf("\n");
  printf("FMO Map (Mb):\n");

  for (j=0; j<img->PicHeightInMbs; j++)
  {
    for (i=0; i<img->PicWidthInMbs; i++)
    {
      printf("%c",48+MbToSliceGroupMap[i+j*img->PicWidthInMbs]);
    }
    printf("\n");
  }
  printf("\n");

#endif

  return 0;
}


/*!
 ************************************************************************
 * \brief
 *    Free memory allocated by FMO functions
 ************************************************************************
 */
int FmoFinit()
{
  if (MbToSliceGroupMap)
  {
    free (MbToSliceGroupMap);
    MbToSliceGroupMap = NULL;
  }
  if (MapUnitToSliceGroupMap)
  {
    free (MapUnitToSliceGroupMap);
    MapUnitToSliceGroupMap = NULL;
  }
  return 0;
}


/*!
 ************************************************************************
 * \brief
 *    FmoGetNumberOfSliceGroup()
 *
 * \par Input:
 *    None
 ************************************************************************
 */
int FmoGetNumberOfSliceGroup()
{
  return NumberOfSliceGroups;
}


/*!
 ************************************************************************
 * \brief
 *    FmoGetLastMBOfPicture()
 *    returns the macroblock number of the last MB in a picture.  This
 *    mb happens to be the last macroblock of the picture if there is only
 *    one slice group
 *
 * \par Input:
 *    None
 ************************************************************************
 */
int FmoGetLastMBOfPicture()
{
  return FmoGetLastMBInSliceGroup (FmoGetNumberOfSliceGroup()-1);
}


/*!
 ************************************************************************
 * \brief
 *    FmoGetLastMBInSliceGroup: Returns MB number of last MB in SG
 *
 * \par Input:
 *    SliceGroupID (0 to 7)
 ************************************************************************
 */

int FmoGetLastMBInSliceGroup (int SliceGroup)
{
  int i;

  for (i=img->PicSizeInMbs-1; i>=0; i--)
    if (FmoGetSliceGroupId (i) == SliceGroup)
      return i;
  return -1;

}


/*!
 ************************************************************************
 * \brief
 *    Returns SliceGroupID for a given MB
 *
 * \param mb
 *    Macroblock number (in scan order)
 ************************************************************************
 */
int FmoGetSliceGroupId (int mb)
{
  assert (mb < (int)img->PicSizeInMbs);
  assert (MbToSliceGroupMap != NULL);
  return MbToSliceGroupMap[mb];
}


/*!
 ************************************************************************
 * \brief
 *    FmoGetNextMBBr: Returns the MB-Nr (in scan order) of the next
 *    MB in the (scattered) Slice, -1 if the slice is finished
 *
 * \param CurrentMbNr
 *    number of the current macroblock
 ************************************************************************
 */
int FmoGetNextMBNr (int CurrentMbNr)
{
  int SliceGroup = FmoGetSliceGroupId (CurrentMbNr);

  while (++CurrentMbNr<(int)img->PicSizeInMbs && MbToSliceGroupMap [CurrentMbNr] != SliceGroup)
    ;

  if (CurrentMbNr >= (int)img->PicSizeInMbs)
    return -1;    // No further MB in this slice (could be end of picture)
  else
    return CurrentMbNr;
}


/*!
 ************************************************************************
 * \brief
 *    Generate interleaved slice group map type MapUnit map (type 0)
 *
 ************************************************************************
 */
static void FmoGenerateType0MapUnitMap (pic_parameter_set_rbsp_t* pps, seq_parameter_set_rbsp_t* sps, unsigned PicSizeInMapUnits )
{
  unsigned iGroup, j;
  unsigned i = 0;
  do
  {
    for( iGroup = 0;
         (iGroup <= pps->num_slice_groups_minus1) && (i < PicSizeInMapUnits);
         i += pps->run_length_minus1[iGroup++] + 1 )
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
 ************************************************************************
 */
static void FmoGenerateType1MapUnitMap (pic_parameter_set_rbsp_t* pps, seq_parameter_set_rbsp_t* sps, unsigned PicSizeInMapUnits )
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
 ************************************************************************
 */
static void FmoGenerateType2MapUnitMap (pic_parameter_set_rbsp_t* pps, seq_parameter_set_rbsp_t* sps, unsigned PicSizeInMapUnits )
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
 ************************************************************************
 */
static void FmoGenerateType3MapUnitMap (pic_parameter_set_rbsp_t* pps, seq_parameter_set_rbsp_t* sps, unsigned PicSizeInMapUnits )
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
 ************************************************************************
 */
static void FmoGenerateType4MapUnitMap (pic_parameter_set_rbsp_t* pps, seq_parameter_set_rbsp_t* sps, unsigned PicSizeInMapUnits )
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
 ************************************************************************
 */
static void FmoGenerateType5MapUnitMap (pic_parameter_set_rbsp_t* pps, seq_parameter_set_rbsp_t* sps, unsigned PicSizeInMapUnits )
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
 ************************************************************************
 */
static void FmoGenerateType6MapUnitMap (pic_parameter_set_rbsp_t* pps, seq_parameter_set_rbsp_t* sps, unsigned PicSizeInMapUnits )
{
  unsigned i;
  for (i=0; i<PicSizeInMapUnits; i++)
  {
    MapUnitToSliceGroupMap[i] = pps->slice_group_id[i];
  }
}

