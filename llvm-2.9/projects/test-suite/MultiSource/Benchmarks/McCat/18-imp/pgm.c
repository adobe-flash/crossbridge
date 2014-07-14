
/****
    Copyright (C) 1996 McGill University.
    Copyright (C) 1996 McCAT System Group.
    Copyright (C) 1996 ACAPS Benchmark Administrator
                       benadmin@acaps.cs.mcgill.ca

    This program is free software; you can redistribute it and/or modify
    it provided this copyright notice is maintained.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
****/

/*
MODULE     : pgm.c
DESCRIPTION: Routines for manipulating PGM files.
AUTHOR     : Fady Habra, 8715030, fady@cs.mcgill.ca
*/

#ifndef PGM
#define PGM
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pgm.h"

static const char *mybasename(const char *str) {
  const char *base = strrchr(str, '/');
  return base ? base+1 : str;
}

/*
FUNCTION   : void PGM_InitImage(ImgP img, char *filename)
DESCRIPTION: Initializes an image information object with the given filename
ARGUMENTS  : img, pointer to image information object
RETURNS    : nothing
*/
void PGM_InitImage(ImgP img, char *filename)
{
  char *p;

  /* Allocate memory for the filename. */
  img->filename = (char *) malloc((strlen(filename) + 1) * sizeof(char));
  strcpy(img->filename, filename);

  /* Determine base name of image file. */
  img->imgname = strdup(img->filename);
  p = img->filename;
  while(*p++ != '\0')
    if (*p == '/')
      strcpy(img->imgname, p + 1);
  p = img->imgname;
  while(*p++ != '\0')
    if (*p == '.')
      *p = '\0';

  /* Clear valid bit. */
  img->valid = 0;
  img->var = 0;
}

/*
FUNCTION   : void PGM_FreeImage(ImgP img)
DESCRIPTION: Frees all memory allocated to an image information object.
ARGUMENTS  : img, pointer to image information object
RETURNS    : nothing
*/
void PGM_FreeImage(ImgP img)
{
  free(img->filename);
  free(img->imgname);
  free(img->data);
/* R and f not used */
/* free(img->R); */
/* free(img->f); */
  free(img->cedge);
  img->valid = 0;
}

/*
FUNCTION   : void PGM_PrintInfo(ImgP img)
DESCRIPTION: Prints all available info in an image information object.
ARGUMENTS  : img, pointer to image information object
RETURNS    : nothing
*/
void PGM_PrintInfo(ImgP img)
{
  if (img->valid)
    {
      printf("filename:\t%s\n", mybasename(img->filename));
      printf("width   :\t%d\n", img->width);
      printf("height  :\t%d\n", img->height);
      printf("max gray:\t%d\n", img->maxgray);
    }
}

/*
FUNCTION   : int PGM_GetValue(ImgP img)
DESCRIPTION: Reads a single value from a PGM file, skipping whitespace and
	     comments.
ARGUMENTS  : img, pointer to image information object
RETURNS    : value read
*/
int PGM_GetValue(ImgP img)
{
  int n;
  char c;

  /* Loop until we can read an integer. */

  while (fscanf(img->fp, "%d", &n) == 0)
    {
      /* Skip comments. */
      fscanf(img->fp, "#%n", &n);

      /* If we found a comment, skip all characters until EOL is reached. */
      if (n > 0)
	while((c = fgetc(img->fp)) != '\n');

      /* Skip newline. */
      fscanf(img->fp, "\n");
    }
  return(n);
}

