/* plot, unix plot file to graphics device translators.
   Copyright (C) 1989 Free Software Foundation, Inc.

   plot is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY.  No author or distributor accepts responsibility to
   anyone for the consequences of using it or for whether it serves any
   particular purpose or works at all, unless he says so in writing.
   Refer to the GNU General Public License for full details.
   
   Everyone is granted permission to copy, modify and redistribute plot,
   but only under the conditions described in the GNU General Public
   License.  A copy of this license is supposed to have been given to you
   along with plot so you can know your rights and responsibilities.  It
   should be in a file named COPYING.  Among other things, the copyright
   notice and this notice must be preserved on all copies.  */


/* This file is the main routine for plot.

   It includes code to read the plot file and call plot functions
   to draw the graphics. */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#ifdef sequent
#include <strings.h>
#else
#include <string.h>
#endif
#include <math.h>
#include "libplot.h"

/*
extern int perror();
extern int exit();
extern int printf();
extern int fprintf();
extern int fclose();
#ifndef mips
extern char *sprintf();
#endif
#ifndef hpux
extern void *malloc();
extern void *realloc();
#endif
extern int fputs();
*/
extern void perror(const char *s);

/* #include "../COPYING" */

enum plot_instruction
{
  ALABEL =	'T',
  ARC =		'a',
  CIRCLE =	'c',
  COLOR =	'C',
  CONT =	'n',
  ERASE =	'e',
  FILL =	'L',
  FONT =	'F',
  FONTSIZE =	'S',
  LABEL =	't',
  LINE =	'l',
  LINEMOD =	'f',
  MOVE =	'm',
  POINT =	'p',
  ROTATE =	'r',
  SPACE =	's'
};

/* This flag specifies that the input contains unsigned (if zero) or
   signed (if nonzero) two byte intgers. The default is signed. */
int signed_input = 1;

/* read in two byte intgers with high_byte_first. The default is to read the low
   byte first. */
int high_byte_first = 0;

/* guess_byte_order is a flag which, if non-zero, indicated that the function
   find_byte_order should be used to guess the byte order for the input file. */
int guess_byte_order = 1;

/* swap_bytes returns the bottom two bytes of its integer argument with
   their bytes reversed. */

int swap_bytes(int x)
{
  unsigned char a, b;
  a = x & 0xff;
  b = (x >> 8) & 0xff;
  return ((a << 8)|b);
}

/* This is a set of known values for the maximum x (abscissa) values
   specified in the plot size command for known devices.  Using this
   set we can construct a heuristic proceedure for recognizing plot
   files in which the bytes of two byte integers are reversed.  We can
   recognize these files by looking for size commands containing these
   known sizes in byte reversed form. The last entry should be 0. */
   
int known_size[32] = {
  504,				/* plot3d output 504x504 */
  2048,				/* versatek plotter 2048x2048 */
  2100,				/* plot3d output */
  3120,				/* Tektronix 4010 terminal 3120x3120 */
  4096,				/* GSI 300 terminal 4096x4096 */
  0				/* the last entry should be 0 */
  };

/* find_byte_order takes four integer arguments and matches third one
   against a set of known values (sizes, see above). If there is a match
   it merely returns.  If there is no match, it check each of the values
   again with the bottom two bytes reversed. If such a match is found, the
   bottom two bytes of each argument is reversed, the high_byte_first flag
   is inverted to indicated how two byte integers should be read and the
   runction returns. */
   
void find_byte_order(int *x0, int *y0, int *x1, int * y1)
{
  int i;
  for (i=0; known_size[i]!=0; i++)
    {
      if (*x1 == known_size[i])
	return;
    }
  /* now check to see if reversing the bytes allows a match... */
  for (i=0; known_size[i]!=0; i++)
    {
      if (*x1 == swap_bytes (known_size[i]))
	{
	  *x0 = swap_bytes( *x0);
	  *y0 = swap_bytes( *y0);
	  *x1 = swap_bytes( *x1);
	  *y1 = swap_bytes( *y1);
	  high_byte_first = ! high_byte_first;
	  return;
	}
    }
}

/* Read a byte */

#define read_byte(stream) (getc (stream))

/* Read a coordinate - a two byte integer. */
  
