/*
 *
 * hcg.h
 *
 */


/*
 *
 * Includes.
 *
 */

#include "types.h"


#ifndef HCG_H
#define HCG_H

/*
 *
 * Defines.
 *
 */


/*
 *
 * Types.
 *
 */

typedef struct _nodeHCGType {
    ulong *	netsHook;
    ulong	nets;
    ulong	netsReached;
} nodeHCGType;


/*
 *
 * Globals.
 *
 */

#ifdef HCG_CODE

nodeHCGType *			HCG;
ulong *				storageRootHCG;
ulong *				storageHCG;
ulong					storageLimitHCG;

#else	/* HCG_CODE */

extern nodeHCGType *			HCG;
extern ulong *			storageRootHCG;
extern ulong *			storageHCG;
extern ulong				storageLimitHCG;

#endif	/* HCG_CODE */


/*
 *
 * Prototypes.
 *
 */

#ifdef HCG_CODE

void
AllocHCG(void);

void
FreeHCG(void);

void
BuildHCG(void);

void
DFSClearHCG(nodeHCGType *);

void
DumpHCG(nodeHCGType *);

void
NoHCV(nodeHCGType *,
      ulong,
      ulong *,
      ulong *);

#else	/* HCG_CODE */

extern void
AllocHCG(void);

extern void
FreeHCG(void);

extern void
BuildHCG(void);

extern void
DFSClearHCG(nodeHCGType *);

extern void
DumpHCG(nodeHCGType *);

extern void
NoHCV(nodeHCGType *,
      ulong,
      ulong *,
      ulong *);

#endif	/* HCG_CODE */

#endif	/* HCG_H */
