#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

/* Simple raytracer
 * ----------------
 * Features reflections, anti-aliasing
 * and soft-shadows.
 * 
 * Written by Marcus Geelnard, benchmarkified by
 * Peter Rundberg, biff@ce.chalmers.se
 */

#define WIDTH  640
#define HEIGHT 480
#define EPSILON (1e-5)  /* Very small value, used for coordinate-comparsions */
#define MAXT (1e5)      /* Maximum t-distance for an intersection-point */
#define MAXREC 6        /* Maximum amount of recursions (reflection etc.) */
/* #define DISTRIB 12 */      /* Amount of distributed rays per "virtual" ray */
int DISTRIB;
#define DISTLEVELS 3    /* How deep in the recursion-tree to allow distribution */

typedef unsigned char UBYTE;
typedef struct { double x,y,z; } VECTOR;

UBYTE memory[3*WIDTH*HEIGHT];

typedef struct {
    VECTOR  color;                      /* Object color (r,g,b) */
    double  diffuse;                    /* Diffuse reflection (0-1) */
    double  reflect;                    /* Relefction (0-1) */
    double  roughness;                  /* How rough the reflection is (0=very sharp) */
} TEXTURE;

typedef struct {
    VECTOR  pos;                        /* Position (x,y,z) */
    double  r;                          /* Radius (or size) */
    TEXTURE t;                          /* Texture */
} OBJ;

/*
 *  Objects ( = spheres ). Only one sphere. Add more if you like :)
 */

OBJ objs[] = {
    /* Object 1 */
    {
        { 0, 4, 1.0 }, 1,
        {
            { 1.0, 0.4, 0.0 },
            0.4,
            0.8,
            0.02
        }
    },
    /* Object 2 */
    {
        { -1, 3, 0.4 }, 0.4,
        {
            { 0.5, 0.3, 1.0 },
            0.5,
            0.9,
            0.01
        }
    },
    /* Object 3 */
    {
        { -0.3, 1, 0.4 }, 0.4,
        {
            { 0.1, 0.95, 0.2 },
            0.6,
            0.8,
            0.01
        }
    },
    /* Object 4 */
    {
        { 1.0, 2, 0.4 }, 0.4,
        {
            { 0.86, 0.83, 0 },
            0.7,
            0.6,
            0.01
        }
    }
};

#define NUMOBJS 4


/*
 * Ground position (z-pos), and textures (tiled)
 */

double Groundpos = 0.0;
TEXTURE Groundtxt[2] = {
    {
        { 0.0, 0.1, 0.5 },
        0.8,
        0.44,
        0.02
    },
    {
        { 0.6, 1.0, 0.5 },
        0.8,
        0.44,
        0.01
    },
};


/*
 * Only one light-source is supported (and it's white).
 */

VECTOR Lightpos = {-3.0, 1.0, 5.0};
double Lightr   = 0.4;              /* Light-radius (for soft shadows) */


/*
 * The camera position (x,y,z), and orientation.
 */

VECTOR Camerapos   = {1.5, -1.4, 1.2};
VECTOR Cameraright = {3.0, 1.0, 0.0};
VECTOR Cameradir   = {-1.0, 3.0, 0.0};
VECTOR Cameraup    = {0.0, 0.0, 2.3717};


/*
 * Ambient lighting (0.0-1.0)
 */

double Ambient = 0.3;


/*
 * Skycolors (Skycolor[0] = horizon, Skycolor[1] = zenit )
 */

VECTOR Skycolor[2] = { { 0.5, 0.3, 0.7 }, { 0.0, 0.0, 0.2 } };


/**************************************************************************
 *
 *  Helpers (geometrical etc).
 *
 **************************************************************************/

unsigned long rnd = 0x52462467L;

static double Jitter( void )
{
    rnd = (1103515245L*rnd + 12345L) & 0x7fffffffL;
    return( 1.0-((double)rnd/(double)0x3fffffff) );
}


