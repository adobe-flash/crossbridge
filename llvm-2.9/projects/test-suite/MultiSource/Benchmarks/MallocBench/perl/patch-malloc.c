extern char *malloc();
extern char *bZa(), *bZr(), *bZc();
extern int free();

char *
malloc(n)
int n;
{
    return bZa(n);
}

char *
calloc(n)
int n;
{
    /* return bZc(n);
     */
}

free(p)
char *p;
{
    bZf(p);
}

char *
realloc(cp, nbytes)
char *cp;
int nbytes;
{
    return bZr(cp, nbytes);
}

