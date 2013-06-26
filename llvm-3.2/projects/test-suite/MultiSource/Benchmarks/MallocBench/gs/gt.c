/* Copyright (C) 1989 Aladdin Enterprises.  All rights reserved.
   Distributed by Free Software Foundation, Inc.

This file is part of Ghostscript.

Ghostscript is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY.  No author or distributor accepts responsibility
to anyone for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.  Refer
to the Ghostscript General Public License for full details.

Everyone is granted permission to copy, modify and redistribute
Ghostscript, but only under the conditions described in the Ghostscript
General Public License.  A copy of this license is supposed to have been
given to you along with Ghostscript so you can know your rights and
responsibilities.  It should be in a file named COPYING.  Among other
things, the copyright notice and this notice must be preserved on all
copies.  */

/* gt.c */
/* Test program for GhostScript library */
#include "math_.h"
#include "gx.h"
#include "malloc_.h"
#include "memory_.h"
#include "gsmatrix.h"
#include "gsstate.h"
#include "gscoord.h"
#include "gspaint.h"
#include "gspath.h"
#include "gxdevice.h"

int code;
private void e(P2(int, char *));
private void ze(P2(char *, char *));
#define z(ptr,msg) ze((char *)(ptr), msg)

private long get_time();

/* Parameters set by swproc and argproc */
private int testn = 0;
private int use_null = 0;
private int show_all = 0;
private int repeat = 1;
#define max_params 2
private int params[max_params];
/* Split main and real_main so we can run under a co-linked debugger. */
int real_main(P2(int, char *[]));
main(int argc, char *argv[])
{	return real_main(argc, argv);
}
int
real_main(int argc, char *argv[])
{	int (*proc)(P2(gs_state *, int *));
	gs_state *pgs;
	float ega_scale = 350.0 / (72 * 11);
	float htproc(P2(floatp, floatp));
	int swproc(P2(char, char *));
	void argproc(P2(char *, int));
	memset(params, 0, max_params * sizeof(int));
	gs_main(argc, argv, "GT.MAP", swproc, argproc);
	switch ( testn )
	   {
#define set_test(tproc)\
	     { int tproc(P2(gs_state *, int *)); proc = tproc; }
	case 1: set_test(test1); break;
	case 2: set_test(test2); break;
	case 3: set_test(test3); break;
	case 4: set_test(test4); break;
	case 6: set_test(test6); break;
	case 99: set_test(test99); break;
	case 100: set_test(test100); break;
	case 101: set_test(test101); break;
	default:
		printf("Unknown test #%d\n", testn);
		exit(1);
	   }
	z(pgs = gs_state_alloc(gs_malloc, gs_free), "alloc");
	e(gs_setdevice(pgs, gs_getdevice(0)), "setdevice");
	if ( use_null )
		gx_device_no_output(pgs);	/* suppress output */
	e(gs_setscreen(pgs, 10.0, 45.0, htproc), "setscreen");
	e(gs_initgraphics(pgs), "initgraphics");
	e(gs_erasepage(pgs), "erasepage");
	   {	gs_matrix mat;
		gs_make_identity(&mat);
		mat.xx = ega_scale * 48 / 35;
		mat.yy = -ega_scale;
		mat.ty = 350;
		e(gs_setmatrix(pgs, &mat), "setmatrix");
	   }
	   {	long time1, ttime;
		int count = repeat;
		time1 = get_time();
		do
		  { e(gs_gsave(pgs), "outer gsave");
		    (*proc)(pgs, params);
		    gs_copypage(pgs);
		    e(gs_grestore(pgs), "outer grestore");
		  }
		while ( --count );
		ttime = get_time() - time1;
		if ( repeat == 1 )
		  printf("%ld\n", ttime);
		else
		  printf("%ld / %d = %ld\n", ttime, repeat, ttime / repeat);
	   }
	getchar();		/* wait for confirmation */
}
float
htproc(floatp x, floatp y)
{	return (float)(1.0 - (x*x + y*y));
}
/* Process switches */
int
swproc(char sw, char *arg)
{	switch ( sw )
	   {
	default:
		return -1;
	case 'A':	/* show all steps */
		show_all = 1;
		break;
	case 'N':	/* select null device */
		use_null = 1;
		break;
	case 'R':	/* repeat test (for timing)  */
		sscanf(arg, "%d", &repeat);
		if ( repeat <= 0 )
		  {	printf("Repeat count <= 0\n");
			exit(1);
		  }
		break;
	   }
	return 0;
}
/* Process parameters */
void
argproc(char *arg, int index)
{	if ( index == 0 )
	  { sscanf(arg, "%d", &testn);
	  }
	else if ( index <= max_params )
	  sscanf(arg, "%d", &params[index - 1]);
	else
	  { printf("Too many arguments\n");
	    exit(1);
	  }
}
private void
e(int code, char *str)
{	if ( show_all ) printf("%s\n", str);
	if ( code < 0 )
	  { printf("Error, code=%d in %s\n", code, str);
	    exit(1);
	  }
}
private void
ze(char *ptr, char *str)
{	if ( show_all ) printf("%s\n", str);
	if ( ptr == NULL )
	  { printf("Error, result=0 in %s\n", code, str);
	    exit(1);
	  }
}

