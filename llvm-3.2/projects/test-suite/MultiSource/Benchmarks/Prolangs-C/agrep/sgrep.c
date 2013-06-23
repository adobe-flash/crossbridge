/* Copyright (c) 1991 Sun Wu and Udi Manber.  All Rights Reserved. */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#define MAXSYM  256
#define MAXMEMBER 8192
#define	CHARTYPE	unsigned char
#define MaxError 20
#define MAXPATT 256
#define MAXLINE 1024
#define MaxCan  2048
#define BLOCKSIZE    8192
#define MAX_SHIFT_2  4096
#define ON      1
#define LOG_ASCII 8
#define LOG_DNA  3
#define MAXMEMBER_1 65536
#define LONG_EXAC  20
#define LONG_APPX  24
#define W_DELIM    128

extern COUNT, FNAME, SILENT, FILENAMEONLY, num_of_matched;
extern DNA ;  /* DNA flag is set in checksg when pattern is DNA pattern and
		 p_size > 16  */
extern WORDBOUND, WHOLELINE, NOUPPER;
extern unsigned char CurrentFileName[],  Progname[]; 
extern unsigned Mask[];
extern unsigned endposition;

unsigned char BSize;                /* log_c m   */
unsigned char char_map[MAXSYM];
	

/* data area */
int shift_1;
CHARTYPE SHIFT[MAXSYM];
CHARTYPE MEMBER[MAXMEMBER];
CHARTYPE pat[MAXPATT];
unsigned Hashmask;
char MEMBER_1[MAXMEMBER_1];
CHARTYPE TR[MAXSYM];

void char_tr(unsigned char *pat, int *m)
{
int i;
unsigned char temp[MAXPATT];
	for(i=0; i<MAXSYM; i++) TR[i] = i;
	if(NOUPPER) {
		for(i='A'; i<= 'Z'; i++) TR[i] = i + 'a' - 'A';
	}
	if(WORDBOUND) { /* SUN: To be added to be more complete */
		for(i=0; i<128; i++) {
			if(!isalnum(i)) TR[i] = W_DELIM;
		}
	}
	if(WHOLELINE) {
		memcpy(temp, pat, *m);
		pat[0] = '\n';
		memcpy(pat+1, temp, *m);
		pat[*m+1] = '\n';
		pat[*m+2] = 0;
		*m = *m + 2;
	}
}

void s_output (CHARTYPE *text, int *i)
{
int bp;
        if(SILENT) return;
        if(COUNT) {
		while(text[*i] != '\n') *i = *i + 1; 
		return;
	}
        if(FNAME == ON) printf("%s: ", CurrentFileName);
        bp = *i;
        while(text[--bp] != '\n');
        while(text[++bp] != '\n') putchar(text[bp]);
        putchar('\n');
        *i = bp;
}

int verify(register int m, register int n, register int D, CHARTYPE *pat, CHARTYPE *text)
{   
    int A[MAXPATT], B[MAXPATT];
    register int last = D;      
    register int cost = 0;  
    register int k, i, c;
    register int m1 = m+1;
    CHARTYPE *textend = text+n;
    CHARTYPE *textbegin = text;

   for (i = 0; i <= m1; i++)  A[i] = B[i] = i;
   while (text < textend)
   {
       for (k = 1; k <= last; k++)
       {
           cost = B[k-1]+1; 
           if (pat[k-1] != *text)
           {   if (B[k]+1 < cost) cost = B[k]+1; 
               if (A[k-1]+1 < cost) cost = A[k-1]+1; }
           else cost = cost -1; 
           A[k] = cost; 
       }
       if(pat[last] == *text++) { A[last+1] = B[last]; last++; }
       if(A[last] < D) A[last+1] = A[last++]+1;
       while (A[last] > D) last = last - 1;
       if(last >= m) return(text - textbegin - 1);
       if(*text == '\n') {
            last = D;
	    for(c = 0; c<=m1; c++) A[c] = B[c] = c;
       }
       for (k = 1; k <= last; k++)
       {
           cost = A[k-1]+1; 
           if (pat[k-1] != *text)
           {   if (A[k]+1 < cost) cost = A[k]+1; 
               if (B[k-1]+1 < cost) cost = B[k-1]+1; }
           else cost = cost -1; 
           B[k] = cost;
       }
       if(pat[last] == *text++) { B[last+1] = A[last]; last++; }
       if(B[last] < D) B[last+1] = B[last++]+1;
       while (B[last] > D) last = last -1;
       if(last >= m)   return(text - textbegin - 1);
       if(*text == '\n') {
            last = D;
	    for(c = 0; c<=m1; c++) A[c] = B[c] = c;
       }
   }    
   return(0);
}

