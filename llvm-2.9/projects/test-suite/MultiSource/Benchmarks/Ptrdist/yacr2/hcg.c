/*
 *
 * hcg.c
 *
 */


#define HCG_CODE


/*
 *
 * Includes.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "types.h"
#include "hcg.h"
#include "channel.h"


/*
 *
 * Code.
 *
 */

void
AllocHCG(void)
{
    HCG = (nodeHCGType *)malloc((channelNets + 1) * sizeof(nodeHCGType));
    storageRootHCG = (ulong *)malloc((channelNets + 1) * (channelNets + 1) * sizeof(ulong));
    storageHCG = storageRootHCG;
    storageLimitHCG = (channelNets + 1) * (channelNets + 1);
}

void
FreeHCG(void)
{
    free(HCG);
    free(storageRootHCG);
    storageLimitHCG = 0;
}

void
BuildHCG(void)
{
    ulong	net;
    ulong	which;
    ulong	constraint;
    ulong	first;
    ulong	last;
    ulong	check;
    ulong	add;

    /*
     * Allocate HCG storage.
     */
    AllocHCG();

    /*
     * Build HCG one net at a time.
     */
    for (net = 1; net <= channelNets; net++) {
	first = FIRST[net];
	last = LAST[net];
	constraint = 0;
	HCG[net].netsHook = storageHCG;
	for (which = 1; which <= channelNets; which++) {
	    if (((FIRST[which] < first) && (LAST[which] < first)) || ((FIRST[which] > last) && (LAST[which] > last))) {
		/*
		 * No constraint.
		 */
	    }
	    else {
		/*
		 * No constraint should ever already exist.
		 * Because there is only one first and last
		 * for each net, the same constraint could
		 * never be added twice.
		 */
		add = TRUE;
		for (check = 0; check < constraint; check++) {
		    if (HCG[net].netsHook[check] == which) {
			add = FALSE;
			break;
		    }
		}
		assert(add);

		/*
		 * Add constraint.
		 */
		assert(storageLimitHCG > 0);
		HCG[net].netsHook[constraint] = which;
		storageHCG++;
		storageLimitHCG--;
		constraint++;
	    }
	}
	HCG[net].nets = constraint;
    }
}

void
DFSClearHCG(nodeHCGType * HCG)
{
    ulong	net;

    for (net = 1; net <= channelNets; net++) {
	HCG[net].netsReached = FALSE;
    }	
}

void
DumpHCG(nodeHCGType * HCG)
{
    ulong	net;
    ulong	which;

    for (net = 1; net <= channelNets; net++) {
	printf("[%d]\n", net);
	for (which = 0; which < HCG[net].nets; which++) {
	    printf("%d ", HCG[net].netsHook[which]);
	}
	printf("\n\n");
    }	
}

void
NoHCV(nodeHCGType * HCG,
      ulong select,
      ulong * netsAssign,
      ulong * tracksNoHCV)
{
    ulong	track;
    ulong	net;
    ulong	which;
    ulong	ok;

    for (track = 1; track <= channelTracks; track++) {
	/*
	 * For each track, check to see if any nets assigned
	 * to it would be an HCV for the selected net.
	 */
	ok = TRUE;
	for (net = 1; net <= channelNets; net++) {
	    if (netsAssign[net] == track) {
		/*
		 * Net assigned to track.
		 */
		for (which = 0; which < HCG[select].nets; which++) {
		    if (HCG[select].netsHook[which] == net) {
			/*
			 * HCV.
			 */
			ok = FALSE;
			break;
		    }
		}
		
		/*
		 * Is net an HCV?
		 */
		if (! ok) {
		    break;
		}
	    }
	}

	/*
	 * Is this track ok (no HCV's)?
	 */
	tracksNoHCV[track] = ok;
    }
}
