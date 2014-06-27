
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

ObjPtr 
Oalloc(char name[57])
{
  ObjPtr o=NULL;
  o = (ObjPtr) calloc(sizeof(object), 1);
  strcpy(o->Name,name);
  o->Color.R = 1;
  o->Color.G = 1;
  o->Color.B = 1;
  o->ReNo = 0;
  o->styletag = FULL;
  o->Number_of_Children  = 0;
  o->Number_of_Points    = 0;
  o->Number_of_Poly3s    = 0;
  o->Number_of_Poly4s    = 0;
  o->Number_of_Textures  = 0;
  o->Number_of_Materials = 0;
  o->Number_of_BitMaps   = 0;
  o->Children = NULL;
  o->Parent  = NULL;
  o->Rotation.x = 0;
  o->Rotation.y = 0;
  o->Rotation.z = 0;
  o->Scale.x = 1;
  o->Scale.y = 1;
  o->Scale.z = 1;
  o->Origin.x = 0;
  o->Origin.y = 0;
  o->Origin.z = 0;
  return (ObjPtr)o;
}

static int FPEqual(double d1, double d2) {
    return fabs(d1-d2) < 0.000001L;
}

ObjPointPtr
InsertPoint(ObjPtr o, double x, double y, double z)
{
  /***** Remember to remove p's when debugged *********/
  PointListPtr tmp,prv=NULL;
  ObjPointPtr p=NULL;
#ifdef DEBUG_OSYS
	printf("****Entered InsertPoint(%2.2f, %2.2f, %2.2f)****\n",x,y,z);
	fflush(stdout);
#endif /*DEBUG_OSYS*/
  if(o->Number_of_Points == 0) /* The simple case */{
#ifdef DEBUG_OSYS
    printf("Starting new structure\n");
    fflush(stdout);
#endif /*DEBUG_OSYS*/
    o->Points = malloc(sizeof(PointList));
    o->Points->P = malloc(sizeof(ObjPoint));
    o->Points->P->x = x;
    o->Points->P->y = y;
    o->Points->P->z = z;
    o->Points->P->tx = x;
    o->Points->P->ty = y;
    o->Points->P->tz = z;

    o->Points->NextPoint = NULL;
    o->Points->PrevPoint = NULL;
    p = o->Points->P;
    o->Number_of_Points++;
  } else {
    tmp = o->Points;
    /*Iterative search through the pointlist, as long as p==NULL point not found*/
    while( ((tmp) != NULL) && (p == NULL) ){
      if( FPEqual(tmp->P->x,x) && 
          FPEqual(tmp->P->y,y) && 
          FPEqual(tmp->P->z,z) )
	{ /* We found the point in the list */
#ifdef DEBUG_OSYS
	  printf("Found (%2.2f,%2.2f,%2.2f,)\n",x,y,z);
	  printf("x,y,z (%2.2f,%2.2f,%2.2f,)\n",x,y,z);
	  fflush(stdout);
#endif /*DEBUG_OSYS*/
	  p = tmp->P;
	} 
      else 
	{ /*We are still looking */
	  prv = tmp;
	  tmp = tmp->NextPoint;
	}
    } 
    if( tmp==NULL && p == NULL )
      { /*Then we haven't found the point in the list so we insert new */
#ifdef DEBUG_OSYS
	  printf("New   (%2.2f,%2.2f,%2.2f,)\n",x,y,z);
	  printf("x,y,z (%2.2f,%2.2f,%2.2f,)\n",x,y,z);
	  fflush(stdout);
#endif /*DEBUG_OSYS*/
	  tmp = malloc(sizeof(PointList));
	  prv->NextPoint = tmp;
	  tmp->P = (ObjPointPtr) malloc(sizeof(ObjPoint));
	  tmp->P->x = x;
	  tmp->P->y = y;
	  tmp->P->z = z;
	  tmp->P->tx = x;
	  tmp->P->ty = y;
	  tmp->P->tz = z;
	  tmp->PrevPoint = prv;
	  tmp->NextPoint = NULL;
	  p = tmp->P;
	  o->Number_of_Points++;
      }
  }	
  
  return p;
}
void  /* The Poly p are inserted in o, with the texture txture and .... */
InsertPoly3(ObjPtr o,Point p[3],TexturePtr txture,MaterialPtr mtrial)
{
  Poly3Ptr tmp=NULL;
  int i;
#ifdef DEBUG_OSYS
  printf("####Entered InsertPoly3####\n");
  fflush(stdout);
#endif /*DEBUG_OSYS*//* first poly ever */
  tmp = malloc(sizeof(Poly3));
  tmp->Next = NULL;
  tmp->Prev = NULL;
  tmp->Mat = NULL;
  tmp->Txt = NULL;
  for(i=0;i<3;i++)
    tmp->P[i] = InsertPoint(o,p[i].x,p[i].y,p[i].z);
  
  if(o->Poly3s == NULL)
    {
      o->Poly3s = tmp;
    }
  else
    {
      o->Poly3s->Prev = tmp;
      tmp->Next = o->Poly3s;
      o->Poly3s = tmp;
    }
  o->Number_of_Poly3s++;
  return;
}

