/* NSBitmapImageRep+JPEG.m

   Methods for reading jpeg images

   Copyright (C) 2003-2010 Free Software Foundation, Inc.
   
   Written by:  Stefan Kleine Stegemann <stefan@wms-network.de>
   Date: Nov 2003
   
   This file is part of the GNUstep GUI Library.

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

#import "config.h"
#import <Foundation/NSData.h>
#import <Foundation/NSDictionary.h>
#import <Foundation/NSException.h>
#import <Foundation/NSString.h>
#import <Foundation/NSValue.h>
#import "AppKit/NSGraphics.h"
#import "NSBitmapImageRep+JPEG.h"
#import "GSGuiPrivate.h"

#if HAVE_LIBJPEG

#include <jerror.h>
#if defined(__MINGW32__)
/* Hack so that INT32 is not redefined in jmorecfg.h. MingW defines this
   as well in basetsd.h */
#ifndef XMD_H
#define XMD_H
#endif
/* And another so that boolean is not redefined in jmorecfg.h. */
#ifndef HAVE_BOOLEAN
#define HAVE_BOOLEAN
/* This MUST match the jpeg definition of boolean */
typedef int jpeg_boolean;
#define boolean jpeg_boolean
#endif
#endif // __MINGW32__
#include <jpeglib.h>
#if defined(__CYGWIN__)
/* Cygwin uses a patched jpeg */
#define GSTEP_PROGRESSIVE_CODEC
#endif

#include <setjmp.h>


/* -----------------------------------------------------------
   The following functions are for interacting with the
   jpeg library 
   ----------------------------------------------------------- */

/* A custom error manager for the jpeg library
 * that 'inherits' from libjpeg's standard
 * error manager.  */
struct gs_jpeg_error_mgr
{
  struct jpeg_error_mgr parent;
   
  /* marks where to return after an error (instead of
     simply exiting) */
  jmp_buf  setjmpBuffer;

  /* a pointer to the last error message, nil if  no
     error occured. if present, string is autoreleased.  */
  NSString *error;
};
typedef struct gs_jpeg_error_mgr *gs_jpeg_error_mgr_ptr;

/* Print the last jpeg library error and returns
 * the control to the caller of the libary.
 * libjpegs default error handling would exit
 * after printing the error.  */
static void gs_jpeg_error_exit(j_common_ptr cinfo)
{
  gs_jpeg_error_mgr_ptr myerr = (gs_jpeg_error_mgr_ptr)cinfo->err;
  (*cinfo->err->output_message)(cinfo);
   
  /* jump back to the caller of the library */
  longjmp(myerr->setjmpBuffer, 1);
}


/* Save the error message in error.  */
static void gs_jpeg_output_message(j_common_ptr cinfo)
{
  char msgBuffer[JMSG_LENGTH_MAX];

  gs_jpeg_error_mgr_ptr myerr = (gs_jpeg_error_mgr_ptr)cinfo->err;

  (*cinfo->err->format_message)(cinfo, msgBuffer);
  myerr->error = [NSString stringWithCString: msgBuffer];
}


/* Initialize our error manager */
static void gs_jpeg_error_mgr_init(gs_jpeg_error_mgr_ptr errMgr)
{
  errMgr->error = nil;
}



/* ------------------------------------------------------------------*/

/* A custom data source manager that allows the jpeg library
 * to read it's data from memory. It 'inherits' from the
 * default jpeg source manager.  */
typedef struct
{
  struct jpeg_source_mgr parent;

  /* the data to be passed to the library functions */
  const unsigned char *data;
  unsigned int length;
} gs_jpeg_source_mgr;

typedef gs_jpeg_source_mgr *gs_jpeg_source_ptr;


static void gs_init_source(j_decompress_ptr cinfo)
{
  /* nothing to do here (so far) */
}


static boolean gs_fill_input_buffer(j_decompress_ptr cinfo)
{
  gs_jpeg_source_ptr src = (gs_jpeg_source_ptr)cinfo->src;

  /* we make all data available at once */
  if (src->length == 0)
    {
      ERREXIT(cinfo, JERR_INPUT_EMPTY);
    }

  src->parent.next_input_byte = src->data;
  src->parent.bytes_in_buffer = src->length;

  return TRUE;
}


static void gs_skip_input_data(j_decompress_ptr cinfo, long numBytes)
{
  gs_jpeg_source_ptr src = (gs_jpeg_source_ptr)cinfo->src;

  if (numBytes > 0)
    {
      src->parent.next_input_byte += numBytes;
      src->parent.bytes_in_buffer -= numBytes;
    }
}


