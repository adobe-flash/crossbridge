
/*!
 ************************************************************************
 * \file
 *    macroblock.h
 *
 * \brief
 *    Arrays for macroblock processing
 *
 * \author
 *    Inge Lille-Langoy               <inge.lille-langoy@telenor.com>     \n
 *    Telenor Satellite Services                                          \n
 *    P.O.Box 6914 St.Olavs plass                                         \n
 *    N-0130 Oslo, Norway
 *
 ************************************************************************/

#ifndef _MACROBLOCK_H_
#define _MACROBLOCK_H_

void proceed2nextMacroblock(void);

void  start_macroblock(int mb_addr, int mb_field);
void  terminate_macroblock(Boolean *end_of_slice, Boolean *recode_macroblock);

void  write_one_macroblock(int eos_bit);

void LumaPrediction4x4   (int, int, int, int, int,   short, short);
void LumaPrediction4x4Bi (int, int, int, int, short, short, int  );

int  LumaResidualCoding8x8 (int*, int64*, int, short, int, int, short, short);
void LumaResidualCoding (void);

void ChromaResidualCoding (int*);

void IntraChromaPrediction (int*, int*, int*);
void IntraChromaRDDecision (RD_PARAMS);

int  TransformDecision(int, int*);

int  B8Mode2Value (int b8mode, int b8pdir);

int  writeMBLayer (int rdopt, int *coeff_rate);
void write_terminating_bit (short bit);

int  writeReferenceFrame  (int mode, int i, int j, int fwd_flag, int  ref);
int  writeMotionVector8x8 (int  i0, int  j0, int  i1, int  j1, int  refframe, int  list_idx, int  mv_mode);

int  writeLumaCoeff4x4_CABAC (int, int, int);
int  writeLumaCoeff8x8_CABAC (int, int);
int  writeLumaCoeff8x8       (int, int, int);

int  writeCoeff4x4_CAVLC     (int block_type, int b8, int b4, int param);

int  find_sad_16x16 (int *intra_mode);

#endif

