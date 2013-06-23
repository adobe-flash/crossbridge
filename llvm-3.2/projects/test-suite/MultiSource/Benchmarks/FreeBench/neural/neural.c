/* (C) 1996, 2001 Fredrik Warg
 *
 * CHANGELOG
 * 2001-02-06: This program was originally an assignment in a course in
 * neural networks, all the way back in 1996. It has been modified for use
 * with FreeBench.
 *
 * 1996-09-29:
 * This program stores letters represented as x*y bit patterns in a Hopfield
 * network using Hebbian/Delta learning. The program can be trained with all letters
 * at once or step-by-step so that the progress can be studied closely. Functions
 * for unlearning using random vectors and for creating generators for the
 * stored patterns using the clamping method are also available. For the function
 * that find the closest stable state for a probe vector, both bipolar and
 * continuous versions are available. The program uses a simple menu system for
 * manouvering. The letters are read from the file 'w.txt'.
 *
 * The program has no error-detection code. Erroneus input or failure to find
 * the input file will create undefined output. So will using the functions
 * in the menu in the wrong order or using a wrongly formatted input file :)
 */

/* include files */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define BENCHMARK /* Run the program as a benchmark */

/* flags for normal or complement hamming distance computation mode */
#define MODE_NORMAL 1
#define MODE_COMPLEMENT -1

/* flags for all at once or step by step T-matrix computation mode */
#define MODE_ALL 1
#define MODE_TRACE 2

/* flags for continuous or two-valued neuron mode */
#define MODE_CONT 1
#define MODE_BIN 2

/* Parametrizing the NN */

/*
#define NNWIDTH 30
#define NNHEIGHT 42
#define NNTOT 1260
#define NUMPATS 10
*/

int NNWIDTH;
int NNHEIGHT;
int NNTOT;
int NUMPATS;

/* Typedefs */
typedef enum {WFALSE, WTRUE} wbool;
typedef float real;

/* Global variables */

/*
char vnames[NUMPATS];
int vectors[NUMPATS][NNTOT], newvectors[NUMPATS][NNTOT], generators[NUMPATS][NNTOT];
real Tmatrix[NNTOT][NNTOT];
*/

char *vnames;
int *stored;
real **Tmatrix;
int **vectors, **newvectors, **generators;
int nmode=MODE_BIN;
unsigned long randnum;


/* Function Declarations  */
static int hamming(int *ny, int *orig, int mode);
static void checkham ();
static void generateT (int mode);
static void delta(real n);
static int run (signed int *source, signed int *dest);
static void readvector (FILE *fp);
static void storecheck ();
static void printT();
static void printV(int vector, signed int *vect);
static void unlearn(int seed, int iter);
static int runcont(signed int *source, signed int *dest);
static void mysrand(unsigned long seed);
static real myrand();
static double myexp(double in);

#ifndef BENCHMARK
static void printvec (int vecs[][]);
static void makegenerators(int n);
static void usegenerators(int n);
#endif

/* The main function types the menu and calls the appropriate function requested by the user
 * until the user types 'q' to quit the program. */
