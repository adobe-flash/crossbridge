/*-----------------------------------------------------------------------------
 * V.h      V Includes
 *
 * V
 * University of Illinois at Urbana-Champaign
 *
 * History:
 *          - Completed V1.0 August 1994
 *          - Modified for V2.0 October 1995
 *
 * Id: V.h,v 1.1 1994/07/08 05:55:20 develop Exp chess $
 *---------------------------------------------------------------------------*/
#ifndef _V_h_
#define _V_h_
#endif

/* ----------------
 * INCLUDES
 * ---------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

/* ----------------
 * TYPE DEFINITIONS
 * ---------------- */
typedef struct V_struct{
    char            *label;        /* label */
    float           *data;         /* data  */
    int             data_type;     /* data type */
    int             ndim;          /* number of dimensions */
    int             *dim;          /* dimensions of data */
} V_struct;

typedef (*FUNC_PTR)();

typedef struct command {
  char *name;                    /* name of the command */
  FUNC_PTR func;                 /* C function corresponding to the command */
} COMMAND;

#include "V_commands.h"

/* ---------------------
 * MISCELLANEOUS DEFINES
 * --------------------- */

#ifndef M_PI
#define M_PI		 3.14159265358979323846
#endif

#define VERSION          "2.00"
#ifndef PI
#define PI               3.14159265358979323846
#endif
#define TWOPI            6.283185307179586476925286766559
#define REAL             0
#define COMPLEX          1
#define FALSE            0
#ifndef TRUE
#define TRUE             1
#endif
#define FFT_BACK         0
#define FFT_FWRD         1
#define NNEIGHBOR        1
#define LINEAR           2
#ifndef NULL
  #define NULL           0
#endif
#define SYNTAX_ERROR     99
#define HDF_ERROR       -1
#define USAGE            2
#define PLOT_DOT         0.0
#define PLOT_BACKGROUND  250.0
#define NGMAX            255
#define NGMIN            0
#define EPS              0.00000001 
#define GAUSS_MASK       6
#define DGAUSS_MASK      8. * sqrt(2.) 
#define POCS_ITMX        25

/* ---------------------------
 * MANPATH DEFINES
 * --------------------------- */
#if defined(SUN) || defined(SUN4) || defined(IRIS4) || defined(SGI) || defined(IBM) || defined(IBM6000)
#define HASDIRECTMANPATH
#endif

#if defined(HP) || defined(HP9000)
#define NODIRECTMANPATH
#endif

/* ---------------------------
 * C FUNCTION DEFINES
 * --------------------------- */
#if defined(IBM) || defined(IBM6000) || defined(HP) || defined(HP9000) || defined(IRIS4) ||defined(SGI)
#define log2(x) (3.321982810*log10(x))
#endif

/* ---------------------------
 * FORTRAN SUBROUTINES DEFINES
 * --------------------------- */

/* If F77 (instead of g77) is used for IBM and HP */
#if defined(IBMF77) || defined(HPF77)
#define SSCAL      sscal
#define CCOPY      ccopy
#define SCOPY      scopy
#define L_CFT      fourt
#define L_PISHFT1D pishft1d
#define L_PISHFT2D pishft2d
#define L_PISHFT3D pishft3d
#define L_PISHFT4D pishft4d
#define L_ABS      getmag
#define L_PHASE    getphs
#define L_AMPFIT   ampfit
#define F_HSVD     hsvd
#define F_LPSVD    lpsvd
#define L_SYNFID   synfid
#define L_RMDC     rmdc
#define L_GS       gs
#define L_TSLC1    tslc1
#define L_BXAMP    bxamp
#define L_BXIMA    bxima
#define L_ZUFALL   zufall
#define L_ZUFALLI  zufalli
#define L_NORMALEN normalen
#define F_CWK_SLIM cwk_slim
#define F_SLIM     f_slim
#define F_SLIM_CSI csi_comp
#define F_GSCOEF   gscoef
#define F_GMATRIX  gmatrix
#define F_CSIVPNLS csivpnls
#define F_ZSVDC    zsvdc
#define F_CSVDC    csvdc
#define F_POLYFIT  wpolyfit1d

#else

