
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
MODULE     : textloc.c
DESCRIPTION: Text locating algorithms.
AUTHOR     : Fady Habra, 8715030, fady@cs.mcgill.ca
*/

#ifndef TEXTLOC
#define TEXTLOC
#endif

#include <stdio.h>
#include <stdlib.h>
#include "pgm.h"
#include "textloc.h"
#ifdef DEBUG
#include "debug.h"
#endif

extern double VSPREAD_THRESHOLD;
extern int SAME_ROW_THRESHOLD;
extern int SAME_ROW_V;
extern int SAME_ROW_H;
extern int MAX_CHAR_SIZE;
extern int MIN_CHAR_SIZE;
extern int KILL_SMALL_COMP;
extern int SMALL_THRESHOLD;
extern int HVAR_WINDOW;

/*
FUNCTION   : void ConvertToFloat(ImgP img)
DESCRIPTION: Converts image data to floating point.
ARGUMENTS  : img, pointer to image information object
RETURNS    : nothing
*/
void ConvertToFloat(ImgP img)
{
  long i;

  img->f = (float *) malloc((img->width * img->height) * sizeof(float));

  for (i = 0; i < (img->width * img->height); i++)
    img->f[i] = (float) img->data[i];
}

/*
FUNCTION   : void HorzVariance(ImgP img, int threshold)
DESCRIPTION: Computes the horizontal variance bitmap of an image.
ARGUMENTS  : img,       pointer to image information object
	     threshold, variance threshold
RETURNS    : nothing
*/
void HorzVariance(ImgP img, int threshold)
{
  int x, y, i, idx;
  float mean;

  img->var = (float *) calloc((img->width * img->height), sizeof(float));

  for (y = 0; y < img->height; y++)
    for (x = 10; x < img->width - 10; x++)
      {
	idx = (y * img->width) + x;

	/* Compute mean of pixels in 1x21 window centered at current pixel. */
	for (i = - HVAR_WINDOW, mean = 0; i <= HVAR_WINDOW; i++)
	  mean += (img->data[idx + i]); /* * img->p[img->data[idx + i]]); */
	mean /= (HVAR_WINDOW * 2 + 1);

	/* Compute horizontal variance. */
	for (i = - HVAR_WINDOW; i <= HVAR_WINDOW; i++)
	  img->var[idx] += (img->data[idx + i] - mean) *
	    (img->data[idx + i] - mean); /*  * img->p[img->data[idx + i]]; */
	img->var[idx] /= (HVAR_WINDOW * 2 + 1);

	if (threshold != -1)
	  if (img->var[idx] <= threshold)
	    img->var[idx] = 0;
	  else
	    img->var[idx] = 255;
      }
}