int main (int c, char *v[])
{
  FILE *fp;
  char indata[100];
  int i;

  fprintf(stderr,"Compile date: %s\n", COMPDATE);
  fprintf(stderr,"Compiler switches: %s\n", CFLAGS);
  
  if (c!=2) { 
    fprintf(stderr,"Wrong number of arguments, 1 needed, %d specified.\n",c-1); 
    fprintf(stderr,"USAGE: %s <datafile>\n",v[0]);
    exit(1); 
  }
   
  fp=fopen(v[1], "r");
  if (fp==NULL) {
    fprintf(stderr,"ABORT: Could not read datafile %s\n",v[1]);
    exit(1); 
  }
  
  /* Size of NeuralNet specifed in datafile. */
  fgets(indata,99,fp);
  NNWIDTH  = atoi(indata);
  fgets(indata,99,fp);
  NNHEIGHT  = atoi(indata);
  fgets(indata,99,fp);
  NUMPATS  = atoi(indata);
  NNTOT    = NNWIDTH*NNHEIGHT;

  printf("Matrix size is %dx%d\n",NNWIDTH,NNHEIGHT);
  
  /* Allocating lots of space... */

  vnames = (char *)malloc(sizeof(char)*NUMPATS);
  stored = (int *)malloc(sizeof(int)*NUMPATS);
  if (!vnames || !stored) {
    fprintf(stderr,"ABORT: Out of memory\n");
    exit(1);
  }
  
  Tmatrix = (real **)malloc(sizeof(real *)*NNTOT);
  if (!Tmatrix) {
    fprintf(stderr,"ABORT: Out of memory\n");
    exit(1);
  } 
  for (i=0; i<NNTOT; i++) {
    Tmatrix[i] = (real *)malloc(sizeof(real)*NNTOT);
    if (!Tmatrix[i]) {
      fprintf(stderr,"ABORT: Out of memory\n");
      exit(1);
    } 
  }
  
  vectors = (int **)malloc(sizeof(int *)*NUMPATS);
  newvectors = (int **)malloc(sizeof(int *)*NUMPATS);
  generators = (int **)malloc(sizeof(int *)*NUMPATS);
  if (!vectors || !newvectors || !generators) {
    fprintf(stderr,"ABORT: Out of memory\n");
    exit(1);
  }    
  for (i=0; i<NUMPATS; i++) {
    vectors[i] = (int *)malloc(sizeof(int)*NNTOT);
    newvectors[i] = (int *)malloc(sizeof(int)*NNTOT);
    generators[i] = (int *)malloc(sizeof(int)*NNTOT);
    if (!vectors[i] || !newvectors[i] || !generators[i]) {
      fprintf(stderr,"ABORT: Out of memory\n");
      exit(1);
    } 
  }  

  /* Do some operations (This is for FreeBench) */
  readvector(fp);     /* read from file */
  printf("Checking hamming distances...\n");
  checkham();               /* check hamming distances */
  printf("Generating T matrix...\n");
  generateT(MODE_ALL);      /* generate t matrix */

#if 0
  printf("Store check...\n");
  storecheck();             /* check if vectors were stored */

  printf("Applying unlearning vectors...\n");
  unlearn(10,15);
  storecheck();         /* check if vectors were stored */
#endif

  nmode=MODE_CONT;      /* set continuous neuron mode */
  printf("Delta learning...\n");
  delta(0.5);           /* try to learn vectors using delta learning */
  printf("Store check...\n");
  storecheck();         /* check if vectors were stored */
   

  /* original menu system removed
   char option;

   printf("\nNeural Computing - assignment 1\n\n");
   do {
      printf("\nr - read vectors from file and check hamming distances\np - print original vectors");
      printf("\nt - generate T-matrix\ns - generate T-matrix step-by-step");
      printf("\nc - check if vectors are stored\nu - unlearning");
      printf("\ng - compute generators for stored vectors and use the generators to print original vectors");
      printf("\nm - print T-matrix\nz - set continuous neuron mode\nx - set two-valued neuron mode (default)");
      printf("\nq - quit program\nOption > ");
      option=getchar();
      getchar();
      switch(option) {
         case 'r' :
            readvector();
            checkham();
            break;
         case 'p' :
            printvec(vectors);
            break;
         case 't' :
            generateT(MODE_ALL);
            break;
         case 's' :
            generateT(MODE_TRACE);
            break;
         case 'c' :
            storecheck();
            break;
         case 'u' :
            unlearn();
            break;
         case 'g' :
            makegenerators();
            usegenerators();
            break;
         case 'm' :
            printT();
            break;
         case 'z' :
            nmode=MODE_CONT;
            printf("Continuous meurons used!");
            break;
         case 'x' :
            nmode=MODE_BIN;
            printf("Two-valued neurons used");
            break;
         }
   } while(option != 'q');
  */

  return 0;
} /* main */


/* Computes the hamming distance between ny and orig, two 35-bit vectors.
 * Mode is one of MODE_NORMAL or MODE_COMPLEMENT. The latter computes the
 * hamming distence between ny and the complement vector of orig. */ 
static int hamming(signed int *ny, signed int *orig, int mode)
{
   int hd=0, neuron;

   for(neuron=0; neuron<NNTOT; neuron++)
      if(ny[neuron] != (orig[neuron]*mode))
         hd++;
   return hd;
} /* hamming */