#define SSCAL      sscal_
#define CCOPY      ccopy_
#define SCOPY      scopy_
#define L_CFT      fourt_
#define L_PISHFT1D pishft1d_
#define L_PISHFT2D pishft2d_
#define L_PISHFT3D pishft3d_
#define L_PISHFT4D pishft4d_
#define L_ABS      getmag_
#define L_PHASE    getphs_
#define L_AMPFIT   ampfit_
#define F_HSVD     hsvd_
#define F_LPSVD    lpsvd_
#define L_SYNFID   synfid_
#define L_RMDC     rmdc_
#define L_GS       gs_
#define L_TSLC1    tslc1_
#define L_BXAMP    bxamp_
#define L_BXIMA    bxima_
#define L_ZUFALL   zufall_
#define L_ZUFALLI  zufalli_
#define L_NORMALEN normalen_
#define F_CWK_SLIM cwk_slim_
#define F_SLIM     f_slim_
#define F_SLIM_CSI csi_comp_
#define F_GSCOEF   gscoef_
#define F_GMATRIX  gmatrix_
#define F_CSIVPNLS csivpnls_
#define F_ZSVDC    zsvdc_
#define F_CSVDC    csvdc_
#define F_POLYFIT  wpolyfit1d_
#endif


/* ----------------------------------
 * LIMITS
 * ---------------------------------- */
#define HISTORY_SIZE 50  
#define MAX_STRING       256
#define MAX_COMMAND      256
#define MAX_NDIM          10
#define MAX_REG          50
#define MAX_FLOAT        ((float)3.40282346638528860e+38) 
#define MIN_FLOAT        ((float)1.40129846432481707e-45)

/* ----------------------------------
 * MACHINE TYPE SIZES
 * (MACHINE DEPENDENT)
 * ---------------------------------- */

#define CWS 1
#define FWS 4
#define DWS 8
#define IWS 4

/* ----------------------------------
 * FILE FORMATS
 * ---------------------------------- */
#define FF_HDF                   1
#define FF_ASCII                 2
#define FF_UBYTE                 3     /* RESERVED */    
#define FF_BYTE                  4     /* RESERVED */    
#define FF_LONG                  5     /* RESERVED */    
#define FF_SHORT                 6     /* RESERVED */    
#define FF_INTEGER               7     /* RESERVED */    
#define FF_INTEGER_MSB           8     /* RESERVED */    
#define FF_FLOAT                 9     /* RESERVED */    
#define FF_DOUBLE               10     /* RESERVED */    
#define FF_CFLOAT               11     /* RESERVED */    
#define FF_CSHORT               12     /* RESERVED */    
#define FF_CLONG                13     /* RESERVED */    
#define FF_VAXSHORT             14     /* RESERVED */    
#define FF_HDF_RASTER_8         15     /* RESERVED */    
#define FF_HDF_RASTER_8_WLUT    16     /* RESERVED */    
#define FF_HDF_RASTER_24        17     /* RESERVED */    
#define FF_PGM                  18
#define FF_SISCO                19

/* ----------------------------------
 * WAVELET TRANSFORM DEFINES
 * ---------------------------------- */
#define   HAAR       1    /* HAAR   wavelet basis  */
#define   DAUB4      2    /* DAUB4  wavelet basis  */
#define   DAUB6      3    /* DAUB6  wavelet basis  */
#define   DAUB8      4    /* DAUB8  wavelet basis  */
#define   DAUB10     5    /* DAUB10 wavelet basis  */
#define   DAUB12     6    /* DAUB12 wavelet basis  */
#define   DAUB14     7    /* DAUB14 wavelet basis  */
#define   DAUB16     8    /* DAUB16 wavelet basis  */
#define   DAUB18     9    /* DAUB18 wavelet basis  */
#define   DAUB20     10   /* DAUB20 wavelet basis  */
#define   BO79       20   /* 7-9 BIORHOGONAL basis */
#define   DWT_FWRD   0    /* Forward transform     */
#define   DWT_BACK   1    /* Inverse transform     */

/* ----------------------------------
 * INLINE FUNCTIONS
 * ---------------------------------- */

#define abs(x)                   ((x>0)?(x):(-(x)))
#define I(DD,x,y,width)          DD[((x)*width)+y]
#define MOD(x, y)                ( sqrt(x*x + y*y) )
#define ZERO(x)                  ( (x<= EPS) && (x>= -EPS) )
#define GEPS(x, y)               ( (x > (y+EPS)) )

#define MAG2(x,y)                (double)sqrt((double)((x)*(x)+(y)*(y)))

