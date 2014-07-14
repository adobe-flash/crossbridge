/*BHEADER****************************************************************
 * (c) 2006   The Regents of the University of California               *
 *                                                                      *
 * See the file COPYRIGHT_and_DISCLAIMER for a complete copyright       *
 * notice and disclaimer.                                               *
 *                                                                      *
 *EHEADER****************************************************************/

//-----------------
//   loop bounds
//      the following information is provided by Mike
//      the input format is
//         kmin kmax jmin jmax imin imax kp   jp
//           where [i|j|k]min = 2
//                 [i|j|k]max = 2 + NZONES_along_i
//                 jp         = 2 + imax
//                 kp         = jp * (jmax + 2)
//         e.g for NZONES_along_i = 25
//         2    27   2    27   2    27   841  29
//-----------------
int kmin;
int kmax;
int jmin;
int jmax;  
int imin;
int imax;
int kp;
int jp;

int i_lb;
int i_ub;
int x_size;


//-----------------
//  copy from temp.h
//    with deletion of some unused data structures.
//-----------------
typedef struct Domain_s {
   int gblk   ;	
   int type   ;
   int imin   ;	
   int jmin   ; 
   int kmin   ; 
   int imax   ; 
   int jmax   ;
   int kmax   ; 
   int nnalls ;
   int nnodes ;
   int namix  ;
   int mixmax ;
   int nmixcnt;
   int imaxmix;
   int nmixzn ;
   int nzones ;	
   int namixgx;
   int jp     ;
   int kp     ;
} Domain_t ;

typedef struct {
   double *dbl ;
   double *dbc ;
   double *dbr ;
   double *dcl ;
   double *dcc ;
   double *dcr ;
   double *dfl ;
   double *dfc ;
   double *dfr ;
   double *cbl ;
   double *cbc ;
   double *cbr ;
   double *ccl ;
   double *ccc ;
   double *ccr ;
   double *cfl ;
   double *cfc ;
   double *cfr ;
   double *ubl ;
   double *ubc ;
   double *ubr ;
   double *ucl ;
   double *ucc ;
   double *ucr ;
   double *ufl ;
   double *ufc ;
   double *ufr ;
} RadiationData_t ;

