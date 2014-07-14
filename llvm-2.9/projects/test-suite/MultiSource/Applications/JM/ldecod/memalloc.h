
/*!
 ************************************************************************
 * \file  memalloc.h
 *
 * \brief
 *    Memory allocation and free helper funtions
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 ************************************************************************
 */

#ifndef _MEMALLOC_H_
#define _MEMALLOC_H_

#include "global.h"

int  get_mem2D(byte ***array2D, int rows, int columns);
int  get_mem3D(byte ****array2D, int frames, int rows, int columns);

int  get_mem2Dint(int ***array2D, int rows, int columns);
int  get_mem3Dint(int ****array3D, int frames, int rows, int columns);
int  get_mem4Dint(int *****array4D, int idx, int frames, int rows, int columns );

int  get_mem3Dint64(int64 ****array3D, int frames, int rows, int columns);
int  get_mem2Dint64(int64 ***array2D, int rows, int columns);

int  get_mem2Dshort(short ***array2D, int rows, int columns);
int  get_mem3Dshort(short ****array3D, int frames, int rows, int columns);
int  get_mem4Dshort(short *****array4D, int idx, int frames, int rows, int columns );

int get_mem2Dpel(imgpel ***array2D, int rows, int columns);
int get_mem3Dpel(imgpel ****array3D, int frames, int rows, int columns);

void free_mem2D(byte **array2D);
void free_mem3D(byte ***array2D, int frames);

void free_mem2Dint(int **array2D);
void free_mem3Dint(int ***array3D, int frames);
void free_mem4Dint(int ****array4D, int idx, int frames);

void free_mem2Dint64(int64 **array2D);
void free_mem3Dint64(int64 ***array3D64, int frames);

void free_mem2Dshort(short **array2D);
void free_mem3Dshort(short ***array3D, int frames);
void free_mem4Dshort(short ****array4D, int idx, int frames);

void free_mem2Dpel(imgpel **array2D);
void free_mem3Dpel(imgpel ***array3D, int frames);

void no_mem_exit(char *where);

#endif