/*
FUNCTION   : CompP BuildConnectedComponents(ImgP img, int bg)
DESCRIPTION: Creates a linked list of connected components in the image.
ARGUMENTS  : img, pointer to image information object
	     bg,  background colour (if -1, all colours will be considered)
RETURNS    : pointer to linked list of connected components
*/
CompP BuildConnectedComponents(ImgP img, int bg)
{
  int x, y, xm, ym;
  int idx, midx;
  CompP comp, cur_comp, last_comp = NULL, merge_comp, mprev_comp;
  PixP pix;

  /* Allocate label map. */
  img->lmap = (int *) calloc(img->width * img->height, sizeof(int));

  for (y = 0; y < img->height; y++)
    for (x = 1; x < img->width - 1; x++)
      {
	idx = (y * img->width) + x;

	/* If current pixel has background colour, skip it. */
	if (bg >= 0 && img->cedge[idx] == bg)
	  continue;

	/* Scan top and side neighbours for a connected component. */
	for (ym = -1; ym <= 0; ym++)
	  {
	    for (xm = -1; xm <= 1; xm++)
	      {
		midx = ((y + ym) * img->width) + (x + xm);
		if (img->lmap[midx])
		  break;
	      }
	    if (img->lmap[midx])
	      break;
	  }

	/* If a connected component was not found, create one. */
	if (!img->lmap[midx])
	  {
	    /* Create, initialize and push new component on stack. */

	    comp = (CompP) malloc(sizeof(CompT));

	    comp->id = (last_comp == NULL) ? 1 : last_comp->id + 1;
	    comp->n = 0;
	    comp->avg_row = y;
	    comp->x_max = x;
	    comp->y_max = y;
	    comp->x_min = x;
	    comp->y_min = y;
	    comp->y_left = y;
	    comp->y_right = y;
	    comp->sv_above = 0;
	    comp->sv_below = 0;
	    comp->pixels = NULL;
	    comp->paired = NULL;
	    comp->extended = 0;
	    comp->prev = last_comp;

	    last_comp = comp;
	    cur_comp = comp;

#if TRACE_BUILDCOMP
	    printf("comp %d at (%d, %d)\n", comp->id, x, y);
#endif
	  }
	else	/* Look for neighboring connected component in list. */
	  {
#if TRACE_BUILDCOMP
	    printf("Found a neighbour for point (%d, %d)\t", x, y);
	    printf("img->lmap[midx] = %d\n", img->lmap[midx]);
#endif
	    cur_comp = comp;
	    while(cur_comp->id != img->lmap[midx])
	      cur_comp = cur_comp->prev;
	  }

	/* Update label map. */
	img->lmap[idx] = cur_comp->id;

	/* Add current pixel to component. */
	pix = (PixP) malloc(sizeof(PixT));
	pix->x = x;
	pix->y = y;
	pix->prev = cur_comp->pixels;
	cur_comp->pixels = pix;

	/* Update component stats. */
	cur_comp->n++;
	cur_comp->avg_row = ((cur_comp->avg_row * (cur_comp->n - 1)) + y)
	  / cur_comp->n;
	cur_comp->x_max = (x > cur_comp->x_max) ? x : cur_comp->x_max;
	cur_comp->y_max = (y > cur_comp->y_max) ? y : cur_comp->y_max;
	cur_comp->x_min = (x < cur_comp->x_min) ? x : cur_comp->x_min;
	cur_comp->y_min = (y < cur_comp->y_min) ? y : cur_comp->y_min;
	if (cur_comp->x_min == x)
	  cur_comp->y_left = y;
	if (cur_comp->x_max == x)
	  cur_comp->y_right = y;
	cur_comp->sv_above += img->var[(y - 1) * img->width + x];
	cur_comp->sv_below += img->var[(y + 1) * img->width + x];

	/* Scan all neighbours for components to merge. */
	for (ym = -1; ym <= 1; ym++)
	  for (xm = -1; xm <= 1; xm++)
	    {
	      midx = ((y + ym) * img->width) + (x + xm);

	      /* Merge adjacent components with different component id's. */
	      if (img->lmap[midx] && img->lmap[midx] != cur_comp->id)
		{
		  /* Scan component list for component to merge. */
		  merge_comp = comp;
		  mprev_comp = NULL;
		  while(merge_comp->id != img->lmap[midx])
		    {
		      mprev_comp = merge_comp;
		      merge_comp = merge_comp->prev;
		    }

		  /* Merge components. */
		  MergeComponents(cur_comp, merge_comp, mprev_comp, &comp,
				  img);
		  if (mprev_comp == NULL)
		    last_comp = comp;
		}
	    }
      }

  return(comp);
}

