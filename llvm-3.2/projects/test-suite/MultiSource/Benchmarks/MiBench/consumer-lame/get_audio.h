/*
** Copyright (C) 1999 Albert Faber
**  
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


#ifndef GET_AUDIO_H_INCLUDED
#define GET_AUDIO_H_INCLUDED

#if (defined LIBSNDFILE || defined LAMESNDFILE)


/* AIFF Definitions */

#define IFF_ID_FORM 0x464f524d /* "FORM" */
#define IFF_ID_AIFF 0x41494646 /* "AIFF" */
#define IFF_ID_COMM 0x434f4d4d /* "COMM" */
#define IFF_ID_SSND 0x53534e44 /* "SSND" */
#define IFF_ID_MPEG 0x4d504547 /* "MPEG" */



void CloseSndFile(lame_global_flags *gfp);
FILE * OpenSndFile(lame_global_flags *gfp,const char* lpszFileName,int default_samp, int 
   default_chan);
unsigned long GetSndSamples(void);
int		GetSndSampleRate(void);
int		GetSndChannels(void);
int		GetSndBitrate(void);


int get_audio(lame_global_flags *gfp,short buffer[2][1152],int stereo);



#ifdef LIBSNDFILE
/* INCLUDE the sound library header file */
#ifdef _MSC_VER
	/* one byte alignment for WIN32 platforms */
	#pragma pack(push,1)
	#include "./libsndfile/src/sndfile.h"
	#pragma pack(pop,1)
#else
	#include "sndfile.h"
#endif


#else
/*****************************************************************
 * LAME/ISO built in audio file I/O routines 
 *******************************************************************/
#include "portableio.h"
#include "ieeefloat.h"


typedef struct  blockAlign_struct {
    unsigned long   offset;
    unsigned long   blockSize;
} blockAlign;

typedef struct  IFF_AIFF_struct {
    short           numChannels;
    unsigned long   numSampleFrames;
    short           sampleSize;
    FLOAT           sampleRate;
    unsigned long   sampleType;
    blockAlign      blkAlgn;
} IFF_AIFF;

extern int            aiff_read_headers(FILE*, IFF_AIFF*);
extern int            aiff_seek_to_sound_data(FILE*);
extern int            aiff_write_headers(FILE*, IFF_AIFF*);
extern int parse_wavheader(void);
extern int parse_aiff(const char fn[]);
extern void   aiff_check(const char*, IFF_AIFF*, int*);


#endif	/* ifndef _LIBSNDDLL */
#endif  /* ifdef LAMESNDFILE or LIBSNDFILE */
#endif	/* ifndef GET_AUDIO_H_INCLUDED*/