void  /* The Poly p are inserted in o, with the texture txture and .... */
InsertPoly4(ObjPtr o,Point p[4],TexturePtr txture,MaterialPtr mtrial)
{
  Poly4Ptr tmp=NULL;
  int i;
#ifdef DEBUG_OSYS
  printf("####Entered InsertPoly4####\n");
  fflush(stdout);
#endif /*DEBUG_OSYS*//* first poly ever */
  tmp = malloc(sizeof(Poly4));
  tmp->Next = NULL;
  tmp->Prev = NULL;
  tmp->Mat = NULL;
  tmp->Txt = NULL;
  for(i=0;i<4;i++)
    tmp->P[i] = InsertPoint(o,p[i].x,p[i].y,p[i].z);
  
  if(o->Poly4s == NULL)
    {
      o->Poly4s = tmp;
    }
  else
    {
      o->Poly4s->Prev = tmp;
      tmp->Next = o->Poly4s;
      o->Poly4s = tmp;
    }
  o->Number_of_Poly4s++;
  return;
}

ObjPtr /*  (         double array[][]        ) giver 1 array med alle tal */
ArrayToPoly3(ObjPtr o,double array[][3],int size)
{ Point p[3];
  int i=0;
#ifdef DEBUG_OSYS
  printf("ArrayToPoly3\n");
#endif
  for(i=0;i<size;i=i+3)
    {
      p[0].x = array[i][0];
      p[0].y = array[i][1];
      p[0].z = array[i][2];
      
      p[1].x = array[i+1][0];
      p[1].y = array[i+1][1];
      p[1].z = array[i+1][2];
      
      p[2].x = array[i+2][0];
      p[2].y = array[i+2][1];
      p[2].z = array[i+2][2];
      
      InsertPoly3(o,p,NULL,NULL);
    }
  return o;
}
ObjPtr /*  (         double array[][]        ) giver 1 array med alle tal */
ArrayToPoly4(ObjPtr o,double array[][3],int size)
{ Point p[4];
  int i=0;
#ifdef DEBUG_SYS
  printf("ArrayToPoly\n");
#endif
  for(i=0;i<size;i=i+4)
    {
      p[0].x = array[i][0];
      p[0].y = array[i][1];
      p[0].z = array[i][2];
      
      p[1].x = array[i+1][0];
      p[1].y = array[i+1][1];
      p[1].z = array[i+1][2];
      
      p[2].x = array[i+2][0];
      p[2].y = array[i+2][1];
      p[2].z = array[i+2][2];

      p[3].x = array[i+3][0];
      p[3].y = array[i+3][1];
      p[3].z = array[i+3][2];
      
      InsertPoly4(o,p,NULL,NULL);
    }
  return o;
}