/*
FUNCTION   : void EliminateLargeSpreadComponents(CompI c, ImgP img)
DESCRIPTION: Eliminates components with large vertical spread. The components
	     that are not rejected are flagged as rising or falling edges.
ARGUMENTS  : c,   pointer to linked list of connected components
	     img, pointer to associated image information object
RETURNS    : nothing
*/
void EliminateLargeSpreadComponents(CompI c, ImgP img)
{
  CompI ip = c;
  CompP dp;
  PixP p;
  int idx;
  char kill_it;
#if TRACE_SPREAD
  int i = 1;
  FILE *fp;
#endif

#if TRACE_SPREAD
  fp = stdout;

  for (i = 0; i < (img->width * img->height); i++)
    {
      fprintf(fp, "%d ", img->lmap[i]);
      if (i % 15 == 0)
	fprintf(fp, "\n");
    }
  i = 1;
#endif

  while (*ip != NULL)
    {
      if (KILL_SMALL_COMP)
	kill_it = ((((*ip)->y_max - (*ip)->y_min) >=
		    (float) img->height * VSPREAD_THRESHOLD) ||
		   (((*ip)->x_max - (*ip)->x_min) < SMALL_THRESHOLD));
      else
	kill_it = (((*ip)->y_max - (*ip)->y_min) >=
		   (float) img->height * VSPREAD_THRESHOLD);
      
      if (kill_it)
	{
#if TRACE_SPREAD
	  printf("Discarding component %d (spread = %d)\n", (*ip)->id,
		 ((*ip)->y_max - (*ip)->y_min));
#endif
	  /* Update label map. */
	  p = (*ip)->pixels;
	  while (p != NULL)
	    {
#if TRACE_SPREAD
	      printf("\tpixel %d of %d at (%d, %d)\n", i++, (*ip)->n, p->x,
		     p->y);
#endif	     
	      idx = (p->y * img->width) + p->x;
	      img->lmap[idx] = 0;
	      p = p->prev;
	    }

	  /* Remove component from list, preserving links. */
	  dp = *ip;
	  *ip = (*ip)->prev;
	  free(dp);
	}
      else
	{
	  (*ip)->type = ((*ip)->sv_above >= (*ip)->sv_below) ?
	    E_FALLING : E_RISING;
	  ip = &((*ip)->prev);
	}
    }

#if TRACE_SPREAD
  fp = stdout;

  for (i = 0; i < (img->width * img->height); i++)
    {
      fprintf(fp, "%d ", img->lmap[i]);
      if (i % 15 == 0)
	fprintf(fp, "\n");
    }
#endif
}

/*
FUNCTION   : void PrintConnectedComponents(CompP c)
DESCRIPTION: Loops through list of connected components and prints out member
	     pixels.
ARGUMENTS  : c, pointer to connected component list
RETURNS    : nothing
*/
void PrintConnectedComponents(CompP c)
{
  CompP p = c;
  int npoints = 0;

  while (p != NULL)
    {
      printf("------------------------\n");
      printf("component:\t %d\n", p->id);
      printf("# points:\t %d\n", p->n);
      printf("average row:\t %d\n", p->avg_row);
      printf("x max:\t\t %d\n", p->x_max);
      printf("y max:\t\t %d\n", p->y_max);
      printf("x min:\t\t %d\n", p->x_min);
      printf("y min:\t\t %d\n", p->y_min);
      printf("type:\t\t %s\n", (p->type == E_FALLING) ? "falling" : "rising");

      npoints += p->n;
      p = p->prev;
    }

  if (c != NULL)
    {
      printf("------------------------\n\n");
      printf("Total points: %d\n", npoints);
    }
}

/*
FUNCTION   : void WriteConnectedComponentsToPGM(CompP comp, ImgP img)
DESCRIPTION: Writes linked list of connected components to PGM file.
ARGUMENTS  : comp, linked list of connected components
	     img,  pointer to associated image information object
RETURNS    : nothing
*/
void WriteConnectedComponentsToPGM(CompP comp, ImgP img)
{
  CompP c = comp;
  PixP p;
  int idx;
  char *s;
  static int index = 0;
  unsigned char *data;
  FILE *fp;

  /* Bail out if no data. */
  if ((c == NULL) || (!img->valid) || (img->width == 0) || (img->height == 0))
    return;

  if (index > 99)
    return;

  /* Allocate data buffer and fill it up with component pixels. */
  data = (unsigned char *) calloc(img->width * img->height, sizeof(char));

  while (c != NULL)
    {
      p = c->pixels;
      while (p != NULL)
	{
	  idx = (p->y * img->width) + p->x;
	  data[idx] = 255;
	  p = p->prev;
	}
      c = c->prev;
    }

  /* Open PGM file and write pixel data to it. */

  s = (char *) malloc((strlen(img->imgname) + strlen(".compXX.pgm") + 1) *
		      sizeof(char));
  sprintf(s, "%s.comp%d.pgm", img->imgname, index++);

  fp = stdout;

  fprintf(fp, "P5\n");
  fprintf(fp, "%d %d\n", img->width, img->height);
  fprintf(fp, "255\n");

  for (idx = 0; idx < (img->width * img->height); idx++)
    fwrite(&data[idx], sizeof(char), 1, fp);

  free(s);
  free(data);
}

