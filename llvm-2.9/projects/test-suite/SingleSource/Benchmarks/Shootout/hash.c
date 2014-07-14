/* -*- mode: c -*-
 * $Id: hash.c 36673 2007-05-03 16:55:46Z laurov $
 * http://www.bagley.org/~doug/shootout/
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simple_hash.h"

int main(int argc, char *argv[]) {
#ifdef SMALL_PROBLEM_SIZE
#define LENGTH 350000
#else
#define LENGTH 3500000
#endif
    int i, c=0, n = ((argc == 2) ? atoi(argv[1]) : LENGTH);
    char buf[32];
	
    struct ht_ht *ht = ht_create(n);
    
    for (i=1; i<=n; i++) {
	sprintf(buf, "%x", i);
	(ht_find_new(ht, buf))->val = i;
    }

    for (i=n; i>0; i--) {
	sprintf(buf, "%d", i);
	if (ht_find(ht, buf)) c++;
    }

    ht_destroy(ht);

    printf("%d\n", c);
    return(0);
}
