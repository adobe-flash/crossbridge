/* Copyright (C) 1989, 1990, 1991 Aladdin Enterprises.  All rights reserved.
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

/* gdevx.c */
/* X Windows driver for GhostScript library */
/* The X include files include <sys/types.h>, which, on some machines */
/* at least, define byte, uint, ushort, and ulong, which std.h also defines. */
/* We have to shuffle the order of includes to make this all work. */
/* First we arrange things so that std.h defines _byte, _uint, _ushort, */
/* and _ulong instead of (our own) byte, uint, ushort, and ulong. */
#  define byte _byte
#  define uint _uint
#  define ushort _ushort
#  define ulong _ulong
#include "gx.h"			/* for gx_bitmap; includes std.h */
#include "malloc_.h"
#include "memory_.h"
#include "gsmatrix.h"			/* needed for gxdevice.h */
#include "gxbitmap.h"
#include "gxdevice.h"
#  undef byte
#  undef uint
#  undef ushort
#  undef ulong
/* Now we can include the X files. */
#include "gdevx.h"

typedef struct gx_device_s gx_device;

/* Flags for patching around bugs in the X library */
#define use_XPutImage 1
#define use_XSetTile 1

/* Define whether to use a backing pixmap to handle expose events. */
/* Note that this is a variable rather than a #define. */
/* Note also that it is consulted each time we open an X device. */
private int use_backing = 1;

/* Define the maximum size of the temporary pixmap for copy_mono */
/* that we are willing to leave lying around in the server */
/* between uses.  (Assume 32-bit ints here!) */
private int max_temp_pixmap = 20000;

/* Forward references */
private int set_tile(P2(gx_device *, gx_bitmap *));
private void free_cp(P1(gx_device *));
/* Screen updating machinery */
#define update_init(dev)\
  ((gx_device_X *)(dev))->up_area = 0,\
  ((gx_device_X *)(dev))->up_count = 0
#define update_flush(dev)\
  if ( ((gx_device_X *)(dev))->up_area != 0 ) update_do_flush(dev)
private void update_do_flush(P1(gx_device *));
private void update_add(P5(gx_device *, int, int, int, int));

/* Procedures */

int x_open(P1(gx_device *));

int x_close(P1(gx_device *));

gx_color_index x_map_rgb_color(P4(gx_device *, _ushort, _ushort, _ushort));

int x_map_color_rgb(P3(gx_device *, gx_color_index, _ushort *));

int x_sync(P1(gx_device *));

int x_fill_rectangle(P6(gx_device *, int, int, int, int, gx_color_index));

int x_tile_rectangle(P8(gx_device *, gx_bitmap *, int, int, int, int, gx_color_index, gx_color_index));

int x_copy_mono(P10(gx_device *, _byte *, int, int, int, int, int, int, gx_color_index, gx_color_index));

int x_copy_color(P8(gx_device *, _byte *, int, int, int, int, int, int));

int x_draw_line(P6(gx_device *, int, int, int, int, gx_color_index));

int x_fill_trapezoid(P8(gx_device *, int, int, int, int, int, int, gx_color_index));

int x_tile_trapezoid(P10(gx_device *, gx_bitmap *, int, int, int, int, int, int, gx_color_index, gx_color_index));

/* The device descriptor */
private gx_device_procs x_procs = {
	x_open,
	gx_default_get_initial_matrix,
	x_sync,
	gx_default_output_page,
	x_close,
	x_map_rgb_color,
	x_map_color_rgb,
	x_fill_rectangle,
	x_tile_rectangle,
	x_copy_mono,
	x_copy_color,
	x_draw_line,
	x_fill_trapezoid,
	x_tile_trapezoid
};

/* Define default window parameters */

#define PROGRAM_NAME "Ghostscript"

#define ARG_BORDER_WIDTH "borderWidth"
#define DEFAULT_BORDER_WIDTH 1

#define ARG_BORDER_COLOR "borderColor"
#define DEFAULT_BORDER_COLOR  (xdev->colors[0]) /* That's black. */

#define ARG_GEOMETRY "geometry"
#define DEFAULT_GEOMETRY "612x792+0+0"

#define DEFAULT_X_POSITION 0
#define DEFAULT_Y_POSITION 0

#define ARG_X_RESOLUTION "xResolution"
#define DEFAULT_X_RESOLUTION 72
#define ARG_Y_RESOLUTION "yResolution"
#define DEFAULT_Y_RESOLUTION 72

/* The page width and height are defined in a slightly peculiar */
/* way so that <resolution> * DEFAULT_{WIDTH|HEIGHT}_INCHES will work. */
#define DEFAULT_WIDTH_INCHES 85/10	/* no parentheses! */
#define DEFAULT_HEIGHT_INCHES 11