#define inch(n) (float)((n) * 72)

/* A program for testing stroking */
/* 1st parameter is # of degrees to rotate between strokes */
test99(register gs_state *pgs, int *params)
{	int i;
	float ang = params[0];
	float atot = 0;
	if ( ang == 0 ) ang = 30;
	gs_translate(pgs, inch(4), inch(5));
	gs_scale(pgs, inch(3.5), inch(3.5));
	gs_setlinewidth(pgs, 0.1);
	while ( atot < 360 )
	   {	gs_moveto(pgs, 0.25, 0.0);
		gs_lineto(pgs, 1.0, 0.0);
		gs_stroke(pgs);
		gs_rotate(pgs, ang);
		atot += ang;
	   }
}

/* A mandala program for testing stroke speed */
test100(register gs_state *pgs, int *params)
{	int N = max(params[0], 2);
	int delta, i;
	float ang1 = 360.0 / N;
	e(gs_translate(pgs, inch(4.25), inch(5.5)), "translate");
	e(gs_scale(pgs, inch(3.5), inch(3.5)), "scale");
	e(gs_setlinewidth(pgs, 0.0), "setlinewidth");
	for ( delta = 1; delta <= N / 2; delta++ )
	   {	float ang = ang1 * delta;
		e(gs_newpath(pgs), "newpath");
		for ( i = 0; i < N; i++ )
		   {	e(gs_moveto(pgs, 0.0, 1.0), "moveto");
			e(gs_rotate(pgs, ang), "rotate 1");
			e(gs_lineto(pgs, 0.0, 1.0), "lineto");
			e(gs_rotate(pgs, (float)(ang1 - ang)), "rotate 2");
		   }
		e(gs_stroke(pgs), "stroke");
	   }
}

/* A program for testing colors */
test101(register gs_state *pgs, int *params)
{	int i, j, k;
	float unit = 36.0;
	for ( i = 0; i <= 4; i++ )
	  for ( j = 0; j <= 4; j++ )
	    for ( k = 0; k <= 4; k++ )
	      { e(gs_setrgbcolor(pgs, i * 0.25, j * 0.25, k * 0.25), "setrgbcolor");
		gs_newpath(pgs);
		gs_moveto(pgs, (j * 5 + k + 0.1) * unit, (i + 0.1) * unit);
		gs_rlineto(pgs, 0.0, unit);
		gs_rlineto(pgs, unit, 0.0);
		gs_rlineto(pgs, 0.0, -unit);
		gs_closepath(pgs);
		gs_fill(pgs);
	      }
}

/* ------ Programs from the PostScript Cookbook ------ */

/* Program 1 from the PostScript Cookbook */
/* 1st parameter gives # of petals, default is 3 */
test1(register gs_state *pgs, int *params)
{	void wedge(P2(gs_state *, floatp));
	int i;
	int param = params[0];
	int nseg = (abs(param) < 3 ? (param = 3) : abs(param));
	float ang = 360.0 / nseg;
	float ang2 = ang / 2;
	e(gs_gsave(pgs), "gsave 1");
	e(gs_translate(pgs, inch(3.75), inch(7.25)), "translate");
	e(gs_scale(pgs, inch(1), inch(1)), "scale");
	wedge(pgs, ang2);
	e(gs_setlinewidth(pgs, 0.02), "setlinewidth");
	e(gs_stroke(pgs), "stroke");
	e(gs_grestore(pgs), "grestore 1");
	e(gs_gsave(pgs), "gsave 2");
	gs_translate(pgs, inch(4.25), inch(4.25));
	gs_scale(pgs, inch(1.75), inch(1.75));
	gs_setlinewidth(pgs, 0.02);
	for ( i = 1; i <= nseg; i++ )
	   {	gs_setgray(pgs, (float)i / param);
		if ( param < 0 ) gs_rotate(pgs, -ang);
		e(gs_gsave(pgs), "gsave 3");
		wedge(pgs, ang2);
		e(gs_gsave(pgs), "gsave 4");
		gs_fill(pgs);
		e(gs_grestore(pgs), "grestore 4");
		gs_setgray(pgs, 0.0);
		gs_stroke(pgs);
		e(gs_grestore(pgs), "grestore 3");
		if ( param > 0 ) gs_rotate(pgs, ang);
	   }
	e(gs_grestore(pgs), "grestore 2");
}
void
wedge(register gs_state *pgs, floatp ang)
{	float asin = sin(ang * M_PI / 180.0);
	e(gs_moveto(pgs, 0.0, 0.0), "moveto");
	e(gs_translate(pgs, 1.0, 0.0), "translate 1");
	e(gs_rotate(pgs, (float)ang), "rotate");
	e(gs_translate(pgs, 0.0, asin), "translate 2");
	e(gs_arc(pgs, 0.0, 0.0, asin, -90.0, 90.0), "arc");
	e(gs_closepath(pgs), "closepath");
}