/* SUN: bm assumes that the content of text[n]...text[n+m-1] is 
pat[m-1] such that the skip loop is guaranteed to terminated */

void bm(CHARTYPE *pat, int m, CHARTYPE *text, CHARTYPE *textend)
{
register int shift;
register int  m1, j, d1; 

/*
printf("%d\t", textend - text);
printf("%c, %c", *text, *textend);
*/

d1 = shift_1;    /* at least 1 */
m1 = m - 1;
shift = 0;       
while (text <= textend) {
	shift = SHIFT[*(text += shift)];
	while(shift) {          
		shift = SHIFT[*(text += shift)];
		shift = SHIFT[*(text += shift)];
		shift = SHIFT[*(text += shift)];
	}
		j = 0;
		while(TR[pat[m1 - j]] == TR[*(text - j)]) {
			if(++j == m)  break;       /* if statement can be
						    saved, but for safty ... */
		}
	        if (j == m ) { 
			if(text > textend) return;
			if(WORDBOUND) {
				if(TR[*(text+1)] != W_DELIM) goto CONT;
				if(TR[*(text-m)] != W_DELIM) goto CONT;
			}
			num_of_matched++;
			if(FILENAMEONLY) return;
			if(!(COUNT)) {
				if(FNAME) printf("%s: ", CurrentFileName);
				while(*(--text) != '\n');
				while(*(++text) != '\n') putchar(*(text));
				putchar(*text);
			}
			else { while(*text != '\n') text++; } 
CONT:
			shift = 1;
                }
		else shift = d1;
  }
return;
}

  
/* initmask() initializes the mask table for the pattern                    */ 
/* endposition is a mask for the endposition of the pattern                 */
/* endposition will contain k mask bits if the pattern contains k fragments */
void initmask(CHARTYPE *pattern, unsigned *Mask, register int m, register int D, unsigned *endposition)
{
  register unsigned Bit1, c;
  register int i, j, frag_num;

  Bit1 = 1 << 31;    /* the first bit of Bit1 is 1, others 0.  */
  frag_num = D+1; *endposition = 0;
  for (i = 0; i < frag_num; i++) *endposition = *endposition | (Bit1 >> i);
  *endposition = *endposition >> (m - frag_num);
  for(i = 0; i < m; i++) 
          if (pattern[i] == '^' || pattern[i] == '$') {
              pattern[i] = '\n'; 
          }
  for(i = 0; i < MAXSYM; i++) Mask[i] = ~0;
  for(i = 0; i < m; i++)     /* initialize the mask table */
  {  c = pattern[i];
     for ( j = 0; j < m; j++)
           if( c == pattern[j] )
               Mask[c] = Mask[c] & ~( Bit1 >> j ) ;
  }
}