/* Define a rectangle structure for update bookkeeping */
typedef struct rect_s {
  int xo, yo, xe, ye;
} rect;

/* Define the X Windows device */
typedef struct gx_device_X_s {
	gx_device_common;

	/* An XImage object for writing bitmap images to the screen */
	XImage image;

	/* Global X state */
	Display *dpy;
	Screen *scr;
	int depth;
	Visual *vis;
	Colormap cmap;
	Window win;
	GC gc;

	/* A backing pixmap so X will handle exposure automatically */
	Pixmap bpixmap;			/* 0 if use_backing is false, */
					/* or if it can't be allocated */
	rect update;		/* region needing updating */
	long up_area;		/* total area of update */
				/* (always 0 if no backing pixmap) */
	int up_count;		/* # of updates since flush */
	Pixmap dest;		/* bpixmap if non-0, else win */
	unsigned long accum_color; /* 'or' of all device colors used so far */

	/* An intermediate pixmap for the stencil case of copy_mono */
	struct {
	  Pixmap pixmap;
	  GC gc;
	  int raster, height;
	} cp;

	/* Structure for dealing with the halftone tile. */
	/* Later this might become a multi-element cache. */
	struct {
	  Pixmap pixmap;
	  Pixmap no_pixmap;	/* kludge to get around X bug */
	  int width, height, raster;
	  _byte *bits;
	  int bits_size;
	  gx_color_index fore_c, back_c;
	} ht;

	/* Cache the fill style from the GC */
	int fill_style;

#define set_fill_style(style)\
  if ( xdev->fill_style != style )\
    XSetFillStyle(xdev->dpy, xdev->gc, (xdev->fill_style = style))
#define set_function(func)\
  XSetFunction(xdev->dpy, xdev->gc, func) /* don't bother to cache */

	/* Map color indices to X pixel values */
	unsigned long colors[8];
#define black colors[0]
#define white colors[7]
	gx_color_index back_color, fore_color;

#define set_back_color(color)\
  if ( xdev->back_color != color )\
   { unsigned long dev_color = xdev->colors[xdev->back_color = color];\
     xdev->accum_color |= dev_color;\
     XSetBackground(xdev->dpy, xdev->gc, dev_color);\
   }
#define set_fore_color(color)\
  if ( xdev->fore_color != color )\
   { unsigned long dev_color = xdev->colors[xdev->fore_color = color];\
     xdev->accum_color |= dev_color;\
     XSetForeground(xdev->dpy, xdev->gc, dev_color);\
   }

} gx_device_X;

/* The instance is public. */
gx_device_X gs_x11_device = {
	sizeof(gx_device_X),
	&x_procs,
	"x11",
	DEFAULT_WIDTH_INCHES, DEFAULT_HEIGHT_INCHES,	/* x and y extent */
	DEFAULT_X_RESOLUTION, DEFAULT_Y_RESOLUTION,	/* x and y density */
		/* Following parameters are initialized for monochrome */
	0,			/* has color */
	1,			/* max r-g-b value */
	1,			/* bits per color pixel */
		/* End of monochrome/color parameters */
	0,			/* connection not initialized */
	{ /* image */
	  0, 0,			/* width, height */
	  0, XYBitmap, NULL,	/* xoffset, format, data */
	  LSBFirst, 8,    	/* byte-order, bitmap-unit */
	  MSBFirst, 8, 1,	/* bitmap-bit-order, bitmap-pad, depth */
	  0, 1,			/* bytes_per_line, bits_per_pixel */
	  0, 0, 0,		/* red_mask, green_mask, blue_mask */
	  NULL,			/* *obdata */
	   { NULL,			/* *(*create_image)() */
	     NULL,			/* (*destroy_image)() */
	     NULL,			/* (*get_pixel)() */
	     NULL,			/* (*put_pixel)() */
	     NULL,			/* *(*sub_image)() */
	     NULL			/* (*add_pixel)() */
	   },
	},
	NULL, NULL, 0, NULL,	/* dpy, scr, depth_all_ones, vis */
				/* (connection not initialized) */
	(Colormap)None,		/* cmap */
	(Window)None,		/* win */
	NULL,			/* gc */
	(Pixmap)0,		/* bpixmap */
	 { 0, 0, 0, 0 }, 0, 0,	/* update, up_area, up_count */
	(Pixmap)0,		/* dest */
	0,			/* accum_color */
	 { /* cp */
	   (Pixmap)0,		/* pixmap */
	   NULL,		/* gc */
	   -1, -1		/* raster, height */
	 },
	 { /* ht */
	   (Pixmap)None,		/* pixmap */
	   (Pixmap)None,		/* no_pixmap */
	   0, 0, 0,			/* width, height, raster */
	   NULL, 1 			/* bits, bits_size */
	 },
	FillSolid,		/* fill_style */
	{ 0, 0, 0, 0, 0, 0, 0, 0 }, /* colors[8] */
	0, 0			/* back_color, fore_color */

};
/* Macro for casting gx_device argument */
#define xdev ((gx_device_X *)dev)

