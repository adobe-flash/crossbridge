
/*!
 ************************************************************************
 * \file refbuf.h
 *
 * \brief
 *    Declarations of the reference frame buffer types and functions
 ************************************************************************
 */
#ifndef _REBUF_H_
#define _REBUF_H_

// global sub-pel image access variables
int height_pad, width_pad;
int height_pad_cr, width_pad_cr;

imgpel *UMVLine4X (imgpel ****Pic, int y, int x);
imgpel *FastLine4X(imgpel ****Pic, int y, int x);
imgpel *UMVLine8X_chroma (imgpel ****Pic, int y, int x);
imgpel *FastLine8X_chroma(imgpel ****Pic, int y, int x);

#endif

