
/*
 *
 * channel.h
 *
 */


/*
 *
 * Includes.
 *
 */

#include "types.h"


#ifndef CHANNEL_H
#define CHANNEL_H


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


/*
 *
 * Globals.
 *
 */

#ifdef CHANNEL_CODE

ulong *		TOP;
ulong *		BOT;
ulong *		FIRST;
ulong *		LAST;
ulong *		DENSITY;
ulong *		CROSSING;
ulong			channelNets;
ulong			channelColumns;
ulong			channelTracks;
ulong			channelTracksCopy;
ulong			channelDensity;
ulong			channelDensityColumn;
char *		channelFile;

#else	/* CHANNEL_CODE */

extern ulong *	TOP;
extern ulong *	BOT;
extern ulong *	FIRST;
extern ulong *	LAST;
extern ulong *	DENSITY;
extern ulong *	CROSSING;
extern ulong		channelNets;
extern ulong		channelColumns;
extern ulong		channelTracks;
extern ulong		channelTracksCopy;
extern ulong		channelDensity;
extern ulong		channelDensityColumn;
extern char *	channelFile;

#endif	/* CHANNEL_CODE */


/*
 *
 * Prototypes.
 *
 */

#ifdef CHANNEL_CODE

void
BuildChannel(void);

void
DimensionChannel(void);

void
DescribeChannel(void);

void
DensityChannel(void);

#else	/* CHANNEL_CODE */

extern void
BuildChannel(void);

extern void
DimensionChannel(void);

extern void
DescribeChannel(void);

extern void
DensityChannel(void);

#endif	/* CHANNEL_CODE */

#endif	/* CHANNEL_H */