/* Macro to validate and coerce arguments */
#define check_rect()\
	if ( x + w > xdev->width ) w = xdev->width - x;\
	if ( y + h > xdev->height ) h = xdev->height - y;\
	if ( w <= 0 || h <= 0 ) return 0;\
	if ( x < 0 || y < 0 ) return -1

#if !use_XPutImage
/* XPutImage doesn't work, do it ourselves. */
#  undef XPutImage
private void alt_put_image();
#  define XPutImage(dpy,win,gc,im,sx,sy,x,y,w,h)\
    alt_put_image(dev,dpy,win,gc,im,sx,sy,x,y,w,h)
#endif


/* Open the X device */
int
x_open(register gx_device *dev)
{	Screen *scr;
	XSizeHints sizehints;
	int border_width;
	char *x_resolution_str, *y_resolution_str;
	char *border_width_str, *border_color_str;
	unsigned long border_color;
	char *geometry;
	XColor screen_color, exact_color;
	XSetWindowAttributes xswa;
	XEvent event;
#ifdef DEBUG
if ( gs_debug['X'] )
	{ extern int _Xdebug;
	  _Xdebug = 1;
	}
#endif
	if ( !(xdev->dpy = XOpenDisplay(NULL)) )
	  { eprintf1("gs: Cannot open X display `%s'.\n",
		     getenv("DISPLAY"));
	    exit(1);
	  }
	scr = DefaultScreenOfDisplay(xdev->dpy);
	xdev->scr = scr;
	xdev->depth = DefaultDepthOfScreen(scr);
	xdev->vis = DefaultVisualOfScreen(scr);
	xdev->cmap = DefaultColormapOfScreen(scr);
	xdev->black = BlackPixelOfScreen(scr);
	xdev->white = WhitePixelOfScreen(scr);

	/* Figure out the resolution of our screen; 25.4 is the
	 * number of millimeters in an inch.  The only reason for
	 * allowing the user to specify the resolution is that
	 * X servers commonly lie about it (and about the screen size).
	 */

	x_resolution_str = XGetDefault(xdev->dpy, PROGRAM_NAME,
				       ARG_X_RESOLUTION);
	y_resolution_str = XGetDefault(xdev->dpy, PROGRAM_NAME,
				       ARG_Y_RESOLUTION);
	xdev->x_pixels_per_inch =
	  (x_resolution_str == NULL ?
	   25.4 * WidthOfScreen(scr) / WidthMMOfScreen(scr) :
	   atoi(x_resolution_str));
	xdev->y_pixels_per_inch =
	  (y_resolution_str == NULL ?
	   25.4 * HeightOfScreen(scr) / HeightMMOfScreen(scr) :
	   atoi(y_resolution_str));

	/* Figure out monochrome vs. color */
	switch ( xdev->vis->class )
	  {
	  case StaticGray:
	  case GrayScale:
	    xdev->has_color = 0;
	    { int i;
	      for ( i = 1; i < 7; i++ ) xdev->colors[i] = xdev->white;
	    }
	    break;
	  default:		/* color */
	    xdev->has_color = 1;
	    /* Just do primary colors for now */
	    { XColor xc;
	      int i;
	      for ( i = 1; i < 7; i++ )
		{ xc.red = (i & 4 ? ~(_ushort)0 : 0);
		  xc.green = (i & 2 ? ~(_ushort)0 : 0);
		  xc.blue = (i & 1 ? ~(_ushort)0 : 0);
		  XAllocColor(xdev->dpy, xdev->cmap, &xc);
		  xdev->colors[i] = xc.pixel;
		}
	    }
	  }
	xdev->ht.pixmap = (Pixmap)0;
	xdev->ht.bits = 0;
	xdev->fill_style = FillSolid;

	/* Get defaults from the database. */
	border_width_str = XGetDefault(xdev->dpy, PROGRAM_NAME,
				       ARG_BORDER_WIDTH);

	border_width = (border_width_str == NULL ? DEFAULT_BORDER_WIDTH :
			atoi(border_width_str));

	border_color_str = XGetDefault(xdev->dpy, PROGRAM_NAME,
				       ARG_BORDER_COLOR);

	border_color = (border_color_str == NULL ||
			 !XAllocNamedColor(xdev->dpy, xdev->cmap, 
					   border_color_str, 
					   &screen_color, &exact_color) ?
			DEFAULT_BORDER_COLOR :
			screen_color.pixel);

	sizehints.x = DEFAULT_X_POSITION;
	sizehints.y = DEFAULT_Y_POSITION;
	sizehints.width = xdev->x_pixels_per_inch * DEFAULT_WIDTH_INCHES;
	sizehints.height = xdev->y_pixels_per_inch * DEFAULT_HEIGHT_INCHES;
	sizehints.flags = 0;

	geometry = XGetDefault(xdev->dpy, PROGRAM_NAME, ARG_GEOMETRY);

	if (geometry != NULL)
	   {	/*
		 * Note that border_width must be set first.  We can't use
		 * scr, because that is a Screen*, and XGeometry wants
		 * the screen number.
		 */
		int bitmask = XGeometry(xdev->dpy, DefaultScreen(xdev->dpy),
					geometry, DEFAULT_GEOMETRY,
					border_width,
					1, 1, /* ``Font'' width and height. */
					0, 0, /* Interior padding. */
					&sizehints.x, &sizehints.y,
					&sizehints.width, &sizehints.height);

		if (bitmask & (XValue | YValue))
			sizehints.flags |= USPosition;

		if (bitmask & (WidthValue | HeightValue))
			sizehints.flags |= USSize;
	   }

	xdev->width = sizehints.width;
	xdev->height = sizehints.height;

	xswa.event_mask = ExposureMask;
	xswa.background_pixel = xdev->black;
	xswa.border_pixel = border_color;
	xdev->win = XCreateWindow(xdev->dpy, RootWindowOfScreen(scr),
				  sizehints.x, sizehints.y, /* upper left */
				  sizehints.width, sizehints.height,
				  border_width,
				  xdev->depth,
				  InputOutput, /* class */
				  xdev->vis, /* visual */
				  CWEventMask | CWBackPixel | CWBorderPixel,
				  &xswa);
	if ( use_backing )
	  xdev->bpixmap =
		XCreatePixmap(xdev->dpy, xdev->win,
			      xdev->width, xdev->height,
			      xdev->depth);
	else
	  xdev->bpixmap = (Pixmap)0;
	xdev->dest = (xdev->bpixmap != (Pixmap)0 ?
		      xdev->bpixmap : (Pixmap)xdev->win);
	xdev->accum_color = 0;
	update_init(dev);

	XChangeProperty(xdev->dpy, xdev->win, XA_WM_NAME, XA_STRING, 8,
			PropModeReplace, (unsigned char *)PROGRAM_NAME,
			strlen(PROGRAM_NAME));
	XSetNormalHints(xdev->dpy, xdev->win, &sizehints);

	/* Set up a graphics context */
	xdev->gc = XCreateGC(xdev->dpy, xdev->win, 0, NULL);
	XSetFunction(xdev->dpy, xdev->gc, GXcopy);
	XSetLineAttributes(xdev->dpy, xdev->gc, 0,
			   LineSolid, CapButt, JoinMiter);

	/* Clear the background pixmap to avoid initializing with garbage. */
	if ( xdev->bpixmap != (Pixmap)0 )
	  { XSetWindowBackgroundPixmap(xdev->dpy, xdev->win, xdev->bpixmap);
	    XSetForeground(xdev->dpy, xdev->gc, xdev->white);
	    XFillRectangle(xdev->dpy, xdev->bpixmap, xdev->gc,
			   0, 0, xdev->width, xdev->height);
	  }

	/* Initialize foreground and background colors */
	xdev->back_color = 7;
	XSetBackground(xdev->dpy, xdev->gc, xdev->white);
	xdev->fore_color = 0;
	XSetForeground(xdev->dpy, xdev->gc, xdev->black);

	/* Make the window appear. */
	XMapWindow(xdev->dpy, xdev->win);

	/* Before anything else, do a flush and wait for */
	/* an exposure event. */
	XFlush(xdev->dpy);
	XNextEvent(xdev->dpy, &event);

	xdev->ht.no_pixmap = XCreatePixmap(xdev->dpy, xdev->win, 1, 1,
					   xdev->depth);

	XSync(xdev->dpy, 0);
	return 0;
}

