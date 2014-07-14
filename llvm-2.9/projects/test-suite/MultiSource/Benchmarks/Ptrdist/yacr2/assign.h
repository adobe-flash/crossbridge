/*
 *
 * assign.h
 *
 */


/*
 *
 * Includes.
 *
 */

#include "types.h"
#include "vcg.h"
#include "hcg.h"

#ifndef ASSIGN_H
#define ASSIGN_H


/*
 *
 * Defines.
 *
 */

#define	LOW		1
#define	MEDIUM		100
#define	HIGH		10000
#define	INFINITY	1000000


/*
 *
 * Types.
 *
 */


/*
 *
 * Globals.
 *
 */

#ifdef ASSIGN_CODE

long * *			costMatrix;
ulong *			tracksNoHCV;
ulong *			tracksNotPref;
ulong *			tracksTopNotPref;
ulong *			tracksBotNotPref;
ulong				cardNotPref;
ulong				cardTopNotPref;
ulong				cardBotNotPref;
ulong *			tracksAssign;
ulong *			netsAssign;
ulong *			netsAssignCopy;

#else	/* ASSIGN_CODE */

extern ulong * *		costMatrix;
extern ulong *		tracksNoHCV;
extern ulong *		tracksNotPref;
extern ulong *		tracksTopNotPref;
extern ulong *		tracksBotNotPref;
extern ulong			cardNotPref;
extern ulong			cardTopNotPref;
extern ulong			cardBotNotPref;
extern ulong *		tracksAssign;
extern ulong *		netsAssign;
extern ulong *		netsAssignCopy;

#endif	/* ASSIGN_CODE */


/*
 *
 * Prototypes.
 *
 */

#ifdef ASSIGN_CODE

void
AllocAssign(void);

void
FreeAssign(void);

void
NetsAssign(void);

void
MaxNetsAssign(void);

void
RightNetsAssign(void);

void
LeftNetsAssign(void);

void
Assign(nodeVCGType *,
       ulong *,
       ulong);

void
Select(nodeVCGType *,
       nodeHCGType *,
       ulong *,
       ulong *,
       ulong *);

void
BuildCostMatrix(nodeVCGType *,
		nodeHCGType *,
		ulong *,
		ulong *);

void
IdealTrack(ulong,
	   ulong,
	   ulong,
	   ulong *);

#else	/* ASSIGN_CODE */

extern void
AllocAssign(void);

extern void
FreeAssign(void);

extern void
NetsAssign(void);

extern void
MaxNetsAssign(void);

extern void
RightNetsAssign(void);

extern void
LeftNetsAssign(void);

extern void
Assign(nodeVCGType *,
       ulong *,
       ulong);

extern void
Select(nodeVCGType *,
       nodeHCGType *,
       ulong *,
       ulong *,
       ulong *);

extern void
BuildCostMatrix(nodeVCGType *,
		nodeHCGType *,
		ulong *,
		ulong *);

extern void
IdealTrack(ulong,
	   ulong,
	   ulong,
	   ulong *);

#endif	/* ASSIGN_CODE */

#endif	/* ASSIGN_H */

