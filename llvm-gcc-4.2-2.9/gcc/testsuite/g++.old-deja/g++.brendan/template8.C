// { dg-do assemble  }
// GROUPS passed templates
#include <stdio.h>

// make sure we accept unions for templates
template<int n>
union Double_alignt{
	double for_alignt;
	char array[n];

};

int main(){

	
	Double_alignt<20000> heap;

	/* APPLE LOCAL default to Wformat-security 5764921 */
	printf(" &heap.array[0] = %p, &heap.for_alignt = %p\n", (void*)&heap.array[0], (void*)&heap.for_alignt);

}
