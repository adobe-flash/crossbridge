/*
!-------------------------------------------------------------------------!
!                                                                         !
!        N  A  S     P A R A L L E L     B E N C H M A R K S  3.3         !
!                                                                         !
!                      S E R I A L     V E R S I O N                      !
!                                                                         !
!                                   D C                                   !
!                                                                         !
!-------------------------------------------------------------------------!
!                                                                         !
!    DC creates all specifided data-cube views.                           !
!    Refer to NAS Technical Report 03-005 for details.                    !
!    It calculates all groupbys in a top down manner using well known     !
!    heuristics and optimizations.                                        !
!                                                                         !
!    Permission to use, copy, distribute and modify this software         !
!    for any purpose with or without fee is hereby granted.  We           !
!    request, however, that all derived work reference the NAS            !
!    Parallel Benchmarks 3.3. This software is provided "as is"           !
!    without express or implied warranty.                                 !
!                                                                         !
!    Information on NPB 3.3, including the technical report, the          !
!    original specifications, source code, results and information        !
!    on how to submit new results, is available at:                       !
!                                                                         !
!           http://www.nas.nasa.gov/Software/NPB/                         !
!                                                                         !
!    Send comments or suggestions to  npb@nas.nasa.gov                    !
!                                                                         !
!          NAS Parallel Benchmarks Group                                  !
!          NASA Ames Research Center                                      !
!          Mail Stop: T27A-1                                              !
!          Moffett Field, CA   94035-1000                                 !
!                                                                         !
!          E-mail:  npb@nas.nasa.gov                                      !
!          Fax:     (650) 604-3957                                        !
!                                                                         !
!-------------------------------------------------------------------------!
! Author: Michael Frumkin                                                 !
!         Leonid Shabanov                                                 !
!-------------------------------------------------------------------------!
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>

#include "adc.h"
#include "macrodef.h"
#include "npbparams.h"

#ifdef UNIX
#include <sys/types.h>
#include <unistd.h>
#endif

void c_print_results( char   *name,
                      char   clss,
                      int    n1, 
                      int    n2,
                      int    n3,
                      int    niter,
		      char   *optype,
                      int    passed_verification,
                      char   *npbversion);

void initADCpar(ADC_PAR *par);
int ParseParFile(char* parfname, ADC_PAR *par); 
int GenerateADC(ADC_PAR *par);
void ShowADCPar(ADC_PAR *par);
int32 DC(ADC_VIEW_PARS *adcpp);
int Verify(long long int checksum,ADC_VIEW_PARS *adcpp);

#define BlockSize 1024

int main ( int argc, char * argv[] ) 
{
  ADC_PAR *parp;
  ADC_VIEW_PARS *adcpp;
  int32 retCode;

  fprintf(stdout,"\n\n NAS Parallel Benchmarks (NPB3.3-SER) - DC Benchmark\n\n" );
  if(argc!=3){
    fprintf(stdout," No Paramter file. Using compiled defaults\n");
  }
  if(argc>3 || (argc>1 && !isdigit(argv[1][0]))){
    fprintf(stderr,"Usage: <program name> <amount of memory>\n");
    fprintf(stderr,"       <file of parameters>\n");
    fprintf(stderr,"Example: bin/dc.S 1000000 DC/ADC.par\n");
    fprintf(stderr,"The last argument, (a parameter file) can be skipped\n");
    exit(1);
  }

  if(  !(parp = (ADC_PAR*) malloc(sizeof(ADC_PAR)))
     ||!(adcpp = (ADC_VIEW_PARS*) malloc(sizeof(ADC_VIEW_PARS)))){
     PutErrMsg("main: malloc failed")
     exit(1);
  }
  initADCpar(parp);
  parp->clss=CLASS;
  if(argc!=3){
    parp->dim=attrnum;
    parp->tuplenum=input_tuples;    
  }else if( (argc==3)&&(!ParseParFile(argv[2], parp))) {
    PutErrMsg("main.ParseParFile failed")
    exit(1);
  }
  ShowADCPar(parp); 
  if(!GenerateADC(parp)) {
     PutErrMsg("main.GenerateAdc failed")
     exit(1);
  }

  adcpp->ndid = parp->ndid;  
  adcpp->clss = parp->clss;
  adcpp->nd = parp->dim;
  adcpp->nm = parp->mnum;
  adcpp->nTasks = 1;
  if(argc>=2)
    adcpp->memoryLimit = atoi(argv[1]);
  else
    adcpp->memoryLimit = 0;
  if(adcpp->memoryLimit <= 0){
    /* size of rb-tree with tuplenum nodes */
    adcpp->memoryLimit = parp->tuplenum*(50+5*parp->dim); 
    fprintf(stdout,"Estimated rb-tree size = %d \n", adcpp->memoryLimit);
  }
  adcpp->nInputRecs = parp->tuplenum;
  strcpy(adcpp->adcName, parp->filename);
  strcpy(adcpp->adcInpFileName, parp->filename);

  if((retCode=DC(adcpp))) {
     PutErrMsg("main.DC failed")
     fprintf(stderr, "main.ParRun failed: retcode = %d\n", retCode);
     exit(1);
  }

  if(parp)  { free(parp);   parp = 0; }
  if(adcpp) { free(adcpp); adcpp = 0; }
  return 0;
}

