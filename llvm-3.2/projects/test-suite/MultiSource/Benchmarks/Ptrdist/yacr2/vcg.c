
/*
 *
 * vcg.c
 *
 */


#define VCG_CODE


/*
 *
 * Includes.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "types.h"
#include "vcg.h"
#include "assign.h"
#include "channel.h"


/*
 *
 * Code.
 *
 */

void
AllocVCG(void)
{
    VCG = (nodeVCGType *)malloc((channelNets + 1) * sizeof(nodeVCGType));
    storageRootVCG = (constraintVCGType *)malloc((channelNets + 1) * (channelNets + 1) * sizeof(constraintVCGType));
    storageVCG = storageRootVCG;
    storageLimitVCG = (channelNets + 1) * (channelNets + 1);
    SCC = (ulong *)malloc((channelNets + 1) * sizeof(ulong));
    perSCC = (ulong *)malloc((channelNets + 1) * sizeof(ulong));
    removeVCG = (constraintVCGType * *)malloc((channelNets + 1) * (channelNets + 1) * sizeof(constraintVCGType *));
}

void
FreeVCG(void)
{
    free(VCG);
    free(storageRootVCG);
    storageLimitVCG = 0;
    free(SCC);
    free(perSCC);
    free(removeVCG);
}

void
BuildVCG(void)
{
    ulong	col;
    ulong	net;
    ulong	constraint;
    ulong	check;
    ulong	add;

    /*
     * Allocate VCG storage.
     */
    AllocVCG();

    /*
     * Build VCG one net at a time.
     */
    for (net = 1; net <= channelNets; net++) {
	/*
	 * Above constraints.
	 */
	constraint = 0;
	VCG[net].netsAboveHook = storageVCG;
	for (col = 1; col <= channelColumns; col++) {
	    if ((TOP[col] == net) && (BOT[col] != net) && (BOT[col] != 0)) {
		/*
		 * Check constraint already exist.
		 */
		add = TRUE;
		for (check = 0; check < constraint; check++) {
		    if (VCG[net].netsAboveHook[check].bot == BOT[col]) {
			add = FALSE;
			break;
		    }
		}

		/*
		 * Add constraint.
		 */
		if (add) {
		    assert(storageLimitVCG > 0);
		    VCG[net].netsAboveHook[constraint].top = TOP[col];
		    VCG[net].netsAboveHook[constraint].bot = BOT[col];
		    VCG[net].netsAboveHook[constraint].col = col;
		    VCG[net].netsAboveHook[constraint].removed = FALSE;
		    storageVCG++;
		    storageLimitVCG--;
		    constraint++;
		}
	    }
	}
	VCG[net].netsAbove = constraint;

	/*
	 * Below constraints.
	 */
	constraint = 0;
	VCG[net].netsBelowHook = storageVCG;
	for (col = 1; col <= channelColumns; col++) {
	    if ((BOT[col] == net) && (TOP[col] != net) && (TOP[col] != 0)) {
		/*
		 * Check constraint already exist.
		 */
		add = TRUE;
		for (check = 0; check < constraint; check++) {
		    if (VCG[net].netsBelowHook[check].top == TOP[col]) {
			add = FALSE;
			break;
		    }
		}

		/*
		 * Add constraint.
		 */
		if (add) {
		    assert(storageLimitVCG > 0);
		    VCG[net].netsBelowHook[constraint].top = TOP[col];
		    VCG[net].netsBelowHook[constraint].bot = BOT[col];
		    VCG[net].netsBelowHook[constraint].col = col;
		    VCG[net].netsBelowHook[constraint].removed = FALSE;
		    storageVCG++;
		    storageLimitVCG--;
		    constraint++;
		}
	    }
	}
	VCG[net].netsBelow = constraint;
    }
}

void
DFSClearVCG(nodeVCGType * VCG)
{
    ulong	net;

    for (net = 1; net <= channelNets; net++) {
	VCG[net].netsAboveLabel = 0;
	VCG[net].netsAboveReached = FALSE;
	VCG[net].netsBelowLabel = 0;
	VCG[net].netsBelowReached = FALSE;
    }	
}