/* Program 2 from the PostScript Cookbook */
/* 1st parameter specifies a rotation amount */
/* If 2nd parameter is non-zero, trace square in opposite direction */
test2(gs_state *pgs, int *params)
{	void centersquare(P2(gs_state *, int));
	gs_matrix cmtx;
	int i;
	float rota = params[0];
	int cw = params[1];
	e(gs_gsave(pgs), "gsave 1");
	e(gs_translate(pgs, inch(2.5), inch(6)), "translate 1");
	e(gs_setlinewidth(pgs, 1.0 / 16), "setlinewidth 1");
	if ( rota != 0 )
		e(gs_rotate(pgs, rota), "rotate");
	for ( i = 1; i <= 5; i++ )
	   {	e(gs_gsave(pgs), "gsave 2");
		e(gs_scale(pgs, i * inch(0.5), i * inch(0.5)), "scale 1");
		centersquare(pgs, cw);
		e(gs_stroke(pgs), "stroke 1");
		e(gs_grestore(pgs), "grestore 2");
	   }
	e(gs_grestore(pgs), "grestore 1");
	e(gs_gsave(pgs), "gsave 3");
	e(gs_translate(pgs, inch(6), inch(6)), "translate 2");
	e(gs_setlinewidth(pgs, 1.0), "setlinewidth 2");
	e(gs_currentmatrix(pgs, &cmtx), "currentmatrix");
	for ( i = 1; i <= 5; i++ )
	   {	e(gs_gsave(pgs), "gsave 4");
		e(gs_scale(pgs, i * inch(0.5), i * inch(0.5)), "scale 2");
		centersquare(pgs, cw);
		e(gs_setmatrix(pgs, &cmtx), "setmatrix");
		e(gs_stroke(pgs), "stroke 2");
		e(gs_grestore(pgs), "grestore 4");
	   }
	e(gs_grestore(pgs), "grestore 3");
}
void
centersquare(register gs_state *pgs, int cw)
{	float d = (cw ? 0.5 : -0.5);
	e(gs_newpath(pgs), "newpath");
	e(gs_moveto(pgs, 0.5, 0.5), "moveto");
	e(gs_lineto(pgs, d, -d), "lineto 1");
	e(gs_lineto(pgs, -0.5, -0.5), "lineto 2");
	e(gs_lineto(pgs, -d, d), "lineto 3");
	e(gs_closepath(pgs), "closepath");
}

/* Program 3 from the PostScript Cookbook */
test3(register gs_state *pgs, int *params)
{	void ellipse(P7(gs_state *, int, int, int, int, int, int));
	e(gs_newpath(pgs), "newpath 1");
	ellipse(pgs, 144, 400, 72, 144, 0, 360);
	e(gs_stroke(pgs), "stroke 1");
	e(gs_newpath(pgs), "newpath 2");
	ellipse(pgs, 400, 400, 144, 36, 0, 360);
	e(gs_fill(pgs), "fill 2");
	e(gs_newpath(pgs), "newpath 3");
	ellipse(pgs, 300, 180, 144, 72, 30, 150);
	e(gs_stroke(pgs), "stroke 3");
	e(gs_newpath(pgs), "newpath 4");
	ellipse(pgs, 480, 150, 30, 50, 270, 90);
	e(gs_fill(pgs), "fill 4");
}
void
ellipse(register gs_state *pgs,
  int x, int y, int xrad, int yrad, int startangle, int endangle)
{	gs_matrix savematrix;
	e(gs_currentmatrix(pgs, &savematrix), "currentmatrix");
	e(gs_translate(pgs, (float)x, (float)y), "translate");
	e(gs_scale(pgs, (float)xrad, (float)yrad), "scale");
	e(gs_arc(pgs, 0.0, 0.0, 1.0, (float)startangle, (float)endangle), "arc");
	e(gs_setmatrix(pgs, &savematrix), "setmatrix");
}

