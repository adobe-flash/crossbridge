
/*!
 ***************************************************************************
 *
 * \file intrarefresh.h
 *
 * \brief
 *    Pseudo-Raqndom Intra macroblock refresh support
 *
 * \date
 *    16 June 2002
 *
 * \author
 *    Stephan Wenger   stewe@cs.tu-berlin.de
 **************************************************************************/

#ifndef _INTRAREFRESH_H_
#define _INTRAREFRESH_H_

void RandomIntraInit(int xsize, int ysize, int refresh);
void RandomIntraUninit(void);
int RandomIntra (int mb);   //! returns 1 for MBs that need forced Intra
void RandomIntraNewPicture (void);  //! to be called once per picture


#endif //_INTRAREFRESH_H_