static void gs_term_source(j_decompress_ptr cinfo)
{
  /* nothing to do here */
}


/* Prepare a decompression object for input from memory. The
 * caller is responsible for providing and releasing the
 * data. After decompression is done, gs_jpeg_memory_src_destroy
 * has to be called.  */
static void gs_jpeg_memory_src_create(j_decompress_ptr cinfo, NSData *data)
{
  gs_jpeg_source_ptr src;
  
  cinfo->src = (struct jpeg_source_mgr *)malloc(sizeof(gs_jpeg_source_mgr));

  src = (gs_jpeg_source_ptr) cinfo->src;
  src->parent.init_source = gs_init_source;
  src->parent.fill_input_buffer = gs_fill_input_buffer;
  src->parent.skip_input_data = gs_skip_input_data;
  src->parent.resync_to_restart = jpeg_resync_to_restart; /* use default */
  src->parent.term_source = gs_term_source;
  src->parent.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
  src->parent.next_input_byte = NULL; /* until buffer loaded */

  src->data = (const unsigned char *)[data bytes];
  src->length = [data length];
}


/* Destroy the source manager of the jpeg decompression object.  */
static void gs_jpeg_memory_src_destroy(j_decompress_ptr cinfo)
{
  gs_jpeg_source_ptr src = (gs_jpeg_source_ptr)cinfo->src;
   
  free(src); // does not free the data
  cinfo->src = NULL;
}

/* ------------------------------------------------------------------*/

/*
 * A custom destination manager.
 */

typedef struct
{
  struct jpeg_destination_mgr pub; // public fields
  unsigned char* buffer;
  unsigned char* data;
  NSData** finalData;
  int length;
} gs_jpeg_destination_mgr;
typedef gs_jpeg_destination_mgr * gs_jpeg_dest_ptr;

/*
        Initialize destination.  This is called by jpeg_start_compress()
        before any data is actually written.  It must initialize
        next_output_byte and free_in_buffer.  free_in_buffer must be
        initialized to a positive value.
*/

static void gs_init_destination (j_compress_ptr cinfo)
{
        //NSLog (@"gs_init_destination");
  gs_jpeg_dest_ptr dest = (gs_jpeg_dest_ptr) cinfo->dest;

  // allocate the output image

  int imageSize = cinfo->image_width * cinfo->image_height;

  dest->buffer = (void*) calloc ((imageSize * cinfo->input_components),
                                sizeof(unsigned char));

  dest->data = (void*) calloc ((imageSize * cinfo->input_components),
                                sizeof(unsigned char));

  dest->pub.next_output_byte = dest->buffer;
  dest->pub.free_in_buffer = imageSize * cinfo->input_components;
  dest->length = 0;
}

/*
        This is called whenever the buffer has filled (free_in_buffer
        reaches zero).  In typical applications, it should write out the
        *entire* buffer (use the saved start address and buffer length;
        ignore the current state of next_output_byte and free_in_buffer).
        Then reset the pointer & count to the start of the buffer, and
        return TRUE indicating that the buffer has been dumped.
        free_in_buffer must be set to a positive value when TRUE is
        returned.  A FALSE return should only be used when I/O suspension is
        desired (this operating mode is discussed in the next section).
*/

static boolean gs_empty_output_buffer (j_compress_ptr cinfo)
{

  //NSLog (@"gs_empty_output_buffer...");
  gs_jpeg_dest_ptr dest = (gs_jpeg_dest_ptr) cinfo->dest;
  //NSLog (@"length added (%d)", dest->length);
  int imageSize = cinfo->image_width * cinfo->image_height;
  int bufSize = imageSize * cinfo->input_components;
  int i;

  for (i = 0; i < bufSize; i++)
    {
      dest->data [dest->length + i] = dest->buffer [i];
    }

  dest->length = dest->length + bufSize;
  dest->pub.next_output_byte = dest->buffer;
  dest->pub.free_in_buffer = imageSize * cinfo->input_components;

  return TRUE;
}

/*
        Terminate destination --- called by jpeg_finish_compress() after all
        data has been written.  In most applications, this must flush any
        data remaining in the buffer.  Use either next_output_byte or
        free_in_buffer to determine how much data is in the buffer.
*/