void prep(CHARTYPE *Pattern, register int M, register int D)  /* preprocessing for partitioning_bm */
{
register int i, j, k, p, shift;
register unsigned m;
unsigned hash, b_size = 3;
	m = M/(D+1);
	p = M - m*(D+1);
	for (i = 0; i < MAXSYM; i++) SHIFT[i] = m;
	for (i = M-1; i>=p ; i--) {
		shift = (M-1-i)%m;
		hash = Pattern[i];
		if(SHIFT[hash] > shift) SHIFT[hash] = shift;
	}
#ifdef DEBUG
	for(i=0; i<M; i++) printf(" %d,", SHIFT[Pattern[i]]);
	printf("\n");
#endif
	shift_1 = m;
	for(i=0; i<D+1; i++) {
		j = M-1 - m*i;
		for(k=1; k<m; k++) {
			for(p=0; p<D+1; p++) 
				if(Pattern[j-k] == Pattern[M-1-m*p]) 
					if(k < shift_1) shift_1 = k;
		}
	}
#ifdef DEBUG
	printf("\nshift_1 = %d", shift_1);
#endif
	if(shift_1 == 0) shift_1 = 1;
	for(i=0; i<MAXMEMBER; i++) MEMBER[i] = 0;
	if (m < 3) b_size = m;
	for(i=0; i<D+1; i++) {
		j = M-1 - m*i;
		hash = 0;
		for(k=0; k<b_size; k++) {
			hash = (hash << 2) + Pattern[j-k];
		}
#ifdef DEBUG
	printf(" hash = %d,", hash);
#endif
		MEMBER[hash] = 1;
	}
}


void agrep( register CHARTYPE *pat, int M, register CHARTYPE *text, register CHARTYPE *textend, int D )
{
  register int i;
  register int m = M/(D+1);
  register CHARTYPE *textstart;
  register int shift, HASH;
  int  j=0, k, d1;
  int  n, cdx;
  int  Candidate[MaxCan][2], round, lastend=0;
  unsigned R1[MaxError+1], R2[MaxError+1]; 
  register unsigned int r1, endpos, c; 
  unsigned currentpos;
  unsigned Bit1;
  unsigned r_newline;

  Candidate[0][0] = Candidate[0][1] = 0; 
  d1 = shift_1;
  cdx = 0;
  if(m < 3) r1 = m;
  else r1 = 3;
  textstart = text;
  shift = m-1;
  while (text < textend) {
	shift = SHIFT[*(text += shift)];
        if (text >= textend) break;
	while(shift) {
		shift = SHIFT[*(text += shift)];
                if (text >= textend) break;
		shift = SHIFT[*(text += shift)];
                if (text >= textend) break;
	}
        if (text >= textend) break;
		j = 1; HASH = *text;
		while(j < r1) { HASH = (HASH << 2) + *(text-j);
				j++; }
	        if (MEMBER[HASH]) { 
			i = text - textstart;
                     	if((i - M - D - 10) > Candidate[cdx][1]) { 	
				Candidate[++cdx][0] = i-M-D-2;
                          	Candidate[cdx][1] = i+M+D; }
                     	else Candidate[cdx][1] = i+M+D;
			shift = d1;
                }
		else shift = d1;
  }


  text = textstart;
  n = textend - textstart;
  r_newline = '\n';
  /* for those candidate areas, find the D-error matches                     */
  if(Candidate[1][0] < 0) Candidate[1][0] = 0;
  endpos = endposition;                /* the mask table and the endposition */
  Bit1 = (1 << 31);
  for(round = 0; round <= cdx; round++)
  {  i = Candidate[round][0] ; 
     if(Candidate[round][1] > n) Candidate[round][1] = n;
     if(i < 0) i = 0;
#ifdef DEBUG
     printf("round: %d, start=%d, end=%d, ", round, i, Candidate[round][1]);
#endif
     R1[0] = R2[0] = ~0;
     R1[1] = R2[1] = ~Bit1;
     for(k = 1; k <= D; k++) R1[k] = R2[k] = (R1[k-1] >> 1) & R1[k-1];
     while (i < Candidate[round][1])                     
     {  
	    c = text[i++];
            if(c == r_newline) {
               for(k = 0 ; k <= D; k++) R1[k] = R2[k] = (~0 );
            }
            r1 = Mask[c];
            R1[0] = (R2[0] >> 1) | r1;
            for(k=1; k<=D; k++)
                R1[k] = ((R2[k] >> 1) | r1) & R2[k-1] & ((R1[k-1] & R2[k-1]) >> 1);
            if((R1[D] & endpos) == 0) { 
                                    num_of_matched++;
                                    if(FILENAMEONLY) { return; }
                                    currentpos = i;
                                    if(i <= lastend) i = lastend;
                                    else {
                                       s_output(text, &currentpos); 
                                       i = currentpos; 
                                    }
                                    lastend = i;
                                    for(k=0; k<=D; k++) R1[k] = R2[k] = ~0;
                                  }
            c = text[i++];
            if(c == r_newline) {
                for(k = 0 ; k <= D; k++) R1[k] = R2[k] = (~0 );
            }
            r1 = Mask[c];
            R2[0] = (R1[0] >> 1) | r1;
            for(k = 1; k <= D; k++)
                R2[k] = ((R1[k] >> 1) | r1) & R1[k-1] & ((R1[k-1] & R2[k-1]) >> 1);
            if((R2[D] & endpos) == 0) { currentpos = i;
                                    num_of_matched++;
                                    if(FILENAMEONLY) { return; }
                                    if(i <= lastend) i = lastend;
                                    else {
                                       s_output(text, &currentpos); 
                                       i = currentpos; 
                                    }
                                    lastend = i;
                                    for(k=0; k<=D; k++) R1[k] = R2[k] = ~0;
                                  }
     }
  }
  return;
}