void
DumpVCG(nodeVCGType * VCG)
{
    ulong	net;
    ulong	which;

    for (net = 1; net <= channelNets; net++) {
	printf("[%d]\n", net);
	printf("above: ");
	for (which = 0; which < VCG[net].netsAbove; which++) {
	    if (! VCG[net].netsAboveHook[which].removed) {
		assert(VCG[net].netsAboveHook[which].top == net);
		printf("%d ", VCG[net].netsAboveHook[which].bot);
	    }
	}

	printf("\n");
	printf("below: ");
	for (which = 0; which < VCG[net].netsBelow; which++) {
	    if (! VCG[net].netsBelowHook[which].removed) {
		assert(VCG[net].netsBelowHook[which].bot == net);
		printf("%d ", VCG[net].netsBelowHook[which].top);
	    }
	}
	printf("\n\n");
    }	
}

void
DFSAboveVCG(nodeVCGType * VCG,
	    ulong net)
{
    ulong	s;
    ulong 	above;

    VCG[net].netsAboveReached = TRUE;
    for (s = 0; s < VCG[net].netsAbove; s++) {
	if (! VCG[net].netsAboveHook[s].removed) {
	    assert(VCG[net].netsAboveHook[s].top == net);
	    above = VCG[net].netsAboveHook[s].bot;
	    if (! VCG[above].netsAboveReached) {
		DFSAboveVCG(VCG, above);
	    }
	}
    }
}

void
DFSBelowVCG(nodeVCGType * VCG,
	    ulong net)
{
    ulong	s;
    ulong 	below;

    VCG[net].netsBelowReached = TRUE;
    for (s = 0; s < VCG[net].netsBelow; s++) {
	if (! VCG[net].netsBelowHook[s].removed) {
	    assert(VCG[net].netsBelowHook[s].bot == net);
	    below = VCG[net].netsBelowHook[s].top;
	    if (! VCG[below].netsBelowReached) {
		DFSBelowVCG(VCG, below);
	    }
	}
    }
}

void
SCCofVCG(nodeVCGType * VCG,
	 ulong * SCC,
	 ulong * perSCC)
{
    ulong      	net;
    ulong      	scc;
    ulong      	per;
    ulong      	label;
    ulong      	which;
    ulong	choose;
    ulong	large;
    ulong	done;
    ;

    /*
     * DFS of above edges.
     */
    label = 0;
    for (net = 1; net <= channelNets; net++){
	if (! VCG[net].netsAboveReached) {
	    SCC_DFSAboveVCG(VCG, net, &label);
	}
    }

    /*
     * DFS of below edges.
     */
    which = 0;
    do {
	done = TRUE;

	/*
	 * Choose not reached net with smallest label.
	 */
	choose = 0;
	large = 0;
	for (net = 1; net <= channelNets; net++) {
	    if (! VCG[net].netsBelowReached) {
		assert(VCG[net].netsAboveLabel > 0);
		if (VCG[net].netsAboveLabel > large) {
		    choose = net;
		    large = VCG[net].netsAboveLabel;
		    done = FALSE;
		}
	    }
	}

	/*
	 * Find all nets in this SCC.
	 */
	if (! done) {
	    which++;
	    SCC_DFSBelowVCG(VCG, choose, which);
	}
    } while (! done);

    /*
     * Identify all SCC.
     */
    totalSCC = 0;
    for (net = 1; net <= channelNets; net++) {
	SCC[net] = VCG[net].netsBelowLabel;
	if (SCC[net] > totalSCC) {
	    totalSCC = SCC[net];
	}
    }
    assert(totalSCC > 0);
    for (scc = 1; scc <= totalSCC; scc++) {
	per = 0;
	for (net = 1; net <= channelNets; net++) {
	    if (SCC[net] == scc) {
		per++;
	    }
	}
	perSCC[scc] = per;
    }
    ;
}

void
SCC_DFSAboveVCG(nodeVCGType * VCG,
		ulong net,
		ulong * label)
{
    ulong	s;
    ulong 	above;

    VCG[net].netsAboveReached = TRUE;
    for (s = 0; s < VCG[net].netsAbove; s++) {
	if (! VCG[net].netsAboveHook[s].removed) {
	    assert(VCG[net].netsAboveHook[s].top == net);
	    above = VCG[net].netsAboveHook[s].bot;
	    if (! VCG[above].netsAboveReached) {
		SCC_DFSAboveVCG(VCG, above, label);
	    }
	}
    }
    (*label)++;
    VCG[net].netsAboveLabel = *label;
}

