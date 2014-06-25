#include "example.h"

void reverseArray(int* in, int* out, int length){
	int i = 0;
    
	while (i < length){
		out[i] = in[length - i - 1];
		i++;
	}
}

void incrementArray(int* in, int* out, int length)
{
	int i = 0;
    
	while (i < length){
		out[i] = in[i] + 1;
		i++;
	}
}