void prep_bm(unsigned char *Pattern, register m)
{
int i;
unsigned hash;
unsigned char lastc;
	for (i = 0; i < MAXSYM; i++) SHIFT[i] = m;
	for (i = m-1; i>=0; i--) {
		hash = TR[Pattern[i]];
		if(SHIFT[hash] >= m - 1) SHIFT[hash] = m-1-i;
	}
	shift_1 = m-1;
	lastc = TR[Pattern[m-1]];
	for (i= m-2; i>=0; i--) {
		if(TR[Pattern[i]] == lastc )
			{ shift_1 = m-1 - i;  i = -1; }
	}
	if(shift_1 == 0) shift_1 = 1;
	if(NOUPPER) for(i='A'; i<='Z'; i++) SHIFT[i] = SHIFT[i +  'a' - 'A'];
#ifdef DEBUG
	for(i='a'; i<='z'; i++) printf("%c: %d", i, SHIFT[i]); printf("\n");
	for(i='A'; i<='Z'; i++) printf("%c: %d", i, SHIFT[i]); printf("\n");
#endif
}


/* a_monkey() the approximate monkey move */

void a_monkey( register CHARTYPE *pat, register int m, register CHARTYPE *text, register CHARTYPE *textend,
               register int D ) 
{
register CHARTYPE *oldtext;
register unsigned hash, hashmask, suffix_error; 
register int  m1 = m-1-D, pos; 

  	hashmask = Hashmask;
  	oldtext  = text;
  	while (text < textend) {
     		text = text+m1;
     		suffix_error = 0;
     		while(suffix_error <= D) {
			hash = *text--;
			while(MEMBER_1[hash]) {
	      			hash = ((hash << LOG_ASCII) + *(text--)) & hashmask;
			}
			suffix_error++;
     		}
     		if(text <= oldtext) {
		           if((pos = verify(m, 2*m+D, D, pat, oldtext)) > 0)  {
				text = oldtext+pos;
				if(text > textend) return;
				num_of_matched++;
				if(FILENAMEONLY) return;
				if(!(COUNT)) {
					if(FNAME) printf("%s: ", CurrentFileName);
					while(*(--text) != '\n');
			 		while(*(++text) != '\n') putchar(*text);
			        	printf("\n");
				}
				else {
					while(*text != '\n') text++;
				}
			   }
			   else { 
			        text = oldtext + m;
			   }
     		}
     		oldtext = text; 
  	}
}

/* monkey uses two characters for delta_1 shifting */

CHARTYPE SHIFT_2[MAX_SHIFT_2];

