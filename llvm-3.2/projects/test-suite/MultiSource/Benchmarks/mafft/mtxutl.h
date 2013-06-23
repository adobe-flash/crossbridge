void MtxuntDouble( double **, int );
void MtxmltDouble( double **, double **, int );

char *AllocateCharVec( int );
void FreeCharVec( char * );

char **AllocateCharMtx( int, int);
void ReallocateCharMtx( char **, int, int);
void FreeCharMtx( char ** );

float *AllocateFloatVec( int );
void FreeFloatVec( float * );

float **AllocateFloatHalfMtx( int );
float **AllocateFloatMtx( int, int );
void FreeFloatHalfMtx( float **, int );
void FreeFloatMtx( float ** );

float **AlocateFloatTri( int );
void FreeFloatTri( float ** );

int *AllocateIntVec( int );
void FreeIntVec( int * );

int **AllocateIntMtx( int, int );
void FreeIntMtx( int ** );

char ***AllocateCharCub( int, int, int );
void FreeCharCub( char *** );

int ***AllocateIntCub( int, int, int );
void FreeIntCub( int *** );

double *AllocateDoubleVec( int );
void FreeDoubleVec( double * );

double **AllocateDoubleMtx( int, int );
void FreeDoubleMtx( double ** );

double ***AllocateDoubleCub( int, int, int );
void FreeDoubleCub( double *** );

float ***AllocateFloatCub( int, int, int );
void FreeFloatCub( float *** );

short *AllocateShortVec( int );
void FreeShortVec( short * );

short **AllocateShortMtx( int, int );
void FreeShortMtx( short ** );
