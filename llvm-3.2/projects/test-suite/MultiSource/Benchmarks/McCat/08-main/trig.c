
/****
    Copyright (C) 1996 McGill University.
    Copyright (C) 1996 McCAT System Group.
    Copyright (C) 1996 ACAPS Benchmark Administrator
                       benadmin@acaps.cs.mcgill.ca

    This program is free software; you can redistribute it and/or modify
    it provided this copyright notice is maintained.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
****/

/*****************************************************************************/
/* Author:  Peter Riishoej Brinkler (riishigh@daimi.aau.dk)                  */
/*****************************************************************************/

#include "header.h"
void
MultMatrixMatrix(Matrix *A, Matrix *B, Matrix *C)
{
  int i=0,j=0,k=0;
  for(i=0;i<4;i++){
    for(j=0;j<4;j++){
      (*C)[i][j] = 0;
      for(k=0;k<4;k++){
	(*C)[i][j] += (*A)[i][k] * (*B)[k][j];
      }
    }
  }
}

HPoint
MultMatrixHPoint(Matrix *mat,HPoint P) /* Array of Horiz Rows in Matrix */
{
  HPoint Res;
  Res.x = P.x * (*mat)[0][0] + P.y * (*mat)[0][1] 
    + P.z * (*mat)[0][2] + P.w * (*mat)[0][3];
  Res.y = P.x * (*mat)[1][0] + P.y * (*mat)[1][1]
    + P.z * (*mat)[1][2] + P.w * (*mat)[1][3];
  Res.z = P.x * (*mat)[2][0] + P.y * (*mat)[2][1] 
    + P.z * (*mat)[2][2] + P.w * (*mat)[2][3];
  Res.w = P.x * (*mat)[3][0] + P.y * (*mat)[3][1] 
    + P.z * (*mat)[3][2] + P.w * (*mat)[3][3];
  return Res;
}
Matrix *
CopyMatrix(Matrix *Mat)
{
  int i,j;
  Matrix *Res=NULL;
  if (Mat!=NULL) {
    Res = malloc(sizeof(Matrix));
    for(i=0;i<4;i++){
      for(j=0;j<4;j++){
	/*printf("Copying Mat[%i][%i] = %.2f\n",i,j,(*Mat)[i][j]);*/
	(*Res)[i][j] = (*Mat)[i][j];
      }
    }
  }
  return Res;
}
Matrix *
IdentMatrix(void)
{
  Matrix SI = { { 1.00, 0.00, 0.00, 0.00 }, 
		{ 0.00, 1.00, 0.00, 0.00 },
		{ 0.00, 0.00, 1.00, 0.00 },
		{ 0.00, 0.00, 0.00, 1.00 }};
  Matrix *I;
  I = malloc(sizeof(Matrix));
  
  I = CopyMatrix(&SI);
  /*PrintMatrix(*I);*/
  return I;
}

Matrix *
TranslateMatrix( double dx, double dy, double dz)
{
  Matrix *TMat;
  TMat = IdentMatrix();
  (*TMat)[0][3] = dx;
  (*TMat)[1][3] = dy;
  (*TMat)[2][3] = dz;
  return TMat;
}
Matrix *
RotateMatrix( double rx, double ry, double rz)
{
  Matrix *RMatX, *RMatY, *RMatZ;
  Matrix *RMatXY, *RMatXYZ;
  double cosrx, sinrx;
  double cosry, sinry;
  double cosrz, sinrz;

  cosrx = cos(DTOR(rx));
  sinrx = sin(DTOR(rx));
  cosry = cos(DTOR(ry));
  sinry = sin(DTOR(ry));
  cosrz = cos(DTOR(rz));
  sinrz = sin(DTOR(rz));
  
  /*printf("cosrx = %.2f, sinrx = %.2f\n",cosrx,sinrx);
    printf("cosry = %.2f, sinry = %.2f\n",cosry,sinry);
    printf("cosrz = %.2f, sinrz = %.2f\n",cosrz,sinrz);*/

  RMatX = IdentMatrix();
  RMatY = IdentMatrix();
  RMatZ = IdentMatrix();
  RMatXY = IdentMatrix();
  RMatXYZ = IdentMatrix();
  (*RMatX)[1][1] =  cosrx;   /*   1    0    0   0 */
  (*RMatX)[1][2] = -sinrx;   /*   0   cos -sin  0 */
  (*RMatX)[2][1] =  sinrx;   /*   0   sin  cos  0 */
  (*RMatX)[2][2] =  cosrx;   /*   0    0    0   1 */

  (*RMatY)[0][0] =  cosry;   /*  cos   0   sin  0 */
  (*RMatY)[0][2] =  sinry;   /*   0    1    0   0 */
  (*RMatY)[2][0] = -sinry;   /* -sin   0   cos  0 */
  (*RMatY)[2][2] =  cosry;   /*   0    0    0   1 */

  (*RMatZ)[0][0] =  cosrz;   /*  cos -sin   0   0 */
  (*RMatZ)[0][1] = -sinrz;   /*  sin  cos   0   0 */
  (*RMatZ)[1][0] =  sinrz;   /*   0    0    1   0 */
  (*RMatZ)[1][1] =  cosrz;   /*   0    0    0   1 */
  MultMatrixMatrix( RMatX, RMatY, RMatXY );
  MultMatrixMatrix( RMatXY, RMatZ, RMatXYZ );  
  return RMatXYZ;
}
Matrix *
ScaleMatrix( double sx, double sy, double sz )
{
  Matrix *I;
  I = IdentMatrix();
  (*I)[0][0] = sx;
  (*I)[1][1] = sy;
  (*I)[2][2] = sz;
  return I;
}


ObjPoint 
RotatePoint( ObjPoint a, double rx, double ry, double rz)
{
  Matrix *Mat;
  /*MultMatrixHPoint(Matrix *mat,HPoint P)  Array of Horiz Rows in Matrix */
  HPoint A;
  A = PointToHPoint(a);
  Mat = RotateMatrix(rx,ry,rz);
  A = MultMatrixHPoint(Mat,A);
  return a;  
}
void
PrintMatrix( Matrix Mat)
{
  printf(" [[ %.2f, %.2f, %.2f, %.2f] \n",Mat[0][0],Mat[0][1],Mat[0][2],Mat[0][3]);
  printf("  [ %.2f, %.2f, %.2f, %.2f] \n",Mat[1][0],Mat[1][1],Mat[1][2],Mat[1][3]);
  printf("  [ %.2f, %.2f, %.2f, %.2f] \n",Mat[2][0],Mat[2][1],Mat[2][2],Mat[2][3]);
  printf("  [ %.2f, %.2f, %.2f, %.2f]]\n",Mat[3][0],Mat[3][1],Mat[3][2],Mat[3][3]);
}
