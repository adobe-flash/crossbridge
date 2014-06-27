
/*!
************************************************************************
* \file  memalloc.c
*
* \brief
*    Memory allocation and free helper funtions
*
* \author
*    Main contributors (see contributors.h for copyright, address and affiliation details)
************************************************************************
*/

#include <stdlib.h>
#include "memalloc.h"


/*!
 ************************************************************************
 * \brief
 *    Allocate 2D memory array -> imgpel array2D[rows][columns]
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************/
int get_mem2Dpel(imgpel ***array2D, int rows, int columns)
{
  int i;

  if((*array2D      = (imgpel**)calloc(rows,        sizeof(imgpel*))) == NULL)
    no_mem_exit("get_mem2Dpel: array2D");
  if(((*array2D)[0] = (imgpel* )calloc(rows*columns,sizeof(imgpel ))) == NULL)
    no_mem_exit("get_mem2Dpel: array2D");

  for(i=1 ; i<rows ; i++)
    (*array2D)[i] =  (*array2D)[i-1] + columns  ;

  return rows*columns*sizeof(imgpel);
}

/*!
 ************************************************************************
 * \brief
 *    Allocate 3D memory array -> imgpel array3D[frames][rows][columns]
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************
 */
int get_mem3Dpel(imgpel ****array3D, int frames, int rows, int columns)
{
  int  j;

  if(((*array3D) = (imgpel***)calloc(frames,sizeof(imgpel**))) == NULL)
    no_mem_exit("get_mem3Dpel: array3D");

  for(j=0;j<frames;j++)
    get_mem2Dpel( (*array3D)+j, rows, columns ) ;

  return frames*rows*columns*sizeof(imgpel);
}

/*!
 ************************************************************************
 * \brief
 *    free 2D memory array
 *    which was alocated with get_mem2Dpel()
 ************************************************************************
 */
void free_mem2Dpel(imgpel **array2D)
{
  if (array2D)
  {
    if (array2D[0])
      free (array2D[0]);
    else error ("free_mem2Dpel: trying to free unused memory",100);

    free (array2D);
  } else
  {
    error ("free_mem2Dpel: trying to free unused memory",100);
  }
}

/*!
 ************************************************************************
 * \brief
 *    free 3D memory array
 *    which was alocated with get_mem3Dpel()
 ************************************************************************
 */
void free_mem3Dpel(imgpel ***array3D, int frames)
{
  int i;

  if (array3D)
  {
    for (i=0;i<frames;i++)
    {
      free_mem2Dpel(array3D[i]);
    }
   free (array3D);
  } else
  {
    error ("free_mem3Dpel: trying to free unused memory",100);
  }
}

/*!
 ************************************************************************
 * \brief
 *    Allocate 2D memory array -> unsigned char array2D[rows][columns]
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************/
// Change 9-Aug-2001 P. List: dont allocate independant row arrays anymore
// but one complete array and move row-pointers to array. Now you can step
// to the next line with an offset of img->width
int get_mem2D(byte ***array2D, int rows, int columns)
{
  int i;

  if((*array2D      = (byte**)calloc(rows,        sizeof(byte*))) == NULL)
    no_mem_exit("get_mem2D: array2D");
  if(((*array2D)[0] = (byte* )calloc(columns*rows,sizeof(byte ))) == NULL)
    no_mem_exit("get_mem2D: array2D");

  for(i=1;i<rows;i++)
    (*array2D)[i] = (*array2D)[i-1] + columns ;

  return rows*columns;
}

/*!
 ************************************************************************
 * \brief
 *    Allocate 2D memory array -> int array2D[rows][columns]
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************
 */
// same change as in get_mem2Dint
int get_mem2Dint(int ***array2D, int rows, int columns)
{
  int i;

  if((*array2D      = (int**)calloc(rows,        sizeof(int*))) == NULL)
    no_mem_exit("get_mem2Dint: array2D");
  if(((*array2D)[0] = (int* )calloc(rows*columns,sizeof(int ))) == NULL)
    no_mem_exit("get_mem2Dint: array2D");

  for(i=1 ; i<rows ; i++)
    (*array2D)[i] =  (*array2D)[i-1] + columns  ;

  return rows*columns*sizeof(int);
}

/*!
 ************************************************************************
 * \brief
 *    Allocate 2D memory array -> int64 array2D[rows][columns]
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************
 */
// same change as in get_mem2Dint
int get_mem2Dint64(int64 ***array2D, int rows, int columns)
{
  int i;

  if((*array2D      = (int64**)calloc(rows,        sizeof(int64*))) == NULL)
    no_mem_exit("get_mem2Dint64: array2D");
  if(((*array2D)[0] = (int64* )calloc(rows*columns,sizeof(int64 ))) == NULL)
    no_mem_exit("get_mem2Dint64: array2D");

  for(i=1 ; i<rows ; i++)
    (*array2D)[i] =  (*array2D)[i-1] + columns  ;

  return rows*columns*sizeof(int64);
}