int coord(FILE *input)
{
  int x;
  if ( high_byte_first )
    {
      x = ((char) read_byte(input)) << 8; /* get sign from high byte */
      x |= read_byte(input) & 0xFF; /* not from low byte */
    }
  else
    {
      x = read_byte (input) & 0xFF; /* ingnore sign in low byte */
      x |= ((char) read_byte (input)) << 8; /* get sign from high byte */
    }
  if ( ! signed_input )
    {
      x &= 0xFFFF;
    }

  return x;
}
  
/*  Read a string, change termination to null.
    note: string (buffer) reads a newline terminated string. */
  
void read_string (FILE *input, char *buffer, int buffer_length)
{
  int length=0;
  char termination = '\n';
  char c = '\0';

  while (!feof (input))
    {
      if (length > buffer_length)
	{
	  buffer_length *= 2;
	  buffer = (char *) realloc (buffer, buffer_length);
	  if (buffer <= (char *) 0)
	    {
	      perror ("realloc failed:");
	      exit (-1);
	    }
	}
      c = read_byte (input);
      if (c == termination)
	break;
      buffer [length++] = c;
    }

  buffer [length] = '\0';	/*  null terminate label */
}


/* read_plot reads a plot file from the standard input and calls
   a plot function according to each plot instruction found in the
   file. */

void read_plot(FILE *in_stream, char *buffer, int buffer_length)
{
  char x_adjust, y_adjust;
  int x0, y0, x1, y1, x2, y2;
  int instruction;

  instruction = read_byte (in_stream);
  while (!feof (in_stream))
    {
      switch (instruction)
	{
	  /*  Note: we must get all but the last argument before calling to
	      ensure reading them in the proper order. */
	  
	case ALABEL:
	  x_adjust = read_byte (in_stream);
	  y_adjust = read_byte (in_stream); 
	  read_string (in_stream, buffer, buffer_length);
	  alabel (x_adjust, y_adjust, buffer);
	  break;
	case ARC:
	  x0 = coord (in_stream);
	  y0 = coord (in_stream);
	  x1 = coord (in_stream);
	  y1 = coord (in_stream); 
	  x2 = coord (in_stream);
	  y2 = coord (in_stream); 
	  arc (x0, y0, x1, y1, x2, y2);
	  break;
	case CIRCLE:
	  x0 = coord (in_stream);
	  y0 = coord (in_stream);
	  x1 = coord (in_stream);
	  circle (x0, y0, x1);
	  break;
	case COLOR:
	  x0 = coord (in_stream)&0xFFFF;
	  y0 = coord (in_stream)&0xFFFF;
	  x1 = coord (in_stream)&0xFFFF;
	  color (x0, y0, x1);
	  break;
	case CONT:
	  x0 = coord (in_stream);
	  y0 = coord (in_stream);
	  cont (x0, y0);
	  break;
	case ERASE:
	  erase (in_stream);
	  break;
	case FILL:
	  fill (coord (in_stream)&0xFFFF); break;
	case FONT:
	  read_string (in_stream, buffer, buffer_length);
	  fontname (buffer);
	  break;
	case FONTSIZE:
	  fontsize (coord (in_stream));
	  break;
	case LABEL:
	  read_string (in_stream, buffer, buffer_length);
	  label (buffer);
	  break;
	case LINE:
	  x0 = coord (in_stream);
	  y0 = coord (in_stream);
	  x1 = coord (in_stream);
	  y1 = coord (in_stream); 
	  line (x0, y0, x1, y1);
	  break;
	case LINEMOD:
	  read_string (in_stream, buffer, buffer_length);
	  linemod (buffer);
	  break;
	case MOVE:
	  x0 = coord (in_stream);
	  y0 = coord (in_stream);
	  move_nasko (x0, y0);
	  break;
	case POINT:
	  x0 = coord (in_stream);
	  y0 = coord (in_stream);
	  point (x0, y0);
	  break;
	case ROTATE:
	  x0 = coord (in_stream);
	  y0 = coord (in_stream);
	  x1 = coord (in_stream);
	  rotate (x0, y0, x1);
	  break;
	case SPACE:
	  x0 = coord (in_stream);
	  y0 = coord (in_stream);
	  x1 = coord (in_stream);
	  y1 = coord (in_stream); 
	  if (guess_byte_order)
	    find_byte_order(&x0, &y0, &x1, &y1);
	  space (x0, y0, x1, y1);
	  break;
	default:
	  fprintf (stderr, "Unrecognized plot command `%c' ignored.\n", instruction);
	}
      instruction = read_byte (in_stream);
    }
  return;
}

