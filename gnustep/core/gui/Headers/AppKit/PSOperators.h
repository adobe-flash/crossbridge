/* PSOperators.h - Drawing engine operators that use default context

   Copyright (C) 1999 Free Software Foundation, Inc.
   Written by:  Richard Frith-Macdonald <richard@brainstorm.co.uk>
   Based on code by Adam Fedor
   Date: Feb 1999
   
   This file is part of the GNU Objective C User Interface library.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; see the file COPYING.LIB.
   If not, see <http://www.gnu.org/licenses/> or write to the 
   Free Software Foundation, 51 Franklin Street, Fifth Floor, 
   Boston, MA 02110-1301, USA.
*/

#ifndef _PSOperators_h_INCLUDE
#define _PSOperators_h_INCLUDE

#import <AppKit/DPSOperators.h>

#if OS_API_VERSION(GS_API_NONE, GS_API_NONE)
#define	DEFCTXT	GSCurrentContext()
#else
#define	DEFCTXT	[NSGraphicsContext currentContext]
#endif

/* ----------------------------------------------------------------------- */
/* Color operations */
/* ----------------------------------------------------------------------- */
static inline void
PScurrentalpha(float* a)
__attribute__((unused));

static inline void
PScurrentcmykcolor(float* c, float* m, float* y, float* k)
__attribute__((unused));

static inline void
PScurrentgray(float* gray)
__attribute__((unused));

static inline void
PScurrenthsbcolor(float* h, float* s, float* b)
__attribute__((unused));

static inline void
PScurrentrgbcolor(float* r, float* g, float* b)
__attribute__((unused));

static inline void
PSsetalpha(float a)
__attribute__((unused));

static inline void
PSsetcmykcolor(float c, float m, float y, float k)
__attribute__((unused));

static inline void
PSsetgray(float gray)
__attribute__((unused));

static inline void
PSsethsbcolor(float h, float s, float b)
__attribute__((unused));

static inline void
PSsetrgbcolor(float r, float g, float b)
__attribute__((unused));



/* ----------------------------------------------------------------------- */
/* Text operations */
/* ----------------------------------------------------------------------- */
static inline void
PSashow(float x, float y, const char* s)
__attribute__((unused));

static inline void
PSawidthshow(float cx, float cy, int c, float ax, float ay, const char* s)
__attribute__((unused));

static inline void
PScharpath(const char* s, int b)
__attribute__((unused));

static inline void
PSshow(const char* s)
__attribute__((unused));

static inline void
PSwidthshow(float x, float y, int c, const char* s)
__attribute__((unused));

static inline void
PSxshow(const char* s, const float* numarray, int size)
__attribute__((unused));

static inline void
PSxyshow(const char* s, const float* numarray, int size)
__attribute__((unused));

static inline void
PSyshow(const char* s, const float* numarray, int size)
__attribute__((unused));



/* ----------------------------------------------------------------------- */
/* Gstate Handling */
/* ----------------------------------------------------------------------- */
static inline void
PSgrestore()
__attribute__((unused));

static inline void
PSgsave()
__attribute__((unused));

static inline void
PSinitgraphics()
__attribute__((unused));

static inline void
PSsetgstate(int gst)
__attribute__((unused));



/* ----------------------------------------------------------------------- */
/* Gstate operations */
/* ----------------------------------------------------------------------- */
static inline void
PScurrentflat(float* flatness)
__attribute__((unused));

static inline void
PScurrentlinecap(int* linecap)
__attribute__((unused));

static inline void
PScurrentlinejoin(int* linejoin)
__attribute__((unused));

static inline void
PScurrentlinewidth(float* width)
__attribute__((unused));

static inline void
PScurrentmiterlimit(float* limit)
__attribute__((unused));

static inline void
PScurrentpoint(float* x, float* y)
__attribute__((unused));

static inline void
PScurrentstrokeadjust(int* b)
__attribute__((unused));

static inline void
PSsetdash(const float* pat, int size, float offset)
__attribute__((unused));

static inline void
PSsetflat(float flatness)
__attribute__((unused));

static inline void
PSsethalftonephase(float x, float y)
__attribute__((unused));

static inline void
PSsetlinecap(int linecap)
__attribute__((unused));

static inline void
PSsetlinejoin(int linejoin)
__attribute__((unused));

static inline void
PSsetlinewidth(float width)
__attribute__((unused));

static inline void
PSsetmiterlimit(float limit)
__attribute__((unused));

static inline void
PSsetstrokeadjust(int b)
__attribute__((unused));


