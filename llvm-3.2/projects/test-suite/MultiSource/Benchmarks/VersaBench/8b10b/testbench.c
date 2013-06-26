/*
 * Copyright (c) 2002 David Wentzlaff
 *
 * Permission  is hereby  granted,  free  of  charge, to  any  person
 * obtaining a  copy of  this software  and  associated documentation
 * files   (the  "Software"),  to   deal  in  the   Software  without
 * restriction, including without  limitation the rights to use, copy,
 * modify, merge, publish,  distribute, sublicense, and/or sell copies
 * of  the Software,  and to  permit persons to  whom the  Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above  copyright notice  and this  permission notice  shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE  SOFTWARE IS  PROVIDED "AS IS",  WITHOUT WARRANTY OF  ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING  BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY,   FITNESS   FOR   A   PARTICULAR    PURPOSE    AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM,  DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,  OUT OF OR IN
 * CONNECTION  WITH THE SOFTWARE OR  THE USE OR OTHER  DEALINGS IN THE
 * SOFTWARE.  
 */

//This is part of David Wentzlaff's Masters Thesis
//This file is Copyright David Wentzlaff 2002 All Rights Reserved.
//
// Filename : testbench.c
// Date : 07/09/2002

/* Modified by: Rodric M. Rabbah 06-03-04 */

#include <stdio.h>
#include <stdlib.h>
#include "calc.h"
#include "testbench.h"

void runTestbench(int numberOfWords, char* inputFileName, char* outputFileName0)
{
	unsigned int * theBigAllocatedThing;
	unsigned int * theInputArray;
	unsigned int * theOutputArray0;
	FILE * inputFile;
	FILE * outputFile0;
	unsigned int readData;
	unsigned int bits0;
	int counter;
  int tmp;
	//first, make the needed space
	//hmm we will put it on the heap for now becasue that
	//will be contiguous.  
	theBigAllocatedThing = (unsigned int *) malloc((numberOfWords * sizeof(unsigned int) * 2));
	theInputArray = theBigAllocatedThing;
	theOutputArray0 = theBigAllocatedThing + numberOfWords;

	
	//read the input into the input array from a file
	inputFile = fopen(inputFileName, "r");
	fread(theInputArray, numberOfWords, sizeof(unsigned int), inputFile);
	fclose(inputFile);

	/*** VERSABENCH START ***/
	//run calc for numberOfWords times
  for (tmp = 0; tmp < numberOfWords; ++tmp)  // LLVM: increase execution time.
	for(counter = 0; counter < numberOfWords; counter ++)
	{
#ifdef BIG_CALC
		bits0 = theInputArray[counter];
		theOutputArray0[counter] = bigTableCalc(bits0);
#else
		bits0 = theInputArray[counter];
		theOutputArray0[counter] = calc(bits0&0xff, bits0>>8);
#endif
	}
	/*** VERSABENCH END ***/
	
	//dump the outputs to a files
	outputFile0 = stdout;
	for(counter = 0; counter < (numberOfWords); counter+= 128)
	{
		bits0 = theOutputArray0[counter];
		fprintf(outputFile0, "%8.8X\n", bits0);
	}

	free(theBigAllocatedThing);
}

#if 0
	for(counter = 0; counter < (numberOfWords); counter++)
	{
		fscanf(inputFile, "%X\n", &readData);
		theInputArray[counter] = readData;
	}

	//touch both of the output arrays so that they are in the L2
	for(counter = 0; counter < numberOfWords; counter ++)
	{
		theOutputArray0[counter] = counter;
	}
#endif
