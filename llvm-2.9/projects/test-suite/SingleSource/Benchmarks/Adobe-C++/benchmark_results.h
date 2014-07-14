/*
    Copyright 2007-2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
    
    Source file for shared result reporting used by most of the benchmarks
*/

/******************************************************************************/

/* 
 Yes, this would be easier with a class or std::vector
  but it needs to work for both C and C++ code
*/

/* declarations */

typedef struct one_result {
	double time;
	const char *label;
 } one_result;

extern one_result *results;

void record_result( double time, const char *label );


/******************************************************************************/

/* implementation */

#include <stdlib.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

one_result *results = NULL;
int current_test = 0;
int allocated_results = 0;

void record_result( double time, const char *label ) {

	if ( results == NULL || current_test >= allocated_results) {
		allocated_results += 10;
		results = (one_result *) realloc( results, allocated_results*sizeof(one_result) );
		if (results == NULL) {
			printf("Could not allocate %d results\n", allocated_results);
			exit(-1);
		}
	}
	
	results[current_test].time = time;
	results[current_test].label = label;
	current_test++;
}

/******************************************************************************/

const int kShowGMeans = 1;
const int kDontShowGMeans = 0;

const int kShowPenalty = 1;
const int kDontShowPenalty = 0;

/******************************************************************************/

/*
I need to be able to parse the label and absolute time from each entry, correctly
	BUT this also needs to be human readable for people testing/debugging the code
	(otherwise I'd use XML and make it really easy (if somewhat slow) to parse)
	(No, XML does not qualify as human readable)

parse as:
%i ([ ]*)\"%s\"  %f sec   %f M      %f\r

*/
void summarize(const char *name, int size, int iterations, int show_gmeans, int show_penalty ) {
	int i;
	double millions = ((double)(size) * iterations)/1000000.0;
	double total_absolute_times = 0.0;
	double gmean_ratio = 0.0;
	
	
	/* find longest label so we can adjust formatting
		12 = strlen("description")+1 */
	int longest_label_len = 12;
	for (i = 0; i < current_test; ++i) {
		int len = (int)strlen(results[i].label);
		if (len > longest_label_len)
			longest_label_len = len;
	}

	printf("\ntest %*s description   absolute   operations   ratio with\n", longest_label_len-12, " ");
	printf("number %*s time       per second   test0\n\n", longest_label_len, " ");

	for (i = 0; i < current_test; ++i)
		printf("%2i %*s\"%s\"  %5.2f sec   %5.2f M     %.2f\n",
				i,
				(int)(longest_label_len - strlen(results[i].label)),
				"",
				results[i].label,
				results[i].time,
				millions/results[i].time,
				results[i].time/results[0].time);

	// calculate total time
	for (i = 0; i < current_test; ++i) {
		total_absolute_times += results[i].time;
	}

	// report total time
	printf("\nTotal absolute time for %s: %.2f sec\n", name, total_absolute_times);

	if ( current_test > 1 && show_penalty ) {
	
		// calculate gmean of tests compared to baseline
		for (i = 1; i < current_test; ++i) {
			gmean_ratio += log(results[i].time/results[0].time);
		}
		
		// report gmean of tests as the penalty
		printf("\n%s Penalty: %.2f\n\n", name, exp(gmean_ratio/(current_test-1)));
	}

	// reset the test counter so we can run more tests
	current_test = 0;
}

/******************************************************************************/

void summarize_simplef( FILE *output, const char *name ) {
	int i;
	double total_absolute_times = 0.0;
	
	/* find longest label so we can adjust formatting
		12 = strlen("description")+1 */
	int longest_label_len = 12;
	for (i = 0; i < current_test; ++i) {
		int len = (int)strlen(results[i].label);
		if (len > longest_label_len)
			longest_label_len = len;
	}

	fprintf(output,"\ntest %*s description   absolute\n", longest_label_len-12, " ");
	fprintf(output,"number %*s time\n\n", longest_label_len, " ");

	for (i = 0; i < current_test; ++i)
		fprintf(output,"%2i %*s\"%s\"  %5.2f sec\n",
				i,
				(int)(longest_label_len - strlen(results[i].label)),
				"",
				results[i].label,
				results[i].time);

	// calculate total time
	for (i = 0; i < current_test; ++i) {
		total_absolute_times += results[i].time;
	}

	// report total time
	fprintf(output,"\nTotal absolute time for %s: %.2f sec\n", name, total_absolute_times);

	// reset the test counter so we can run more tests
	current_test = 0;
}

/******************************************************************************/
