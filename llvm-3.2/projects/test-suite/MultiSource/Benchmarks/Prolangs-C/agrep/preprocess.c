/* Copyright (c) 1991 Sun Wu and Udi Manber.  All Rights Reserved. */
/* substitute metachar with special symbol                               */
/* if regularr expression, then set flag REGEX                           */
/* if REGEX and MULTIPAT then report error message,                      */
/* -w only for single word pattern. If WORDBOUND & MULTIWORD error       */
/* process start of line, endof line symbol,                             */
/* process -w WORDBOUND option, append special symbol at begin&end of    */
/* process -d option before this routine                                 */
/* the delimiter pattern is in D_pattern (need to end with '; ')          */
/* if '-t' (suggestion: how about -B) the pattern is passed to sgrep     */
/* and doesn't go here                                                   */
/* in that case, -d is ignored? or not necessary                         */
/* upon return, Pattern contains the pattern to be processed by maskgen  */
/* D_pattern contains transformed D_pattern                              */
#include <stdlib.h>
#include <string.h>
#include "agrep.h"
  
extern int SIMPLEPATTERN, WHOLELINE, REGEX, RE_ERR, DELIMITER, TAIL, WORDBOUND;
extern int HEAD;
extern CHAR Progname[];
extern int D_length;
extern int table[WORD][WORD];

extern int init(char *s, int table[32][32]);
  