void
SCC_DFSBelowVCG(nodeVCGType * VCG,
		ulong net,
		ulong label)
{
    ulong	s;
    ulong 	below;

    VCG[net].netsBelowReached = TRUE;
    for (s = 0; s < VCG[net].netsBelow; s++) {
	if (! VCG[net].netsBelowHook[s].removed) {
	    assert(VCG[net].netsBelowHook[s].bot == net);
	    below = VCG[net].netsBelowHook[s].top;
	    if (! VCG[below].netsBelowReached) {
		SCC_DFSBelowVCG(VCG, below, label);
	    }
	}
    }
    VCG[net].netsBelowLabel = label;
}

void
DumpSCC(ulong * SCC,
	ulong * perSCC)
{
    ulong	net;
    ulong	scc;

    for (scc = 1; scc <= totalSCC; scc++) {
	printf("[%d]\t", scc);
	for (net = 1; net <= channelNets; net++) {
	    if (SCC[net] == scc) {
		printf("%d ", net);
	    }
	}
	printf("<%d>", perSCC[scc]);
	printf("\n");
    }
    printf("\n");
}

void
AcyclicVCG(void)
{
    ulong	done;
    ulong	scc;
    ulong	net;
    ulong      	top;
    ulong	bot;
    ulong	rep;
    ulong	which;
    ulong     	total;
    ulong     	cycle;
    ulong	acyclic;

    for (net = 1; net <= channelNets; net++) {
	for (which = 0; which < VCG[net].netsAbove; which++) {
	    VCG[net].netsAboveHook[which].removed = FALSE;
	}
	for (which = 0; which < VCG[net].netsBelow; which++) {
	    VCG[net].netsBelowHook[which].removed = FALSE;
	}
    }	

    acyclic = TRUE;
    removeTotalVCG = 0;
    do {
	done = TRUE;

	/*
	 * Check acyclic (and more).
	 */
	DFSClearVCG(VCG);
	SCCofVCG(VCG, SCC, perSCC);
	for (scc = 1; scc <= totalSCC; scc++) {
	    if (perSCC[scc] > 1) {
		acyclic = FALSE;
		done = FALSE;
		break;
	    }
	}

	/*
	 * Attempt to eliminate cycles by the
	 * removal of a constraint from each SCC.
	 */
	if (! done) {
	    RemoveConstraintVCG(VCG, SCC, perSCC, removeVCG);
	}
    } while (! done);

    /*
     * Replace redundant constraints.
     * That is, those constraints which were removed
     * but can be added back without introducing a cycle.
     */
    total = removeTotalVCG;
    for (rep = 0; rep < removeTotalVCG; rep++) {
	/*
	 * Constraint to consider.
	 */
	
	top = (*removeVCG[rep]).top;
	bot = (*removeVCG[rep]).bot;

	/*
	 * Replace above.
	 */
	for (which = 0; which < VCG[top].netsAbove; which++) {
	    if (VCG[top].netsAboveHook[which].bot == bot) {
		VCG[top].netsAboveHook[which].removed = FALSE;
		break;
	    }
	}
	
	/*
	 * Replace below.
	 */
	for (which = 0; which < VCG[bot].netsBelow; which++) {
	    if (VCG[bot].netsBelowHook[which].top == top) {
		VCG[bot].netsBelowHook[which].removed = FALSE;
		break;
	    }
	}

	/*
	 * Does replacement introduce a cycle?
	 */
	cycle = FALSE;
	DFSClearVCG(VCG);
	SCCofVCG(VCG, SCC, perSCC);
	for (scc = 1; scc <= totalSCC; scc++) {
	    if (perSCC[scc] > 1) {
		cycle = TRUE;
		break;
	    }
	}
	if (cycle) {
	    /*
	     * Introduces cycle.
	     * Remove constraint (again).
	     */
	    for (which = 0; which < VCG[top].netsAbove; which++) {
		if (VCG[top].netsAboveHook[which].bot == bot) {
		    VCG[top].netsAboveHook[which].removed = TRUE;
		    break;
		}
	    }
	    for (which = 0; which < VCG[bot].netsBelow; which++) {
		if (VCG[bot].netsBelowHook[which].top == top) {
		    VCG[bot].netsBelowHook[which].removed = TRUE;
		    break;
		}
	    }
	}
	else {
	    /*
	     * Does not introduce cycle.
	     * Replace ok.
	     */
	    total--;
	}
    }

    if (acyclic) {
	printf("\n*** Input is acyclic! ***\n");
    }
    else {
	printf("\n*** Input is cyclic! ***\n");
	printf("*** VC's removed (%d) ***\n", total);
    }
}

