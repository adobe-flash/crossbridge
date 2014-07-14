/*
 *
 * channel.c
 *
 */


#define CHANNEL_CODE


/*
 *
 * Includes.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "channel.h"


/*
 *
 * Code.
 *
 */

void
BuildChannel(void)
{
    /*
     * What is the length dimension of the channel?
     * That is, how many columns in the channel?
     */
    DimensionChannel();

    /*
     * What are the particulargs of the channel?
     * That is, what are the nets and pin locations?
     */
    DescribeChannel();

    /*
     * What is the maximum density of the channel?
     * That is, what is the lower bound on the number of tracks
     * required to route the channel?
     */
    DensityChannel();
}

void
DimensionChannel(void)
{
    FILE	*channelFP;
    ulong	line;
    ulong	dim;
    ulong	net;
    ulong	col;
    ulong	bot;
    ulong	top;
    long	stat;

    /*
     * Open channel description file.
     */
    channelFP = fopen(channelFile, "r");
    if (channelFP == NULL) {
	/*
	 * Error in channel file description.
	 */
	printf("Error:\n");
	printf("\tChannel file cannot be opened.\n");
	exit(1);
    }
    
    /*
     * Scan the file to find the last column
     * number.  The channel file description
     * contains non-negative integers in the
     * format...
     *
     * [column #] [bottom net #] [top net #]
     */
    line = 0;
    dim = 0;
    net = 0;
    do {
	line++;
	unsigned int c1, b1, t1;
	stat = fscanf(channelFP, "%u%u%u", &c1, &b1, &t1);
	col = c1; bot = b1; top = t1;
	if (stat != EOF) {
	    if (stat == 3) {
		/*
		 * Update column #.
		 */
		if (col > dim) {
		    dim = col;
		}

		/*
		 * Determine how many channel nets.
		 */
		if (bot > net) {
		    net = bot;
		}
		if (top > net) {
		    net = top;
		}
	    }
	    else {
		/*
		 * Error in channel file description.
		 */
		printf("Error:\n");
		printf("\tChannel file description invalid at line %d.\n", line);
		printf("\tIncorrect number of specifiers.\n");
		exit(1);
	    }
	}
    } while (stat != EOF);

    /*
     * Close channel description file.
     */
    if (fclose(channelFP) == EOF) {
	/*
	 * Error in channel file description.
	 */
	printf("Error:\n");
	printf("\tChannel file cannot be closed.\n");
	exit(1);
    }

    /*
     * Check channel dimension.
     */
    if (dim == 0) {
	/*
	 * Error in channel file description.
	 */
	printf("Error:\n");
	printf("\tChannel description invalid.\n");
	printf("\tChannel has null dimension.\n");
	exit(1);
    }

    /*
     * Set global channel info.
     */
    channelColumns = dim;
    channelNets = net;
}

void
DescribeChannel(void)
{
    FILE	*channelFP;
    ulong	line;
    ulong	col;
    ulong	bot;
    ulong	top;
    long	stat;

    /*
     * Top terminals of channel.
     */
    TOP = (ulong *)malloc((channelColumns+1) * sizeof(ulong));

    /*
     * Bottom terminals of channel.
     */
    BOT = (ulong *)malloc((channelColumns+1) * sizeof(ulong));

    /*
     * Initialize terminals of channel.
     */
    for (col = 0; col <= channelColumns; col++) {
	TOP[col] = 0;
	BOT[col] = 0;
    }

    /*
     * Open channel description file.
     */
    channelFP = fopen(channelFile, "r");
    if (channelFP == NULL) {
	/*
	 * Error in channel file description.
	 */
	printf("Error:\n");
	printf("\tChannel file cannot be opened.\n");
	exit(1);
    }

    /*
     * Scan the file to find the last column
     * number.  The channel file description
     * contains non-negative integers in the
     * format...
     *
     * [column #] [bottom net #] [top net #]
     */
    line = 0;
    do {
	line++;
	unsigned int c1, b1, t1;
	stat = fscanf(channelFP, "%u%u%u", &c1, &b1, &t1);
	col = c1; bot = b1; top = t1;
	if (stat != EOF) {
	    if (stat == 3) {
		/*
		 * Build column.
		 */
		if (col > channelColumns) {
		    /*
		     * Error in channel file description.
		     */
		    printf("Error:\n");
		    printf("\tChannel file description invalid at line %d.\n", line);
		    printf("\tColumn number out of range.\n");
		    exit(1);
		}
		else {
		    /*
		     * Doit.
		     */
		    BOT[col] = bot;
		    TOP[col] = top;
		}
	    }
	    else {
		/*
		 * Error in channel file description.
		 */
		printf("Error:\n");
		printf("\tChannel file description invalid at line %d.\n", line);
		printf("\tIncorrect number of specifiers.\n");
		exit(1);
	    }
	}
    } while (stat != EOF);

    /*
     * Close channel description file.
     */
    if (fclose(channelFP) == EOF) {
	/*
	 * Error in channel file description.
	 */
	printf("Error:\n");
	printf("\tChannel file cannot be closed.\n");
	exit(1);
    }
}

void
DensityChannel(void)
{
    ulong	init;
    ulong	which;
    ulong 	col;
    ulong 	bound;
    ulong	boundColumn;

    /*
     * Allocate track dimension structures.
     */
    FIRST = (ulong *)malloc((channelNets+1) * sizeof(ulong));
    LAST = (ulong *)malloc((channelNets+1) * sizeof(ulong));
    DENSITY = (ulong *)malloc((channelColumns+1) * sizeof(ulong));
    CROSSING = (ulong *)malloc((channelNets+1) * sizeof(ulong));

    /*
     * Initialize track dimension structures.
     */
    for (init = 0; init <= channelNets; init++) {
	FIRST[init] = 0;
	LAST[init] = 0;
	CROSSING[init] = 0;
    }
    for (init = 0; init <= channelColumns; init++) {
	DENSITY[init] = 0;
    }

    /*
     * Compute the span for each net.
     */
    for (which = 1; which <= channelNets; which++) {
	/*
	 * Compute first.  This is the location of
	 * the first column for this net.
	 */
	for (col = 1; col <= channelColumns; col++) {
	    if ((BOT[col] == which) || (TOP[col] == which)) {
		FIRST[which] = col;
		break;
	    }
	}

	/*
	 * Compute last.  This is the location of
	 * the last column for this net.
	 */
	for (col = channelColumns; col >= 1; col--) {
	    if ((BOT[col] == which) || (TOP[col] == which)) {
		LAST[which] = col;
		break;
	    }
	}

	/*
	 * Increment the track for each column
	 * of the channel which this net spans.
	 */
	for (col = FIRST[which]; col <= LAST[which]; col++) {
	    DENSITY[col]++;
	}
    }

    /*
     * Compute lower bound on channel tracks.
     */
#ifdef SCOTT
    bound = 0;
    for (col = 1; col <= channelColumns; col++) {
	if (DENSITY[col] > bound) {
	    bound = DENSITY[col];
	    boundColumn = col;
	}
    }
#else
    bound = 0;
    for (col = channelColumns; col >= 1; col--) {
	if (DENSITY[col] > bound) {
	    bound = DENSITY[col];
	    boundColumn = col;
	}
    }
#endif

    /*
     * Set global channel info.
     */
    channelTracks = bound;			/* tracks available for routing */
    channelDensity = bound;			/* max channel density */
    channelDensityColumn = boundColumn;		/* column of max channel density */
}









