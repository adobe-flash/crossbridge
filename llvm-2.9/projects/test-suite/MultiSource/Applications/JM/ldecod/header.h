
/*!
 *************************************************************************************
 * \file header.h
 *
 * \brief
 *    Prototypes for header.c
 *************************************************************************************
 */

#ifndef _HEADER_H_
#define _HEADER_H_

int FirstPartOfSliceHeader();
int RestOfSliceHeader();

void dec_ref_pic_marking(Bitstream *currStream);

void decode_poc(struct img_par *img);
int dumppoc(struct img_par *img);

#endif