static void ReflectVector( VECTOR *v2, VECTOR *v1, VECTOR *n )
{
    double  a, b;
 
    b = n->x*n->x + n->y*n->y + n->z*n->z;      /* b = |n|^2 */
    a = v1->x*n->x + v1->y*n->y + v1->z*n->z;   /* a = v1·n  */
    a = -2.0 * a / b;                           /* a = -2*(v1·n)/|n|^2 */
    v2->x = v1->x + a*n->x;                     /* v2 = v1 + n*a */
    v2->y = v1->y + a*n->y;
    v2->z = v1->z + a*n->z;
}


static double VectorLength( VECTOR *v )
{
    return( sqrt( v->x*v->x + v->y*v->y + v->z*v->z ) );
}


static void ScaleVector( VECTOR *v, double s )
{
    v->x *= s; v->y *= s; v->z *= s;
}


static void DistribVector( VECTOR *d, VECTOR *n, double sa, double sb )
{
    VECTOR  a, b;
    double  nl;

    if( fabs( n->z ) > EPSILON ) {
        a.x = n->y*n->z; a.y = -n->x*n->z; a.z = 0.0;
        b.x = a.y*n->z; b.y = -a.x*n->z; b.z = a.x*n->y - a.y*n->x;
    } else {
        a.x = n->y; a.y = -n->x; a.z = 0.0;
        b.x = b.y = 0.0; b.z = 1.0;
    }
    nl = VectorLength( n );
    ScaleVector( &a, sa*(nl/VectorLength( &a ))*Jitter() );
    ScaleVector( &b, sb*(nl/VectorLength( &b ))*Jitter() );
    d->x = a.x+b.x; d->y = a.y+b.y; d->z = a.z+b.z;
}


/**************************************************************************
 *
 *  Object intersection calculation routines.
 *
 **************************************************************************/

static double IntersectObjs( VECTOR *LinP, VECTOR *LinD,
                      VECTOR *Pnt, VECTOR *Norm, TEXTURE **txt )
{
    int     objn, tilenum;
    double  t, ttmp, A, B, C;
    VECTOR  Pos;

    t = -1.0;

    /* Try intersection with ground plane first */
    if( fabs(LinD->z) > EPSILON ) {
        ttmp = (Groundpos - LinP->z)/LinD->z;
        if( ( ttmp > EPSILON ) && ( ttmp < MAXT ) ) {
            t = ttmp;
            Pnt->x = LinP->x + LinD->x*t;   /* Calculate intersection point */
            Pnt->y = LinP->y + LinD->y*t;
            Pnt->z = LinP->z + LinD->z*t;
            Norm->x = 0.0;                  /* Surface normal (always up) */
            Norm->y = 0.0;
            Norm->z = 1.0;
            tilenum = ( ((int)(Pnt->x+50000.0))+((int)(Pnt->y+50000.0)) ) & 1;
            *txt = & Groundtxt[ tilenum ];
        }
    }

    /* Get closest intersection (if any) */
    for( objn = 0; objn < NUMOBJS; objn++ ) {
        Pos = objs[objn].pos;
        Pos.x -= LinP->x;                 /* Translate object into "line-space" */
        Pos.y -= LinP->y;
        Pos.z -= LinP->z;
        A = 1.0 / (LinD->x*LinD->x + LinD->y*LinD->y + LinD->z*LinD->z);
        B = (Pos.x*LinD->x + Pos.y*LinD->y + Pos.z*LinD->z) * A;
        C = (objs[objn].r*objs[objn].r - Pos.x*Pos.x - Pos.y*Pos.y - Pos.z*Pos.z) * A;
        if( (A = C + B*B) > 0.0 ) {       /* ...else no hit */
            A = sqrt(A);
            if( (ttmp = B - A) < EPSILON ) ttmp = B + A;
            if( (EPSILON<ttmp) && ( (ttmp<t)||(t<0.0) ) ) {
        	t = ttmp;
        	Pnt->x = LinD->x*t;       /* Calculate intersection point */
        	Pnt->y = LinD->y*t;
        	Pnt->z = LinD->z*t;
        	Norm->x = Pnt->x-Pos.x;   /* Calcualate surface normal */
        	Norm->y = Pnt->y-Pos.y;
        	Norm->z = Pnt->z-Pos.z;
        	Pnt->x += LinP->x;        /* Translate object back to "true-space" */
        	Pnt->y += LinP->y;
        	Pnt->z += LinP->z;
        	*txt = &objs[objn].t;     /* Get surface properties */
            }
        }
    }

    return( t );
}