static void gs_term_destination (j_compress_ptr cinfo)
{
  //NSLog (@"gs_term_destination");
  gs_jpeg_dest_ptr dest = (gs_jpeg_dest_ptr) cinfo->dest;
  int imageSize = cinfo->image_width * cinfo->image_height;
  int bufSize = imageSize * cinfo->input_components;
  int i;

  for (i = 0; i < bufSize; i++)
    {
      dest->data [dest->length + i] = dest->buffer [i];
    }
  dest->length = dest->length + bufSize;

  *dest->finalData = [[NSData alloc] initWithBytes: dest->data
    length: (dest->length) - dest->pub.free_in_buffer];
}

static void gs_jpeg_memory_dest_create (j_compress_ptr cinfo, NSData** data)
{
  gs_jpeg_dest_ptr dest;

  cinfo->dest = (struct jpeg_destination_mgr*)
    malloc (sizeof (gs_jpeg_destination_mgr));

  dest = (gs_jpeg_dest_ptr) cinfo->dest;

  dest->pub.init_destination = gs_init_destination;
  dest->pub.empty_output_buffer = gs_empty_output_buffer;
  dest->pub.term_destination = gs_term_destination;
  dest->finalData = data;
}

static void gs_jpeg_memory_dest_destroy (j_compress_ptr cinfo)
{
  gs_jpeg_dest_ptr dest = (gs_jpeg_dest_ptr) cinfo->dest;
  free (dest->buffer);
  free (dest->data);
  free (dest);
  cinfo->dest = NULL;
}


/* -----------------------------------------------------------
   The jpeg loading part of NSBitmapImageRep
   ----------------------------------------------------------- */

@implementation NSBitmapImageRep (JPEGReading)


