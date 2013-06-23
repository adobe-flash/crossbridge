
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
/****************************************************************************/
HPoint 
PointToHPoint(ObjPoint P)
{
  HPoint res;
  res.x = P.x;
  res.y = P.y;
  res.z = P.z;
  res.w = 1;
  return res;
}
HPoint 
TPointToHPoint(ObjPoint TP)
{
  HPoint res;
  res.x = TP.tx;
  res.y = TP.ty;
  res.z = TP.tz;
  res.w = 1;
  return res;
}
ObjPoint 
HPointToPoint(HPoint P)
{
  ObjPoint res;
  res.x = P.x;
  res.y = P.y;
  res.z = P.z;
  return res;
}
ObjPoint 
HPointToTPoint(HPoint P)
{
  ObjPoint res;
  res.tx = P.x;
  res.ty = P.y;
  res.tz = P.z;
  return res;
}