void 
PrintPoints(ObjPtr o)
{

  int i=0;
  PointListPtr tmp=NULL;
  if(o->Points != NULL)
    tmp = o->Points;
  while(tmp != NULL) {
    printf("Point[%i] = (%.2f, %.2f, %.2f)",i,tmp->P->x,tmp->P->y,tmp->P->z);
    printf(" -> (%.2f, %.2f, %.2f)\n",tmp->P->tx,tmp->P->ty,tmp->P->tz);
    ++i;
    tmp = tmp->NextPoint;
  }return;
}
void 
PrintPoly3s(ObjPtr o)
{
  Poly3Ptr tmp = NULL;
  int i,j=0;
  if(o->Poly3s != NULL) {
    tmp = o->Poly3s;
    while(tmp != NULL) {
      for(i=0;i<3;i++){
	printf("PrintPoly3s[%i] = (%.2f, %.2f, %.2f)",i
	       ,tmp->P[i]->x,tmp->P[i]->y,tmp->P[i]->z);
	printf("-> (%.2f, %.2f, %.2f)\n",tmp->P[i]->tx,tmp->P[i]->ty,tmp->P[i]->tz);
      }tmp = tmp->Next;
      j++;
    }
  }
}
void 
PrintPoly4s(ObjPtr o)
{
  Poly4Ptr tmp = NULL;
  int i,j=0;
  if(o->Poly4s != NULL) {
    tmp = o->Poly4s;
    while(tmp != NULL) {
      for(i=0;i<4;i++){
	printf("PrintPoly4s[%i] = %.2f, %.2f, %.2f",i
	       ,tmp->P[i]->x,tmp->P[i]->y,tmp->P[i]->z);
	printf("-> (%.2f, %.2f, %.2f)\n",tmp->P[i]->tx,tmp->P[i]->ty,tmp->P[i]->tz);
      }tmp = tmp->Next;
      j++;
    }
  }
}

void
PrintObject(ObjPtr o)
{
  printf("Name: %s\n",o->Name);
  PrintPoints(o);
  PrintPoly3s(o);
  PrintPoly4s(o);
  /*
    Print names of textures and materials
   */
  printf("Scale    : (%.2f,%.2f,%.2f)\n",o->Scale.x,o->Scale.y,o->Scale.z);
  printf("Origin   : (%.2f,%.2f,%.2f)\n",o->Origin.x,o->Origin.y,o->Origin.z);
  printf("Rotation : (%.2f,%.2f,%.2f)\n",o->Rotation.x,o->Rotation.y,o->Rotation.z);
  printf("Color    : (%.2f,%.2f,%.2f)\n",o->Color.R,o->Color.G,o->Color.B);
  if(o->styletag==FULL)
    printf("Style     : FULL\n");
  if(o->styletag==HALF)
    printf("Style     : HALF\n");
  if(o->styletag==NONE)
    printf("Style     : NONE\n");
  /* Print Names of Children & Parents */
}
void 
InsertChild(ObjPtr par, ObjPtr chld)
{
  if (par!=NULL)
    {
      if (par->Children==NULL)
	par->Children=chld;
      else {
	par->Children->next = chld;
	chld->prev = par->Children;
	chld->next = NULL;
	par->Children = chld;
      }
    }
}
/****************************** Trig Routines ***********************************/

