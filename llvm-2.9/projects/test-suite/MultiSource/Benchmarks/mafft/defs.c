#include <stdio.h>
#include "dp.h"
#include "mltaln.h"

int commonAlloc1 = 0;
int commonAlloc2 = 0;
int **commonIP = NULL;
int **commonJP = NULL;

char modelname[100];
int njob, nlenmax;
int amino_n[0x80];
char amino_grp[0x80];
int amino_dis[0x80][0x80];
int amino_disLN[0x80][0x80];
double amino_dis_consweight_multi[0x80][0x80];
int n_dis[26][26];
int n_disFFT[26][26];
float n_dis_consweight_multi[26][26];
char amino[26];
double polarity[20];
double volume[20];
int ribosumdis[37][37];

int ppid;
double thrinter;
double fastathreshold;
int pslocal, ppslocal;
int constraint;
int divpairscore;
int fmodel; // 1-> fmodel 0->default -1->raw
int nblosum; // 45, 50, 62, 80
int kobetsubunkatsu;
int bunkatsu;
int dorp;
int niter;
int contin;
int calledByXced;
int devide;
int scmtd;
int weight;
int utree;
int tbutree;
int refine;
int check;
double cut;
int cooling;
int penalty, ppenalty, penaltyLN;
int RNApenalty, RNAppenalty;
int RNApenalty_ex, RNAppenalty_ex;
int penalty_ex, ppenalty_ex, penalty_exLN;
int penalty_EX, ppenalty_EX;
int penalty_OP, ppenalty_OP;
int RNAthr, RNApthr;
int offset, poffset, offsetLN, offsetFFT;
int scoremtx;
int TMorJTT;
char use_fft;
char force_fft;
int nevermemsave;
int fftscore;
int fftWinSize;
int fftThreshold;
int fftRepeatStop;
int fftNoAnchStop;
int divWinSize;
int divThreshold;
int disp;
int outgap;
char alg;
int cnst;
int mix;
int tbitr;
int tbweight;
int tbrweight;
int disopt;
int pamN;
int checkC;
float geta2;
int treemethod;
int kimuraR;
char *swopt;
int fftkeika;
int score_check;
int makedistmtx;
char *inputfile;
int rnakozo;
char rnaprediction;

char *signalSM;
FILE *prep_g;
FILE *trap_g;
char **seq_g;
char **res_g;

float consweight_multi = 1.0;
float consweight_rna = 0.0;
char RNAscoremtx = 'n';
