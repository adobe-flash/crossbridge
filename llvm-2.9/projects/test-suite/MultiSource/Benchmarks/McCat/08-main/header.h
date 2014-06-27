#include "Includes.h"
/********************************************************************************
 * Golbal defs for Mesa dependent features and objects,                         *
 * X dependent defs are in Interface.h                                          *
 ********************************************************************************/
#define PI 3.14159265359
#define DTOR(v)  (PI/180*v)
#define RTOD(v)  (180/PI*v)

/********************************************************************************
 * Prototypes for the object system                                             *
 ********************************************************************************/
ObjPtr Oalloc(char name[57]);

void PrintObject(ObjPtr o);
void PrintPoints(ObjPtr o);
void PrintPoly3s(ObjPtr o);
void PrintPoly4s(ObjPtr o);
ObjPointPtr InsertPoint(ObjPtr o, double x, double y, double z);
void InsertPoly3(ObjPtr o,Point p[3],TexturePtr txture,MaterialPtr mtrial);
void InsertPoly4(ObjPtr o,Point p[3],TexturePtr txture,MaterialPtr mtrial);
void InsertChild(ObjPtr par, ObjPtr chld);
ObjPtr ArrayToPoly3(ObjPtr o,double array[][3],int size);
ObjPtr ArrayToPoly4(ObjPtr o,double array[][3],int size);
void SetObjectColor(ObjPtr o, float R, float G, float B);
void CalcObjectChildren(ObjPtr o,double sx, double sy, double sz,
			double ax, double ay, double az,
			double dx, double dy, double dz);
void CalcObject(ObjPtr o);
void TranslateAll(ObjPtr o, double dx, double dy, double dz);
void TranslateObjectOverwrite(ObjPtr o, double dx, double dy, double dz);
void TranslateObjectAdd(ObjPtr o, double dx, double dy, double dz);
void RotateAll(ObjPtr o, double ax, double ay, double az);
void RotateObjectAdd(ObjPtr o, double ax, double ay, double az);
void RotateObjectOverwrite(ObjPtr o, double ax, double ay, double az);
void ScaleAll(ObjPtr o, double sx, double sy, double sz);
void ScaleObjectAdd(ObjPtr o, double ax, double ay, double az);
void ScaleObjectOverwrite(ObjPtr o, double ax, double ay, double az);
void Draw_Children(ObjPtr o);
void Draw_Object(ObjPtr o);
void Draw_All_Nexts(ObjPtr o);
void Draw_All_Prevs(ObjPtr o);
void Draw_All(ObjPtr o);
/************************** Trig.c Headers **************************/
void MultMatrixMatrix(Matrix *A, Matrix *B, Matrix *C);
HPoint MultMatrixHPoint(Matrix *mat,HPoint P);
ObjPoint RotatePoint( ObjPoint a, double rx, double ry, double rz);
void PrintMatrix( Matrix Mat);
Matrix * CopyMatrix(Matrix *Mat);
void trigmain(void);
Matrix *ScaleMatrix( double sx, double sy, double sz );
Matrix *RotateMatrix( double rx, double ry, double rz);
Matrix *TranslateMatrix( double dx, double dy, double dz);
Matrix *IdentMatrix( void );

/************************* Convert.c Headers ************************/
HPoint PointToHPoint(ObjPoint P);
HPoint TPointToHPoint(ObjPoint TP);
ObjPoint HPointToPoint(HPoint P);
ObjPoint HPointToTPoint(HPoint P);