/* Checks the hamming distance between all vectors and complements of the global
 * ten-letter storage variable 'vectors'. If the hamming distance
 * is less than 2 the output is highlighted with square brackets because a hamming
 * distance of <2 is not allowed and the original vectors will have to be changed. */
static void checkham()
{
   int vec, comp, hd;
   int hamwarn=0;

   /* printf("\nHamming distances for original vectors:\n"); */
   for(vec=0; vec<NUMPATS; vec++) {
      for(comp=(vec+1); comp<NUMPATS; comp++) {
         if((hd=hamming(vectors[vec], vectors[comp], MODE_NORMAL))<2) 
	   /* printf ("[!!%c-%c->%d!!] ", vnames[vec], vnames[comp], hd); */ hamwarn++;
         else
           /* printf ("%c-%c->%d ",vnames[vec], vnames[comp], hd) */ ;
         if((hd=hamming(vectors[vec], vectors[comp], MODE_COMPLEMENT))<2) 
	   /* printf ("[!!%c-%c'->%d!!] ", vnames[vec], vnames[comp], hd); */ hamwarn++;
         else
           /* printf ("%c-%c'->%d ",vnames[vec], vnames[comp], hd)*/ ;
      } /* for */
   } /* for */
   /* printf("\n"); */
   if (hamwarn)
     printf("WARNING: %d vectors have a hamming distance <2, please modify input vectors!\n",hamwarn);

} /* checkham */


/* Generates a T-matrix with Hebbian learning. Mode can be MODE_ALL or MODE_TRACE.
 * MODE_ALL trains all 10 vectors at once and MODE_TRACE views the current
 * learning status between each vector. */
static void generateT(int mode)
{
   int row, col, vec;
   char option='0';

   for(row=0; row<NNTOT; row++) 
      for(col=0; col<NNTOT; col++) 
         Tmatrix[row][col]=0.0;

   for(vec=0; vec<10; vec++) {
      for(row=0; row<NNTOT; row++) {
         for(col=0; col<NNTOT; col++) {
            if(row==col)
               Tmatrix[row][col]=0.0;
            else
               Tmatrix[row][col]+=vectors[vec][row]*vectors[vec][col];
         } /* for */
      } /* for */
      if (mode==MODE_TRACE) {
         storecheck();
         printf("\nc-cont, b-break: ");
         option=getchar();
         getchar();
      } /* if */
      if(option=='b')
         break;
   } /* for */

} /* generateT */


/* Trains the T-matrix with Delta learning. The learning rate, c, can be
 * altered in order to study the effects of different learning rates. */
static void delta (real n)
{
  int vec, row, col, neuron;
  real *tempvecC; /* n=0.5 */
  wbool status;

  tempvecC=(real *)malloc(NNTOT*sizeof(real));
  if (!tempvecC) {
    fprintf(stderr,"ABORT: Out of memory\n");
    exit(1);
  }
  /* Set learning rate
  if (nn==-1.0) {
    printf("\nChoose value n: ");
    scanf("%f", &n);
    getchar();
  */
  
  do {
    status=WTRUE;
    for(vec=0; vec<NUMPATS; ++vec) {
      
      if(nmode==MODE_BIN)
	run(vectors[vec], newvectors[vec]);
      else
	runcont(vectors[vec], newvectors[vec]);
      
      for(neuron=0; neuron<NNTOT; ++neuron)
	if((tempvecC[neuron]=(real)(vectors[vec][neuron]-newvectors[vec][neuron])*n) != 0.0)
	  status=WFALSE;
      
      for(row=0; row<NNTOT; ++row) {
	for(col=0; col<NNTOT; ++col) {
	  if(row==col)
	    Tmatrix[row][col]=0.0;
	  else
	    Tmatrix[row][col]+=tempvecC[row]*(real)(vectors[vec][col]);
	} /* for */
      } /* for */
      
    } /* for */
  } while (!status);
  
} /* delta */


/* Updates the vector in source with asynchronous updating and places the result in dest.
 * This function uses the sgn function for the updating. The number of iterations needed
 * to reach a stable state is returned. */
