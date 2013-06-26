/*@z34.c:Rotation Service:Declarations@***************************************/
/*                                                                           */
/*  THE LOUT DOCUMENT FORMATTING SYSTEM (VERSION 3.24)                       */
/*  COPYRIGHT (C) 1991, 2000 Jeffrey H. Kingston                             */
/*                                                                           */
/*  Jeffrey H. Kingston (jeff@cs.usyd.edu.au)                                */
/*  Basser Department of Computer Science                                    */
/*  The University of Sydney 2006                                            */
/*  AUSTRALIA                                                                */
/*                                                                           */
/*  This program is free software; you can redistribute it and/or modify     */
/*  it under the terms of the GNU General Public License as published by     */
/*  the Free Software Foundation; either Version 2, or (at your option)      */
/*  any later version.                                                       */
/*                                                                           */
/*  This program is distributed in the hope that it will be useful,          */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            */
/*  GNU General Public License for more details.                             */
/*                                                                           */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program; if not, write to the Free Software              */
/*  Foundation, Inc., 59 Temple Place, Suite 330, Boston MA 02111-1307 USA   */
/*                                                                           */
/*  FILE:         z34.c                                                      */
/*  MODULE:       Rotation Service                                           */
/*  EXTERNS:      RotateSize()                                               */
/*                                                                           */
/*****************************************************************************/
#include <math.h>
#ifndef M_PI
#define M_PI       3.1415926535897931160E0
#endif
#include "externs.h"

typedef struct { double x, y;          } rect_coord;
typedef struct { double angle, radius; } polar_coord;

#define rect_to_polar(rect, polar)				\
polar.angle = atan2(rect.y, rect.x),				\
polar.radius = sqrt(rect.x*rect.x + rect.y*rect.y)

#define polar_to_rect(polar, rect)				\
rect.x = polar.radius * cos(polar.angle),			\
rect.y = polar.radius * sin(polar.angle)


/*@::RotateSize()@************************************************************/
/*                                                                           */
/*  RotateSize(xcb, xcf, xrb, xrf, y, theta)                                 */
/*                                                                           */
/*  Calculate the size of x, assuming that it is y rotated by theta degrees. */
/*                                                                           */
/*****************************************************************************/

void RotateSize(FULL_LENGTH *xcb, FULL_LENGTH *xcf, FULL_LENGTH *xrb,
  FULL_LENGTH *xrf, OBJECT y, FULL_LENGTH theta)
{ rect_coord ycorners[4], xcorner;  polar_coord pol;
  double maxx, maxy, minx, miny, ang;  int i;
#if DEBUG_ON
  char buff1[20], buff2[20];
#endif

  /* calculate theta in radians */
  ang = (double) theta * 2 * M_PI / (double) (DG * 360);
  ifdebug(DRS, D, sprintf(buff2, "%.1f", ang));
  debug2(DRS, D, "RotateSize( %s, %s )", EchoObject(y), buff2);
  debug4(DRS, DD, "  ycb %s, ycf %s, yrb %s, yrf %s",
	EchoLength(back(y, COLM)), EchoLength(fwd(y, COLM)),
	EchoLength(back(y, ROWM)), EchoLength(fwd(y, ROWM)));

  /* set up coordinates of the four corners of y */
  ycorners[0].x =   (float) fwd(y, COLM);
  ycorners[0].y =   (float) back(y, ROWM);
  ycorners[1].x = - (float) back(y, COLM);
  ycorners[1].y =   (float) back(y, ROWM);
  ycorners[2].x = - (float) back(y, COLM);
  ycorners[2].y = - (float) fwd(y, ROWM);
  ycorners[3].x =   (float) fwd(y, COLM);
  ycorners[3].y = - (float) fwd(y, ROWM);

  /* rotate these four corners by theta and store their extremes */
  maxx = maxy = (float) - MAX_FULL_LENGTH;
  minx = miny = (float) MAX_FULL_LENGTH;
  for( i = 0;  i < 4;  i++ )
  {	
    if( ycorners[i].x == 0 && ycorners[i].y == 0 )
    {	pol.radius = 0; pol.angle  = 0; }
    else rect_to_polar(ycorners[i], pol);
    ifdebug(DRS, DD, sprintf(buff1, "%.1f", pol.angle));
    ifdebug(DRS, DD, sprintf(buff2, "%.1f", ang));
    debug5(DRS, DD, "  transforming (%s, %s) -> (%s, %s) + %s",
      EchoLength( (int) ycorners[i].x), EchoLength( (int) ycorners[i].y),
      EchoLength( (int) pol.radius), buff1, buff2);
    pol.angle += ang;
    polar_to_rect(pol, xcorner);
    ifdebug(DRS, DD, sprintf(buff1, "%.1f", pol.angle));
    debug4(DRS, DD, "    transforming (%s, %s) -> (%s, %s)",
      EchoLength( (int) pol.radius), buff1,
      EchoLength( (int) xcorner.x), EchoLength( (int) xcorner.y) );
    maxx = find_max(maxx, xcorner.x);    minx = find_min(minx, xcorner.x);
    maxy = find_max(maxy, xcorner.y);    miny = find_min(miny, xcorner.y);
  }

  /* store sizes back into x and return */
  *xcb = - (int) minx;    *xcf  =   (int) maxx;
  *xrb =   (int) maxy;    *xrf  = - (int) miny;
  debug0(DRS, D, "RotateSize returning.");
  debug4(DRS, DD, "  xcb %s, xcf %s, xrb %s, xrf %s",
    EchoLength(*xcb), EchoLength(*xcf),
    EchoLength(*xrb), EchoLength(*xrf));
} /* end RotateSize */
