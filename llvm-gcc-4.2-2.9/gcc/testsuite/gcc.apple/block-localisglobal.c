/* APPLE LOCAL file radar 6230297 */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-mmacosx-version-min=10.6" { target *-*-darwin* } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

/*
 *  localisglobal.c
 *  testObjects
 *
 *  Created by Blaine Garst on 9/29/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 *  works in all configurations
 *  CONFIG
 *  rdar://6230297
 */

#include <stdio.h>

void (^global)(void) = ^{ printf("hello world\n"); };

int aresame(void *first, void *second) {
    long *f = (long *)first;
    long *s = (long *)second;
    return *f == *s;
}
int main(int argc, char *argv[]) {
    int i = 10;
    void (^local)(void) = ^ { printf("hi %d\n", i); };
    void (^localisglobal)(void) = ^ { printf("hi\n"); };
    
    if (aresame(local, localisglobal)) {
        printf("local block could be global, but isn't\n");
        return 1;
    }
    if (!aresame(global, localisglobal)) {
        printf("local block is not global, not stack, what is it??\n");
        return 1;
    }
    printf("%s: success\n", argv[0]);
    return 0;
    
}