void
RemoveConstraintVCG(nodeVCGType * VCG,
		    ulong * SCC,
		    ulong * perSCC,
		    constraintVCGType * * removeVCG)
{
    ulong			scc;
    ulong			net;
    ulong			which;
    ulong			best;
    ulong			weight;
    ulong			top;
    ulong			bot;
    ulong			col;
    constraintVCGType *	remove;

    for (scc = 1; scc <= totalSCC; scc++) {
	/*
	 * For each SCC attempt to remove cycle.
	 */
	if (perSCC[scc] > 1) {
	    /*
	     * SCC of more than one net in SCC, thus cycle.
	     */
	    remove = NULL;
	    best = FULL + 1;
	    for (net = 1; net <= channelNets; net++) {
		/*
		 * For each net in the SCC.
		 */
		if (SCC[net] == scc) {
		    /*
		     * Choose constraint to remove.
		     * Consider only constraints within SCC.
		     */
		    for (which = 0; which < VCG[net].netsAbove; which++) {
			bot = VCG[net].netsAboveHook[which].bot;
			if ((SCC[bot] == scc) && (! VCG[net].netsAboveHook[which].removed)) {
			    /*
			     * Constraint within SCC.
			     * Weigh its removal.
			     */

			    /*
			     * Note: we consider the column from which the
			     * constraint was added only.  That is, since
			     * we do not add the same constraint twice, only
			     * first occurrence of the constraint has the
			     * column location registered.  Thus, when the
			     * columns are inspected to choose the easiest to
			     * route constraint to remove, the weight of the
			     * chosen constraint will only reflect that single
			     * column.  This may lead to a poor choice, but
			     * it is not likely to occur in practice, and it
			     * is not worth the effort to handle the problem.
			     */
			    col = VCG[net].netsAboveHook[which].col;
			    weight = 0;
			    if (col == 1) {
				weight += 3; /* no left column */
				if (TOP[col+1] && BOT[col+1]) {
				    weight += 3;
				}
				else if (! (TOP[col+1] || BOT[col+1])) {
				}
				else {
				    weight += 2;
				}
			    }
			    else if (col == channelColumns) {
				weight += 3; /* no right column */
				if (TOP[col-1] && BOT[col-1]) {
				    weight += 3;
				}
				else if (! (TOP[col-1] || BOT[col-1])) {
				}
				else {
				    weight += 2;
				}
			    }
			    else {
				if (TOP[col-1] && BOT[col-1]) {
				    weight += 3;
				}
				else if (! (TOP[col-1] || BOT[col-1])) {
				}
				else {
				    weight += 2;
				}
				if (TOP[col+1] && BOT[col+1]) {
				    weight += 3;
				}
				else if (! (TOP[col+1] || BOT[col+1])) {
				}
				else {
				    weight += 2;
				}
			    }
			    
			    /*
			     * Update best.
			     */
			    if (weight < best) {
				best = weight;
				remove = &VCG[net].netsAboveHook[which];
			    }
			}
		    }
		}
	    }

	    /*
	     * Remove.
	     */
	    assert(remove != NULL);
	    fflush(stdout);
	    assert(removeTotalVCG < ((channelNets + 1) * (channelNets + 1)));
	    removeVCG[removeTotalVCG] = remove;
	    removeTotalVCG++;
	    top = (*remove).top;
	    bot = (*remove).bot;

	    /*
	     * Remove above constraint.
	     */
	    (*remove).removed = TRUE;

	    /*
	     * Remove below constraint.
	     */
	    for (which = 0; which < VCG[bot].netsBelow; which++) {
		if (VCG[bot].netsBelowHook[which].top == top) {
		    VCG[bot].netsBelowHook[which].removed = TRUE;
		    break;
		}
	    }
	}
    }
}