/* Program 4 from the PostScript Cookbook */
test4(register gs_state *pgs, int *params)
{	void arrow(P8(gs_state *, int, int, int, int, int, int, floatp));
	e(gs_newpath(pgs), "newpath 1");
	arrow(pgs, 318, 340, 72, 340, 10, 30, 72.0);
	e(gs_fill(pgs), "fill 1");
	e(gs_newpath(pgs), "newpath 2");
	arrow(pgs, 382, 400, 542, 560, 72, 232, 116.0);
	e(gs_setlinewidth(pgs, 3.0), "setlinewidth");
	e(gs_stroke(pgs), "stroke");
	e(gs_newpath(pgs), "newpath 3");
	arrow(pgs, 400, 300, 400, 90, 90, 200, 200 * 1.732 / 2);
	e(gs_setgray(pgs, 0.65), "setgray");
	e(gs_fill(pgs), "fill 2");
}
void
arrow(register gs_state *pgs,
  int tailx, int taily, int tipx, int tipy, int thickness, int headthickness,
  floatp hl)
{	float ht = (float)thickness / 2;
	float hht = (float)headthickness / 2;
	float dx = tipx - tailx;
	float dy = tipy - taily;
	float al = sqrt(dx * dx + dy * dy);
	float angle = atan2(dy, dx) * (180.0 / M_PI);
	float base = al - hl;
	gs_matrix savematrix;
	e(gs_currentmatrix(pgs, &savematrix), "currentmatrix");
	e(gs_translate(pgs, (float)tailx, (float)taily), "translate");
	e(gs_rotate(pgs, angle), "rotate");
	e(gs_moveto(pgs, 0.0, (float)-ht), "moveto");
	e(gs_lineto(pgs, base, (float)-ht), "line 1");
	e(gs_lineto(pgs, base, (float)-hht), "line 2");
	e(gs_lineto(pgs, al, 0.0), "line 3");
	e(gs_lineto(pgs, base, hht), "line 4");
	e(gs_lineto(pgs, base, ht), "line 5");
	e(gs_lineto(pgs, 0.0, ht), "line 6");
	e(gs_closepath(pgs), "closepath");
	e(gs_setmatrix(pgs, &savematrix), "setmatrix");
}

/* Program 6 from the PostScript Cookbook */
/* (actually, the test program for the imagemask operator */
/* that appears in the PostScript reference manual) */
/* First parameter is amount to rotate image */
/* If second parameter is non-zero, clip the image */
int ii;
test6(register gs_state *pgs, int *params)
{	gs_matrix mat;
	int i6proc(P2(byte **, int *));
	e(gs_translate(pgs, inch(3), inch(4)), "translate");
	e(gs_scale(pgs, inch(2), inch(2)), "scale");
	gs_moveto(pgs, 0.0, 0.0);
	gs_lineto(pgs, 0.0, 1.0);
	gs_lineto(pgs, 1.0, 1.0);
	gs_lineto(pgs, 1.0, 0.0);
	gs_closepath(pgs);
	gs_setgray(pgs, 0.9);
/***	gs_fill(pgs);	***/
	gs_setgray(pgs, 0.4);
	/* The following is not in the original program. */
	/* It is here to test clipping of images. */
	if ( params[1] )
	   {	e(gs_newpath(pgs), "newpath");
		e(gs_moveto(pgs, 0.0, 0.0), "moveto");
		e(gs_lineto(pgs, 1.0, 3.0), "line1");
		e(gs_lineto(pgs, 3.0, 1.0), "line2");
		e(gs_closepath(pgs), "closepath");
		e(gs_clip(pgs), "clip");
	   }
	e(gs_rotate(pgs, (float)params[0]), "rotate");
	gs_make_identity(&mat);
	mat.xx = 24;
	mat.yy = -23;
	mat.ty = 23;
	ii = 0;
	e(gs_imagemask(pgs, 24, 23, 1, &mat, i6proc), "image");
}
private byte i6data[3 * 23] =
   {	0x00,0x3b,0x00, 0x00,0x27,0x00, 0x00,0x24,0x80, 0x0e,0x49,0x40,
	0x11,0x49,0x20, 0x14,0xb2,0x20, 0x3c,0xb6,0x50, 0x75,0xfe,0x88,
	0x17,0xff,0x8c, 0x17,0x5f,0x14, 0x1c,0x07,0xe2, 0x38,0x03,0xc4,
	0x70,0x31,0x82, 0xf8,0xed,0xfc, 0xb2,0xbb,0xc2, 0xbb,0x6f,0x84,
	0x31,0xbf,0xc2, 0x18,0xea,0x3c, 0x0e,0x3e,0x00, 0x07,0xfc,0x00,
	0x03,0xf8,0x00, 0x1e,0x18,0x00, 0x1f,0xf8,0x00
   };
int
i6proc(byte **pdata, int *psize)
{	*pdata = &i6data[ii++ * 23];
	*psize = 23;
	return 0;
}

/* Read the current time (in milliseconds since midnight). */
private long
get_time()
{	long date_time[2];
	gs_get_clock(date_time);
	return date_time[0] * 86400000L + date_time[1];
}
