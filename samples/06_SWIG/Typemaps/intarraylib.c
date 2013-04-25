// Copyright (c) 2013 Adobe Systems Inc

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "intarraylib.h"

int sumArray(int* buffer, int bufferSize){
	int i;
	int sum = 0;

	for(i = 0; i < bufferSize; i++){
		sum += buffer[i];
	}

	return sum;
}

int dotProductArray(int* buffer, int bufferSize){
	int i;
	int product = 1;

	for(i = 0; i < bufferSize; i++){
		product *= buffer[i];
	}

	return product;
}

IntArray reverseArray(int* buffer, int bufferSize){
	int i = 0;
	int j = bufferSize - 1;

	while (i < j){
		int temp = buffer[i];
		buffer[i] = buffer[j];
		buffer[j] = temp;

		i++;
		j--;
	}
	
	IntArray r = {buffer, bufferSize};
	
	return r;
}

IntArray incrementArray(int* buffer, int bufferSize){
	int i;
	
	for (i = 0; i < bufferSize; i++){
		buffer[i]++;
	}

	IntArray r = {buffer, bufferSize};
	
	return r;
}
