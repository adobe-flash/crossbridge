/* Copyright (c) 1991 Sun Wu and Udi Manber.  All Rights Reserved. */
/* multipattern matcher */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#define MAXPAT  256
#define MAXLINE 1024
#define MAXSYM  256
#define MAXMEMBER1 4096
#define MAXPATFILE 260000
#define BLOCKSIZE  8192
#define MAXHASH    8192
#define mm 	   8191
#define max_num    30000  
#define W_DELIM	   128
#define L_DELIM    10 

extern COUNT, FNAME, SILENT, FILENAMEONLY, num_of_matched;
extern INVERSE;
extern WORDBOUND, WHOLELINE, NOUPPER;
extern unsigned char  CurrentFileName[], Progname[]; 
extern total_line;

int LONG  = 0;
int SHORT = 0;
int p_size=0;
unsigned char SHIFT1[MAXMEMBER1];
unsigned char tr[MAXSYM];
unsigned char tr1[MAXSYM];
struct pat_list {
	int  index;
	struct pat_list *next;
} *HASH[MAXHASH];
struct pat_list  *pt, *qt;
unsigned char buf[MAXPATFILE+BLOCKSIZE];
unsigned char pat_spool[MAXPATFILE+2*max_num+MAXPAT];
unsigned char *patt[max_num];
unsigned char pat_len[max_num];

extern char *strncpy(char *, const char *, size_t);

void countline(unsigned char *text, int len)
{
int i;
	for (i=0; i<len; i++) if(text[i] == '\n') total_line++;
}

void m_short( register unsigned char *text, int start, int end  )
{
register unsigned char *textend;
register int  j; 
register struct pat_list *p;
register int pat_index; 
int MATCHED=0;
int OUT=0;
unsigned char *lastout;
unsigned char *qx;

textend = text + end;
lastout = text + start + 1;
text = text + start - 1 ;
while (++text <= textend) {
		p = HASH[*text];
		while(p != 0) {
			pat_index = p->index;
			p = p->next;
			qx = text;
			j = 0;
			while(tr[patt[pat_index][j]] == tr[*(qx++)]) j++;
			if(pat_len[pat_index] <= j) {
				if(text >= textend) return;
                		num_of_matched++;
                		if(FILENAMEONLY || SILENT)  return;
	        		if(COUNT) {
			  		while (*text != '\n') text++;
				}
				else {
			  	    if(FNAME) printf("%s: ",CurrentFileName);
				    if(!INVERSE) {
                          		while(*(--text) != '\n');
                          		while(*(++text) != '\n') putchar(*text);
			  		printf("\n");
					MATCHED = 1;
				    }
				    else {
                          		while(*(--text) != '\n');
					if(lastout < text) OUT=1;
					while(lastout < text) putchar(*lastout++);
					if(OUT) {
						putchar('\n');
						OUT=0;
					}
                          		while(*(++text) != '\n');
					lastout=text+1;
					MATCHED = 1;
				    }
				}
                	}
			if(MATCHED) break;
		}
		MATCHED = 0;
  } /* while */
  if(INVERSE && !COUNT) while(lastout <= textend) putchar(*lastout++);
}

void f_prep(int pat_index, unsigned char *Pattern)
{
int i, m;
register unsigned hash, Mask=15;
	m = p_size;
	for (i = m-1; i>=(1+LONG); i--) {
		hash = (Pattern[i] & Mask);
		hash = (hash << 4) + (Pattern[i-1]& Mask);
		if(LONG) hash = (hash << 4) + (Pattern[i-2] & Mask);
		if(SHIFT1[hash] >= m-1-i) SHIFT1[hash] = m-1-i;
	}
	if(SHORT) Mask = 255;  /* 011111111 */
	hash = 0;
	for(i = m-1; i>=0; i--)  {
	    hash = (hash << 4) + (tr[Pattern[i]]&Mask);
	}
/*
	if(INVERSE) hash = Pattern[1];
*/
	hash=hash&mm;
	qt = (struct pat_list *) malloc(sizeof(struct pat_list));
	qt->index = pat_index;
	pt = HASH[hash];
	qt->next = pt;
	HASH[hash] = qt;
}

