
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
double pyramid[][3] = 
{
  { 1.00,-0.67, 0.67},   /* BundFladen */
  {-1.00,-0.67, 0.67},
  { 0.00,-0.67,-1.00},

  /* Flade 1  Right Side*/
  { 0.00, 1.00, 0.00},   
  { 1.00,-0.67, 0.67},
  { 0.00,-0.67,-1.00},
  /* Flade 2 */
  { 0.00, 1.00, 0.00},
  { 0.00,-0.67,-1.00},
  {-1.00,-0.67, 0.67},
  /* Flade 3 Front */ 
  { 0.00, 1.00, 0.00},
  { 1.00,-0.67, 0.67},
  {-1.00,-0.67, 0.67},

};
double SPyramid[][3] =
{
  /* BundFladen */
  { 1.00,-1.00, 1.00},
  {-1.00,-1.00, 1.00},
  {-1.00,-1.00,-1.00},
  { 1.00,-1.00,-1.00},
  /* TopFladen */
  { 1.00, 1.00, 1.00},
  { 1.00, 1.00,-1.00},
  {-1.00, 1.00,-1.00},
  {-1.00, 1.00, 1.00},
  /* FrontFladen */
  { 1.00,-1.00, 1.00},
  { 1.00, 1.00, 1.00},
  {-1.00, 1.00, 1.00},
  {-1.00,-1.00, 1.00},
  /* BagFladen */
  { 1.00,-1.00,-1.00}, 
  {-1.00,-1.00,-1.00},
  {-1.00, 1.00,-1.00},
  { 1.00, 1.00,-1.00},
  /* HSideFlade */
  { 1.00,-1.00,-1.00}, 
  { 1.00, 1.00,-1.00},
  { 1.00, 1.00, 1.00},
  { 1.00,-1.00, 1.00},
  /* VSideFlade */
  {-1.00,-1.00,-1.00}, 
  {-1.00,-1.00, 1.00},
  {-1.00, 1.00, 1.00},
  {-1.00, 1.00,-1.00}
};
ObjPtr
MakeSphere( ObjPtr o,int sli, int pol, double r)
{
  Point p[4];
  double fi,theta;
  double dfi, dtheta;
  /*printf("MakeSphere");*/
  dfi    = 2*PI/sli;    /* The step in the slices */
  dtheta = 2*PI/pol;    /* The step in the amount of polys in a slice */
  for(fi=-PI;fi<PI+0.000001;fi=fi+dfi){ /* The slices */
    for(theta=-PI;theta<PI+0.000001;theta=theta+dtheta){   /* The polys in the slices */
      /*printf(".");
      fflush(stdout);*/
      p[0].x = cos(theta)*cos(fi)*r;
      p[0].y = sin(theta)*cos(fi)*r;
      p[0].z = sin(fi)*r;

      p[1].x = cos(theta)*cos(fi+dfi)*r;
      p[1].y = sin(theta)*cos(fi+dfi)*r;
      p[1].z = sin(fi+dfi)*r;

      p[2].x = cos(theta+dtheta)*cos(fi+dfi)*r;
      p[2].y = sin(theta+dtheta)*cos(fi+dfi)*r;
      p[2].z = sin(fi+dfi)*r;

      p[3].x = cos(theta+dtheta)*cos(fi)*r;
      p[3].y = sin(theta+dtheta)*cos(fi)*r;
      p[3].z = sin(fi)*r;

      InsertPoly4(o,p,NULL,NULL);
    }
  }
  /*  printf("\n");*/
  return o;
}
int 
main(int argc, char **argv)
{
  int i = 0;
  ObjPtr o = NULL, tmp = NULL, ttmp = NULL;
  o = Oalloc("TestObject");
  tmp = Oalloc("SphereObject");
  tmp = MakeSphere(tmp,25,25,10);
  tmp->Origin.x = 87;
  tmp->Origin.y = 87;
  InsertChild(o,tmp);
  
  for(i=0;i<360;i++){
    o->Rotation.y = i;
    CalcObject(o);
  }

  PrintObject(tmp);
  
  o=ttmp;

  Draw_All(o);
  
  return 0;
}
