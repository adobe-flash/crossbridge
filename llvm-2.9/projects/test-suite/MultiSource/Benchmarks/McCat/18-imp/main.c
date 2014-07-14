
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
MODULE     : main.c
DESCRIPTION: Main program module.
AUTHOR     : Fady Habra, 8715030, fady@cs.mcgill.ca
*/

#ifndef MAIN
#define MAIN
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pgm.h"
/*
#include "commands.h"
*/
#include "textloc.h"
#ifdef DEBUG
#include "debug.h"
#endif

#define INT 0
#define STR 1
#define DBL 2

typedef struct
{
  char parm[25];
  char type;
  void *var;
} ParmT;

char IMAGE[800];
double SIGMA;
int VAR_THRESHOLD = -1;
double VSPREAD_THRESHOLD = 0.02;
int SAME_ROW_THRESHOLD = 3;
int SAME_ROW_V = 3;
int SAME_ROW_H = 30;
int MAX_CHAR_SIZE = 50;
int MIN_CHAR_SIZE = 5;
int KILL_SMALL_COMP = 1;
int SMALL_THRESHOLD = 10;
int HVAR_WINDOW = 10;

int main(int, char *[]);
void DisplayUsage(char *);
void ParseInputFile(char *);

extern int L_canny(float, float *, int, int, float **, char *);

/*
FlgT done = FALSE;

#define NUM_COMMANDS 2

CmiT commands[NUM_COMMANDS] =
{
  {"LOAD", P_1(P_STR, "<filename>"), (HdlT) LoadFile},
  {"LIST", P_0, (HdlT) ListImages}
};
*/

/*
FUNCTION   : void DisplayUsage(char *prog)
DESCRIPTION: Displays program usage.
ARGUMENTS  : prog, program name
RETURNS    : nothing
*/
void DisplayUsage(char *prog)
{
  printf("Usage: %s <PGM image file> <sigma> <variance threshold>\n", prog);
}

/*
FUNCTION   : void ParseInputFile(char *file)
DESCRIPTION: Parses input file and sets appropriate variables.
ARGUMENTS  : file, input filename
RETURNS    : nothing
*/
void ParseInputFile(char *file)
{
  FILE *fp;
  char s[80];
  int p;
  ParmT parms[] = {
    {"image", STR, &IMAGE},
    {"sigma", DBL, &SIGMA},
    {"var_thresh", INT, &VAR_THRESHOLD},
    {"vspread_thresh", DBL, &VSPREAD_THRESHOLD}, 
    {"same_row_thresh", INT, &SAME_ROW_THRESHOLD},
    {"same_row_v", INT, &SAME_ROW_V},
    {"same_row_h", INT, &SAME_ROW_H},
    {"max_char_size", INT, &MAX_CHAR_SIZE},
    {"min_char_size", INT, &MIN_CHAR_SIZE},
    {"kill_small", INT, &KILL_SMALL_COMP},
    {"small_thresh", INT, &SMALL_THRESHOLD},
    {"hvar_window", INT, &HVAR_WINDOW},
    {"", 0, NULL}
  };
  char read_parm[25];
  char read_val[80];

  /* Hack: put image file in same directory as input file */
  strcpy(IMAGE, file);
  parms[0].var = strrchr(IMAGE, '/') + 1;

  if ((fp = fopen(file, "r")) == NULL)
    {
      printf("%s not found. Exiting.\n", file);
      exit(1);
    }

  while ((fgets(s, 80, fp) != NULL) && (s[0] != '\n'))
    {
      p = 0;
      strcpy(read_parm, strtok(s, " \t"));
      strcpy(read_val, strtok(NULL, " \t"));

      while((parms[p].var != NULL) && strcmp(read_parm, parms[p].parm))
	p++;

      if (parms[p].var == NULL)
	continue;

      switch(parms[p].type)
	{
	case INT:
	  *(int *)(parms[p].var) = atoi(read_val);
	  break;

	case DBL:
	  *(double *)(parms[p].var) = strtod(read_val, NULL);
	  break;

	case STR:
	  strcpy((char *) parms[p].var, read_val);
	  ((char *) (parms[p].var))[strlen((char *) (parms[p].var)) - 1] =
	    '\0';
	  break;
	}
    }
  fclose(fp);
}

