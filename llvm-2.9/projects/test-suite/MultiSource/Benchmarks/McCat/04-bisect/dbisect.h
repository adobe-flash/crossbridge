#ifndef __DBISECT_H_DEF

#define __DBISECT_H_DEF

#define TRACE_ITERATION 0x1
#define TRACE_INTERVALS 0x2
#define TRACE_INPUT 0x4

void dbisect(double c[], double b[], double beta[], int n, int m1, int m2, 
	     double eps1, double *eps2, int *z, double x[]);

#endif
