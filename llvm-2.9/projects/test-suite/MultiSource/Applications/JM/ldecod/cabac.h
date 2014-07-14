
/*!
 ***************************************************************************
 * \file
 *    cabac.h
 *
 * \brief
 *    Headerfile for entropy coding routines
 *
 * \author
 *    Detlev Marpe                                                         \n
 *    Copyright (C) 2000 HEINRICH HERTZ INSTITUTE All Rights Reserved.
 *
 * \date
 *    21. Oct 2000 (Changes by Tobias Oelbaum 28.08.2001)
 ***************************************************************************
 */

#ifndef _CABAC_H_
#define _CABAC_H_

#include "global.h"

MotionInfoContexts* create_contexts_MotionInfo(void);
TextureInfoContexts* create_contexts_TextureInfo(void);
void init_contexts_MotionInfo(struct img_par *img, MotionInfoContexts *enco_ctx);
void init_contexts_TextureInfo(struct img_par *img, TextureInfoContexts *enco_ctx);
void delete_contexts_MotionInfo(MotionInfoContexts *enco_ctx);
void delete_contexts_TextureInfo(TextureInfoContexts *enco_ctx);

void cabac_new_slice();

void readMB_typeInfo_CABAC(SyntaxElement *se, struct img_par *img, DecodingEnvironmentPtr dep_dp);
void readB8_typeInfo_CABAC(SyntaxElement *se, struct img_par *img, DecodingEnvironmentPtr dep_dp);
void readIntraPredMode_CABAC(SyntaxElement *se, struct img_par *img, DecodingEnvironmentPtr dep_dp);
void readRefFrame_CABAC(SyntaxElement *se, struct img_par *img, DecodingEnvironmentPtr dep_dp);
void readMVD_CABAC(SyntaxElement *se, struct img_par *img, DecodingEnvironmentPtr dep_dp);
void readCBP_CABAC(SyntaxElement *se, struct img_par *img, DecodingEnvironmentPtr dep_dp);
void readRunLevel_CABAC(SyntaxElement *se, struct img_par *img,  DecodingEnvironmentPtr dep_dp);
void readDquant_CABAC(SyntaxElement *se,struct img_par *img,DecodingEnvironmentPtr dep_dp);
void readCIPredMode_CABAC(SyntaxElement *se,struct img_par *img,DecodingEnvironmentPtr dep_dp);
void readMB_skip_flagInfo_CABAC( SyntaxElement *se, struct img_par *img, DecodingEnvironmentPtr dep_dp);
void readFieldModeInfo_CABAC(SyntaxElement *se, struct img_par *img,DecodingEnvironmentPtr dep_dp);

void readMB_transform_size_flag_CABAC( SyntaxElement *se, struct img_par *img, DecodingEnvironmentPtr dep_dp);

int  readSyntaxElement_CABAC(SyntaxElement *se, struct img_par *img, DataPartition *this_dataPart);

int  check_next_mb_and_get_field_mode_CABAC(SyntaxElement *se,struct img_par *img,DataPartition  *act_dp);
void CheckAvailabilityOfNeighborsCABAC();


#endif  // _CABAC_H_