/* Close the device.  NOT SURE WHAT TO DO HERE YET. */
int
x_close(gx_device *dev)
{	return 0;
}

/* Map a color.  The "device colors" are just r,g,b packed together. */
gx_color_index
x_map_rgb_color(register gx_device *dev, _ushort r, _ushort g, _ushort b)
{	return (r << 2) + (g << 1) + b;
}


/* Map a "device color" back to r-g-b. */
int
x_map_color_rgb(register gx_device *dev, gx_color_index color, _ushort *prgb)
{	prgb[0] = (color >> 2) & 1;
	prgb[1] = (color >> 1) & 1;
	prgb[2] = color & 1;
	return 0;
}

/* Synchronize the display with the commands already given */
int
x_sync(register gx_device *dev)
{	update_flush(dev);
	XSync(xdev->dpy, 0);
	return 0;
}

/* Fill a rectangle with a color. */
int
x_fill_rectangle(register gx_device *dev,
  int x, int y, int w, int h, gx_color_index color)
{	check_rect();
	set_fill_style(FillSolid);
	set_fore_color(color);
	set_function(GXcopy);
	XFillRectangle(xdev->dpy, xdev->dest, xdev->gc, x, y, w, h);
	if ( xdev->bpixmap != (Pixmap)0 )
	 { update_add(dev, x, y, w, h);
	 }
#ifdef DEBUG
if ( gs_debug['F'] )
	printf("[F] fill (%d,%d):(%d,%d) %ld\n",
	       x, y, w, h, (long)color);
#endif
	return 0;
}