/*!
 ************************************************************************
 * \brief
 *    Allocate 3D memory array -> unsigned char array3D[frames][rows][columns]
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************
 */
// same change as in get_mem2Dint
int get_mem3D(byte ****array3D, int frames, int rows, int columns)
{
  int  j;

  if(((*array3D) = (byte***)calloc(frames,sizeof(byte**))) == NULL)
    no_mem_exit("get_mem3D: array3D");

  for(j=0;j<frames;j++)
    get_mem2D( (*array3D)+j, rows, columns ) ;

  return frames*rows*columns;
}

/*!
 ************************************************************************
 * \brief
 *    Allocate 3D memory array -> int array3D[frames][rows][columns]
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************
 */
// same change as in get_mem2Dint
int get_mem3Dint(int ****array3D, int frames, int rows, int columns)
{
  int  j;

  if(((*array3D) = (int***)calloc(frames,sizeof(int**))) == NULL)
    no_mem_exit("get_mem3Dint: array3D");

  for(j=0;j<frames;j++)
    get_mem2Dint( (*array3D)+j, rows, columns ) ;

  return frames*rows*columns*sizeof(int);
}

/*!
 ************************************************************************
 * \brief
 *    Allocate 3D memory array -> int64 array3D[frames][rows][columns]
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************
 */
// same change as in get_mem2Dint
int get_mem3Dint64(int64 ****array3D, int frames, int rows, int columns)
{
  int  j;

  if(((*array3D) = (int64***)calloc(frames,sizeof(int64**))) == NULL)
    no_mem_exit("get_mem3Dint64: array3D");

  for(j=0;j<frames;j++)
    get_mem2Dint64( (*array3D)+j, rows, columns ) ;

  return frames*rows*columns*sizeof(int64);
}

/*!
 ************************************************************************
 * \brief
 *    Allocate 4D memory array -> int array3D[frames][rows][columns][component]
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************
 */
// same change as in get_mem2Dint
int get_mem4Dint(int *****array4D, int idx, int frames, int rows, int columns )
{
  int  j;

  if(((*array4D) = (int****)calloc(idx,sizeof(int**))) == NULL)
    no_mem_exit("get_mem4Dint: array4D");

  for(j=0;j<idx;j++)
    get_mem3Dint( (*array4D)+j, frames, rows, columns ) ;

  return idx*frames*rows*columns*sizeof(int);
}

/*!
 ************************************************************************
 * \brief
 *    free 2D memory array
 *    which was alocated with get_mem2D()
 ************************************************************************
 */
void free_mem2D(byte **array2D)
{
  if (array2D)
  {
    if (array2D[0])
      free (array2D[0]);
    else error ("free_mem2D: trying to free unused memory",100);

    free (array2D);
  } else
  {
    error ("free_mem2D: trying to free unused memory",100);
  }
}

/*!
 ************************************************************************
 * \brief
 *    free 2D memory array
 *    which was alocated with get_mem2Dint()
 ************************************************************************
 */
void free_mem2Dint(int **array2D)
{
  if (array2D)
  {
    if (array2D[0])
      free (array2D[0]);
    else error ("free_mem2Dint: trying to free unused memory",100);

    free (array2D);

  } else
  {
    error ("free_mem2Dint: trying to free unused memory",100);
  }
}

/*!
 ************************************************************************
 * \brief
 *    free 2D memory array
 *    which was alocated with get_mem2Dint64()
 ************************************************************************
 */
void free_mem2Dint64(int64 **array2D)
{
  if (array2D)
  {
    if (array2D[0])
      free (array2D[0]);
    else error ("free_mem2Dint64: trying to free unused memory",100);

    free (array2D);

  } else
  {
    error ("free_mem2Dint64: trying to free unused memory",100);
  }
}


/*!
 ************************************************************************
 * \brief
 *    free 3D memory array
 *    which was alocated with get_mem3D()
 ************************************************************************
 */
void free_mem3D(byte ***array3D, int frames)
{
  int i;

  if (array3D)
  {
    for (i=0;i<frames;i++)
    {
      free_mem2D(array3D[i]);
    }
   free (array3D);
  } else
  {
    error ("free_mem3D: trying to free unused memory",100);
  }
}

/*!
 ************************************************************************
 * \brief
 *    free 3D memory array
 *    which was alocated with get_mem3Dint()
 ************************************************************************
 */
