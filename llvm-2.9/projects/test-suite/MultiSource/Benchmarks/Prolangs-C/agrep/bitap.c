/* Copyright (c) 1991 Sun Wu and Udi Manber.  All Rights Reserved. */
/* if the pattern is not simple fixed pattern, then after preprocessing */
/* and generating the masks, the program goes here. four cases:  1.     */ 
/* the pattern is simple regular expression and no error, then do the   */
/* matching here.  2. the pattern is simple regular expression and      */
/* unit cost errors are allowed: then go to asearch().                  */
/* 3. the pattern is simple regular expression, and the edit cost is    */
/* not uniform, then go to asearch1().                                  */
/* if the pattern is regular expression then go to re() if M < 14,      */
/* else go to re1()                                                     */
/* input parameters: old_D_pat: delimiter pattern.                      */
/* fd, input file descriptor, M: size of pattern, D: # of errors.       */
#include <unistd.h>
#include "agrep.h"

extern unsigned Init1, D_endpos, endposition, Init[], Mask[], Bit[];
extern int DELIMITER, FILENAMEONLY, D_length, I, AND, REGEX, JUMP, INVERSE; 
extern char D_pattern[];
extern int TRUNCATE, DD, S;
extern char Progname[], CurrentFileName[];
extern int num_of_matched;

extern void re(int Text, int M, int D);
extern void re1(int Text, int M, int D);
extern void asearch1(char old_D_pat[], int Text, register unsigned D);
extern void asearch(CHAR old_D_pat[], int text, register unsigned D);
extern int fill_buf(int fd, unsigned char *buf, int record_size);
extern void output(register CHAR *buffer, int i1, int i2, int j);

/* bitap dispatches job */

void bitap(char old_D_pat[], char *Pattern, int fd, int M, int D)
{
char c;  
register unsigned r1, r2, r3, CMask, i;
register unsigned end, endpos, r_Init1;
register unsigned D_Mask;
int  ResidueSize , FIRSTROUND, lasti, print_end, j, num_read;
int  k;
char buffer[Max_record+Max_record+BlockSize];
  D_length = strlen(old_D_pat);
  for(i=0; i<D_length; i++) if(old_D_pat[i] == '^' || old_D_pat[i] == '$')
                               old_D_pat[i] = '\n';
  if (REGEX) { 
      if (D > 4) {
          fprintf(stderr, "%s: the maximum number of erorrs allowed for full regular expression is 4\n", Progname);
          exit(2);
      }
      if (M <= SHORTREG) { re(fd, M, D);   /* SUN: need to find a even point */ 
                     return; }
      else { re1(fd, M, D); 
             return; }
  }   
  if (D > 0 && JUMP == ON) 
     { asearch1(old_D_pat, fd, D); return; }
  if (D > 0) 
     { asearch(old_D_pat, fd, D); return; }
  if(I == 0) Init1 = 037777777777;

  j=0;
  lasti = Max_record;
  buffer[Max_record-1] = '\n';
  r_Init1 = Init1;
  r1 = r2 = r3 = Init[0];
  endpos = D_endpos;
  
  buffer[Max_record-1] = '\n';
  D_Mask = D_endpos;
  for(i=1 ; i<D_length; i++) D_Mask = (D_Mask << 1) | D_Mask;
  D_Mask = ~D_Mask;
  FIRSTROUND = ON;

  while ((num_read = fill_buf(fd, buffer + Max_record, Max_record)) > 0)
  {
    i=Max_record; end = Max_record + num_read; 
    if(FIRSTROUND) {  i = Max_record - 1 ;

			if(DELIMITER) {
				for(k=0; k<D_length; k++) {
					if(old_D_pat[k] != buffer[Max_record+k]) 						break;
				}
				if(k>=D_length) j--;
			}

                      FIRSTROUND = OFF;  }
    if(num_read < BlockSize) {
                      strncpy(buffer+Max_record+num_read, old_D_pat, D_length);
                      end = end + D_length;
                      buffer[end] = '\0';
    }
    while (i < end)
    {
        c = buffer[i++];
        CMask = Mask[c];
              r1 = r_Init1 & r3;
              r2 = (( r3 >> 1 ) & CMask) | r1;
        if ( r2 & endpos ) {
           j++;
           if(((AND == 1) && ((r2 & endposition) == endposition)) ||                           ((AND == 0) && (r2 & endposition)) ^ INVERSE )
               { 
                 if(FILENAMEONLY) {
                    num_of_matched++;
                    printf("%s\n", CurrentFileName);
                    return; }
                 print_end = i - D_length - 1;
                 if(!(lasti >= Max_record+num_read - 1))
                    output(buffer, lasti, print_end, j); 
               }
           lasti = i - D_length; 
           TRUNCATE = OFF;
           r2 = r3 = r1 = Init[0];
           r1 = r_Init1 & r3;
           r2 = ((( r2 >> 1) & CMask) | r1 ) & D_Mask;
        }
        c = buffer[i++];
        CMask = Mask[c];
              r1 = r_Init1 & r2;
              r3 = (( r2 >> 1 ) & CMask) | r1; 
        if ( r3 & endpos ) {
           j++;
           if(((AND == 1) && ((r3 & endposition) == endposition)) ||                           ((AND == 0) && (r3 & endposition)) ^ INVERSE )
               { 
                 if(FILENAMEONLY) {
                    num_of_matched++;
                    printf("%s\n", CurrentFileName);
                    return; }
                 print_end = i - D_length - 1;
                 if(!(lasti >= Max_record+num_read - 1))
                    output(buffer, lasti, print_end, j);
               }
           lasti = i - D_length ;
           TRUNCATE = OFF;
           r2 = r3 = r1 = Init[0]; 
           r1 = r_Init1 & r2;
           r3 = ((( r2 >> 1) & CMask) | r1 ) & D_Mask;
       }   
    }
    ResidueSize = num_read + Max_record - lasti;
    if(ResidueSize > Max_record) {
            ResidueSize = Max_record;
            TRUNCATE = ON;   
    }
    strncpy(buffer+Max_record-ResidueSize, buffer+lasti, ResidueSize);
    lasti = Max_record - ResidueSize;
    if(lasti < 0) {
       lasti = 1;
    } 
  }
  return;
}

int fill_buf(int fd, unsigned char *buf, int record_size)
{
int num_read=1;
int total_read=0;
	while(total_read < record_size && num_read > 0) {
		num_read = read(fd, buf+total_read, 4096);
		total_read = total_read + num_read;
	}
	return(total_read);
}