/*
FUNCTION   : void FreeConnectedComponents(CompP comp)
DESCRIPTION: Frees linked list of connected components.
ARGUMENTS  : comp, linked list of connected components
RETURNS    : nothing
*/
void FreeConnectedComponents(CompP comp)
{
  CompP c1 = comp, c2;
  PixP p1, p2;

  while (c1 != NULL)
    {
      c2 = c1->prev;
      p1 = c1->pixels;
      while (p1 != NULL)
	{
	  p2 = p1->prev;
	  free(p1);
	  p1 = p2;
	}
      free(c1);
      c1 = c2;
    }
}

/*
FUNCTION   : void MergeComponents(CompP c, CompP m, CompP p, CompI h, ImgP img)
DESCRIPTION: Merges two components into one, freeing the second.
ARGUMENTS  : c,   destination component
	     m,   component to merge
	     p,   component previous to m in list
	     h,   pointer to component linked list head
	     img, associated image information object
RETURNS    : nothing
*/
void MergeComponents(CompP c, CompP m, CompP p, CompI h, ImgP img)
{
  PixP pix, mpix;
  int pidx;

#if TRACE_MERGE
  printf("merging component %d with %d\n", m->id, c->id);
#endif

  /* Update label map at each merged pixel. */
  mpix = pix = m->pixels;

  while(pix != NULL)
    {
      pidx = (pix->y * img->width) + pix->x;
      img->lmap[pidx] = c->id;
      mpix = pix;
      pix = pix->prev;
    }

  /* Merge pixel lists. */
  if (mpix != NULL)
    {
      mpix->prev = c->pixels;
      c->pixels = m->pixels;

      /* Update component stats. */
      c->n += m->n;
      c->avg_row = ((c->avg_row * (c->n - m->n)) + (m->avg_row * m->n))
	/ (c->n);
      c->x_max = (m->x_max > c->x_max) ? m->x_max : c->x_max;
      c->y_max = (m->y_max > c->y_max) ? m->y_max : c->y_max;
      c->x_min = (m->x_min < c->x_min) ? m->x_min : c->x_min;
      c->y_min = (m->y_min < c->y_min) ? m->y_min : c->y_min;

      if (c->x_min == m->x_min)
	c->y_left = m->y_left;
      if (c->x_max == m->x_max)
	c->y_right = m->y_right;
    }

  /* Get rid of merged component, preserving links. */
  if (p != NULL)
    p->prev = m->prev;
  else
    *h = m->prev;

  free(m);
}

/*
FUNCTION   : void MergeRowComponents(CompI head, ImgP img)
DESCRIPTION: Merges connected components with approximately same average row
	     coordinates.
ARGUMENTS  : head, lpointer to inked list of connected components
	     img,  pointer to associated image information object
RETURNS    : nothing
*/
void MergeRowComponents(CompI head, ImgP img)
{
  CompP c = *head;

  while (c != NULL)
    {
      MergeToLeft(c, head, img);
      c = c->prev;
    }
}

