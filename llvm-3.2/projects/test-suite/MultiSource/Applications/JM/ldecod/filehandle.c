
/*!
 **************************************************************************************
 * \file
 *    filehandle.c
 * \brief
 *     Trace file handling and standard error handling function.
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *      - Karsten Suehring            <suehring@hhi.de>
 ***************************************************************************************
 */

#include <stdlib.h>

#include "contributors.h"
#include "global.h"
#include "mbuffer.h"

/*!
 ************************************************************************
 * \brief
 *    Error handling procedure. Print error message to stderr and exit
 *    with supplied code.
 * \param text
 *    Error message
 * \param code
 *    Exit code
 ************************************************************************
 */
void error(char *text, int code)
{
  fprintf(stderr, "%s\n", text);
  flush_dpb();
  exit(code);
}

#if TRACE

static int bitcounter = 0;

/*!
 ************************************************************************
 * \brief
 *    Tracing bitpatterns for symbols
 *    A code word has the following format: 0 Xn...0 X2 0 X1 0 X0 1
 ************************************************************************
 */
void tracebits(
    const char *trace_str,  //!< tracing information, char array describing the symbol
    int len,                //!< length of syntax element in bits
    int info,               //!< infoword of syntax element
    int value1)
{

  int i, chars;
  // int outint = 1;


  if(len>=34)
  {
    snprintf(errortext, ET_SIZE, "Length argument to put too long for trace to work");
    error (errortext, 600);
  }

  putc('@', p_trace);
  chars = fprintf(p_trace, "%i", bitcounter);
  while(chars++ < 5)
    putc(' ',p_trace);

  chars += fprintf(p_trace, " %s", trace_str);
  while(chars++ < 55)
    putc(' ',p_trace);

  // Align bitpattern
  if(len<15)
  {
    for(i=0 ; i<15-len ; i++)
      fputc(' ', p_trace);
  }

  // Print bitpattern
  for(i=0 ; i<len/2 ; i++)
  {
    fputc('0', p_trace);
  }
  // put 1
  fprintf(p_trace, "1");

  // Print bitpattern
  for(i=0 ; i<len/2 ; i++)
  {
      if (0x01 & ( info >> ((len/2-i)-1)))
        fputc('1', p_trace);
      else
        fputc('0', p_trace);
  }

  fprintf(p_trace, " (%3d) \n", value1);
  bitcounter += len;

  fflush (p_trace);

}

/*!
 ************************************************************************
 * \brief
 *    Tracing bitpatterns
 ************************************************************************
 */
void tracebits2(
    const char *trace_str,  //!< tracing information, char array describing the symbol
    int len,                //!< length of syntax element in bits
    int info)
{

  int i, chars;
  // int outint = 1;

  if(len>=45)
  {
    snprintf(errortext, ET_SIZE, "Length argument to put too long for trace to work");
    error (errortext, 600);
  }

  putc('@', p_trace);
  chars = fprintf(p_trace, "%i", bitcounter);
  while(chars++ < 5)
    putc(' ',p_trace);
  chars += fprintf(p_trace, " %s", trace_str);
  while(chars++ < 55)
    putc(' ',p_trace);

  // Align bitpattern
  if(len<15)
    for(i=0 ; i<15-len ; i++)
      fputc(' ', p_trace);


  bitcounter += len;
  while (len >= 32)
  {
    for(i=0 ; i<8 ; i++)
    {
      fputc('0', p_trace);
    }
    len -= 8;

  }
  // Print bitpattern
  for(i=0 ; i<len ; i++)
  {
    if (0x01 & ( info >> (len-i-1)))
      fputc('1', p_trace);
    else
      fputc('0', p_trace);
  }

  fprintf(p_trace, " (%3d) \n", info);

  fflush (p_trace);
}
#endif

