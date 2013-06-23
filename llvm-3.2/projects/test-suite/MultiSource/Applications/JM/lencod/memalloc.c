
/*!
 ************************************************************************
 * \file  memalloc.c
 *
 * \brief
 *    Memory allocation and free helper functions
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 ************************************************************************
 */

#include <stdlib.h>

#include "global.h"

 /*!
 ************************************************************************
 * \brief
 *    Initialize 2-dimensional top and bottom field to point to the proper
 *    lines in frame
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************/
int init_top_bot_planes(imgpel **imgFrame, int rows, int columns, imgpel ***imgTopField, imgpel ***imgBotField)
{
  int i;

  if((*imgTopField   = (imgpel**)calloc(rows/2,        sizeof(imgpel*))) == NULL)
    no_mem_exit("init_top_bot_planes: imgTopField");

  if((*imgBotField   = (imgpel**)calloc(rows/2,        sizeof(imgpel*))) == NULL)
    no_mem_exit("init_top_bot_planes: imgBotField");

  for(i=0 ; i<rows/2 ; i++)
  {
    (*imgTopField)[i] =  imgFrame[2*i  ];
    (*imgBotField)[i] =  imgFrame[2*i+1];
  }

  return rows*sizeof(imgpel*);
}

 /*!
 ************************************************************************
 * \brief
 *    free 2-dimensional top and bottom fields without freeing target memory
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************/
void free_top_bot_planes(imgpel **imgTopField, imgpel **imgBotField)
{
  free (imgTopField);
  free (imgBotField);
}


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
 *    Allocate 4D memory array -> imgpel array4D[sub_x][sub_y][rows][columns]
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************
 */
int get_mem4Dpel(imgpel *****array4D, int sub_x, int sub_y, int rows, int columns)
{
  int  j;

  if(((*array4D) = (imgpel****)calloc(sub_x,sizeof(imgpel***))) == NULL)
    no_mem_exit("get_mem4Dpel: array4D");

  for(j=0;j<sub_x;j++)
    get_mem3Dpel( (*array4D)+j, sub_y, rows, columns ) ;

  return sub_x*sub_y*rows*columns*sizeof(imgpel);
}

/*!
 ************************************************************************
 * \brief
 *    Allocate 5D memory array -> imgpel array5D[dims][sub_x][sub_y][rows][columns]
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************
 */
int get_mem5Dpel(imgpel ******array5D, int dims, int sub_x, int sub_y, int rows, int columns)
{
  int  j;

  if(((*array5D) = (imgpel*****)calloc(dims,sizeof(imgpel****))) == NULL)
    no_mem_exit("get_mem5Dpel: array5D");

  for(j=0;j<dims;j++)
    get_mem4Dpel( (*array5D)+j, sub_x, sub_y, rows, columns ) ;

  return dims*sub_x*sub_y*rows*columns*sizeof(imgpel);
}

/*!
 ************************************************************************
 * \brief
 *    free 2D memory array
 *    which was allocated with get_mem2Dpel()
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
 *    which was allocated with get_mem3Dpel()
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
 *    free 4D memory array
 *    which was allocated with get_mem4Dpel()
 ************************************************************************
 */
void free_mem4Dpel(imgpel ****array4D, int sub_x, int sub_y)
{
  int i;

  if (array4D)
  {
    for (i=0;i<sub_x;i++)
      free_mem3Dpel(array4D[i], sub_y);
    free (array4D);
  }
  else
  {
    error ("free_mem4Dpel: trying to free unused memory",100);
  }
}

/*!
 ************************************************************************
 * \brief
 *    free 5D memory array
 *    which was allocated with get_mem5Dpel()
 ************************************************************************
 */