/* Tile a rectangle. */
int
x_tile_rectangle(register gx_device *dev, gx_bitmap *tile,
  int x, int y, int w, int h, gx_color_index zero, gx_color_index one)
{	check_rect();

	/* Check for a colored tile.  We should implement this */
	/* properly someday, since X can handle it. */

	if ( one == gx_no_color_index && zero == gx_no_color_index )
		return -1;

	/* 
	 * Remember, an X tile is already filled with particular
	 * pixel values (i.e., colors).  Therefore if we are changing
	 * fore/background color, we must invalidate the tile (using
	 * the same technique as in set_tile).  This problem only
	 * bites when using grayscale -- you may want to change
	 * fg/bg but use the same halftone screen.
	 */
	if ( (zero != xdev->ht.back_c) || (one != xdev->ht.fore_c) )
	  if ( xdev->ht.bits ) *xdev->ht.bits = ~*tile->data;

	set_back_color(zero);
	set_fore_color(one);
	set_function(GXcopy);
	if ( !set_tile(dev, tile) )
	 { /* Bad news.  Fall back to the default algorithm. */
	   set_fill_style(FillSolid);
	   return gx_default_tile_rectangle(dev, tile, x, y, w, h, zero, one);
	 }
	else
	  { /* Use the tile to fill the rectangle */
	    XFillRectangle(xdev->dpy, xdev->dest, xdev->gc, x, y, w, h);
	    if ( xdev->bpixmap != (Pixmap)0 )
	     { update_add(dev, x, y, w, h);
	     }
	  }
#ifdef DEBUG
if ( gs_debug['F'] )
	printf("[F] tile (%d,%d):(%d,%d) %ld,%ld\n",
	       x, y, w, h, (long)zero, (long)one);
#endif
	return 0;
}

/* Set up with a specified tile. */
/* Return false if we can't do it for some reason. */
private int
set_tile(register gx_device *dev, register gx_bitmap *tile)
{
#ifdef DEBUG
if ( gs_debug['T'] )
	return 0;
#endif
	/* Set up the tile Pixmap */
	if ( tile->width != xdev->ht.width ||
	     tile->height != xdev->ht.height ||
	     xdev->ht.pixmap == (Pixmap)0
	   )
	  { if ( xdev->ht.pixmap != (Pixmap)0 )
	      XFreePixmap(xdev->dpy, xdev->ht.pixmap);
	    if ( xdev->ht.bits )
	      { free(xdev->ht.bits);
		xdev->ht.bits = 0;
	      }
	    xdev->ht.pixmap = XCreatePixmap(xdev->dpy, xdev->win,
					    tile->width, tile->height,
					    xdev->depth);
	    if ( xdev->ht.pixmap == (Pixmap)0 )
	      return 0;
	    xdev->ht.bits_size = tile->raster * tile->height;
	    xdev->ht.bits = (_byte *)malloc(xdev->ht.bits_size);
	    if ( xdev->ht.bits == 0 )
	      { XFreePixmap(xdev->dpy, xdev->ht.pixmap);
		xdev->ht.pixmap = (Pixmap)0;
		return 0;
	      }
	    xdev->ht.width = tile->width, xdev->ht.height = tile->height;
	    xdev->ht.raster = tile->raster;
	    *xdev->ht.bits = ~*tile->data; /* force copying */
	  }
	xdev->ht.fore_c = xdev->fore_color;
	xdev->ht.back_c = xdev->back_color;
	/* Copy the tile into the Pixmap if needed */
	if ( memcmp(xdev->ht.bits, tile->data, xdev->ht.bits_size) )
	  { memcpy(xdev->ht.bits, tile->data, xdev->ht.bits_size);
	    xdev->image.data = (char *)tile->data;
	    xdev->image.width = tile->width;
	    xdev->image.height = tile->height;
	    xdev->image.bytes_per_line = tile->raster;
	    xdev->image.format = XYBitmap;
	    set_fill_style(FillSolid);
#ifdef DEBUG
if ( gs_debug['H'] )
	    { int i;
	      printf("[H] 0x%x: width=%d height=%d raster=%d\n",
		     tile->data, tile->width, tile->height, tile->raster);
	      for ( i = 0; i < tile->raster * tile->height; i++ )
		printf(" %02x", tile->data[i]);
	      printf("\n");
	    }
#endif
	    XSetTile(xdev->dpy, xdev->gc, xdev->ht.no_pixmap); /* *** X bug *** */
	    XPutImage(xdev->dpy, xdev->ht.pixmap, xdev->gc, &xdev->image,
		      0, 0, 0, 0, tile->width, tile->height);
	    XSetTile(xdev->dpy, xdev->gc, xdev->ht.pixmap);
	  }
	set_fill_style(FillTiled);
	return use_XSetTile;
}