void prepf(int fp)
{
    int length=0, i, p=1, num_pat;
    unsigned char *pat_ptr=pat_spool;
    unsigned Mask = 15;
    int num_read;

    while((num_read = read(fp, buf+length, BLOCKSIZE)) > 0) {
	length = length + num_read;
	if(length > MAXPATFILE) {
		fprintf(stderr, "%s: maximum pattern file size is %d\n", Progname, MAXPATFILE);
		exit(2);
	}
    }
    buf[length] = '\n';
    i = 0; p=1;
    while(i<length) {
	patt[p] = pat_ptr;
	if(WORDBOUND) *pat_ptr++ = W_DELIM;
	if(WHOLELINE) *pat_ptr++ = L_DELIM;
	while((*pat_ptr = buf[i++]) != '\n') pat_ptr++;
	if(WORDBOUND) *pat_ptr++ = W_DELIM;
	if(WHOLELINE) *pat_ptr++ = L_DELIM;           /* Can't be both on */
	*pat_ptr++ = 0;
	p++;  
    }
    if(p>max_num) {
	fprintf(stderr, "%s: maximum number of patterns is %d\n", Progname, max_num); 
	exit(2);
    }
    for(i=1; i<20; i++) *pat_ptr = i;  /* boundary safety zone */
    for(i=0; i< MAXSYM; i++) tr[i] = i;
    if(NOUPPER) {
	for(i='A'; i<= 'Z'; i++) tr[i] = i + 'a' - 'A';
    }
    if(WORDBOUND) {
	for(i=0; i<128; i++) if(!isalnum(i)) tr[i] = W_DELIM;
    }
    for(i=0; i< MAXSYM; i++) tr1[i] = tr[i]&Mask;
    num_pat = p-1;
    p_size = MAXPAT;
    for(i=1 ; i <= num_pat; i++) {
	p = strlen(patt[i]);
	pat_len[i] = p;
	if(p!=0 && p < p_size) p_size = p;
    }
    if(p_size == 0) {
	fprintf(stderr, "the pattern file is empty\n");
	exit(2);
    }
    if(length > 400 && p_size > 2) LONG = 1;
    if(p_size == 1) SHORT = 1;
    for(i=0; i<MAXMEMBER1; i++) SHIFT1[i] = p_size - 2;
    for(i=0; i<MAXHASH; i++) {
	HASH[i] = 0;
    }
    for(i=1; i<= num_pat; i++) f_prep(i, patt[i]);
}

void monkey1( register unsigned char *text, int start, int end  )
{
register unsigned char *textend;
register unsigned hash, i;
register unsigned char shift; 
register int  m1, j, Long=LONG; 
int pat_index, m=p_size; 
int MATCHED=0;
register unsigned char *qx;
register struct pat_list *p;
unsigned char *lastout;
int OUT=0;

textend = text + end;
m1 = m - 1;
lastout = text+start+1;
text = text + start + m1 ;
while (text <= textend) {
	hash=tr1[*text];
	hash=(hash<<4)+(tr1[*(text-1)]);
	if(Long) hash=(hash<<4)+(tr1[*(text-2)]);
	shift = SHIFT1[hash];
	if(shift == 0) {
		hash=0;
		for(i=0;i<=m1;i++)  {
		    hash=(hash<<4)+(tr1[*(text-i)]);
		}
		hash=hash&mm;
		p = HASH[hash];
		while(p != 0) {
			pat_index = p->index;
			p = p->next;
			qx = text-m1;
			j = 0;
			while(tr[patt[pat_index][j]] == tr[*(qx++)]) j++;
	        	if (j > m1 ) { 
			   if(pat_len[pat_index] <= j) {
				if(text > textend) return;
                		num_of_matched++;
                		if(FILENAMEONLY || SILENT)  return;
				MATCHED=1;
	        		if(COUNT) {
			  		while (*text != '\n') text++;
				}
				else {
				    if(!INVERSE) {
			  		if(FNAME) printf("%s: ",CurrentFileName);
                          		while(*(--text) != '\n');
                          		while(*(++text) != '\n') putchar(*text);
			  		printf("\n");
				    }
				    else {
			  		if(FNAME) printf("%s: ",CurrentFileName);
                          		while(*(--text) != '\n');
					if(lastout < text) OUT=1;
					while(lastout < text) putchar(*lastout++);
					if(OUT) {
						putchar('\n');
						OUT=0;
					}
                          		while(*(++text) != '\n');
					lastout=text+1;
				    }
				}
/*
				else {
			  		if(FNAME) printf("%s: ",CurrentFileName);
                          		while(*(--text) != '\n');
                          		while(*(++text) != '\n') putchar(*text);
			  		printf("\n");
				}
*/
			   }
                	}
			if(MATCHED) break;
		}
		if(!MATCHED) shift = 1;
		else {
			MATCHED = 0;
			shift = m1;
		}
        }
	text = text + shift;
  }
  if(INVERSE && !COUNT) while(lastout <= textend) putchar(*lastout++);
}

void mgrep(int fd)
{ 
    register char r_newline = '\n';
    unsigned char text[2*BLOCKSIZE+MAXLINE]; 
    register int buf_end, num_read, start, end, residue = 0;

    text[MAXLINE-1] = '\n';  /* initial case */
    start = MAXLINE-1;

    while( (num_read = read(fd, text+MAXLINE, BLOCKSIZE)) > 0) 
    {
       if(INVERSE && COUNT) countline(text+MAXLINE, num_read);
       buf_end = end = MAXLINE + num_read -1 ;
       while(text[end]  != r_newline && end > MAXLINE) end--;
       residue = buf_end - end  + 1 ;
       text[start-1] = r_newline;
       if(SHORT) m_short(text, start, end);
       else      monkey1(text, start, end);
       if(FILENAMEONLY && num_of_matched) {
		printf("%s\n", CurrentFileName);
		return;
       }
       start = MAXLINE - residue;
       if(start < 0) {
            start = 1; 
       }
       strncpy(text+start, text+end, residue);
    } /* end of while(num_read = ... */
    text[MAXLINE] = '\n';
    text[start-1] = '\n';
    if(residue > 1) {
        if(SHORT) m_short(text, start, end);
        else      monkey1(text, start, end);
    }
    return;
} /* end mgrep */






