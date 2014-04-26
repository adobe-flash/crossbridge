/* APPLE LOCAL file */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define	SIZE	8192
char	*buf;
long	tests = 0;
int	(*function)(const void *s1, const void *s2, size_t size);

void test(int x, int y, int z) {
    int		i,j;
    char	*s1, *s2;
    
    s1 = &buf[x];
    s2 = &buf[y];
    
    i = function(s1,s2,z); /* call through to LibC */
    j = memcmp(s1,s2,z);   /* this is normally inlined */
    
    if (i==j)	return;
    
    abort();
}
    

int main() {
     
    int		i,j,k;
    
    buf = malloc(SIZE);
    for( i = 0; i < SIZE; i++) 
        buf[i] = i;
	
    function = memcmp;
        
    for( i = 0; i < (SIZE/2); i++ ) 
        for( j = 0; j < (SIZE/2); j++ ) 
            test(i,j,i+j);
            
    exit(0);
}
    