void monkey( register CHARTYPE *pat, register int m, register CHARTYPE *text, register CHARTYPE *textend )
{
register unsigned hash; 
register CHARTYPE shift;
register int  m1, j; 
register unsigned r_newline;

r_newline = '\n';

  m1 = m - 1;
  text = text+m1;
  while (text < textend) {
	hash = *text;
	hash = (hash << 3) + *(text-1);
	shift = SHIFT_2[hash];
	while(shift) {
		text = text + shift;
		hash = (*text << 3) + *(text-1);
		shift = SHIFT_2[hash];
	}
	j = 0;
	while(TR[pat[m1 - j]] == TR[*(text - j)]) { if(++j == m) break; }
	if (j == m ) { 
		if(text >= textend) return;
                num_of_matched++;
                if(FILENAMEONLY)  return;
	        if(COUNT) {
			  while (*text != r_newline) text++;
			  text--;
		}
		else {
			  if(FNAME) printf("%s: ", CurrentFileName);
                          while(*(--text) != r_newline);
                          while(*(++text) != r_newline) putchar(*text);
			  printf("\n");
			  text--;
		}
        }
	text++;
  }
}
 
void am_preprocess(CHARTYPE *Pattern)
{
int i, m;
	m = strlen(Pattern);
	for (i = 1, Hashmask = 1 ; i<16 ; i++) Hashmask = (Hashmask << 1) + 1 ;
	for (i = 0; i < MAXMEMBER_1; i++) MEMBER_1[i] = 0;
	for (i = m-1; i>=0; i--) {
		MEMBER_1[Pattern[i]] = 1;
        }
	for (i = m-1; i > 0; i--) {
	   	MEMBER_1[(Pattern[i] << LOG_ASCII) + Pattern[i-1]] = 1;
	}
}

/* preprocessing for monkey()   */

void m_preprocess(CHARTYPE *Pattern)
{
int i, j, m;
unsigned hash;
	m = strlen(Pattern);
	for (i = 0; i < MAX_SHIFT_2; i++) SHIFT_2[i] = m;
	for (i = m-1; i>=1; i--) {
		hash = Pattern[i];
		hash = hash << 3;
		for (j = 0; j< MAXSYM; j++) {
			if(SHIFT_2[hash+j] == m) SHIFT_2[hash+j] = m-1;
		}
		hash = hash + Pattern[i-1];
		if(SHIFT_2[hash] >= m - 1) SHIFT_2[hash] = m-1-i;
	}
	shift_1 = m-1;
	for (i= m-2; i>=0; i--) {
		if(Pattern[i] == Pattern[m-1] )
			{ shift_1 = m-1 - i;  i = -1; }
	}
	if(shift_1 == 0) shift_1 = 1;
	SHIFT_2[0] = 0;
}

/* monkey4() the approximate monkey move */

char *MEMBER_D;

void monkey4( register unsigned char *pat, register int m, register unsigned char *text, register unsigned char *textend,
              register int D  )
{
register unsigned char *oldtext;
register unsigned hash, hashmask, suffix_error; 
register int m1=m-1-D, pos; 

  hashmask = Hashmask;
  oldtext = text ;
  while (text < textend) {
     text = text + m1;
     suffix_error = 0;
     while(suffix_error <= D) {
	hash = char_map[*text--];
	hash = ((hash << LOG_DNA) + char_map[*(text--)]) & hashmask;
	while(MEMBER_D[hash]) {
	      hash = ((hash << LOG_DNA) + char_map[*(text--)]) & hashmask;
	}
	suffix_error++;
     }
     if(text <= oldtext) {
		           if((pos = verify(m, 2*m+D, D, pat, oldtext)) > 0)  {
				text = oldtext+pos;
				if(text > textend) return;
				num_of_matched++;
				if(FILENAMEONLY) return;
				if(!(COUNT)) {
					if(FNAME) printf("%s:", CurrentFileName);
					while(*(--text) != '\n');
			 		while(*(++text) != '\n') putchar(*text);
			        	printf("\n");
					text++;
				}
				else {
					while(*text != '\n') text++;
					text++;
				}
			   }
			   else text = oldtext + m;
     }
     oldtext = text; 
  }
}

