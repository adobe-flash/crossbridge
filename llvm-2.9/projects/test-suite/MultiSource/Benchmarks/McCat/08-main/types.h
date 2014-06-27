/********************************************************************************
 * Point definition and pointlist definition                                    *
 ********************************************************************************/
typedef struct PointStruct *PointPtr;
typedef struct PointStruct
{
  double x;
  double y;
  double z;
}Point;

typedef struct ObjPointStruct *ObjPointPtr;
typedef struct ObjPointStruct
{
  double x;
  double y;
  double z;
  double tx;
  double ty;
  double tz;
}ObjPoint;
typedef struct HPointStruct *HPointPtr;
typedef struct HPointStruct
{
  double x;
  double y;
  double z;
  double w;
}HPoint;
typedef struct PointListStruct *PointListPtr;
typedef struct PointListStruct
{
  ObjPointPtr P;
  PointListPtr NextPoint;
  PointListPtr PrevPoint;
}PointList;
typedef double Matrix[4][4];
typedef Matrix *MatrixPtr;

/********************************************************************************
 * Texture, Mateial and Bitmap definitions and listdefinitions                  *
 ********************************************************************************/

typedef struct BitMapStruct
{
  unsigned char *bitmap;
}BitMap;

typedef struct BitMapListStruct *BitMapListPtr;
typedef struct BitMapListStruct 
{ 
  BitMap current;
  BitMapListPtr NextBitMap;
  BitMapListPtr PrevBitMap;
}BitMapList;

typedef struct Texture *TexturePtr;
typedef struct Texture
{
  Point Color;
  BitMapListPtr Textures;
  /*float DiffuseIndex;
  BitMapListPtr DiffuseBitmaps;
  float SpecularIndex;
  enum{ low, medium, high}
  int spectag;
  float Reflectivity;
  BitMapListPtr ReflectionBitmaps;
  float transparency
  enum{ edge, opaque }
  int transptag;
  BitMapList BumpMaps;*/
  /*******************************Texture Coords*********************************/
  Point center;
  Point direction;
  Point scale;
  /*******************************End of Texture*********************************/
  TexturePtr next;
  TexturePtr prev;
}Texture;

typedef struct Material *MaterialPtr;
typedef struct Material
{
  float Elasticity;
  float Airness;
  float Softness;
  MaterialPtr next;
  MaterialPtr prev;
} Material;

/********************************************************************************
 * Poly definition and polylist definition                                      *
 ********************************************************************************/
typedef struct Poly3Struct *Poly3Ptr;
typedef struct Poly3Struct
{
  ObjPointPtr P[3];
  MaterialPtr Mat;
  TexturePtr Txt;
  Poly3Ptr Next;
  Poly3Ptr Prev;
}Poly3;

typedef struct Poly4Struct *Poly4Ptr;
typedef struct Poly4Struct
{
  ObjPointPtr P[4];
  MaterialPtr Mat;
  TexturePtr Txt;
  Poly4Ptr Next;
  Poly4Ptr Prev;
}Poly4;

typedef struct RGBStruct
{
  double R;
  double G;
  double B;
}RGB;

/********************************************************************************
 * Object definition and objectlist definition                                  *
 ********************************************************************************/
typedef struct ObjectStruct *ObjPtr;
typedef struct ObjectStruct
{
  char Name[57]; /* Chosen to 57, since the future platform supports 57 chars*/
  
  PointListPtr Points;
  Poly3Ptr Poly3s;
  Poly4Ptr Poly4s;
  TexturePtr Textures;
  MaterialPtr Material;
  
  RGB Color;             /* In layout */
  int ReNo;
  enum style {FULL,HALF,NONE} styletag;

  Point Origin;
  Point Rotation;
  Point Scale;
  
  ObjPtr Children;
  ObjPtr Parent;
  int Number_of_Children;

  int Number_of_Points;
  int Number_of_Poly3s;
  int Number_of_Poly4s;
  int Number_of_Textures;
  int Number_of_Materials;
  int Number_of_BitMaps;
  ObjPtr next;
  ObjPtr prev;
}object;

/********************************************************************************
 * Various definitions                                                          *
 ********************************************************************************/
typedef struct LightStruct *LightPtr;
typedef struct LightStruct
{
  char Name[57];
  int Number;
  RGB Color;                            /* In LayOut */

  float pos_x,pos_y,pos_z;              /* Position */
  float rot_x,rot_y,rot_z;              /* X,Y,Z Rotation factor */

  float amb_r,amb_g,amb_b,amb_a;        /* Ambient Intensity */
  float spec_r,spec_g,spec_b,spec_a;    /* Specular Intensity */
  float diff_r,diff_g,diff_b,diff_a;    /* Diffuseness */

  float Spot_Exponent, Spot_Cutoff;      
  float Const_Attenuation, Linear_Attenuation, Quad_Attenuation;

  enum LightTypes { POINT, SPOT, DIR } light_tag;

  LightPtr next;
  LightPtr prev;
}Light;

typedef struct ButtonMotionStruct 
{
  enum button {N,B1,B2,B3} Tag;
  float x0;
  float y0;

  float x;
  float y;
}ButtonMotion;