/*
FUNCTION   : int PGM_Open(ImgP img)
DESCRIPTION: Opens a PGM file for further processing.
ARGUMENTS  : img, pointer to image information object
RETURNS    : PGM_OK, if successful
	     PGM_NOT_FOUND, if file not found
	     PGM_NOT_PGM, if file is not a PGM file
*/
int PGM_Open(ImgP img)
{
  char magic[2];

  /* Open the file for reading. */
  if ((img->fp = fopen(img->filename, "r")) == NULL)
    return(PGM_NOT_FOUND);

  /* Extract the magic number. */
  if ((fread(magic, 1, 2, img->fp) != 2) || (strncmp(magic, "P5", 2) != 0))
    return(PGM_NOT_PGM);

  /* Extract the image width. */
  img->width = PGM_GetValue(img);
  /* Extract the image height. */
  img->height = PGM_GetValue(img);
  /* Extract the maximum gray value. */
  img->maxgray = PGM_GetValue(img);

  /* Set valid bit. */
  img->valid = 1;

  return(PGM_OK);
}

/*
FUNCTION   : void PGM_Close(ImgP img)
DESCRIPTION: Closes a PGM file.
ARGUMENTS  : img, pointer to image information object
RETURNS    : nothing
*/
void PGM_Close(ImgP img)
{
  fclose(img->fp);
}

/*
FUNCTION   : int PGM_LoadImage(ImgP img)
DESCRIPTION: Loads a PGM file.
ARGUMENTS  : img, pointer to image information object
RETURNS    : PGM_OK, if successful
	     PGM_NOT_FOUND, if file not found
	     PGM_NOT_PGM, if file is not a PGM file
	     PGM_NO_DATA, if image has 0 length
*/
int PGM_LoadImage(ImgP img)
{
  int rc;
  unsigned char byte;
  int n = 0, c, pixel;

  /* Open the image file for processing. */
  if ((rc = PGM_Open(img)) != PGM_OK)
    return(rc);

  /* Do nothing if either height or width is 0. */
  if (img->width == 0 || img->height == 0)
    {
      PGM_Close(img);
      return(PGM_NO_DATA);
    }

  /* Allocate memory for image and histogram. */
  img->data = (char *) malloc((img->width * img->height) * sizeof(char));
  img->hist = (int *) calloc(img->maxgray + 1, sizeof(int));
  img->p = (double *) calloc(img->maxgray + 1, sizeof(double));
  img->var = NULL;

  /* Skip comments. */
  fscanf(img->fp, "#%n", &n);
  /* If we found a comment, skip all characters until EOL is reached. */
  if (n > 0)
    while((c = fgetc(img->fp)) != '\n');
  /* Skip newline. */
  fscanf(img->fp, "\n");

  /* Load the image. */
  for (pixel = 0; pixel < (img->width * img->height); pixel++)
    {
      fread(&byte, 1, 1, img->fp);
      img->data[pixel] = byte;
      img->hist[byte]++;
    }

  /* Normalize the probability array. */
  for (byte = 0; byte < img->maxgray; byte++)
    img->p[byte] = (double) ((double) img->hist[byte] / 
			     (double) (img->width * img->height));

  PGM_Close(img);
  return(PGM_OK);
}

/*
FUNCTION   : int PGM_WriteBinary(ImgP img);
DESCRIPTION: Writes a binary image to a PGM file.
ARGUMENTS  : img, pointer to an image information object
RETURNS	   : PGM_OK, if successful
	     PGM_NO_DATA, if no data
*/
int PGM_WriteBinary(ImgP img)
{
  char *s;
  FILE *fp;
  long pixel;
  unsigned char val;

  if ((!img->valid) || (img->width == 0) || (img->height == 0))
    return PGM_NO_DATA;

  s = (char *) malloc((strlen(img->imgname) + strlen(".jo.pgm") + 1) *
		      sizeof(char));
  sprintf(s, "%s.jo.pgm", img->imgname);

  fp = stdout;

  fprintf(fp, "P5\n");
  fprintf(fp, "%d %d\n", img->width, img->height);
  fprintf(fp, "%d\n", img->maxgray);

  for (pixel = 0; pixel < (img->width * img->height); pixel++)
    {
      val = (img->data[pixel] > (unsigned char) img->threshold) ?
	(unsigned char) img->maxgray : 0;
      fwrite(&val, sizeof(char), 1, fp);
      /*
      if (val == (unsigned char) img->maxgray)
	printf("%d\n", (int) img->data[pixel]);
      */
    }

  free(s);

  return(PGM_OK);
}

