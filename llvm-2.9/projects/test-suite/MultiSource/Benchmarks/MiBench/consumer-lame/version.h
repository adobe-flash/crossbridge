/*
 *	Version numbering for LAME.
 *
 *	Copyright (c) 1999 A.L. Faber
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef LAME_H_INCLUDED
#define LAME_H_INCLUDED

#define LAME_MAJOR_VERSION	3	/* Major version number */
#define LAME_MINOR_VERSION	70	/* Minor version number */
#define LAME_ALPHAVERSION	0	/* Set number if this is an alpha version, otherwise zero */
#define LAME_BETAVERSION        0	/* Set number if this is a beta version, otherwise zero */

#define PSY_MAJOR_VERSION	0	/* Major version number */
#define PSY_MINOR_VERSION	77	/* Minor version number */
#define PSY_ALPHAVERSION	0	/* Set number if this is an alpha version, otherwise zero */
#define PSY_BETAVERSION		0	/* Set number if this is a beta version, otherwise zero */

#define MP3X_MAJOR_VERSION	0	/* Major version number */
#define MP3X_MINOR_VERSION	82	/* Minor version number */
#define MP3X_ALPHAVERSION	0	/* Set number if this is an alpha version, otherwise zero */
#define MP3X_BETAVERSION	0	/* Set number if this is a beta version, otherwise zero */

#include "machine.h"
void lame_print_version(FILE *);
char* get_lame_version(void);		/* returns lame version number string */
char* get_psy_version(void);		/* returns psy model version number string */
char* get_mp3x_version(void);		/* returns mp3x version number string */

#endif
