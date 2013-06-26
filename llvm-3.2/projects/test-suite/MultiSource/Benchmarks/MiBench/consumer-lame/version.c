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


#include "version.h"
#include "lame.h"
#include <stdio.h>

static char lpszVersion[80];

void lame_print_version(FILE *ofile) {
  fprintf(ofile,"LAME version %s (www.sulaco.org/mp3) \n",get_lame_version());
  fprintf(ofile,"GPSYCHO: GPL psycho-acoustic and noise shaping model version %s. \n",get_psy_version());
#ifdef LIBSNDFILE
  fprintf(ofile,"Input handled by libsndfile (www.zip.com.au/~erikd/libsndfile)\n");
#endif
}


char* get_lame_version(void)
{
	if (LAME_ALPHAVERSION>0)
		sprintf(lpszVersion,"%d.%02d (alpha %d)",LAME_MAJOR_VERSION,LAME_MINOR_VERSION,LAME_ALPHAVERSION);
	else if (LAME_BETAVERSION>0)
		sprintf(lpszVersion,"%d.%02d (beta %d)",LAME_MAJOR_VERSION,LAME_MINOR_VERSION,LAME_BETAVERSION);
	else
		sprintf(lpszVersion,"%d.%02d",LAME_MAJOR_VERSION,LAME_MINOR_VERSION);
	return lpszVersion;
}

char* get_psy_version(void)
{
	if (PSY_ALPHAVERSION>0)
		sprintf(lpszVersion,"%d.%02d (alpha %d)",PSY_MAJOR_VERSION,PSY_MINOR_VERSION,PSY_ALPHAVERSION);
	else if (PSY_BETAVERSION>0)
		sprintf(lpszVersion,"%d.%02d (beta %d)",PSY_MAJOR_VERSION,PSY_MINOR_VERSION,PSY_BETAVERSION);
	else
		sprintf(lpszVersion,"%d.%02d",PSY_MAJOR_VERSION,PSY_MINOR_VERSION);
	return lpszVersion;
}

char* get_mp3x_version(void)
{
	if (MP3X_ALPHAVERSION>0)
		sprintf(lpszVersion,"%d:%02d (alpha %d)",MP3X_MAJOR_VERSION,MP3X_MINOR_VERSION,MP3X_ALPHAVERSION);
	else if (MP3X_BETAVERSION>0)
		sprintf(lpszVersion,"%d:%02d (beta %d)",MP3X_MAJOR_VERSION,MP3X_MINOR_VERSION,MP3X_BETAVERSION);
	else
		sprintf(lpszVersion,"%d:%02d",MP3X_MAJOR_VERSION,MP3X_MINOR_VERSION);
	return lpszVersion;
}