static int run (signed int *source, signed int *dest)
{
   signed int neuron, thesum, row, max=0, *tempvecA, *tempvecB;
   wbool stable=WFALSE;

   tempvecA=(int *)malloc(NNTOT*sizeof(int));
   tempvecB=(int *)malloc(NNTOT*sizeof(int));

   if (!tempvecA || !tempvecB) {
     fprintf(stderr,"ABORT: Out of memory\n");
     exit(1);
   }

   for(neuron=0; neuron<NNTOT; neuron++) 
      tempvecA[neuron]=source[neuron];

   while((!stable) && (max<500)) {
      for(row=0; row<NNTOT; row++) {
         thesum=0;
         for(neuron=0; neuron<NNTOT; neuron++) 
            thesum+=Tmatrix[row][neuron]*tempvecA[neuron];
         tempvecB[row]= (thesum>=0) ? 1 : -1;
      } /* for */
      for(neuron=0; neuron<NNTOT; neuron++)
         if(tempvecA[neuron]==0)
            tempvecA[neuron]=tempvecB[neuron];
      if(hamming(tempvecB, tempvecA, MODE_NORMAL)==0)
         stable=WTRUE;
      else {
         neuron=0;
         while((neuron<NNTOT) && (tempvecB[neuron]==tempvecA[neuron]))
            neuron++;
         tempvecA[neuron]=tempvecB[neuron];
      } /* else */
      max++;
   } /* while */ 

   if(max==500)
      printf("Warning! No stable state reached after 500 iterations, aborting!");

   for(neuron=0; neuron<NNTOT; neuron++)
      dest[neuron]=tempvecA[neuron];

   return max;

} /* run */


/* Reads NUMPATS bit patterns from the file 'w.txt' and places them in 'vectors'. The actual
 * letters are stored in 'vnames'. The patterns are stored as -1 (.) and +1 (X). */
static void readvector (FILE *fp)
{
  char *srow;
  int vec, row, column, vpos;

  srow = (char *)malloc((NNWIDTH+2)*sizeof(char));
  if (!srow) {
     fprintf(stderr,"ABORT: Out of memory\n");
     exit(1);
  }

  for(vec=0; vec<NUMPATS; vec++) {
    vpos=0;
    fscanf(fp, "%s", srow);
    vnames[vec]=srow[0];
    for(row=0; row<NNHEIGHT; row++) {
      fscanf(fp, "%s", srow);
      for(column=0; column<NNWIDTH; column++) {
	vectors[vec][vpos] = (srow[column]=='X') ? 1 : -1;
	vpos++;
      } /* for */
    } /* for */
  } /* for */
  fclose(fp);
  
  printf("Vectors read from file!\n");
} /* readvector */


/* Checks all original vectors to see if they are stored. All vectors are written on the
 * screen. If a vector is stored the word 'Stored' appears beneath the typed pattern,
 * otherwise the hamming distance between the original and computed vector is displayed.
 * The number of iterations needed to reach a stable state is displayed below the "hamming
 * distance or stored" line. */
static void storecheck ()
{
  int vec, hd, *iter;
   
  iter = (int *)malloc((NUMPATS)*sizeof(int));
  if (!iter) {
     fprintf(stderr,"ABORT: Out of memory\n");
     exit(1);
  }

  for(vec=0; vec<NUMPATS; vec++) 
    if(nmode==MODE_BIN)
      iter[vec]=run(vectors[vec], newvectors[vec]);
    else
      iter[vec]=runcont(vectors[vec], newvectors[vec]);
  
  /* putchar('\n');
     printvec(newvectors); */
  
  for(vec=0; vec<NUMPATS; vec++) {
    if((hd=hamming(vectors[vec], newvectors[vec], MODE_NORMAL))==0) {
      stored[vec]=1;
      printf("Pattern %d stored.\n", vec);
    }
    else {
      stored[vec]=0;
      printf("Pattern %d: hamming distance=%-.2d.\n",vec,hd);
    }
  } /* for */

  /*
  printf("\nNumber of iterations used:\n");
  for (vec=0; vec<NUMPATS; vec++)
    printf("%.5d  ", iter[vec]); 
    putchar('\n');
  */

} /* storecheck */


#ifndef BENCHMARK
/* Prints the vectors in the NUMPATS vector storage variable 'vecs' on the screen. -1's are
 * printed as '.' and +1's as 'X' */