/* ----------------------------------------------------------------------- */
/* Matrix operations */
/* ----------------------------------------------------------------------- */
static inline void
PSconcat(const float* m)
__attribute__((unused));

static inline void
PSinitmatrix()
__attribute__((unused));

static inline void
PSrotate(float angle)
__attribute__((unused));

static inline void
PSscale(float x, float y)
__attribute__((unused));

static inline void
PStranslate(float x, float y)
__attribute__((unused));



/* ----------------------------------------------------------------------- */
/* Paint operations */
/* ----------------------------------------------------------------------- */
static inline void
PSarc(float x, float y, float r, float angle1, float angle2)
__attribute__((unused));

static inline void
PSarcn(float x, float y, float r, float angle1, float angle2)
__attribute__((unused));

static inline void
PSarct(float x1, float y1, float x2, float y2, float r)
__attribute__((unused));

static inline void
PSclip()
__attribute__((unused));

static inline void
PSclosepath()
__attribute__((unused));

static inline void
PScurveto(float x1, float y1, float x2, float y2, float x3, float y3)
__attribute__((unused));

static inline void
PSeoclip()
__attribute__((unused));

static inline void
PSeofill()
__attribute__((unused));

static inline void
PSfill()
__attribute__((unused));

static inline void
PSflattenpath()
__attribute__((unused));

static inline void
PSinitclip()
__attribute__((unused));

static inline void
PSlineto(float x, float y)
__attribute__((unused));

static inline void
PSmoveto(float x, float y)
__attribute__((unused));

static inline void
PSnewpath()
__attribute__((unused));

static inline void
PSpathbbox(float* llx, float* lly, float* urx, float* ury)
__attribute__((unused));

static inline void
PSrcurveto(float x1, float y1, float x2, float y2, float x3, float y3)
__attribute__((unused));

static inline void
PSrectclip(float x, float y, float w, float h)
__attribute__((unused));

static inline void
PSrectfill(float x, float y, float w, float h)
__attribute__((unused));

static inline void
PSrectstroke(float x, float y, float w, float h)
__attribute__((unused));

static inline void
PSreversepath()
__attribute__((unused));

static inline void
PSrlineto(float x, float y)
__attribute__((unused));

static inline void
PSrmoveto(float x, float y)
__attribute__((unused));

static inline void
PSstroke()
__attribute__((unused));

static inline void
PSshfill(NSDictionary *shaderDictionary)
__attribute__((unused));


/*-------------------------------------------------------------------------*/
/* Graphics Extensions Ops */
/*-------------------------------------------------------------------------*/
static inline void
PScomposite(float x, float y, float w, float h, int gstateNum, float dx, float dy, int op)
__attribute__((unused));

static inline void
PScompositerect(float x, float y, float w, float h, int op)
__attribute__((unused));

static inline void
PSdissolve(float x, float y, float w, float h, int gstateNum, float dx, float dy, float delta)
__attribute__((unused));



/* ----------------------------------------------------------------------- */
/* Postscript Client functions */
/* ----------------------------------------------------------------------- */
static inline void
PSPrintf(const char * fmt, va_list args)
__attribute__((unused));

static inline void
PSWriteData(const char * buf, unsigned int count)
__attribute__((unused));


/* ----------------------------------------------------------------------- */
/* Color operations */
/* ----------------------------------------------------------------------- */
static inline void
PScurrentalpha(float* a)
{
  DPScurrentalpha(DEFCTXT, a);
}

static inline void
PScurrentcmykcolor(float* c, float* m, float* y, float* k)
{
  DPScurrentcmykcolor(DEFCTXT, c, m, y, k);
}

static inline void
PScurrentgray(float* gray)
{
  DPScurrentgray(DEFCTXT, gray);
}

static inline void
PScurrenthsbcolor(float* h, float* s, float* b)
{
  DPScurrenthsbcolor(DEFCTXT, h, s, b);
}

static inline void
PScurrentrgbcolor(float* r, float* g, float* b)
{
  DPScurrentrgbcolor(DEFCTXT, r, g, b);
}

static inline void
PSsetalpha(float a)
{
  DPSsetalpha(DEFCTXT, a);
}

static inline void
PSsetcmykcolor(float c, float m, float y, float k)
{
  DPSsetcmykcolor(DEFCTXT, c, m, y, k);
}

static inline void
PSsetgray(float gray)
{
  DPSsetgray(DEFCTXT, gray);
}

static inline void
PSsethsbcolor(float h, float s, float b)
{
  DPSsethsbcolor(DEFCTXT, h, s, b);
}