/* Copy a monochrome bitmap. */
int
x_copy_mono(register gx_device *dev, _byte *base, int sourcex, int raster,
  int x, int y, int w, int h, gx_color_index zero, gx_color_index one)
/*
 * X doesn't readily support the simple operation of writing a color
 * through a mask.  The plot is the following: 
 *  If neither color is gx_no_color_index ("transparent"),
 *	use XPutImage with the "copy" function as usual.
 *  If the color is 0 or bitwise-includes every color written to date
 *    (a special optimization for writing black/white on color displays),
 *	use XPutImage with an appropriate Boolean function.
 *  Otherwise, do the following complicated stuff:
 *	Create pixmap of depth 1 if necessary.
 *	If foreground color is "transparent" then
 *	  invert the raster data (using GXcopyInverted in XPutImage).
 *	Use XPutImage to copy the raster image to the newly
 *	  created Pixmap.
 *	Install the Pixmap as the clip_mask in the X GC and
 *	  tweak the clip origin.
 *	Do an XFillRectangle, fill style=solid, specifying a
 *	  rectangle the same size as the original raster data.
 *	De-install the clip_mask.
 */
{	int function = GXcopy;
	unsigned long
	  bc = xdev->colors[zero],
	  fc = xdev->colors[one];
	
	check_rect();

	xdev->image.width = raster << 3;
	xdev->image.height = h;
	xdev->image.data = (char *)base;
	xdev->image.bytes_per_line = raster;
	set_fill_style(FillSolid);

	/* Check for null, easy 1-color, hard 1-color, and 2-color cases. */
	if ( zero != gx_no_color_index )
	  { if ( one != gx_no_color_index )
	      { /* Simply replace existing bits with what's in the image */
	      }
	    else if ( bc == 0 )
	      function = GXand,
	      fc = xdev->accum_color;
	    else if ( !(~bc & xdev->accum_color) )
	      function = GXorInverted,
	      fc = bc, bc = 0;
	    else
	      goto hard;
	  }
	else
	  { if ( one == gx_no_color_index ) /* no-op */
	      return 0;
	    else if ( fc == 0 )
	      function = GXandInverted,
	      bc = 0, fc = xdev->accum_color;
	    else if ( !(~fc & xdev->accum_color) )
	      function = GXor,
	      bc = 0;
	    else
	      goto hard;
	  }
	xdev->image.format = XYBitmap;
	set_function(function);
	XSetBackground(xdev->dpy, xdev->gc, bc);
	XSetForeground(xdev->dpy, xdev->gc, fc);
	xdev->back_color = xdev->fore_color = gx_no_color_index;
	xdev->accum_color |= fc | bc;
	XPutImage(xdev->dpy, xdev->dest, xdev->gc, &xdev->image,
		  sourcex, 0, x, y, w, h);

	goto out;

hard:	/* Handle the hard 1-color case. */
	if ( raster > xdev->cp.raster || h > xdev->cp.height )
	  { /* Must allocate a new pixmap and GC. */
	    /* Release the old ones first. */
	    free_cp(dev);

	    /* Create the clipping pixmap, depth must be 1. */
	    xdev->cp.pixmap =
	      XCreatePixmap(xdev->dpy, xdev->win, raster << 3, h, 1);
	    if ( xdev->cp.pixmap == (Pixmap)0 )
	      {	dprintf("x_copy_mono: can't allocate pixmap\n");
		exit(1);
	      }
	    xdev->cp.gc = XCreateGC(xdev->dpy, xdev->cp.pixmap, 0, 0);
	    if ( xdev->cp.gc == (GC)0 )
	      {	dprintf("x_copy_mono: can't allocate GC\n");
		exit(1);
	      }
	    xdev->cp.raster = raster;
	    xdev->cp.height = h;
	  }

	/* Initialize static mask image params */
	xdev->image.format = ZPixmap;

	/* Select function based on fg/bg transparency. */
	if ( one == gx_no_color_index )
	  { XSetFunction(xdev->dpy, xdev->cp.gc, GXcopyInverted);
	    XPutImage(xdev->dpy, xdev->cp.pixmap, xdev->cp.gc,
		      &xdev->image, sourcex, 0, 0, 0, w, h);
	    /* Reset function in stencil gc */
	    XSetFunction(xdev->dpy, xdev->cp.gc, GXcopy);
	    set_fore_color(zero);
	  }
	else
	  { XPutImage(xdev->dpy, xdev->cp.pixmap, xdev->cp.gc,
		      &xdev->image, sourcex, 0, 0, 0, w, h);
	    set_fore_color(one);
	  }

	/* Install as clipmask. */
	XSetClipMask(xdev->dpy, xdev->gc, xdev->cp.pixmap);
	XSetClipOrigin(xdev->dpy, xdev->gc, x, y);

	/*
	 * Draw a solid rectangle through the raster clip mask.
	 * Note fill style is guaranteed to be solid from above.
	 */
	XFillRectangle(xdev->dpy, xdev->dest, xdev->gc, x, y, w, h);

	/* Tidy up.  Free the pixmap if it's big. */
	XSetClipMask(xdev->dpy, xdev->gc, None);
	if ( raster * h > max_temp_pixmap )
	  free_cp(dev);

out:	if ( xdev->bpixmap != (Pixmap)0 )
	  { /* We wrote to the pixmap, so update the display now. */
	    update_add(dev, x, y, w, h);
	  }

	return 0;
}