static void printvec (int vecs[][])
{
  int row, vec, neuron;
  
  for(row=0; row<NNHEIGHT; row++) {
    for(vec=0; vec<NUMPATS; vec++) {
      for(neuron=0; neuron<NNWIDTH; neuron++) {
	if(vecs[vec][row*NNWIDTH+neuron]==1)
	  putchar('X');
	else
	  putchar('.'); 
      } /* for */
      printf("  ");
    } /* for */
    putchar('\n');
  } /* for */
  putchar ('\n');
} /* printvec */
#endif

/* Prints the T-matrix on the screen. */
static void printT()
{
  int row, col;
  
  putchar('\n');
  printf("The T-matrix:\n");
  
  for (row=0; row<NNTOT; row++) {
    for(col=0; col<NNTOT; col++)
      printf("%.1f ", Tmatrix[row][col]);
    putchar('\n');
  } /* for */
  
  putchar('\n');
} /* printT */


/* Prints the vector vect on screen as 1's and -1's. The argument 'vector' is used to
 * indicate the number of the vector so that the actual letter can be printed from vnames. */ 
static void printV (int vector, signed int vect[])
{
  int neuron;
  
  printf("%c -> ",vnames[vector]);
  for (neuron=0; neuron<NNTOT; ++neuron)
    if(vect[neuron] != 0)
      printf("%d ", vect[neuron]);
  putchar('\n');
} /* printV */


/* Applies random unlearning to the T-matrix. The user is asked to type in a random
 * seed and number of unlearning vectors (recommended about 100).
 * Unlearning could improve the number of stored patterns, on the other hand, it
 * might also make things worse ;)
 */ 
static void unlearn (int seed, int iter)
{
  int vecs, neuron, row, col;
  real *tempvec;
  
  tempvec = (real *)malloc((NNTOT)*sizeof(real));
  if (!tempvec) {
     fprintf(stderr,"ABORT: Out of memory\n");
     exit(1);
  }

  /*
  printf("\nRandom seed: ");
  scanf("%d", &seed);
  srand(seed);
  printf("\nNumber of unlearning vectors: ");
  scanf("%d", &iter);
  getchar();
  */

  mysrand(seed);

  for(vecs=0; vecs<iter; vecs++) {
    for(neuron=0; neuron<NNTOT; neuron++) 
      tempvec[neuron]= (myrand()>=0.5) ? 1 : -1; 
    for(row=0; row<NNTOT; row++) {
      for(col=0; col<NNTOT; col++) {
	if(row==col)
	  Tmatrix[row][col]=0.0;
	else
	  Tmatrix[row][col]+=0.01*tempvec[row]*tempvec[col];
      } /* for */
    } /* for */
  } /* for */
  
} /* unlearn */


/* Creates generators for all (or n first) stored vectors in 'newvectors' and places the
 * generators in the global variable 'generators'. The generators are found using
 * the clamping method. */ 
#ifndef BENCHMARK
static void makegenerators (int n)
{
   int vec, neuron, generator, count=0;
   signed int probe[NNTOT], dest[NNTOT];

   probe = (int *)malloc((NNTOT)*sizeof(int));
   dest = (int *)malloc((NNTOT)*sizeof(int));
   if (!probe || !dest) {
     fprintf(stderr,"ABORT: Out of memory\n");
     exit(1);
   }

   for(vec=0; vec<NUMPATS; vec++)
      for(neuron=0; neuron<NNTOT; neuron++)
         generators[vec][neuron]=0;

   for(vec=0; vec<NUMPATS; vec++) {
     if(hamming(vectors[vec], newvectors[vec], MODE_NORMAL)==0) {
       generator=0;
       for(neuron=0; neuron<NNTOT; neuron++)
	 probe[neuron]=0;
       do {
	 probe[generator]=newvectors[vec][generator];
	 if(nmode==MODE_BIN)
	   run(probe, dest);
	 else
	   runcont(probe, dest);
	 generator++;
       } while((hamming(dest, newvectors[vec], MODE_NORMAL) != 0) && (generator<NNTOT));
       for(neuron=0; neuron<NNTOT; neuron++)
	 generators[vec][neuron]=probe[neuron];
       count++; if (count>=n) break;
     } /* if */
   } /* for */
} /* generators */


