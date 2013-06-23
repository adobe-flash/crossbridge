/*!
 ************************************************************************
 *  \file
 *     loopfilter.h
 *  \brief
 *     external loop filter interface
 ************************************************************************
 */

#ifndef _LOOPFILTER_H_
#define _LOOPFILTER_H_

#include "global.h"
#include "mbuffer.h"

void DeblockPicture(struct img_par *img, StorablePicture *p) ;

#endif //_LOOPFILTER_H_