/* Internal routine to free the GC and pixmap used for copying. */
private void
free_cp(register gx_device *dev)
{	if ( xdev->cp.gc != NULL )
	   {	XFreeGC(xdev->dpy, xdev->cp.gc);
		xdev->cp.gc = NULL;
	   }
	if ( xdev->cp.pixmap != (Pixmap)0 )
	   {	XFreePixmap(xdev->dpy, xdev->cp.pixmap);
		xdev->cp.pixmap = (Pixmap)0;
	   }
	xdev->cp.raster = -1;	/* mark as unallocated */
}

/* Copy a "color" bitmap.  Since "color" is the same as monochrome, */
/* this just reduces to copying a monochrome bitmap. */
/****** THIS ROUTINE IS COMPLETELY WRONG, SINCE WE DO SUPPORT COLOR. ******/
/* Fortunately, no one uses it at the moment. */
int
x_copy_color(register gx_device *dev, _byte *base, int sourcex, int raster,
  int x, int y, int w, int h)
{	return x_copy_mono(dev, base, sourcex, raster, x, y, w, h, (gx_color_index)0, (gx_color_index)7);
}

/* Draw a line */
int
x_draw_line(register gx_device *dev,
  int x0, int y0, int x1, int y1, gx_color_index color)
{	set_fore_color(color);
	set_fill_style(FillSolid);
	set_function(GXcopy);
	XDrawLine(xdev->dpy, xdev->dest, xdev->gc, x0, y0, x1, y1);
	if ( xdev->bpixmap != (Pixmap)0 )
	 { int x = x0, y = y0, w = x1 - x0, h = y1 - y0;
	   if ( w < 0 ) x = x1, w = - w;
	   if ( h < 0 ) y = y1, h = - h;
	   update_add(dev, x, y, w+1, h+1);
	 }
	return 0;
}

/* Define a common macro for setting up a trapezoid. */
#define setup_trapezoid()\
	XPoint vlist[4];\
	XPoint *pv = vlist + 1;\
	vlist[0].x = x0, vlist[0].y = y0;\
	if ( w0 )\
	  pv->x = x0 + w0, pv->y = y0, pv++;\
	pv->x = x1 + w1, pv->y = y1, pv++;\
	if ( w1 )\
	  pv->x = x1, pv->y = y1, pv++
#define vcount (pv - vlist)
/* Define a common macro for finishing a trapezoid. */
#define finish_trapezoid()\
	set_function(GXcopy);\
	XFillPolygon(xdev->dpy, xdev->dest, xdev->gc,\
		     vlist, vcount, Convex, CoordModeOrigin);\
	if ( xdev->bpixmap != (Pixmap)0 )\
	 { int xl = x0, xr = x0 + w0;\
	   if ( x1 < xl ) xl = x1;\
	   if ( x1 + w1 > xr ) xr = x1 + w1;\
	   update_add(dev, xl, y0, xr - xl, y1 - y0);\
	 }

/* Fill a trapezoid */
int
x_fill_trapezoid(register gx_device *dev,
  int x0, int y0, int w0, int x1, int y1, int w1, gx_color_index color)
{	setup_trapezoid();
	set_fore_color(color);
	set_fill_style(FillSolid);
	finish_trapezoid();
	return 0;
}