void
CalcObjectChildren(ObjPtr o,
		   double sx, double sy, double sz,
		   double ax, double ay, double az,
		   double dx, double dy, double dz)
{
  ObjPtr tmp = NULL;
  if(o != NULL) {
    /*printf("CalcObjChildren(%s,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f)\n",
	   o->Name,
	   sx,sy,sz,
	   ax,ay,az,
	   dx,dy,dz);*/
    tmp = o->Children;
    while(tmp!=NULL){ 
      ScaleObjectAdd(tmp,sx,sy,sz);
      RotateObjectAdd(tmp,ax,ay,az);
      TranslateObjectAdd(tmp,dx,dy,dz);
      CalcObjectChildren(tmp,sx,sy,sz,ax,ay,az,dx,dy,dz);
      tmp=tmp->next;
    }
  }
}
void
CalcObject(ObjPtr o)
{
  ObjPtr tmp = NULL;
  if(o != NULL) {
    /*printf("CalcObj(%s)\n",o->Name);*/
    tmp = o->Children;
    while(tmp!=NULL){
      CalcObject(tmp);
      tmp=tmp->next;
    }
    CalcObjectChildren(o,
		       o->Scale.x,o->Scale.y,o->Scale.z,
		       o->Rotation.x,o->Rotation.y,o->Rotation.z,
    		       o->Origin.x,o->Origin.y,o->Origin.z);
    ScaleObjectOverwrite(o,o->Scale.x,o->Scale.y,o->Scale.z);
    RotateObjectAdd(o,o->Rotation.x,o->Rotation.y,o->Rotation.z); 
    TranslateObjectAdd(o,o->Origin.x,o->Origin.y,o->Origin.z);
  }
}
void
TranslateObjectAdd(ObjPtr o, double dx, double dy, double dz)
{
  Matrix *Rot = NULL;
  HPoint hp;
  PointListPtr index = NULL;
  if (o!=NULL){
    /*printf("TranslateObjectAdd(%s, %.2f, %.2f, %.2f)\n",o->Name,dx,dy,dz);*/
    Rot = TranslateMatrix(dx,dy,dz);
    index = o->Points;
    while (index!=NULL)
      {
	hp = TPointToHPoint(*index->P);
	hp = MultMatrixHPoint(Rot,hp);
	index->P->tx = hp.x;
	index->P->ty = hp.y;
	index->P->tz = hp.z;
	index = index->NextPoint;
      }  
  }else{
    fprintf(stderr,"Cannot Translate NULL-object\n");
  }
}
void
TranslateObjectOverwrite(ObjPtr o, double dx, double dy, double dz)
{
  Matrix *Rot = NULL;
  HPoint hp;
  PointListPtr index = NULL;
  if (o!=NULL){
    /*printf("TranslateObjectOverwrite(%s, %.2f, %.2f, %.2f)\n",o->Name,dx,dy,dz);*/
    Rot = TranslateMatrix(dx,dy,dz);
    index = o->Points;
    while (index!=NULL)
      {
	hp = PointToHPoint(*index->P);
	hp = MultMatrixHPoint(Rot,hp);
	index->P->tx = hp.x;
	index->P->ty = hp.y;
	index->P->tz = hp.z;
	index = index->NextPoint;
      }  
  }else{
    fprintf(stderr,"Cannot Translate NULL-object\n");
  }
}
void
RotateObjectAdd(ObjPtr o, double ax, double ay, double az)
{
  Matrix *Rot = NULL;
  HPoint hp;
  PointListPtr index = NULL;
  if (o!=NULL){
    /*printf("RotateObjectAdd(%s, %.2f, %.2f, %.2f)\n",o->Name,ax,ay,az);*/
    Rot = RotateMatrix(ax,ay,az);
    index = o->Points;
    while (index!=NULL)
      {
	hp = TPointToHPoint(*index->P);
	hp = MultMatrixHPoint(Rot,hp);
	index->P->tx = hp.x;
	index->P->ty = hp.y;
	index->P->tz = hp.z;
	index = index->NextPoint;
      }  
  }else{
    fprintf(stderr,"Cannot Rotate NULL-object\n");
  }
}
void 
RotateObjectOverwrite(ObjPtr o, double ax, double ay, double az)
{
  Matrix *Rot = NULL;
  HPoint hp;
  PointListPtr index = NULL;
  /*printf("RotateObjectOverwrite(%s, %.2f, %.2f, %.2f)\n",o->Name,ax,ay,az);*/
  Rot = RotateMatrix(ax,ay,az);
  if (o!=NULL){
    index = o->Points;
    while (index!=NULL)
      {
	hp = PointToHPoint(*index->P);
	hp = MultMatrixHPoint(Rot,hp);
	index->P->tx = hp.x;
	index->P->ty = hp.y;
	index->P->tz = hp.z;
	index = index->NextPoint;
      }
  }else{
    fprintf(stderr,"Cannot Rotate NULL-object\n");
  }
}
void 
ScaleObjectAdd(ObjPtr o, double sx, double sy, double sz)
{
  Matrix *Scale = NULL;
  HPoint hp;
  PointListPtr index = NULL;
  /*printf("ScaleObjectAdd(%s, %.2f, %.2f, %.2f)\n",o->Name,sx,sy,sz);*/
  Scale = ScaleMatrix(sx,sy,sz);
  if (o!=NULL){
    index = o->Points;
    while (index!=NULL)
      {
	hp = TPointToHPoint(*index->P);
	hp = MultMatrixHPoint(Scale,hp);
	index->P->tx = hp.x;
	index->P->ty = hp.y;
	index->P->tz = hp.z;
	index = index->NextPoint;
      }
  }else{
    fprintf(stderr,"Cannot Scale NULL-object\n");
  }
}
void 
ScaleObjectOverwrite(ObjPtr o, double sx, double sy, double sz)
{
  Matrix *Scale = NULL;
  HPoint hp;
  PointListPtr index = NULL;
  /*printf("ScaleObjectOverwrite(%s, %.2f, %.2f, %.2f)\n",o->Name,sx,sy,sz);*/
  Scale = ScaleMatrix(sx,sy,sz);
  if (o!=NULL){
    index = o->Points;
    while (index!=NULL)
      {
	hp = PointToHPoint(*index->P);
	hp = MultMatrixHPoint(Scale,hp);
	index->P->tx = hp.x;
	index->P->ty = hp.y;
	index->P->tz = hp.z;
	index = index->NextPoint;
      }
  }else{
    fprintf(stderr,"Cannot Scale NULL-object\n");
  }
}

