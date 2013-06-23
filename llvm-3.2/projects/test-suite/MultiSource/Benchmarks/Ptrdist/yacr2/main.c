
/*
 *
 * main.c
 *
 */


#define MAIN_CODE


/*
 *
 * Includes.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "option.h"
#include "channel.h"
#include "vcg.h"
#include "hcg.h"
#include "assign.h"
#include "maze.h"


/*
 *
 * Code.
 *
 */

int
main(int argc,
     char *argv[])
{
    ulong      	done;
    ulong	fail;
    ulong	net;
    ulong	insert;
    int		netsLeft;
int TIMELOOP;
for (TIMELOOP = 0; TIMELOOP < 20; ++TIMELOOP) {

    Option(argc, argv);

    BuildChannel();
    BuildVCG();
    AcyclicVCG();
    BuildHCG();

    do {
	/*
	 * Setup.
	 */
	AllocAssign();
	NetsAssign();
	InitAllocMaps();

	/*
	 * Copy the nets assign.
	 */
	channelTracksCopy = channelTracks;
	for (net = 1; net <= channelNets; net++) {
	    netsAssignCopy[net] = netsAssign[net];
	}

	/*
	 * Route, adding a row if necessary.
	 */
	fail = 0;
	do {
	    done = TRUE;
	    if ((netsLeft = DrawNets()) != 0) {
		printf("Assignment could not route %d columns, trying maze1...\n",
		       netsLeft);
		if ((netsLeft = Maze1()) != 0) {
		    printf("Maze1 could not route %d columns, trying maze2...\n",
			   netsLeft);
		    if ((netsLeft = Maze2()) != 0) {
			printf("Maze2 could not route %d columns, trying maze3...\n",
			       netsLeft);
			if ((netsLeft = Maze3()) != 0) {
			    printf("Maze3 could not route %d columns, adding a track...\n",
				   netsLeft);
			    /* PrintChannel(); */
			    if (! fail) {
				channelTracks++;
			    }
			    fail++;

			    /*
			     * Restore the nets assign.
			     */
			    for (net = 1; net <= channelNets; net++) {
				netsAssign[net] = netsAssignCopy[net];
			    }

			    /*
			     * Damn!
			     */
			    done = FALSE;
			}
		    }
		}
	    }

	    /*
	     * Add a track at track # fail, thereby shifting
	     * all tracks at that point down one track.
	     */
	    if ((! done) && fail) {
#ifdef VERBOSE
		printf("\n*** fail (insert track at %d) ***\n", fail);
#endif
		for (insert = 1; insert <= channelNets; insert++) {
		    if (netsAssign[insert] >= fail) {
			netsAssign[insert]++;
		    }
		}
	    }
	} while ((! done) && (fail <= channelTracksCopy + 1));

	/*
	 * Did adding a row within existing assignment work?
	 * If not, just start over.
	 */
	if (! done) {
	    FreeAllocMaps();
	    FreeAssign();
	    assert(channelTracks == channelTracksCopy + 1);
	}
    } while (! done);

    printf("\n");
    PrintChannel();
#ifdef PLUS_STATS
    PrintDerefStats(stderr);
    PrintHeapSize(stderr);
#endif /* PLUS_STATS */
}
    exit(0);
}