ulong
ExistPathAboveVCG(nodeVCGType * VCG,
		  ulong above,
		  ulong below)
{
    DFSClearVCG(VCG);
    DFSAboveVCG(VCG, above);
    return(VCG[below].netsAboveReached);
}

void
LongestPathVCG(nodeVCGType * VCG,
	       ulong net)
{
    ulong	track;
    ulong	bot;
    ulong	top;
    ulong	not;

    /*
     * How many nets this net is above (including this net)?
     * That is, longest path through nets which this net
     * is above.
     */
    DFSClearVCG(VCG);
    cardBotNotPref = DFSAboveLongestPathVCG(VCG, net) - 1;
    bot = cardBotNotPref;
    for (track = channelTracks; track >= 1; track--) {
	if (bot > 0) {
	    tracksBotNotPref[track] = TRUE;
	    bot--;
	}
	else {
	    tracksBotNotPref[track] = FALSE;
	}
    }
    
    /*
     * How many nets this net is below (including this net)?
     * That is, longest path through nets which this net
     * is below.
     */
    DFSClearVCG(VCG);
    cardTopNotPref = DFSBelowLongestPathVCG(VCG, net) - 1;
    top = cardTopNotPref;
    for (track = 1; track <= channelTracks; track++) {
	if (top > 0) {
	    tracksTopNotPref[track] = TRUE;
	    top--;
	}
	else {
	    tracksTopNotPref[track] = FALSE;
	}
    }

    /*
     * How many tracks are guaranteed to make an HCV?
     * That is, what tracks contain nets which this
     * net must either be above or below.
     */

    not = 0;
    for (track = 1; track <= channelTracks; track++) {
	if (tracksTopNotPref[track] || tracksBotNotPref[track]) {
	    tracksNotPref[track] = TRUE;
	    not++;
	}
	else {
	    tracksNotPref[track] = FALSE;
	}
    }
    cardNotPref = not;
}

ulong
DFSAboveLongestPathVCG(nodeVCGType * VCG,
		       ulong net)
{
    ulong	s;
    ulong 	above;
    ulong	path;
    ulong	longest;

    longest = 0;
    VCG[net].netsAboveReached = TRUE;
    for (s = 0; s < VCG[net].netsAbove; s++) {
	if (! VCG[net].netsAboveHook[s].removed) {
	    assert(VCG[net].netsAboveHook[s].top == net);
	    above = VCG[net].netsAboveHook[s].bot;
	    if (! VCG[above].netsAboveReached) {
		path = DFSAboveLongestPathVCG(VCG, above);
		if (path > longest) {
		    longest = path;
		}
	    }
	}
    }
    return(longest+1);
}

ulong
DFSBelowLongestPathVCG(nodeVCGType * VCG,
		       ulong net)
{
    ulong	s;
    ulong 	below;
    ulong	path;
    ulong	longest;

    longest = 0;
    VCG[net].netsBelowReached = TRUE;
    for (s = 0; s < VCG[net].netsBelow; s++) {
	if (! VCG[net].netsBelowHook[s].removed) {
	    assert(VCG[net].netsBelowHook[s].bot == net);
	    below = VCG[net].netsBelowHook[s].top;
	    if (! VCG[below].netsBelowReached) {
		path = DFSBelowLongestPathVCG(VCG, below);
		if (path > longest) {
		    longest = path;
		}
	    }
	}
    }
    return(longest+1);
}

ulong
VCV(nodeVCGType * VCG,
    ulong check,
    ulong track,
    ulong * assign)
{
    ulong	net;
    ulong	vcv;

    vcv = 0;
    for (net = 1; net <= channelNets; net++) {
	if (assign[net]) {
	    if (assign[net] < track) {
		/*
		 * Above VCV?
		 */
		if (ExistPathAboveVCG(VCG, net, check)) {
		    vcv++;
		}
	    }
	    else if (assign[net] > track) {
		/*
		 * Below VCV?
		 */
		if (ExistPathAboveVCG(VCG, check, net)) {
		    vcv++;
		}
	    }
	    else {
		/*
		 * Above or Below VCV?
		 */
		if (ExistPathAboveVCG(VCG, net, check) || ExistPathAboveVCG(VCG, check, net)) {
		    vcv++;
		}
	    }
	}
    }
    return(vcv);
}