/***********************Converted  GL Routines ****************************/
void 
SetObjectColor(ObjPtr o, float R, float G, float B)
{
  if(o!=NULL)
    /*    printf("SetColor(%s)\n",o->Name);*/
  o->Color.R=R;
  o->Color.G=G;
  o->Color.B=B;
}
void 
Draw_Children(ObjPtr o)
{
  ObjPtr tmp = NULL;
  if (o!=NULL){
    /*    printf("Draw_Children(%s)",o->Name);*/
    PrintObject(o);
    tmp = o->Children;
    while (tmp != NULL){
      Draw_Children(tmp);
      tmp = tmp -> next;
    }
  }
}
void
Draw_Object(ObjPtr o)
{
  if (o!=NULL){
    /*    printf("Draw_Object(%s)\n",o->Name);*/
    PrintObject(o);
    /*printf("o->Children: %x\n",(unsigned) o->Children);*/
    Draw_Children(o->Children);
  }
}
void
Draw_All_Nexts(ObjPtr o)
{
  ObjPtr tmp=NULL;
  if(o!=NULL) {
    /*    printf("Draw_All_Nexts(%s)\n",o->Name);*/
    tmp = o->next;
    while(tmp!=NULL){
      CalcObject(tmp);
      Draw_Object(tmp);
      tmp=tmp->next;
    }
  }
}
void 
Draw_All_Prevs(ObjPtr o)
{
  ObjPtr tmp=NULL;
  if(o!=NULL) {
    /*    printf("Draw_All_Prevs(%s)\n",o->Name);*/
    tmp = o->prev;
    while(tmp!=NULL){
      CalcObject(tmp);
      Draw_Object(tmp);
      tmp=tmp->prev;
    }
  }
}
void
Draw_All(ObjPtr o)
{
  if(o!=NULL)
    /*    printf("Draw_all(%s)\n",o->Name);*/
  CalcObject(o);
  Draw_All_Prevs(o);
  Draw_Object(o);
  Draw_All_Nexts(o);
}