/*
FUNCTION   : int PGM_WriteImage(ImgP img, char itype)
DESCRIPTION: Writes given image data to PGM file.
ARGUMENTS  : img  , pointer to image information object
	     itype, image type
RETURNS    : PGM_OK,      if successful
	     PGM_NO_DATA, if image information object is empty
*/
int PGM_WriteImage(ImgP img, char itype)
{
  char *s;
  FILE *fp;
  long pixel;
  unsigned char val;

  if ((!img->valid) || (img->width == 0) || (img->height == 0))
    return(PGM_NO_DATA);

  switch(itype)
    {
    case PGM_IDATA:
      if (img->data == NULL)
	return(PGM_NO_DATA);

      s = (char *) malloc((strlen(img->imgname) + strlen(".raw.pgm") + 1) *
			  sizeof(char));

      sprintf(s, "%s.raw.pgm", img->imgname);

      fp = stdout;

      fprintf(fp, "P5\n");
      fprintf(fp, "%d %d\n", img->width, img->height);
      fprintf(fp, "%d\n", img->maxgray);

      for (pixel = 0; pixel < (img->width * img->height); pixel++)
	fwrite(&(img->data[pixel]), sizeof(char), 1, fp);

      free(s);
      break;

    case PGM_IBLUR:
      /* Not implemented yet. */
      return(PGM_NO_DATA);
      break;
/*
      if (img->R == NULL)
	return(PGM_NO_DATA);
      
      s = (char *) malloc((strlen(img->imgname) + strlen(".blur.pgm") + 
			   (int) log10(sigma) + 2) * sizeof(char));
      sprintf(s, "%s.blur%d.pgm", img->imgname, sigma);

      offset = (img->Rmin < 0) ? - img->Rmin : 0;
      scale = 255 / (img->Rmax - img->Rmin);

      fp = stdout;

      fprintf(fp, "P5\n");
      fprintf(fp, "%d %d\n", img->width, img->height);
      fprintf(fp, "%d\n", 255);

      for (i = 0; i < (img->width * img->height); i++)
	{
	  val = (unsigned char) ((img->R[i] + offset) * scale);
	  fwrite(&val, sizeof(char), 1, fp);
	}

      free(s);
*/
    case PGM_CEDGE:
      if (img->cedge == NULL)
	return(PGM_NO_DATA);

      s = (char *) malloc((strlen(img->imgname) + strlen(".cedge.pgm") + 1) *
			  sizeof(char));

      sprintf(s, "%s.cedge.pgm", img->imgname);

      fp = stdout;

      fprintf(fp, "P5\n");
      fprintf(fp, "%d %d\n", img->width, img->height);
      fprintf(fp, "255\n");

      for (pixel = 0; pixel < (img->width * img->height); pixel++)
	{
	  val = (unsigned char) img->cedge[pixel];
	  fwrite(&val, sizeof(char), 1, fp);
	}

      free(s);
      break;

    case PGM_IVAR:
      if (img->var == NULL)
	return(PGM_NO_DATA);

      s = (char *) malloc((strlen(img->imgname) + strlen(".hvar.pgm") + 1) *
			  sizeof(char));

      sprintf(s, "%s.hvar.pgm", img->imgname);

      fp = stdout;

      fprintf(fp, "P5\n");
      fprintf(fp, "%d %d\n", img->width, img->height);
      fprintf(fp, "255\n");

      for (pixel = 0; pixel < (img->width * img->height); pixel++)
	{
	  val = (unsigned char) img->var[pixel];
	  fwrite(&val, sizeof(char), 1, fp);
	}

      free(s);
      break;
    }

  return(PGM_OK);
}  
