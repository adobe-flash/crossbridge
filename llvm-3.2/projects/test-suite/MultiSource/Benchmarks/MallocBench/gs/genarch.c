/* genarch.c */
/* Generate a header file (arch.h) with parameters */
/* reflecting the machine architecture. */

#include <stdio.h>

/* We should write the result on stdout, but the Turbo C 'make' */
/* can't handle output redirection (sigh). */

main(argc, argv)
    int argc;
    char *argv[];
{	long one = 1;
	int v16 = 1 << 16;
	FILE *f = fopen("arch.h", "w");
	if ( f == NULL )
	   {	fprintf(stderr, "genarch.c: can't open arch.h for writing\n");
		exit(1);
	   }
	fprintf(f, "#define big_endian %d\n", 1 - *(char *)&one);
	fprintf(f, "#define ints_are_short %d\n", v16 == 0);
	fclose(f);
	return 0;
}