/* Return YES if this looks like a JPEG. */
+ (BOOL) _bitmapIsJPEG: (NSData *)imageData
{
  struct jpeg_decompress_struct  cinfo;
  struct gs_jpeg_error_mgr  jerrMgr;

  memset((void*)&cinfo, 0, sizeof(struct jpeg_decompress_struct));

  /* Be sure imageData contains data */
  if (![imageData length])
    {
      return NO;
    }

  /* Establish the our custom error handler */
  gs_jpeg_error_mgr_init(&jerrMgr);
  cinfo.err = jpeg_std_error(&jerrMgr.parent);
  jerrMgr.parent.error_exit = gs_jpeg_error_exit;
  jerrMgr.parent.output_message = gs_jpeg_output_message;

  // establish return context for error handling
  if (setjmp(jerrMgr.setjmpBuffer))
    {
      gs_jpeg_memory_src_destroy(&cinfo);
      jpeg_destroy_decompress(&cinfo);
      return NO;
    }

  /* read the header to see if we have a jpeg */
  jpeg_create_decompress(&cinfo);

  /* Establish our own data source manager */
  gs_jpeg_memory_src_create(&cinfo, imageData);

  jpeg_read_header(&cinfo, TRUE);
  gs_jpeg_memory_src_destroy(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  return YES;
}


/* Read the jpeg image. Assume it is from a jpeg file and imageData
 * is not nil.
 */
- (id) _initBitmapFromJPEG: (NSData *)imageData
	      errorMessage: (NSString **)errorMsg
{
  struct jpeg_decompress_struct  cinfo;
  struct gs_jpeg_error_mgr  jerrMgr;
  JDIMENSION sclcount, samplesPerRow, i, j, rowSize;
  JSAMPARRAY sclbuffer = NULL;
  unsigned char *imgbuffer = NULL;
  BOOL isProgressive;

  if (!(self = [super init]))
    return nil;

  memset((void*)&cinfo, 0, sizeof(struct jpeg_decompress_struct));

  /* Establish the our custom error handler */
  gs_jpeg_error_mgr_init(&jerrMgr);
  cinfo.err = jpeg_std_error(&jerrMgr.parent);
  jerrMgr.parent.error_exit = gs_jpeg_error_exit;
  jerrMgr.parent.output_message = gs_jpeg_output_message;

  // establish return context for error handling
  if (setjmp(jerrMgr.setjmpBuffer))
    {
      /* assign the description of possible occured error to errorMsg */
      if (errorMsg)
	*errorMsg = (jerrMgr.error ? (id)jerrMgr.error : (id)nil);
      gs_jpeg_memory_src_destroy(&cinfo);
      jpeg_destroy_decompress(&cinfo);
      if (imgbuffer)
        {
          free(imgbuffer);
        }
      RELEASE(self);
      return nil;
    }

  /* jpeg-decompression */

  jpeg_create_decompress(&cinfo);

  /* Establish our own data source manager */
  gs_jpeg_memory_src_create(&cinfo, imageData);

  jpeg_read_header(&cinfo, TRUE);

  /* we use RGB as target color space; others are not yet supported */
  cinfo.out_color_space = JCS_RGB;

  /* decompress */
  jpeg_start_decompress(&cinfo);

  /* process the decompressed  data */
  samplesPerRow = cinfo.output_width * cinfo.output_components;
  rowSize = samplesPerRow * sizeof(unsigned char);
  NSAssert(sizeof(JSAMPLE) == sizeof(unsigned char),
    @"unexpected sample size");

  sclbuffer = cinfo.mem->alloc_sarray((j_common_ptr)&cinfo,
                                      JPOOL_IMAGE,
                                      samplesPerRow,
                                      cinfo.rec_outbuf_height);
  /* sclbuffer is freed when cinfo is destroyed */

  imgbuffer = NSZoneMalloc([self zone], cinfo.output_height * rowSize);
  if (!imgbuffer)
    {
      NSLog(@"NSBitmapImageRep+JPEG: failed to allocated image buffer");
      RELEASE(self);
      return nil;
    }

  i = 0;
  while (cinfo.output_scanline < cinfo.output_height)
    {
      sclcount
	= jpeg_read_scanlines(&cinfo, sclbuffer, cinfo.rec_outbuf_height);

      for (j = 0; j < sclcount; j++)
        {
	  // copy a row to the image buffer
	  memcpy((imgbuffer + (i * rowSize)),
	    *(sclbuffer + (j * rowSize)),
	    rowSize);
	  i++;
        }
    }

#ifdef GSTEP_PROGRESSIVE_CODEC
  isProgressive = (cinfo.process == JPROC_PROGRESSIVE);
#else
  isProgressive = cinfo.progressive_mode;
#endif

  /* done */
  jpeg_finish_decompress(&cinfo);

  gs_jpeg_memory_src_destroy(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  if (jerrMgr.parent.num_warnings)
    {
      NSLog(@"NSBitmapImageRep+JPEG: %ld warnings during jpeg decompression, "
        @"image may be corrupted", jerrMgr.parent.num_warnings);
    }

  // create the imagerep
  //BITS_IN_JSAMPLE is defined by libjpeg
  [self initWithBitmapDataPlanes: &imgbuffer
		      pixelsWide: cinfo.output_width
		      pixelsHigh: cinfo.output_height
		   bitsPerSample: BITS_IN_JSAMPLE
		 samplesPerPixel: cinfo.output_components
			hasAlpha: (cinfo.output_components == 3 ? NO : YES)
			isPlanar: NO
		  colorSpaceName: NSCalibratedRGBColorSpace
		     bytesPerRow: rowSize
		    bitsPerPixel: BITS_IN_JSAMPLE * cinfo.output_components];

  [self setProperty: NSImageProgressive
          withValue: [NSNumber numberWithBool: isProgressive]];

  _imageData = [[NSData alloc]
    initWithBytesNoCopy: imgbuffer
		 length: (rowSize * cinfo.output_height)];

  return self;
}


/* -----------------------------------------------------------
   The jpeg writing part of NSBitmapImageRep
   ----------------------------------------------------------- */

- (NSData*) _JPEGRepresentationWithProperties: (NSDictionary*) properties
                                 errorMessage: (NSString **)errorMsg
{
  NSData			*ret;
  unsigned char			*imageSource;
  int				sPP;
  int				width;
  int				height;
  int				row_stride;
  int				quality = 90;
  NSNumber			*qualityNumber = nil;
  NSNumber			*progressiveNumber = nil;
  NSString			*colorSpace = nil;
  BOOL                          isRGB;
  struct jpeg_compress_struct	cinfo;
  struct gs_jpeg_error_mgr      jerrMgr;
  JSAMPROW			row_pointer[1]; // pointer to a single row

  // TODO: handles planar images 

  if ([self isPlanar])
    {
      NSString * em = @"JPEG image rep: Planar Image, not handled yet !";
      if (errorMsg != NULL)
        *errorMsg = em;
      else
        NSLog (@"JPEG image rep: Planar Image, not handled yet !");
      return nil;
    }

  memset((void*)&cinfo, 0, sizeof(struct jpeg_compress_struct));

  imageSource = [self bitmapData];
  sPP = [self samplesPerPixel];
  width = [self size].width;
  height = [self size].height;
  row_stride = width * sPP;

  /* Establish the our custom error handler */
  gs_jpeg_error_mgr_init(&jerrMgr);
  cinfo.err = jpeg_std_error(&jerrMgr.parent);
  jerrMgr.parent.error_exit = gs_jpeg_error_exit;
  jerrMgr.parent.output_message = gs_jpeg_output_message;

  // establish return context for error handling
  if (setjmp(jerrMgr.setjmpBuffer))
    {
      /* assign the description of possible occured error to errorMsg */
      if (errorMsg)
	*errorMsg = (jerrMgr.error ? (id)jerrMgr.error : (id)nil);
      gs_jpeg_memory_dest_destroy(&cinfo);
      jpeg_destroy_compress(&cinfo);
      return nil;
    }

  // initialize libjpeg for compression

  jpeg_create_compress (&cinfo);

  // specify the destination for the compressed data.. 

  gs_jpeg_memory_dest_create (&cinfo, &ret);

  // set parameters

  colorSpace = [self colorSpaceName];
  isRGB = ([colorSpace isEqualToString: NSDeviceRGBColorSpace]
           || [colorSpace isEqualToString: NSCalibratedRGBColorSpace]);
  cinfo.image_width  = width;
  cinfo.image_height = height;
  // note we will strip alpha from RGBA
  cinfo.input_components = (isRGB && [self hasAlpha])? 3 : sPP;
  cinfo.in_color_space = JCS_UNKNOWN;
  if (isRGB) cinfo.in_color_space = JCS_RGB;
  if (sPP == 1) cinfo.in_color_space = JCS_GRAYSCALE;
  if ([colorSpace isEqualToString: NSDeviceCMYKColorSpace])
    cinfo.in_color_space = JCS_CMYK;
  if (cinfo.in_color_space == JCS_UNKNOWN)
    NSLog(@"JPEG image rep: Using unknown color space with unpredictable results");

  jpeg_set_defaults (&cinfo);

  // set quality
  
  qualityNumber = [properties objectForKey: NSImageCompressionFactor];
  if (qualityNumber != nil)
    {
      quality = (int) ((1-[qualityNumber floatValue] / 255.0) * 100.0);
    }

  // set progressive mode
  progressiveNumber = [properties objectForKey: NSImageProgressive];
  if (progressiveNumber != nil)
    {
#ifdef GSTEP_PROGRESSIVE_CODEC
      if ([progressiveNumber boolValue])
        cinfo.process = JPROC_PROGRESSIVE;
#else
      cinfo.progressive_mode = [progressiveNumber boolValue];
#endif
    }


  // compress the image

  jpeg_set_quality (&cinfo, quality, TRUE);
  jpeg_start_compress (&cinfo, TRUE);

  if (isRGB && [self hasAlpha])	// strip alpha channel before encoding
    {
      unsigned char * RGB, * pRGB, * pRGBA;
      unsigned int iRGB, iRGBA;
      RGB = malloc(sizeof(unsigned char)*3*width);
      while (cinfo.next_scanline < cinfo.image_height)
	{
	  iRGBA = cinfo.next_scanline * row_stride;
	  pRGBA = &imageSource[iRGBA];
	  pRGB = RGB;
	  for (iRGB = 0; iRGB < 3*width; iRGB += 3)
	    {
	      memcpy(pRGB, pRGBA, 3);
	      pRGB +=3;
	      pRGBA +=4;
	    }
	  row_pointer[0] = RGB;
	  jpeg_write_scanlines (&cinfo, row_pointer, 1);
	}
      free(RGB);
    }
  else	// no alpha channel
    {
      while (cinfo.next_scanline < cinfo.image_height)
	{
	  int	index = cinfo.next_scanline * row_stride;

	  row_pointer[0] = &imageSource[index];
	  jpeg_write_scanlines (&cinfo, row_pointer, 1);
	}
    }

  jpeg_finish_compress(&cinfo);

  gs_jpeg_memory_dest_destroy (&cinfo);

  jpeg_destroy_compress(&cinfo);

  return AUTORELEASE(ret);
}

@end

#else /* !HAVE_LIBJPEG */

@implementation NSBitmapImageRep (JPEGReading)
+ (BOOL) _bitmapIsJPEG: (NSData *)imageData
{
  return NO;
}

- (id) _initBitmapFromJPEG: (NSData *)imageData
	      errorMessage: (NSString **)errorMsg
{
  RELEASE(self);
  return nil;
}
- (NSData *) _JPEGRepresentationWithProperties: (NSDictionary *) properties
                                  errorMessage: (NSString **)errorMsg
{
  return nil;
}
@end

#endif /* !HAVE_LIBJPEG */