static inline void
PSsetrgbcolor(float r, float g, float b)
{
  DPSsetrgbcolor(DEFCTXT, r, g, b);
}

static inline void
PScountwindowlist(int __attribute__((unused)) d, int __attribute__((unused)) *c)
{
  // dummy implementation for now... GJC
}

static inline void
PSwindowlist(int __attribute__((unused)) d,
             int __attribute__((unused)) c,
             int __attribute__((unused)) *array)
{
  // dummy implementation for now... GJC
}

/* ----------------------------------------------------------------------- */
/* Text operations */
/* ----------------------------------------------------------------------- */
static inline void
PSashow(float x, float y, const char* s)
{
  DPSashow(DEFCTXT, x, y, s);
}

static inline void
PSawidthshow(float cx, float cy, int c, float ax, float ay, const char* s)
{
  DPSawidthshow(DEFCTXT, cx, cy, c, ax, ay, s);
}

static inline void
PScharpath(const char* s, int b)
{
  DPScharpath(DEFCTXT, s, b);
}

static inline void
PSshow(const char* s)
{
  DPSshow(DEFCTXT, s);
}

static inline void
PSwidthshow(float x, float y, int c, const char* s)
{
  DPSwidthshow(DEFCTXT, x, y, c, s);
}

static inline void
PSxshow(const char* s, const float* numarray, int size)
{
  DPSxshow(DEFCTXT, s, numarray, size);
}

static inline void
PSxyshow(const char* s, const float* numarray, int size)
{
  DPSxyshow(DEFCTXT, s, numarray, size);
}

static inline void
PSyshow(const char* s, const float* numarray, int size)
{
  DPSyshow(DEFCTXT, s, numarray, size);
}



/* ----------------------------------------------------------------------- */
/* Gstate Handling */
/* ----------------------------------------------------------------------- */
static inline void
PSgsave()
{
  DPSgsave(DEFCTXT);
}

static inline void
PSgrestore()
{
  DPSgrestore(DEFCTXT);
}

static inline void
PSinitgraphics()
{
  DPSinitgraphics(DEFCTXT);
}

static inline void
PSsetgstate(int gst)
{
  DPSsetgstate(DEFCTXT, gst);
}



/* ----------------------------------------------------------------------- */
/* Gstate operations */
/* ----------------------------------------------------------------------- */
static inline void
PScurrentflat(float* flatness)
{
  DPScurrentflat(DEFCTXT, flatness);
}

static inline void
PScurrentlinecap(int* linecap)
{
  DPScurrentlinecap(DEFCTXT, linecap);
}

static inline void
PScurrentlinejoin(int* linejoin)
{
  DPScurrentlinejoin(DEFCTXT, linejoin);
}

static inline void
PScurrentlinewidth(float* width)
{
  DPScurrentlinewidth(DEFCTXT, width);
}

static inline void
PScurrentmiterlimit(float* limit)
{
  DPScurrentmiterlimit(DEFCTXT, limit);
}

static inline void
PScurrentpoint(float* x, float* y)
{
  DPScurrentpoint(DEFCTXT, x, y);
}

static inline void
PScurrentstrokeadjust(int* b)
{
  DPScurrentstrokeadjust(DEFCTXT, b);
}

static inline void
PSsetdash(const float* pat, int size, float offset)
{
  DPSsetdash(DEFCTXT, pat, size, offset);
}

static inline void
PSsetflat(float flatness)
{
  DPSsetflat(DEFCTXT, flatness);
}

static inline void
PSsethalftonephase(float x, float y)
{
  DPSsethalftonephase(DEFCTXT, x, y);
}

static inline void
PSsetlinecap(int linecap)
{
  DPSsetlinecap(DEFCTXT, linecap);
}

static inline void
PSsetlinejoin(int linejoin)
{
  DPSsetlinejoin(DEFCTXT, linejoin);
}

static inline void
PSsetlinewidth(float width)
{
  DPSsetlinewidth(DEFCTXT, width);
}

static inline void
PSsetmiterlimit(float limit)
{
  DPSsetmiterlimit(DEFCTXT, limit);
}

static inline void
PSsetstrokeadjust(int b)
{
  DPSsetstrokeadjust(DEFCTXT, b);
}


/* ----------------------------------------------------------------------- */
/* Matrix operations */
/* ----------------------------------------------------------------------- */
static inline void
PSconcat(const float* m)
{
  DPSconcat(DEFCTXT, m);
}

static inline void
PSinitmatrix()
{
  DPSinitmatrix(DEFCTXT);
}