int blog(int base, int m )
{
int i, exp;
	exp = base;
        m = m + m/2;
	for (i = 1; exp < m; i++) exp = exp * base;
	return(i);
}
 
void prep4(char *Pattern, int m)
{
int i, j, k;
unsigned hash;

for(i=0; i< MAXSYM; i++) char_map[i] = 0;
char_map['a'] = char_map['A'] = 4;
char_map['g'] = char_map['g'] = 1;
char_map['t'] = char_map['t'] = 2;
char_map['c'] = char_map['c'] = 3;
char_map['n'] = char_map['n'] = 5;

	BSize = blog(4, m);
	for (i = 1, Hashmask = 1 ; i<BSize*LOG_DNA; i++) Hashmask = (Hashmask << 1) + 1 ;
	MEMBER_D = (char *) malloc((Hashmask+1)  * sizeof(char));
#ifdef DEBUG
	printf("BSize = %d", BSize);
#endif 
	for (i=0; i <= Hashmask; i++) MEMBER_D[i] = 0;
	for (j=0; j < BSize; j++) {
            for(i=m-1; i >= j; i--) {
               hash = 0;
	       for(k=0; k <= j; k++) 
		  hash = (hash << LOG_DNA) +char_map[Pattern[i-k]]; 
#ifdef DEBUG
	       printf("< %d >, ", hash);
#endif
	       MEMBER_D[hash] = 1;
            }
        }
}

void sgrep(CHARTYPE *pat, int m, int fd, int D)
{ 
    CHARTYPE text[BLOCKSIZE+2*MAXLINE+MAXPATT]; /* input text stream */
    int offset = 2*MAXLINE;
    int buf_end, num_read, i, start, end, residue = 0;
    if(pat[0] == '^' || pat[0] == '$') pat[0] = '\n';
    if(pat[m-1] == '^' || pat[m-1] == '$') pat[m-1] = '\n';
    char_tr(pat, &m);   /* will change pat, and m if WHOLELINE is ON */
    text[offset-1] = '\n';  /* initial case */
    for(i=0; i < MAXLINE; i++) text[i] = 0;   /* security zone */
    start = offset;   
    if(WHOLELINE) start--;
    if(m >= MAXPATT) {
         fprintf(stderr, "%s: pattern too long\n", Progname);
         exit(2);
    }
    if(D == 0) {
	if(m > LONG_EXAC) m_preprocess(pat);
	else prep_bm(pat, m);
    }
    else if (DNA) prep4(pat, m);
	 else 	if(m >= LONG_APPX) am_preprocess(pat);
		else {
			prep(pat, m, D);
			initmask(pat, Mask, m, 0, &endposition); 
		}
    for(i=1; i<=m; i++) text[BLOCKSIZE+offset+i] = pat[m-1];
		/* to make sure the skip loop in bm() won't go out of bound */
    while( (num_read = read(fd, text+offset, BLOCKSIZE)) > 0) 
    {
       buf_end = end = offset + num_read -1 ;
       while(text[end]  != '\n' && end > offset) end--;
       residue = buf_end - end + 1 ;
       text[start-1] = '\n';
       if(D==0)  {
		if(m > LONG_EXAC) monkey(pat, m, text+start, text+end);
		else bm(pat, m, text+start, text+end);
       }
       else {
		if(DNA) monkey4( pat, m, text+start, text+end, D  );
		else {
		  if(m >= LONG_APPX) a_monkey(pat, m, text+start, text+end, D);
		  else       agrep(pat, m, text+start, text+end, D);
		}
       }
       if(FILENAMEONLY && num_of_matched) {
            printf("%s\n", CurrentFileName);
            return; }
       start = offset - residue ;
       if(start < MAXLINE) {
            start = MAXLINE; 
       }
       strncpy(text+start, text+end, residue);
       start++;
    } /* end of while(num_read = ... */
    return;
} /* end sgrep */