void preprocess(CHAR *D_pattern, CHAR *Pattern)   /* need two parameters  */
{
  CHAR temp[Maxline], *r_pat, *old_pat;  /* r_pat for r.e. */
  CHAR old_D_pat[MaxDelimit];
  int i, j=0, rp=0, m, t=0, num_pos, ANDON = 0;
  int d_end ;  
  int IN_RANGE=0;
  old_pat = Pattern; /* to remember the starting position */
  m = strlen(Pattern);
  for(i=0; i< m; i++) {
      if(Pattern[i] == '\\') i++;
      else if(Pattern[i] == '|' || Pattern[i] == '*' ) REGEX = ON;   
  }
  r_pat = (CHAR *) malloc(strlen(Pattern)+2*strlen(D_pattern));
  strcpy(temp, D_pattern);
  d_end = t = strlen(temp);  /* size of D_pattern, including '; ' */
  if (WHOLELINE) { temp[t++] = LANGLE; 
                   temp[t++] = NNLINE; 
                   temp[t++] = RANGLE;
                   temp[t] = '\0';
                   strcat(temp, Pattern);
                   m = strlen(temp);
                   temp[m++] = LANGLE; 
                   temp[m++] = '\n'; 
                   temp[m++] = RANGLE; 
                   temp[m] = '\0';  }
  else {
     if (WORDBOUND) { temp[t++] = LANGLE; 
                      temp[t++] = WORDB; 
                      temp[t++] = RANGLE;
                      temp[t] = '\0'; }
     strcat(temp, Pattern);
     m = strlen(temp);
     if (WORDBOUND) { temp[m++] = LANGLE; 
                      temp[m++] = WORDB; 
                      temp[m++] = RANGLE; }
     temp[m] = '\0';
  }
        /* now temp contains augmented pattern , m it's size */

  D_length = 0;
  for (i=0, j=0; i< d_end-2; i++) {
      switch(temp[i]) 
      {
         case '\\' : i++; 
                     Pattern[j++] = temp[i];
                     old_D_pat[D_length++] = temp[i];
                     break;
         case '<'  : Pattern[j++] = LANGLE;
                     break;
         case '>'  : Pattern[j++] = RANGLE;
                     break;
         case '^'  : Pattern[j++] = '\n';
                     old_D_pat[D_length++] = temp[i];
                     break;
         case '$'  : Pattern[j++] = '\n';
                     old_D_pat[D_length++] = temp[i];
                     break;
         default  :  Pattern[j++] = temp[i];
                     old_D_pat[D_length++] = temp[i];
                     break;
     }
  }
  if(D_length > MAXDELIM) {
     fprintf(stderr, "%s: delimiter pattern too long\n", Progname);
     exit(2);
  }
  Pattern[j++] = ANDPAT;
  old_D_pat[D_length] = '\0';
  strcpy(D_pattern, old_D_pat);
  D_length++;
/*
  Pattern[j++] = ' ';
*/
  Pattern[j] = '\0';
  rp = 0; 
  if(REGEX) {
      r_pat[rp++] = '.';    /* if REGEX: always append '.' in front */
      r_pat[rp++] = '(';
      Pattern[j++] = NOCARE;
      HEAD = ON;
  }
  for (i=d_end; i < m ; i++)
  {
       switch(temp[i]) 
       {
           case '\\': i++;  Pattern[j++] = temp[i]; 
                      r_pat[rp++] = 'o';   /* the symbol doesn't matter */
                      break;
           case '#':  if(REGEX) {
                         Pattern[j++] = NOCARE;
                         r_pat[rp++] = '.';
                         r_pat[rp++] = '*';
                         break; }
                      Pattern[j++] = WILDCD;
                      break; 
           case '(':  Pattern[j++] = LPARENT; 
                      r_pat[rp++] = '(';     
                      break;
           case ')':  Pattern[j++] = RPARENT; 
                      r_pat[rp++] = ')'; 
                      break;
           case '[':  Pattern[j++] = LRANGE;  
                      r_pat[rp++] = '[';
                      IN_RANGE = ON;
                      break;
           case ']':  Pattern[j++] = RRANGE;  
                      r_pat[rp++] = ']'; 
		      IN_RANGE = OFF;
                      break;
           case '<':  Pattern[j++] = LANGLE;  
                      break;
           case '>':  Pattern[j++] = RANGLE;  
                      break;
           case '^':  if (temp[i-1] == '[') Pattern[j++] = NOTSYM;
                      else Pattern[j++] = '\n';
                      r_pat[rp++] = '^';
                      break;
           case '$':  Pattern[j++] = '\n'; 
                      r_pat[rp++] = '$';
                      break;
           case '.':  Pattern[j++] = NOCARE;
                      r_pat[rp++] = '.';
                      break;
           case '*':  Pattern[j++] = STAR; 
                      r_pat[rp++] = '*';
                      break;
           case '|':  Pattern[j++] = ORSYM; 
                      r_pat[rp++] = '|';
                      break;
           case ',':  Pattern[j++] = ORPAT;  
                      RE_ERR = ON; 
                      break;
           case ';':  if(ANDON) RE_ERR = ON; 
                      Pattern[j++] = ANDPAT;
                      ANDON = ON;
                      break;
           case '-':  if(IN_RANGE) {
                          Pattern[j++] = HYPHEN; 
                          r_pat[rp++] = '-';
                      }
                      else { 
                          Pattern[j++] = temp[i];
                          r_pat[rp++] = temp[i];
                      }  
                      break;
           case NNLINE :
                      Pattern[j++] = temp[i];
                      r_pat[rp++] = 'N';
                      break;
           default:   Pattern[j++] = temp[i]; 
                      r_pat[rp++] = temp[i];
                      break;
      }
  }
  if(REGEX) {           /* append ').' at end of regular expression */
      r_pat[rp++] = ')';
      r_pat[rp++] = '.';
      Pattern[j++] = NOCARE;
      TAIL = ON;
  }
  Pattern[j] = '\0'; 
  m = j;
  r_pat[rp] = '\0'; 
  if(REGEX)
  {  
     if(DELIMITER || WORDBOUND)  {
          fprintf(stderr, "%s: -d or -w option is not supported for this pattern\n", Progname);
          exit(2);
     }
     if(RE_ERR) {
        fprintf(stderr, "%s: illegal regular expression\n", Progname);
        exit(2);
     }
     while(*Pattern != NOCARE && m-- > 0) Pattern++;  /* poit to . */
     num_pos = init(r_pat, table);
     if(num_pos <= 0) {
         fprintf(stderr, "%s: illegal regular expression\n", Progname);
         exit(2);
     }
     if(num_pos > 30) {
        fprintf(stderr, "%s: regular expression too long\n", Progname);
        exit(2);
     }
  strcpy(old_pat, Pattern); /* do real change to the Pattern to be returned */
  return;
  } /* if regex */

  return;
}
