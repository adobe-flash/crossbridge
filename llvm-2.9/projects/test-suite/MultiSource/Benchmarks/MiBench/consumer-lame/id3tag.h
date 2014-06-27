#ifndef ID3TAG_H_INCLUDED
#define ID3TAG_H_INCLUDED
typedef struct
{
	int used;
	int valid;
	char title[31];
	char artist[31];
	char album[31];
	char year[5];
	char comment[31];
	char tagtext[128];
	char genre[1];
	unsigned char track;

}   ID3TAGDATA;

void id3_inittag(ID3TAGDATA *tag);
void id3_buildtag(ID3TAGDATA *tag);
int id3_writetag(char* filename, ID3TAGDATA *tag);


/*
 * Array of all possible music genre. Grabbed from id3ed
 */
extern ID3TAGDATA id3tag;          /* id3tag info */
extern int genre_last;
extern char *genre_list[];
#endif