/* Proves that the generators works by computing the original vector from the
 * generator and print it on the screen. */ 
static void usegenerators (int n)
{
  signed int genvec[NUMPATS][NNTOT];
  int vec, neuron, count=0;
  
  for(vec=0; vec<NUMPATS; vec++)
    for(neuron=0; neuron<NNTOT; neuron++)
      genvec[vec][neuron]=0;
  
  for(vec=0; vec<NUMPATS; vec++) {
    if(generators[vec][0] != 0) {
      printV(vec, generators[vec]);
      if(nmode==MODE_BIN)
	run(generators[vec], genvec[vec]);
      else
	runcont(generators[vec], genvec[vec]);
    } /* if */
    if (stored[vec]) count++; if (count>=n) break;
  }
  printf("\nThe vectors recreated using the generators:\n");
  printvec(genvec);
  putchar('\n');

} /* usegenerators */
#endif

/* Same as run, updates 'source' until a stable state is reached and copies the
 * computed vector to 'dest'. But runcont uses continuous neurons. Number of iterations
 * needed to reach a stable state is returned. */
static int runcont (signed int source[], signed int dest[])
{
  signed int neuron, row, max=0, maxcont=0;
  real *tempvecA, thesum;
  signed int *tempvecC;
  wbool stable=WFALSE, threshold=WFALSE;
  
  tempvecA = (real *)malloc((NNTOT)*sizeof(real));
  tempvecC = (signed int *)malloc((NNTOT)*sizeof(signed int));
  if (!tempvecA || !tempvecC) {
    fprintf(stderr,"ABORT: Out of memory\n");
    exit(1);
  }

  for(neuron=0; neuron<NNTOT; neuron++) 
    tempvecA[neuron]=dest[neuron]=source[neuron];
  
  while((!stable) && (max<500)) {
    maxcont=0;
    for(row=0; row<NNTOT; row++) {
      thesum=0.0;
      for(neuron=0; neuron<NNTOT; neuron++) 
	thesum+=Tmatrix[row][neuron]*source[neuron];
      tempvecA[row]=(1.0-myexp(-1.0*thesum))/(1.0+myexp(-1.0*thesum));  
    } /* for */ 
    while((!threshold) && (maxcont<50)) {
      threshold=WTRUE;
      for(row=0; row<NNTOT; row++) {
	if(fabs(tempvecA[row])<0.7) {
	  thesum=0.0;
	  for(neuron=0; neuron<NNTOT; neuron++) 
	    thesum+=Tmatrix[row][neuron]*tempvecA[neuron];
	  if ((tempvecA[row]=(1.0-myexp(-1.0*thesum))/(1.0+myexp(-1.0*thesum)))<0.7)  
	    threshold=WFALSE;
	} /* if */ 
      } 
      maxcont++;
    } /* while */ 
    for(neuron=0; neuron<NNTOT; neuron++)
      tempvecC[neuron] = (tempvecA[neuron]>0) ? 1 : -1;
    if(hamming(dest, tempvecC, MODE_NORMAL)==0)
      stable=WTRUE;
    else {
      neuron=0;
      while((neuron<NNTOT) && (dest[neuron]==tempvecC[neuron]))
	neuron++;
      dest[neuron]=tempvecC[neuron];
      for(neuron=0; neuron<NNTOT; neuron++)
	tempvecA[neuron]=dest[neuron];
    } /* else */
    max++;
  } /* while */ 
  
  if(max==500)
    printf("Warning! No stable state reached after 500 iterations!");
  return max;
  
} /* runcont */

/* A pseudo-random algorithm, just to make sure you always get the
 * same pseudo-random sequence, to make sure the benchmark is fair for all
 * platforms. For our purposes, it is not very important that this algorithm
 * gives a good random sequence, so don't worry about the quality of it.
 */
static void mysrand(unsigned long seed) {
  randnum = seed;
}

static real myrand() {
  randnum = (randnum*1103515245+12345)%4294967296;
  return ((real)randnum/(real)4294967296);
}

static double myexp(double in)
{
  if (in>2.0e2)
    in=200.0;
  if (in<-2.0e2)
    in=-200.0;

  return exp(in);
}

/* END OF PROGRAM */





