/* Tile a trapezoid */
int
x_tile_trapezoid(register gx_device *dev, gx_bitmap *tile,
  int x0, int y0, int w0, int x1, int y1, int w1,
  gx_color_index zero, gx_color_index one)
{
	/* Check for a colored tile.  We should implement this */
	/* properly someday, since X can handle it. */

	if ( one == gx_no_color_index && zero == gx_no_color_index )
	  return -1;
	/* See x_tile_rectangle for the significance of the following. */
	if ( (zero != xdev->ht.back_c) || (one != xdev->ht.fore_c) )
	  if ( xdev->ht.bits ) *xdev->ht.bits = ~*tile->data;

	{ setup_trapezoid();
	  set_back_color(zero);
	  set_fore_color(one);
	  if ( !set_tile(dev, tile) )
	   { set_fill_style(FillSolid);
	     return gx_default_tile_trapezoid(dev, tile,
	       x0, y0, w0, x1, y1, w1, zero, one);
	   }
	  /* Use the tile to fill the trapezoid */
	  finish_trapezoid();
	 }
	return 0;
}

/* ------ Screen update procedures ------ */

/* Flush updates to the screen if needed. */
private void
update_do_flush(register gx_device *dev)
{	int xo = xdev->update.xo, yo = xdev->update.yo;
	set_function(GXcopy);
	XCopyArea(xdev->dpy, xdev->bpixmap, xdev->win, xdev->gc,
		  xo, yo, xdev->update.xe - xo, xdev->update.ye - yo,
		  xo, yo);
	update_init(dev);
}

/* Add a region to be updated. */
/* This is only called if xdev->bpixmap != 0. */
private void
update_add(register gx_device *dev, int xo, int yo, int w, int h)
{	int xe = xo + w, ye = yo + h;
	long new_area = (long)w * h;
	++xdev->up_count;
	if ( xdev->up_area != 0 )
	  { /* See whether adding this rectangle */
	    /* would result in too much being copied unnecessarily. */
	    long old_area = xdev->up_area;
	    long new_up_area;
	    rect u;
	    u.xo = min(xo, xdev->update.xo);
	    u.yo = min(yo, xdev->update.yo);
	    u.xe = max(xe, xdev->update.xe);
	    u.ye = max(ye, xdev->update.ye);
	    new_up_area = (long)(u.xe - u.xo) * (u.ye - u.yo);
	    if ( new_up_area > 100 &&
		old_area + new_area < new_up_area * 2 / 3 ||
		xdev->up_count >= 200
		)
	      update_do_flush(dev);
	    else
	      { xdev->update = u;
		xdev->up_area = new_up_area;
		return;
	      }
	  }
	xdev->update.xo = xo;
	xdev->update.yo = yo;
	xdev->update.xe = xe;
	xdev->update.ye = ye;
	xdev->up_area = new_area;
}

/* ------ Internal procedures ------ */

/* Substitute for XPutImage using XFillRectangle. */
/* This is a total hack to get around an apparent bug */
/* in the X server.  It only works with the specific */
/* parameters (bit/byte order, padding) used above. */
private void
alt_put_image(gx_device *dev, Display *dpy, Drawable win, GC gc,
  XImage *pi, int sx, int sy, int dx, int dy, unsigned w, unsigned h)
{	int raster = pi->bytes_per_line;
	_byte *data = (_byte *)pi->data + sy * raster + (sx >> 3);
	int init_mask = 0x80 >> (sx & 7);
	int invert;
	int yi;
#define nrects 40
	XRectangle rects[nrects];
	XRectangle *rp = rects;
	if ( xdev->fore_color != gx_no_color_index )
	  { if ( xdev->back_color != gx_no_color_index )
	      { XSetForeground(dpy, gc, xdev->colors[xdev->back_color]);
		XFillRectangle(dpy, win, gc, dx, dy, w, h);
	      }
	    XSetForeground(dpy, gc, xdev->colors[xdev->fore_color]);
	    invert = 0;
	  }
	else if ( xdev->back_color != gx_no_color_index )
	  { XSetForeground(dpy, gc, xdev->colors[xdev->back_color]);
	    invert = 0xff;
	  }
	else
	  return;
	for ( yi = 0; yi < h; yi++, data += raster )
	  { register int mask = init_mask;
	    register _byte *dp = data;
	    register int xi = 0;
	    while ( xi < w )
	      { if ( (*dp ^ invert) & mask )
		  { int xleft = xi;
		    if ( rp == &rects[nrects] )
		      { XFillRectangles(dpy, win, gc, rects, nrects);
			rp = rects;
		      }
		    /* Scan over a run of 1-bits */
		    rp->x = dx + xi, rp->y = dy + yi;
		    do
		      { if ( !(mask >>= 1) ) mask = 0x80, dp++;
			xi++;
		      }
		    while ( xi < w && (*dp & mask) );
		    rp->width = xi - xleft, rp->height = 1;
		    rp++;
		  }
		else
		  { if ( !(mask >>= 1) ) mask = 0x80, dp++;
		    xi++;
		  }
	      }
	  }
	XFillRectangles(dpy, win, gc, rects, rp - rects);
}