void free_mem3Dint(int ***array3D, int frames)
{
  int i;

  if (array3D)
  {
    for (i=0;i<frames;i++)
    {
      free_mem2Dint(array3D[i]);
    }
   free (array3D);
  } else
  {
    error ("free_mem3D: trying to free unused memory",100);
  }
}


/*!
 ************************************************************************
 * \brief
 *    free 3D memory array
 *    which was alocated with get_mem3Dint64()
 ************************************************************************
 */
void free_mem3Dint64(int64 ***array3D, int frames)
{
  int i;

  if (array3D)
  {
    for (i=0;i<frames;i++)
    {
      free_mem2Dint64(array3D[i]);
    }
   free (array3D);
  } else
  {
    error ("free_mem3Dint64: trying to free unused memory",100);
  }
}

/*!
 ************************************************************************
 * \brief
 *    free 4D memory array
 *    which was alocated with get_mem4Dint()
 ************************************************************************
 */
void free_mem4Dint(int ****array4D, int idx, int frames )
{
  int  j;

  if (array4D)
  {
    for(j=0;j<idx;j++)
      free_mem3Dint( array4D[j], frames) ;
    free (array4D);
  } else
  {
    error ("free_mem4D: trying to free unused memory",100);
  }
}


/*!
 ************************************************************************
 * \brief
 *    Exit program if memory allocation failed (using error())
 * \param where
 *    string indicating which memory allocation failed
 ************************************************************************
 */
void no_mem_exit(char *where)
{
   snprintf(errortext, ET_SIZE, "Could not allocate memory: %s",where);
   error (errortext, 100);
}

/*!
 ************************************************************************
 * \brief
 *    Allocate 2D short memory array -> short array2D[rows][columns]
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************
 */
int get_mem2Dshort(short ***array2D, int rows, int columns)
{
  int i;

  if((*array2D      = (short**)calloc(rows,        sizeof(short*))) == NULL)
    no_mem_exit("get_mem2Dshort: array2D");
  if(((*array2D)[0] = (short* )calloc(rows*columns,sizeof(short ))) == NULL)
    no_mem_exit("get_mem2Dshort: array2D");

  for(i=1 ; i<rows ; i++)
    (*array2D)[i] =  (*array2D)[i-1] + columns  ;

  return rows*columns*sizeof(short);
}

/*!
 ************************************************************************
 * \brief
 *    Allocate 3D memory short array -> short array3D[frames][rows][columns]
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************
 */
int get_mem3Dshort(short ****array3D, int frames, int rows, int columns)
{
  int  j;

  if(((*array3D) = (short***)calloc(frames,sizeof(short**))) == NULL)
    no_mem_exit("get_mem3Dshort: array3D");

  for(j=0;j<frames;j++)
    get_mem2Dshort( (*array3D)+j, rows, columns ) ;

  return frames*rows*columns*sizeof(short);
}

/*!
 ************************************************************************
 * \brief
 *    Allocate 4D memory short array -> short array3D[frames][rows][columns][component]
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************
 */
int get_mem4Dshort(short *****array4D, int idx, int frames, int rows, int columns )
{
  int  j;

  if(((*array4D) = (short****)calloc(idx,sizeof(short**))) == NULL)
    no_mem_exit("get_mem4Dshort: array4D");

  for(j=0;j<idx;j++)
    get_mem3Dshort( (*array4D)+j, frames, rows, columns ) ;

  return idx*frames*rows*columns*sizeof(short);
}

/*!
 ************************************************************************
 * \brief
 *    free 2D short memory array
 *    which was allocated with get_mem2Dshort()
 ************************************************************************
 */
void free_mem2Dshort(short **array2D)
{
  if (array2D)
  {
    if (array2D[0])
      free (array2D[0]);
    else error ("free_mem2Dshort: trying to free unused memory",100);

    free (array2D);

  } else
  {
    error ("free_mem2Dshort: trying to free unused memory",100);
  }
}

/*!
 ************************************************************************
 * \brief
 *    free 3D short memory array
 *    which was allocated with get_mem3Dshort()
 ************************************************************************
 */
void free_mem3Dshort(short ***array3D, int frames)
{
  int i;

  if (array3D)
  {
    for (i=0;i<frames;i++)
    {
      free_mem2Dshort(array3D[i]);
    }
   free (array3D);
  } else
  {
    error ("free_mem3Dshort: trying to free unused memory",100);
  }
}

/*!
 ************************************************************************
 * \brief
 *    free 4D short memory array
 *    which was allocated with get_mem4Dshort()
 ************************************************************************
 */
void free_mem4Dshort(short ****array4D, int idx, int frames )
{
  int  j;

  if (array4D)
  {
    for(j=0;j<idx;j++)
      free_mem3Dshort( array4D[j], frames) ;
    free (array4D);
  } else
  {
    error ("free_mem4Dshort: trying to free unused memory",100);
  }
}