int main (int argc, char *argv[])
{
  int i;
  char *buffer;
  int  buffer_length;
  int  named_plot=0;		/* count named plot files on command line. */
  int  show_usage=0;		/* remember whether to output usage message. */
  int  opened=0;		/* remember whether we have opened output. */

  buffer_length = 1024;
  buffer = (char *) malloc (buffer_length);
  if (buffer <= (char *) 0)
    {
      perror ("malloc failed:");
      exit (-1);
    }

  for (i = 1; i < argc; i++)
    {
      if ((strcmp (argv [i], "-fontsize") == 0)
	  || (strcmp (argv [i], "-fs") == 0))
	{
	  /* Sizes supported by X: 8, 10, 12, 14, 18, and 24. */
	  fontsize (atoi (argv[i+1]));
	  i += 1;
	}
      else if ((strcmp (argv [i], "-fontname") == 0)
	       || (strcmp (argv [i], "-fn") == 0))
	{
	  fontname (argv [i+1]);
	  i += 1;
	}
      else if ((strcmp (argv [i], "-high-byte-first") == 0)
	       || (strcmp (argv [i], "-h") == 0))
	{
	  guess_byte_order = 0;
	  high_byte_first = 1;
	}
      else if ((strcmp (argv [i], "-low-byte-first") == 0)
	       || (strcmp (argv [i], "-l") == 0))
	{
	  guess_byte_order = 0;
	  high_byte_first = 0;
	}
      else if ((strcmp (argv [i], "-warranty") == 0)
	       || (strcmp (argv [i], "-copying") == 0))
	{
/*
	  for (i=0; copy_notice[i][0]>0; i++)
	    fputs (copy_notice[i], stdout);
*/
	  named_plot++;
	}
      else if ((strcmp (argv [i], "-help") == 0)
	       || (strcmp (argv [i], "-V") == 0))
	{
	  printf ("%s version %s\n", argv[0], "0.0");
	  show_usage++;
	  named_plot++;
	}
      else if ((strcmp (argv [i], "-signed") == 0))
	{
	  signed_input=1;
	}
      else if ((strcmp (argv [i], "-unsigned") == 0))
	{
	  signed_input=0;
	}
      else if ((strcmp (argv [i], "-") == 0))
	{
	  read_plot (stdin, buffer, buffer_length);
	  named_plot++;
	}
      else
	{
	  FILE *filep;
	  filep = fopen ( argv [i], "r");
	  if (filep == NULL)
	    {
	      fprintf (stderr, "Unrecognized option or file `%s' ignored.\n",
		       argv [i]);
	      show_usage++;
	    }
	  else
	    {
	      named_plot++;
	      if (!opened)
		{
		  openpl ();
		  opened ++;
		}
	      read_plot (filep, buffer, buffer_length);
	      fclose (filep);
	    }
	}
    }



  /* if there were no named plot files on the command line, then read
     the standard input. */
  if (named_plot == 0)
    {
      if (!opened)
	{
	  openpl ();
	  opened ++;
	}
      read_plot (stdin, buffer, buffer_length);
    }
  if (opened)
    closepl();

  if ( show_usage || !opened)
    {
      fprintf (stderr, "\n\
usage: %s [-fontsize|-fs SIZE] [-high-byte-first|-h]\n\
       [-low-byte-first|-l] [-fontname|-fn FONTNAME]\n\
       [-help|-V] [-copying|-warranty] [PLOT_FILE_NAMES ...]\n",
	       *argv);
      fprintf (stderr, "\n\
    %s version 0.9, Copyright (C) 1989 Free Software Foundation, Inc.\n\
    %s comes with ABSOLUTELY NO WARRANTY.  This is free software, and\n\
    you are welcome to redistribute it. Type `%s -warranty' for warranty\n\
    details and copying conditions.\n\n",
	       *argv, *argv, *argv);
    }
  return 0;
}
