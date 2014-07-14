/* Copyright (c) 1991 Sun Wu and Udi Manber.  All Rights Reserved. */
#include "agrep.h"
 
extern unsigned D_endpos, endposition, Init1, wildmask;
extern Mask[], Bit[], Init[], NO_ERR_MASK;
extern int AND, SIMPLEPATTERN, REGEX, NOUPPER, D_length;
extern unsigned char Progname[];
	   
int maskgen(unsigned char *Pattern, int D)
{
struct term { int flag; unsigned char class1[WORD];
            } position[WORD+10];
unsigned char c;

int i, j, k, l, M, OR=0, EVEN = 0, base, No_error;


for(i=0; i<WORD; i++) position[i].class1[0] = '\0';
for(i=0; i<WORD; i++) position[i].flag = 0;
wildmask = NO_ERR_MASK = endposition = 0;
No_error = 0;
M = strlen(Pattern);
if(NOUPPER) {
              for(i=0; i<M; i++) if(isalpha(Pattern[i])) 
                     if (isupper(Pattern[i])) Pattern[i] = tolower(Pattern[i]);
            }
#ifdef DEBUG
	for(i=0; i<M; i++) printf(" %d", Pattern[i]);
	printf("\n");
#endif
for (i=0, j=1; i< M; i++)
{
  switch (Pattern[i])
  {
    case WILDCD : if(REGEX) {
                     position[j].class1[0] = '.';
                     position[j].class1[1] = '.';
                     position[j++].class1[2] = '\0'; 
                     break;
                  }
                  wildmask = wildmask | Bit[j-1]; break;
    case STAR   : break; 
    case ORSYM  : break; 
    case LPARENT: break;
    case RPARENT: break;
    case LANGLE : No_error = ON; EVEN++;
                  break;
    case RANGLE : No_error = OFF; EVEN--;
                  if(EVEN < 0) {
                     fprintf(stderr, "%s: illegal pattern, unmatched '<', '>'\n", Progname);
                     exit(2);
                  }
                  break;
    case LRANGE : if(No_error == ON) NO_ERR_MASK = NO_ERR_MASK | Bit[j]; 
                  i=i+1; 
                  if (Pattern[i] == NOTSYM) { position[j].flag = Compl; i++; }
                  k=0;
                  while (Pattern[i] != RRANGE && i < M)
                  { 
                    if(Pattern[i] == HYPHEN) 
                       { position[j].class1[k-1] = Pattern[i+1]; i=i+2; }
                    else { 
                     position[j].class1[k] = position[j].class1[k+1] = Pattern[i];
                     k = k+2; i++;
                    }
                  }
                  if(i == M) {
                     fprintf(stderr, "%s: illegal pattern, unmatched '[', ']'\n",Progname);
                     exit(2);
                  }
                  position[j].class1[k] = '\0';
                  j++; break;
    case RRANGE : fprintf(stderr, "%s: illegal pattern, unmatched '[', ']'\n", Progname); 
                  exit(2);
                  break;     
    case ORPAT  : if(REGEX == ON || AND == ON) {
                     fprintf(stderr, "illegal pattern \n");
                     exit(2);
                  }
                  OR = ON;
                  position[j].flag = 2; position[j].class1[0] = '\0';
                  endposition = endposition | Bit[j++]; break;
    case ANDPAT : position[j].flag = 2; position[j].class1[0] = '\0'; 
                  if(j > D_length) AND = ON;
                  if(OR || (REGEX == ON && j>D_length)) {
                     fprintf(stderr, "illegal pattern \n");
                     exit(2);
                  }
                  endposition = endposition | Bit[j++]; break;
/*
    case ' '    : if (Pattern[i-1] == ORPAT || Pattern[i-1] == ANDPAT) break;
                  if(No_error == ON) NO_ERR_MASK = NO_ERR_MASK | Bit[j];
                  position[j].flag = 0;
                  position[j].class1[0] = position[j].class1[1] = Pattern[i];
                  position[j++].class1[2] = '\0';  break;
*/
    case '\n'   : NO_ERR_MASK = NO_ERR_MASK | Bit[j];
                  position[j].class1[0] = position[j].class1[1] = '\n';
                  position[j++].class1[2] = '\0'; 
                  break;
    case WORDB  : NO_ERR_MASK = NO_ERR_MASK | Bit[j];
                  position[j].class1[0] = 1;
                  position[j].class1[1] = 47;
                  position[j].class1[2] = 58;
                  position[j].class1[3] = 64;
                  position[j].class1[4] = 91;
                  position[j].class1[5] = 96;
                  position[j].class1[6] = 123;
                  position[j].class1[7] = 127;
                  position[j++].class1[8] = '\0';
                  break;    
    case NNLINE : NO_ERR_MASK |= Bit[j];
                  position[j].class1[0] = position[j].class1[1] = '\n';
                  position[j].class1[2] = position[j].class1[3] = NNLINE;
                  position[j++].class1[4] = '\0';
                  break;
    default : if(No_error == ON) NO_ERR_MASK = NO_ERR_MASK | Bit[j];
                  position[j].flag = 0;
                  position[j].class1[0] = position[j].class1[1] = Pattern[i];
                  position[j++].class1[2] = '\0'; 
  }
  if(j > WORD) {
     fprintf(stderr, "%s: pattern too long\n", Progname);
     exit(2);
  }
}
  if (EVEN != 0) {
     fprintf(stderr, "%s: illegal pattern, unmatched '<', '>'\n", Progname);
     exit(2);
  }
M = j - 1;
base = WORD - M;
wildmask = (wildmask >> base);
endposition = (endposition >> base);
NO_ERR_MASK = (NO_ERR_MASK >> 1) & (~Bit[1]);
NO_ERR_MASK = ~NO_ERR_MASK >> (base-1);
  for (i=1; i<= WORD - M ; i++) Init[0] = Init[0] | Bit[i];
  Init[0] = Init[0] | endposition;
             /* not necessary for INit[i], i>0, */
             /* but at every begining of the matching process append one
                no-match character to initialize the error vectors */
  endposition = ( endposition << 1 ) + 1;
  Init1 = (Init[0] | wildmask | endposition) ;
  D_endpos = ( endposition >> ( M - D_length ) ) << ( M - D_length);
  endposition = endposition ^ D_endpos;
#ifdef DEBUG
	printf("endposition: %o\n", endposition);
	printf("no_err_mask: %o\n", NO_ERR_MASK);
#endif
  for(c=0, i=0; i < MAXSYM; c++, i++)
  {
     for (k=1, l=0; k<=M ; k++, l=0)  {
         while (position[k].class1[l] != '\0') {
               if (position[k].class1[l] == NOCARE && (c != '\n' || REGEX) ) 
                  {  Mask[c] = Mask[c] | Bit[base + k]; break; }
               if (c >= position[k].class1[l] && c <= position[k].class1[l+1])
                  {  Mask[c] = Mask[c] | Bit[base + k]; break; }
               l = l + 2;  }
         if (position[k].flag == Compl) Mask[c] = Mask[c] ^ Bit[base+k];
     }
  }
  if(NOUPPER) for(c='A'; c<='Z'; c=c+1) if (isupper(c)) 
                  Mask[c] = Mask[tolower(c)]; 
  return(M);
}