int32		 CloseAdcView(ADC_VIEW_CNTL *adccntl);  
int32		 PartitionCube(ADC_VIEW_CNTL *avp);				
ADC_VIEW_CNTL *NewAdcViewCntl(ADC_VIEW_PARS *adcpp, uint32 pnum);
int32		 ComputeGivenGroupbys(ADC_VIEW_CNTL *adccntl);

int32 DC(ADC_VIEW_PARS *adcpp) {
   int32 itsk=0;
   int verified;

   typedef struct { 
      int    verificationFailed;
      uint32 totalViewTuples;
      uint64 totalViewSizesInBytes;
      uint32 totalNumberOfMadeViews;
      uint64 checksum;
   } PAR_VIEW_ST;
   
   PAR_VIEW_ST *pvstp;
   ADC_VIEW_CNTL *adccntlp;

   pvstp = (PAR_VIEW_ST*) malloc(sizeof(PAR_VIEW_ST));
   pvstp->verificationFailed = 0;
   pvstp->totalViewTuples = 0;
   pvstp->totalViewSizesInBytes = 0;
   pvstp->totalNumberOfMadeViews = 0;
   pvstp->checksum = 0;
   
   adccntlp = NewAdcViewCntl(adcpp, itsk);
   if (!adccntlp) { 
      PutErrMsg("ParRun.NewAdcViewCntl: returned NULL")
      return ADC_INTERNAL_ERROR;
   }else{
     if (adccntlp->retCode!=0) {
   	fprintf(stderr, 
   		 "DC.NewAdcViewCntl: return code = %d\n",
   						adccntlp->retCode); 
     }
   }
   if( PartitionCube(adccntlp) ) {
      PutErrMsg("DC.PartitionCube failed");
   }
   if( ComputeGivenGroupbys(adccntlp) ) {
      PutErrMsg("DC.ComputeGivenGroupbys failed");
   }
   pvstp->verificationFailed += adccntlp->verificationFailed;
   if (!adccntlp->verificationFailed) {
     pvstp->totalNumberOfMadeViews += adccntlp->numberOfMadeViews;
     pvstp->totalViewSizesInBytes += adccntlp->totalViewFileSize;
     pvstp->totalViewTuples += adccntlp->totalOfViewRows;
     pvstp->checksum += adccntlp->totchs[0];
   }   
   if(CloseAdcView(adccntlp)) {
     PutErrMsg("ParRun.CloseAdcView: is failed");
     adccntlp->verificationFailed = 1;
   }

   pvstp->verificationFailed=Verify(pvstp->checksum,adcpp);
   verified = (pvstp->verificationFailed == -1)? -1 :
              (pvstp->verificationFailed ==  0)?  1 : 0;

   fprintf(stdout,"\n*** DC Benchmark Results:\n");
   fprintf(stdout," Input Tuples     =         %12d\n", (int) adcpp->nInputRecs);
   fprintf(stdout," Number of Views  =         %12d\n",
           (int) pvstp->totalNumberOfMadeViews);
   fprintf(stdout," Number of Tasks  =         %12d\n", (int) adcpp->nTasks);
   fprintf(stdout," Tuples Generated = %20.0f\n",
           (double) pvstp->totalViewTuples);
   if (pvstp->verificationFailed)
      fprintf(stdout, " Verification failed\n");

   c_print_results("DC",
  		   adcpp->clss,
  		   (int)adcpp->nInputRecs,
                   0,
                   0,
                   1,
  		   "Tuples generated", 
  		   verified,
  		   NPBVERSION); 
   return ADC_OK;
}

long long checksumS=464620213;
long long checksumWlo=434318;
long long checksumWhi=1401796;
long long checksumAlo=178042;
long long checksumAhi=7141688;
long long checksumBlo=700453;
long long checksumBhi=9348365;

int Verify(long long int checksum,ADC_VIEW_PARS *adcpp){
  switch(adcpp->clss){
    case 'S':
      if(checksum==checksumS) return 0;
      break;
    case 'W':
      if(checksum==checksumWlo+1000000*checksumWhi) return 0;
      break;
    case 'A':
      if(checksum==checksumAlo+1000000*checksumAhi) return 0;
      break;
    case 'B':
      if(checksum==checksumBlo+1000000*checksumBhi) return 0;
      break;
    default:
      return -1; /* CLASS U */
  }
  return 1;
}

