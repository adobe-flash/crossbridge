/* -*- mode: c -*-
 * $Id: strcat.c 14069 2004-06-08 17:21:42Z lattner $
 * http://www.bagley.org/~doug/shootout/
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STUFF "hello\n"

int
main(int argc, char *argv[]) {
    int n = ((argc == 2) ? atoi(argv[1]) : 10000000);
    int i, buflen = 32;
    char *strbuf = calloc(sizeof(char), buflen);
    char *strend = strbuf;
    int stufflen = strlen(STUFF);

    if (!strbuf) { perror("calloc strbuf"); exit(1); }
    for (i=0; i<n; i++) {
	if (((strbuf+buflen)-strend) < (stufflen+1)) {
	    buflen = 2*buflen;
	    strbuf = realloc(strbuf, buflen);
	    if (!strbuf) { perror("realloc strbuf"); exit(1); }
	    strend = strbuf + strlen(strbuf);
	}
	/* much faster to strcat to strend than to strbuf */
	strcat(strend, STUFF);
	strend += stufflen;
    }
    printf("%d\n", (int)strlen(strbuf));
    free(strbuf);

    return(0);
}
