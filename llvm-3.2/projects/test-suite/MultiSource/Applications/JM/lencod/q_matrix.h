
/*!
 ***************************************************************************
 * \file
 *    q_matrix.h
 *
 * \brief
 *    Headerfile for q_matrix array
 *
 * \date
 *    07. Apr 2004
 ***************************************************************************
 */

#ifndef _Q_MATRIX_H_
#define _Q_MATRIX_H_

extern int ****LevelScale4x4Luma;
extern int *****LevelScale4x4Chroma;
extern int ****LevelScale8x8Luma;

extern int ****InvLevelScale4x4Luma;
extern int *****InvLevelScale4x4Chroma;
extern int ****InvLevelScale8x8Luma;

extern short ScalingList4x4input[6][16];
extern short ScalingList8x8input[2][64];
extern short ScalingList4x4[6][16];
extern short ScalingList8x8[2][64];

extern short UseDefaultScalingMatrix4x4Flag[6];
extern short UseDefaultScalingMatrix8x8Flag[2];

extern int *qp_per_matrix;
extern int *qp_rem_matrix;


void Init_QMatrix (void);
void CalculateQuantParam(void);
void CalculateQuant8Param(void);
void free_QMatrix(void);

#endif