/*
FUNCTION   : int main(int argc, char *argv[])
DESCRIPTION: Main program body.
ARGUMENTS  : argc, argv
RETURNS    : program exit code
*/
int main(int argc, char *argv[])
{
  int rc;
  char err[80];
  ImgT image;
  double sigma;
  int var_threshold;
  CompP comp;

#ifdef BOGUS
  /* 4 arguments needed: prog name, input file, sigma, variance threshold. */
  if (argc != 4)
    {
      DisplayUsage(argv[0]);
      return(-1);
    }

  sigma = strtod(argv[2], NULL);
  var_threshold = atoi(argv[3]);
#else
  ParseInputFile(argv[1]);
#endif

  /* Initialize image information object. */
#ifdef BOGUS
  PGM_InitImage(&image, argv[1]);
#else
  PGM_InitImage(&image, IMAGE);
#endif

  sigma = SIGMA;
  var_threshold = VAR_THRESHOLD;

  /* Load image data. */
  if ((rc = PGM_LoadImage(&image)) != PGM_OK)
    {
      switch(rc)
	{
	case PGM_NOT_FOUND:
	  printf("Error: %s not found. Exiting.\n", image.filename);
	  break;

	case PGM_NOT_PGM:
	  printf("Error: %s is not a PGM file. Exiting.\n", image.filename);
	  break;

	case PGM_NO_DATA:
	  printf("Error: %s has 0 length. Exiting.\n", image.filename);
	  break;
	}
      free(image.filename);
      free(image.imgname);
      return(-2);
    }

  /* Print image info. */
  PGM_PrintInfo(&image);

  /* Compute horizontal intensity variance bitmap. */
  HorzVariance(&image, var_threshold);

  /* Convert image data to floating point. (L_canny requires this.) */
  /* ConvertToFloat(&image); */

#ifdef INTERMEDIATE_OUTPUT
  /* Write horizontal variance bitmap to PGM file. */
  PGM_WriteImage(&image, PGM_IVAR);
#endif

  /* Perform Canny edge detection on the variance bitmap. */
  if (L_canny(sigma, image.var, image.width, image.height, &(image.cedge),
	      err))
    {
      printf("Error: '%s' in L_canny(). Exiting.\n", err);
      return(-3);
    }

#ifdef INTERMEDIATE_OUTPUT
  /* Write horizontal variance Canny edge data to PGM file. */
  PGM_WriteImage(&image, PGM_CEDGE);
#endif

  /* Build connected components in edge image. */
  comp = BuildConnectedComponents(&image, 0);

#if TRACE1
  /* Print connected components. */
  PrintConnectedComponents(comp);
#endif

#ifdef INTERMEDIATE_OUTPUT
  WriteConnectedComponentsToPGM(comp, &image);
#endif

  /* Eliminate connected components with large vertical spread. */
  EliminateLargeSpreadComponents(&comp, &image);

#if TRACE2
  PrintConnectedComponents(comp);
#endif

  /* Merge same row components. */
  MergeRowComponents(&comp, &image);

#ifdef INTERMEDIATE_OUTPUT
  /* Write connected components to PGM file. */
  WriteConnectedComponentsToPGM(comp, &image);
#endif

#if TRACE3
  PrintConnectedComponents(comp);
#endif

  /* Find opposing connected component pairs. */
  PairComponents(&comp, &image);

#if TRACE4
  PrintConnectedComponents(comp);
#endif

  ComputeBoundingBoxes(comp, &image);

  /* Free connected components list. */
  FreeConnectedComponents(comp);

  /* Free image information object. */
  PGM_FreeImage(&image);

  return(0);
}
