
/*!
 *****************************************************************************
 *
 * \file intrarefresh.c
 *
 * \brief
 *    Encoder support for pseudo-random intra macroblock refresh
 *
 * \date
 *    16 June 2002
 *
 * \author
 *    Stephan Wenger   stewe@cs.tu-berlin.de
 *****************************************************************************/

#include <stdlib.h>
#include <assert.h>

#include "global.h"

static int *RefreshPattern;
static int *IntraMBs;
static int WalkAround = 0;
static int NumberOfMBs = 0;
static int NumberIntraPerPicture;

/*!
 ************************************************************************
 * \brief
 *    RandomIntraInit: Initializes Random Intra module.  Should be called
 *    only after initialization (or changes) of the picture size or the
 *    random intra refresh value.  In version jm2.1 it is impossible to
 *    change those values on-the-fly, hence RandomIntraInit should be
 *    called immediately after the parsing of the config file
 *
 * \par Input:
 *    xsize, ysize: size of the picture (in MBs)
 *    refresh     : refresh rate in MBs per picture
 ************************************************************************
 */

void RandomIntraInit(int xsize, int ysize, int refresh)
{
  int i, pos;

  srand (1);      // A fixed random initializer to make things reproducible
  NumberOfMBs = xsize * ysize;
  NumberIntraPerPicture = refresh;

  if (refresh != 0)
  {
    RefreshPattern = malloc (sizeof (int) * NumberOfMBs);
    if (RefreshPattern == NULL) no_mem_exit("RandomIntraInit: RefreshPattern");

    IntraMBs = malloc (sizeof (int) * refresh);
    if (IntraMBs == NULL) no_mem_exit("RandomIntraInit: IntraMBs");

    for (i= 0; i<NumberOfMBs; i++)
      RefreshPattern[i] = -1;

    for (i=0; i<NumberOfMBs; i++)
    {
      do
      {
        pos = rand() % NumberOfMBs;
      } while (RefreshPattern [pos] != -1);
      RefreshPattern [pos] = i;
    }
    /*
    for (i=0; i<NumberOfMBs; i++) printf ("%d\t", RefreshPattern[i]);
    getchar();
    */
  }
  else
  {
    RefreshPattern = NULL;
    IntraMBs = NULL;
  }
}

/*!
 ************************************************************************
 * \brief
 *    RandomIntra: Code an MB as Intra?
 *
 * \par Input
 *    MacroblockNumberInScanOrder
 * \par Output
 *    1 if an MB should be forced to Intra, according the the
 *      RefreshPattern
 *    0 otherwise
 *
 ************************************************************************
 */

int RandomIntra (int mb)
{
  int i;

  for (i=0; i<NumberIntraPerPicture; i++)
    if (IntraMBs[i] == mb)
      return 1;
  return 0;
}


/*!
 ************************************************************************
 * \brief
 *    RandomIntraNewPicture: Selects new set of MBs for forced Intra
 *
 * \par
 *    This function should be called exactly once per picture, and
 *    requires a finished initialization
 *
 ************************************************************************
 */

void RandomIntraNewPicture ()
{
  int i, j;

  WalkAround += NumberIntraPerPicture;
  for (j=0,i=WalkAround; j<NumberIntraPerPicture; j++, i++)
    IntraMBs[j] = RefreshPattern [i%NumberOfMBs];
}

void RandomIntraUninit()
{
  if (NumberIntraPerPicture >0 )
  {
    free(RefreshPattern);
    free(IntraMBs);
  }
}