/*
FUNCTION   : void MergeToLeft(CompP comp, CompI head, ImgP img)
DESCRIPTION: Recursively merges each neigbouring component to the left of the
	     given component.
ARGUMENTS  : comp, pointer to connected component whose left neighbors are
		   sought
	     head, pointer to head of connected component linked list
	     img,  pointer to associated image information object
RETURNS    : nothing
*/
void MergeToLeft(CompP comp, CompI head, ImgP img)
{
  int xl, yl;
  int xm, ym;
  int idx, last_comp = 0;
  CompP c = *head, p, pp;

#if TRACE_ROWMERGE
  printf("MergeToLeft() called for component %d... ", comp->id);
  if (comp->extended)
    printf("already extended, exiting.\n");
  else
    printf("\n");
#endif
#ifdef BOGUS
  if (comp->extended)
    return;

  comp->extended = 1;
#endif


  xl = comp->x_min;
  yl = comp->y_left;

  /* Look for a component in left endpoint's adjacency zone.    */
  /* The adacency zone is approximately a half-circle of radius */
  /* SAME_ROW_THRESHOLD pixels centered at the left endpoint.   */

/*
  for (ym = - SAME_ROW_THRESHOLD; ym <= SAME_ROW_THRESHOLD; ym++)
    for (xm = - (SAME_ROW_THRESHOLD + 1) + abs(ym); xm <= 0; xm++)
*/
  for (ym = - SAME_ROW_V; ym <= SAME_ROW_V; ym++)
    for (xm = - SAME_ROW_H; xm <= 0; xm++)
      {
	if ((yl + ym < 0) || (yl + ym > img->height - 1) ||
	    (xl + xm < 0) || (xl + xm > img->width - 1) ||
	    (idx = ((yl + ym) * img->width) + (xl + xm)) >
	    (img->width * img->height) || (idx < 0))
	  continue;

	if ((img->lmap[idx]) && (img->lmap[idx] != comp->id) &&
	    (img->lmap[idx] != last_comp))
	  {
#if TRACE_ROWMERGE
	    printf("Found component %d in vicinity of component %d\n",
		   img->lmap[idx], comp->id);
#endif
	    /* Locate info object of component obtained from label map. */
	    p = pp = NULL;
	    c = *head;
	    while ((c != NULL) && (c->id != img->lmap[idx]))
	      {
		pp = p;
		p = c;
		c = c->prev;
	      }

	    if (c == NULL)
	      {
		printf("Unexpected error in MergeToLeft(), aborting.\n");
		exit(1);
	      }

	    /* Perform merge only if components have same orientation. */
	    if (comp->type == c->type)
	      {
#if TRACE_ROWMERGE
		printf("%d and %d have same orientation. Merging.\n",
		       comp->id, c->id);
#endif

#ifdef BOGUS
		/* Recurse to build maximum left extent of component. */
		MergeToLeft(c, head, img);

		if (p == NULL)
		  {
		    printf("This shouldn't happen. Exiting.\n");
		    exit(1);
		    /* *head = c->prev; */
		  }

		if (pp->prev == c) /* big hack */
		  p = pp;

		/* Perform actual component merge operation. */
		MergeComponents(comp, c, p, NULL, img);
#endif
		MergeComponents(comp, c, p, head, img);
		MergeToLeft(comp, head, img);
	      }

	    last_comp = c->id;
	  }
      }
#if TRACE_ROWMERGE
  printf("MergeToLeft() for component %d exiting.\n", comp->id);
#endif
}

