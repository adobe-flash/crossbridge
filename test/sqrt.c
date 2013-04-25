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

/*
compile: gcc sqrt.c -o sqrt

expected output:
670842112
786182671

On cygwin the last digit of the second line will be 0, but with the player and the official shell build this is not the case
*/

#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#if __AVM2__
#include <AS3/AS3.h>
#endif

typedef union { double d; uint64_t ll; } dbl_ll_union;

dbl_ll_union X1,X2;

void blah() {
	X2.d = sqrt(X1.d);
}

int main()
{
	X1.ll = 4592377297834360064ULL;
	blah();

	#if __AVM2__
		inline_as3(
			"trace(%0+\"\\n\"+%1)\n" : : "r"((int)X1.ll), "r"((int)X2.ll)
		);
	#else
		printf("%llu\n%llu\n", X1.ll, X2.ll);
	#endif
}