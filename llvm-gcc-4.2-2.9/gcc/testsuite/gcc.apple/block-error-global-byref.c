/* APPLE LOCAL file radar 5803600 */
/* Test for propery handling of 'byref' globals in nested blocks. */
/* { dg-do compile } */
/* { dg-options "-mmacosx-version-min=10.5 -fblocks" { target *-*-darwin* } } */

#include <stdio.h>

void CallBlock (void (^voidvoidBlock)(void)) {
   voidvoidBlock();
}

int glob = 10;
static int stat = 10;

int foo() {
    static int static_local = 10;
    int local;
    CallBlock( ^ {++glob;  		
		  CallBlock(^ { 
				++glob;
				stat++;
			        ++static_local;
			      });
		  ++local;  		/* { dg-error "increment of read-only variable" } */
		  ++static_local; 	
		 });
    return 0;
}