/**************************************************************************
 *
 *  Line-tracer routine (works recursively).
 *
 **************************************************************************/

static void TraceLine( VECTOR *LinP, VECTOR *LinD, VECTOR *Color, int reccount )
{
    VECTOR  Pnt, Norm, LDir, NewDir, NewDir2, TmpCol, TmpCol2;
    VECTOR  TmpPnt, TmpNorm, D;
    double  t, A, cosfi;
    TEXTURE *txt, *tmptxt;
    int     i, shadowcount, usedist;

    Color->x = Color->y = Color->z = 0.0;

    if( reccount > 0 ) {
        /* Only use distributed tracing in higher nodes of the recursion tree */
        usedist = ( (MAXREC-reccount) < DISTLEVELS ) ? 1 : 0;

        /* Try intersection with objects */
        t = IntersectObjs( LinP, LinD, &Pnt, &Norm, &txt );

        /* Get light-intensity in intersection-point (store in cosfi) */
        if( t > EPSILON ) {
            LDir.x = Lightpos.x-Pnt.x;       /* Get line to light from surface */
            LDir.y = Lightpos.y-Pnt.y;
            LDir.z = Lightpos.z-Pnt.z;
            cosfi = LDir.x*Norm.x + LDir.y*Norm.y + LDir.z*Norm.z;
            if(cosfi > 0.0) {    /* If angle between lightline and normal < PI/2 */
                shadowcount = 0;
                if( usedist ) {
                    A = Lightr / VectorLength( &LDir );
                    for( i = 0; i < DISTRIB; i++ ) {
                        DistribVector( &D, &LDir, A, A );
                        NewDir = LDir;
                        NewDir.x += D.x; NewDir.y += D.y; NewDir.z += D.z;
                        /* Check for shadows (ignore hit info, may be used though) */
                        t = IntersectObjs( &Pnt, &NewDir, &TmpPnt, &TmpNorm, &tmptxt );
                        if( ( t < EPSILON ) || ( t > 1.0 ) ) shadowcount++;
                    }
                } else {
                    t = IntersectObjs( &Pnt, &LDir, &TmpPnt, &TmpNorm, &tmptxt );
                    if( ( t < EPSILON ) || ( t > 1.0 ) ) shadowcount = DISTRIB;
                }
                if( shadowcount > 0 ) {
                    A = Norm.x*Norm.x + Norm.y*Norm.y + Norm.z*Norm.z;
                    A *= LDir.x*LDir.x + LDir.y*LDir.y + LDir.z*LDir.z;
                    cosfi = (cosfi/sqrt(A))*txt->diffuse*(double)shadowcount/DISTRIB;
                } else {
                    cosfi = 0.0;
                }
            } else {
                cosfi = 0.0;
            }
            Color->x = txt->color.x*(Ambient+cosfi);
            Color->y = txt->color.y*(Ambient+cosfi);
            Color->z = txt->color.z*(Ambient+cosfi);
            if( txt->reflect > EPSILON ) {
                ReflectVector( &NewDir, LinD, &Norm );
                TmpCol.x = TmpCol.y = TmpCol.z = 0.0;
                if( usedist && ( txt->roughness > EPSILON ) ) {
                    for( i = 0; i < DISTRIB; i++ ) {
                        DistribVector( &D, &NewDir, txt->roughness, txt->roughness );
                        NewDir2 = NewDir;
                        NewDir2.x += D.x; NewDir2.y += D.y; NewDir2.z += D.z;
                        TraceLine( &Pnt, &NewDir2, &TmpCol2, reccount-1 );
                        TmpCol.x += TmpCol2.x;
                        TmpCol.y += TmpCol2.y;
                        TmpCol.z += TmpCol2.z;
                    }
                    ScaleVector( &TmpCol, 1.0/DISTRIB );
                } else {
                    TraceLine( &Pnt, &NewDir, &TmpCol, reccount-1 );
                }
                Color->x += TmpCol.x * txt->reflect;
                Color->y += TmpCol.y * txt->reflect;
                Color->z += TmpCol.z * txt->reflect;
            }
        } else {
            /* Get sky-color (interpolate between horizon and zenit) */
            A = sqrt( LinD->x*LinD->x + LinD->y*LinD->y );
            if( A > 0.0 ) A = atan( fabs( LinD->z ) / A )*0.63661977;
            else A = 1.0;
            Color->x = Skycolor[1].x*A + Skycolor[0].x*(1.0-A);
            Color->y = Skycolor[1].y*A + Skycolor[0].y*(1.0-A);
            Color->z = Skycolor[1].z*A + Skycolor[0].z*(1.0-A);
        }

        /* Make sure that the color does not exceed the maximum level */
        if(Color->x > 1.0) Color->x = 1.0;
        if(Color->y > 1.0) Color->y = 1.0;
        if(Color->z > 1.0) Color->z = 1.0;
    }
}