/*
FUNCTION   : void PairComponents(CompI head, ImgP img)
DESCRIPTION: Locates opposing connected component pairs.
ARGUMENTS  : head, pointer to linked list of connected components
             img,  pointer to associated image information object
RETURNS    : nothing
*/
void PairComponents(CompI head, ImgP img)
{
  CompP c = *head, p = NULL, o; /* current, previous, opposite */
  int xm, ym, y, idx;
  int last_comp = 0;
  char discard = 0, found = 0, end_row = 0;
  PixP cpix, ppix;
  
  /* Loop through all connected components. */
  
  while (c != NULL)
    {
#if TRACE_PAIR
      printf("Pairing component %d\n", c->id);
#endif

      /* Process only components which have not been paired yet. */
      
      if (c->paired == NULL)
        {
          /* Scan inside rectangular grid above or below current component. */
	  
          for (y = 1; (y <= MAX_CHAR_SIZE) && !(discard || found); y++)
            for (xm = c->x_min; (xm <= c->x_max) && !(discard || found); xm++)
              {
                if (end_row && (xm == c->x_min))
                  {
                    discard = 1;
                    break;
                  }
		
                ym = (c->type == E_FALLING) ? -y : y;
                idx = ((c->avg_row + ym) * img->width) + xm;
		
                /* Make sure current index is within image bounds. */
             	if ((c->avg_row + ym < 0) ||
                    (c->avg_row + ym > img->height - 1) ||
                    (idx > img->width * img->height) ||
                    (idx < 0))
             	  continue;
		
                /* Check if there's a different component at this position. */
          	if ((img->lmap[idx]) && (img->lmap[idx] != c->id) &&
             	    (img->lmap[idx] != last_comp))
                  {
#if TRACE_PAIR
		    printf("Found component %d at (%d, %d), ym = %d\n",
			   img->lmap[idx], c->avg_row + ym, xm, ym);
#endif

                    /* If found component is too close, discontinue */
                    /* processing of current component. */
                    if (abs(ym) < MIN_CHAR_SIZE)
                      discard = 1;
                    else
                      {
                        /* Locate component info object. */
                        o = *head;
                        while ((o != NULL) && (o->id != img->lmap[idx]))
                          o = o->prev;
			
                        if (o == NULL)
                          {
                            printf("Unexpected error in PairComponents(). ");
                            printf("Exiting.\n");
                            exit(1);
                          }
			
                      	/* Discontinue processing if component found   */
                      	/* has same orientation as current component.  */
                        /* Otherwise, test for overlap, and if it      */
                        /* fails, continue scan only until end of row. */

#if TRACE_PAIR
			if (c->type == o->type)
			  printf("%d and %d have same orientation.\n",
				 c->id, o->id);
#endif
                      	if (c->type == o->type)
                   	  discard = 1;
                        else
                          {
                            if (Overlap(c, o))
			      {
#if TRACE_PAIR
				printf("%d and %d overlap. Pairing.\n",
				       c->id, o->id);
#endif
				found = 1;
				c->paired = o;
				o->paired = c;
			      }
                            else
                              end_row = 1;
                          }
                      }
                  }
		
                last_comp = img->lmap[idx];
              } /* grid scan */
	  
          /* If scan was aborted or no opposite component    */
          /* was found, discard current component from list. */
	  
          if (discard || !found)
            {
#if TRACE_PAIR
	      printf("Discarding component %d, discard = %d, found = %d\n",
		     c->id, discard, found);
#endif
              cpix = c->pixels;
              while (cpix != NULL)
                {
                  img->lmap[(cpix->y * img->width) + cpix->x] = 0;
                  ppix = cpix;
                  cpix = cpix->prev;
                  free(ppix);
                }
	      
              if (p != NULL)
                p->prev = c->prev;
              else
                *head = c->prev;
	      
              free(c);
            }
        } /* paired check */
      else
	{
#if TRACE_PAIR
	  printf("Component %d already paired. Skipping.\n", c->id);
#endif
	  found = 1;
	}
      
      if (discard || !found)
	{
	  if (p != NULL)
	    c = p->prev;
	  else
	    c = *head;
	}
      else
	{
	  p = c;
	  c = c->prev;
	}
      last_comp = 0;
      discard = found = end_row = 0;
    } /* component loop */
}