void free_mem5Dpel(imgpel *****array5D, int dims, int sub_x, int sub_y)
{
  int i;

  if (array5D)
  {
    for (i=0;i<dims;i++)
      free_mem4Dpel(array5D[i], sub_x, sub_y);
    free (array5D);
  }
  else
  {
    error ("free_mem5Dpel: trying to free unused memory",100);
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
 *    Allocate 4D memory array -> int array4D[frames][rows][columns][component]
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************
 */
int get_mem4Dint(int *****array4D, int idx, int frames, int rows, int columns )
{
  int  j;

  if(((*array4D) = (int****)calloc(idx,sizeof(int***))) == NULL)
    no_mem_exit("get_mem4Dint: array4D");

  for(j=0;j<idx;j++)
    get_mem3Dint( (*array4D)+j, frames, rows, columns ) ;

  return idx*frames*rows*columns*sizeof(int);
}

/*!
 ************************************************************************
 * \brief
 *    Allocate 5D memory array -> int array5D[refs][blocktype][rows][columns][component]
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************
 */
int get_mem5Dint(int ******array5D, int refs, int blocktype, int rows, int columns, int component)
{
  int  j;

  if(((*array5D) = (int*****)calloc(refs,sizeof(int****))) == NULL)
    no_mem_exit("get_mem5Dint: array5D");

  ;
  for(j=0;j<refs;j++)
    get_mem4Dint( (*array5D)+j, blocktype, rows, columns, component) ;

  return refs*blocktype*rows*columns*component*sizeof(int);
}


/*!
 ************************************************************************
 * \brief
 *    free 2D memory array
 *    which was allocated with get_mem2D()
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
 *    which was allocated with get_mem2Dint()
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
 *    which was allocated with get_mem2Dint64()
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
 *    which was allocated with get_mem3D()
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
 *    which was allocated with get_mem3Dint()
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
    error ("free_mem3Dint: trying to free unused memory",100);
  }
}


/*!
 ************************************************************************
 * \brief
 *    free 3D memory array
 *    which was allocated with get_mem3Dint64()
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
 *    which was allocated with get_mem4Dint()
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
    error ("free_mem4Dint: trying to free unused memory",100);
  }
}

/*!
 ************************************************************************
 * \brief
 *    free 5D int memory array
 *    which was allocated with get_mem5Dint()
 ************************************************************************
 */
void free_mem5Dint(int *****array5D, int refs, int blocktype, int height)
{
  int  j;

  if (array5D)
  {
    for(j=0;j<refs;j++)
      free_mem4Dint( array5D[j], blocktype, height) ;
    free (array5D);
  } else
  {
    error ("free_mem5Dint: trying to free unused memory",100);
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
 *    Allocate 5D memory array -> short array5D[refs][blocktype][rows][columns][component]
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************
 */
int get_mem5Dshort(short ******array5D, int refs, int blocktype, int rows, int columns, int component)
{
  int  j;

  if(((*array5D) = (short*****)calloc(refs,sizeof(short****))) == NULL)
    no_mem_exit("get_mem5Dshort: array5D");

  ;
  for(j=0;j<refs;j++)
    get_mem4Dshort( (*array5D)+j, blocktype, rows, columns, component) ;

  return refs*blocktype*rows*columns*component*sizeof(short);
}

/*!
 ************************************************************************
 * \brief
 *    Allocate 6D memory array -> short array6D[list][refs][blocktype][rows][columns][component]
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************
 */
int get_mem6Dshort(short *******array6D, int list, int refs, int blocktype, int rows, int columns, int component)
{
  int  j;

  if(((*array6D) = (short******)calloc(list,sizeof(short*****))) == NULL)
    no_mem_exit("get_mem6Dshort: array6D");

  ;
  for(j=0;j<list;j++)
    get_mem5Dshort( (*array6D)+j, refs, blocktype, rows, columns, component) ;

  return list * refs * blocktype * rows * columns * component * sizeof(short);
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

/*!
 ************************************************************************
 * \brief
 *    free 5D short memory array
 *    which was allocated with get_mem5Dshort()
 ************************************************************************
 */
void free_mem5Dshort(short *****array5D, int refs, int blocktype, int height)
{
  int  j;

  if (array5D)
  {
    for(j=0;j<refs;j++)
      free_mem4Dshort( array5D[j], blocktype, height) ;
    free (array5D);
  }
  else
  {
    error ("free_mem5Dshort: trying to free unused memory",100);
  }
}

/*!
 ************************************************************************
 * \brief
 *    free 6D short memory array
 *    which was allocated with get_mem6Dshort()
 ************************************************************************
 */
void free_mem6Dshort(short ******array6D, int list, int refs, int blocktype, int height)
{
  int  j;

  if (array6D)
  {
    for(j=0;j<list;j++)
      free_mem5Dshort( array6D[j], refs, blocktype, height) ;
    free (array6D);
  }
  else
  {
    error ("free_mem6Dshort: trying to free unused memory",100);
  }
}


/*!
 ************************************************************************
 * \brief
 *    Allocate 2D memory array -> double array2D[rows][columns]
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************
 */
int get_mem2Ddouble(double ***array2D, int rows, int columns)
{
  int i;

  if((*array2D      = (double**)calloc(rows,        sizeof(double*))) == NULL)
    no_mem_exit("get_mem2Ddouble: array2D");
  if(((*array2D)[0] = (double* )calloc(rows*columns,sizeof(double ))) == NULL)
    no_mem_exit("get_mem2Ddouble: array2D");

  for(i=1 ; i<rows ; i++)
    (*array2D)[i] =  (*array2D)[i-1] + columns  ;

  return rows*columns*sizeof(double);
}

/*!
 ************************************************************************
 * \brief
 *    Allocate 2D memory array -> double array2D[rows][columns]
 *    Note that array is shifted towards offset allowing negative values
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************
 */
int get_mem2Ddb_offset(double ***array2D, int rows, int columns, int offset)
{
  int i;

  if((*array2D      = (double**)calloc(rows,        sizeof(double*))) == NULL)
    no_mem_exit("get_mem2Ddb_offset: array2D");
  if(((*array2D)[0] = (double* )calloc(rows*columns,sizeof(double ))) == NULL)
    no_mem_exit("get_mem2Ddb_offset: array2D");

  (*array2D)[0] += offset;

  for(i=1 ; i<rows ; i++)
    (*array2D)[i] =  (*array2D)[i-1] + columns  ;

  return rows*columns*sizeof(double);
}

/*!
 ************************************************************************
 * \brief
 *    Allocate 3D memory double array -> double array3D[pels][rows][columns]
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************
 */
int get_mem3Ddb_offset(double ****array3D, int rows, int columns, int pels, int offset)
{
  int  i,j;

  if(((*array3D) = (double***)calloc(rows,sizeof(double**))) == NULL)
    no_mem_exit("get_mem3Ddb_offset: array3D");

  if(((*array3D)[0] = (double** )calloc(rows*columns,sizeof(double*))) == NULL)
    no_mem_exit("get_mem3Ddb_offset: array3D");

  (*array3D) [0] += offset;

  for(i=1 ; i<rows ; i++)
    (*array3D)[i] =  (*array3D)[i-1] + columns  ;

  for (i = 0; i < rows; i++)
    for (j = -offset; j < columns - offset; j++)
      if(((*array3D)[i][j] = (double* )calloc(pels,sizeof(double))) == NULL)
        no_mem_exit("get_mem3Ddb_offset: array3D");

  return rows*columns*pels*sizeof(double);
}
/*!
 ************************************************************************
 * \brief
 *    Allocate 3D memory int array -> int array3D[rows][columns][pels]
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************
 */


int get_mem3Dint_offset(int ****array3D, int rows, int columns, int pels, int offset)
{
  int  i,j;

  if(((*array3D) = (int***)calloc(rows,sizeof(int**))) == NULL)
    no_mem_exit("get_mem3Dint_offset: array3D");

  if(((*array3D)[0] = (int** )calloc(rows*columns,sizeof(int*))) == NULL)
    no_mem_exit("get_mem3Dint_offset: array3D");

  (*array3D) [0] += offset;

  for(i=1 ; i<rows ; i++)
    (*array3D)[i] =  (*array3D)[i-1] + columns  ;

  for (i = 0; i < rows; i++)
    for (j = -offset; j < columns - offset; j++)
      if(((*array3D)[i][j] = (int* )calloc(pels,sizeof(int))) == NULL)
        no_mem_exit("get_mem3Dint_offset: array3D");

  return rows*columns*pels*sizeof(int);
}


/*!
 ************************************************************************
 * \brief
 *    free 2D double memory array
 *    which was allocated with get_mem2Ddouble()
 ************************************************************************
 */
void free_mem2Ddouble(double **array2D)
{
  if (array2D)
  {
    if (array2D[0])
      free (array2D[0]);
    else 
      error ("free_mem2Ddouble: trying to free unused memory",100);

    free (array2D);

  }
  else
  {
    error ("free_mem2Ddouble: trying to free unused memory",100);
  }
}


/*!
************************************************************************
* \brief
*    free 2D double memory array (with offset)
*    which was allocated with get_mem2Ddouble()
************************************************************************
*/
void free_mem2Ddb_offset(double **array2D, int offset)
{
  if (array2D)
  {
    array2D[0] -= offset;
    if (array2D[0])
      free (array2D[0]);
    else error ("free_mem2Ddb_offset: trying to free unused memory",100);

    free (array2D);

  } else
  {
    error ("free_mem2Ddb_offset: trying to free unused memory",100);
  }
}
/*!
 ************************************************************************
 * \brief
 *    free 3D memory array with offset
 ************************************************************************
 */
void free_mem3Ddb_offset(double ***array3D, int rows, int columns, int offset)
{
  int i, j;

  if (array3D)
  {
    for (i = 0; i < rows; i++)
    {
      for (j = -offset; j < columns - offset; j++)
      {
        if (array3D[i][j])
          free(array3D[i][j]);
        else
          error ("free_mem3Ddb_offset: trying to free unused memory",100);
      }
    }
    array3D[0] -= offset;
    if (array3D[0])
      free(array3D[0]);
    else
      error ("free_mem3Ddb_offset: trying to free unused memory",100);
    free (array3D);
  }
  else
  {
    error ("free_mem3Ddb_offset: trying to free unused memory",100);
  }
}

/*!
 ************************************************************************
 * \brief
 *    free 3D memory array with offset
 ************************************************************************
 */
void free_mem3Dint_offset(int ***array3D, int rows, int columns, int offset)
{
  int i, j;

  if (array3D)
  {
    for (i = 0; i < rows; i++)
    {
      for (j = -offset; j < columns - offset; j++)
      {
        if (array3D[i][j])
          free(array3D[i][j]);
        else
          error ("free_mem3Dint_offset: trying to free unused memory",100);
      }
    }
    array3D[0] -= offset;
    if (array3D[0])
      free(array3D[0]);
    else
      error ("free_mem3Dint_offset: trying to free unused memory",100);
    free (array3D);
  }
  else
  {
    error ("free_mem3Dint_offset: trying to free unused memory",100);
  }
}
/*!
 ************************************************************************
 * \brief
 *    Allocate 2D memory array -> int array2D[rows][columns]
 *    Note that array is shifted towards offset allowing negative values
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************
 */
int get_mem2Dint_offset(int ***array2D, int rows, int columns, int offset)
{
  int i;

  if((*array2D      = (int**)calloc(rows, sizeof(int*))) == NULL)
    no_mem_exit("get_mem2Dint: array2D");
  if(((*array2D)[0] = (int* )calloc(rows*columns,sizeof(int))) == NULL)
    no_mem_exit("get_mem2Dint: array2D");

  (*array2D)[0] += offset;

  for(i=1 ; i<rows ; i++)
    (*array2D)[i] =  (*array2D)[i-1] + columns  ;

  return rows*columns*sizeof(int);
}


/*!
************************************************************************
* \brief
*    free 2D double memory array (with offset)
*    which was allocated with get_mem2Ddouble()
************************************************************************
*/
void free_mem2Dint_offset(int **array2D, int offset)
{
  if (array2D)
  {
    (*array2D)[0] -= offset;
    if (array2D[0])
      free (array2D[0]);
    else 
      error ("free_mem2Dint_offset: trying to free unused memory",100);

    free (array2D);

  } 
  else
  {
    error ("free_mem2Dint_offset: trying to free unused memory",100);
  }
}