static void TraceScene(void)
{
  VECTOR  PixColor, Col, LinD, Scale;
  VECTOR  LinD2, D;
  int     sx, sy, i;

  Scale.y = 1.0;
  for( sy = 0; sy < HEIGHT; sy++ ) {
    Scale.z = ((double)(HEIGHT/2-sy))/(double)HEIGHT;
    for( sx = 0; sx < WIDTH; sx++ ) {
      Scale.x = ((double)(sx-WIDTH/2))/(double)WIDTH;
      
      /* Calculate line-direction (from camera-center through a pixel) */
      LinD.x = Cameraright.x*Scale.x + Cameradir.x*Scale.y + Cameraup.x*Scale.z;
      LinD.y = Cameraright.y*Scale.x + Cameradir.y*Scale.y + Cameraup.y*Scale.z;
      LinD.z = Cameraright.z*Scale.x + Cameradir.z*Scale.y + Cameraup.z*Scale.z;
      
      /* Get color for pixel */
#if (DISTLEVELS > 0)
      PixColor.x = PixColor.y = PixColor.z = 0.0;
      for( i = 0; i < DISTRIB; i++ ) {
	DistribVector( &D, &LinD, 0.5/(double)WIDTH, 0.5/(double)HEIGHT );
	LinD2 = LinD; LinD2.x += D.x; LinD2.y += D.y; LinD2.z += D.z;
	TraceLine( &Camerapos, &LinD2, &Col, MAXREC );
	PixColor.x += Col.x;
	PixColor.y += Col.y;
	PixColor.z += Col.z;
      }
      ScaleVector( &PixColor, 1.0/DISTRIB );
#else
      TraceLine( &Camerapos, &LinD, &PixColor, MAXREC );
#endif
  
      memory[3*(sx+sy*WIDTH)]=(UBYTE)(PixColor.x*255.0);
      memory[3*(sx+sy*WIDTH)+1]=(UBYTE)(PixColor.y*255.0);
      memory[3*(sx+sy*WIDTH)+2]=(UBYTE)(PixColor.z*255.0);
    }
  }
}


/**************************************************************************
 *
 *  main()  - Camera emulation and picture output to stdout.
 *
 **************************************************************************/

int main(int c, char *v[])
{
  int i;

  FILE *in_fp;

  fprintf(stderr,"Compile date: %s\n", COMPDATE);
  fprintf(stderr,"Compiler switches: %s\n", CFLAGS);

  in_fp=fopen(v[1],"r");
  if (!in_fp) {
    printf("ERROR: Could not open indata file\n");
    exit(1);
  }

  fscanf(in_fp,"%d",&DISTRIB);
  fclose(in_fp);
  /* End of Benchmark stuff */


  /* Write PPM header to stdout */
  fprintf( stdout, "P6" ); fputc( 10, stdout );
  fprintf( stdout, "%d %d", WIDTH, HEIGHT ); fputc( 10, stdout );
  fprintf( stdout, "255" ); fputc( 10, stdout );

  /***...calculate image...***/
  TraceScene();

  /***...write image to stdout...***/
  for (i=0 ; i<3*WIDTH*HEIGHT ; ) {
    fputc( memory[i++]&~1, stdout);
    fputc( memory[i++]&~1, stdout);
    fputc( memory[i++]&~1, stdout);
  }

  return 0; /***...ANSI C wants main to return an int...***/
}