/*
FUNCTION   : char Overlap(CompP c1, Comp c2)
DESCRIPTION: Determines whether two connected components overlap sufficiently.
ARGUMENTS  : c1, first component
             c2, second component
RETURNS    : 0, if overlap is insufficient
             1, if overlap is at least 50% of each component's length
*/
char Overlap(CompP c1, CompP c2)
{
  int r1l2, l2l1, r2r1, r2l1, r1l1, r2l2;
  float overlap1, overlap2;

#if TRACE_PAIR
  printf("Overlap() called for components %d and %d\n", c1->id, c2->id);
#endif

  r1l2 = c1->x_max - c2->x_min;
  l2l1 = c2->x_min - c1->x_min;
  r2r1 = c2->x_max - c1->x_max;
  r2l1 = c2->x_max - c1->x_min;

  r1l1 = c1->x_max - c1->x_min;
  r2l2 = c2->x_max - c2->x_min;

  if ((r1l2 >= 0) && (l2l1 >= 0) && (r2r1 >= 0) && (r2l1 > 0))
    {
      overlap1 = (float) r1l2 / (float) r1l1;
      overlap2 = (float) r1l2 / (float) r2l2;
      
      return ((overlap1 >= .5) && (overlap2 >= .5));
    }
  
  if ((r1l2 > 0) && (l2l1 <= 0) && (r2r1 <= 0) && (r2l1 >= 0))
    {
      overlap1 = (float) r2l1 / (float) r1l1;
      overlap2 = (float) r2l1 / (float) r2l2;
      
      return ((overlap1 >= .5) && (overlap2 >= .5));
    }
  
  if ((r1l2 > 0) && (l2l1 <= 0) && (r2r1 >= 0) && (r2l1 > 0))
    {
      /* overlap1 = 1; */
      overlap2 = (float) r1l1 / (float) r2l2;
      
      return (overlap2 >= .5);
    }
  
  if ((r1l2 > 0) && (l2l1 >= 0) && (r2r1 <= 0) && (r2l1 > 0))
    {
      overlap1 = (float) r2l2 / (float) r1l1;
      /* overlap2 = 1; */
      
      return (overlap1 >= .5);
    }
  
  if ((r1l2 < 0) && (l2l1 > 0) && (r2r1 > 0) && (r2l1 > 0))
    {
      /* overlap1 = 0; */
      /* overlap2 = 0; */
      
      return (0);
    }
  
  if ((r1l2 > 0) && (l2l1 < 0) && (r2r1 < 0) && (r2l1 < 0))
    {
      /* overlap1 = 0; */
      /* overlap2 = 0; */
      
      return (0);
    }
  
  printf("Unexpected case or mathematical absurdity reached in Overlap(). ");
  printf("Twink!\n");
  exit(1);
  
  return(0); /* To shut compiler up. */
}

/*
FUNCTION   : void ComputeBoundingBoxes(CompP comp, ImgP img)
DESCRIPTION: Computes and draws text bounding boxes.
ARGUMENTS  : comp, linked list of connected components
             img,  pointer to image information object
RETURNS    : nothing
*/
void ComputeBoundingBoxes(CompP comp, ImgP img)
{
  CompP c = comp;
  int x_min, x_max, y_min, y_max;
  int x, y, idx, pixel;
  FILE *fp;
  char *s;
  unsigned char val;

  if (c == NULL)
    return;

  while (c != NULL)
    {
      if (c->type != E_FALLING)
	{
	  c = c->prev;
	  continue;
	}

      x_min = (c->x_min < c->paired->x_min) ? c->x_min : c->paired->x_min;
      x_max = (c->x_max > c->paired->x_max) ? c->x_max : c->paired->x_max;
      y_min = (c->y_min < c->paired->y_min) ? c->y_min : c->paired->y_min;
      y_max = (c->y_max > c->paired->y_max) ? c->y_max : c->paired->y_max;

      for (x = x_min; x <= x_max; x++)
	{
	  idx = (c->y_max * img->width) + x;
	  img->data[idx] = 255;
	  idx = (c->paired->y_min * img->width) + x;
	  img->data[idx] = 255;
	}

      for (y = y_min; y <= y_max; y++)
	{
	  idx = (y * img->width) + x_min;
	  img->data[idx] = 255;
	  idx = (y * img->width) + x_max;
	  img->data[idx] = 255;
	}

      c = c->prev;
    }

  s = (char *) malloc((strlen(img->imgname) + strlen(".out.pgm") + 1) *
		      sizeof(char));
  sprintf(s, "%s.out.pgm", img->imgname);

  fp = stdout;

  fprintf(fp, "P5\n");
  fprintf(fp, "%d %d\n", img->width, img->height);
  fprintf(fp, "255\n");

  for (pixel = 0; pixel < (img->width * img->height); pixel++)
    {
      val = img->data[pixel];
      fwrite(&val, sizeof(char), 1, fp);
    }

  free(s);
}