static inline void
PSrotate(float angle)
{
  DPSrotate(DEFCTXT, angle);
}

static inline void
PSscale(float x, float y)
{
  DPSscale(DEFCTXT, x, y);
}

static inline void
PStranslate(float x, float y)
{
  DPStranslate(DEFCTXT, x, y);
}



/* ----------------------------------------------------------------------- */
/* Paint operations */
/* ----------------------------------------------------------------------- */
static inline void
PSarc(float x, float y, float r, float angle1, float angle2)
{
  DPSarc(DEFCTXT, x, y, r, angle1, angle2);
}

static inline void
PSarcn(float x, float y, float r, float angle1, float angle2)
{
  DPSarcn(DEFCTXT, x, y, r, angle1, angle2);
}

static inline void
PSarct(float x1, float y1, float x2, float y2, float r)
{
  DPSarct(DEFCTXT, x1, y1, x2, y2, r);
}

static inline void
PSclip()
{
  DPSclip(DEFCTXT);
}

static inline void
PSclosepath()
{
  DPSclosepath(DEFCTXT);
}

static inline void
PScurveto(float x1, float y1, float x2, float y2, float x3, float y3)
{
  DPScurveto(DEFCTXT, x1, y1, x2, y2, x3, y3);
}

static inline void
PSeoclip()
{
  DPSeoclip(DEFCTXT);
}

static inline void
PSeofill()
{
  DPSeofill(DEFCTXT);
}

static inline void
PSfill()
{
  DPSfill(DEFCTXT);
}

static inline void
PSflattenpath()
{
  DPSflattenpath(DEFCTXT);
}

static inline void
PSinitclip()
{
  DPSinitclip(DEFCTXT);
}

static inline void
PSlineto(float x, float y)
{
  DPSlineto(DEFCTXT, x, y);
}

static inline void
PSmoveto(float x, float y)
{
  DPSmoveto(DEFCTXT, x, y);
}

static inline void
PSnewpath()
{
  DPSnewpath(DEFCTXT);
}

static inline void
PSpathbbox(float* llx, float* lly, float* urx, float* ury)
{
  DPSpathbbox(DEFCTXT, llx, lly, urx, ury);
}

static inline void
PSrcurveto(float x1, float y1, float x2, float y2, float x3, float y3)
{
  DPSrcurveto(DEFCTXT, x1, y1, x2, y2, x3, y3);
}

static inline void
PSrectclip(float x, float y, float w, float h)
{
  DPSrectclip(DEFCTXT, x, y, w, h);
}

static inline void
PSrectfill(float x, float y, float w, float h)
{
  DPSrectfill(DEFCTXT, x, y, w, h);
}

static inline void
PSrectstroke(float x, float y, float w, float h)
{
  DPSrectstroke(DEFCTXT, x, y, w, h);
}

static inline void
PSreversepath()
{
  DPSreversepath(DEFCTXT);
}

static inline void
PSrlineto(float x, float y)
{
  DPSrlineto(DEFCTXT, x, y);
}

static inline void
PSrmoveto(float x, float y)
{
  DPSrmoveto(DEFCTXT, x, y);
}

static inline void
PSstroke()
{
  DPSstroke(DEFCTXT);
}

static inline void
PSshfill(NSDictionary *shaderDictionary)
{
  DPSshfill(DEFCTXT, shaderDictionary);
}


/*-------------------------------------------------------------------------*/
/* Graphics Extensions Ops */
/*-------------------------------------------------------------------------*/
static inline void
PScomposite(float x, float y, float w, float h, int gstateNum, float dx, float dy, int op)
{
  DPScomposite(DEFCTXT, x, y, w, h, gstateNum, dx, dy, op);
}

static inline void
PScompositerect(float x, float y, float w, float h, int op)
{
  DPScompositerect(DEFCTXT, x, y, w, h, op);
}

static inline void
PSdissolve(float x, float y, float w, float h, int gstateNum, float dx, float dy, float delta)
{
  DPSdissolve(DEFCTXT, x, y, w, h, gstateNum, dx, dy, delta);
}



/* ----------------------------------------------------------------------- */
/* Postscript Client functions */
/* ----------------------------------------------------------------------- */
static inline void
PSPrintf(const char * fmt, va_list args)
{
  DPSPrintf(DEFCTXT, fmt, args);
}

static inline void
PSWriteData(const char * buf, unsigned int count)
{
  DPSWriteData(DEFCTXT, buf, count);
}

static inline void
PSWait()
{
  // do nothing.
}
#endif	
