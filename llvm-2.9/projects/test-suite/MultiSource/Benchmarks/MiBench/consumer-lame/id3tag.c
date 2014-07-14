/*
 * functions for writing ID3 tags in LAME
 *
 * text functions stolen from mp3info by Ricardo Cerqueira <rmc@rccn.net>
 * adapted for LAME by Conrad Sanderson <c.sanderson@me.gu.edu.au>
 *
 * 
 */ 
 
#include <stdio.h>
#include <string.h>
#include "id3tag.h"
ID3TAGDATA id3tag;
 
/*
 * If "string" is shorter than "length", pad it with ' ' (spaces)
 */

static void id3_pad(char *string, int length) {
	int l;  l=strlen(string);
	
	while(l<length) { string[l] = ' '; l++; }
	string[l]='\0';
	}


/*
 * initialize temporary fields
 */

void id3_inittag(ID3TAGDATA *tag) {
	strcpy( tag->title, "");
	strcpy( tag->artist, "");
	strcpy( tag->album, "");
	strcpy( tag->year, "");    
	strcpy( tag->comment, "");
	strcpy( tag->genre, "ÿ");	/* unset genre */
	tag->track = 0;

	tag->valid = 0;		/* not ready for writing*/
	}

/*
 * build an ID3 tag from temporary fields
 */

void id3_buildtag(ID3TAGDATA *tag) {
	strcpy(tag->tagtext,"TAG");

	id3_pad( tag->title, 30);   strncat( tag->tagtext, tag->title,30);
	id3_pad( tag->artist, 30);  strncat( tag->tagtext, tag->artist,30);
	id3_pad( tag->album, 30);   strncat( tag->tagtext, tag->album,30);
	id3_pad( tag->year, 4);     strncat( tag->tagtext, tag->year,4);
	id3_pad( tag->comment, 30); strncat( tag->tagtext, tag->comment,30);
	id3_pad( tag->genre, 1);    strncat( tag->tagtext, tag->genre,1);

	if( tag->track != 0 ) {
		tag->tagtext[125] = '\0';
		tag->tagtext[126] = tag->track;
	}
	tag->valid = 1;		/* ready for writing*/
	}

/*
 * write ID3 tag 
 */

int id3_writetag(char* filename, ID3TAGDATA *tag) {
	FILE* f;
	if( ! tag->valid ) return -1;

	f=fopen(filename,"rb+");	if(!f) return -1;

	fseek(f,0,SEEK_END); fwrite(tag->tagtext,1,128,f);
	fclose(f); return 0;
	}





int genre_last=147;
char *genre_list[]={
	"Blues", "Classic Rock", "Country", "Dance", "Disco", "Funk",
	"Grunge", "Hip-Hop", "Jazz", "Metal", "New Age", "Oldies",
	"Other", "Pop", "R&B", "Rap", "Reggae", "Rock",
	"Techno", "Industrial", "Alternative", "Ska", "Death Metal", "Pranks",
	"Soundtrack", "Euro-Techno", "Ambient", "Trip-Hop", "Vocal", "Jazz+Funk",
	"Fusion", "Trance", "Classical", "Instrumental", "Acid", "House",
	"Game", "Sound Clip", "Gospel", "Noise", "AlternRock", "Bass",
	"Soul", "Punk", "Space", "Meditative", "Instrumental Pop", "Instrumental Rock",
	"Ethnic", "Gothic", "Darkwave", "Techno-Industrial", "Electronic", "Pop-Folk",
	"Eurodance", "Dream", "Southern Rock", "Comedy", "Cult", "Gangsta",
	"Top 40", "Christian Rap", "Pop/Funk", "Jungle", "Native American", "Cabaret",
	"New Wave", "Psychadelic", "Rave", "Showtunes", "Trailer", "Lo-Fi",
	"Tribal", "Acid Punk", "Acid Jazz", "Polka", "Retro", "Musical",
	"Rock & Roll", "Hard Rock", "Folk", "Folk/Rock", "National Folk", "Swing",
	"Fast-Fusion", "Bebob", "Latin", "Revival", "Celtic", "Bluegrass", "Avantgarde",
	"Gothic Rock", "Progressive Rock", "Psychedelic Rock", "Symphonic Rock", "Slow Rock", "Big Band",
	"Chorus", "Easy Listening", "Acoustic", "Humour", "Speech", "Chanson",
	"Opera", "Chamber Music", "Sonata", "Symphony", "Booty Bass", "Primus",
	"Porn Groove", "Satire", "Slow Jam", "Club", "Tango", "Samba",
	"Folklore", "Ballad", "Power Ballad", "Rhythmic Soul", "Freestyle", "Duet",
	"Punk Rock", "Drum Solo", "A capella", "Euro-House", "Dance Hall",
	"Goa", "Drum & Bass", "Club House", "Hardcore", "Terror",
	"Indie", "BritPop", "NegerPunk", "Polsk Punk", "Beat",
	"Christian Gangsta", "Heavy Metal", "Black Metal", "Crossover", "Contemporary C",
	"Christian Rock", "Merengue", "Salsa", "Thrash Metal", "Anime", "JPop",
	"SynthPop",
};


